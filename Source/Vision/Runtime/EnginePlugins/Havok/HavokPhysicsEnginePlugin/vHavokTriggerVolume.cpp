/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokTriggerVolume.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokShapeFactory.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokShapeCache.hpp>

#include <Physics2012/Utilities/CharacterControl/CharacterProxy/hkpCharacterProxy.h>
#include <Physics2012/Utilities/Collide/TriggerVolume/hkpTriggerVolume.h>
#include <Physics2012/Dynamics/Phantom/hkpShapePhantom.h>

// -------------------------------------------------------------------------- //
// vHavokTriggerVolumeInternal                                                //
// -------------------------------------------------------------------------- //

class vHavokTriggerVolumeInternal: public hkpTriggerVolume
{
public:
  HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE);

  vHavokTriggerVolumeInternal(hkpRigidBody *pTriggerBody, vHavokTriggerVolume *pOwner) : hkpTriggerVolume(pTriggerBody), m_pOwner(pOwner) {}

  // Bring in the other callback we are not overriding
  using hkpTriggerVolume::triggerEventCallback;

  virtual void triggerEventCallback(hkpRigidBody *pBody, EventType type)
  {
    if (type & ENTERED_EVENT)
    {
      vHavokTriggerInfo triggerInfo;
      triggerInfo.m_pTriggerVolume = m_pOwner;
      triggerInfo.m_pTriggerSourceComponent = m_pOwner->m_spOnObjectEnter;
      triggerInfo.m_pObject = pBody;
      vHavokPhysicsModule::GetInstance()->OnVolumeTrigger(triggerInfo);
    }

    if (type & LEFT_EVENT)
    {
      vHavokTriggerInfo triggerInfo;
      triggerInfo.m_pTriggerVolume = m_pOwner;
      triggerInfo.m_pTriggerSourceComponent = m_pOwner->m_spOnObjectLeave;
      triggerInfo.m_pObject = pBody;
      vHavokPhysicsModule::GetInstance()->OnVolumeTrigger(triggerInfo);
    }
  }

  virtual void triggerEventCallback(hkpCharacterProxy *pProxy, EventType type)
  {
    if (type & ENTERED_EVENT)
    {
      vHavokTriggerInfo triggerInfo;
      triggerInfo.m_pTriggerVolume = m_pOwner;
      triggerInfo.m_pTriggerSourceComponent = m_pOwner->m_spOnCharacterEnter;
      triggerInfo.m_pObject = pProxy->getShapePhantom();
      vHavokPhysicsModule::GetInstance()->OnVolumeTrigger(triggerInfo);
    }

    if (type & LEFT_EVENT)
    {
      vHavokTriggerInfo triggerInfo;
      triggerInfo.m_pTriggerVolume = m_pOwner;
      triggerInfo.m_pTriggerSourceComponent = m_pOwner->m_spOnCharacterLeave;
      triggerInfo.m_pObject = pProxy->getShapePhantom();
      vHavokPhysicsModule::GetInstance()->OnVolumeTrigger(triggerInfo);
    }
  }

private:
  vHavokTriggerVolume *m_pOwner;
};

// -------------------------------------------------------------------------- //
// Constructor / Destructor                                                   //
// -------------------------------------------------------------------------- //

V_IMPLEMENT_SERIAL(vHavokTriggerVolume, IVObjectComponent, 0, &g_vHavokModule);

vHavokTriggerVolume::vHavokTriggerVolume(VHavokTriggerVolumeShapeType_e eShapeType)
{
  m_pTriggerVolume = NULL;

  m_spOnObjectEnter = NULL;
  m_spOnObjectLeave = NULL;
  m_spOnCharacterEnter = NULL;
  m_spOnCharacterLeave = NULL;

  Havok_ShapeType = eShapeType;
  Havok_MotionType = VHavokTriggerVolumeMotionType_FIXED; 
  Havok_QualityType = VHavokTriggerVolumeQualityType_FIXED;
  Havok_CollisionLayer = vHavokPhysicsModule::HK_LAYER_COLLIDABLE_STATIC;
  Havok_CollisionGroup = 0;
  Havok_SubSystemId = 0;
  Havok_SubSystemDontCollideWith = 0;

  Debug_Render = FALSE;
  Debug_Color = VColorRef(0, 0, 255, 255);
}

vHavokTriggerVolume::~vHavokTriggerVolume()
{
  CommonDeinit();
}

void vHavokTriggerVolume::DisposeObject()
{
  CommonDeinit();
  IVObjectComponent::DisposeObject();
}

// -------------------------------------------------------------------------- //
// Common Init/ Deint                                                         //
// -------------------------------------------------------------------------- //

void vHavokTriggerVolume::CommonInit()
{
  // Do not initialize the component in case our module is not active
  vHavokPhysicsModule* pModule = vHavokPhysicsModule::GetInstance();
  if (pModule == NULL)
  {
    hkvLog::Warning("Failed to initialize vHavokTriggerVolume since a non Havok physics module is currently active");
    return;
  }

  // Get owner custom volume object
  VCustomVolumeObject *pCustomVolume = GetOwnerCustomVolume();
  VVERIFY_OR_RET(pCustomVolume != NULL); 

  // Get the static mesh from the custom volume object
  VisStaticMesh_cl *pMesh = pCustomVolume->GetStaticMesh();
  if (pMesh == NULL)
    return;
  pMesh->EnsureLoaded();

  if ((!pMesh->IsLoaded()) || (pMesh->GetNumOfTriangles() < 1))
    return;
 
  // Get scaling vector from custom volume object
  const hkvVec3 vScale = pCustomVolume->GetScale();

  // Check whether the static mesh has valid size
  const hkvAlignedBBox& bbox = pMesh->GetBoundingBox();
  hkVector4 bbox_min; vHavokConversionUtils::VisVecToPhysVec_noscale(bbox.m_vMin, bbox_min);
  hkVector4 bbox_max; vHavokConversionUtils::VisVecToPhysVec_noscale(bbox.m_vMax, bbox_max);
  hkVector4 bbox_scale; vHavokConversionUtils::VisVecToPhysVec_noscale(vScale, bbox_scale);

  bbox_scale.mul(vHavokConversionUtils::GetVision2HavokScaleSIMD());

  hkVector4 bbox_extent; bbox_extent.setSub(bbox_max,bbox_min); bbox_extent.mul(bbox_scale);

  hkVector4 meshTol; meshTol.setAll(hkReal((Havok_ShapeType==VHavokTriggerVolumeShapeType_CONVEX) ? HKVIS_CONVEX_SHAPE_TOLERANCE : HKVIS_MESH_SHAPE_TOLERANCE));
  hkVector4Comparison notLargeEnough = bbox_extent.less(meshTol);
  if (notLargeEnough.anyIsSet<hkVector4ComparisonMask::MASK_XYZ>())
  {
    const char *szMeshFilename = (pMesh->GetFilename()!=NULL) ? pMesh->GetFilename() : "Unnamed";
    hkvLog::Warning("Initializing vHavokTriggerVolume with a static mesh [%s] with undersized extents (%.4f, %4f, %.4f)", 
      szMeshFilename, bbox_extent(0), bbox_extent(1), bbox_extent(2));
    return;
  }
  
  // Create Havok trigger volume 
  const unsigned int uiCreationFlags = vHavokShapeFactory::VShapeCreationFlags_CACHE_SHAPE | vHavokShapeFactory::VShapeCreationFlags_USE_VCOLMESH;
  bool bResult = CreateHkTriggerVolume(pMesh, vScale, uiCreationFlags);
  VASSERT_MSG(bResult, "Failed to create Havok Trigger Volume");

  // Set debug visibility 
  SetDebugRendering(Debug_Render);

  // Note, that the trigger components are only created here inside vForge. Serialization reads these components from the archive.
  if (!m_spOnObjectEnter)
  {
    m_spOnObjectEnter = new VisTriggerSourceComponent_cl(VHAVOKTRIGGERVOLUME_ONOBJECTENTER, VIS_OBJECTCOMPONENTFLAG_NONE);
    AddComponent(m_spOnObjectEnter);
  }
  if (!m_spOnObjectLeave)
  {
    m_spOnObjectLeave = new VisTriggerSourceComponent_cl(VHAVOKTRIGGERVOLUME_ONOBJECTLEAVE, VIS_OBJECTCOMPONENTFLAG_NONE);
    AddComponent(m_spOnObjectLeave);
  }
  if (!m_spOnCharacterEnter)
  {
    m_spOnCharacterEnter = new VisTriggerSourceComponent_cl(VHAVOKTRIGGERVOLUME_ONCHARACTERENTER, VIS_OBJECTCOMPONENTFLAG_NONE);
    AddComponent(m_spOnCharacterEnter);
  }
  if (!m_spOnCharacterLeave)
  {
    m_spOnCharacterLeave = new VisTriggerSourceComponent_cl(VHAVOKTRIGGERVOLUME_ONCHARACTERLEAVE, VIS_OBJECTCOMPONENTFLAG_NONE);
    AddComponent(m_spOnCharacterLeave);
  }
}

void vHavokTriggerVolume::CommonDeinit()
{
  RemoveHkTriggerVolume();
  m_spOnObjectEnter = NULL;
  m_spOnObjectLeave = NULL;
  m_spOnCharacterEnter = NULL;
  m_spOnCharacterLeave = NULL;
}

bool vHavokTriggerVolume::CreateHkTriggerVolume(VisStaticMesh_cl* pMesh, const hkvVec3& vScale, unsigned int uiCreationFlags)
{
  vHavokPhysicsModule* pModule = vHavokPhysicsModule::GetInstance();
  VVERIFY_OR_RET_VAL(pModule != NULL  && pMesh != NULL, false);

  // Remove the old instance, if available
  if (m_pTriggerVolume != NULL)
  {
    ASSERT(m_pOwner); /// If there's no owner then the refcount will get 0 => crash
    RemoveHkTriggerVolume();
  }

  hkpRigidBodyCinfo cInfo;

  // Set motion type
  switch (Havok_MotionType)
  {
  case VHavokTriggerVolumeMotionType_FIXED:
    cInfo.m_motionType = hkpMotion::MOTION_FIXED;
    cInfo.m_qualityType = HK_COLLIDABLE_QUALITY_FIXED;
    break;

  case VHavokTriggerVolumeMotionType_KEYFRAMED: 
    cInfo.m_motionType = hkpMotion::MOTION_KEYFRAMED;
    cInfo.m_qualityType = HK_COLLIDABLE_QUALITY_KEYFRAMED;
    break;

  default: 
    {
      VASSERT_MSG(false, "Unsupported motion type!");
      return false;
    }
  }

  // OVERWRITES qualtiy type (See quality type settings done in the paragraph before this line)
  // Only do that for non motion fixed trigger volumes
  if (Havok_MotionType != VHavokTriggerVolumeMotionType_FIXED)
  {
    switch (Havok_QualityType)
    {
    case VHavokTriggerVolumeQualityType_AUTO:
      {
        // Nothing, as this property only overwrites automatic done settings one paragraph earlier
      }
      break;

    case VHavokTriggerVolumeQualityType_FIXED:
      cInfo.m_qualityType = HK_COLLIDABLE_QUALITY_FIXED;
      break;

    case VHavokTriggerVolumeQualityType_KEYFRAMED:
      cInfo.m_qualityType = HK_COLLIDABLE_QUALITY_KEYFRAMED;
      break;

    case VHavokTriggerVolumeQualityType_KEYFRAMED_REPORTING:
      cInfo.m_qualityType = HK_COLLIDABLE_QUALITY_KEYFRAMED_REPORTING;
      break;

    default: 
      {
        VASSERT_MSG(false, "Unsupported quality type!");
        return false;
      }
    }
  }

  hkRefPtr<hkpShape> spShape = vHavokShapeFactory::CreateShapeFromMesh(pMesh, vScale, 
    uiCreationFlags | ((Havok_ShapeType == VHavokTriggerVolumeShapeType_CONVEX) ? vHavokShapeFactory::VShapeCreationFlags_CONVEX : vHavokShapeFactory::VShapeCreationFlags_MESH));
  cInfo.m_shape = spShape;
  VASSERT_MSG(cInfo.m_shape != NULL, "Failed to create shape for Havok Trigger Volume");

  // no offset applied
  cInfo.m_position.setZero();

  // calculate collision filter
  cInfo.m_collisionFilterInfo = hkpGroupFilter::calcFilterInfo(Havok_CollisionLayer, Havok_CollisionGroup, 
    Havok_SubSystemId, Havok_SubSystemDontCollideWith);

  // Create rigid body 
  hkpRigidBody *pRigidBody = new hkpRigidBody(cInfo);

  // Convert rigid body into trigger volume  
  m_pTriggerVolume = new vHavokTriggerVolumeInternal(pRigidBody, this);

  // If we already have an owner, then add the trigger volume to the Havok world
  if (GetOwner() != NULL)
  {
    UpdateVision2Havok();
    pModule->AddTriggerVolume(this);
  }
  return true;
}

void vHavokTriggerVolume::RemoveHkTriggerVolume()
{
  if (m_pTriggerVolume == NULL)
    return;

  vHavokPhysicsModule* pModule = vHavokPhysicsModule::GetInstance();

  // If we still have an owner, then remove the trigger volume from the Havok world.
  if (GetOwner() != NULL)
    pModule->RemoveTriggerVolume(this);

  // Remove shape from cache
  m_pTriggerVolume->getTriggerBody()->markForRead();
  const hkpShape* pShape = m_pTriggerVolume->getTriggerBody()->getCollidable()->getShape();
  m_pTriggerVolume->getTriggerBody()->unmarkForRead();

  m_pTriggerVolume->getTriggerBody()->removeReference();
  m_pTriggerVolume->removeReference();
  m_pTriggerVolume = NULL;

  vHavokShapeCache::RemoveShape(pShape);
}

void vHavokTriggerVolume::UpdateVision2Havok()
{
  VisObject3D_cl *pOwner3d = GetOwner3D();
  vHavokPhysicsModule* pModule = vHavokPhysicsModule::GetInstance();
  VVERIFY_OR_RET(m_pTriggerVolume != NULL && pOwner3d != NULL && pModule != NULL);

  // Get the transformation from Vision
  const hkvVec3& vPos = pOwner3d->GetPosition();
  const hkvMat3& vRot = pOwner3d->GetRotationMatrix();

  // Set the transformation in Havok
  hkTransform hkTfOut;
  vHavokConversionUtils::VisMatVecToPhysTransformWorld(vRot, vPos, hkTfOut);

  pModule->MarkForWrite();
  m_pTriggerVolume->getTriggerBody()->setTransform(hkTfOut); 
  pModule->UnmarkForWrite();
}

// -------------------------------------------------------------------------- //
// Property Functions                                                         //
// -------------------------------------------------------------------------- //

void vHavokTriggerVolume::SetPosition(const hkvVec3& value)
{
  vHavokPhysicsModule* pModule = vHavokPhysicsModule::GetInstance();
  VVERIFY_OR_RET(pModule != NULL && m_pTriggerVolume != NULL);

  hkVector4 pos; vHavokConversionUtils::VisVecToPhysVecWorld(value,pos);
  pModule->MarkForWrite();
  m_pTriggerVolume->getTriggerBody()->setPosition(pos);
  pModule->UnmarkForWrite();
}

hkvVec3 vHavokTriggerVolume::GetPosition() const
{
  vHavokPhysicsModule* pModule = vHavokPhysicsModule::GetInstance();
  VVERIFY_OR_RET_VAL(pModule != NULL && m_pTriggerVolume != NULL, hkvVec3(hkvNoInitialization));

  hkvVec3 v; 
  pModule->MarkForRead();
  const hkVector4& pos = m_pTriggerVolume->getTriggerBody()->getPosition();   
  vHavokConversionUtils::PhysVecToVisVecWorld(pos,v);
  pModule->UnmarkForRead();
  return v;
}

void vHavokTriggerVolume::SetRotation(const hkvMat3& value)
{
  vHavokPhysicsModule* pModule = vHavokPhysicsModule::GetInstance();
  VVERIFY_OR_RET(pModule != NULL && m_pTriggerVolume != NULL);

  hkQuaternion hkQuatOut;
  vHavokConversionUtils::VisMatrixToHkQuat(value, hkQuatOut);

  pModule->MarkForWrite();
  m_pTriggerVolume->getTriggerBody()->setRotation(hkQuatOut);
  pModule->UnmarkForWrite();
}

hkvMat3 vHavokTriggerVolume::GetRotation() const 
{
  vHavokPhysicsModule* pModule = vHavokPhysicsModule::GetInstance();
  VVERIFY_OR_RET_VAL(pModule != NULL && m_pTriggerVolume != NULL, hkvMat3::IdentityMatrix());

  hkvMat3 visMatrixOut;
  pModule->MarkForRead();
  const hkQuaternion& rot = m_pTriggerVolume->getTriggerBody()->getRotation();
  vHavokConversionUtils::HkQuatToVisMatrix(rot, visMatrixOut);
  pModule->UnmarkForRead();

  return visMatrixOut;
}

void vHavokTriggerVolume::SetMotionType(VHavokTriggerVolumeMotionType_e eMotionType)
{
  vHavokPhysicsModule* pModule = vHavokPhysicsModule::GetInstance();
  VVERIFY_OR_RET(pModule != NULL && m_pTriggerVolume != NULL);

  //initialize with default value
  hkpMotion::MotionType eHKMotionType = hkpMotion::MOTION_FIXED; 
 
  switch (eMotionType)
  {
  case VHavokTriggerVolumeMotionType_FIXED: 
    eHKMotionType = hkpMotion::MOTION_FIXED;
    break;

  case VHavokTriggerVolumeMotionType_KEYFRAMED: 
    eHKMotionType = hkpMotion::MOTION_KEYFRAMED;
    break;

  default:
    VASSERT_MSG(false, "Unsupported motion type!");
    return; 
  }

  pModule->MarkForWrite();   
  m_pTriggerVolume->getTriggerBody()->setMotionType(eHKMotionType);
  pModule->UnmarkForWrite();
  
  Havok_MotionType = eMotionType;
}

void vHavokTriggerVolume::SetQualityType(VHavokTriggerVolumeQualityType_e eQualityType)
{
  vHavokPhysicsModule* pModule = vHavokPhysicsModule::GetInstance();
  VVERIFY_OR_RET(pModule != NULL && m_pTriggerVolume != NULL);

  //initialize with default value
  hkpCollidableQualityType eHKQualityType = HK_COLLIDABLE_QUALITY_FIXED;

  switch (eQualityType)
  {
  case VHavokTriggerVolumeQualityType_AUTO:
    if (Havok_MotionType == VHavokTriggerVolumeQualityType_FIXED)
      eHKQualityType= HK_COLLIDABLE_QUALITY_FIXED;
    else
      eHKQualityType = HK_COLLIDABLE_QUALITY_KEYFRAMED;
    break;

  case VHavokTriggerVolumeQualityType_FIXED: 
    eHKQualityType = HK_COLLIDABLE_QUALITY_FIXED;
    break;

  case VHavokTriggerVolumeQualityType_KEYFRAMED: 
    eHKQualityType = HK_COLLIDABLE_QUALITY_KEYFRAMED;
    break;

  case VHavokTriggerVolumeQualityType_KEYFRAMED_REPORTING:
    eHKQualityType = HK_COLLIDABLE_QUALITY_KEYFRAMED_REPORTING;
    break;

  default:
    VASSERT_MSG(false, "Unsupported quality type!");
    return; 
  }

  pModule->MarkForWrite();   
  m_pTriggerVolume->getTriggerBody()->setQualityType(eHKQualityType);
  pModule->UnmarkForWrite();

  Havok_QualityType = eQualityType;
}

void vHavokTriggerVolume::SetCollisionInfo(int iLayer, int iGroup, int iSubsystem, int iSubsystemDontCollideWith)
{
  vHavokPhysicsModule* pModule = vHavokPhysicsModule::GetInstance();
  VVERIFY_OR_RET(pModule != NULL && m_pTriggerVolume != NULL);

  int iFilterInfo = hkpGroupFilter::calcFilterInfo(iLayer, iGroup, iSubsystem, iSubsystemDontCollideWith);

  pModule->MarkForWrite();
  hkpRigidBody *pRigidBody = m_pTriggerVolume->getTriggerBody();
  pRigidBody->setCollisionFilterInfo(iFilterInfo);
  hkpWorld* world = pRigidBody->getWorld();
  if (world)
    world->updateCollisionFilterOnEntity(pRigidBody, HK_UPDATE_FILTER_ON_ENTITY_FULL_CHECK, HK_UPDATE_COLLECTION_FILTER_IGNORE_SHAPE_COLLECTIONS);
  pModule->UnmarkForWrite();

  Havok_CollisionLayer = iLayer;
  Havok_CollisionGroup = iGroup;
  Havok_SubSystemId = iSubsystem;
  Havok_SubSystemDontCollideWith = iSubsystemDontCollideWith;
}

// -------------------------------------------------------------------------- //
// Access to Havok Internals                                                  //
// -------------------------------------------------------------------------- //

hkpRigidBody* vHavokTriggerVolume::GetHkTriggerBody()const
{ 
  VASSERT(m_pTriggerVolume != NULL);
  return m_pTriggerVolume->getTriggerBody(); 
} 

// -------------------------------------------------------------------------- //
// Debug Rendering                                                            //
// -------------------------------------------------------------------------- //

void vHavokTriggerVolume::SetDebugRendering(BOOL bEnable)
{
  Debug_Render = bEnable;

  if (m_pTriggerVolume == NULL)
    return;

  vHavokPhysicsModule* pInstance = vHavokPhysicsModule::GetInstance();

  const bool bEnableDebug = Debug_Render || (pInstance ? pInstance->m_bDebugRenderTriggerVolumes : false);

  // Get ID (cast from collidable pointer as its is used for display geometry ID)
  hkpWorld* world = pInstance ? pInstance->GetPhysicsWorld() : HK_NULL;
  if (world) 
    world->markForRead();
  hkUlong id = (hkUlong)m_pTriggerVolume->getTriggerBody()->getCollidable();
  if (world) 
    world->unmarkForRead();

  // Ensure debug display handler is created
  if (bEnableDebug)
    ((vHavokPhysicsModule*) Vision::GetApplication()->GetPhysicsModule())->SetEnabledDebug(true);

  // Set display properties
  vHavokDisplayHandler* pDisplay = ((vHavokPhysicsModule*) Vision::GetApplication()->GetPhysicsModule())->GetHavokDisplayHandler();
  if (!pDisplay)
    return;

  // Set debug color and visibility state
  pDisplay->SetVisible(id, bEnableDebug);
  pDisplay->SetColor(id, Debug_Color);

  // In Editor attach the Havok Debug Shape to the owner object
  if (bEnableDebug)
    pDisplay->SetOwner(id, GetOwner3D());
}

void vHavokTriggerVolume::SetDebugColor(VColorRef color)
{
  Debug_Color = color;

  if (m_pTriggerVolume == NULL)
    return;

  // Get ID (cast from collidable pointer as its is used for display geometry ID)
  hkpWorld* world = vHavokPhysicsModule::GetInstance()? vHavokPhysicsModule::GetInstance()->GetPhysicsWorld() : HK_NULL;
  if (world) 
    world->markForRead();
  hkUlong id = (hkUlong)m_pTriggerVolume->getTriggerBody()->getCollidable();
  if (world)
    world->unmarkForRead();

  vHavokDisplayHandler* pDisplay = ((vHavokPhysicsModule*) Vision::GetApplication()->GetPhysicsModule())->GetHavokDisplayHandler();
  if (!pDisplay)
    return;

  // Set debug color
  pDisplay->SetColor(id, Debug_Color);
}

// -------------------------------------------------------------------------- //
// IVObjectComponent Virtual Overrides                                        //
// -------------------------------------------------------------------------- //

BOOL vHavokTriggerVolume::CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut)
{
  if (!IVObjectComponent::CanAttachToObject(pObject, sErrorMsgOut))
    return FALSE;

  // Object has to be derived from VCustomVolumeObject 
  if (vdynamic_cast<VCustomVolumeObject*>(pObject) == NULL)
  {
    sErrorMsgOut = "Component can only be added to instances of VCustomVolumeObject or derived classes.";
    return FALSE;
  }

  return TRUE;
}

void vHavokTriggerVolume::SetOwner(VisTypedEngineObject_cl *pOwner)
{
  // Do not initialize the component in case our module is not active
  vHavokPhysicsModule* pModule = vHavokPhysicsModule::GetInstance();
  if (pModule == NULL)
  {
    hkvLog::Warning("Failed to initialize vHavokTriggerVolume since a non Havok physics module is currently active");
    return;
  }

  // Add or remove from manager according to whether we have an owner or not
  if (pOwner)
  {
    IVObjectComponent::SetOwner(pOwner);

    // Use matrix directly instead of euler angles when doing real physics, as Euler
    // angles are slower and less accurate. Before, however, ensure that the object's
    // rotation matrix is current.
    GetOwner3D()->EnsureCachedRotationMatrixValid();
    GetOwner3D()->SetUseEulerAngles(false); 

    // Add the trigger volume to the Havok physics world
    if (m_pTriggerVolume)
    {
      // Set initial object position in Havok
      UpdateVision2Havok();

      pModule->AddTriggerVolume(this);
    }
    else
    {
      CommonInit();
    }
  }
  else
  {
    // Remove the trigger volume from the Havok physics world
    if (m_pTriggerVolume)
      pModule->RemoveTriggerVolume(this);

    IVObjectComponent::SetOwner(NULL);
  }
}

void vHavokTriggerVolume::OnVariableValueChanged(VisVariable_cl *pVar, const char * value)
{
  // Do not touch the trigger volume in case our module is not active
  vHavokPhysicsModule* pModule = vHavokPhysicsModule::GetInstance();
  if (pModule == NULL)
    return;

  if (m_pTriggerVolume == NULL)
    return;

  CommonInit();
}

void vHavokTriggerVolume::MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB)
{
  IVObjectComponent::MessageFunction(iID,iParamA,iParamB);

  // Do not touch the trigger volume in case our module is not active
  vHavokPhysicsModule* pModule = vHavokPhysicsModule::GetInstance();
  if (pModule == NULL)
    return;

  if (iID == VIS_MSG_EDITOR_PROPERTYCHANGED)
  {
    const char *szPropertyName = (const char *) iParamA;

    // Recreate the trigger volume in case the mesh gets changed within vForge
    if (_stricmp(szPropertyName, "Scaling") == 0 ||  _stricmp(szPropertyName, "VolumeGeometry") == 0 ||
        _stricmp(szPropertyName, "CustomStaticMesh") == 0 ||  _stricmp(szPropertyName, "StaticMeshPath") == 0)
    {
      CommonInit();
    }

    // Reposition the trigger volume in case the mesh gets positioned/ orientated within vForge
    if (m_pTriggerVolume)
    {
      if (_stricmp(szPropertyName, "Position") == 0 || _stricmp(szPropertyName, "Orientation") == 0)
        UpdateVision2Havok();
    }
  
    return;
  }
}

// -------------------------------------------------------------------------- //
// Serialization                                                              //
// -------------------------------------------------------------------------- //

void vHavokTriggerVolume::Serialize( VArchive &ar )
{
  char iLocalVersion = VHAVOKTRIGGERVOLUME_VERSION_CURRENT;
  IVObjectComponent::Serialize(ar);
  if (ar.IsLoading())
  {
    char iFileVersion = 0;
    ar >> iFileVersion;
    VASSERT_MSG(iFileVersion <= iLocalVersion, "Invalid file version. Please re-export");
  
    ar >> Havok_ShapeType;
    ar >> Havok_MotionType;
    ar >> Havok_QualityType;
    ar >> Havok_CollisionLayer;
    ar >> Havok_CollisionGroup;
    ar >> Havok_SubSystemId;
    ar >> Havok_SubSystemDontCollideWith;
    ar >> Debug_Color;

    VisTriggerSourceComponent_cl *pComp = NULL;
    ar >> pComp;m_spOnObjectEnter = pComp;
    ar >> pComp;m_spOnObjectLeave = pComp;
    ar >> pComp;m_spOnCharacterEnter = pComp;
    ar >> pComp;m_spOnCharacterLeave = pComp;
  } 
  else
  {
    ar << iLocalVersion;

    ar << Havok_ShapeType;
    ar << Havok_MotionType; 
    ar << Havok_QualityType;
    ar << Havok_CollisionLayer;
    ar << Havok_CollisionGroup;
    ar << Havok_SubSystemId;
    ar << Havok_SubSystemDontCollideWith;
    ar << Debug_Color;

    ar << m_spOnObjectEnter;
    ar << m_spOnObjectLeave;
    ar << m_spOnCharacterEnter;
    ar << m_spOnCharacterLeave;
  }
}

void vHavokTriggerVolume::OnDeserializationCallback(const VSerializationContext &context)
{
  // We can finally create the trigger volume now, since the owner entity is guaranteed to be deserialized completely.
  if (!m_pTriggerVolume)
    CommonInit();
}

// -------------------------------------------------------------------------- //
// Variable Attributes                                                        //
// -------------------------------------------------------------------------- //

START_VAR_TABLE(vHavokTriggerVolume, IVObjectComponent, "Havok Trigger Volume Component", VVARIABLELIST_FLAGS_NONE, "Havok Trigger Volume" )
  DEFINE_VAR_ENUM     (vHavokTriggerVolume, Havok_ShapeType, "Geometry used for Physics. Note that if 'Mesh' is selected, objects are only detected as being 'inside' the volume when touching the faces of the mesh.", "Convex Hull","Convex Hull/Mesh", 0, 0);
  DEFINE_VAR_ENUM     (vHavokTriggerVolume, Havok_MotionType, "Type of Physics:\nFixed - Detects collisions, but doesn't move\nKeyframed - Can be moved", "Fixed", "Fixed/Keyframed", 0, 0);
  DEFINE_VAR_ENUM     (vHavokTriggerVolume, Havok_QualityType,
    "Quality type of collidable. Does not affect fixed trigger volumes!\n"
    "Auto - Default, automatic assignment by motion type\n"
    "Fixed - Use this for fixed trigger volumes.\n"
    "Keyframed - Use this for keyframed trigger volumes.\n"
    "Keyframed_Reporting - Use this for keyframed trigger volumes to additionally detect trigger events from static and other keyframed objects.", 
    "Auto", "Auto/Fixed/Keyframed/Keyframed_Reporting", 0, 0);
  DEFINE_VAR_INT      (vHavokTriggerVolume, Havok_CollisionLayer, "Defines the collision layer this Trigger Volume is assigned to.\n"COLLISION_LAYER_NUMBER_VARTABLE_DESCRIPTION, "2", 0, "Clamp(0, 31)");
  DEFINE_VAR_INT      (vHavokTriggerVolume, Havok_CollisionGroup, "Defines the collision group this Trigger Volume is assigned to.", "0", 0, "Clamp(0, 65535)");
  DEFINE_VAR_INT      (vHavokTriggerVolume, Havok_SubSystemId, "Defines the sub system ID of this Trigger Volume.", "0", 0, "Clamp(0, 31)");
  DEFINE_VAR_INT      (vHavokTriggerVolume, Havok_SubSystemDontCollideWith, "Defines the sub system ID this Trigger Volume should not collide with.", "0", 0, "Clamp(0, 31)");
  DEFINE_VAR_BOOL     (vHavokTriggerVolume, Debug_Render, "Enables/Disables Physics Debug Rendering.", "FALSE", 0, 0);
  DEFINE_VAR_COLORREF (vHavokTriggerVolume, Debug_Color, "Color of this Trigger Volume when Debug Rendering is active.", "0,0,255,255", 0, NULL);
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
