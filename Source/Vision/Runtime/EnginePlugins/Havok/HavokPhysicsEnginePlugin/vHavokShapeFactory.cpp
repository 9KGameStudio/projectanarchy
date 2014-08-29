/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokShapeFactory.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokFileStreamAccess.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokShapeCache.hpp>

#if defined(SUPPORTS_TERRAIN)
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainSector.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainSectorManager.hpp>
#endif

#include <Physics2012/Collide/Shape/Compound/Collection/SimpleMesh/hkpSimpleMeshShape.h>
#include <Physics2012/Collide/Shape/HeightField/SampledHeightField/hkpSampledHeightFieldShape.h>
#include <Physics2012/Collide/Shape/HeightField/SampledHeightField/hkpSampledHeightFieldBaseCinfo.h>
#include <Physics2012/Collide/Shape/HeightField/TriSampledHeightField/hkpTriSampledHeightFieldCollection.h>
#include <Physics2012/Collide/Shape/HeightField/TriSampledHeightField/hkpTriSampledHeightFieldBvTreeShape.h>
#include <Physics2012/Internal/Collide/BvCompressedMesh/hkpBvCompressedMeshShape.h>
#include <Physics2012/Internal/Collide/BvCompressedMesh/hkpBvCompressedMeshShapeCinfo.h> 

#include <Common/Base/Config/hkConfigVersion.h>
#include <Common/Base/Container/LocalArray/hkLocalArray.h>
#include <Common/Base/Reflection/Registry/hkVtableClassRegistry.h>
#include <Common/Base/Reflection/hkClass.h>

//-----------------------------------------------------------------------------------

hkRefNew<hkpShape> vHavokShapeFactory::CreateShapeFromMesh(VBaseMesh* pMesh, const hkvVec3& vScale, 
  unsigned int uiCreationFlags, VisWeldingType_e eWeldingType)
{
  VASSERT(pMesh != NULL);

  const vHavokPhysicsModule *pModule = vHavokPhysicsModule::GetInstance();
  VASSERT(pModule != NULL);

  const bool bCacheShape = (pMesh->GetFilename() != NULL && (uiCreationFlags & VShapeCreationFlags_CACHE_SHAPE) != 0);
  char szShapeId[512];

  const bool bIsConvex = IsShapeConvex(pMesh, uiCreationFlags);

  hkpShape* pShape = HK_NULL;
  if (bCacheShape)
  {
    // Check whether the shape has already been cached.
    if (bIsConvex)
      vHavokShapeCache::ComputeShapeIdForConvexShape(szShapeId, pMesh->GetFilename(), vScale, uiCreationFlags);
    else
      vHavokShapeCache::ComputeShapeIdForMeshShape(szShapeId, pMesh->GetFilename(), vScale, VisStaticMeshInstance_cl::VIS_COLLISION_BEHAVIOR_CUSTOM, eWeldingType);

    pShape = vHavokShapeCache::FindShapeOnDisk(szShapeId, GetCollisionMesh(pMesh, uiCreationFlags)->GetFileTime());
    if (pShape != HK_NULL)
      return pShape;
  }

  // Create new shape.
  hkvMat4 mTransform;
  mTransform.setScalingMatrix(vScale);

  if (bIsConvex)
  {
    pShape = CreateConvexHullShapeFromMesh(pMesh, mTransform, uiCreationFlags);
  }
  else
  {
    pShape = CreateCompressedBvMeshShapeFromMesh(pMesh, mTransform, uiCreationFlags, eWeldingType);
  }

  // Add to cache.
  if (bCacheShape && pShape != HK_NULL)
  {
    vHavokShapeCache::AddShape(szShapeId, pShape);
  }

  return pShape;
}

hkRefNew<hkpShape> vHavokShapeFactory::CreateShapeFromStaticMeshInstances(
  VisStaticMeshInstance_cl const* const* ppInstances, unsigned int uiNumInstances, unsigned int uiCreationFlags)
{
  VASSERT(uiNumInstances > 0 && ppInstances[0] != NULL);
  VBaseMesh* pMesh = ppInstances[0]->GetMesh();
  VASSERT(pMesh != NULL);

  const vHavokPhysicsModule *pModule = vHavokPhysicsModule::GetInstance();
  VASSERT(pModule != NULL);

  // Only create a convex shape if a single mesh instance is used, since otherwise merging multiple mesh instances in one single convex hull
  // will provide in most cases not the desired behavior. Moreover we can only create either a convex hull or a concave mesh shape, therefore
  // mesh instances with different collision types can't be merged into one shape.
  const bool bIsConvex = (uiNumInstances == 1 && IsShapeConvex(ppInstances[0]->GetMesh(), uiCreationFlags));

  // We are just caching single static mesh instances and no static mesh collections.
  const bool bCacheShape = (pMesh->GetFilename() != NULL && (uiCreationFlags & VShapeCreationFlags_CACHE_SHAPE) != 0 && uiNumInstances == 1);

  char szShapeId[512];
  hkpShape* pShape = HK_NULL;

  if (bCacheShape)
  {
    const hkvVec3 vScale = ExtractScaling(ppInstances[0]->GetTransform());

    // Check whether the shape has already been cached.
    if (bIsConvex)
      vHavokShapeCache::ComputeShapeIdForConvexShape(szShapeId, pMesh->GetFilename(), vScale, uiCreationFlags);
    else
      vHavokShapeCache::ComputeShapeIdForMeshShape(szShapeId, pMesh->GetFilename(), vScale, ppInstances[0]->GetCollisionBehavior(), VIS_WELDING_TYPE_NONE);

    pShape = vHavokShapeCache::FindShapeOnDisk(szShapeId, GetCollisionMesh(pMesh, uiCreationFlags)->GetFileTime());
    if (pShape != NULL)
      return pShape;
  }

  if (bIsConvex)
  {
    // Calculate scaling matrix in local space.
    hkvMat4 mTransform = ComputeReferenceTransform(ppInstances[0]->GetTransform());
    mTransform = mTransform.multiply(ppInstances[0]->GetTransform()); 

    pShape = CreateConvexHullShapeFromMesh(pMesh, mTransform, uiCreationFlags);
  }
  else
  {
    pShape = CreateMeshShapeFromStaticMeshInstances(ppInstances, uiNumInstances, uiCreationFlags);
  }

  if (bCacheShape && pShape != HK_NULL)
  {
    vHavokShapeCache::AddShape(szShapeId, pShape);
  }

  return pShape;
}

//-----------------------------------------------------------------------------------
// Shape Creation

hkpShape* vHavokShapeFactory::CreateConvexHullShapeFromMesh(VBaseMesh* pMesh, const hkvMat4& mTransform, unsigned int uiCreationFlags)
{
  // Get the collision mesh for the specified mesh
  const IVCollisionMesh *pColMesh = GetCollisionMesh(pMesh, uiCreationFlags);
  VASSERT(pColMesh != NULL);

  hkGeometry geom;

  const int iNumColMeshes = hkvMath::Max(pColMesh->GetSubmeshCount(), 1);
  for (int i = 0; i < iNumColMeshes; i++)
    BuildGeometryFromCollisionMesh(pColMesh, i, mTransform, true, geom);
  VVERIFY_OR_RET_VAL(geom.m_vertices.getSize() > 0, NULL);

  // Set the build configuration to set planes equations and connectivity automatically
  hkpConvexVerticesShape::BuildConfig config;
  config.m_createConnectivity = true;
  config.m_shrinkByConvexRadius = (uiCreationFlags & VShapeCreationFlags_SHRINK) != 0;

  hkStridedVertices stridedVerts;
  stridedVerts.m_numVertices = geom.m_vertices.getSize();
  stridedVerts.m_striding = sizeof(hkVector4);
  stridedVerts.m_vertices = (const hkReal*)geom.m_vertices.begin();

  // Create convex shape
  return new hkvConvexVerticesShape(pColMesh->GetFileTime(), stridedVerts, config);
}

hkpShape* vHavokShapeFactory::CreateCompressedBvMeshShapeFromMesh(VBaseMesh* pMesh, const hkvMat4& mTransform, unsigned int uiCreationFlags, VisWeldingType_e eWeldingType)
{
  // Get the collision mesh for the specified mesh
  const IVCollisionMesh *pColMesh = GetCollisionMesh(pMesh, uiCreationFlags);
  VASSERT(pColMesh != NULL);
  
  const int iNumColMeshes = hkvMath::Max(pColMesh->GetSubmeshCount(), 1);
  hkGeometry geom;
  for (int i = 0; i < iNumColMeshes; i++)
    BuildGeometryFromCollisionMesh(pColMesh, i, mTransform, false, geom);
  VVERIFY_OR_RET_VAL(geom.m_vertices.getSize() > 0, NULL);

  // Note: A DynamicMesh can be animated. We are treating it as static here.
  hkpDefaultBvCompressedMeshShapeCinfo ci(&geom);
  ci.m_collisionFilterInfoMode = hkpBvCompressedMeshShape::PER_PRIMITIVE_DATA_NONE;
  ci.m_userDataMode = hkpBvCompressedMeshShape::PER_PRIMITIVE_DATA_NONE; // Materials
  ci.m_weldingType = vHavokConversionUtils::VisToHkWeldingType(eWeldingType);

  hkvBvCompressedMeshShape* pCompressedMeshShape = new hkvBvCompressedMeshShape(ci, pColMesh->GetFileTime());

  if (pCompressedMeshShape->getNumChildShapes() <= 0)
  {
    pCompressedMeshShape->removeReference();

    const char* szMeshFilename = (pMesh->GetFilename() != NULL) ? pMesh->GetFilename() : "<noname>";
    hkvLog::Warning("Physics Shape for [%s] is empty. Volume too small?",  szMeshFilename);
    return HK_NULL;
  }

  return pCompressedMeshShape;
}

class vHavokCompressedInfoCinfo : public hkpDefaultBvCompressedMeshShapeCinfo
{
public:
  vHavokCompressedInfoCinfo(const hkGeometry* geometry, hkUint8* collisionInfoPerTri) 
    : hkpDefaultBvCompressedMeshShapeCinfo(geometry)
    , m_collisionInfoPerTri(collisionInfoPerTri)
  {
  }

  virtual ~vHavokCompressedInfoCinfo() {}

  virtual hkUint32 getTriangleCollisionFilterInfo(int triangleIndex) const 
  {
    if (m_collisionInfoPerTri)
    {
      hkUint8 cdinfo = m_collisionInfoPerTri[ triangleIndex ];
      return cdinfo;
    }
    else return 0;
  }

  virtual hkUint32 getTriangleUserData(int triangleIndex) const
  {
    int matId = m_geometry->m_triangles[triangleIndex].m_material;
    if (matId >= 0) 
    {
      VASSERT(matId < 256);
      return (hkUint32)matId;
    }
    return 0;
  }

  hkUint8* m_collisionInfoPerTri;
};

hkpShape* vHavokShapeFactory::CreateMeshShapeFromStaticMeshInstances(
  VisStaticMeshInstance_cl const* const* ppInstances, unsigned int uiNumInstances, unsigned int uiCreationFlags)
{
  VASSERT(uiNumInstances > 0);

  // Get the reference transformation. We use the first static mesh as reference
  // transformation, and thus as the position of the corresponding rigid body.
  const hkvMat4 refTransform = ComputeReferenceTransform(ppInstances[0]->GetTransform());

  // For single mesh instances the per instance welding type is used. For merged mesh instances the global merged welding type is used. 
  const VisWeldingType_e eWeldingType = (uiNumInstances == 1) ? 
    ppInstances[0]->GetWeldingType() : 
    static_cast<VisWeldingType_e>(vHavokPhysicsModule_GetWorldSetupSettings().m_iMergedStaticWeldingType);

  const bool bAllowPerTriCollisionInfo = (ppInstances[0]->GetCollisionBehavior() == VisStaticMeshInstance_cl::VIS_COLLISION_BEHAVIOR_FROMFILE);

  hkGeometry geom;
  hkInt64 iFileTime = 0;

  // Iterate all the passed mesh instances
  hkvMeshMaterialCache materials;
  hkArray<hkUint8> collisionMask;
  int subPartIndex = 0;
  bool bHasValidShape = false;

  for (unsigned int i = 0; i < uiNumInstances; i++)
  { 
    // Get the collision mesh for the static mesh instance
    const VisStaticMeshInstance_cl *pMeshInstance = ppInstances[i];
    VisStaticMesh_cl *pMesh = pMeshInstance->GetMesh();
    IVCollisionMesh *pColMesh = GetCollisionMesh(pMesh, uiCreationFlags);
    if (pColMesh == NULL)
    {
      VASSERT(false);
      continue;
    }

    if(pColMesh->GetMeshData()->GetIndexCount() == 0)
      continue;
    else
      bHasValidShape = true;

    // Simply retrieve file time from last static mesh instance
    iFileTime = pColMesh->GetFileTime();

    // Create the Havok triangle sub part
    {
      int startingNumTris = geom.m_triangles.getSize();

      // We transform each static mesh into the reference space.
      hkvMat4 mTransform = refTransform;
      mTransform = mTransform.multiply(pMeshInstance->GetTransform());

      int iNumColMeshes = hkvMath::Max(pColMesh->GetSubmeshCount(), 1);
      for (int i=0;i<iNumColMeshes;i++)
      {
        int startingNumVerts = geom.m_vertices.getSize();
        BuildGeometryFromCollisionMesh(pColMesh, i, mTransform, false, geom);
        int endNumVerts = geom.m_vertices.getSize();
        int endNumTris = geom.m_triangles.getSize();
        VASSERT( (endNumVerts - startingNumVerts) > 0 );

        if (bAllowPerTriCollisionInfo && (pColMesh->GetSubmeshCount() > 0))
        {
          const VPhysicsSubmesh& submesh = pColMesh->GetSubmeshes()[i];
          hkUint32 cdMask = submesh.iGroupFilter & 0xff; // only lower 8 bits stored in compressed mesh.

          int numCD = endNumTris - collisionMask.getSize();
          hkUint8* cdi = collisionMask.expandBy( numCD );
          hkString::memSet(cdi, cdMask, numCD);
        }
      }
   
      // ColMesh can have Materials. Also need material to store the orig static mesh index (so if count > 1)
      if ( pColMesh->GetTriSrfIndices() )
      {
        VColMeshMaterial *pColMaterials = pColMesh->GetMaterials();
        VASSERT(pColMaterials);
        int iNumMats = pColMesh->GetMaterialCount();
        int matIndexOffset = materials.getSize();

        for(int ii=0;ii<iNumMats;ii++)
        {
          hkvMeshMaterial& mat = materials.expandOne();
          // in Havok there is no StaticFriction, DynamicFriction, anisotropic StaticFriction/ DynamicFriction
          // instead there is only Friction available
          // so DynamicFriction is used for Havok Friction -> is there a better workaround?
          mat.m_fFriction = pColMaterials[ii].fDynamicFriction; 

          mat.m_fRestitution = pColMaterials[ii].fRestitution;
          if (! pColMaterials[ii].szUserData.IsEmpty()) 
            mat.m_userData = pColMaterials[ii].szUserData.GetChar();   

          mat.m_iOrignalSubMeshIndex = subPartIndex;
        }

        // reindex geom mat id
        if (matIndexOffset> 0)
        {
          for (int ti=startingNumTris; ti < geom.m_triangles.getSize(); ++ti)
          {
            VASSERT(geom.m_triangles[ti].m_material < iNumMats);
            VASSERT(geom.m_triangles[ti].m_material >= 0);
            geom.m_triangles[ti].m_material += matIndexOffset;
          }
        }
      }

      // per instance part
      ++subPartIndex;     
    }
  }

  if(!bHasValidShape)
    return HK_NULL;

  const bool bHaveTriCDData = !collisionMask.isEmpty();
  vHavokCompressedInfoCinfo ci(&geom, bHaveTriCDData ? collisionMask.begin() : HK_NULL);
  ci.m_weldingType = vHavokConversionUtils::VisToHkWeldingType(eWeldingType);
  ci.m_collisionFilterInfoMode = bHaveTriCDData ? hkpBvCompressedMeshShape::PER_PRIMITIVE_DATA_8_BIT : hkpBvCompressedMeshShape::PER_PRIMITIVE_DATA_NONE;
  
  hkvBvCompressedMeshShape *pCompressedMeshShape = HK_NULL;
  if ( materials.getSize() > 0 && materials.getSize() < 255)
  {
    ci.m_userDataMode = hkpBvCompressedMeshShape::PER_PRIMITIVE_DATA_8_BIT; 
    pCompressedMeshShape = new hkvBvCompressedMeshShape(ci, materials, iFileTime);
  }
  else
  {
    ci.m_userDataMode = hkpBvCompressedMeshShape::PER_PRIMITIVE_DATA_NONE; 
    pCompressedMeshShape = new hkvBvCompressedMeshShape(ci, iFileTime);
  }

  if(pCompressedMeshShape->getNumChildShapes() < 1)
  {
    hkvLog::Error("[vHavokPhysics] hkvBvCompressedMeshShape could not be created for static mesh. One reason for this can be an undersized mesh.");
    pCompressedMeshShape->removeReference();
    pCompressedMeshShape = NULL;
  }
  
  return pCompressedMeshShape;
}

// -------------------------------------------------------------------------- //
// Havok Shape - Terrain                                                      //
// -------------------------------------------------------------------------- //

#if defined(SUPPORTS_TERRAIN)

hkpShape* vHavokShapeFactory::CreateShapeFromTerrain(const VTerrainSector& sector)
{
  // Check if shape is already cached on disk. Only try to load cached shape file outside of vForge, since otherwise the physics
  // representation will not be updated on terrain editing.
  const bool bAllowDiskCaching = (vHavokPhysicsModule_GetDefaultWorldRuntimeSettings().m_bEnableDiskShapeCaching == TRUE);
  if (!Vision::Editor.IsInEditor() && bAllowDiskCaching)
  {
    hkpShape* pCachedShape = vHavokShapeCache::LoadTerrainSectorShape(&sector);
    if (pCachedShape)
    {
      return pCachedShape;
    }
#if defined(HK_DEBUG)
    else
    {
      // Only warn when terrain sector was created from file, since code-generated sectors can't have cached shapes. 
      // Those are only generated in vForge.
      char szFilename[FS_MAX_PATH];
      sector.m_Config.GetSectorFilename(szFilename, sector.m_iIndexX, sector.m_iIndexY, "hmap", true);
      if (Vision::File.Exists(szFilename))
      {
        hkvLog::Warning("Cached HKT file for %s is missing. Please generate HKT file (see documentation for details).", szFilename);
      }
    }
#endif
  }

  hkpSampledHeightFieldBaseCinfo ci; 

  ci.m_xRes = sector.m_Config.m_iHeightSamplesPerSector[0]+1;
  ci.m_zRes = sector.m_Config.m_iHeightSamplesPerSector[1]+1;

  // Overlapping samples into next sectors on all edges (so res-1 is scale)
  ci.m_scale.set( VIS2HK_FLOAT_SCALED(sector.m_Config.m_vSectorSize.x/float(ci.m_xRes-1)), 1, VIS2HK_FLOAT_SCALED(sector.m_Config.m_vSectorSize.y/float(ci.m_zRes-1)) );
  ci.m_minHeight = VIS2HK_FLOAT_SCALED(sector.m_fMinHeightValue);
  ci.m_maxHeight = VIS2HK_FLOAT_SCALED(sector.m_fMaxHeightValue);

  const bool bHasHoles = sector.HasHoles();
  const bool bPrecise = (sector.GetPhysicsType() >= VTerrainSector::VPHYSICSTYPE_PRECISE);
  const bool bTriShape = (bPrecise || bHasHoles);

  // Tight-fit only needed for triangle shapes since they have a convex shape radius.
  const bool bTightFit = (bTriShape && sector.GetPhysicsTightFit());
  hkpSampledHeightFieldShape* pHeightFieldShape = bTightFit ?
    new hkvSampledOffsetHeightFieldShape(ci, &sector, -hkConvexShapeDefaultRadius) :
    new hkvSampledHeightFieldShape(ci, &sector);

  if (!bPrecise && bHasHoles)
    hkvLog::Info("Warning: Terrain sector has holes but uses approximated physics representation. Therefore precise physics representation enforced.");

  // Two choices here: Use just the height field as is, or wrap it in a triangle sampler. 
  // The triangle sampler will give exact collisions, the height field alone will be based on collision spheres.
  // We *have* to use the accurate version in case the sector has holes.
  if (bTriShape)
  {
    hkpTriSampledHeightFieldCollection* pCollection;
    hkpTriSampledHeightFieldBvTreeShape* pBvTree;

    if (bHasHoles) // use a slightly modified version of hkpTriSampledHeightFieldCollection in case it has holes
    {
      pCollection = new hkvTriSampledHeightFieldCollection(&sector, pHeightFieldShape);
      pBvTree = new hkvTriSampledHeightFieldBvTreeShape(pCollection);
    } 
    else
    {
      pCollection = new hkpTriSampledHeightFieldCollection(pHeightFieldShape);
      pBvTree = new hkpTriSampledHeightFieldBvTreeShape(pCollection);
    }

    // Winding must be set to WELDING_TYPE_ANTICLOCKWISE (see hkpTriSampledHeightFieldCollection::initWeldingInfo()).
    if (sector.GetPhysicsType() == VTerrainSector::VPHYSICSTYPE_PRECISE_OFFLINE_WELDING)
      hkpMeshWeldingUtility::computeWeldingInfo(pCollection, pBvTree, hkpWeldingUtility::WELDING_TYPE_ANTICLOCKWISE);

    pCollection->removeReference();
    pHeightFieldShape->removeReference();
    return pBvTree;
  }
  else
  {
    return pHeightFieldShape;
  }
}

#endif

//-----------------------------------------------------------------------------------
// Helpers

const hkvVec3 vHavokShapeFactory::GetPivotOffset(const VDynamicMesh *pMesh, const hkvVec3& vScaleEntity)
{
  // Get center of bounding box
  hkvAlignedBBox bbox = pMesh->GetCollisionBoundingBox();
  // fall back to rendering bbox
  if (!bbox.isValid())
  {
    bbox = pMesh->GetBoundingBox();
    if (!bbox.isValid())
      return hkvVec3::ZeroVector();
  }

  // Take scaling into account
  return bbox.getCenter().compMul(vScaleEntity);
}

const hkvVec3 vHavokShapeFactory::GetPivotOffset(const VDynamicMesh *pMesh, float fUniformScaleEntity)
{
  // Get center of bounding box
  const hkvAlignedBBox& bbox = pMesh->GetCollisionBoundingBox();

  // Take scaling into account
  return bbox.getCenter() * fUniformScaleEntity;
}

const hkvVec3 vHavokShapeFactory::ExtractScaling(const hkvMat4& mTransform)
{
  hkvVec3 vx(hkvNoInitialization), vy(hkvNoInitialization), vz(hkvNoInitialization);
  mTransform.getAxisXYZ(&vx, &vy, &vz);
  return hkvVec3(vx.getLength(), vy.getLength(), vz.getLength());
}

const hkvMat4 vHavokShapeFactory::ComputeReferenceTransform(const hkvMat4& mTransform)
{
  hkvMat4 mReferenceTransform = mTransform;

  // Remove any scaling from the reference matrix. This one has to be applied to
  // the Havok shapes, and thus needs to be part of the mesh transforms.
  mReferenceTransform.setScalingFactors(hkvVec3(1.0f));

  // We need the inverse to transform each instance into reference space
  // Since we have a rigid transformation, transposing the rotational part and 
  // inverting the rotated translation vector is just fine.
  const hkvMat3 mInvRotation = mReferenceTransform.getRotationalPart().getTransposed();
  mReferenceTransform.setRotationalPart(mInvRotation);
  mReferenceTransform.setTranslation(mInvRotation.transformDirection(-mReferenceTransform.getTranslation()));

  return mReferenceTransform;
}

void vHavokShapeFactory::BuildGeometryFromCollisionMesh(
  const IVCollisionMesh *pColMesh, int iSubmeshIndex,const hkvMat4 &transform, bool bConvex, hkGeometry& geom)
{
  const VSimpleCollisionMeshBase *pMeshBase = pColMesh->GetMeshData();

  int iNumVerts = pMeshBase->GetVertexCount(); 
  int iFirstVertexIndex = 0;
  int iNumTris = pMeshBase->GetIndexCount() / 3; 
  int iStartIndex = 0;

  const bool bSubmeshsSupported = pColMesh->GetSubmeshCount() > 0;
  if (bSubmeshsSupported)
  {
    VASSERT(iSubmeshIndex < pColMesh->GetSubmeshCount());
    VPhysicsSubmesh submesh = pColMesh->GetSubmeshes()[iSubmeshIndex];
    iNumVerts = (submesh.iLastVertexIndex-submesh.iFirstVertexIndex)+1;
    iFirstVertexIndex = submesh.iFirstVertexIndex;
    iNumTris = submesh.iNumIndices / 3;
    iStartIndex = submesh.iStartIndex;
    VASSERT(pColMesh->GetTriSrfIndices());
  }

  hkTransform hkT; // in Vision scaling
  {
    hkMatrix4 m; vHavokConversionUtils::VisMatrixToHkMatrix(transform, m, false, false, true);
    m.get(hkT);
  }

  VASSERT(iNumVerts > 0);
  const unsigned int uiVtxComponentCount = iNumVerts * 3;

  int geomVertexStartIndex = geom.m_vertices.getSize();
  int vertexIndexOffset = geomVertexStartIndex - iFirstVertexIndex;

  hkVector4 *pVertices = (hkVector4*)geom.m_vertices.expandBy( iNumVerts );
  hkvVec3* pVertex = pMeshBase->GetVertexPtr();

  // start retrieving vertices from the start of the vertex-list (iFirstVertexIndex=0) when submeshes are not supported,
  // otherwise from the iFirstVertexIndex supplied by the submesh
  pVertex = &pVertex[iFirstVertexIndex]; 
  for (int i=0; i < iNumVerts; ++i, pVertex++) 
  {
    hkVector4 d; vHavokConversionUtils::VisVecToPhysVec_noscale(*pVertex,d);
    pVertices[i]._setTransformedPos(hkT, d);
    pVertices[i].mul(vHavokConversionUtils::GetVision2HavokScaleSIMD());
  }

  // For convex hulls we do not need to retrieve triangle information.
  if (bConvex)
    return;

  VASSERT(iNumTris > 0);

  int indexStriding = 0;   
  void *pIndices = NULL;

  int geomTriStartIndex = geom.m_triangles.getSize();
  geom.m_triangles.expandBy(iNumTris);

  short* triMatIndices = pColMesh->GetTriSrfIndices()? pColMesh->GetTriSrfIndices() + (iStartIndex/3) : HK_NULL;
  if (pMeshBase->m_pIndex16)
  {
    unsigned short* visionInd = &pMeshBase->m_pIndex16[iStartIndex];
    for (int ti=0; ti < iNumTris; ++ti)
    {
      hkGeometry::Triangle& tri = geom.m_triangles[geomTriStartIndex + ti];
      // Note we flip the winding for the static meshes so that 'front facing' is CW 
      tri.m_a = (int)*visionInd + vertexIndexOffset; ++visionInd;
      tri.m_c = (int)*visionInd + vertexIndexOffset; ++visionInd;
      tri.m_b = (int)*visionInd + vertexIndexOffset; ++visionInd;
      if (triMatIndices)
      {
        tri.m_material = triMatIndices[ti];
      }
      else
      {
        tri.m_material = 0;
      }
    }
  }
  else if (pMeshBase->m_pIndex32)
  {
    unsigned int* visionInd = &pMeshBase->m_pIndex32[iStartIndex];
    for (int ti=0; ti < iNumTris; ++ti)
    {
      hkGeometry::Triangle& tri = geom.m_triangles[geomTriStartIndex + ti];
      // Note we flip the winding for the static meshes so that 'front facing' is CW 
      tri.m_a = (int)*visionInd + vertexIndexOffset; ++visionInd;
      tri.m_c = (int)*visionInd + vertexIndexOffset; ++visionInd;
      tri.m_b = (int)*visionInd + vertexIndexOffset; ++visionInd;
      if (triMatIndices)
      {
        tri.m_material = triMatIndices[ti];
      }
      else
      {
        tri.m_material = 0;
      }
    }
  }
  else
  {
    VASSERT(FALSE);
  }
}

//-----------------------------------------------------------------------------------
// Resource dependencies

#if defined(SUPPORTS_SNAPSHOT_CREATION)

void vHavokShapeFactory::GetHktDependencies(VResourceSnapshot &snapshot, VisStaticMeshInstance_cl *pMeshInstance)
{
  VASSERT(pMeshInstance != NULL);

  if (!vHavokPhysicsModule_GetDefaultWorldRuntimeSettings().m_bEnableDiskShapeCaching)
    return;

  if (pMeshInstance->GetPhysicsObject() == NULL)
    return;

  vHavokStaticMesh* pWrappedStaticMesh = static_cast<vHavokStaticMesh*>(pMeshInstance->GetPhysicsObject());
  hkpRigidBody *pRigidBody = pWrappedStaticMesh->GetHkRigidBody();
  if (pRigidBody == NULL)
    return;

  pRigidBody->markForRead();
  const hkpShape* pShape = pRigidBody->getCollidable()->getShape();
  pRigidBody->unmarkForRead();
  if (pShape == NULL)
    return;

  VStaticString<FS_MAX_PATH> szCachedShapePath;
  if (vHavokShapeCache::GetShapePath(pShape, szCachedShapePath, false))
  {
    // The hkt file may not yet exist, so don't try to open the file.
    snapshot.AddFileDependency(pMeshInstance->GetMesh(), szCachedShapePath, pShape->getAllocatedSize());
  }
}

void vHavokShapeFactory::GetHktDependencies(VResourceSnapshot &snapshot, VisBaseEntity_cl *pEntity)
{
  VASSERT(pEntity != NULL);

  if (!vHavokPhysicsModule_GetDefaultWorldRuntimeSettings().m_bEnableDiskShapeCaching)
    return;

  // Get wrapped rigid body
  vHavokRigidBody *pWrappedRigidBody = pEntity->Components().GetComponentOfType<vHavokRigidBody>();
  if (pWrappedRigidBody == NULL)
    return;

  // Get shape
  hkpRigidBody *pRigidBody = pWrappedRigidBody->GetHkRigidBody();
  if (pRigidBody == NULL)
    return;

  pRigidBody->markForRead();
  const hkpShape* pShape = pRigidBody->getCollidable()->getShape();
  pRigidBody->unmarkForRead();
  if (pShape == NULL)
    return;

  // Get mesh
  VDynamicMesh *pMesh = pEntity->GetMesh();
  if (pMesh == NULL)
    return;

  // Get HKT file dependency for convex / mesh rigid body.
  VStaticString<FS_MAX_PATH> szCachedShapePath;
  if (vHavokShapeCache::GetShapePath(pShape, szCachedShapePath, false))
  {
    // The hkt file may not yet exist, so don't try to open the file.
    snapshot.AddFileDependency(pMesh, szCachedShapePath, pShape->getAllocatedSize());
  }
}

void vHavokShapeFactory::GetHktDependencies(VResourceSnapshot &snapshot, VTerrainSector *pSector)
{
  VASSERT(pSector != NULL);

  if (!vHavokPhysicsModule_GetDefaultWorldRuntimeSettings().m_bEnableDiskShapeCaching)
    return;

  // Return when no physics representation
  vHavokTerrain* pHavokTerrain = static_cast<vHavokTerrain*>(pSector->GetPhysicsUserData());
  if (pHavokTerrain == NULL)
    return;

  // Build the cached shape filename
  VStaticString<FS_MAX_PATH> szCachedShapePath;
  if (!vHavokShapeCache::GetTerrainSectorShapePath(szCachedShapePath, pSector))
    return;

  // The Terrain HKT file is always written when its respective sector is saved, so it's safe to open it.
  IVFileInStream* pIn = Vision::File.Open(szCachedShapePath);
  if (pIn == NULL)
  {
    // User might have deleted the HKT file manually -> re-generate.
    pHavokTerrain->SaveShapeToFile();
    pIn = Vision::File.Open(szCachedShapePath);

    if (pIn == NULL)
    {
      hkvLog::Warning("vHavokShapeFactory::GetHktDependencies: Terrain Sector Shape '%s' was not saved to disk.", szCachedShapePath.AsChar());
      return;
    }
  }

  snapshot.AddFileDependency(pSector, szCachedShapePath, pIn->GetSize());
  pIn->Close();
}

#endif

/*
 * Havok SDK - Base file, BUILD(#20140710)
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
