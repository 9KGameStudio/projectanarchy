/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VPostProcessNoiseAndVignette.hpp

#ifndef VPOSTPROCESSNOISEANDVIGNETTE_HPP_INCLUDED
#define VPOSTPROCESSNOISEANDVIGNETTE_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/PostProcessBase.hpp>



/// \brief
///   Postprocessor for simple noise and a vignette texture. Can be attached to deferred and (mobile-)forward renderer nodes
class VPostProcessNoiseAndVignette : public VPostProcessingBaseComponent
{
public:
  /// \brief
  ///   Constructor
  EFFECTS_IMPEXP VPostProcessNoiseAndVignette();

  /// \brief
  ///   Destructor
  EFFECTS_IMPEXP virtual ~VPostProcessNoiseAndVignette();

  /// \brief
  ///   Toggles noise rendering independent from the post processor's active state. If disabled, noise does not cause any performance impact
  inline void SetEnableNoise(bool bEnabled) {EnableNoise = bEnabled?TRUE:FALSE;}

  /// \brief
  ///   Toggles vignette rendering independent from the post processor's active state. If disabled, noise does not cause any performance impact
  inline void SetEnableVignette(bool bEnabled) {EnableVignette = bEnabled?TRUE:FALSE;}

  /// \brief
  ///   Set the parameters that define noise. Please refer to vForge UI for description of parameters
  EFFECTS_IMPEXP void SetNoiseParams(float fIntensity, float fGranularity, float fThreshold=0.0f);

  /// \brief
  ///   Set the parameters that define the vignette. Please refer to vForge UI for description of parameters. If no texture is passed, no vignette is rendered
  EFFECTS_IMPEXP void SetVignette(VTextureObject *pTexture, VIS_TransparencyType blending);

  /// \brief
  ///   Set the parameters that define the vignette. Please refer to vForge UI for description of parameters. If no texture is passed, no vignette is rendered
  inline void SetVignette(const char *szTextureFile, VIS_TransparencyType blending)
  {
    VTextureObject *pTex = (szTextureFile==NULL || szTextureFile[0]==0) ? NULL : Vision::TextureManager.Load2DTexture(szTextureFile);
    SetVignette(pTex, blending);
  }

public:
  #ifndef _VISION_DOC
    EFFECTS_IMPEXP virtual bool SetVariable(const char *szName, const char *szValue) HKV_OVERRIDE;
    EFFECTS_IMPEXP virtual BOOL CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut) HKV_OVERRIDE;
    EFFECTS_IMPEXP virtual bool GetDefaultActiveStateForRenderer(VRendererNodeCommon *pRenderer) HKV_OVERRIDE {return false;}

    V_DECLARE_VARTABLE(VPostProcessNoiseAndVignette, EFFECTS_IMPEXP);

    V_DECLARE_SERIAL_DLLEXP(VPostProcessNoiseAndVignette, EFFECTS_IMPEXP);
    EFFECTS_IMPEXP virtual void Serialize(VArchive &ar) HKV_OVERRIDE;

    // vForge params
    float Intensity;    ///< noise intensity in 0..1 range
    float Granularity;  ///< grain size
    float Threshold;    ///< clip noise threshold, typically <0.1
    BOOL EnableNoise, EnableVignette; ///< independent enable states 
    VTextureObjectPtr m_spVignetteTexture; ///< vignette texture
    int VignetteBlending; ///< internal blending enum - NOT compatible with VIS_TransparencyType, see function SetVignette

  #endif

protected:
  EFFECTS_IMPEXP virtual void InitializePostProcessor() HKV_OVERRIDE;
  EFFECTS_IMPEXP virtual void DeInitializePostProcessor() HKV_OVERRIDE;
  EFFECTS_IMPEXP virtual void Execute() HKV_OVERRIDE;
  void RemoveContext();
  void SetupContext();

  VCompiledTechniquePtr m_spNoiseTechnique;
};


#endif //VPOSTPROCESSNOISEANDVIGNETTE_HPP_INCLUDED

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
