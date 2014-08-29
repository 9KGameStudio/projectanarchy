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
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/VRigidBodyFollowPathComponent.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokRigidBody.hpp>

/// =========================================================================== ///
/// VRigidBodyFollowPathComponent Methods                                       ///
/// =========================================================================== ///
VRigidBodyFollowPathComponent::VRigidBodyFollowPathComponent(int iComponentFlags)
  : IVObjectComponent(0, iComponentFlags)
  // Exposed
  , Looped(true)
  , InitialOffset(10.0f)
  , PositionOffset(hkvVec3::ZeroVector())
  , FollowPointOffset(50.0f)
  , DrawDebug(true)
  , DistanceToSlowDown(0.0)

  // Not exposed
  , Speed(2.0f)
  , m_pPath(NULL)
  , m_fCurrentTime(0.0f)
  , v3FollowPoint(hkvVec3::ZeroVector())
  , m_v3OldPos(hkvVec3::ZeroVector())
{
  // Exposed
  PathKey.Reset();
  PathObject.SetReferencedObject(NULL);

  // Not exposed
  m_ForcesAction.m_pParent = this;
  m_fCurrentSpeed = 0;
  m_fAcceleration = .1;
}

VRigidBodyFollowPathComponent::~VRigidBodyFollowPathComponent()
{
  m_ForcesAction.m_pParent = NULL;
}

void VRigidBodyFollowPathComponent::Init()
{
  if (!m_pOwner) return;

  // Get path by path key
  if (PathObject.GetReferencedObject() != NULL)
    m_pPath = (VisPath_cl*)PathObject.GetReferencedObject();
  else if (!PathKey.IsEmpty())
    m_pPath = Vision::Game.SearchPath(PathKey.AsChar());  

  if (!m_pPath) 
  {
    hkvLog::Info("Follow Path Component: The follow path component does not have any reference to a path object.");
    return;
  }

  ((VisObject3D_cl *)m_pOwner)->SetUseEulerAngles(TRUE);
  m_fCurrentTime = InitialOffset / 100.f;
  
  m_v3OldPos = ((VisObject3D_cl *)m_pOwner)->GetPosition();
}


const IvHavokForcesQuerierComponent::Forces* VRigidBodyFollowPathComponent::QueryForces(float fET, int &iForcesCountOut)
{
  if (!m_pOwner || !m_pPath)  return NULL;

  VisBaseEntity_cl *pEntity = (VisBaseEntity_cl *)m_pOwner;
  vHavokRigidBody *pRB = pEntity->Components().GetComponentOfType<vHavokRigidBody>();
  if (NULL == pRB) return NULL;

  hkvVec3 v3CurTarget;  m_pPath->EvalPointSmooth(m_fCurrentTime, v3CurTarget);
  const hkvVec3 v3ForcePos = pEntity->GetWorldMatrix() * PositionOffset;
  const hkvVec3 v3EntityPos = pEntity->GetPosition();
  const hkvVec3 v3PositionOffsetToFollowPoint = v3CurTarget - v3EntityPos;
  hkvVec3 v3DirNormalized = v3PositionOffsetToFollowPoint.getNormalized();
  
  hkvVec3 v3AngularVelocity = pRB->GetAngularVelocity();
  float fAngularVelocityLen = v3AngularVelocity.getLength();
  
  const float fFrameTimeTravelledDistance = (v3EntityPos - m_v3OldPos).getLength() / m_pPath->GetLen();  // Frame Travelled Distance / Path Length
  
  // Wait if we catch the follow point otherwise let it run and adjust it if we are looping.
  m_fCurrentTime += v3PositionOffsetToFollowPoint.getLength() > FollowPointOffset ? 0.f : 2.0f * fFrameTimeTravelledDistance;
  if (m_fCurrentTime > 1.0f)
    m_fCurrentTime = Looped ? m_fCurrentTime - 1.0f : 1.0f;
  
  UpdateSpeed(v3CurTarget, v3EntityPos);

  hkvVec3 v3DirToTarget = v3CurTarget-v3EntityPos;
  hkvVec3 vForward = pEntity->GetRotationMatrix().getAxis(1);
  hkvVec3 vRight = pEntity->GetRotationMatrix().getAxis(0);

  if (DrawDebug)
  {
    Vision::Game.DrawSingleLine(v3EntityPos, v3EntityPos+v3DirToTarget, V_RGBA_PURPLE);
    Vision::Game.DrawSingleLine(v3EntityPos, v3EntityPos+(vForward*100), V_RGBA_YELLOW);
  }

  hkvVec3 v3CenterOfMass;
  vHavokConversionUtils::PhysVecToVisVecWorld( pRB->GetHkRigidBody()->getCenterOfMassInWorld(), v3CenterOfMass );
  hkvVec3 v3DirToForcePos = v3ForcePos - v3CenterOfMass;
  v3DirToForcePos.z = 0;

  hkvVec3 vForceDir = v3DirNormalized;

  bool bPull = v3DirToForcePos.dot(vForward) > 0;
  int debug=0;

  if ( bPull )
  {
    iForcesCountOut = 1;

    // linear force
    m_aForce.m_v3Dir = vForceDir.compMul(ForceFactor) * pRB->Havok_Mass * m_fCurrentSpeed;
    m_aForce.m_v3Point = v3ForcePos;
  }
  else // push object at center of mass with angular force to turn its heading towards next path point
  {
      //restrict heading to some angle from forward direction
    v3DirToTarget.normalize();

    //vForward.z = 0;
    vForward.normalize();

    // force will be applied at rudder in the back, which will cause the boat to rotate around the center of mass the opposite way
    hkvVec3 vBackwards = vForward;
    vBackwards.negate();
    Vision::Game.DrawSingleLine(v3EntityPos, v3EntityPos+(vBackwards*100), V_RGBA_BLACK);

    //float fMaxHeadingAngleRad = hkvMath::Deg2Rad(45.0f);
    float fMaxHeadingAngle = 45.0;
    float dotToTarget = v3DirToTarget.dot( vForward );
    hkvVec3 vCross = vForward.cross( v3DirToTarget );
    vCross.normalize();

    if (DrawDebug)
    {
      if ( vCross.dot(hkvVec3(0,0,1)) > 0 )
      {
        // need to left
        Vision::Game.DrawSingleLine(v3ForcePos, v3ForcePos+(vRight*-100), V_RGBA_BLACK);
      }
      else
      {
        //need to go right
        Vision::Game.DrawSingleLine(v3ForcePos, v3ForcePos+(vRight*100), V_RGBA_BLACK);
      }
    }
    
    //if ( dotToTarget < cos( fMaxHeadingAngleRad ) )
    //{
      float fAngleRad = acos(dotToTarget);
      //fAngle = min( fAngle, fMaxHeadingAngleRad );
      float fAngleDeg = hkvMath::Rad2Deg( fAngleRad );


      if ( vCross.dot(hkvVec3(0,0,1)) < 0 )
      {
        fAngleDeg *= -1;
      }

      if ( fAngleDeg < -fMaxHeadingAngle )
      {
        fAngleDeg = -fMaxHeadingAngle;
      }
      else if ( fAngleDeg > fMaxHeadingAngle )
      {
        fAngleDeg = fMaxHeadingAngle;
      }

      fAngleDeg *= -1;
      fAngleRad = hkvMath::Deg2Rad( fAngleDeg );

    iForcesCountOut = 1;

    // linear force
    m_aForce.m_v3Dir = vForceDir.compMul(ForceFactor) * pRB->Havok_Mass * m_fCurrentSpeed;
    m_aForce.m_v3Point = v3ForcePos;

    hkvVec3 v3AngularVelocity = pRB->GetAngularVelocity();

    if ( fAngleDeg > 0 )
    {
      v3AngularVelocity.z -= .02;
    }
    else if ( fAngleDeg < 0 )
    {
      v3AngularVelocity += 0.02;
    }

    pRB->SetAngularVelocity( v3AngularVelocity );
  }

  if (DrawDebug)
  {
    Vision::Game.DrawSingleLine(v3ForcePos, v3ForcePos - (vForceDir * 50.f), V_RGBA_RED);
    Vision::Game.DrawCube( v3CurTarget, 5.f, V_RGBA_RED);
    m_pPath->DrawPath();
  }

  m_v3OldPos = v3EntityPos;

  return &m_aForce;
}

void VRigidBodyFollowPathComponent::UpdateSpeed(const hkvVec3& v3CurTarget, const hkvVec3& v3EntityPos)
{
  hkvVec3 v3DirToTarget = v3CurTarget-v3EntityPos;
  v3DirToTarget.z = 0;
  float fDistanceToTarget = v3DirToTarget.getLength();
  float fEndSpeed = 0;

  if ( m_pPath->GetParent() )
  {
    VisBaseEntity_cl* pParent = (VisBaseEntity_cl*)m_pPath->GetParent();
    vHavokRigidBody* pParentRB = pParent->Components().GetComponentOfType<vHavokRigidBody>();
    fEndSpeed = pParentRB->GetLinearVelocity().getLength();
  }
  
  if ( fDistanceToTarget < DistanceToSlowDown )
  {
    //decelerate
    float t = (DistanceToSlowDown - fDistanceToTarget)/DistanceToSlowDown;
    m_fCurrentSpeed = Speed * (1-t) + fEndSpeed * t;
  }
  else if ( m_fCurrentSpeed < Speed )
  {
    //accelerate
    float fTime = Vision::GetTimer()->GetCurrentTime();
    float vSpeedIncrease = ( .5f * m_fAcceleration * fTime*fTime ); 
    m_fCurrentSpeed = hkvMath::Min( Speed, m_fCurrentSpeed + vSpeedIncrease );
  }
}


/// =========================================================================== ///
/// IVObjectComponent Overrides                                                 ///
/// =========================================================================== ///

BOOL VRigidBodyFollowPathComponent::CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut)
{
  if (!IVObjectComponent::CanAttachToObject(pObject, sErrorMsgOut))
    return false;

  if (
    !pObject->IsOfType(V_RUNTIME_CLASS(VisBaseEntity_cl)) 
    || NULL == ((VisBaseEntity_cl *)pObject)->GetMesh()
    || NULL == pObject->Components().GetComponentOfType<vHavokRigidBody>())
  {
    sErrorMsgOut += "Component can only be added to instances of VisBaseEntity entity with a HavokRigidBody attached and the entity must contain a valid model file.";
    
    return FALSE;
  }

  return TRUE;

}

void VRigidBodyFollowPathComponent::SetOwner(VisTypedEngineObject_cl *pOwner)
{
  IVObjectComponent::SetOwner(pOwner);

  if (pOwner != NULL)
  {
    Vision::Callbacks.OnUpdateSceneFinished += this;
    Vision::Callbacks.OnAfterSceneLoaded += this;
    
    vHavokRigidBody *pRB = ((VisBaseEntity_cl *)pOwner)->Components().GetComponentOfType<vHavokRigidBody>();
    if (NULL == pRB) return;
    m_ForcesAction.SetEntity(pRB);
  }
  else
  {
    Vision::Callbacks.OnUpdateSceneFinished -= this;
    Vision::Callbacks.OnAfterSceneLoaded -= this;

    m_ForcesAction.SetEntity(NULL);
  }
}


void VRigidBodyFollowPathComponent::OnVariableValueChanged(VisVariable_cl *pVar, const char * value)
{
  if (!strcmp(pVar->GetName(), "PathObject"))
  {
    if (value == NULL)
      return;

    VTypedObject* pTemp = NULL;
    if (sscanf(value, "%p", &pTemp) == -1)
      return;

    if (pTemp == NULL)
      return;

    PathObject.SetReferencedObject(pTemp);
  }
  else if (!strcmp(pVar->GetName(), "PathKey"))
  {
    if (value == NULL)
      return;

    // Get path by path key
    if (!PathKey.IsEmpty())
    {
      m_pPath = Vision::Game.SearchPath(PathKey.AsChar());

      if (m_pPath)
      {
        PathObject.SetReferencedObject(m_pPath);
      }
    }
  }
}

void VRigidBodyFollowPathComponent::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if (&Vision::Callbacks.OnUpdateSceneFinished == pData->m_pSender)
  {
    // call update function on every component
    if (Vision::Editor.IsAnimatingOrPlaying())
    { // The implementation of the Havok Rigid Body component destroys and recreates the rigid body when
      // changing any of its parameters in vForge. This causes any other components that rely in its rigid
      // body to stop working. Here we re-add the action in the case the rigid body has been deleted from
      // the simulation world.
      vHavokRigidBody *pRB = ((VisBaseEntity_cl *)m_pOwner)->Components().GetComponentOfType<vHavokRigidBody>();
      if (!m_ForcesAction.m_bActionAdded && pRB)  m_ForcesAction.SetEntity(pRB);
    }
  }
  else if (&Vision::Callbacks.OnAfterSceneLoaded == pData->m_pSender)
  {
    this->Init();
  }
}

// Versions
#define RIGIDBODYFOLLOWPATHCOMPONENT_VERSION_0          0     // Initial version
#define RIGIDBODYFOLLOWPATHCOMPONENT_VERSION_1          1     // Added DistanceToSlowDown
#define RIGIDBODYFOLLOWPATHCOMPONENT_VERSION_CURRENT    RIGIDBODYFOLLOWPATHCOMPONENT_VERSION_1     // Current version

// Register the class in the engine module so it is available for RTTI
extern VModule g_vHavokModule;
V_IMPLEMENT_SERIAL(VRigidBodyFollowPathComponent,IVObjectComponent,0,&g_vHavokModule);

void VRigidBodyFollowPathComponent::Serialize( VArchive &ar )
{
  char iLocalVersion = RIGIDBODYFOLLOWPATHCOMPONENT_VERSION_CURRENT;
  IVObjectComponent::Serialize(ar);
  if (ar.IsLoading())
  {
    ar >> iLocalVersion;
    m_pPath = NULL;
    ar >> m_pPath;
    if (m_pPath==NULL)
      ar >> PathKey;
    ar >> Looped;
    ar >> InitialOffset;

    ar >> FollowPointOffset;
    SerializeX( ar, PositionOffset );
    ar >> Speed;
    SerializeX( ar, ForceFactor );

    ar >> DistanceToSlowDown;

    PathObject.SetReferencedObject(m_pPath);
  } 
  else
  {
    ar << iLocalVersion;
    ar << m_pPath;
    if (m_pPath==NULL)
      ar << PathKey;
    ar << Looped; 
    ar << InitialOffset;
    
    ar << FollowPointOffset;
    SerializeX( ar, PositionOffset );
    ar << Speed;
    SerializeX( ar, ForceFactor );

    ar << DistanceToSlowDown;
  }
}

/// =========================================================================== ///
/// VRigidBodyFollowPathComponent Variable Table                                         ///
/// =========================================================================== ///

START_VAR_TABLE(VRigidBodyFollowPathComponent,IVObjectComponent, "Rigid Body's Follow Path Component. Can be attached to any 3D object that has already a rigid body attached. It will follow a path shape specified with the path key.", VVARIABLELIST_FLAGS_NONE, "Rigid Body's Follow Path " )
  DEFINE_VAR_VSTRING(VRigidBodyFollowPathComponent, PathKey, "The object key of the path shape to follow. Obsolete if a PathObject is specified directly", "", 0,0, NULL);
  DEFINE_VAR_OBJECT_REFERENCE(VRigidBodyFollowPathComponent, PathObject, "Reference to the path the owner object should move on", NULL, 0, 0);
  DEFINE_VAR_BOOL(VRigidBodyFollowPathComponent, Looped, "If true, the owner object will move along the path in loop", "TRUE", 0, NULL);
  DEFINE_VAR_FLOAT(VRigidBodyFollowPathComponent, InitialOffset, "Initial offset on the path [0..100]", "0.0", 0, "Slider(0,100)");
  
  DEFINE_VAR_FLOAT(VRigidBodyFollowPathComponent, FollowPointOffset, "Maximum distance the point to follow will take in the path when going through it. It is calculated from the PositionOffset parameter.", "0.0", 0, 0); 
  DEFINE_VAR_FLOAT(VRigidBodyFollowPathComponent, DistanceToSlowDown, "The rigid body will decelerate within this distance of the end point.", "0.0", 0, 0); 
  DEFINE_VAR_VECTOR_FLOAT(VRigidBodyFollowPathComponent, PositionOffset, "Position offset relative to the center of the mesh that will be used to pull the entity over the path.", "0.0/0.0/0.0", 0, 0);

  DEFINE_VAR_FLOAT(VRigidBodyFollowPathComponent, Speed, "Speed of the rigid body in m/s", "0.0", 0, "Slider(0,100)");
  DEFINE_VAR_VECTOR_FLOAT(VRigidBodyFollowPathComponent, ForceFactor, "Factor by which the resultant force will be multiplied.", "1.0/1.0/1.0", 0, 0);
  DEFINE_VAR_BOOL(VRigidBodyFollowPathComponent, DrawDebug, "Show debug draw.", "TRUE", 0, NULL);
END_VAR_TABLE

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
