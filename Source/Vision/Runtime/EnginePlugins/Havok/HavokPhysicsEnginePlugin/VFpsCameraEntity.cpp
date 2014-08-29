/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VFpsCameraEntity.cpp

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/VFpsCameraEntity.hpp>
#include <Vision/Runtime/Engine/System/VisApiSerialization.hpp>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokCharacterController.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>

//-----------------------------------------------------------------------------------

VFpsCameraEntity::VFpsCameraEntity()
  : m_pCharacterController(NULL)
  , m_fHeight(90.0f * Vision::World.GetGlobalUnitScaling())
  , m_fEyeHeight(85.0f * Vision::World.GetGlobalUnitScaling())
  , m_fRadius(36.0f * Vision::World.GetGlobalUnitScaling())
{
}

//-----------------------------------------------------------------------------------

void VFpsCameraEntity::InitFunction()
{
  VFreeCamera::InitFunction();

  // The character controller component only needs to be set up once
  // it will be serialized too when exporting.
  SetupPhysics(); 

  CommonInit();
}

void VFpsCameraEntity::DeInitFunction()
{
  VFreeCamera::DeInitFunction();
}

//-----------------------------------------------------------------------------------

void VFpsCameraEntity::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if (pData->m_pSender == &Vision::Callbacks.OnUpdateSceneFinished)
  {
    if (GetInputMap()->GetTrigger(CONTROL_JUMP) && m_pCharacterController != NULL)
      m_pCharacterController->SetWantJump(true);

    const float fTimeDiff = Vision::GetTimer()->GetTimeDifference();
    ProcessInput(fTimeDiff);
  }
  else if (pData->m_pSender == &Vision::Callbacks.OnFrameUpdatePreRender)
  {
    // Disable input processing per frame.
    return;
  }

  VFreeCamera::OnHandleCallback(pData);
}

void VFpsCameraEntity::GetCurrentMoveAxes(hkvVec3& vForward, hkvVec3& vRight, hkvVec3& vUp) const
{
  VFreeCamera::GetCurrentMoveAxes(vForward, vRight, vUp);

  // Disable vertical movement.
  vUp = hkvVec3::ZeroVector();

  // Constrain horizontal movement to the XY-plane.
  vForward.z = vRight.z = 0.0f;
  vForward.normalize();
  vRight.normalize();
}

void VFpsCameraEntity::OnActiveStateChanged(bool bActive)
{
  if (bActive)
  {
    Vision::Callbacks.OnUpdateSceneFinished += this;
  }
  else
  {
    Vision::Callbacks.OnUpdateSceneFinished -= this;
  }
}

void VFpsCameraEntity::OnVariableValueChanged(VisVariable_cl* pVar, const char* value)
{
  UpdateCharacterController();
}

//-----------------------------------------------------------------------------------

void VFpsCameraEntity::SetHeight(float fHeight)
{
  m_fHeight = fHeight;
  UpdateCharacterController();
}

void VFpsCameraEntity::SetEyeHeight(float fHeight)
{
  m_fEyeHeight = fHeight;
  UpdateCharacterController();
}

void VFpsCameraEntity::SetRadius(float fRadius)
{
  m_fRadius = fRadius;
  UpdateCharacterController();
}

//-----------------------------------------------------------------------------------

void VFpsCameraEntity::OnDeserializationCallback(const VSerializationContext &context)
{
  VFreeCamera::OnDeserializationCallback(context);

  // Retrieve serialized character controller component.
  m_pCharacterController = Components().GetComponentOfType<vHavokCharacterController>();

  CommonInit();
}

void VFpsCameraEntity::CommonInit()
{
  // Disable speed modifiers.
  GetInputMap()->UnmapInput(CONTROL_SPEED_FAST);
  GetInputMap()->UnmapInput(CONTROL_SPEED_FASTER);

#if defined(_VISION_ANDROID)
  // Add jump control.
  GetInputMap()->MapTrigger(CONTROL_JUMP, VInputManager::GetPad(0), CT_PAD_CROSS,  VInputOptions::Once(ONCE_ON_PRESS));

#elif defined(_VISION_WIN32)
  GetInputMap()->MapTrigger(CONTROL_JUMP, V_KEYBOARD, CT_KB_SPACE);

#endif

  // Tell VFreeCamera to use motion delta by specifying the physics object.
  SetPhysicsObject(m_pCharacterController);
}

void VFpsCameraEntity::SetupPhysics()
{
  m_pCharacterController = new vHavokCharacterController();
  m_pCharacterController->Initialize();
  m_pCharacterController->Max_Slope = 75.0f;
  m_pCharacterController->SetPosition(GetPosition());

  UpdateCharacterController();

  AddComponent(m_pCharacterController);
}

void VFpsCameraEntity::UpdateCharacterController()
{
  if (m_pCharacterController == NULL)
    return;

  const float fActualStandHeight = hkvMath::Max(m_fHeight, m_fEyeHeight);
  const float fActualRadius = hkvMath::Min(m_fRadius, fActualStandHeight * 0.5f);

  m_pCharacterController->Capsule_Radius = fActualRadius;
  m_pCharacterController->Character_Top = hkvVec3(0.0f, 0.0f, fActualStandHeight - m_fEyeHeight);
  m_pCharacterController->Character_Bottom = hkvVec3(0.0f, 0.0f, -m_fEyeHeight);
  m_pCharacterController->Step_Height = fActualStandHeight / 3.0f;
  m_pCharacterController->Jump_Height = (fActualStandHeight / 3.0f) * vHavokConversionUtils::GetVision2HavokScale();

  // Re-create the Havok Physics controller if it was already initialized.
  if (m_pCharacterController->GetCharacterProxy() != NULL)
  {
    m_pCharacterController->DeleteHavokController();
    m_pCharacterController->CreateHavokController();
  }
}

//-----------------------------------------------------------------------------------

bool VFpsCameraEntity::TouchesGround()
{
  class CheckGroundRaycastFirstResult : public VisPhysicsRaycastBase_cl
  {
  public:
    CheckGroundRaycastFirstResult(VFpsCameraEntity* PlayerEntity) 
      : m_pPlayerEntity(PlayerEntity)
      , m_bHit(false)
    {
    }

    virtual void Reset() HKV_OVERRIDE
    {
      VisPhysicsRaycastBase_cl::Reset();
      m_bHit = false;
    }

    virtual bool onHit(VisPhysicsHit_t &hit) HKV_OVERRIDE
    {
      if(!m_bHit && hit.pHitObject != m_pPlayerEntity)
        m_bHit  = true;
      return !m_bHit;
    }

    virtual bool allHits() HKV_OVERRIDE
    {
      return false;
    }

    inline bool IsHit() const { return m_bHit; }

  private:
    VFpsCameraEntity* const m_pPlayerEntity;
    bool m_bHit;
  };

  CheckGroundRaycastFirstResult result(this);
  result.Reset();

  result.vRayStart = m_pCharacterController->GetPosition();
  result.vRayEnd = result.vRayStart - hkvVec3(0.0f, 0.0f, m_fEyeHeight * 1.05f);

  Vision::GetApplication()->GetPhysicsModule()->PerformRaycast(&result);
  return result.IsHit();
}

//-----------------------------------------------------------------------------------
// Serialization

#define VFPSCAMERA_VERSION_0        0 // Initial version
#define VFPSCAMERA_VERSION_CURRENT  VFPSCAMERA_VERSION_0

V_IMPLEMENT_SERIAL(VFpsCameraEntity, VFreeCamera, 0, &g_vHavokModule);

void VFpsCameraEntity::Serialize(VArchive &ar)
{
  VFreeCamera::Serialize(ar);

  if (ar.IsLoading())
  {
    // No serialization prior to archive version 34.
    if (ar.GetLoadingVersion() < VISION_ARCHIVE_VERSION_034)
      return;

    int iLocalVersion;
    ar >> iLocalVersion;
    VASSERT_MSG(iLocalVersion <= VFPSCAMERA_VERSION_CURRENT && iLocalVersion >= VFPSCAMERA_VERSION_0, "Invalid local version. Please re-export");

    ar >> m_fHeight;
    ar >> m_fEyeHeight;
    ar >> m_fRadius;
  } 
  else
  {
    const int iLocalVersion = VFPSCAMERA_VERSION_CURRENT;
    ar << iLocalVersion;

    ar << m_fHeight;
    ar << m_fEyeHeight;
    ar << m_fRadius;
  }
}

START_VAR_TABLE(VFpsCameraEntity, VFreeCamera, "VFpsCameraEntity", 0, "")
  DEFINE_VAR_FLOAT_AND_NAME(VFpsCameraEntity, m_fHeight, "CharacterHeight", "The overall height of the character controller.", "90.0", DISPLAY_HINT_GLOBALUNITSCALED, "Clamp(0.0, 1e20)");
  DEFINE_VAR_FLOAT_AND_NAME(VFpsCameraEntity, m_fEyeHeight, "CharacterEyeHeight", "The height of the camera.", "85.0", DISPLAY_HINT_GLOBALUNITSCALED, "Clamp(0.0, 1e20)");
  DEFINE_VAR_FLOAT_AND_NAME(VFpsCameraEntity, m_fRadius, "CharacterRadius", "The radius of the character controller.", "36.0", DISPLAY_HINT_GLOBALUNITSCALED, "Clamp(0.0, 1e20)");
END_VAR_TABLE

/*
 * Havok SDK - Base file, BUILD(#20140726)
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2014
 * Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
 * Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
 * rights, and intellectual property rights in the Havok software remain in
 * Havok and/or its suppliers.
 * 
 * Use of this software for evaluation purposes is subject to and indicates
 * acceptance of the End User licence Agreement for this product. A copy of
 * the license is included with this software and is also available from salesteam@havok.com.
 * 
 */
