/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/VPostProcessNoiseAndVignette.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RendererNode/VRendererNodeCommon.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/MobileForwardRenderer/VMobileForwardRenderer.hpp>
#if !defined( HK_ANARCHY )
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ForwardRenderer/ForwardRenderer.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/DeferredShading/DeferredShadingApp.hpp>
#endif

// mapping table from vartable enum to VIS_TransparencyType
// AlphaBlending,AlphaTest,Multiplicative,Multiplicative2x,Additive,Subtractive
static const VIS_TransparencyType Blending2Transp[6] = 
 {VIS_TRANSP_ALPHA,VIS_TRANSP_ALPHATEST,VIS_TRANSP_MULTIPLICATIVE,VIS_TRANSP_MODULATE2X,VIS_TRANSP_ADDITIVE, VIS_TRANSP_SUBTRACTIVE}; 


VPostProcessNoiseAndVignette::VPostProcessNoiseAndVignette() :
  VPostProcessingBaseComponent()
{
  m_iRequiredBufferFlags = VBUFFERFLAG_FINALCOLOR;
  m_fPriority = VPOSTPROCESSOR_PRIORITY_SCREENMASKS - 80.0f; // after FXAA 
  m_bufferUsageFlags = SAMPLES_ACCUMULATION_BUFFER | DRAWS_FULLSCREEN_QUAD | USES_BLENDING;

  m_bActive = false;
  EnableNoise = TRUE;
  EnableVignette = FALSE;
  Intensity = 1.f;
  Granularity = 0.f;
  Threshold = 0.f;
  VignetteBlending = 2; // multiplicative
}


VPostProcessNoiseAndVignette::~VPostProcessNoiseAndVignette()
{
  DeInitializePostProcessor();
}


void VPostProcessNoiseAndVignette::RemoveContext()
{
  GetOwner()->RemoveContext(GetTargetContext());
}

void VPostProcessNoiseAndVignette::SetupContext()
{
  VASSERT_MSG(GetOwner() != NULL, "Postprocessing effects require a valid renderer node!");

  GetOwner()->AddContext(GetTargetContext());

  m_spSourceTextures[0] = GetOwner()->GetGBuffer(IVRendererNode::VGBT_Accumulation);
  m_iNumSourceTextures = 1;
  VASSERT(m_spSourceTextures[0] != NULL);
}



bool VPostProcessNoiseAndVignette::SetVariable(const char *szName, const char *szValue)
{
  bool bRes = VPostProcessingBaseComponent::SetVariable(szName, szValue);

  // load vignette texture
  if (szName && !strcmp(szName,"VignetteTexture"))
  {
    if (!szValue || !szValue[0])
      m_spVignetteTexture = NULL;
    else
      m_spVignetteTexture = Vision::TextureManager.Load2DTexture(szValue);
  }

  return bRes;
}


BOOL VPostProcessNoiseAndVignette::CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut)
{
  if (!VPostProcessingBaseComponent::CanAttachToObject(pObject, sErrorMsgOut))
    return FALSE;
#if !defined( HK_ANARCHY )
  // pp can be applied to these renderer node types
  if (pObject->IsOfType(VDeferredRenderingSystem::GetClassTypeId()))
    return TRUE;
  if (pObject->IsOfType(VForwardRenderingSystem::GetClassTypeId()))
    return TRUE;
#endif
  if (pObject->IsOfType(VMobileForwardRenderingSystem::GetClassTypeId()))
    return TRUE;
  
  sErrorMsgOut = "Component can not be attached to this type of renderer node";
  return FALSE;
}


void VPostProcessNoiseAndVignette::InitializePostProcessor()
{
  if (m_bIsInitialized)
    return;

  VASSERT_MSG(GetOwner()->IsInitialized(), "The renderer node that owns this post processor must be initialized before initializing the post processor.");

  SetupContext();

  // Load shader library
  VShaderEffectLib* pShaderLib = Vision::Shaders.LoadShaderLibrary("Shaders\\Postprocess.ShaderLib", SHADERLIBFLAG_HIDDEN);
  VASSERT(pShaderLib != NULL); // file not found?
  m_spNoiseTechnique = Vision::Shaders.CreateTechnique("SimpleNoise",NULL,NULL,EFFECTCREATEFLAG_NONE,pShaderLib);
  m_bIsInitialized = true;
}


void VPostProcessNoiseAndVignette::DeInitializePostProcessor()
{
  if (!m_bIsInitialized)
    return;

  RemoveContext();
  m_spNoiseTechnique = NULL;
  m_bIsInitialized = false;
}

void VPostProcessNoiseAndVignette::SetNoiseParams(float fIntensity, float fGranularity, float fThreshold)
{
  Intensity = fIntensity;
  Granularity = fGranularity;
  Threshold = fThreshold;
}

void VPostProcessNoiseAndVignette::SetVignette(VTextureObject *pTexture, VIS_TransparencyType blending)
{
  // map from VIS_TransparencyType to internal enum
  int iCount = sizeof(Blending2Transp)/sizeof(Blending2Transp[0]);
  bool bBlendmodeSupported = false;
  for (int i=0;i<iCount;i++)
    if (Blending2Transp[i]==blending)
    {
      VignetteBlending = i;
      bBlendmodeSupported = true;
    }
  VASSERT_MSG(bBlendmodeSupported, "The passed blend mode is not supported");
  m_spVignetteTexture = pTexture;
}


void VPostProcessNoiseAndVignette::Execute()
{
  INSERT_PERF_MARKER_SCOPE("VPostProcessNoiseAndVignette");

  int iWidth, iHeight;
  VisRenderContext_cl *pContext = VisRenderContext_cl::GetCurrentContext();
  pContext->GetSize(iWidth, iHeight);
  hkvVec2 vScreen((float)iWidth, (float)iHeight);

  IVRender2DInterface* pRI = Vision::RenderLoopHelper.BeginOverlayRendering();

  if (EnableNoise && Intensity>0.f && m_spNoiseTechnique!=NULL)
  {
    // encode noise params into color (0..1 range)
    VColorRef iColorParams; // x=Intensity,y=Threshold, zw=Secondary offset
    iColorParams.FromFloat4(hkvVec4(Intensity, Threshold, Vision::Game.GetFloatRand(),Vision::Game.GetFloatRand()));

    // we can encode all noise parameters into the vertices of the 2d overlay
    float fOfsX = Vision::Game.GetFloatRand();
    float fOfsY = Vision::Game.GetFloatRand();
    float fScaleX = vScreen.x/(128.f * Granularity) * (1.f+Vision::Game.GetFloatRandNeg()*0.1f);
    float fScaleY = vScreen.y/(128.f * Granularity) * (1.f+Vision::Game.GetFloatRandNeg()*0.1f);

    for (int i=0;i<(int)m_spNoiseTechnique->GetShaderCount();i++)
    {
      pRI->DrawTexturedQuadWithShader(hkvVec2(0.0f, 0.0f), vScreen, NULL,
        hkvVec2(fOfsX, fOfsY), hkvVec2(fOfsX+fScaleX, fOfsY+fScaleY), iColorParams, *m_spNoiseTechnique->GetShader(i));
    }
  }

  if (EnableVignette && m_spVignetteTexture!=NULL)
  {

    // sanity check
    if (VignetteBlending<0 || VignetteBlending > sizeof(Blending2Transp)/sizeof(Blending2Transp[0]))
      VignetteBlending = 2; // fall back to multiplicative

    // render a simple blended quad, no specific shader
    VSimpleRenderState_t iState(Blending2Transp[VignetteBlending],RENDERSTATEFLAG_FRONTFACE|RENDERSTATEFLAG_ALWAYSVISIBLE|RENDERSTATEFLAG_FILTERING|RENDERSTATEFLAG_NOWIREFRAME|RENDERSTATEFLAG_SAMPLERCLAMPING);
    pRI->DrawTexturedQuad(hkvVec2(0.0f, 0.0f), vScreen, m_spVignetteTexture, hkvVec2(0, 0), hkvVec2(1,1), V_RGBA_WHITE, iState);
  }

  Vision::RenderLoopHelper.EndOverlayRendering();
}


#define NOISE_SERIALIZATION_VERSION_0        0
#define NOISE_SERIALIZATION_VERSION_CURRENT  NOISE_SERIALIZATION_VERSION_0

V_IMPLEMENT_SERIAL(VPostProcessNoiseAndVignette, VPostProcessingBaseComponent, 0, &g_VisionEngineModule);
void VPostProcessNoiseAndVignette::Serialize(VArchive& ar)
{
  char iLocalVersion = NOISE_SERIALIZATION_VERSION_CURRENT;
  VPostProcessingBaseComponent::Serialize(ar);

  if (ar.IsLoading())
  {
    ar >> iLocalVersion; 
    VASSERT(iLocalVersion <= NOISE_SERIALIZATION_VERSION_CURRENT);

    ar >> m_bActive;
    ar >> EnableNoise >> EnableVignette;
    ar >> Intensity >> Granularity >> Threshold >> VignetteBlending;
    m_spVignetteTexture = VTextureObject::DoArchiveExchange(ar);
  }
  else
  {
    ar << iLocalVersion;
    
    ar << m_bActive;
    ar << EnableNoise << EnableVignette;
    ar << Intensity << Granularity << Threshold << VignetteBlending;
    VTextureObject::DoArchiveExchange(ar, m_spVignetteTexture);
  }
}



START_VAR_TABLE(VPostProcessNoiseAndVignette, VPostProcessingBaseComponent, "VPostProcessNoiseAndVignette", 0, "Noise and Vignette")
  DEFINE_CATEGORY(VPostProcessNoiseAndVignette, "Noise");
  DEFINE_VAR_BOOL(VPostProcessNoiseAndVignette, EnableNoise,  "Toggle noise", "TRUE", 0, "");
  DEFINE_VAR_FLOAT(VPostProcessNoiseAndVignette, Intensity,  "Intensity of the noise in 0..1 range", "0.5", 0, "Slider(0,1)");
  DEFINE_VAR_FLOAT(VPostProcessNoiseAndVignette, Granularity,"Size of the grain", "2.0", 0, NULL);
  DEFINE_VAR_FLOAT(VPostProcessNoiseAndVignette, Threshold,  "Threshold value to clip noise. With a value of 0 noise appears all over the screen", "0.0", 0, "Slider(0,1)");
  DEFINE_CATEGORY(VPostProcessNoiseAndVignette, "Vignette");
  DEFINE_VAR_BOOL(VPostProcessNoiseAndVignette, EnableVignette,  "Enable the vignette. A vignette is a screen overlay, e.g. to fade out color towards the edges", "FALSE", 0, "");
  DEFINE_VAR_STRING_CALLBACK(VPostProcessNoiseAndVignette,VignetteTexture,"2D texture file used as vignette screen overlay","",DISPLAY_HINT_TEXTUREFILE,0);
  DEFINE_VAR_ENUM(VPostProcessNoiseAndVignette, VignetteBlending, "Blending mode of the vignette", "Multiplicative", "AlphaBlending,AlphaTest,Multiplicative,Multiplicative2x,Additive,Subtractive", 0, 0);
END_VAR_TABLE

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
