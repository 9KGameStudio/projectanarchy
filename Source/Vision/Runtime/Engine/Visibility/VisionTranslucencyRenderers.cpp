/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/Engine/Engine.hpp> 
#include <Vision/Runtime/Engine/Visibility/VisionTranslucencyRenderers.hpp>


/////////////////////////////////////////////////////////////////////
// VisTranslucencyEntityRenderer class
/////////////////////////////////////////////////////////////////////
VisTranslucencyEntityRenderer VisTranslucencyEntityRenderer::s_Instance;


VisTranslucencyEntityRenderer::VisTranslucencyEntityRenderer()
{}

VisTranslucencyEntityRenderer::~VisTranslucencyEntityRenderer()
{}

VisTranslucencyEntityRenderer& VisTranslucencyEntityRenderer::GlobalInstance()
{
  return s_Instance;
}

void VisTranslucencyEntityRenderer::RenderBatch(IVisVisibilityCollector_cl* pVisCollector, void** pInstances, int iCount, void* pUserData)
{
  INSERT_PERF_MARKER_SCOPE("VisTranslucencyEntityRenderer::RenderBatch");
  VisDrawCallInfo_t SurfaceShaderList[RLP_MAX_ENTITY_SURFACESHADERS];

  const VTagFilter_e eTagFilter = pVisCollector->GetInterleavedTranslucencySorter()->GetTagFilter();

  Vision::RenderLoopHelper.BeginEntityRendering();
  for (int i = 0; i < iCount; i++)
  {
    VisBaseEntity_cl* pEntity = (VisBaseEntity_cl*)pInstances[i];

    if (eTagFilter == VTF_IGNORE_TAGGED_ENTRIES && pEntity->IsTagged())
      continue;
 
    if (pEntity->HasShadersForPass(VPT_TransparentPass))
    {
      // Get a list of the corresponding pre or post basepass surface shaders
      VisShaderSet_cl* pShaderSet = pEntity->GetActiveShaderSet();
      if (pShaderSet == NULL)
        continue;

      int iNumSurfaceShaders = pShaderSet->GetShaderAssignmentList(SurfaceShaderList, VPT_TransparentPass, RLP_MAX_ENTITY_SURFACESHADERS);
      VASSERT(iNumSurfaceShaders < RLP_MAX_ENTITY_SURFACESHADERS);
      if (iNumSurfaceShaders == 0)
        continue;
      
      VDynamicMesh* pMesh = pEntity->GetMesh();

      // If the model has lit surfaces, and if the shaders makes use of the lighting information, we need to set up the global lights.
      if (pMesh != NULL && pMesh->HasLitSurfaces() && 
        (pShaderSet->GetCombinedTrackingMask() & (VSHADER_TRACKING_LIGHTGRID_PS|VSHADER_TRACKING_LIGHTGRID_GS|VSHADER_TRACKING_LIGHTGRID_VS)) )
      {
         Vision::RenderLoopHelper.TrackLightGridInfo(pEntity);
      }
      // Render the entity with the surface shader list
      Vision::RenderLoopHelper.RenderEntityWithSurfaceShaderList(pEntity, iNumSurfaceShaders, SurfaceShaderList);
    }
  }
  Vision::RenderLoopHelper.EndEntityRendering();
}


/////////////////////////////////////////////////////////////////////
// VisTranslucencyStaticGeometryRenderer class
/////////////////////////////////////////////////////////////////////
VisTranslucencyStaticGeometryRenderer VisTranslucencyStaticGeometryRenderer::s_Instance;

VisTranslucencyStaticGeometryRenderer::VisTranslucencyStaticGeometryRenderer() :
  m_StaticGeoCollection(0, 1024)
{}

VisTranslucencyStaticGeometryRenderer::~VisTranslucencyStaticGeometryRenderer()
{}

VisTranslucencyStaticGeometryRenderer& VisTranslucencyStaticGeometryRenderer::GlobalInstance()
{
  return s_Instance;
}

void VisTranslucencyStaticGeometryRenderer::RenderBatch(IVisVisibilityCollector_cl* pVisCollector, void** pInstances, int iCount, void* pUserData)
{
  INSERT_PERF_MARKER_SCOPE("VisTranslucencyStaticGeometryRenderer::RenderBatch");

  m_StaticGeoCollection.EnsureSize(1024);
  m_StaticGeoCollection.Clear();

  const VTagFilter_e eTagFilter = pVisCollector->GetInterleavedTranslucencySorter()->GetTagFilter();

  for (int i = 0; i < iCount; i++)
  {
    VisStaticGeometryInstance_cl* pGeoInst = (VisStaticGeometryInstance_cl*)pInstances[i];

    if (eTagFilter == VTF_IGNORE_TAGGED_ENTRIES && pGeoInst->IsTagged())
      continue;

    m_StaticGeoCollection.AppendEntryFast(pGeoInst);
  }

  Vision::RenderLoopHelper.RenderStaticGeometrySurfaceShaders(m_StaticGeoCollection, VPT_TransparentPass);
}


/////////////////////////////////////////////////////////////////////
// VisTranslucencyMeshBufferObjectRenderer class
/////////////////////////////////////////////////////////////////////
VisTranslucencyMeshBufferObjectRenderer VisTranslucencyMeshBufferObjectRenderer::s_Instance;

VisTranslucencyMeshBufferObjectRenderer::VisTranslucencyMeshBufferObjectRenderer() :
  m_MeshBufferObjectCollection(0, 1024)
{}

VisTranslucencyMeshBufferObjectRenderer::~VisTranslucencyMeshBufferObjectRenderer()
{}

VisTranslucencyMeshBufferObjectRenderer& VisTranslucencyMeshBufferObjectRenderer::GlobalInstance()
{
  return s_Instance;
}

void VisTranslucencyMeshBufferObjectRenderer::RenderBatch(IVisVisibilityCollector_cl* pVisCollector, void** pInstances, int iCount, void* pUserData)
{
  INSERT_PERF_MARKER_SCOPE("VisTranslucencyMeshBufferObjectRenderer::RenderBatch");

  m_MeshBufferObjectCollection.EnsureSize(1024);
  m_MeshBufferObjectCollection.Clear();

  for (int i = 0; i < iCount; i++)
  {
    VisMeshBufferObject_cl* pGeoInst = (VisMeshBufferObject_cl*)pInstances[i];
    m_MeshBufferObjectCollection.AppendEntryFast(pGeoInst);
  }

  Vision::RenderLoopHelper.RenderMeshBufferObjects(m_MeshBufferObjectCollection);
}


/////////////////////////////////////////////////////////////////////
// VisTranslucencyParticleGroupRenderer class
/////////////////////////////////////////////////////////////////////
VisTranslucencyParticleGroupRenderer VisTranslucencyParticleGroupRenderer::s_Instance;

VisTranslucencyParticleGroupRenderer::VisTranslucencyParticleGroupRenderer() :
  m_ParticleGroupCollection(0, 1024)
{}

VisTranslucencyParticleGroupRenderer::~VisTranslucencyParticleGroupRenderer()
{}

VisTranslucencyParticleGroupRenderer& VisTranslucencyParticleGroupRenderer::GlobalInstance()
{
  return s_Instance;
}

void VisTranslucencyParticleGroupRenderer::RenderBatch(IVisVisibilityCollector_cl* pVisCollector, void** pInstances, int iCount, void* pUserData)
{
  INSERT_PERF_MARKER_SCOPE("VisTranslucencyParticleGroupRenderer::RenderBatch");

  m_ParticleGroupCollection.EnsureSize(1024);
  m_ParticleGroupCollection.Clear();

  for (int i = 0; i < iCount; i++)
  {
    VisParticleGroup_cl* pGeoInst = (VisParticleGroup_cl*)pInstances[i];
    m_ParticleGroupCollection.AppendEntryFast(pGeoInst);
  }

  // Also sort groups here (note particle groups are also using other sorting criteria like a sorting key to prevent z-fighting)
  VisParticleGroup_cl::GetParticleGroupSorter().SortGroups(m_ParticleGroupCollection);

  Vision::RenderLoopHelper.RenderParticleSystems(&m_ParticleGroupCollection);
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
