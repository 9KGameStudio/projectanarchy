/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokStaticMesh.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokUserData.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokShapeFactory.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokShapeCache.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>

#include <Common/Base/Reflection/Registry/hkVtableClassRegistry.h>

// --------------------------------------------------------------------------
// Constructor / Destructor
// --------------------------------------------------------------------------

void vHavokStaticMesh::SetDebugRendering (bool bEnable)
{
  if(m_pRigidBody == NULL)
    return;

  vHavokPhysicsModule* pInstance = vHavokPhysicsModule::GetInstance();

  // Get ID (cast from collidable pointer as its is used for display geometry ID)
  hkpWorld* world = pInstance ? pInstance->GetPhysicsWorld() : HK_NULL;
  if (world) world->markForRead();

  const bool bEnabledDebug = bEnable || (pInstance ? pInstance->m_bDebugRenderStaticMeshes : false);

  hkUlong id = (hkUlong)m_pRigidBody->getCollidable();

  if (world) world->unmarkForRead();

  // Ensure debug display handler is created
  if (bEnabledDebug)
    ((vHavokPhysicsModule*) Vision::GetApplication()->GetPhysicsModule())->SetEnabledDebug(true);

  // Set display properties
  vHavokDisplayHandler* pDisplay = ((vHavokPhysicsModule*) Vision::GetApplication()->GetPhysicsModule())->GetHavokDisplayHandler();
  if (!pDisplay)
    return;

  // Set debug color and visibility state
  pDisplay->SetVisible(id, bEnabledDebug);
  pDisplay->SetColor(id, V_RGBA_PURPLE);
}


vHavokStaticMesh::vHavokStaticMesh()
  : m_pRigidBody(NULL)
  , m_staticMeshes()
  , m_iNumValidStaticMeshes(0)
  , m_vScale()
{
}

vHavokStaticMesh::~vHavokStaticMesh()
{
  CommonDeinit();
}

void vHavokStaticMesh::DisposeObject()
{
  CommonDeinit();
}

void vHavokStaticMesh::CommonDeinit()
{
  RemoveHkRigidBody();

  // Remove the link to this rigid body from the static meshes
  int iSMCount = m_staticMeshes.GetLength();
  for (int i = 0 ; i < iSMCount; i++)
  {
    if (m_staticMeshes[i])
	    m_staticMeshes[i]->SetPhysicsObject(NULL);
  }

  m_iNumValidStaticMeshes = 0;
}

// --------------------------------------------------------------------------
// Initialization
// --------------------------------------------------------------------------

void vHavokStaticMesh::Init(VisStaticMeshInstance_cl &meshInstance)
{
  if (m_pRigidBody != HK_NULL)
    return;

  // Add the static mesh instance
  m_staticMeshes.Append(&meshInstance);
  meshInstance.SetPhysicsObject(this);
  m_iNumValidStaticMeshes = 1;

  // Build the Havok shape
  CreateHkRigidBody();
}

void vHavokStaticMesh::Init(VisStaticMeshInstCollection &meshInstances)
{
  if (m_pRigidBody != HK_NULL)
    return;

  // Add the static mesh instances
  int iCount = meshInstances.GetLength();
  for (int i = 0; i < iCount; i++)
  {
    m_staticMeshes.Append(meshInstances[i]);
    meshInstances[i]->SetPhysicsObject(this);
    m_iNumValidStaticMeshes++;
  }

  CreateHkRigidBody();
}

int vHavokStaticMesh::GetNumValidStaticMeshes() const
{
  return m_iNumValidStaticMeshes;
}

int vHavokStaticMesh::GetNumWrappedStaticMeshes()
{
  return m_staticMeshes.GetLength();
}

VisStaticMeshInstance_cl* vHavokStaticMesh::GetWrappedStaticMesh(int index)
{
  return m_staticMeshes[index];
}

void vHavokStaticMesh::RemoveStaticMesh(VisStaticMeshInstance_cl *pMesh)
{
  VVERIFY_OR_RET(pMesh != NULL);

  for (int i = 0; i < m_staticMeshes.GetLength(); ++i)
  {
    if (m_staticMeshes[i] != pMesh)
      continue;

    m_staticMeshes[i]->SetPhysicsObject(NULL);
    m_staticMeshes[i] = NULL;

    m_iNumValidStaticMeshes--;
    VASSERT(m_iNumValidStaticMeshes >= 0);

    break;
  }

  // Get rid of the Havok rigid body if there are no more meshes left.
  if (m_iNumValidStaticMeshes == 0) 
    RemoveHkRigidBody();
}

// --------------------------------------------------------------------------
// Havok Rigid Body Creation
// --------------------------------------------------------------------------

void vHavokStaticMesh::CreateHkRigidBody()
{
  VASSERT(m_pRigidBody == HK_NULL);

  vHavokPhysicsModule* pModule = vHavokPhysicsModule::GetInstance();
  VASSERT(pModule != NULL);

  // Create the Havok shape
  hkpRigidBodyCinfo cInfo;
  cInfo.m_motionType = hkpMotion::MOTION_FIXED;

  // We use the first static mesh instance as origin reference
  VASSERT(!m_staticMeshes.IsEmpty());
  VisStaticMeshInstance_cl *pMeshInstance = m_staticMeshes[0];

  cInfo.m_collisionFilterInfo = pMeshInstance->GetCollisionBitmask();
  if (cInfo.m_collisionFilterInfo & (1<<15))
  {
    cInfo.m_collisionFilterInfo &= ~(1<<15);
    hkvLog::Warning("vHavok: Static mesh [%s] has outdated collision information. Please reexport scene.", pMeshInstance->GetMesh()->GetFilename());
  }
  
  hkRefPtr<hkpShape> spShape = CreateHkShape();
  if (spShape == NULL)
    return;

  cInfo.m_shape = spShape;

  // When CollisionBehavior_e::FromFile was selected and there is no collisionFilterInfo available from file (due to old vcolmesh format, convex shape),
  // a default collisionFilterInfo will be used.
  if (cInfo.m_collisionFilterInfo==0)
  {
    bool bHasPerPrimitiveCollisionFilter = false;
    const hkpShape *pShape = (hkpShape*)spShape.val();
    const hkClass *pClass = pShape->getClassType();
    if (pClass == &hkvBvCompressedMeshShapeClass)
    {
      const hkvBvCompressedMeshShape *pMeshShape = (hkvBvCompressedMeshShape*)(pShape);
      bHasPerPrimitiveCollisionFilter = (pMeshShape->getCollisionFilterInfoMode() != hkvBvCompressedMeshShape::PER_PRIMITIVE_DATA_NONE);
    }
    if (!bHasPerPrimitiveCollisionFilter)
      cInfo.m_collisionFilterInfo = hkpGroupFilter::calcFilterInfo(vHavokPhysicsModule::HK_LAYER_COLLIDABLE_STATIC, 0, 0, 0);
  }
  cInfo.m_numShapeKeysInContactPointProperties = -1;  // Ensure shape keys are stored.

  m_pRigidBody = new hkpRigidBody(cInfo);

  // Set user data to identify this static mesh during collision detection
  m_pRigidBody->setUserData((hkUlong)vHavokUserDataPointerPair_t::CombineTypeAndPointer(this, V_USERDATA_STATIC));

  // Add our instance to the module
  UpdateVision2Havok();
  pModule->AddStaticMesh(this);
}

void vHavokStaticMesh::RemoveHkRigidBody()
{
  if(m_pRigidBody == NULL)
    return;

  vHavokPhysicsModule* pModule = vHavokPhysicsModule::GetInstance();
  VASSERT(pModule != NULL);

  // Keep our object alive
  VSmartPtr<vHavokStaticMesh> keepAlive = this;

  // If we still have an owner, then remove the rigid body from the Havok world
  pModule->RemoveStaticMesh(this); 
  
  // Free the rigid body 
  m_pRigidBody->markForRead();
  const hkpShape* pShape = m_pRigidBody->getCollidable()->getShape();
  m_pRigidBody->unmarkForRead();

  m_pRigidBody->removeReference();
  m_pRigidBody = NULL;

  // Remove shape from cache.
  vHavokShapeCache::RemoveShape(pShape);
}

hkRefNew<hkpShape> vHavokStaticMesh::CreateHkShape()
{
  VASSERT(!m_staticMeshes.IsEmpty());
  VisStaticMeshInstance_cl *pMeshInstance = m_staticMeshes[0];

  // Get the scaling of the first static mesh which is used as a reference
  {
    hkMatrix4 mTransform;
    vHavokConversionUtils::VisMatrixToHkMatrix(pMeshInstance->GetTransform(), mTransform, false, false, true);
    hkVector4 vScale;
    vScale.set(mTransform.getColumn<0>().lengthSquared<3>(),
      mTransform.getColumn<1>().lengthSquared<3>(),
      mTransform.getColumn<2>().lengthSquared<3>(),
      hkSimdReal_1);
    vScale.setSqrt(vScale);
    vHavokConversionUtils::PhysVecToVisVec_noscale(vScale, m_vScale);
  }

  return vHavokShapeFactory::CreateShapeFromStaticMeshInstances(m_staticMeshes, 
    vHavokShapeFactory::VShapeCreationFlags_CACHE_SHAPE | vHavokShapeFactory::VShapeCreationFlags_USE_VCOLMESH);
}

// --------------------------------------------------------------------------
// Synchronization
// --------------------------------------------------------------------------

void vHavokStaticMesh::UpdateVision2Havok()
{
  VVERIFY_OR_RET(m_staticMeshes.GetLength() >= 1);

  // We use the first static mesh instance as origin reference
  VisStaticMeshInstance_cl *pMeshInstance = m_staticMeshes[0];

  // Get the static mesh instance position and rotation
  hkMatrix4 mTransform;
  vHavokConversionUtils::VisMatrixToHkMatrix(pMeshInstance->GetTransform(), mTransform, false, false, true);

  // Split the rotation into scale and normal matrix
  hkRotation mRotation; 
  mRotation.setCols(mTransform.getColumn<0>(),mTransform.getColumn<1>(),mTransform.getColumn<2>());
  hkVector4 vScale;
  vScale.set(mRotation.getColumn<0>().normalizeWithLength<3>(),
           mRotation.getColumn<1>().normalizeWithLength<3>(),
           mRotation.getColumn<2>().normalizeWithLength<3>(),
           hkSimdReal_1);
  
  bool bUpdateDebugRendering = false;

  // Check here if we need to recalculate the precomputed collision mesh
  // should only happen inside the editor.
  hkVector4 mvScale; vHavokConversionUtils::VisVecToPhysVec_noscale(m_vScale, mvScale);
  if (!vScale.allEqual<3>(mvScale, hkSimdReal::fromFloat(HKVMATH_LARGE_EPSILON)))
  {
    // Keep our object alive
    VSmartPtr<vHavokStaticMesh> keepAlive = this;

    RemoveHkRigidBody(); //Remove the old collision object
    CreateHkRigidBody(); //Create a new one (because the scale changed) 

    // Since vHavokStaticMesh object will be deleted via VSmartPtr when creation fails, m_pRigidBody
    // will be then be invalid. Therefore it is necessary to return at this point.
    if (m_pRigidBody == NULL)
      return;

    vHavokConversionUtils::PhysVecToVisVec_noscale(vScale, m_vScale);

    bUpdateDebugRendering = true;
  }

  if (m_pRigidBody != NULL)
  {  
    // Set the transformation in Havok
    hkTransform hkTfOut;
    hkTfOut.setRotation(mRotation);
    hkTfOut.getTranslation().setMul(mTransform.getColumn<3>(),vHavokConversionUtils::GetVision2HavokScaleSIMD());
    m_pRigidBody->setTransform(hkTfOut);

    if (bUpdateDebugRendering)
    {
      SetDebugRendering (false);
    }
  }
}

// --------------------------------------------------------------------------
// Accessing Havok Instances
// --------------------------------------------------------------------------

const hkpShape *vHavokStaticMesh::GetHkShape() const
{
  if (m_pRigidBody == NULL) 
    return NULL;

  return  m_pRigidBody->getCollidable()->getShape();
}

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
