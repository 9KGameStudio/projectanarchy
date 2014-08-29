/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisionTranslucencySorter.hpp

#ifndef DEFINE_VISIONTRANSLUCENCYSORTER
#define DEFINE_VISIONTRANSLUCENCYSORTER


/// \brief
///   Pure interface to render a list of certain objects (e.g. entities)
///
class IVisInterleavedTranslucencyRenderer
{
public:
  VISION_APIFUNC virtual ~IVisInterleavedTranslucencyRenderer()
  {}

  /// \brief
  ///   This method is called when interleaved translucency rendering is active and one or more objects of a certain kind should be
  ///   rendered. The number of objects depends of the sorting order. E.g. when close to the camera is one entity followed by two
  ///   static geometry instances and a second entity is behind those objects, this function is called three times; one time for the
  ///   closest entity, one time for the two static geometry instances behind and another time for the farest entity. The renderer that
  ///   implements this method is responsible to render all given instances (that were added by IVisTranslucencySorter::AddInstance)
  ///   and has to take care that the render brackets after rendering are in the same state than before this method is called (e.g.
  ///   each BeginEntityRendering requires one EndEntityRendering call etc.).
  ///
  /// \param pVisCollector
  ///   The visibility collector currently in use.
  ///
  /// \param pInstances
  ///   Points to the pointers of the instances to render; the type needs to be known be the specific renderer.
  ///
  /// \param iCount
  ///   Number of instances to render.
  ///
  /// \param pUserData
  ///   Optional user data that can be set when adding an instance by IVisTranslucencySorter::AddInstance (default is NULL)
  ///
  /// \sa IVisTranslucencySorter::AddInstance()
  VISION_APIFUNC virtual void RenderBatch(IVisVisibilityCollector_cl* pVisCollector, void** pInstances, int iCount, void* pUserData) = 0;
};


/// \brief
///   Holds an object instance together with its renderer, sorting distance and (optional) user data
///
struct VSortableObjectRef_t
{
  inline void Set(IVisInterleavedTranslucencyRenderer* pRenderer, void* pInstance,  float fSortingDistanceSqr, void* pUserData)
  {
    m_pRenderer = pRenderer;
    m_pInstance = pInstance;
    m_pUserData = pUserData;
    m_fSortingDistanceSqr = fSortingDistanceSqr;
  }

  IVisInterleavedTranslucencyRenderer* m_pRenderer; ///< Individual renderer that is responsible to render a certain kind of instance type
  void* m_pInstance;                                ///< Pointer to the instance to render
  void* m_pUserData;                                ///< User data that can be defined optionally
  float m_fSortingDistanceSqr;                      ///< Squared sorting distance used to sort all kind of instances from far to near
};


/// \brief
///   Abstract class that can be derived by concrete interleaved tranlucency sorters. See derived class for further information.
///
/// \sa VisionTranslucencySorter
///
class IVisTranslucencySorter : public VRefCounter, public VisTypedEngineObject_cl
{
public:
  VISION_APIFUNC IVisTranslucencySorter();
  VISION_APIFUNC virtual ~IVisTranslucencySorter();

public:

  /// \brief
  ///   Adds a translucent object instance to the rendering queue. The purpose of this method is to collect all kind of translucent
  ///   renderable object instances, which are finally sorted by distance (far to near) and rendered in batches through the callback
  ///   mechanisum given by the individual renderers.
  ///
  /// \note
  ///   It's only allowed to add instances after OnDoVisibilityDetermination() and before OnRender(). If you try to add an instance
  ///   at any other point, the function will trigger an assertion.
  ///
  /// \param renderer
  ///   The specific renderer that can handle the added instance.
  ///
  /// \param pInstance
  ///   Pointer to the object instance to render.
  ///
  /// \param fSortingDistSqr
  ///   The squared distance used for sorting. E.g. the distance from the camera to the middle of the object instance.
  ///
  /// \param pUserData
  ///   Specific user data that are pass-through the IVisInterleavedTranslucencyRenderer::RenderBatch method (optional).
  ///
  /// \sa IVisInterleavedTranslucencyRenderer::RenderBatch()
  inline void AddInstance(IVisInterleavedTranslucencyRenderer& renderer, void* pInstance,  float fSortingDistSqr, void* pUserData = NULL)
  {
    VASSERT_MSG(m_iInstanceCount >= 0, "IVisTranslucencySorter::AddInstance: Not allowed to add translucent objects to sort at this point!");
    m_Instances[m_iInstanceCount++].Set(&renderer, pInstance, fSortingDistSqr, pUserData);
  }

  /// \brief
  ///   Returns the object instances collected so far.
  inline const VSortableObjectRef_t* GetInstances(int& iCount)
  {
    iCount = m_iInstanceCount;
    return m_Instances.GetDataPtr();
  }

  /// \brief
  ///   Sets the tag filter that specifics whether to render tagged objects or not (currently only used by the mobile forward renderer).
  ///
  /// \param eFilter
  ///   The tag filter to apply
  inline void SetTagFilter(VTagFilter_e eFilter)
  {
    m_eTagFilter = eFilter;
  }
  
  /// \brief
  ///   Returns the current tag filter.
  inline VTagFilter_e GetTagFilter() const
  {
    return m_eTagFilter;
  }

public:
  /// \brief
  ///   Called by the visibility collector in IVisVisibilityCollector_cl::OnDoVisibilityDetermination when visibility collection is about
  ///   to begin.
  VISION_APIFUNC virtual void OnDoVisibilityDetermination() = 0;

  /// \brief
  ///   Called by the visibility collector when visibility collection is in progress. Note this function may be called from a spearate thread,
  ///   so the behaviour of this function needs to be thread-safe.
  ///
  /// \param pVisCollector
  ///   The visibility collector that is calling this method.
  VISION_APIFUNC virtual void OnPerformVisibilityDetermination(IVisVisibilityCollector_cl* pVisCollector) = 0;

  /// \brief
  ///   Called by the visibility collector when visibility collection is finished. Generally used to add vision's default object instances
  ///   like entities, static geometry instances, mesh buffer objects and particle groups to the render queue.
  ///
  /// \param pVisCollector
  ///   The visibility collector that is calling this method.
  VISION_APIFUNC virtual void OnPostProcessVisibilityResults(IVisVisibilityCollector_cl* pVisCollector) = 0;

  /// \brief
  ///   This method is responsible to sort all collected translucent object instances far to near and render it in batches. It is called
  ///   either by the class VPostProcessTranslucencies (when using forward or deferred rendering) or by the individual renderer loop
  ///   implementation.
  ///
  /// \note
  ///   This function triggers all callbacks beginning from VRH_POST_TRANSPARENT_PASS_GEOMETRY to VRH_TRANSLUCENT_VOLUMES but it's up to the user
  ///   to not ignore these callbacks and not to render something in case interleaved translucency rendering is active since otherwise geoemtry
  ///   might be rendered twice.
  ///
  /// \param pVisCollector
  ///   The visibility collector that is currently active.
  ///
  /// \param bTriggerCallbacks
  ///   Whether to trigger the render hook callbacks or not.
  VISION_APIFUNC virtual void OnRender(IVisVisibilityCollector_cl* pVisCollector, bool bTriggerCallbacks) = 0;

protected:
  DynObjArray_cl<VSortableObjectRef_t> m_Instances;                       ///< Collection of renderable instances
  int m_iInstanceCount;                                                   ///< Instance count
  VTagFilter_e m_eTagFilter;                                              ///< Tag filter

public:
  VISION_APIDATA static VisCallback_cl OnAddInterleavedSortingInstances;  ///< Callback that can be used by custom objects to add renderable object instances to sort

public:
  V_DECLARE_DYNAMIC_DLLEXP(IVisTranslucencySorter, VISION_APIDATA)
};

typedef VSmartPtr<IVisTranslucencySorter> IVisTranslucencySorterPtr;


/// \brief
///   Callback data object used in callbacks that are triggered when translucenct objects that shall be rendered interleaved together
///   with other translucent objects need to be added to the rendering queue. Note this callback is called from the visibility collector
///   within a separate thread, so it is important that the callback function behaves thread-safe.
///
/// \sa IVisTranslucencySorter::OnAddInterleavedSortingInstance
///
class VisTranslucencySortingData : public IVisCallbackDataObject_cl
{
public:
  inline VisTranslucencySortingData(VCallback* pSender, IVisTranslucencySorter* pSorter, IVisVisibilityCollector_cl* pVisCollector) :
    IVisCallbackDataObject_cl(pSender),
    m_pSorter(pSorter),
    m_pVisCollector(pVisCollector)
  {}

  IVisTranslucencySorter* m_pSorter;           ///< Pointer to the sorter to which all translucenct objects that need to be sorted interleaved should be added to
  IVisVisibilityCollector_cl* m_pVisCollector; ///< Pointer to visibility collector that can be used for visibility determination
};


/// \brief
///   Vision's default translucency sorter class that is used to handle and render interleaved translucenct objects of certain objects like static sub meshes,
///   entities, particle system and custom user objects.
///
///   Note that a custom object that is normally rendered within a render hook needs to register a renderer (see IVisInterleavedTranslucencyRenderer) next to
///   the renderable objects and sorting distances. Since there are two renderer modes present in renderer nodes - non-interleaved and interleaved transparency
///   sorting, it is important that the render hook only renders the object when non-interleaved sorting is used (to avoid double rendering in case the object
///   supports interleaved rendering). When interleaved sorting is used and a translucent object should be rendered in correct order from far to near, it is
///   necassary to provide a renderer and to hook into the callback OnAddInterleavedSortingInstance (see IVisTranslucencySorter) so that the object(s) to
///   render are taken into account while interleaved z-sorted rendering is used.
class VisionTranslucencySorter : public IVisTranslucencySorter
{
public:
  VISION_APIFUNC VisionTranslucencySorter();
  VISION_APIFUNC virtual ~VisionTranslucencySorter();

public:
  /// \copydoc IVisTranslucencySorter::OnDoVisibilityDetermination
  VISION_APIFUNC virtual void OnDoVisibilityDetermination() HKV_OVERRIDE;

  /// \copydoc IVisTranslucencySorter::OnPerformVisibilityDetermination
  VISION_APIFUNC virtual void OnPerformVisibilityDetermination(IVisVisibilityCollector_cl* pVisCollector) HKV_OVERRIDE;

  /// \copydoc IVisTranslucencySorter::OnPostProcessVisibilityResults
  VISION_APIFUNC virtual void OnPostProcessVisibilityResults(IVisVisibilityCollector_cl* pVisCollector) HKV_OVERRIDE;

  /// \copydoc IVisTranslucencySorter::OnRender
  VISION_APIFUNC virtual void OnRender(IVisVisibilityCollector_cl* pVisCollector, bool bTriggerCallbacks) HKV_OVERRIDE;

protected:
  /// \brief
  ///   Internal method that adds all visibile entities that are rendered in transparent pass to the collection to sort.
  ///
  /// \param vCamPos
  ///   Current visibility collector's camera position.
  ///
  /// \param pEntities
  ///   Collection of entity instances to add.
  void AddEntities(const hkvVec3& vCamPos, const VisEntityCollection_cl* pEntities);

  /// \brief
  ///   Internal method that adds all visibile static geometry instances that are rendered in transparent pass to the collection to sort.
  ///
  /// \param vCamPos
  ///   Current visibility collector's camera position.
  ///
  /// \param pStaticGeo
  ///   Collection of static geometroy instances to add.
  void AddStaticGeometry(const hkvVec3& vCamPos, const VisStaticGeometryInstanceCollection_cl* pStaticGeo);

  /// \brief
  ///   Internal method that adds all visibilily objects (mesh buffer objects and particle groups) that are rendered in transparent pass to the collection to sort.
  ///
  /// \param vCamPos
  ///   Current visibility collector's camera position.
  ///
  /// \param pVisObjectColl
  ///   Collection of visibility object instances to add.
  void AddVisibileVisibilityObjects(const hkvVec3& vCamPos, const VisVisibilityObjectCollection_cl* pVisObjectColl);

public:
  V_DECLARE_DYNAMIC_DLLEXP(VisionTranslucencySorter, VISION_APIDATA)
};

typedef VSmartPtr<VisionTranslucencySorter> VisionTranslucencySorterPtr;


#endif //DEFINE_VISIONTRANSLUCENCYSORTER

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
