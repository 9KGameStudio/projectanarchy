/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisionTranslucencyRenderers.hpp

#ifndef DEFINE_VISIONTRANSLUCENCYRENDERERS
#define DEFINE_VISIONTRANSLUCENCYRENDERERS

#include <Vision/Runtime/Engine/Visibility/VisionTranslucencySorter.hpp>


/// \brief
///   Implements IVisInterleavedTranslucencyRenderer that renders a list of entities in transparent mode
class VisTranslucencyEntityRenderer : public IVisInterleavedTranslucencyRenderer
{
public:
  VISION_APIFUNC VisTranslucencyEntityRenderer();
  VISION_APIFUNC virtual ~VisTranslucencyEntityRenderer();

  /// \brief
  ///   Static function to access global manager instance
  VISION_APIFUNC static VisTranslucencyEntityRenderer& GlobalInstance();

  // IVisInterleavedTranslucencyRenderer implementation
  VISION_APIFUNC virtual void RenderBatch(IVisVisibilityCollector_cl* pVisCollector, void** pInstances, int iCount, void* pUserData) HKV_OVERRIDE;

protected:
  static VisTranslucencyEntityRenderer s_Instance;
};


/// \brief
///   Implements IVisInterleavedTranslucencyRenderer that renders a list of static geometry instances in transparent mode
class VisTranslucencyStaticGeometryRenderer : public IVisInterleavedTranslucencyRenderer
{
public:
  VISION_APIFUNC VisTranslucencyStaticGeometryRenderer();
  VISION_APIFUNC virtual ~VisTranslucencyStaticGeometryRenderer();

  /// \brief
  ///   Static function to access global manager instance
  VISION_APIFUNC static VisTranslucencyStaticGeometryRenderer& GlobalInstance();

  // IVisInterleavedTranslucencyRenderer implementation
  VISION_APIFUNC virtual void RenderBatch(IVisVisibilityCollector_cl* pVisCollector, void** pInstances, int iCount, void* pUserData) HKV_OVERRIDE;

protected:
  VisStaticGeometryInstanceCollection_cl m_StaticGeoCollection;
  static VisTranslucencyStaticGeometryRenderer s_Instance;
};


/// \brief
///   Implements IVisInterleavedTranslucencyRenderer that renders a list of mesh buffer object instances in transparent mode
class VisTranslucencyMeshBufferObjectRenderer : public IVisInterleavedTranslucencyRenderer
{
public:
  VISION_APIFUNC VisTranslucencyMeshBufferObjectRenderer();
  VISION_APIFUNC virtual ~VisTranslucencyMeshBufferObjectRenderer();

  /// \brief
  ///   Static function to access global manager instance
  VISION_APIFUNC static VisTranslucencyMeshBufferObjectRenderer& GlobalInstance();

  // IVisInterleavedTranslucencyRenderer implementation
  VISION_APIFUNC virtual void RenderBatch(IVisVisibilityCollector_cl* pVisCollector, void** pInstances, int iCount, void* pUserData) HKV_OVERRIDE;

protected:
  VisMeshBufferObjectCollection_cl m_MeshBufferObjectCollection;
  static VisTranslucencyMeshBufferObjectRenderer s_Instance;
};


/// \brief
///   Implements IVisInterleavedTranslucencyRenderer that renders a list of particle group instances in transparent mode
class VisTranslucencyParticleGroupRenderer : public IVisInterleavedTranslucencyRenderer
{
public:
  VISION_APIFUNC VisTranslucencyParticleGroupRenderer();
  VISION_APIFUNC virtual ~VisTranslucencyParticleGroupRenderer();

  /// \brief
  ///   Static function to access global manager instance
  VISION_APIFUNC static VisTranslucencyParticleGroupRenderer& GlobalInstance();

  // IVisInterleavedTranslucencyRenderer implementation
  VISION_APIFUNC virtual void RenderBatch(IVisVisibilityCollector_cl* pVisCollector, void** pInstances, int iCount, void* pUserData) HKV_OVERRIDE;

protected:
  VisParticleGroupCollection_cl m_ParticleGroupCollection;
  static VisTranslucencyParticleGroupRenderer s_Instance;
};


#endif //DEFINE_VISIONTRANSLUCENCYRENDERERS

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
