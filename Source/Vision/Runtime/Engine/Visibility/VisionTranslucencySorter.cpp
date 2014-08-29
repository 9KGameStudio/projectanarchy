/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/Engine/Engine.hpp> 
#include <Vision/Runtime/Engine/Visibility/VisionTranslucencySorter.hpp>
#include <Vision/Runtime/Engine/Visibility/VisionVisibilityCollector.hpp>
#include <Vision/Runtime/Engine/Visibility/VisionTranslucencyRenderers.hpp>


/////////////////////////////////////////////////////////////////////////////
// IVisTranslucencySorter interface
/////////////////////////////////////////////////////////////////////////////
V_IMPLEMENT_DYNAMIC(IVisTranslucencySorter, VisTypedEngineObject_cl, Vision::GetEngineModule());

VisCallback_cl VISION_ALIGN(16) IVisTranslucencySorter::OnAddInterleavedSortingInstances;


IVisTranslucencySorter::IVisTranslucencySorter() :
  m_Instances(2048),
  m_iInstanceCount(-1),
  m_eTagFilter(VTF_IGNORE_NONE)
{}

IVisTranslucencySorter::~IVisTranslucencySorter()
{}


/////////////////////////////////////////////////////////////////////////////
// VisionTranslucencySorter class
/////////////////////////////////////////////////////////////////////////////
V_IMPLEMENT_DYNAMIC(VisionTranslucencySorter, IVisTranslucencySorter, Vision::GetEngineModule());

VisionTranslucencySorter::VisionTranslucencySorter()
{}

VisionTranslucencySorter::~VisionTranslucencySorter()
{}

void VisionTranslucencySorter::OnDoVisibilityDetermination()
{
  m_iInstanceCount = 0;
}

void VisionTranslucencySorter::OnPerformVisibilityDetermination(IVisVisibilityCollector_cl* pVisCollector)
{
  // Call OnAddInterleavedSortingInstances callback to grab all objects that were normally rendered by render hook callbacks
  VisTranslucencySortingData dataObject(&OnAddInterleavedSortingInstances, this, pVisCollector);
  OnAddInterleavedSortingInstances.TriggerCallbacks(&dataObject);
}

void VisionTranslucencySorter::OnPostProcessVisibilityResults(IVisVisibilityCollector_cl* pVisCollector)
{
  // add common objects like entities, static submeshes etc
  hkvVec3 vCamPos = pVisCollector->GetSourceObject()->GetPosition();

  bool bIR = false;
  if (Vision::Renderer.GetCurrentRendererNode() != NULL)
    bIR = (Vision::Renderer.GetCurrentRendererNode()->GetReferenceContext()->GetUsageHint() & VIS_CONTEXTUSAGEFLAG_INFRARED) != 0;

  if (!bIR)
  {
    AddEntities(vCamPos, pVisCollector->GetVisibleEntitiesForPass(VPT_TransparentPass));
    AddStaticGeometry(vCamPos, pVisCollector->GetVisibleStaticGeometryInstancesForPass(VPT_TransparentPass));
  }

  AddVisibileVisibilityObjects(vCamPos, pVisCollector->GetVisibleVisObjects());
}

void VisionTranslucencySorter::AddEntities(const hkvVec3& vCamPos, const VisEntityCollection_cl* pEntities)
{
  const unsigned int iCount = pEntities->GetNumEntries();
  for (unsigned int i = 0; i < iCount; i++)
  {
    VisBaseEntity_cl* pEntity = pEntities->GetEntry(i);
    VASSERT(pEntity != NULL);

    AddInstance(VisTranslucencyEntityRenderer::GlobalInstance(), (void*)pEntity, pEntity->GetPosition().getDistanceToSquared(vCamPos));
  }
}

void VisionTranslucencySorter::AddStaticGeometry(const hkvVec3& vCamPos, const VisStaticGeometryInstanceCollection_cl* pStaticGeoColl)
{
  const unsigned int iCount = pStaticGeoColl->GetNumEntries();
  for (unsigned int i = 0; i < iCount; i++)
  {
    VisStaticGeometryInstance_cl* pStaticGeo = pStaticGeoColl->GetEntry(i);
    VASSERT(pStaticGeo != NULL);

    float fSortingDistSqr;
    if (pStaticGeo->GetGeometryType() == STATIC_GEOMETRY_TYPE_MESHINSTANCE)
    {
      VisStaticSubmeshInstance_cl* pSubmeshInst = (VisStaticSubmeshInstance_cl*)pStaticGeo;
      
      hkvVec3 vPos = pSubmeshInst->GetMeshInstance()->GetTransform().transformPosition(pSubmeshInst->GetSubmesh()->GetBoundingBox().getCenter());
      fSortingDistSqr = vCamPos.getDistanceToSquared(vPos);
//      Vision::Game.DrawBoundingBox(hkvAlignedBBox(vPos - hkvVec3(0.5f), vPos + hkvVec3(0.5f)));
    }
    else /*if (pStaticGeo->GetGeometryType() == STATIC_GEOMETRY_TYPE_TERRAIN)*/
    {
      // terrain is always opaque and no chance to handle custom geo instances here, so assign always a sorting value of 0
      fSortingDistSqr = 0.0f;
    }

    AddInstance(VisTranslucencyStaticGeometryRenderer::GlobalInstance(), (void*)pStaticGeo, fSortingDistSqr);
  }
}

void VisionTranslucencySorter::AddVisibileVisibilityObjects(const hkvVec3& vCamPos, const VisVisibilityObjectCollection_cl* pVisObjectColl)
{
  if (pVisObjectColl == NULL || pVisObjectColl->GetNumEntries() == 0)
    return;

  VisRenderContext_cl* pContext = VisRenderContext_cl::GetCurrentContext();
  int iRenderFlags = pContext->GetRenderFilterMask();

  // Go through all visible visibility objects and add their particle systems/mesh buffer objects to the respective collections
  const int iNumEntries = pVisObjectColl->GetNumEntries();
  VisVisibilityObject_cl** pVisObjects = pVisObjectColl->GetDataPtr();

  for (int i = 0; i < iNumEntries; i++, pVisObjects++)
  {
    const int iNumObjects = (*pVisObjects)->GetObjectCount();
    VisObject3D_cl** pObjects = (*pVisObjects)->GetObjectList();
    for (int j = 0; j < iNumObjects; j++, pObjects++)
    {
      if ((*pObjects)->GetSceneElementType() == VIS_SCENEELEMENTTYPE_PARTICLESYSTEM)
      {
        VisParticleGroup_cl* pGroup = vstatic_cast<VisParticleGroup_cl *>(*pObjects);
        if ((pGroup->GetVisibleBitmask() & iRenderFlags) == 0)
          continue;

        int iOrder = pGroup->GetRenderOrder();
        if (iOrder >= VRH_PRE_TRANSPARENT_PASS_GEOMETRY && iOrder <= VRH_TRANSLUCENT_VOLUMES)
        {
          float fDistSqr = pGroup->GetInterleavedSortingDistanceSqr();
          if (fDistSqr < 0.0f)
          {
            if (pGroup->GetRenderState().IsFlagSet(RENDERSTATEFLAG_ALWAYSVISIBLE))
              fDistSqr = 0.0f;
            else
              fDistSqr = pGroup->GetPosition().getDistanceToSquared(vCamPos);
          }

          AddInstance(VisTranslucencyParticleGroupRenderer::GlobalInstance(), (void*)pGroup, fDistSqr);
        }
      }
      else if ((*pObjects)->GetSceneElementType() == VIS_SCENEELEMENTTYPE_MESHBUFFEROBJECT)
      {
        VisMeshBufferObject_cl* pMBO = vstatic_cast<VisMeshBufferObject_cl *>(*pObjects);

        if ((pMBO->GetVisibleBitmask() & iRenderFlags) == 0)
          continue;

        int iOrder = pMBO->GetOrder();
        if (iOrder >= VRH_PRE_TRANSPARENT_PASS_GEOMETRY && iOrder <= VRH_TRANSLUCENT_VOLUMES)
          AddInstance(VisTranslucencyMeshBufferObjectRenderer::GlobalInstance(), (void*)pMBO, pMBO->GetPosition().getDistanceToSquared(vCamPos));
      }
      else
      {
        VASSERT(FALSE);
      }
    }
  }

  // In order to maintain backwards compatibility, we also add all particle groups / mesh buffer objects WITHOUT visibility
  // objects. Ideally, you should have all particle groups/mesh buffer objects tied to visibility objects. In such cases, you
  // can simply skip the following step and save a bit of time that way.
  const int iGroupCount = VisParticleGroup_cl::ElementManagerGetSize();
  for (int i = 0; i < iGroupCount; i++)
  {
    VisParticleGroup_cl* pGroup = VisParticleGroup_cl::ElementManagerGet(i);
    if (pGroup == NULL || pGroup->GetVisibilityObject() != NULL || (pGroup->GetVisibleBitmask() & iRenderFlags) == 0)
      continue;

    int iOrder = pGroup->GetRenderOrder();
    if (iOrder >= VRH_PRE_TRANSPARENT_PASS_GEOMETRY && iOrder <= VRH_TRANSLUCENT_VOLUMES)
    {
      float fDistSqr = pGroup->GetInterleavedSortingDistanceSqr();
      if (fDistSqr < 0.0f)
      {
        if (pGroup->GetRenderState().IsFlagSet(RENDERSTATEFLAG_ALWAYSVISIBLE))
          fDistSqr = 0.0f;
        else
          fDistSqr = pGroup->GetPosition().getDistanceToSquared(vCamPos);
      }

      AddInstance(VisTranslucencyParticleGroupRenderer::GlobalInstance(), (void*)pGroup, fDistSqr);
    }
  }

  const int iMBOCount = VisMeshBufferObject_cl::ElementManagerGetSize();
  for (int i = 0; i < iMBOCount; i++)
  {
    VisMeshBufferObject_cl* pMBO = VisMeshBufferObject_cl::ElementManagerGet(i);
    if (pMBO == NULL || pMBO->GetVisibilityObject() != NULL || (pMBO->GetVisibleBitmask() & iRenderFlags) == 0)
      continue;

    int iOrder = pMBO->GetOrder();
    if (iOrder >= VRH_PRE_TRANSPARENT_PASS_GEOMETRY && iOrder <= VRH_TRANSLUCENT_VOLUMES)
      AddInstance(VisTranslucencyMeshBufferObjectRenderer::GlobalInstance(), (void*)pMBO, pMBO->GetPosition().getDistanceToSquared(vCamPos));
  }
}

static int SortObjectRefsByDistance(const void* a, const void* b)
{
  const float fDistA = ((VSortableObjectRef_t*)a)->m_fSortingDistanceSqr;
  const float fDistB = ((VSortableObjectRef_t*)b)->m_fSortingDistanceSqr;
 
  if (fDistA > fDistB) return -1;
  if (fDistA < fDistB) return 1;
  return 0;
}

void VisionTranslucencySorter::OnRender(IVisVisibilityCollector_cl* pVisCollector, bool bTriggerCallbacks)
{
  VisRenderHookDataObject_cl data(&Vision::Callbacks.OnRenderHook, VRH_PRE_TRANSPARENT_PASS_GEOMETRY);
  if (bTriggerCallbacks)
  {
    // Trigger render hook callbacks before interleaved sorted rendering
    for (; data.m_iEntryConst <= VRH_POST_TRANSPARENT_PASS_GEOMETRY; data.m_iEntryConst = data.m_iEntryConst << 1)
     Vision::Callbacks.OnRenderHook.TriggerCallbacks(&data);
  }

  // and finally sort all added objects by its sorting distance
  if (m_iInstanceCount > 1)
    qsort(m_Instances.GetDataPtr(), m_iInstanceCount, sizeof(VSortableObjectRef_t), SortObjectRefsByDistance);

  // Disable sorting of mesh buffer objects since the collection is already sorted
  const BOOL bOldSubOrderingState = VisMeshBufferObject_cl::GetEnableSubOrderSorting();
  VisMeshBufferObject_cl::SetEnableSubOrderSorting(FALSE);

  {
    INSERT_PERF_MARKER_SCOPE("InterleavedTransparencyPass");

    // Process the list of all translucent objects
    IVisInterleavedTranslucencyRenderer* pLastRenderer = NULL;
    void* pLastUserData = NULL;

    VSortableObjectRef_t* pObjRef = m_Instances.GetDataPtr();
    void* pObjList[1024];

    int iBatchSize = 0;
    const int iCount = m_iInstanceCount;

    for (int i = 0; i < iCount; i++, pObjRef++)
    {
      if (pObjRef->m_pRenderer != pLastRenderer || pObjRef->m_pUserData != pLastUserData || iBatchSize == 1024)
      {
        if (iBatchSize > 0)
          pLastRenderer->RenderBatch(pVisCollector, pObjList, iBatchSize, pLastUserData);
        iBatchSize = 0;
        pLastRenderer = pObjRef->m_pRenderer;
        pLastUserData = pObjRef->m_pUserData;
      }

      pObjList[iBatchSize++] = pObjRef->m_pInstance;
    }

    if (iBatchSize > 0)
      pLastRenderer->RenderBatch(pVisCollector, pObjList, iBatchSize, pLastUserData);
  }

  // Reset sorting state of mesh buffer objects
  VisMeshBufferObject_cl::SetEnableSubOrderSorting(bOldSubOrderingState);

  if (bTriggerCallbacks)
  {
    // Trigger render hook callbacks after interleaved sorted rendering
    for (; data.m_iEntryConst <= VRH_TRANSLUCENT_VOLUMES; data.m_iEntryConst = data.m_iEntryConst << 1)
      Vision::Callbacks.OnRenderHook.TriggerCallbacks(&data);
  }

  m_iInstanceCount = -1; // forbid to add translucenct instances after this point (until next frame on OnDoVisibilityDetermination)
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
