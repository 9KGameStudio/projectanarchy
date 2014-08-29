/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokTerrain.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokUserData.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokShapeFactory.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokShapeCache.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainSector.hpp>
#include <Physics2012/Internal/Collide/BvCompressedMesh/hkpBvCompressedMeshShape.h>
#include <Physics2012/Internal/Collide/BvCompressedMesh/hkpBvCompressedMeshShapeCinfo.h>
#include <Physics2012/Internal/Collide/StaticCompound/hkpStaticCompoundShape.h>
#include <Physics2012/Collide/Shape/Convex/ConvexTranslate/hkpConvexTranslateShape.h> 
#include <Physics2012/Collide/Shape/Convex/Triangle/hkpTriangleShape.h>
#include <Common/Base/Container/PointerMultiMap/hkPointerMultiMap.h>
#include <Geometry/Collide/Algorithms/Triangle/hkcdTriangleUtil.h>

// -------------------------------------------------------------------------- //
// Constructor / Destructor                                                   //
// -------------------------------------------------------------------------- //

vHavokTerrain::vHavokTerrain(vHavokPhysicsModule &module) : m_bInitialized(FALSE), m_pRigidBody(NULL), m_terrainSector(NULL), m_module(module)
{
}

vHavokTerrain::~vHavokTerrain()
{
  CommonDeinit();
}

void vHavokTerrain::DisposeObject()
{
  CommonDeinit();
}

void vHavokTerrain::CommonDeinit()
{
  RemoveHkRigidBody();
}


// -------------------------------------------------------------------------- //
// Initialisation                                                             //
// -------------------------------------------------------------------------- //

void vHavokTerrain::Init(const VTerrainSector &terrainSector)
{
  // Init may only be called once
  VVERIFY_OR_RET(m_bInitialized == false);
  m_bInitialized = true;

  m_terrainSector = &terrainSector;

  // Build the Havok shape
  CreateHkRigidBody();
}


// -------------------------------------------------------------------------- //
// Havok Rigid Body Creation                                                  //
// -------------------------------------------------------------------------- //

void vHavokTerrain::CreateHkRigidBody()
{
  // Create the Havok shape
  hkpShape *pShape = vHavokShapeFactory::CreateShapeFromTerrain(*m_terrainSector);

  // Create the Havok rigid body
  hkpRigidBodyCinfo rci;
  rci.m_motionType = hkpMotion::MOTION_FIXED;
  rci.m_shape = pShape;

  //subtract the y sample spacing from the original position to make sure that the physical representation and the visible terrain match
  hkvVec3 vSpacing(0, m_terrainSector->m_Config.m_vSampleSpacing.y, 0);
  vHavokConversionUtils::VisVecToPhysVecWorld(hkvVec3(m_terrainSector->GetSectorOrigin() - vSpacing),rci.m_position);
  rci.m_collisionFilterInfo = hkpGroupFilter::calcFilterInfo(vHavokPhysicsModule::HK_LAYER_COLLIDABLE_TERRAIN);

  // We have two choices here. Heightfields in Havok are on the X,Z local plane, but in Vision they are 
  // X,Y, so some transform is required. We could rotate by -90 and invert the heights but that results 
  // in normals that are the opposite to what you would expect (which is fine though as double sided, but not ideal).
  // The second option is to rotate the opposite way and invert the lookup on Y (Havok local Z)
  // This second option of course shifts the direction of the extends, so we need to also include the translation for that:
  // y, but take into account the single sample edge overlap in the terrain
  hkSimdReal yvOffset; 
  yvOffset.setFromFloat( m_terrainSector->m_Config.m_vSectorSize.y + m_terrainSector->m_Config.m_vSampleSpacing.y );
  yvOffset.mul(vHavokConversionUtils::GetVision2HavokScaleSIMD());
  // Counter act the shift the extent dir
  rci.m_position.addMul( hkVector4::getConstant<HK_QUADREAL_0100>(), yvOffset );  
  rci.m_rotation.setAxisAngle(hkVector4::getConstant<HK_QUADREAL_1000>(),hkSimdReal_PiOver2); // Basis change
  
  rci.m_friction = hkReal(0.2f);

  m_pRigidBody = new hkpRigidBody( rci );

  // Set user data to identify this terrain during collision detection (raycast etc)
  m_pRigidBody->setUserData((hkUlong)vHavokUserDataPointerPair_t::CombineTypeAndPointer(this, V_USERDATA_TERRAIN));

  // Add our instance to the module
  m_module.AddTerrain(this);

  // Remove reference
  pShape->removeReference();   
}

void vHavokTerrain::RemoveHkRigidBody()
{
  if (!m_pRigidBody)
    return;

  // Keep our object alive
  VSmartPtr<vHavokTerrain> keepAlive = this;

  // If we still have an owner, then remove the rigid body to the Havok world
  m_module.RemoveTerrain(this);

  // Free the rigid body 
  m_pRigidBody->removeReference();
  m_pRigidBody = NULL;
}

// -------------------------------------------------------------------------- //
// Accessing Havok Instances                                                  //
// -------------------------------------------------------------------------- //

const hkpShape *vHavokTerrain::GetHkShape() const
{
  if (m_pRigidBody == NULL) 
    return NULL;

  return m_pRigidBody->getCollidable()->getShape();
}

// -------------------------------------------------------------------------- //
// Accessing Vision Instances                                                  //
// -------------------------------------------------------------------------- //

const VTerrainSector* vHavokTerrain::GetWrappedTerrainSector()
{
  return m_terrainSector;
}

// -------------------------------------------------------------------------- //
// Helper methods                                                             //
// -------------------------------------------------------------------------- //

void vHavokTerrain::SaveShapeToFile()
{
  VASSERT(m_pRigidBody != NULL);
  m_module.MarkForRead(); 
  const hkpShape *pShape = m_pRigidBody->getCollidable()->getShape();
  m_module.UnmarkForRead();
  vHavokShapeCache::SaveTerrainSectorShape(m_terrainSector, pShape);
}

// -------------------------------------------------------------------------- //
// Decoration group
// -------------------------------------------------------------------------- //

class hkvDecorationMeshShape;

// Custom raycast collector to filter out holes
class vHavokDecorationHitCollector : public hkpRayHitCollector
{
public:
	vHavokDecorationHitCollector(const hkvDecorationMeshShape *pDecoMesh, hkpRayHitCollector& collector, const hkpShapeRayCastInput& input) 
		: m_decoMeshShape(pDecoMesh), m_collector(collector), m_input(input)
	{
	}

  // see implementation below
	virtual void addRayHit(const hkpCdBody& cdBody, const hkpShapeRayCastCollectorOutput& hitInfo) HKV_OVERRIDE;
private:
	hkpRayHitCollector& m_collector;
	const hkpShapeRayCastInput& m_input;
  const hkvDecorationMeshShape *m_decoMeshShape;
};

// Custom compound shape for decoration group. Overrides the raycast method to support alphatested maerials
class hkvDecorationMeshShape : public hkpStaticCompoundShape
{
public:
  // constructor: Prepare additional information for the model
  hkvDecorationMeshShape(VBaseMesh *pRenderMesh, const hkArray<hkUint32>& triangleToKeyMap)
  {
    // build additional model information for the UV lookup
    int iTriCount = pRenderMesh->GetNumOfTriangles();
    VASSERT(iTriCount==triangleToKeyMap.getSize());

    // first find the maximum key index
    m_iKeyCount = 0;
    for (int i=0;i<iTriCount;i++)
      m_iKeyCount = hkvMath::Max(m_iKeyCount, triangleToKeyMap[i]+1);

    // allocate a lookup array for each key
    m_pKeyMapping = new KeyInfo[m_iKeyCount];

    // fill lookup array with data from the render mesh
    int iTriCounter=0;
    VMemoryTempBuffer<1024*64> indexRawBuffer;
    VMemoryTempBuffer<1024*256> vertexRawBuffer(pRenderMesh->GetNumOfVertices()*sizeof(hkvVec2));
    hkvVec2 *pUV = (hkvVec2 *)vertexRawBuffer.GetBuffer();
    VisMBVertexDescriptor_t desc_UV;
    desc_UV.m_iStride = sizeof(hkvVec2);
    desc_UV.m_iTexCoordOfs[0] = 0;
    desc_UV.SetFormatDefaults();
    pRenderMesh->CopyMeshVertices(pUV,desc_UV); // get UV array from all vertices

    for (int iSubMesh=0;iSubMesh<pRenderMesh->GetSubmeshCount();iSubMesh++)
    {
      VBaseSubmesh *pSubMesh = pRenderMesh->GetBaseSubmesh(iSubMesh);
      int iStartIndex, iNumIndices;
      pSubMesh->GetRenderRange(iStartIndex, iNumIndices);
      if (pSubMesh->GetSurface()==NULL || pSubMesh->GetSurface()->GetTransparencyType()==VIS_TRANSP_NONE)
      {
        iTriCounter += iNumIndices/3;
        continue;
      }
      const char *szTextureFile = pSubMesh->GetSurface()->GetBaseTexture();
      if (szTextureFile==NULL)
      {
        iTriCounter += iNumIndices/3;
        continue;
      }
      // load opacity map that corresponds to the material texture
      vHavokOpacityMap *pOpacityMap = (vHavokOpacityMap *)vHavokOpacityMapManager::GetManager().LoadResource(szTextureFile);

      // copy index range (i.e. triangle data) from this submesh
      indexRawBuffer.EnsureCapacity(iNumIndices*sizeof(int));
      unsigned int *pIndices = (unsigned int *)indexRawBuffer.GetBuffer();
      pRenderMesh->CopyMeshIndices32(pIndices,iStartIndex, iNumIndices);
      iNumIndices/=3;
      for (int iTri=0;iTri<iNumIndices;iTri++,iTriCounter++)
      {
        // set array entry at the position of the actual key, because we perform lookup with the key later
        hkUint32 iKey = triangleToKeyMap[iTriCounter];
        KeyInfo &info(m_pKeyMapping[iKey]);
        VASSERT(info.m_spOpacityMap==NULL || info.m_spOpacityMap==pOpacityMap);
        const hkvVec2 &uv0(pUV[pIndices[iTri*3+0]]); // dont add iStartIndex here because we copied array with an offset already
        const hkvVec2 &uv1(pUV[pIndices[iTri*3+1]]);
        const hkvVec2 &uv2(pUV[pIndices[iTri*3+2]]);
        info.m_triangleUV[0].set(uv0.x,uv0.y);
        info.m_triangleUV[1].set(uv1.x,uv1.y);
        info.m_triangleUV[2].set(uv2.x,uv2.y);
        info.m_spOpacityMap = pOpacityMap;
      }
    }
    VASSERT(iTriCounter==iTriCount);
  }

  virtual ~hkvDecorationMeshShape()
  {
    delete[] m_pKeyMapping;
  }

  void castRayWithCollector(const hkpShapeRayCastInput& input, const hkpCdBody& cdBody, hkpRayHitCollector& collector) const
  {
    vHavokDecorationHitCollector collectorWithHoleSupport(this, collector, input);
    hkpStaticCompoundShape::castRayWithCollector(input, cdBody, collectorWithHoleSupport);    
  }

  // a structure that keeps per-key information of the triangle. The parent shape holds an array of m_iKeyCount elements
  struct KeyInfo
  {
    VSmartPtr<vHavokOpacityMap> m_spOpacityMap; ///< the opacity map to perform lookup. Can be NULL if no lookup should be performed (opaque materials)
    hkVector2 m_triangleUV[3];  ///< 3 UV coordinates that correspond to the UV of the triangle
  };

  hkUint32 m_iKeyCount;     ///< number of keys generated
  KeyInfo *m_pKeyMapping;   ///< array of m_iKeyCount elements to perform the lookup
};


void vHavokDecorationHitCollector::addRayHit(const hkpCdBody& cdBody, const hkpShapeRayCastCollectorOutput& hitInfo)
{
	hkpShapeKey key = cdBody.getShapeKey();
	VASSERT(key != HK_INVALID_SHAPE_KEY);

	// we know this shape belongs to the decoration model, because this collector is used for hkvDecorationMeshShape.
  // so we can do the following casts
  hkvDecorationMeshShape *pDecoShape = (hkvDecorationMeshShape *)cdBody.getParent()->getShape();
  hkpTriangleShape *pTriangle = (hkpTriangleShape *)cdBody.getShape();

  // split key into instance and triangle
  int instanceId;
  hkpShapeKey childKey;
  pDecoShape->decomposeShapeKey(key, instanceId, childKey);

  // lookup array and perform opacity testing
  VASSERT((int)childKey<pDecoShape->m_iKeyCount);
  hkvDecorationMeshShape::KeyInfo &triInfo(pDecoShape->m_pKeyMapping[childKey]);
  if (triInfo.m_spOpacityMap!=NULL)
  {
    // calculate barycentric coordinates for UV lookup
    hkVector4 bary;
    hkVector4 hitPointWS; 
    hitPointWS.setInterpolate( m_input.m_from, m_input.m_to, hitInfo.m_hitFraction );

    // Transform triangle vertices to world space
    const hkQsTransform& tr = pDecoShape->getInstances()[instanceId].getTransform();
    hkVector4 tv[3];
    tv[0]._setTransformedPos(tr, pTriangle->getVertex<0>());
    tv[1]._setTransformedPos(tr, pTriangle->getVertex<1>());
    tv[2]._setTransformedPos(tr, pTriangle->getVertex<2>());

    hkcdTriangleUtil::calcBarycentricCoordinates(hitPointWS, tv[0], tv[1], tv[2], bary);

    // convert barycentric to actual texture UV
    hkVector2 hitPointUV;
    hitPointUV.setMul(triInfo.m_triangleUV[2], bary.getComponent<0>()); // using order UV[2],UV[0],UV[1] found through trial and error
    hitPointUV.addMul(bary.getComponent<1>(), triInfo.m_triangleUV[0]);
    hitPointUV.addMul(bary.getComponent<2>(), triInfo.m_triangleUV[1]);
/*
    if (true) // very useful code path for debugging
    {
      hkvVec3 vWSPos;
      vHavokConversionUtils::PhysVecToVisVecWorld(tv[0],vWSPos);
      Vision::Game.DrawCube(vWSPos,1.f);
      vHavokConversionUtils::PhysVecToVisVecWorld(tv[1],vWSPos);
      Vision::Game.DrawCube(vWSPos,1.f);
      vHavokConversionUtils::PhysVecToVisVecWorld(tv[2],vWSPos);
      Vision::Game.DrawCube(vWSPos,1.f);
      vHavokConversionUtils::PhysVecToVisVecWorld(hitPointWS,vWSPos);
      Vision::Game.DrawCube(vWSPos,0.5f);
      static VisScreenMask_cl *pMask = NULL;
      if (pMask==NULL)
      {
        VSmartPtr<vHavokOpacityMapPreview> spPreview = (vHavokOpacityMapPreview *)triInfo.m_spOpacityMap->CreateResourcePreview();
        spPreview->OnActivate();
        pMask = new VisScreenMask_cl();
        pMask->SetTextureObject(spPreview->m_spTexture);
        pMask->SetVisible(TRUE);
        pMask->SetTargetSize(256.f,256.f);
        pMask->SetPos(0,0);
        spPreview->OnDeActivate();
      }
#define SHOW_CROSS(_u,_v,col) \
      {\
        float cx = fmodf(_u,1.f) * 256.f;\
        float cy = fmodf(_v,1.f) * 256.f;\
        Vision::Game.DrawSingleLine2D(cx-3,cy,cx+3,cy,col);\
        Vision::Game.DrawSingleLine2D(cx,cy-3,cx,cy+3,col);\
      }
      SHOW_CROSS(triInfo.m_triangleUV[0].x,triInfo.m_triangleUV[0].y,V_RGBA_YELLOW);
      SHOW_CROSS(triInfo.m_triangleUV[1].x,triInfo.m_triangleUV[1].y,V_RGBA_YELLOW);
      SHOW_CROSS(triInfo.m_triangleUV[2].x,triInfo.m_triangleUV[2].y,V_RGBA_YELLOW);
      VColorRef hitColor = triInfo.m_spOpacityMap->IsOpaqueAtUV(hitPointUV.x, hitPointUV.y) ? V_RGBA_YELLOW : V_RGBA_GREEN;
      SHOW_CROSS(hitPointUV.x,hitPointUV.y,hitColor);
    }
*/
    // transparent pixel? The return without hit
    if (!triInfo.m_spOpacityMap->IsOpaqueAtUV(hitPointUV.x, hitPointUV.y))
      return;
  }
  // propagate to original collector
	m_collector.addRayHit(cdBody, hitInfo);
}


void VHavokDecorationGroup::CreateFromRenderMesh(IVisDecorationGroup_cl &group, bool bUseAlphaTest)
{
  int iDecoCount = group.GetDecorationObjectCount();
 
  // in this version, all instances must have the same model
#if defined(HK_DEBUG_SLOW)
  for ( int i=1; i<iDecoCount; i++ )
  {
    VTerrainDecorationInstance *pInst = group.GetDecorationObject(i);
    VASSERT_MSG(pInst->GetModel() == group.GetDecorationObject(0)->GetModel(), "groups with different models are currently not supported");      
  }
#endif


  VTerrainDecorationInstance *pInstance = group.GetDecorationObject(0);
  IVTerrainDecorationModel *pModel = pInstance->GetModel();
  VBaseMesh *pRenderMesh = pModel->GetCollisionSubObject(0)->GetRenderMesh(); 
  const IVCollisionMesh* pColMesh = pRenderMesh->GetTraceMesh( true, true );

  // Build the hkGeometry for this
  hkGeometry collisionGeometry;
  {
    int iNumColMeshes = hkvMath::Max(pColMesh->GetSubmeshCount(), 1);
    for (int i=0;i<iNumColMeshes;i++)
    {
      vHavokShapeFactory::BuildGeometryFromCollisionMesh(pColMesh, i, hkvMat4::IdentityMatrix(), false, collisionGeometry);
    }
  }

  // Create the mesh shape and compound shape
  hkpStaticCompoundShape *compoundShape;
  hkpBvCompressedMeshShape* meshShape;
  hkpDefaultBvCompressedMeshShapeCinfo cinfo(&collisionGeometry);
  if (bUseAlphaTest)
  {
    hkArray<hkUint32> triangleToKeyMap;
    cinfo.m_triangleIndexToShapeKeyMap = &triangleToKeyMap;
    meshShape = new hkpBvCompressedMeshShape(cinfo);
    compoundShape = new hkvDecorationMeshShape(pRenderMesh, triangleToKeyMap); // with alpha test support
  } else
  {
    meshShape = new hkpBvCompressedMeshShape(cinfo);
    compoundShape = new hkpStaticCompoundShape();
  }

  // For each decoration object
  for (int i = 0; i < iDecoCount; i++)
  {
    VTerrainDecorationInstance *pInstance = group.GetDecorationObject(i);
    IVTerrainDecorationModel *pModel = pInstance->GetModel();

    const float instanceScale = pInstance->GetScaling();

    // Compute base instance transformation
    const hkSimdReal hkInstanceScale = hkSimdReal::fromFloat(instanceScale);

    hkvMat3 normalizedOrientation = pInstance->m_Orientation;
    normalizedOrientation.normalize();

    // Get the model instance transformation.
    hkQsTransform instanceTransform;
    vHavokConversionUtils::VisMatrixToHkQuat(normalizedOrientation, instanceTransform.m_rotation);
    vHavokConversionUtils::VisVecToPhysVecWorld(pInstance->m_vPosition, instanceTransform.m_translation);
    instanceTransform.m_scale.setAll(hkInstanceScale);

    compoundShape->addInstance(meshShape, instanceTransform);
  }

  // Bake the compound shape and create the Havok Rigid Body
  compoundShape->bake();
  hkpRigidBodyCinfo rigidBodyInfo;
  rigidBodyInfo.m_motionType = hkpMotion::MOTION_FIXED;
  rigidBodyInfo.m_collisionFilterInfo = hkpGroupFilter::calcFilterInfo(vHavokPhysicsModule::HK_LAYER_COLLIDABLE_DECORATION);
  rigidBodyInfo.m_shape = compoundShape;
  m_pCompoundRigidBody= new hkpRigidBody(rigidBodyInfo);

  // Cleanup
  meshShape->removeReference();
  compoundShape->removeReference();
}


struct VHavokDecorationGroup_ModelAndScale 
{
  IVTerrainDecorationModel *model;
  float scale;
  int shapeStartIndex;
};

struct VHavokDecorationGroup_ShapeInfo
{
  hkpShape* pShape;
  hkVector4 vLocalOffset;
};


void VHavokDecorationGroup::CreateFromCapsules(IVisDecorationGroup_cl &group)
{
  int iDecoCount = group.GetDecorationObjectCount();

  hkpStaticCompoundShape *compoundShape = new hkpStaticCompoundShape();
  hkArray<VHavokDecorationGroup_ShapeInfo> shapeInfos;

  // Set up a cache of decoration collision capsules
  hkArray<VHavokDecorationGroup_ModelAndScale> capsulesCache;
  hkPointerMultiMap<IVTerrainDecorationModel*, int> modelToCache;
  const float scaleMatchTolerance = 0.05f; // raise this to lower memory by reducing set of shapes

  // Find all unique Model@Scale combinations (hkpStaticCompoundShape only supports scale
  // for hkpConvexVerticesShapes in 2011.2, so have to bake scale here)

  // For each decoration object
  for (int i = 0; i < iDecoCount; i++)
  {
    VTerrainDecorationInstance *pInstance = group.GetDecorationObject(i);
    IVTerrainDecorationModel *pModel = pInstance->GetModel();
    if ( pModel == NULL )
      continue;

    const int iCapsulesCount = pModel->GetCollisionSubObjectCount();
    if ( iCapsulesCount == 0 )
      continue;

    const float instanceScale = pInstance->GetScaling();

    // Is the Model/Scale pair in the cache?
    int iCacheIndex = -1;
    hkPointerMultiMap< IVTerrainDecorationModel*, int >::Iterator iter = modelToCache.findKey(pModel);
    for (; modelToCache.isValid(iter); iter = modelToCache.getNext(iter, pModel))
    {
      const int thisCacheIndex = modelToCache.getValue(iter);
      VASSERT( thisCacheIndex >= 0 && thisCacheIndex < capsulesCache.getSize());
      VHavokDecorationGroup_ModelAndScale& ms = capsulesCache[thisCacheIndex];
      VASSERT( ms.model == pModel );

      if ( hkMath::equal(ms.scale, instanceScale, scaleMatchTolerance) )
      {
        // Model/Scale found in cache!
        iCacheIndex = thisCacheIndex;
        break;
      }
    }

    // Compute base instance transformation
    const hkSimdReal hkInstanceScale = hkSimdReal::fromFloat(instanceScale) * 
      vHavokConversionUtils::GetVision2HavokScaleSIMD();

    hkvMat3 normalizedOrientation = pInstance->m_Orientation;
    normalizedOrientation.normalize();

    // Get the model instance transformation.
    hkQsTransform instanceTransform;
    vHavokConversionUtils::VisMatrixToHkQuat(normalizedOrientation, instanceTransform.m_rotation);
    vHavokConversionUtils::VisVecToPhysVecWorld(pInstance->m_vPosition, instanceTransform.m_translation);
    instanceTransform.setScale(hkVector4::getConstant<HK_QUADREAL_1>());

    // If not found, create a new shape for each collision sub-object
    // and update the capsules cache with the new information.
    int iShapesIndex;
    if ( iCacheIndex < 0 )
    {
      iShapesIndex = shapeInfos.getSize();

      for (int j=0; j<iCapsulesCount; j++)
      {
        const VDecorationCollisionPrimitive &decoCapsuleIn(*pModel->GetCollisionSubObject(j));

        // Compute capsule start and end point
        hkvVec3 vStart = decoCapsuleIn.GetStart();
        hkvVec3 vEnd = decoCapsuleIn.GetEnd();

		hkVector4 vs; vHavokConversionUtils::VisVecToPhysVec_noscale(vStart,vs);        
		hkVector4 ve; vHavokConversionUtils::VisVecToPhysVec_noscale(vEnd,ve);

        vs.mul(hkInstanceScale);
        ve.mul(hkInstanceScale); 

        // vs is relative to the origin of the shape's local space
        // -> translate the shape instance in the compound shape accordingly
        // (more efficient than using a hkpConvexTranslateShape for the sphere)
        hkVector4 vd;
        vd.setSub(ve, vs);
     
        hkReal fRadius = hkReal(VIS2HK_FLOAT_SCALED(decoCapsuleIn.GetRadius() * instanceScale));

        // Havok asserts for capsules with start~=end, so make it a sphere
        hkpShape *pShape;
		    if (vd.lengthSquared<3>() < hkSimdReal::fromFloat(1e-6f)) 
        {
          pShape = new hkpSphereShape(fRadius);
        }
        else
        {
          pShape = new hkpCapsuleShape(hkVector4::getConstant<HK_QUADREAL_0>(), vd, fRadius);
        }

        VHavokDecorationGroup_ShapeInfo shapeInfo;
        shapeInfo.pShape = pShape;
        shapeInfo.vLocalOffset = vs;
        shapeInfos.pushBack(shapeInfo);

        // translate to origin of capsule
        hkQsTransform shapeTransform = instanceTransform;
        vs._setRotatedDir(instanceTransform.m_rotation, vs); // we need vs in world space
        shapeTransform.m_translation.add(vs);

        // Add to compound shape
        compoundShape->addInstance(pShape, shapeTransform);
      }

      // Update cache
      modelToCache.insert(pModel, capsulesCache.getSize());
      VHavokDecorationGroup_ModelAndScale& ms = capsulesCache.expandOne();
      ms.model = pModel;
      ms.scale = instanceScale;
      ms.shapeStartIndex = iShapesIndex;      
    }
    // If found, retrieve the index into the array of shapes from the cache.
    else
    {
      iShapesIndex = capsulesCache[iCacheIndex].shapeStartIndex;
    }

	

    // Add all capsules and their transformation to the compound shape.
      // (translation needs to be computed though)
      const VHavokDecorationGroup_ShapeInfo* const shapeInfoPtr = &shapeInfos[iShapesIndex];
      for (int j = 0; j < iCapsulesCount; j++)
    {
        // translate to origin of capsule
        hkQsTransform shapeTransform = instanceTransform;
        hkVector4 vs;
        vs._setRotatedDir(instanceTransform.m_rotation, shapeInfoPtr[j].vLocalOffset);
        shapeTransform.m_translation.add(vs);

        // Add to compound shape
        compoundShape->addInstance(shapeInfoPtr[j].pShape, shapeTransform);
      }
    }

  // Bake the compound shape and create the Havok Rigid Body
  compoundShape->bake();
  hkpRigidBodyCinfo rigidBodyInfo;
  rigidBodyInfo.m_motionType = hkpMotion::MOTION_FIXED;
  rigidBodyInfo.m_collisionFilterInfo = hkpGroupFilter::calcFilterInfo(vHavokPhysicsModule::HK_LAYER_COLLIDABLE_DECORATION);
  rigidBodyInfo.m_shape = compoundShape;
  m_pCompoundRigidBody= new hkpRigidBody(rigidBodyInfo);

  // Clear
  compoundShape->removeReference();
  for (int s = 0; s < shapeInfos.getSize(); s++)
  {
    shapeInfos[s].pShape->removeReference();
  }	
}


VHavokDecorationGroup::VHavokDecorationGroup(IVisDecorationGroup_cl &group)
{
  int iDecoCount = group.GetDecorationObjectCount();

#if defined(HK_DEBUG_SLOW)
  int iCollisionObjectsCount = 0;
  for ( int i=0; i<iDecoCount; i++ )
  {
    VTerrainDecorationInstance *pInst = group.GetDecorationObject(i);
    if ( pInst->GetModel() != NULL )
      iCollisionObjectsCount += pInst->GetModel()->GetCollisionSubObjectCount();
  }
  VASSERT_MSG(iCollisionObjectsCount>0, "These objects should only be created if the group has collision information.");
#endif

  IVTerrainDecorationModel *pFirstModel = group.GetDecorationObject(0)->GetModel();
  if (pFirstModel->GetCollisionSubObjectCount()==1)
  {
    VDecorationCollisionPrimitive &prim(*pFirstModel->GetCollisionSubObject(0));
    if (prim.GetCollisionType()==VDecorationCollisionPrimitive::VDecorationCollision_RenderMesh)
    {
      CreateFromRenderMesh(group, false);
      return;
    }
    if (prim.GetCollisionType()==VDecorationCollisionPrimitive::VDecorationCollision_RenderMeshAlphaTest)
    {
      CreateFromRenderMesh(group, true);
      return;
    }
  }

  // in all other cases:
  CreateFromCapsules(group);
}


VHavokDecorationGroup::~VHavokDecorationGroup()
{
  CommonDeinit();
}

void VHavokDecorationGroup::CommonDeinit()
{
  m_pCompoundRigidBody->removeReference();
  m_pCompoundRigidBody = HK_NULL;
}

void VHavokDecorationGroup::DisposeObject()
{
  CommonDeinit();
}

hkpRigidBody* VHavokDecorationGroup::GetHkRigidBody()
{
  return (hkpRigidBody*)m_pCompoundRigidBody;
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
