/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// **********************************************************************************
// Declares rigid body's follow path component to be applied to any entity containing a
//	rigid body.
// Copyright (C) Havok, Inc. All rights reserved.
// **********************************************************************************
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef VRIGIDBODYFOLLOWPATHCOMPONENT_HPP_INCLUDED
#define VRIGIDBODYFOLLOWPATHCOMPONENT_HPP_INCLUDED

#ifndef SHARED_IMPEXP
#define SHARED_IMPEXP VHAVOK_IMPEXP
#endif

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>
#include <Vision/Runtime/Engine/SceneElements/VisApiPath.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/IvHavokForcesQuerierComponent.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vApplyForcesAction.hpp>

/// 
/// \brief
///   Path component that can be added to any entity containing a rigid body component so they will be able to 
/// follow any path objects by getting a force applied to the entity.
/// 
class VRigidBodyFollowPathComponent : public IVObjectComponent, public IvHavokForcesQuerierComponent, IVisCallbackHandler_cl
{
public: 

  ///
  /// @name Constructor
  /// @{
  ///

  ///
  ///   \brief
  ///     Component Constructor
  /// 
  ///   \param iComponentFlags
  ///     See IVObjectComponent
  /// 
  ///   \see
  ///     IVObjectComponent
  /// 
  SHARED_IMPEXP VRigidBodyFollowPathComponent(int iComponentFlags=VIS_OBJECTCOMPONENTFLAG_NONE);

  /// 
  ///   \brief
  ///     Component Destructor
  /// 
  SHARED_IMPEXP virtual ~VRigidBodyFollowPathComponent();
  ///
  /// @}
  ///

  ///
  /// @name Component methods
  /// @{
  ///

  ///
  ///   \brief
  ///     Initializes the follow path component by setting the initial values
  /// 
  SHARED_IMPEXP void Init();


  ///
  /// @}
  ///

  ///
  /// @name IVObjectComponent Overrides
  /// @{
  ///

  ///   \brief
  ///     Overridden method to respond to owner changes
  SHARED_IMPEXP virtual void SetOwner(VisTypedEngineObject_cl *pOwner) HKV_OVERRIDE;

  ///   \brief
  ///     Overridden method returning true or false whether if the given object is attachable to this entity or not.
  SHARED_IMPEXP virtual BOOL CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut) HKV_OVERRIDE;

  ///   \brief
  ///     Overridden method to respond to variable changes.
  SHARED_IMPEXP virtual void OnVariableValueChanged(VisVariable_cl *pVar, const char * value) HKV_OVERRIDE;

  ///   \brief
  ///     Callback method that takes care of updating the managed instances each frame.
  SHARED_IMPEXP virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData) HKV_OVERRIDE;

  ///   \brief
  ///     Overridden method that returns a set of forces to be applied to the attached entity in the next simulation step.
  SHARED_IMPEXP virtual const IvHavokForcesQuerierComponent::Forces* QueryForces(float fET, int &iForcesCountOut) HKV_OVERRIDE;

  ///
  /// @}
  ///

  ///
  /// @name Serialization
  /// @{
  ///

  ///   \brief
  ///     RTTI macro
  V_DECLARE_SERIAL_DLLEXP(VRigidBodyFollowPathComponent, SHARED_IMPEXP)

  ///   \brief
  ///     RTTI macro to add a variable table
  V_DECLARE_VARTABLE(VRigidBodyFollowPathComponent, SHARED_IMPEXP)

  ///   \brief
  ///     Serialization function
  ///
  ///   \param ar
  ///     Binary archive
  SHARED_IMPEXP virtual void Serialize(VArchive &ar) HKV_OVERRIDE;

  ///
  /// @}
  ///

  void UpdateSpeed(const hkvVec3& v3CurTarget, const hkvVec3& v3EntityPos);

private:  

  // Exposed members in vForge.
  VString PathKey;                          ///< The object key of the path
  VObjectReference<VisPath_cl> PathObject;  ///< Path object reference
  float InitialOffset;                      ///< Initial offset on the path for the follow point. [0%..100%] 
  float FollowPointOffset;                  ///< Maximum distance from the pull point to the follow point .
  hkvVec3 PositionOffset;                   ///< Pull point relative to the center of the entity. This is where the force will be applied. If behind the center of mass, then a push force will be applied to turn the object.
  BOOL Looped;                              ///< The entity will go through the path in a never-ending loop if this is activated.
  float Speed;                              ///< Speed of the attached entity across the path. Always constant.
  hkvVec3 ForceFactor;                      ///< Factor to apply to each one of the axis-vector of the force applied to move the entity. Quite useful to restrict forces to a set of axis.
  BOOL DrawDebug;                        ///< Enables or disables the debug drawing of the component.
  float DistanceToSlowDown;                 ///< Object will decelerate when it is within this distance of the end point.
  
  // Not exposed.
  VisPath_cl *m_pPath;   ///< Path object with the specified key if found
  float m_fCurrentTime;  ///< Current time needed for evaluating current position on path
  hkvVec3 v3FollowPoint; ///< Position of the point to follow across the path.
  hkvVec3 m_v3OldPos;    ///< Previous position of the point to follow across the path.

  vApplyForcesAction m_ForcesAction;              ///< Action that will be applied to the entity containing the forces.
  IvHavokForcesQuerierComponent::Forces m_aForce; ///< Result of the calculations of the forces to apply to the entity.
  float m_fCurrentSpeed;
  float m_fAcceleration;
};


#endif // VRIGIDBODYFOLLOWPATHCOMPONENT_HPP_INCLUDED

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
