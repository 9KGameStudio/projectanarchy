/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokCharacterController.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokCharacterControllerHelpers.hpp>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>

// -------------------------------------------------------------------------- //
// Constructor / Destructor                                                   //
// -------------------------------------------------------------------------- //

vHavokCharacterController::vHavokCharacterController()
  : IVisPhysicsObject_cl()
  , Capsule_Radius(0.5f * vHavokConversionUtils::GetHavok2VisionScale())
  , Character_Top(0.f, 0.f, (1.8f * vHavokConversionUtils::GetHavok2VisionScale()) - Capsule_Radius)
  , Character_Bottom(0.f, 0.f, Capsule_Radius)
  , Static_Friction(0.f)
  , Dynamic_Friction(0.8f)
  , Max_Slope(75.f)
  , Character_Mass(100.f)
  , Character_Strength(5000.f)
  , Gravity_Scale(1.f)
  , Jump_Height(1.5f)
  , Step_Height(30.f) // in centimeters
  , Step_Threshold(60.f) // in degrees
  , Max_Velocity(1500.0f) // in centimeters / seconds
  , Max_Acceleration(4000.0f) // in centimeters / seconds^2
  , PenetrationRecoverySpeed(0.5f)
  , m_bEnabled(TRUE)
  , Debug(FALSE)
  , DebugColor(255, 50, 50, 255)
  , Fly_State(FALSE)
  , m_pCharacterProxy(NULL)
  , m_pCharacterContext(NULL)
  , m_currentVelocity(0.0f)
  , m_wantJump(false)
  , m_characterInput()
  , m_iCollisionFilter(hkpGroupFilter::calcFilterInfo(vHavokPhysicsModule::HK_LAYER_COLLIDABLE_CONTROLLER))
{
  m_characterInput.m_fly = false;
}

vHavokCharacterController::~vHavokCharacterController()
{
}

void vHavokCharacterController::SetEnabled(BOOL bEnabled)
{
  m_bEnabled = bEnabled;

  if (m_pCharacterProxy != NULL)
  {
    vHavokPhysicsModule* pModule = vHavokPhysicsModule::GetInstance();
    VASSERT(pModule != NULL);

    if (m_bEnabled && m_pCharacterProxy->getShapePhantom()->getWorld() == NULL)
    {
      pModule->AddCharacterController(this);

      // Set Position in case it has changed
      VisBaseEntity_cl* pOwnerEntity = static_cast<VisBaseEntity_cl*>(GetOwner());
      SetPosition(pOwnerEntity->GetPosition());
    }
    else if (!m_bEnabled && m_pCharacterProxy->getShapePhantom()->getWorld() != NULL)
    {
      pModule->RemoveCharacterController(this);
    }
  }
}

// -------------------------------------------------------------------------- //
// IVObjectComponent Virtual Overrides                                        //
// -------------------------------------------------------------------------- //

void vHavokCharacterController::SetOwner(VisTypedEngineObject_cl *pOwner)
{
  IVObjectComponent::SetOwner(pOwner);

  // Do not initialize the component in case our module is not active
  if (!vHavokPhysicsModule::GetInstance())
  {
    hkvLog::Warning("Failed to initialize vHavokCharacterController since a non Havok physics module is currently active");
    return;
  }

  // Insert code here to respond to attaching this component to an object
  // This function is called with pOwner==NULL when de-attaching.
  if (pOwner!=NULL)
  {
    if (m_pCharacterProxy == NULL)
    {
      CreateHavokController();
      SetDebugRendering(Debug);
    }
    m_characterInput.m_fly = Fly_State;
  }
  else
  { 
    DeleteHavokController();
  }
}

BOOL vHavokCharacterController::CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut)
{
  if (!IVObjectComponent::CanAttachToObject(pObject, sErrorMsgOut))
    return FALSE;

  // Define criteria here that allows the editor to attach this component to the passed object.
  // In our example, the object should be derived from VisObject3D_cl to be positionable.
  BOOL bIsValidClass = pObject->IsOfType(V_RUNTIME_CLASS(VisBaseEntity_cl));
  if (!bIsValidClass)
    sErrorMsgOut += "Component can only be added to instances of VisBaseEntity_cl or derived classes.";

  if ( !bIsValidClass)
    return FALSE;

  // vHavokCharacterController component can not be attached to objects, that already have a vHavokRigidBody component attached
  if (pObject->Components().GetComponentOfType<vHavokRigidBody>())
    return FALSE;

  return TRUE;
}

void vHavokCharacterController::OnVariableValueChanged(VisVariable_cl *pVar, const char * value)
{
  if (m_pCharacterProxy == NULL)
    return;

  // this value is used directly and does not require the controller to be reinitialized
  if (!strcmp(pVar->name,"Gravity_Scale")) 
  {
    return;
  }
  else if(!strcmp(pVar->name, "m_bEnabled"))
  {
    SetEnabled(m_bEnabled);
    return;
  }

  // Recreate controller
  DeleteHavokController();
  CreateHavokController();

  // Check rendering
  if (m_pCharacterProxy)
    SetDebugRendering(Debug);
}

void vHavokCharacterController::MessageFunction( int iID, INT_PTR iParamA, INT_PTR iParamB )
{
  IVObjectComponent::MessageFunction(iID,iParamA,iParamB);

  if ( iID == VIS_MSG_EDITOR_PROPERTYCHANGED )
  {
    const char *szPropertyName = (const char *) iParamA;
    if (_stricmp(szPropertyName, "ModelFile") == 0)
    {
      // Recreate controller
      DeleteHavokController();
      CreateHavokController();

      // Check rendering
      if (m_pCharacterProxy)
        SetDebugRendering(Debug);

      return;
    }
    if (_stricmp(szPropertyName, "Scaling") == 0)
    {
      // update scaling
      UpdateBoundingVolume();
      return;
    }
  }
  else if ( iID == VIS_MSG_HAVOK_ONCOLLISION )
  {
    vHavokPhysicsModule::TriggerCollisionScriptFunction(this, (vHavokCollisionInfo_t *)iParamA);
    return;
  }
}

// -------------------------------------------------------------------------- //
// Serialization                                                              //
// -------------------------------------------------------------------------- //

// Register the class in the engine module so it is available for RTTI
V_IMPLEMENT_SERIAL(vHavokCharacterController, IVObjectComponent, 0, &g_vHavokModule);

/// Serialization versions
#define VHAVOKCHARACTERCONTROLLER_VERSION_0           0     // Initial version
#define VHAVOKCHARACTERCONTROLLER_VERSION_1           1     // More exposed properties
#define VHAVOKCHARACTERCONTROLLER_VERSION_2           2     // Switchable motion delta Z behavior
#define VHAVOKCHARACTERCONTROLLER_VERSION_3           3     // Gravity_Scale
#define VHAVOKCHARACTERCONTROLLER_VERSION_4           4     // Jump_Height
#define VHAVOKCHARACTERCONTROLLER_VERSION_5           5     // Step_Height, Step_Threshold
#define VHAVOKCHARACTERCONTROLLER_VERSION_6           6     // Enabled state
#define VHAVOKCHARACTERCONTROLLER_VERSION_7           7     // Max Velocity / Acceleration / Penetration Speed
#define VHAVOKCHARACTERCONTROLLER_VERSION_CURRENT     VHAVOKCHARACTERCONTROLLER_VERSION_7

void vHavokCharacterController::Serialize(VArchive &ar)
{
  char iLocalVersion = VHAVOKCHARACTERCONTROLLER_VERSION_CURRENT;

  IVObjectComponent::Serialize(ar);
  if (ar.IsLoading())
  {
    char iFileVersion = 0;
    ar >> iFileVersion;
    VASSERT_MSG(iFileVersion >= VHAVOKCHARACTERCONTROLLER_VERSION_0 && 
      iFileVersion <= VHAVOKCHARACTERCONTROLLER_VERSION_CURRENT, 
      "Invalid file version. Please re-export");

    // VHAVOKCHARACTERCONTROLLER_VERSION_0
    Character_Top.SerializeAsVec3 (ar);
    Character_Bottom.SerializeAsVec3 (ar);
    ar >> Capsule_Radius; 

    // VHAVOKCHARACTERCONTROLLER_VERSION_1
    if (iFileVersion >= VHAVOKCHARACTERCONTROLLER_VERSION_1)
    {
      ar >> Static_Friction;
      ar >> Dynamic_Friction;
      ar >> Max_Slope;
      ar >> Character_Mass;
      ar >> Character_Strength;
    }

    // VHAVOKCHARACTERCONTROLLER_VERSION_2
    if (iFileVersion >= VHAVOKCHARACTERCONTROLLER_VERSION_2)
    {
      ar >> Fly_State;
    }

    // VHAVOKCHARACTERCONTROLLER_VERSION_3
    if (iFileVersion >= VHAVOKCHARACTERCONTROLLER_VERSION_3)
    {
      ar >> Gravity_Scale;
    }

    // VHAVOKCHARACTERCONTROLLER_VERSION_4
    if ( iFileVersion >= VHAVOKCHARACTERCONTROLLER_VERSION_4 )
    {
      ar >> Jump_Height;
    }

    // VHAVOKCHARACTERCONTROLLER_VERSION_5
    if ( iFileVersion >= VHAVOKCHARACTERCONTROLLER_VERSION_5 )
    {
      ar >> Step_Height;
      ar >> Step_Threshold;
    }

    // VHAVOKCHARACTERCONTROLLER_VERSION_6
    if (iFileVersion >= VHAVOKCHARACTERCONTROLLER_VERSION_6)
    {
      ar >> m_bEnabled;
    }

    // VHAVOKCHARACTERCONTROLLER_VERSION_7
    if (iFileVersion >= VHAVOKCHARACTERCONTROLLER_VERSION_7)
    {
      ar >> Max_Velocity;
      ar >> Max_Acceleration;
      ar >> PenetrationRecoverySpeed;
    }
  }
  else
  {
    ar << iLocalVersion;

    // VHAVOKCHARACTERCONTROLLER_VERSION_0
    Character_Top.SerializeAsVec3 (ar);
    Character_Bottom.SerializeAsVec3 (ar);
    ar << Capsule_Radius;

    // VHAVOKCHARACTERCONTROLLER_VERSION_1
    ar << Static_Friction;
    ar << Dynamic_Friction;
    ar << Max_Slope;
    ar << Character_Mass;
    ar << Character_Strength;

    // VHAVOKCHARACTERCONTROLLER_VERSION_2
    ar << Fly_State;

    // VHAVOKCHARACTERCONTROLLER_VERSION_3
    ar << Gravity_Scale;

    // VHAVOKCHARACTERCONTROLLER_VERSION_4
    ar << Jump_Height;

    // VHAVOKCHARACTERCONTROLLER_VERSION_5
    ar << Step_Height;
    ar << Step_Threshold;

    // VHAVOKCHARACTERCONTROLLER_VERSION_6
    ar << m_bEnabled;

    // VHAVOKCHARACTERCONTROLLER_VERSION_7
    ar << Max_Velocity;
    ar << Max_Acceleration;
    ar << PenetrationRecoverySpeed;
  }
}

void vHavokCharacterController::OnDeserializationCallback(const VSerializationContext &context)
{
  if(!m_pCharacterProxy)
  {
    CreateHavokController();
    SetDebugRendering(Debug);
  }
}

// -------------------------------------------------------------------------- //
// Simulation Step                                                             //
// -------------------------------------------------------------------------- //

class CharacterPointCollector: public hkpAllCdPointCollector 
{ 
public:
  HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_DEMO, CharacterPointCollector);

  CharacterPointCollector()
    : hkpAllCdPointCollector()
  { }

  virtual void addCdPoint(const hkpCdPoint& event) 
  {
    const hkpCollidable* pCollidable = event.m_cdBodyB.getRootCollidable();
    if(m_collidables.indexOf(pCollidable) == -1)
    {
      m_collidables.pushBack(pCollidable);

      m_shapes.pushBack(event.m_cdBodyB.getShapeKey()); // support for reporting material type of the mesh we are on etc

      // Push back contact . N.B. The *distance* here is may be an actual (world ) distance,
      // or it may be a parameterization [0,1] of a linear cast, depending on what algorithm is using the collector.
      m_contacts.pushBack(event.getContact());
    }

    hkpAllCdPointCollector::addCdPoint(event); 
  }

  inline void reset()
  {
    hkpAllCdPointCollector::reset();

    m_collidables.clear();
    m_shapes.clear();
    m_contacts.clear();
  }

  hkArray<const hkpCollidable*> m_collidables;
  hkArray<hkpShapeKey> m_shapes;
  hkArray<hkContactPoint> m_contacts;
}; 

void vHavokCharacterController::Step(float fTimeStep, int iNumSteps, float fAccumulatedTime)
{
  vHavokPhysicsModule* pModule = vHavokPhysicsModule::GetInstance();
  if ((!GetOwner()) || (!pModule) || (!pModule->GetPhysicsWorld()) || (!m_pCharacterProxy))
    return;

  // Always draw the debug geometry.
  m_pCharacterProxy->drawDebug(this);

  if (iNumSteps < 1)
    return;

  VisBaseEntity_cl* pOwnerEntity = vstatic_cast<VisBaseEntity_cl*>(GetOwner());

  // Process Motion Delta.
  hkvVec3 vMotionVelocity;
  {
    const hkvVec3 vMotionDelta = pOwnerEntity->GetMotionDeltaWorldSpace() + 
      pOwnerEntity->GetRotationMatrix() * pOwnerEntity->GetMotionDeltaLocalSpace();

    const float fTotalSimulationTime = static_cast<float>(iNumSteps) * fTimeStep;
    const float fUsedFraction = hkvMath::saturate(fTotalSimulationTime / fAccumulatedTime);
    const hkvVec3 vMotionDeltaUsed = vMotionDelta * fUsedFraction;

    // The controller will have effects such as gravity in its previous velocity
    // too. So we really just want to take the planar part (XY) from the motion 
    // delta and the Z (gravity etc) from the previous Havok step.
    // This is use-case dependent, so feel free to change.
    vMotionVelocity.x = vMotionDeltaUsed.x / fTotalSimulationTime;
    vMotionVelocity.y = vMotionDeltaUsed.y / fTotalSimulationTime;
    vMotionVelocity.z = Fly_State ? (vMotionDeltaUsed.z / fTotalSimulationTime) : 0.0f;

    // Re-add the remaining motion delta for the next simulation step.
    pOwnerEntity->ResetMotionDelta();
    pOwnerEntity->SetMotionDeltaWorldSpace(vMotionDelta - vMotionDeltaUsed);
  }

  hkVector4 hVel;
  vHavokConversionUtils::VisVecToPhysVecLocal(vMotionVelocity, hVel);

  // We back up the maxSlopeCosine parameter, as it will be reset if character is on a step.
  const hkReal maxSlopeCosine = m_pCharacterProxy->m_maxSlopeCosine;
 
  // Setup character input / output.
  hkpCharacterOutput output;
  hkpCharacterInput input;
  input.m_userData = hkUlong(&m_characterInput); // set additional character input (currently only fly_state)
  input.m_inputLR = hkReal(0);
  (-hVel.length<3>()).store<1>(&input.m_inputUD); // negative for always forward movement
  input.m_wantJump = m_wantJump;
  input.m_atLadder = false;
  input.m_stepInfo.m_deltaTime = fTimeStep;
  input.m_stepInfo.m_invDeltaTime = 1.0f / fTimeStep;

  // Forward vector should not be zero.
  if (hkMath::equal(input.m_inputUD, 0.0f))
  {
    input.m_inputUD = hkReal(0);
    input.m_forward = hkVector4::getConstant<HK_QUADREAL_1000>();
  }
  else
  {
    input.m_forward = hVel;
    input.m_forward.normalize<3>();
  }

  hkpWorld* world = pModule->GetPhysicsWorld();
  world->lock();

  hkVector4 scaledGravity = world->getGravity();
  scaledGravity.mul(hkSimdReal::fromFloat(Gravity_Scale));
  input.m_characterGravity = scaledGravity;

  GetUpVectorFromGravity(input.m_up);
  hkVector4 down;
  down.setNeg4(input.m_up);

  // Update the state.
  hkpAllCdPointCollector castContactsCollector;
  CharacterPointCollector startContactsCollector;

  for (int i = 0; i < iNumSteps; i++)
  {
    const float fPhysicsTime = world->getCurrentTime() + static_cast<float>(i) * fTimeStep;
    m_pCharacterProxy->m_fCurrentTime = fPhysicsTime;
    m_pCharacterProxy->clearPushedRigidBodies();

    // For the fly state use the motion delta as velocity, for non-fly state set the last velocity.
    input.m_velocity = Fly_State ? hVel : m_pCharacterProxy->getLinearVelocity();
    input.m_position = m_pCharacterProxy->getPosition();

    m_pCharacterProxy->checkSupport(down, input.m_surfaceInfo);

    m_pCharacterContext->update(input, output);

    // Handle steps (can modify character position and output.m_velocity)
    if (Step_Height >= 0.0f && Step_Threshold > 0.0f && Step_Threshold <= 90.0f)
    {
      m_pCharacterProxy->handleSteps(this, input, output);
    }

    m_pCharacterProxy->setLinearVelocity(output.m_velocity);

    // We set m_maxSlopeCosine to 0 which implicitly implies that the maximum slope 
    // that the character can climb is vertical.
    // This ultimately disables the additional vertical planes insertion, which is 
    // required when the character is resting on step edges.
    m_pCharacterProxy->m_maxSlopeCosine = m_pCharacterProxy->isOnStep() ? hkReal(0) : maxSlopeCosine;

    hkStepInfo si(fPhysicsTime, fPhysicsTime + fTimeStep);
    m_pCharacterProxy->integrateWithCollectors(si, scaledGravity, castContactsCollector, startContactsCollector);

    // May want to know some collision events (with the immediate hit objects, so just the start collector)
    for (int ci = 0; ci < startContactsCollector.m_contacts.getSize(); ++ci)
    {
      vHavokCollisionInfo_t info;

      // Fake some rb like contact info
      const hkContactPoint& contactPoint = startContactsCollector.m_contacts[ci];
      const hkpCollidable* pCollidable = startContactsCollector.m_collidables[ci];
      const hkpWorldObject* pHitObject = hkpGetWorldObject(pCollidable);

      vHavokConversionUtils::PhysVecToVisVecWorld(contactPoint.getPosition(), info.m_vPoint);
      vHavokConversionUtils::PhysVecToVisVec_noscale(contactPoint.getNormal(), info.m_vNormal);
      info.m_fVelocity = 0; //Todo: compute this (See Physics/Dynamics/Constraint/Contact/hkpSimpleContactConstraintUtil)
      info.m_Collider[0].m_eType = V_USERDATA_OBJECT;
      info.m_Collider[0].m_pCharacter = this;

      info.m_Collider[1].SetInfo((void *)(pHitObject->getUserData())); // ptr back to the vHavokRigidBody, terrain etc
      switch (info.m_Collider[1].m_eType)
      {
      case V_USERDATA_OBJECT:
      case V_USERDATA_TERRAIN:
        {
          // Perform a check to be on the safe side, though the hitObject should never be a character here.
          if (!info.m_Collider[1].m_pCharacter)
          {
            hkpRigidBody *pRigidBody = hkpGetRigidBody(pCollidable); 
            VASSERT(pRigidBody);
            info.m_Materials[1].fDynamicFriction = static_cast<float>(pRigidBody->getFriction());
            info.m_Materials[1].fRestitution = static_cast<float>(pRigidBody->getRestitution());
          }
        }
        break;

      case V_USERDATA_STATIC:
        {
          // Get the leaf shape key of the contact
          const hkpShapeKey& key = startContactsCollector.m_shapes[ci];

          // Get shape
          const hkpShape *pShape = info.m_Collider[1].m_pStaticMesh->GetHkShape();
          VASSERT(pShape);

          if (pShape->getClassType() == &hkvBvCompressedMeshShapeClass)
          {
            const hkvBvCompressedMeshShape *pMeshShape = static_cast<const hkvBvCompressedMeshShape*>(pShape);
            const hkvMeshMaterialCache& matCache = pMeshShape->GetMaterialCache();
            if (!matCache.isEmpty())
            {
              hkUint32 matId = pMeshShape->getPrimitiveUserData( key );
              VASSERT(matId <= static_cast<hkUint32>(matCache.getSize()));
              const hkvMeshMaterial& pMaterial = matCache[matId];

              info.m_Materials[1].fDynamicFriction = pMaterial.m_fFriction;
              info.m_Materials[1].fRestitution = pMaterial.m_fRestitution;
              info.m_Materials[1].szUserData = pMaterial.m_userData.cString(); 

              break;
            }
          }
          
          hkpRigidBody* pRigidBody = info.m_Collider[1].m_pStaticMesh->GetHkRigidBody();
          info.m_Materials[1].fDynamicFriction = static_cast<float>(pRigidBody->getFriction());
          info.m_Materials[1].fRestitution = static_cast<float>(pRigidBody->getRestitution());
        }
        break;

      default:
        break;
      }

      info.m_pSenderTarget[0] = GetOwner();
      //Need to send to rb as well?
      if (info.m_Collider[1].m_pRigidBody && info.m_Collider[1].m_pRigidBody->GetOwner())
        info.m_pSenderTarget[1] = info.m_Collider[1].m_pRigidBody->GetOwner();
      vHavokPhysicsModule::GetInstance()->OnObjectCollision(info);
    }

    // Reset collectors.
    castContactsCollector.reset();
    startContactsCollector.reset();
  }

  // Restore maxSlopeCosine
  m_pCharacterProxy->m_maxSlopeCosine = maxSlopeCosine;

  // Get new velocity.
  vHavokConversionUtils::PhysVecToVisVecLocal(m_pCharacterProxy->getLinearVelocity(), m_currentVelocity);

  world->unlock();

  m_wantJump = false;
}

void vHavokCharacterController::UpdateOwner()
{
  vHavokPhysicsModule* pModule = vHavokPhysicsModule::GetInstance();
  if ((!GetOwner()) || (!pModule) || (!pModule->GetPhysicsWorld()))
    return;

  VisBaseEntity_cl *pOwnerEntity = static_cast<VisBaseEntity_cl*>(GetOwner());

  // Rotation is not relevant for capsule so apply rotation delta directly.
  if (pOwnerEntity->HasRotationDelta())
  {
    const hkvVec3 vRot = pOwnerEntity->GetRotationDelta();
    pOwnerEntity->IncOrientation(vRot);
    pOwnerEntity->ResetRotationDelta();
  }

  if (m_pCharacterProxy)
  {
    hkvVec3 vPos;

    // Get the transformation from Havok Physics.
    pModule->MarkForRead();
    const hkVector4& hPos = m_pCharacterProxy->getPosition();
    vHavokConversionUtils::PhysVecToVisVecWorld(hPos, vPos); 
    pModule->UnmarkForRead();

    // Rotation is not relevant for capsule.
    pOwnerEntity->SetPosition(vPos);
  }
}

void vHavokCharacterController::GetCurrentLinearVelocity(hkvVec3& currentVelocity) const
{
  currentVelocity = m_currentVelocity;
}

void vHavokCharacterController::ApplyEntityScale(hkVector4 &vTop, hkVector4 &vBottom, float &fRadius)
{
  VisBaseEntity_cl *pOwnerEntity = (VisBaseEntity_cl *) GetOwner();
  VASSERT(pOwnerEntity);

  hkvMat4 worldTransform;
  pOwnerEntity->GetWorldMatrix(worldTransform);
  hkvVec3 vScale = worldTransform.getScalingFactors();
  hkVector4 vHkScale; 
  vHavokConversionUtils::VisVecToPhysVec_noscale(vScale, vHkScale);
 
  vTop.mul(vHkScale);
  vBottom.mul(vHkScale);
  fRadius = fRadius*vScale[1];

  // Check bounds (top and bottom)
  hkVector4 vd; 
  vd.setSub(vTop, vBottom);
  if (vd.lengthSquared<3>() < hkSimdReal::fromFloat(HKVMATH_LARGE_EPSILON*HKVMATH_LARGE_EPSILON))
  {
    vTop.setAddMul(vBottom, hkVector4::getConstant<HK_QUADREAL_0010>(), hkSimdReal::fromFloat(HKVMATH_LARGE_EPSILON));
    hkvLog::Info("Warning: The points which define the vHavokCharacterController capsule shape are equal - capsule is auto-adjusted!");
  }

  // Check Radius
  if (fRadius < HKVMATH_LARGE_EPSILON)
  {
    fRadius = HKVMATH_LARGE_EPSILON;
    hkvLog::Info("Warning: The capsule shape radius of the vHavokCharacterController is too small - capsule is auto-adjusted!");
  }
}

void vHavokCharacterController::CreateStateMachineAndContext()
{
  hkpCharacterState* state;
  hkpCharacterStateManager* manager = new hkpCharacterStateManager();

  state = new vCharacterStateOnGround();
  ((vCharacterStateOnGround*)state)->setSpeed(1.0f);
  manager->registerState( state,	HK_CHARACTER_ON_GROUND);
  state->removeReference();

  state = new vCharacterStateInAir();
  ((vCharacterStateInAir*)state)->setSpeed(1.0f);
  manager->registerState( state,	HK_CHARACTER_IN_AIR);
  state->removeReference();

  state = new vCharacterStateJumping();
  ((vCharacterStateJumping*)state)->setJumpHeight(Jump_Height);
  manager->registerState( state,	HK_CHARACTER_JUMPING);
  state->removeReference();

  state = new vCharacterStateClimbing();
  manager->registerState( state,	HK_CHARACTER_CLIMBING);
  state->removeReference();

  state = new vCharacterStateFlying();
  manager->registerState( state,	HK_CHARACTER_FLYING);
  state->removeReference();

  m_pCharacterContext = new hkpCharacterContext(manager, HK_CHARACTER_ON_GROUND);
  m_pCharacterContext->setCharacterType(hkpCharacterContext::HK_CHARACTER_PROXY);
  m_pCharacterContext->setFilterEnable(true);
  m_pCharacterContext->setFilterParameters(1.0f, VIS2HK_FLOAT_SCALED(Max_Velocity), VIS2HK_FLOAT_SCALED(Max_Acceleration));

  manager->removeReference();
}

void vHavokCharacterController::CreateHavokController()
{
  // Do not initialize the component in case our module is not active
  vHavokPhysicsModule* pModule = vHavokPhysicsModule::GetInstance();
  if (pModule == NULL)
  {
    hkvLog::Warning("Failed to initialize vHavokCharacterController since a non Havok physics module is currently active");
    return;
  }

  // allow reinit
  if (m_pCharacterProxy != NULL) 
  {
    DeleteHavokController();
  }

  VisBaseEntity_cl *pOwnerEntity = (VisBaseEntity_cl *) GetOwner();
  VASSERT(pOwnerEntity != NULL);

  m_characterInput.m_fly = Fly_State;

  // Construct a shape
  hkVector4 vTop; vHavokConversionUtils::VisVecToPhysVecLocal(Character_Top, vTop);
  hkVector4 vBottom; vHavokConversionUtils::VisVecToPhysVecLocal(Character_Bottom, vBottom);
  float fRadius = float(VIS2HK_FLOAT_SCALED(Capsule_Radius));
    
  ApplyEntityScale(vTop, vBottom, fRadius);

  // Create a capsule to represent the character standing
  hkpShape* standCapsule = new hkpCapsuleShape(vTop, vBottom, fRadius);

  // Construct a Shape Phantom
  hkTransform hkTfOut;
  vHavokConversionUtils::VisMatVecToPhysTransformWorld(hkvMat3::IdentityMatrix(), pOwnerEntity->GetPosition(), hkTfOut);
  hkpShapePhantom* phantom = new hkpSimpleShapePhantom(standCapsule, hkTfOut, m_iCollisionFilter);
  standCapsule->removeReference();

  // Construct a character proxy
  hkpCharacterProxyCinfo cpci;
  vHavokConversionUtils::VisVecToPhysVecWorld(pOwnerEntity->GetPosition(), cpci.m_position);

  GetUpVectorFromGravity(cpci.m_up);

  // Controller properties
  cpci.m_staticFriction = Static_Friction;
  cpci.m_dynamicFriction = Dynamic_Friction;
  cpci.m_userPlanes = 4;
  cpci.m_keepDistance = 0.05f;
  cpci.m_maxSlope = hkvMath::Deg2Rad(Max_Slope);

  // This value will affect how much the character pushes down on objects it stands on. 
  cpci.m_characterMass = Character_Mass;

  // This value will affect how much the character is able to push other objects around.
  cpci.m_characterStrength = Character_Strength;

  cpci.m_shapePhantom = phantom;

  cpci.m_penetrationRecoverySpeed = PenetrationRecoverySpeed;

  // Temporarily disable warning: "Shape phantom has not yet been added to the world. Initial position has been ignored".
  // If the character controller is enabled then the phantom will be added to the world and its position will be updated below.
  hkError::getInstance().setEnabled(0x6cee9071, false);
  m_pCharacterProxy = new vHavokCharacterPushableProxy( cpci, this );
  hkError::getInstance().setEnabled(0x6cee9071, true);

  //
  // Create the Character state machine and context
  //
  CreateStateMachineAndContext();

  if (m_bEnabled)
  {
    pModule->AddCharacterController(this);
    SetPosition(pOwnerEntity->GetPosition());
  }
}

//-----------------------------------------------------------------------------------

void vHavokCharacterController::SetCollisionInfo(int iLayer, int iGroup,
  int iSubsystem, int iSubsystemDontCollideWith)
{
  int iFilter = hkpGroupFilter::calcFilterInfo(
    iLayer, iGroup, iSubsystem, iSubsystemDontCollideWith);

  SetCollisionInfo(iFilter);
}

void vHavokCharacterController::SetCollisionInfo(int iCollisionFilter)
{
  VVERIFY_OR_RET(m_pCharacterProxy);
  vHavokPhysicsModule* pModule = vHavokPhysicsModule::GetInstance();
  VASSERT(pModule != NULL);

  m_iCollisionFilter = iCollisionFilter;

  pModule->MarkForWrite();

  m_pCharacterProxy->getShapePhantom()->getCollidableRw()->setCollisionFilterInfo(iCollisionFilter);

  hkpWorld* pWorld = pModule->GetPhysicsWorld();
  if (pWorld != NULL)
  {
    pWorld->updateCollisionFilterOnPhantom(m_pCharacterProxy->getShapePhantom(), 
      HK_UPDATE_COLLECTION_FILTER_IGNORE_SHAPE_COLLECTIONS);
  }

  pModule->UnmarkForWrite();
}

//-----------------------------------------------------------------------------------

void vHavokCharacterController::SetPosition(const hkvVec3 &x)
{
  hkVector4 vec; vHavokConversionUtils::VisVecToPhysVecWorld(x,vec);

  if (m_pCharacterProxy)
  {
    vHavokPhysicsModule* pModule = vHavokPhysicsModule::GetInstance();
    VASSERT(pModule != NULL);

    pModule->MarkForWrite();
    m_pCharacterProxy->setPosition(vec);
    pModule->UnmarkForWrite();
  }
}

hkvVec3 vHavokCharacterController::GetPosition() const
{
  if (m_pCharacterProxy)
  {
    hkvVec3 v; 
        vHavokPhysicsModule* pModule = vHavokPhysicsModule::GetInstance();
        VASSERT(pModule != NULL);

    pModule->MarkForRead();
    const hkVector4& vec = m_pCharacterProxy->getPosition();
    vHavokConversionUtils::PhysVecToVisVecWorld(vec,v);
    pModule->UnmarkForRead();
    return v;
  }
  return hkvVec3::ZeroVector();
}

bool vHavokCharacterController::IsStanding() const
{
  return CheckSupport(hkvVec3(0,0,-1));
}

bool vHavokCharacterController::CheckSupport(const hkvVec3& v) const
{
  hkVector4 dir;
  vHavokConversionUtils::VisVecToPhysVec_noscale(v,dir);
  hkpSurfaceInfo pSurfaceInfo;

  if (m_pCharacterProxy)
  {
    vHavokPhysicsModule* pModule = vHavokPhysicsModule::GetInstance();
    VASSERT(pModule != NULL);
    hkpWorld* world = pModule->GetPhysicsWorld();
    if (world)  world->lock();
    m_pCharacterProxy->checkSupport(dir, pSurfaceInfo);
    if (world) world->unlock();

    return (pSurfaceInfo.m_supportedState != hkpSurfaceInfo::UNSUPPORTED);
  }
  else
  {
    return false;
  }
}

void vHavokCharacterController::SetWantJump( bool wantJump )
{
  m_wantJump = wantJump;
}

void vHavokCharacterController::UpdateBoundingVolume()
{
  hkVector4 vTop; vHavokConversionUtils::VisVecToPhysVecLocal(Character_Top, vTop);
  hkVector4 vBottom; vHavokConversionUtils::VisVecToPhysVecLocal(Character_Bottom, vBottom);
  float fRadius = float(VIS2HK_FLOAT_SCALED(Capsule_Radius)); 

  ApplyEntityScale(vTop, vBottom, fRadius);

  if (m_pCharacterProxy && m_pCharacterProxy->getShapePhantom())
  {
    vHavokPhysicsModule* pModule = vHavokPhysicsModule::GetInstance();
    VASSERT(pModule != NULL);

    hkpShapePhantom* phantom = m_pCharacterProxy->getShapePhantom();
    hkpShape* capsule = new hkpCapsuleShape(vTop, vBottom, fRadius);

    pModule->MarkForWrite();
    phantom->setShape(capsule);
    pModule->UnmarkForWrite();

    capsule->removeReference();
  }
}

bool vHavokCharacterController::TryUpdateBoundingVolume(const hkvVec3 &capTop, const hkvVec3 &capBottom)
{
  hkVector4 vTop; vHavokConversionUtils::VisVecToPhysVecLocal(capTop, vTop);
  hkVector4 vBottom; vHavokConversionUtils::VisVecToPhysVecLocal(capBottom, vBottom);
  float fRadius = float(VIS2HK_FLOAT_SCALED(Capsule_Radius));

  ApplyEntityScale(vTop, vBottom, fRadius);

  if (m_pCharacterProxy && m_pCharacterProxy->getShapePhantom())
  {
    vHavokPhysicsModule* pModule = vHavokPhysicsModule::GetInstance();
    VASSERT(pModule != NULL);

    hkpShapePhantom* phantom = m_pCharacterProxy->getShapePhantom();
    hkpShape* capsule = new hkpCapsuleShape(vTop, vBottom, fRadius);

    pModule->MarkForWrite();

    phantom->setShape(capsule);

    struct Collector : hkpCdBodyPairCollector
    {
      virtual void addCdBodyPair(const hkpCdBody& bodyA, const hkpCdBody& bodyB)
      {
        m_earlyOut = true;
        success = false;
      }
      bool success;
    } coll;
    coll.success = true;

    phantom->getPenetrations(coll);

    if (coll.success)
    {
      Character_Top = capTop;
      Character_Bottom = capBottom;
    }
    else
    {
      vHavokConversionUtils::VisVecToPhysVecLocal(Character_Top, vTop);
      vHavokConversionUtils::VisVecToPhysVecLocal(Character_Bottom, vBottom);

      ApplyEntityScale(vTop, vBottom, fRadius);

      capsule->removeReference();
      capsule = new hkpCapsuleShape(vTop, vBottom, fRadius);
      phantom->setShape(capsule);
    }

    pModule->UnmarkForWrite();

    capsule->removeReference();

    return coll.success;
  }
  return false;
}

void vHavokCharacterController::DeleteHavokController()
{
  if (m_pCharacterProxy != NULL)
  {
    vHavokPhysicsModule* pModule = vHavokPhysicsModule::GetInstance();
    VASSERT(pModule != NULL);

    // Remove from world.
    if (m_pCharacterProxy->getShapePhantom()->getWorld() != NULL)
      pModule->RemoveCharacterController(this);

    pModule->MarkForWrite();

    m_pCharacterProxy->getShapePhantom()->removeReference();

    m_pCharacterProxy->removeReference();
    m_pCharacterProxy	= HK_NULL;

    pModule->UnmarkForWrite();
  }

  if (m_pCharacterContext != NULL)
  {
    m_pCharacterContext->removeReference();
    m_pCharacterContext	= HK_NULL;
  }
}


// -------------------------------------------------------------------------- //
// Debug Rendering                                                            //
// -------------------------------------------------------------------------- //

void vHavokCharacterController::SetDebugRendering(BOOL bEnable)
{
  vHavokPhysicsModule* pModule = vHavokPhysicsModule::GetInstance();
  VASSERT(pModule != NULL);

  Debug = bEnable;
  const bool bEnableDebug = Debug || (pModule->m_bDebugRenderCharacterControllers);

  // Get ID (cast from collidable pointer as its is used for display geometry ID)
  pModule->MarkForRead();
  hkUlong id = (hkUlong)m_pCharacterProxy->getShapePhantom()->getCollidable();
  pModule->UnmarkForRead();

  // Ensure debug display handler is created
  if (bEnableDebug)
    pModule->SetEnabledDebug(true);

  // Set display properties
  vHavokDisplayHandler* pDisplay = pModule->GetHavokDisplayHandler();
  if (!pDisplay)
    return;

  // Set debug color and visibility state
  pDisplay->SetVisible(id, bEnableDebug);
  pDisplay->SetColor(id, DebugColor);

  // In Editor attach the Havok Debug Shape to the owner object
  if (bEnableDebug)
    pDisplay->SetOwner(id, (VisObject3D_cl*)GetOwner());
}

void vHavokCharacterController::SetDebugColor(VColorRef color)
{
  vHavokPhysicsModule* pModule = vHavokPhysicsModule::GetInstance();
  VASSERT(pModule != NULL);

  DebugColor = color;

  // Get ID (cast from collidable pointer as its is used for display geometry ID)
  pModule->MarkForRead();
  hkUlong id = (hkUlong)m_pCharacterProxy->getShapePhantom()->getCollidable();
  pModule->UnmarkForRead();

  vHavokDisplayHandler* pDisplay = pModule->GetHavokDisplayHandler();
  if (!pDisplay)
    return;

  // Set debug color
  pDisplay->SetColor(id, DebugColor);
}


void vHavokCharacterController::SetFlyState(bool bIsFlying)
{
  Fly_State = bIsFlying;
  m_characterInput.m_fly = Fly_State;
}

/*static*/ void vHavokCharacterController::GetUpVectorFromGravity(hkVector4& up)
{
  vHavokPhysicsModule* pModule = vHavokPhysicsModule::GetInstance();
  if (pModule == NULL || pModule->GetPhysicsWorld() == NULL)
  {
    up.setZero();
    return;
  }

  hkpWorld* pWorld = pModule->GetPhysicsWorld();

  pWorld->markForRead();
  up = pWorld->getGravity(); 
  pWorld->unmarkForRead();

  up.normalize<3>();
  up.setNeg4(up);
}

// -------------------------------------------------------------------------- //
// Variable Attributes                                                        //
// -------------------------------------------------------------------------- //

#ifdef _VISION_WIN32

void vHavokCharacterController::GetVariableAttributes(VisVariable_cl *pVariable, VVariableAttributeInfo &destInfo)
{
  // Check whether we need to display the debug color property
  if (!strcmp(pVariable->GetName(),"DebugColor"))
  {
    // Get debug display state
    BOOL b;
    GetVariable("Debug")->GetValueDirect(this, (void*) &b, true);

    if (b == false)
      destInfo.m_bHidden = true;
  }
}

#endif


START_VAR_TABLE(vHavokCharacterController,IVObjectComponent,"Can be attached to entities to enable physics processing of character control.", VVARIABLELIST_FLAGS_NONE, "Havok Character Controller" )
  DEFINE_VAR_BOOL_AND_NAME(vHavokCharacterController, m_bEnabled, "Enabled", "Enable or disable component", "True", 0, 0);
  DEFINE_VAR_VECTOR_FLOAT(vHavokCharacterController, Character_Top, "Top of character in local entity space.", "0/0/150", 0, 0); 
  DEFINE_VAR_VECTOR_FLOAT(vHavokCharacterController, Character_Bottom, "Bottom of character in local entity space.", "0/0/30", 0, 0); 
  DEFINE_VAR_FLOAT(vHavokCharacterController, Capsule_Radius, "Radius of the character in local entity space.", "30.0", 0, "Clamp(1e-6,1e6)"); 
  DEFINE_VAR_BOOL(vHavokCharacterController, Debug, "Enables/Disables Physics Debug Rendering.", "FALSE", 0, 0);
  DEFINE_VAR_COLORREF(vHavokCharacterController, DebugColor, "Color of this RB when Debug Rendering is active.", "255,50,50,255", 0, NULL);
  DEFINE_VAR_FLOAT(vHavokCharacterController, Static_Friction, " Controls the static friction for surfaces hit.", "0.0", 0, "Clamp(0,1)");
  DEFINE_VAR_FLOAT(vHavokCharacterController, Dynamic_Friction, "Controls the dynamic friction for surfaces hit. A value of 0 maintains character momentum. A value of 1 clips all momentum against the normal of the surface hit.", "0.8", 0, "Clamp(0,1)");
  DEFINE_VAR_FLOAT(vHavokCharacterController, Max_Slope, "Maximum slope that the character can walk up (In degrees, Max: 90 deg.).", "75", 0, "Clamp(0, 90)");
  DEFINE_VAR_FLOAT(vHavokCharacterController, Character_Mass, "Mass of the character, standing on things.", "100", 0, "Clamp(0,1000)");
  DEFINE_VAR_FLOAT(vHavokCharacterController, Character_Strength, "Maximum force that the character controller can impart onto moving objects.", "5000", 0, "Clamp(0,7.9E+28)");
  DEFINE_VAR_BOOL(vHavokCharacterController, Fly_State, "Whether the character should fly. If set to true the gravity will be ignored and also the Z value of the motion delta is applied", "FALSE", 0, 0);
  DEFINE_VAR_FLOAT(vHavokCharacterController, Gravity_Scale, "Scalar factor to scale the global gravity strength. Use 0.0 for no gravity. Default is 1.0 (normal gravity)", "1.0", 0, "Clamp(0,1e3)"); 
  DEFINE_VAR_FLOAT(vHavokCharacterController, Jump_Height, "Jump Height of the character in meters.", "1.5", 0, "Clamp(0,100)"); 
  DEFINE_VAR_FLOAT(vHavokCharacterController, Step_Height, "Maximum step height that the character can climb in Vision units.", "30.0", 0, 0);
  DEFINE_VAR_FLOAT(vHavokCharacterController, Step_Threshold, "Minimum slope threshold for when to identify steps (In degrees, Max: 90 deg.).", "60.0", 0, "Clamp(0, 90)");
  DEFINE_VAR_FLOAT(vHavokCharacterController, Max_Velocity, "Maximum velocity in Vision units: Used to stabilize character controller movement.", "1500.0", 0, "Clamp(0, 1e12)");
  DEFINE_VAR_FLOAT(vHavokCharacterController, Max_Acceleration, "Maximum acceleration in Vision units: Used to stabilize character controller movement.", "4000.0", 0, "Clamp(0, 1e12)");
  DEFINE_VAR_FLOAT(vHavokCharacterController, PenetrationRecoverySpeed, "Controls how quickly the character controller is pushed away when penetrating collision geometry.", "0.5", 0, "Clamp(0, 10)");
END_VAR_TABLE

/*
 * Havok SDK - Base file, BUILD(#20140628)
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
