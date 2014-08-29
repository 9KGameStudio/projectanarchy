/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VIS_API_DEBUG_SHADING_RENDER_LOOP_HPP
#define VIS_API_DEBUG_SHADING_RENDER_LOOP_HPP

/// \brief
///   Renderloop class that renders geometry with a debug shader
class VisDebugShadingRenderLoop_cl : public VisionRenderLoop_cl
{
public:

  /// \brief
  ///   Constructor
  inline VisDebugShadingRenderLoop_cl() : 
    m_LightMapInstances(1024), m_LightGridInstances(1024), m_OneInstance(1)
  {
    m_bPerPrimitiveChanges = m_bPerSurfaceChanges = false;
    m_pMultiVisZoneTex = NULL;
  }

  /// \brief
  ///   Destructor
  VOVERRIDE ~VisDebugShadingRenderLoop_cl()
  {
    V_SAFE_RELEASE(m_pMultiVisZoneTex);
  }

  /// \brief
  ///   Overridden renderloop function
  VISION_APIFUNC virtual void OnDoRenderLoop(void *pUserData) HKV_OVERRIDE;

  /// \brief
  ///   Called to change the shading mode
  VISION_APIFUNC void SetEffects(VCompiledEffect *pGeometryFX, VCompiledEffect* pParticleEffect, bool bRenderSky = true);


private:

  /// \brief
  ///   Performs debug rendering for particle effects.
  void RenderParticles(const VisParticleGroupCollection_cl* pVisibleParticleGroups);

  /// \brief
  ///   Internal function to update the zone color (if m_LMVisZoneColorReg,m_LGVisZoneColorReg is
  ///   valid)
  void SetVisibilityZoneColor(VCompiledShaderPass *pShader, VConstantBufferRegister &colorReg, VConstantBufferRegister &textureReg, VisVisibilityZone_cl *pZone, int iModulation);

  /// \brief
  ///   Wraps around SetZoneColor for object3d
  void SetVisibilityZoneColorObj(VCompiledShaderPass *pShader, VConstantBufferRegister &colorReg, VConstantBufferRegister &textureReg, VisObject3D_cl *pObj);

  /// \brief
  ///   Internal function to update the zone color (if m_LMZoneColorReg,m_LGZoneColorReg is valid)
  void SetZoneColor(VCompiledShaderPass *pShader, VConstantBufferRegister &colorReg, IVisZone_cl *pZone, int iModulation);

  /// \brief
  ///   Internal function to update texture related properties
  void SetBaseTextureParams(VCompiledShaderPass *pShader, VConstantBufferRegister &colorReg, const VTextureObject* pTexture) const;

  /// \brief
  ///   Generates and sets a unique color per material pointer
  void SetMaterialColor(VCompiledShaderPass *pShader, VConstantBufferRegister &colorReg, const VisSurface_cl *pSurface);

  /// \brief
  ///   Generates and sets color per material pointer whether the texture is missing
  void SetMaterialMissingColor(VCompiledShaderPass *pShader, VConstantBufferRegister &colorReg, const VisSurface_cl *pSurface);

  /// \brief
  ///   Generates and sets color per material pointer whether the texture is missing
  void SetMaterialMissingColor(VCompiledShaderPass *pShader, VConstantBufferRegister &colorReg, const VTextureObject *pTexture);
  

  /// \brief
  ///   Internal function to update the worldspace texture density levels used for interpolating the density 'color' of pixels when shading
  void SetTextureDensityLevels(VCompiledShaderPass *pShader, VConstantBufferRegister &densityLevelsReg);

  /// \brief
  ///   Internal function to update the size (X/Y resolution) of the base texture, which is used to determine the worldspace texture density of a pixel
  void SetTextureDensitySize(VCompiledShaderPass *pShader, VConstantBufferRegister &densitySizeReg, const VTextureObject *pTexture);

  void SplitGeometryInstances(const VisStaticGeometryInstanceCollection_cl &visibleInst);

  VCompiledTechniquePtr m_spLGTechnique; ///< effect for rendering lightgrid scene geometry
  VCompiledTechniquePtr m_spLMTechnique; ///< effect for rendering lightmapped scene geometry
  VCompiledTechniquePtr m_spNotAvailableTechnique; ///< this technique is used if the geometry does not support rendering with the shader (e.g. streams not set)

  VConstantBufferRegister m_LGVisZoneColorReg, m_LMVisZoneColorReg;     ///< If valid, the rendered geometry needs individual zone color tinting
  VConstantBufferRegister m_LGVisZoneTextureReg, m_LMVisZoneTextureReg; ///< If valid, the rendered geometry needs individual zone texture binding
  VConstantBufferRegister m_LGZoneColorReg, m_LMZoneColorReg;     ///< If valid, the rendered geometry needs individual zone color tinting
  VConstantBufferRegister m_MaterialIDReg;                        ///< If valid, the rendered geometry needs individual per material color
  VConstantBufferRegister m_MaterialMissingReg;                   ///< If valid, the rendered geometry needs individual per material color indicating whether a texture is missing or not
  VConstantBufferRegister m_BaseTextureParamsReg;                 ///< If valid, the rendered geometry needs individual texture parameter setup
  VConstantBufferRegister m_TextureDensitySizeReg;		  ///< If valid, the rendered geometry needs to know the size of its base texture
  VConstantBufferRegister m_TextureDensityLevelsReg;		  ///< If valid, the rendered geometry needs to know the min/normal/max texture density ranges to interpolate its own density color
  bool m_bPerPrimitiveChanges;    ///< true if geometry needs to be rendered 1-by-1 with inbetween updates via SetZoneColor (for instance)
  bool m_bPerSurfaceChanges;      ///< true if geometry needs update parameter every time the surface changes while rendering
  bool m_bRenderSky;              ///< true if the sky should be rendered

  VCompiledEffectPtr m_spParticleEffect; ///< Effect for debug rendering of particles.
  VConstantBufferRegister m_VisZoneTextureReg_Particle;       ///< If valid, the rendered particles need individual zone texture binding.
  VConstantBufferRegister m_VisZoneColorReg_Particle;         ///< If valid, the rendered particles need individual zone color tinting.
  VConstantBufferRegister m_ZoneColorReg_Particle;            ///< If valid, the rendered particles need individual zone color tinting.
  VConstantBufferRegister m_MaterialMissingReg_Particle;      ///< If valid, the rendered particles need individual per material color indicating whether a texture is missing or not.
  VConstantBufferRegister m_BaseTextureParamsReg_Particle;    ///< If valid, the rendered particles need individual texture parameter setup.
  VConstantBufferRegister m_TextureDensitySizeReg_Particle;		///< If valid, the rendered particles need to know the size of its base texture.
  VConstantBufferRegister m_TextureDensityLevelsReg_Particle;	///< If valid, the rendered particles need to know the min/normal/max texture density ranges to interpolate its own density color.


  VTextureObject *m_pMultiVisZoneTex;  ///< texture that holds the "multiple viszones" texture

  VisStaticGeometryInstanceCollection_cl m_LightMapInstances, m_LightGridInstances, m_NotAvailableInstances, m_OneInstance;
};

#endif

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
