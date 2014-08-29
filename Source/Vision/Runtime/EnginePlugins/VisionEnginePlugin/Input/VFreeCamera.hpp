/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VFreeCamera.hpp

#ifndef VFREECAMERA_HPP_INCLUDED
#define VFREECAMERA_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>
class VVirtualThumbStick;

/// \brief
///   Entity class that attaches the main camera and implements movement via mouse and keyboard
///   input. The camera can be controlled via the cursor keys (move) and mouse (rotate). Holding
///   down a mouse button while moving increases the speed.
class VFreeCamera : public VisBaseEntity_cl, public IVisCallbackHandler_cl
{
public:
  V_DECLARE_SERIAL_DLLEXP(VFreeCamera, EFFECTS_IMPEXP)
  V_DECLARE_VARTABLE(VFreeCamera, EFFECTS_IMPEXP)

  enum Control 
  {
    CONTROL_MOVE_FORWARD = 0,
    CONTROL_MOVE_BACKWARD,
    CONTROL_MOVE_LEFT,
    CONTROL_MOVE_RIGHT,
    CONTROL_MOVE_UP,
    CONTROL_MOVE_DOWN,

    CONTROL_ANY_ACTION,

    CONTROL_SPEED_FAST,
    CONTROL_SPEED_FASTER,

    CONTROL_HORIZONTAL_LOOK,
    CONTROL_VERTICAL_LOOK,

    // for touch screen devices
    CONTROL_TAP_X,
    CONTROL_TAP_Y,

    CONTROL_LAST_ELEMENT = CONTROL_TAP_Y
  };

  /// \brief
  ///   Static factory method to create a free camera entity which optionally uses the position and
  ///   orientation from a given entity, e.g. a camera position shape.
  /// 
  /// \param szCameraRefObjectKey
  ///   Entity object key of the entity that defines the initial camera position and rotation.
  ///
  /// \param fMoveSpeed
  ///   Camera move speed. A value less than 0 uses default value.
  ///
  /// \return
  ///   Pointer to the created camera entity
  /// 
  /// \example
  ///   \code
  ///     VFreeCamera::Create("CameraPosition", 10.0f);
  ///   \endcode
  EFFECTS_IMPEXP static VFreeCamera* Create(const char* szCameraRefObjectKey = NULL, float fMoveSpeed = -1.0f);

  EFFECTS_IMPEXP VFreeCamera();
  EFFECTS_IMPEXP virtual ~VFreeCamera();
  
  EFFECTS_IMPEXP virtual void InitFunction() HKV_OVERRIDE;
  EFFECTS_IMPEXP virtual void DeInitFunction() HKV_OVERRIDE;
  EFFECTS_IMPEXP virtual void ThinkFunction() HKV_OVERRIDE
  { /* Keep this function in order to have the ThinkFunctionStatus set to true. (See base implementation) */ }

  EFFECTS_IMPEXP void SetWASDControlsEnabled(bool bEnabled);
  inline bool AreWASDControlsEnabled() const { return m_bWASDEnabled; }

  inline float GetSensitivity() const { return m_fSensitivity; }
  inline void SetSensititvity(const float fSensitivity) { m_fSensitivity = hkvMath::Max(0.0f, fSensitivity); }

  inline float GetMoveSpeed() const { return m_fMoveSpeed; }
  EFFECTS_IMPEXP void SetMoveSpeed(const float fMoveSpeed);

  inline VInputMap* GetInputMap() { return m_pInputMap; }

  EFFECTS_IMPEXP virtual void Serialize(VArchive& ar) HKV_OVERRIDE;
  EFFECTS_IMPEXP virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData) HKV_OVERRIDE;

#if defined(SUPPORTS_MULTITOUCH)
  /// \brief
  ///   Forces the virtual thumb stick and the look touch area to be created.
  ///
  /// This function creates a virtual thumb stick if it wasn't created by default.
  EFFECTS_IMPEXP void CreateVirtualThumbStick();

  /// \brief
  ///   Returns the virtual thumb stick instance (if created).
  inline VVirtualThumbStick* GetVirtualThumbStick()
  {
    return m_pVirtualThumbStick;
  }

#endif

protected:
  /// \brief
  ///   Base initialization: code that's shared between InitFunction and Serialize.
  EFFECTS_IMPEXP void BaseInit();

  /// \brief
  ///   Override this function to set custom move axes.
  EFFECTS_IMPEXP virtual void GetCurrentMoveAxes(hkvVec3& vForward, hkvVec3& vRight, hkvVec3& vUp) const;

  /// \brief
  ///   Processes the input and sets the motion delta accordingly.
  EFFECTS_IMPEXP void ProcessInput(float fTimeDiff);

  EFFECTS_IMPEXP virtual void OnThinkFunctionStatusChanged() HKV_OVERRIDE;

  EFFECTS_IMPEXP virtual void OnVariableValueChanged(VisVariable_cl* pVar, const char* value) HKV_OVERRIDE;

  EFFECTS_IMPEXP virtual void ModSysAddChild(VisModuleSystemChild_cl *child) HKV_OVERRIDE;
  EFFECTS_IMPEXP virtual void ModSysRemoveChild(VisModuleSystemChild_cl *child, BOOL deleteChild) HKV_OVERRIDE;

  /// \brief
  ///   This function gets called whenever the active state of the free camera changes.
  virtual void OnActiveStateChanged(bool bActive) {}

private:
  void UpdateActiveState();
  void ClearWASDAlternativeIndices();

#if defined(SUPPORTS_MULTITOUCH)
  void UpdateVirtualThumbstickVisibility();
#endif

  // Private Variables
  float m_fSensitivity;  ///< Look sensitivity
  float m_fMoveSpeed;    ///< Move speed

  bool m_bWASDEnabled;
  int m_iWASDAlternativeIndices[6];

  bool m_bActive;

  VInputMap* m_pInputMap;

#if defined(SUPPORTS_MULTITOUCH)
  VVirtualThumbStick* m_pVirtualThumbStick;
  VTouchAreaPtr m_spLookTouchArea;
#endif
};

#if !defined(_VISION_DOC)
// Serialization fall back to old name of the free camera.
class VisMouseCamera_cl : public VFreeCamera
{
public:
  EFFECTS_IMPEXP VisMouseCamera_cl() : VFreeCamera() {}

  V_DECLARE_SERIAL_DLLEXP(VisMouseCamera_cl, EFFECTS_IMPEXP)
  V_DECLARE_VARTABLE(VisMouseCamera_cl, EFFECTS_IMPEXP)

  EFFECTS_IMPEXP virtual void Serialize(VArchive &ar) HKV_OVERRIDE
  {
    VFreeCamera::Serialize(ar);
  }
};
#endif

#endif

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
