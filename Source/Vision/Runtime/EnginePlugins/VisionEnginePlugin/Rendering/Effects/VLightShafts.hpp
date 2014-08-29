/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VLightShafts.hpp

#ifndef VLIGHT_SHAFTS_HPP_INCLUDED
#define VLIGHT_SHAFTS_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>

class VLightShafts;
typedef VSmartPtr<VLightShafts> VLightShaftsPtr;

// Versions
#define VLIGHT_SHAFTS_VERSION_0         0                         // initial version                             
#define VLIGHT_SHAFTS_VERSION_CURRENT   VLIGHT_SHAFTS_VERSION_0   // current version

/// \brief
///   Component that can be attached to a light source (point, spot, directional) to simulate light shafts.
///
/// Since this effect requires to read the depth buffer, it is only supported on devices with the ability of sampling the depth 
/// buffer. For the Forward and Deferred Renderer the available linear depth buffer is used, for the Mobile Forward Renderer
/// directly the native depth buffer, to avoid a depth buffer linearization pass. Each VLightShafts effect will be rendered once 
/// into each renderer node during the VRH_CORONAS_AND_FLARES callback.
/// 
/// The effect is generated in 6 passes:
/// 1. For point and directional lights a full-screen quad, for spot lights a cone is rendered into a half sized render target 
///    with the scissor rectangle set to the area that will be blurred radially in the next steps. Pixels that have a smaller
///    view space depth than a specified threshold are masked out, all other pixels fetch a lookup texture that controls the 
///    shape, structure and color of the light shafts.
/// 2. Generated mask is radially blurred with 8 samples along the first quarter of the blur vector with an opaque draw call 
///    into a quarter sized render-target.
/// 3. Generated mask is radially blurred with 8 samples along the second quarter of the blur vector with an additive draw call
///    into a quarter sized render-target.
/// 4. Result from step 2/ 3 is radially blurred with 8 samples along the first half of the blur vector with an opaque draw call
///    into a quarter sized render-target.
/// 5. Result from step 2/ 3 is radially blurred with 8 samples along the second half of the blur vector with an additive draw call
///    into a quarter sized render-target.
/// 6. Result from step 4/ 5 is written into main render context with an additive draw call.
class VLightShafts: public IVObjectComponent, public IVisCallbackHandler_cl
{
public:

  ///
  /// @name Constructor
  /// @{
  ///

  /// \brief
  ///   Constructor.
  EFFECTS_IMPEXP VLightShafts(int iComponentFlags=VIS_OBJECTCOMPONENTFLAG_NONE);

  ///
  /// @}
  ///

  ///
  /// @name Light Shaft Properties
  /// @{
  ///

  /// \brief
  ///   Returns the parent light source.
  EFFECTS_IMPEXP VisLightSource_cl* GetLightSource() const;

  /// \brief
  ///   Enables/ disables effect.
  inline void SetEnabled(bool bStatus)
  {
    Enabled = bStatus ? TRUE : FALSE;
  }

  /// \brief
  ///   Indicates whether effect is currently enabled.
  inline bool IsEnabled() const 
  {
    return (Enabled==TRUE);
  }

  /// \brief
  ///   Sets the intensity of the the light shafts. 
  /// 
  /// \param fIntensity
  ///   Light shafts intensity. Must be a non-negative value.
  inline void SetIntensity(float fIntensity)
  {
    VASSERT(fIntensity >= 0.0f);
    Intensity = fIntensity;
  }

  /// \brief
  ///   Gets the intensity of the the light shafts.
  inline float GetIntensity() const
  {
    return Intensity;
  }

  /// \brief
  ///   Sets the radius in texture space of the area that will be blurred to create the light shafts.
  ///
  /// \param fRadius
  ///   Radius in texture space. Must be in the range 0.00001 - 1.0.
  inline void SetRadius(float fRadius)
  {
    VASSERT((fRadius >= HKVMATH_DEFAULT_EPSILON) && (fRadius <= 1.0f));
    Radius = fRadius;
  }

  /// \brief
  ///   Gets the radius in texture space of the area that will be blurred to create the light shafts.
  inline float GetRadius() const
  {
    return Radius;
  }

  /// \brief
  ///   Sets the depth threshold in linear view space. Pixels with a depth difference to the parent light 
  ///   source in view space greater than this value will be treated as blockers.
  /// 
  /// \param fDepthThreshold
  ///   Depth threshold in linear view space. Must be a non-negative value.
  inline void SetDepthThreshold(float fDepthThreshold)
  {
    VASSERT(fDepthThreshold >= 0.0f);
    DepthThreshold = fDepthThreshold;
  }

  /// \brief
  ///   Gets the depth threshold in view space.
  inline float GetDepthThreshold() const
  {
    return DepthThreshold;
  }

  /// \brief
  ///   Sets the length of the light shafts in texture space.
  /// 
  /// \param fLightShaftLength
  ///   Light shaft length in texture space. Must be in the range 0.0 - 1.0.
  inline void SetLightShaftLength(float fLightShaftLength)
  {
    VASSERT((fLightShaftLength >= 0.0f) && (fLightShaftLength <= 1.0f));
    LightShaftLength = fLightShaftLength;
  }

  /// \brief
  ///   Gets the length the light shafts in texture space.
  inline float GetLightShaftLength() const
  {
    return LightShaftLength;
  }

  /// \brief
  ///   Sets a lookup texture to generate the source of the light shafts. Can be set to NULL to use a default texture.
  EFFECTS_IMPEXP void SetLookupTexture(VTextureObject *pTex);

  /// \brief
  ///   Loads and sets a texture by filename to generate the source of the light shafts.
  EFFECTS_IMPEXP void SetLookupTexture(const char *szFilename);

  /// \brief
  ///   Returns the lookup texture. Returns NULL if no texture is set.
  inline VTextureObject* GetLookupTexture() const 
  {
    return m_spLookupTexture;
  }

  /// \brief
  ///   Specifies the visibility bit-mask to exclude light shafts rendering from specific contexts.
  inline void SetVisibleBitmask(unsigned int uiMask)
  {
    m_iVisibleBitmask = uiMask;
  }

  /// \brief
  ///   Gets the visibility bit-mask for excluding light shafts rendering from specific contexts.
  inline unsigned int GetVisibleBitmask() const
  {
    return m_iVisibleBitmask;
  }

  ///
  /// @}
  ///

  ///
  /// @name IVObjectComponent Overrides
  /// @{
  ///

  /// \brief
  ///   Overridden function to respond to owner changes.
  EFFECTS_IMPEXP virtual void SetOwner(VisTypedEngineObject_cl *pOwner) HKV_OVERRIDE;

  /// \brief
  ///   Light shafts can be only attached to light sources.
  EFFECTS_IMPEXP virtual BOOL CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut) HKV_OVERRIDE;

  /// \brief
  ///   Overridden function to respond to variable changes.
  EFFECTS_IMPEXP virtual void OnVariableValueChanged(VisVariable_cl *pVar, const char * value) HKV_OVERRIDE;

  ///
  /// @}
  ///

  ///
  /// @name IVisCallbackHandler_cl Overrides
  /// @{
  ///

  /// \brief
  ///   Callback handler.
  virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData) HKV_OVERRIDE;

  ///
  /// @}
  ///

  ///
  /// @name Serialization
  /// @{
  ///

  V_DECLARE_SERIAL_DLLEXP(VLightShafts, EFFECTS_IMPEXP)

  V_DECLARE_VARTABLE(VLightShafts, EFFECTS_IMPEXP)

  EFFECTS_IMPEXP virtual void Serialize(VArchive &ar) HKV_OVERRIDE;

  ///
  /// @}
  ///

private:

  void Init();

  void DeInit();

  bool CreateTextures();

  bool CreateShaders(bool bForce=false);

  void OnRender(VisRenderContext_cl *pContext);

  VTextureObject* GetDepthTexture(VRendererNodeCommon *pRendererNode, bool bNativeDepthBuffer);

  bool Update(VisRenderContext_cl *pContext, bool bNativeDepthBuffer);

  void GenerateMaskPass(VisRenderContext_cl *pContext, VTextureObject *pDepthTexture);

  void RadialBlurPass();

  void ApplyPass();

  BOOL Enabled;
  float Intensity;
  float Radius;
  float DepthThreshold;
  float LightShaftLength;
 
  bool m_bIsInitialized;
  unsigned int m_iVisibleBitmask;
  hkvVec2 m_vPosition; // center of light shafts in texture space
  VRectanglef m_scissorRect; 
  hkvVec4 m_bounds; // effect bounds in texture space (not clamped to texture boundaries)
  hkvVec4 m_vColorDepthThreshold; // xyz: color of the light shafts pre-multiplied with intensity, w = depth threshold
  VTextureObjectPtr m_spLookupTexture;
  VisTextureAnimInstancePtr m_spLookupTextureAnim;

  hkvVec2 m_vHalfSizeExtents;
  
  // half size
  VisRenderableTexturePtr m_spMaskTexture; 
  VisRenderContextPtr m_spMaskRenderContext;

  // quarter size
  VisRenderableTexturePtr m_spBlurTextures[2];  
  VisRenderContextPtr m_spBlurRenderContexts[2]; 

  VCompiledTechniquePtr m_spGenMaskTechnique; 
  VCompiledTechniquePtr m_spRadialBlurTechnique; 
  VCompiledTechniquePtr m_spApplyTechnique;     

  VConstantBufferRegister m_regColorDepthThreshold;
  VConstantBufferRegister m_regBounds;
  VConstantBufferRegister m_regHalfTexelOffset;
  VConstantBufferRegister m_regPositionLength;

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
