/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/MobileForwardRenderer/VMobileForwardRenderer.hpp>

#include <Vision/Runtime/Engine/Renderer/Lighting/VisApiLightGridManager.hpp>
#include <Vision/Runtime/Engine/Renderer/Shader/VisionMobileShaderProvider.hpp>
#include <Vision/Runtime/Engine/Visibility/VisionTranslucencySorter.hpp>

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/MobileForwardRenderer/VMobileForwardRenderLoop.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/ToneMapping.hpp> 
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/VPostProcessScreenMasks.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/VPostProcessUpscale.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RenderingHelpers/ScratchTexturePool.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RenderingHelpers/RenderingOptimizationHelpers.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RenderingHelpers/TimeOfDay.hpp>


VCallback VMobileForwardRenderingSystem::OnSetResolution;

VMobileForwardRenderingSystem::VMobileForwardRenderingSystem()
{
  GammaCorrection = VGC_None;
  RenderingResolutionMode = VRSM_FULL_RESOLUTION;
  DesiredRenderingDpi = 150.f;
  UseUpscaleFiltering = FALSE;
  ResolutionTolerance = 30.f;
  UseInterleavedTranslucencySorting = FALSE;
  m_bIsInitialized = false;  
  m_bOffscreenContextRequired = false;

  Vision::Callbacks.OnEnterForeground += this;
}

VMobileForwardRenderingSystem::VMobileForwardRenderingSystem(VisRenderContext_cl *pTargetContext) : VRendererNodeCommon(pTargetContext)
{
  GammaCorrection = VGC_None;
  RenderingResolutionMode = VRSM_FULL_RESOLUTION;
  DesiredRenderingDpi = 150.f;
  UseUpscaleFiltering = FALSE;
  ResolutionTolerance = 30.f;
  UseInterleavedTranslucencySorting = FALSE;
  m_bIsInitialized = false;
  m_bOffscreenContextRequired = false;

  Vision::Callbacks.OnEnterForeground += this;
}

VMobileForwardRenderingSystem::~VMobileForwardRenderingSystem()
{
  DeInitializeRenderer();
  ScratchTexturePool_cl::GlobalManager().PurgeUnusedTextures();

  Vision::Callbacks.OnEnterForeground -= this;
}


void VMobileForwardRenderingSystem::SetResolutionMode(VRendererResolutionMode_e eResMode, float fDesiredDpi, bool bUseFiltering, float fResolutionTolerance)
{
  if (RenderingResolutionMode != eResMode || UseUpscaleFiltering != (bUseFiltering ? TRUE: FALSE) || fDesiredDpi != DesiredRenderingDpi || fResolutionTolerance != ResolutionTolerance)
  {
    RenderingResolutionMode = eResMode;
    DesiredRenderingDpi = fDesiredDpi;
    UseUpscaleFiltering = bUseFiltering ? TRUE : FALSE;
    ResolutionTolerance = fResolutionTolerance;

    ReInitializeRenderer();
  }
}

void VMobileForwardRenderingSystem::SetUseInterleavedTranslucencySorting(bool bUseInterleavedTranslucencySorting)
{
  BOOL BUseInterleavedTranslucencySorting = bUseInterleavedTranslucencySorting ? TRUE : FALSE;
  if (UseInterleavedTranslucencySorting == BUseInterleavedTranslucencySorting)
    return;

  VASSERT_MSG(!IsInitialized(), "The mobile forward renderer must be explicitly deinitialized before changing the interleaved translucency sorting settings.");

  UseInterleavedTranslucencySorting = BUseInterleavedTranslucencySorting;
}



void VMobileForwardRenderingSystem::GetTargetSizeFromDeviceDPI(const int *pOriginalSize, int *pTargetSize) const
{
  float fDisplayDpi = Vision::Video.GetDeviceDpi();

  // Platforms without support for retrieving the DPI will return 0, so we simply assume 72dpi here.
  if (fDisplayDpi <= 0.0f)
    fDisplayDpi = 72.0f;

  const float fTargetDpi = hkvMath::Max(1.0f, DesiredRenderingDpi);

  // Compute deviation of target DPI relative to display DPI.
  const float fDpiDeviation = (1.0f - hkvMath::saturate(fTargetDpi / fDisplayDpi)) * 100.0f;

  // Use original resolution
  if (fDpiDeviation < ResolutionTolerance)
  {
    for (int i=0; i<2; i++)
    {
      pTargetSize[i] = pOriginalSize[i];
    }
  }
  // Use downscaled resolution.
  else
  {
    for (int i=0; i<2; i++)
    {
      pTargetSize[i] = static_cast<int>(
        (fTargetDpi / fDisplayDpi) * static_cast<float>(pOriginalSize[i]));

      // Round resolution, so that it is a multiple of 4.
      const int iMinStep = 4;
      pTargetSize[i] = ((pTargetSize[i] + iMinStep/2) / iMinStep) * iMinStep;

      // Size of the render target should never exceed the size of the original target buffer.
      pTargetSize[i] = hkvMath::clamp(pTargetSize[i], 16, pOriginalSize[i]);
    }
  }

  #if defined(HK_DEBUG)
    hkvLog::Info(
      "Adaptive resolution - display density: %.1f, target density: %.1f, render resolution: %ix%i", 
      fDisplayDpi, DesiredRenderingDpi, pTargetSize[0], pTargetSize[1]);
  #endif
}

void VMobileForwardRenderingSystem::DetermineRenderResolution()
{
  // Retrieve original final target size.
  int iOriginalSize[2] = { -1, -1 };

  GetFinalTargetContext()->GetSize(iOriginalSize[0], iOriginalSize[1]);

  VASSERT_MSG(iOriginalSize[0] >= 0 && iOriginalSize[1] >= 0, "Invalid original size!");

  // Determine target size
  switch (RenderingResolutionMode)
  {
  case VRSM_FULL_RESOLUTION:
    m_iOffscreenContextSize[0] = iOriginalSize[0]; m_iOffscreenContextSize[1] = iOriginalSize[1];
    break;
  case VRSM_QUARTER_RESOLUTION:
    m_iOffscreenContextSize[0] = iOriginalSize[0] >> 1; m_iOffscreenContextSize[1] = iOriginalSize[1] >> 1;
    break;
  case VRSM_USE_TARGET_DPI:
    GetTargetSizeFromDeviceDPI(iOriginalSize, m_iOffscreenContextSize);
    break;
  default:
    VASSERT_MSG(false, "Invalid rendering resolution mode!");
    break;
  };
   
  VSetResolutionCallbackObject resolutionCallbackObject(&OnSetResolution, m_iOffscreenContextSize, this);
  OnSetResolution.TriggerCallbacks(&resolutionCallbackObject);

  // Make sure resolution is still valid after callback handler may have modified it.
  // Size of the render target should never exceed the size of the original target buffer
  m_iOffscreenContextSize[0] = hkvMath::Min(iOriginalSize[0], resolutionCallbackObject.m_iTargetSize[0]);
  m_iOffscreenContextSize[1] = hkvMath::Min(iOriginalSize[1], resolutionCallbackObject.m_iTargetSize[1]);

  // Finally set up upscaling.
  const bool bUseUpscaling = (m_iOffscreenContextSize[0] != iOriginalSize[0]) || (m_iOffscreenContextSize[1] != iOriginalSize[1]);
  SetUpscaling(bUseUpscaling);
}

void VMobileForwardRenderingSystem::InitializeRenderer()
{
  if (m_bIsInitialized)
    return;

  // Decide which render resolution to use and set up upscaling if needed.
  DetermineRenderResolution();

  VRendererNodeCommon::InitializeRenderer();

  VisLightGridManager_cl::GlobalManager().SetLightGridMode(VisLightGridManager_cl::Simple);

  if(Components().GetComponentOfType<VPostProcessScreenMasks>() == NULL)
  {
    AddComponent(new VPostProcessScreenMasks());
  }

  CreateShaderProvider();

  // Since the copy PP will be added later on in InitializePostProcessors(), it can be savely removed at this point.
  // This is necessary since when the renderer is reinitialized in order to not render into an off-screen context
  // the simply copy post-processor can be still attached at this point, which would again cause to create an off-
  // screen context.
  if (!m_bOffscreenContextRequired)
  {
    VSimpleCopyPostprocess *pSimplyCopy = Components().GetComponentOfType<VSimpleCopyPostprocess>();
    if (pSimplyCopy != NULL)
    {
      RemoveComponent(pSimplyCopy);
    }
  }

#if defined(_VISION_MOBILE) || defined(_VISION_WIN32) || defined(_VISION_NACL)
  // Only create offscreen context if we have PPs other than the Screen Mask PP
  m_bUsesDirectRenderToFinalTargetContext =  (!m_bOffscreenContextRequired) && (m_Components.Count() <= 1) && (!IsUsingUpscaling());
#else
  // Direct rendering to final target context isn't implemented/tested on consoles
  m_bUsesDirectRenderToFinalTargetContext = false;
#endif
  CreateOffscreenContext();

  // Reapply FOV and clip planes
  OnViewPropertiesChanged();

  m_bIsInitialized = true;

  InitializePostProcessors();
}

void VMobileForwardRenderingSystem::DeInitializeRenderer()
{
  if (!m_bIsInitialized)
    return;

  DeInitializePostProcessors();

  RemoveOffscreenContext();
  ResetShaderProvider();

  // In case we changed the render targets of the main render contexts, set them back to their original state again (backbuffer).
  if (m_spFinalTargetContext == Vision::Contexts.GetMainRenderContext())
  {
    VisRenderContext_cl::ResetMainRenderContext();
  }

  /*-----------------------------------------------------------------------------------------
  Removal of: Vision::Renderer.SetGlobalAmbientColor(hkvVec4::ZeroVector());

  Setting up a global ambient color to black may overwrite a global ambient setting used by
  other components such as VAtmosphere. If this call occurs during rendering that may cause
  incorrect computed values. Furthermore the existence of a global ambient color (whenever it
  makes sense) seems more appropriates to be included in an upper layer and be common to all 
  renderers. Another possibility would be that each render context should be able to have its
  own global ambient color. In this case also the renderer would not be the appropriate place
  to reset it (but the context).

  Real case scenario that causes an undesired behavior: updatable cubemaps with a renderer 
  node in combination with VAtmosphere. Cubemap initializes and deinitializes a renderer node
  when the cubemap faces are renderer. Overwriting the global ambient color causes an incorrect
  calculation of the cloud layer cloud color, until the correct value is reset by the VAtmosphere
  object in the next tick.
  ------------------------------------------------------------------------------------------*/
  //Vision::Renderer.SetGlobalAmbientColor(hkvVec4::ZeroVector());

  m_bIsInitialized = false;

  // Needs to be called after de-initializing the renderer; otherwise, it would re-enter DeInitializeRenderer().
  SetUpscaling(false);

  VRendererNodeCommon::DeInitializeRenderer();
}

void VMobileForwardRenderingSystem::CreateShaderProvider()
{
  // set the mobile forward shader provider
  if (mobileShaderProvider==NULL)
  {
    oldShaderProvider = Vision::GetApplication()->GetShaderProvider();

    if(oldShaderProvider == NULL || (oldShaderProvider != NULL && !oldShaderProvider->IsOfType(V_RUNTIME_CLASS(VisionMobileShaderProvider))))  
    {
      mobileShaderProvider = new VisionMobileShaderProvider();
      Vision::GetApplication()->SetShaderProvider(mobileShaderProvider);
      mobileShaderProvider->CreateLightingShaderForWorld(true);
      mobileShaderProvider->CreateLightingShaderForAllModels(true);
    }
    else
    {
      mobileShaderProvider = oldShaderProvider;
    }
  }
}

void VMobileForwardRenderingSystem::ResetShaderProvider()
{
  IVisShaderProviderPtr currentShaderProvider = Vision::GetApplication()->GetShaderProvider();
  if (currentShaderProvider == mobileShaderProvider)
  {
    Vision::GetApplication()->SetShaderProvider(oldShaderProvider);
    if (oldShaderProvider)
    {
      oldShaderProvider->CreateLightingShaderForWorld(true);
      oldShaderProvider->CreateLightingShaderForAllModels(true);
      oldShaderProvider = NULL;
    }
  }
  mobileShaderProvider = NULL;
}

VType *VMobileForwardRenderingSystem::GetSupportedTimeOfDaySystem()
{
  return V_RUNTIME_CLASS(VTimeOfDay);
}

void VMobileForwardRenderingSystem::SetUpscaling(bool bStatus)
{
  VASSERT(!IsInitialized());

  if (IsUsingUpscaling() == bStatus)
    return;

  if (bStatus)
  {
    // Disable 2D rendering in screen mask postprocessor. We need to render 2D overlay geometry in the upscale render loop.
    // 3D overlay geometry is rendered in the upscale render loop, as it requires a depth buffer.
    VPostProcessScreenMasks *pPostProcessScreenMasks = m_Components.GetComponentOfType<VPostProcessScreenMasks>();
    if (pPostProcessScreenMasks)
      pPostProcessScreenMasks->SetRender2dElements(false);

    AddComponent(new VPostProcessUpscale(UseUpscaleFiltering == TRUE));
  }
  else
  {
    // Re-enable 2D rendering in the screen mask postprocessor.
    VPostProcessScreenMasks *pPostProcessScreenMasks = m_Components.GetComponentOfType<VPostProcessScreenMasks>();
    if (pPostProcessScreenMasks)
      pPostProcessScreenMasks->SetRender2dElements(true);

    if(VPostProcessUpscale* pUpscale = m_Components.GetComponentOfType<VPostProcessUpscale>())
    {
      RemoveComponent(pUpscale);
    }
  }

}

void VMobileForwardRenderingSystem::CreateOffscreenContext()
{
  if (!m_bUsesDirectRenderToFinalTargetContext)
  {
    // Create offscreen render target
    VisRenderableTextureConfig_t config;
    GetRenderTargetConfig(config);
    m_spOffscreenRenderTarget = Vision::TextureManager.CreateRenderableTexture("<OffscreenBuffer>", config, VTM_FLAG_FORCETYPEDSURFACE);
    VASSERT(m_spOffscreenRenderTarget);
    m_spOffscreenRenderTarget->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);

    // Create offscreen depth stencil target
    // TODO: If the final target context has a depth-stencil target, we could use that one to save some space!
    GetDepthStencilConfig(config);
    m_spOffscreenDepthStencilTarget = Vision::TextureManager.CreateRenderableTexture("<OffscreenDepthStencilBuffer>", config);
    VASSERT(m_spOffscreenDepthStencilTarget);
    m_spOffscreenDepthStencilTarget->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);
  }

  // Create offscreen render context
  m_spOffscreenContext = new VisRenderContext_cl();

  int iFlags = VIS_RENDERCONTEXT_FLAGS_ALLFEATURES;
  m_spOffscreenContext->SetRenderFlags(iFlags);

  m_spOffscreenContext->SetCamera(GetFinalTargetContext()->GetCamera());
  m_spOffscreenContext->SetViewProperties(GetFinalTargetContext()->GetViewProperties());

  int iViewportPosX, iViewportPosY, iViewportSizeX, iViewportSizeY;
  GetFinalTargetContext()->GetViewport(iViewportPosX, iViewportPosY, iViewportSizeX, iViewportSizeY);
  m_spOffscreenContext->SetViewport(iViewportPosX, iViewportPosY, iViewportSizeX, iViewportSizeY);

  if (m_bUsesDirectRenderToFinalTargetContext)
  {
    m_spOffscreenContext->SetRenderAndDepthStencilTargets(GetFinalTargetContext());
  }
  else
  {
    m_spOffscreenContext->SetRenderTarget(0, m_spOffscreenRenderTarget);
    m_spOffscreenContext->SetDepthStencilTarget(m_spOffscreenDepthStencilTarget); 
  }

  VMobileForwardRenderLoop *pRenderLoop = new VMobileForwardRenderLoop(this);
  pRenderLoop->InitRenderLoop();
  m_spOffscreenContext->SetRenderLoop(pRenderLoop);

  IVisVisibilityCollector_cl *pVisColl = GetFinalTargetContext()->GetVisibilityCollector();
  if (!pVisColl)
  {
    pVisColl = new VisionVisibilityCollector_cl;
    static_cast<VisionVisibilityCollector_cl*>(pVisColl)->SetBehaviorFlags(VIS_VISCOLLECTOR_USEPORTALS | VIS_VISCOLLECTOR_USEFOV); 
  }

  pVisColl->SetInterleavedTranslucencySorter(UseInterleavedTranslucencySorting? (new VisionTranslucencySorter()) : NULL);

  GetFinalTargetContext()->SetVisibilityCollector(pVisColl, false);
  m_spOffscreenContext->SetVisibilityCollector(pVisColl, true);
  pVisColl->SetOcclusionQueryRenderContext(m_spOffscreenContext);

  m_spOffscreenContext->SetUserData(this); 
  m_spOffscreenContext->SetPriority(VIS_RENDERCONTEXTPRIORITY_SCENE);
  m_spOffscreenContext->SetUsageHint((VisContextUsageHint_e)VIS_CONTEXTUSAGE_VIEWCONTEXT);

  m_spOffscreenContext->SetName("MobileForwardRenderer");

  this->AddContext(m_spOffscreenContext);
}

void VMobileForwardRenderingSystem::RemoveOffscreenContext()
{
  this->RemoveContext(m_spOffscreenContext);
  m_spOffscreenContext->SetRenderTarget(0, NULL);
  m_spOffscreenContext->SetDepthStencilTarget(NULL);
  m_spOffscreenContext = NULL;
  m_spOffscreenRenderTarget = NULL;
  m_spOffscreenDepthStencilTarget = NULL;
}

void VMobileForwardRenderingSystem::SetRequiresOffscreenContext(void* pObject, bool bStatus) 
{ 
  VASSERT(pObject != NULL);

  if (bStatus)
  {
    bool bValue;
    if (m_objectsRequiringOffscreenContext.Lookup(pObject, bValue))
      return;

    m_objectsRequiringOffscreenContext.SetAt(pObject, bStatus);
  }
  else
  {
    m_objectsRequiringOffscreenContext.RemoveKey(pObject);
  }

  bool bOffscreenContextRequired = (m_objectsRequiringOffscreenContext.GetCount() > 0);
  if (bOffscreenContextRequired != m_bOffscreenContextRequired)
  {
    m_bOffscreenContextRequired = bOffscreenContextRequired;
    ReInitializeRenderer();
  }
}

bool VMobileForwardRenderingSystem::GetRequiresOffscreenContext() const
{ 
  return m_bOffscreenContextRequired;
}

void VMobileForwardRenderingSystem::GetRenderTargetConfig(VisRenderableTextureConfig_t &config)
{
  config.m_iWidth = m_iOffscreenContextSize[0];
  config.m_iHeight = m_iOffscreenContextSize[1];
  config.m_eFormat = VTextureLoader::DEFAULT_RENDERTARGET_FORMAT_32BPP;
  config.m_iMultiSampling = 0;
  config.m_bResolve = true;
}

void VMobileForwardRenderingSystem::GetDepthStencilConfig(VisRenderableTextureConfig_t &config)
{
  GetRenderTargetConfig(config);

  config.m_eFormat = VTextureObject::GetCompatibleDepthTextureFormat(*Vision::Video.GetCurrentConfig());
  if (config.m_eFormat == VTextureLoader::UNKNOWN)
  {
    config.m_eFormat = VVideo::GetSupportedDepthStencilFormat(VTextureLoader::D24S8, *Vision::Video.GetCurrentConfig());
    config.m_bRenderTargetOnly = true;
  }
  else
  {
    config.m_bRenderTargetOnly = false;
  }

#if defined(_VISION_WIIU)
  config.m_bResolve = true;
#else
  config.m_bResolve = false;
#endif
  config.m_bIsDepthStencilTarget = true; 
}

void VMobileForwardRenderingSystem::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if (pData->m_pSender == &Vision::Callbacks.OnUpdateSceneFinished)
  {
    // Make sure to disable height fog in the forward renderer
    VFogParameters fog = Vision::World.GetFogParameters();
    fog.heightFogMode = VFogParameters::Off;
    Vision::World.SetFogParameters(fog);
  }
  else if (pData->m_pSender==&Vision::Callbacks.OnWorldDeInit)
  {
    DestroySky();
    DeInitializeRenderer();
  }
  else if(pData->m_pSender == &Vision::Callbacks.OnEnterForeground)
  {
    if (m_bUsesDirectRenderToFinalTargetContext)
    {
      // Reassign render targets (in case they're copied from the final target render context).
      if (m_spOffscreenContext != NULL)
        m_spOffscreenContext->SetRenderAndDepthStencilTargets(GetFinalTargetContext());
    }
  }

  VRendererNodeCommon::OnHandleCallback(pData);
}

void VMobileForwardRenderingSystem::Resize(int iNewWidth, int iNewHeight)
{
  if (IsInitialized())
    ReInitializeRenderer();
}

VisRenderContext_cl *VMobileForwardRenderingSystem::GetReferenceContext()
{
  return m_spOffscreenContext;
}

VisRenderContext_cl *VMobileForwardRenderingSystem::GetTranslucencyReferenceContext()
{
  return GetReferenceContext();
}

VisRenderContext_cl *VMobileForwardRenderingSystem::GetFinalTargetContext()
{
  return m_spFinalTargetContext;
}

void VMobileForwardRenderingSystem::SetFinalTargetContext(VisRenderContext_cl *pContext)
{
  VASSERT_MSG(!IsInitialized(), "The mobile forward renderer must be explicitly deinitialized before changing the final target context.");

  ScratchTexturePool_cl::GlobalManager().PurgeUnusedTextures();
  m_spFinalTargetContext = pContext;
}

VTextureObject *VMobileForwardRenderingSystem::GetPostProcessColorTarget(VRenderTargetVersion_e eVersion) 
{ 
  return GetReferenceContext()->GetRenderTarget(0);
}

VTextureObject *VMobileForwardRenderingSystem::GetPostProcessDepthStencilTarget(VRenderTargetVersion_e eVersion) 
{
  return GetReferenceContext()->GetDepthStencilTarget();
}

VTextureObject *VMobileForwardRenderingSystem::GetGBuffer(VGBufferTarget_e eTarget) 
{
  switch (eTarget)
  {
  case VGBT_Accumulation:
  case VGBT_Diffuse:
    return GetReferenceContext()->GetRenderTarget(0);
  case VGBT_Normal:
    return NULL;
  case VGBT_DepthSpec:
    return GetDepthTexture(VRTV_RESOLVED); // currently returns NULL
  case VGBT_DepthStencil:
    return GetReferenceContext()->GetDepthStencilTarget();
  };
  return NULL; 
}

int VMobileForwardRenderingSystem::GetSupportedBufferFlags()
{
  return VBUFFERFLAG_FINALCOLOR;
}

V_IMPLEMENT_SERIAL(VMobileForwardRenderingSystem, VRendererNodeCommon, 0, &g_VisionEngineModule );

void VMobileForwardRenderingSystem::Serialize( VArchive &ar )
{
  char iLocalVersion = VMOBILE_FORWARDRENDERER_VERSION_CURRENT;
  if (ar.IsLoading())
  {
    ar >> iLocalVersion; 
    VASSERT_MSG(iLocalVersion <= VMOBILE_FORWARDRENDERER_VERSION_CURRENT, "Mobile Forward Rendering: Invalid version number");
  
    int iGammaCorrection;
    ar >> iGammaCorrection;
    GammaCorrection = (VGammaCorrection_e)iGammaCorrection;

    if (iLocalVersion >= VMOBILE_FORWARDRENDERER_VERSION_2)
    {
      int iResMode;
      ar >> iResMode;
      RenderingResolutionMode = (VRendererResolutionMode_e)iResMode;
      ar >> DesiredRenderingDpi;
      ar >> UseUpscaleFiltering;

      if (iLocalVersion >= VMOBILE_FORWARDRENDERER_VERSION_3)
        ar >> ResolutionTolerance;

      if (iLocalVersion >= VMOBILE_FORWARDRENDERER_VERSION_4)
        ar >> UseInterleavedTranslucencySorting;
    }

    if (iLocalVersion >= VMOBILE_FORWARDRENDERER_VERSION_1)
    {
      VRendererNodeCommon::Serialize(ar);
    }
    else
    {
      IVRendererNode::Serialize(ar);
    }
  }
  else
  {
    ar << iLocalVersion;
    ar << (int)GammaCorrection; 
    ar << (int)RenderingResolutionMode;
    ar << DesiredRenderingDpi;
    ar << UseUpscaleFiltering;
    ar << ResolutionTolerance;
    ar << UseInterleavedTranslucencySorting;
    VRendererNodeCommon::Serialize(ar);
  }
}

void VMobileForwardRenderingSystem::OnPostProcessorChanged()
{
  if(m_iPostProcessorUpdateCounter == 0 && IsInitialized())
  {
    ReInitializeRenderer();
  }
  else
  {
    m_bPostProcessorAssignmentDirty = true;
  }

}

void VMobileForwardRenderingSystem::SetGammaCorrection(VGammaCorrection_e gammaCorrection)
{
  VASSERT_MSG(!IsInitialized(), "The mobile forward renderer must be explicitly deinitialized before changing the gamma settings.");
  GammaCorrection = gammaCorrection;
}

bool VMobileForwardRenderingSystem::IsUsingUpscaling() const
{
  return m_Components.GetComponentOfType<VPostProcessUpscale>() != NULL;
}


START_VAR_TABLE(VMobileForwardRenderingSystem, VRendererNodeCommon, "VMobileForwardRenderingSystem", 0, "")  
  // OpenGL ES 2.0 does only support hardware gamma-correction via the GL_EXT_sRGB extension. Since support for this extension seems to be not common among current mobile devices,
  // all related sRGB conversions would have to be done manually in the shader code. Therefore for time being GammaCorrection is not exposed in vForge. 
  // DEFINE_VAR_ENUM(VMobileForwardRenderingSystem, GammaCorrection, "This setting determines whether gamma correction is applied, and if so, whether it's applied before or after the color transformations (contrast, saturation, etc). 'PreTransform' is the more common option.", "Off", "Off,PreTransform,PostTransform", 0, 0);
  DEFINE_VAR_ENUM(VMobileForwardRenderingSystem, RenderingResolutionMode, "This setting specifies the rendering resolution mode. When FULLSIZE, rendering is always performed at the device's full resolution. When QUARTERSIZE, rendering is performed at a quarter of the display resolution. DPI allows you to specify a desired target resolution.", "FULLSIZE", "FULLSIZE,QUARTERSIZE,DPI", 0, 0);
  DEFINE_VAR_FLOAT(VMobileForwardRenderingSystem, DesiredRenderingDpi, "If the the resolution mode is set to DPI, you can specify the desired target resolution in DPI with this value.", "150", 0, "Clamp(0,600)"); 
  DEFINE_VAR_FLOAT(VMobileForwardRenderingSystem, ResolutionTolerance, "Defines the maximum tolerance for matching rendering and display resolution. If e.g. set to 10%,  and the resolutions differ by less than 10%, the native display resolution will be used.", "30", 0, "Clamp(1,100)"); 
  DEFINE_VAR_BOOL(VMobileForwardRenderingSystem, UseUpscaleFiltering, "If rendering resolution and display resolution differ, this setting controls whether bilinear filtering should be used for scaling the image.", "TRUE", 0, 0);
  DEFINE_VAR_BOOL(VMobileForwardRenderingSystem, UseInterleavedTranslucencySorting, "Using interleaved sorting of translucent objects increases visual appearance since all kind of translucent objects are sorted instead of each individual type within each self, but may slow down performance.", "FALSE", 0, 0); 
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
