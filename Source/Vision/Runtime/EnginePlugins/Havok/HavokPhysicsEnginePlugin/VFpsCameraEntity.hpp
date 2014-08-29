/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VFpsCameraEntity.hpp

#ifndef VFPSCAMERAENTITY_HPP_INCLUDED
#define VFPSCAMERAENTITY_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Input/VFreeCamera.hpp>

class vHavokCharacterController;
class VInputMap;

///\brief
/// Entity class that exposes first-person shooter camera controls to a camera.
///   
class VFpsCameraEntity : public VFreeCamera
{
public:
  enum FpsControl
  {
    CONTROL_JUMP = CONTROL_LAST_ELEMENT + 1
  };

  V_DECLARE_SERIAL_DLLEXP(VFpsCameraEntity, VHAVOK_IMPEXP)
  V_DECLARE_VARTABLE(VFpsCameraEntity, VHAVOK_IMPEXP)

  VFpsCameraEntity();

  VHAVOK_IMPEXP virtual void InitFunction() HKV_OVERRIDE;
  VHAVOK_IMPEXP virtual void DeInitFunction() HKV_OVERRIDE;

  VHAVOK_IMPEXP virtual void Serialize(VArchive &ar) HKV_OVERRIDE;

  VHAVOK_IMPEXP virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData) HKV_OVERRIDE;

  VHAVOK_IMPEXP virtual void OnDeserializationCallback(const VSerializationContext &context) HKV_OVERRIDE;
  VHAVOK_IMPEXP virtual VBool WantsDeserializationCallback(const VSerializationContext &context) HKV_OVERRIDE
  {
    return Vision::Editor.IsInEditor() ? FALSE : TRUE;
  }

  VHAVOK_IMPEXP void SetHeight(float fHeight);

  inline float GetStandHeight() const
  {
    return m_fHeight;
  }

  VHAVOK_IMPEXP void SetEyeHeight(float fHeight);

  inline float GetEyeHeight() const
  {
    return m_fEyeHeight;
  }

  VHAVOK_IMPEXP void SetRadius(float fRadius);

  inline float GetRadius() const
  {
    return m_fRadius;
  }

  VHAVOK_IMPEXP virtual void OnVariableValueChanged(VisVariable_cl* pVar, const char* value) HKV_OVERRIDE;

protected:
  // VFreeCamera overrides
  VHAVOK_IMPEXP virtual void GetCurrentMoveAxes(hkvVec3& vForward, hkvVec3& vRight, hkvVec3& vUp) const HKV_OVERRIDE;
  VHAVOK_IMPEXP virtual void OnActiveStateChanged(bool bActive) HKV_OVERRIDE;

private:
  // private functions
  void CommonInit();

  void SetupPhysics();            // Sets up physics and collision behavior.
  bool TouchesGround();           // Returns true if player touches the ground.

  void UpdateCharacterController();

  vHavokCharacterController* m_pCharacterController;

  float m_fHeight;
  float m_fEyeHeight;
  float m_fRadius;
};

#endif // VFPSCAMERAENTITY_HPP_INCLUDED

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
