/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef MOBILEWATER_HPP_INCLUDED
#define MOBILEWATER_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>

/// \brief A water object which is optimized for rendering on mobile platforms
class VMobileWater : public VisObject3D_cl
{
public:
  /// \brief if called for deserialization or not
  enum Deserialize_e
  {
    DeserializeYes, // called for deserialization
    DeserializeNo   // not called for deserialization
  };

  /// \brief constructor
  EFFECTS_IMPEXP VMobileWater(Deserialize_e deserialize = DeserializeNo);

  /// \brief destructor
  EFFECTS_IMPEXP ~VMobileWater();

  // overridden methods
  EFFECTS_IMPEXP virtual void OnObject3DChanged(int iO3DFlags) HKV_OVERRIDE;
  EFFECTS_IMPEXP virtual void DisposeObject() HKV_OVERRIDE;
  EFFECTS_IMPEXP virtual VBool WantsDeserializationCallback(const VSerializationContext &context) HKV_OVERRIDE;
  EFFECTS_IMPEXP virtual void OnDeserializationCallback(const VSerializationContext &context) HKV_OVERRIDE;

  /// \brief Sets the technique to be used for rendering the mobile water.
  EFFECTS_IMPEXP void SetTechnique(VCompiledTechnique *pTechnique);

  /// \brief Sets the effect to be used for rendering the mobile water.
  EFFECTS_IMPEXP void SetEffect(VCompiledEffect *pEffect);

  /// \brief Reassign the effect to the mobile water shape.
  EFFECTS_IMPEXP void ReassignEffect();

  /// \brief Returns the size of the mobile water plane.
  inline const hkvVec2 GetSize() const { return m_size; }

  /// \brief Sets the size for the mobile water plane.
  EFFECTS_IMPEXP void SetSize(hkvVec2 size);

  /// \brief Sets the number of subdivisions to be used for the water plane.
  EFFECTS_IMPEXP void SetNumGridSubdivisions(int x, int y);

  /// \brief Sets if a radial grid should be used instead of a plane.
  EFFECTS_IMPEXP void SetUseRadialGrid(bool bValue);

  /// \brief Sets the renderhook to be used for rendering the mobile water.
  EFFECTS_IMPEXP void SetRenderHook(VRenderHook_e eRenderHook);

  /// \brief Returns the world space corners of the plane.
  EFFECTS_IMPEXP void GetWorldSpaceVertices(hkvVec3 (&vertices)[4]) const;

  /// \brief Traces a ray against the water plane.
  EFFECTS_IMPEXP float TracePlane(const hkvVec3& vStart, const hkvVec3& vEnd, bool bDoubleSided) const;

  /// \brief Sets the visibility bitmask for the rendering of the mobile water.
  EFFECTS_IMPEXP void SetVisibilityBitMask(unsigned int uiVisibilityBitmask);

  /// \brief Sets the amount of texture tiling.
  EFFECTS_IMPEXP void SetTextureTiling(hkvVec2 tiling);

  /// \brief Returns the amount of texture tiling.
  inline const hkvVec2 GetTextureTiling() const { return m_textureTiling; }

  /// \brief Updates the water plane with the latest light grid information.
  EFFECTS_IMPEXP void UpdateStaticLighting();

  /// \brief Enables or disables the use of static lighting for the water mesh.
  EFFECTS_IMPEXP void SetUseStaticLighting(bool bValue);

#ifndef _VISION_DOC
  V_DECLARE_SERIAL_DLLEXP( VMobileWater,  EFFECTS_IMPEXP );
  EFFECTS_IMPEXP virtual void Serialize( VArchive &ar );
#endif

private:
  // non-copyable
  VMobileWater(const VMobileWater&);
  void operator = (const VMobileWater&);

  struct VertexLayout
  {
    hkvVec3 pos;
    hkvVec3 normal;
    hkvVec2 tex;
  };

  struct VertexLayoutColor
  {
    hkvVec3 pos;
    hkvVec3 normal;
    hkvVec2 tex;
    hkvVec3 color;
  };

  void CommonInit();
  void CreateMesh();
  void ReInitMeshBuffer();
  EFFECTS_IMPEXP void UpdateMeshTransformation();
  const hkvAlignedBBox& GetBoundingBox();

  // member variables
  VisMeshBufferObjectPtr m_spMeshBufferObject;
  hkvVec3 m_localPlaneCorners[4];
  int  m_iNumGridSubdivisions[2];
  bool m_bUseRadialGrid;
  bool m_bUseStaticLighting;
  hkvVec2 m_textureTiling;
  hkvVec2 m_size;
  VRenderHook_e m_eRenderHook;
  unsigned int m_uiVisibilityBitMask;
  VCompiledTechniquePtr   m_spWaterTechnique;    ///< Shader effect used
  VisEffectConfig_cl      m_EffectConfig;         ///< Source shader effect
  hkvAlignedBBox m_BoundingBox;
};

/// \brief
///   Manager class that manages all mobile water objects in the scene.
///   One global instance can be accessed via VMobileWaterManager::GlobalManager()
class VMobileWaterManager : public IVisCallbackHandler_cl
{
  friend class VMobileWater;
public:
  /// \brief initializes the mobile water manager
  EFFECTS_IMPEXP void OneTimeInit();
  /// \brief deinitializes the mobile water manager. Deletes all registered mobile water instances.
  EFFECTS_IMPEXP void OneTimeDeInit();
  /// \brief returns the global instance of the mobile water manager
  EFFECTS_IMPEXP static VMobileWaterManager& GlobalManager();
  /// \brief overridden method
  EFFECTS_IMPEXP virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData) HKV_OVERRIDE;

private:
  void Add(VMobileWater* pElement);
  void Remove(VMobileWater* pElement);

  static VMobileWaterManager g_MobileWaterManager;
  VRefCountedCollection<VMobileWater> m_Instances;
};

/// \brief 
///   Shader class used for updating shader constants of mobile water shaders
class VMobileWaterShader : public VCompiledShaderPass
{
public:
  V_DECLARE_DYNCREATE_DLLEXP( VMobileWaterShader,  EFFECTS_IMPEXP )
  EFFECTS_IMPEXP virtual void PostCompileFunction(VShaderEffectResource *pSourceFX, VShaderPassResource *pSourceShader) HKV_OVERRIDE;

  /// \brief checks if the shader needs a shader constant update for the given renderer node
  EFFECTS_IMPEXP bool NeedsUpdate(IVRendererNode* pRendererNode) const;

  /// \brief performs an update of the shader constants for the given renderer node
  EFFECTS_IMPEXP void UpdateConstants(IVRendererNode* pRendererNode);

private:
  VConstantBufferRegister m_regRight, m_regDown, m_regUpperLeftCorner;
  unsigned int m_uiLastUpdate;
  IVRendererNode* m_pLastRendererNode; // this pointer might contain a non null value but still be invalid!
};

/// \brief Class which generates a planar or radial grid for water rendering
class VWaterPlaneGenerator
{
public:
  /// \brief default vertex layout, additional members can be added at the end
  struct VertexLayout
  {
    hkvVec3 pos;
    hkvVec3 normal;
    hkvVec2 tex;
  };

  VWaterPlaneGenerator() : m_uiStride(0) {}

  /// \brief generates a water plane with the given parameters
  template <typename LAYOUT>
  VisMeshBuffer_cl* Generate(bool bUseRadialGrid, const int (&iNumGridSubdivisions)[2], const hkvVec3 (&planeCorners)[4], hkvVec2 numTextureTiles)
  {
    { V_COMPILE_ASSERT(offsetof(LAYOUT, pos) == offsetof(VertexLayout, pos)); }
    { V_COMPILE_ASSERT(offsetof(LAYOUT, normal) == offsetof(VertexLayout, normal)); }
    { V_COMPILE_ASSERT(offsetof(LAYOUT, tex) == offsetof(VertexLayout, tex)); }
    m_uiStride = sizeof(LAYOUT);
    return DoGenerate(bUseRadialGrid, iNumGridSubdivisions, planeCorners, numTextureTiles);
  }

protected:
  // Override these functions to customize the generator

  /// \brief adds additional fields to the vertex descriptor
  virtual void FillVertexDescriptor(VisMBVertexDescriptor_t& desc);

  /// \brief fills custom fields with data
  virtual void FillCustomData(void* pVertexData, unsigned int uiNumVertices);

  size_t m_uiStride;

private:
  VisMeshBuffer_cl* DoGenerate(bool bUseRadialGrid, const int (&iNumGridSubdivisions)[2], const hkvVec3 (&planeCorners)[4], hkvVec2 numTextureTiles);
};

/// \brief generates a water plane with additional simple light grid color evaluation
class VWaterPlaneGeneratorLightgrid : public VWaterPlaneGenerator
{
public:
  /// \brief vertex layout used
  struct VertexLayoutLightgrid
  {
    hkvVec3 pos;
    hkvVec3 normal;
    hkvVec2 tex;
    hkvVec3 lightGridColor;
  };
  
  VWaterPlaneGeneratorLightgrid(hkvVec3 vWorldPosition, hkvMat3 rotation, hkvVec3 scale, VLightGrid_cl* pLightGrid) :
    m_vWorldPosition(vWorldPosition),
    m_vScale(scale),
    m_rotation(rotation),
    m_pLightGrid(pLightGrid)
  {

  }

  VWaterPlaneGeneratorLightgrid(hkvVec3 vWorldPosition, hkvMat3 rotation, hkvVec3 scale, VLightGrid_cl* pLightGrid, size_t uiStride ) :
    m_vWorldPosition(vWorldPosition),
    m_vScale(scale),
    m_rotation(rotation),
    m_pLightGrid(pLightGrid)
  {
    m_uiStride = uiStride;
  }

  VisMeshBuffer_cl* Generate(bool bUseRadialGrid, const int (&iNumGridSubdivisions)[2], const hkvVec3 (&planeCorners)[4], hkvVec2 numTextureTiles);

  virtual void FillCustomData(void* pVertexData, unsigned int uiNumVertices) HKV_OVERRIDE;

protected:
  virtual void FillVertexDescriptor(VisMBVertexDescriptor_t& desc) HKV_OVERRIDE;

private:
  hkvVec3 m_vWorldPosition;
  hkvVec3 m_vScale;
  VLightGrid_cl* m_pLightGrid;
  hkvMat3 m_rotation;
};

#endif

/*
 * Havok SDK - Base file, BUILD(#20140327)
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
