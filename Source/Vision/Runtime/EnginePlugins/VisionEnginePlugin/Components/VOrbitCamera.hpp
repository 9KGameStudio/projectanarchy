/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VOrbitCamera.hpp

#ifndef VORBITCAMERA_HPP_INCLUDED
#define VORBITCAMERA_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>

class VZoomDetector;

/// 
/// \brief
///   linked to the parent entity.
/// 
/// This component can be attached to an entity to use it as the look-at position. 
/// It has three different modes: free orbit, follower and fixed. 
/// In the free orbit mode you can rotate the camera freely around the parent entity using
/// the mouse. 
/// The follower mode additionally aligns the camera orientation to the parent entity 
/// and follows its rotation.  
/// The fixed mode completely disables processing mouse input for orientation changes.
///
class VOrbitCamera : public IVObjectComponent, public IVisCallbackHandler_cl
{
public: 
  ///
  /// @name Constructor
  /// @{
  ///

  ///
  /// \brief
  ///   Component Constructor
  /// 
  /// \param iComponentFlags
  ///   See IVObjectComponent.
  /// 
  /// \see
  ///   IVObjectComponent
  /// 
  EFFECTS_IMPEXP VOrbitCamera(int iComponentFlags = VIS_OBJECTCOMPONENTFLAG_NONE);

  /// 
  /// \brief
  ///   Component Destructor
  /// 
  EFFECTS_IMPEXP virtual ~VOrbitCamera();

  ///
  /// @}
  ///

  ///
  /// @name Orbit Camera Properties
  /// @{
  ///

  ///
  /// \brief
  ///   Enables or disables the orbit camera.
  /// 
	/// \param bStatus
	///   State value specifying the enabled state of the orbit camera.
	/// 
  /// \see
  ///   IsEnabled
	/// 
  EFFECTS_IMPEXP void SetEnabled(bool bStatus = true);

  ///
  /// \brief
  ///   Indicates whether the orbit camera is currently enabled.
  /// 
	/// \returns
	///   TRUE if orbit camera is enabled, FALSE otherwise.
	/// 
	/// \see
  ///   SetEnabled
	/// 
	inline bool IsEnabled() const 
  { 
    return (Enabled == TRUE); 
  }

  ///
  /// \brief
  ///   Resets the position and orientation that is used for smoothing the camera movement.
  /// 
  EFFECTS_IMPEXP void Teleport();

  ///
  /// @}
  ///

  ///
  /// @name IVObjectComponent Overrides
  /// @{
  ///

  /// \brief
  ///   Overridden function to respond to owner changes.
  EFFECTS_IMPEXP virtual void SetOwner(VisTypedEngineObject_cl* pOwner) HKV_OVERRIDE;

  /// \brief
  ///   Overridden function. Blob shadows can be attached to VisObject3D_cl instances.
  EFFECTS_IMPEXP virtual BOOL CanAttachToObject(VisTypedEngineObject_cl* pObject, VString& sErrorMsgOut) HKV_OVERRIDE;

  /// \brief
  ///   Overridden function to respond to variable changes.
  EFFECTS_IMPEXP virtual void OnVariableValueChanged(VisVariable_cl* pVar, const char* value) HKV_OVERRIDE;

  ///
  /// @}
  ///

  ///
  /// @name Serialization
  /// @{
  ///

  /// \brief
  ///   RTTI macro
  V_DECLARE_SERIAL_DLLEXP(VOrbitCamera, EFFECTS_IMPEXP)

  /// \brief
  ///   RTTI macro to add a variable table
  V_DECLARE_VARTABLE(VOrbitCamera, EFFECTS_IMPEXP)

  /// \brief
  ///   Serialization function
  ///
  /// \param ar
  ///   binary archive
  EFFECTS_IMPEXP virtual void Serialize(VArchive &ar) HKV_OVERRIDE;
  
  /// \brief
  ///   Virtual overridable that gets called when a loading archive closes
  /// 
  EFFECTS_IMPEXP virtual void OnDeserializationCallback(const VSerializationContext &context) HKV_OVERRIDE;

  /// \brief
  ///   Virtual overridable that gets called to determine if the object needs the OnDeserializationCallback
  /// 
  EFFECTS_IMPEXP virtual VBool WantsDeserializationCallback(const VSerializationContext &context) HKV_OVERRIDE
  {
    return TRUE;
  }

  ///
  /// @}
  ///

  ///
  /// @name IVisCallbackHandler_cl Overrides
  /// @{
  ///

  /// 
  /// \brief
  ///   Overridden function that gets called by all callbacks that this handler is registered to.
  /// 
  /// \param pData
  ///   See IVisCallbackHandler_cl.
  ///
  /// \see
  ///   IVisCallbackHandler_cl
  /// 
  EFFECTS_IMPEXP virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData) HKV_OVERRIDE;

  /// \brief
  ///   This function supplies the sorting key for the calling order. Handlers with small key value
  ///   are called first.
  /// 
  /// \param pCallback
  ///   The current callback instance that performs sorting.
  /// 
  /// \return
  ///   int iSortinKey: Arbitrary sorting key value. The value can be positive or negative.
  ///   We want the camera to update after other callbacks as they can change the owner entity's position.
  virtual int64 GetCallbackSortingKey(VCallback *pCallback) HKV_OVERRIDE
  {
	  return 42000;
  }

  ///
  /// @}
  ///

protected:
  // May be overridden to modify the component's input processing.
  EFFECTS_IMPEXP virtual void ProcessInput();

  // Camera Update Step (called each frame in OnHandleCallback) 
  EFFECTS_IMPEXP void UpdateCamera(float fYawStep, float fPitchStep, float fDistanceStep, float fWeight);

private:  
  // Initial Camera Setup (called in SetOwner)
  void SetupCamera();

  // Attaches/Detaches the camera
  void UpdateAttachment();

  const hkvVec3 ComputeBaseOrientation() const;
  const hkvVec3 ComputeCenterPosition() const;
  float ComputeCollisionDistance(const hkvVec3& vLookAtPos, float fCameraDistance) const;

public:
  // Exposed to vForge:
  BOOL Enabled;                       ///< State of the camera
  BOOL Collides;                      ///< Whether camera collides with world geometry
  BOOL Follow;                        ///< Whether the camera rotates with the parent object
  BOOL Zoom;                          ///< Whether camara zoom is enabled
  BOOL DepthOfField;                  ///< Whether depth of field focus is adjusted automatically
  BOOL FollowFixed;                   ///< Determines if the camera orientation can be controlled by the user.

  float InitialYaw;                   ///< Yaw angle - horizontal rotation
  float InitialPitch;                 ///< Pitch angle - vertical rotation
  float MinimumPitch;                 ///< Minimum pitch angle
  float MaximumPitch;                 ///< Maximum pitch angle
  float CameraDistance;               ///< Initial distance from the camera to the character
  float MinimalDistance;              ///< Minimal camera distance
  float MaximalDistance;              ///< Maximal camera distance
  float RelativeLookAtHeight;         ///< The relative lookat height of the character the camera looks at
  
  float CameraSensitivity;            ///< Camera movement sensitivity
  float MoveSmoothness;               ///< Smoothness of the camera movement (ease in/out only for follow camera)
  
private:
  VSmartPtr<VisBaseEntity_cl> m_spCameraProxy; ///< Camera entity placeholder

  hkvVec3 m_vOldLookAt;               ///< Position of owner in last update (needed for ease in/out)
  hkvVec3 m_vOldBaseOri;              ///< Orientation of owner in last update.
  hkvVec3 m_vOffsetOri;               ///< Accumulated orientation change made by the user.
  float m_fCurrentDistance;

#if defined(SUPPORTS_MULTITOUCH)
  VZoomDetector* m_pZoomDetector;
#endif

public:
  VInputMap *m_pInputMap;             ///< Input map for the camera control;
};

#if !defined(_VISION_DOC)
// Serialization fall back to old name VPlayerCamera
class VPlayerCamera : public VOrbitCamera
{
public:
  EFFECTS_IMPEXP VPlayerCamera(int iComponentFlags = VIS_OBJECTCOMPONENTFLAG_NONE) 
    : VOrbitCamera(iComponentFlags)
  {}

  V_DECLARE_SERIAL_DLLEXP(VPlayerCamera, EFFECTS_IMPEXP)
  V_DECLARE_VARTABLE(VPlayerCamera, EFFECTS_IMPEXP)

  EFFECTS_IMPEXP virtual void Serialize(VArchive &ar) HKV_OVERRIDE
  {
    VOrbitCamera::Serialize(ar);
  }
};
#endif

#endif

/*
 * Havok SDK - Base file, BUILD(#20140618)
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
