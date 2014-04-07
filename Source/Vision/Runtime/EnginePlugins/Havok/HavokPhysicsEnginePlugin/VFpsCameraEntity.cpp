/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VFpsCameraEntity.cpp

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/VFpsCameraEntity.hpp>
#include <Vision/Runtime/Engine/SceneElements/VisApiObject3D.hpp>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokCharacterController.hpp>

// bounding box related defines
#define FPSPLAYER_BBOX_WIDTH            36.0f
#define FPSPLAYER_BBOX_BREADTH          20.0f
#define FPSPLAYER_BBOX_HEIGHT_STAND     90.0f
#define FPSPLAYER_BBOX_TOP_OFFSET       5.0f

VFpsCameraEntity::VFpsCameraEntity()
  : m_pCharacterController(NULL)
  , m_bbStand()
{
}

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

void VFpsCameraEntity::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if (pData->m_pSender == &Vision::Callbacks.OnUpdateSceneFinished)
  {
    VASSERT(GetThinkFunctionStatus() == TRUE);

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

void VFpsCameraEntity::OnThinkFunctionStatusChanged()
{
  VFreeCamera::OnThinkFunctionStatusChanged();

  const bool bEnabled = (GetThinkFunctionStatus() == TRUE);
  if (bEnabled)
  {
    Vision::Callbacks.OnUpdateSceneFinished += this;
  }
  else
  {
    Vision::Callbacks.OnUpdateSceneFinished -= this;
  }
}

V_IMPLEMENT_SERIAL(VFpsCameraEntity, VFreeCamera, 0, &g_vHavokModule);

void VFpsCameraEntity::Serialize(VArchive &ar)
{
  VFreeCamera::Serialize(ar);

  if (ar.IsLoading())
  {
  } 
  else
  {
  }
}

VHAVOK_IMPEXP void VFpsCameraEntity::OnDeserializationCallback(const VSerializationContext &context)
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

  // HACK!!! Retrieve our initial position via a specific entity
  VisBaseEntity_cl* pEntity = Vision::Game.SearchEntity("FpsCamInitPos");
  if (pEntity != NULL)
  {
    SetPosition(pEntity->GetPosition());
    SetOrientation(pEntity->GetOrientation());
  }

  // Tell VFreeCamera to use motion delta by specifying the physics object.
  SetPhysicsObject(m_pCharacterController);
}

void VFpsCameraEntity::SetupPhysics()
{
  // set up bounding boxes
  m_bbStand.m_vMin.x = -(FPSPLAYER_BBOX_BREADTH / 2);
  m_bbStand.m_vMax.x =  (FPSPLAYER_BBOX_BREADTH / 2);
  m_bbStand.m_vMin.y = -(FPSPLAYER_BBOX_WIDTH / 2);
  m_bbStand.m_vMax.y =  (FPSPLAYER_BBOX_WIDTH / 2);
  m_bbStand.m_vMin.z = -(FPSPLAYER_BBOX_HEIGHT_STAND - FPSPLAYER_BBOX_TOP_OFFSET);
  m_bbStand.m_vMax.z =  (FPSPLAYER_BBOX_TOP_OFFSET);

  m_pCharacterController = new vHavokCharacterController();
  m_pCharacterController->Initialize();
  m_pCharacterController->Capsule_Radius = FPSPLAYER_BBOX_WIDTH;
  m_pCharacterController->Character_Top    = hkvVec3(0,0,  m_bbStand.getSizeZ()*0.1f);
  m_pCharacterController->Character_Bottom = hkvVec3(0,0, -m_bbStand.getSizeZ()*0.9f);
  m_pCharacterController->Max_Slope = 75.0f;
  m_pCharacterController->SetPosition(GetPosition());

  AddComponent(m_pCharacterController);
}

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

  const float fBBoxHeight = FPSPLAYER_BBOX_HEIGHT_STAND;
  CheckGroundRaycastFirstResult result(this);
  result.Reset();

  result.vRayStart = m_pCharacterController->GetPosition();
  result.vRayEnd = result.vRayStart - hkvVec3(0.0f, 0.0f, fBBoxHeight*0.70f);

  Vision::GetApplication()->GetPhysicsModule()->PerformRaycast(&result);
  return result.IsHit();
}

START_VAR_TABLE(VFpsCameraEntity, VFreeCamera, "VFpsCameraEntity", 0, "")  
END_VAR_TABLE

/*
 * Havok SDK - Base file, BUILD(#20140327)
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
