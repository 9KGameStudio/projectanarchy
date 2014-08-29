/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RenderingHelpers/ScratchTexturePool.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/MobileForwardRenderer/VMobileForwardRenderer.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/VLightShafts.hpp>

#define DEFAULT_LOOKUP_TEXTURE "\\flare13.dds"

V_IMPLEMENT_SERIAL(VLightShafts, IVObjectComponent, 0, &g_VisionEngineModule);

VLightShafts::VLightShafts(int iComponentFlags): 
  IVObjectComponent(0, iComponentFlags), 
  Enabled(TRUE),
  Intensity(1.0f),
  Radius(0.2f),
  DepthThreshold(10.0f),
  LightShaftLength(0.5f),
  m_bIsInitialized(false),  
  m_iVisibleBitmask(0xffffffff),
  m_vPosition(0.0f),
  m_bounds(0.0f),
  m_vColorDepthThreshold(0.0f),
  m_vHalfSizeExtents(0.0f)
{
  // Set a default texture so that code generated light shafts have a reasonable texture set, when
  // no texture is explicitly specified by the user.
  SetLookupTexture(DEFAULT_LOOKUP_TEXTURE); 
}

void VLightShafts::Init()
{
  m_spMaskRenderContext = new VisRenderContext_cl(NULL, 90.f, 90.f, 1, 1, 1.0f, 1000.0f);
  m_spMaskRenderContext->SetVisibilityCollector(NULL);
  m_spMaskRenderContext->SetName("LightShaftsMask");

  for (unsigned int i=0; i<2; i++)
  {
    m_spBlurRenderContexts[i] = new VisRenderContext_cl(NULL, 90.f, 90.f, 1, 1, 1.0f, 1000.0f);
    m_spBlurRenderContexts[i]->SetVisibilityCollector(NULL);
    m_spBlurRenderContexts[i]->SetName((i==0) ? "LightShaftsBlur0" : "LightShaftsBlur1");
  }

  m_bIsInitialized = CreateTextures() && CreateShaders();

  Vision::Callbacks.OnRenderHook += this;
  Vision::Callbacks.OnRendererNodeChanged += this;
  Vision::Callbacks.OnRendererNodeSwitching += this;

  if(Vision::Editor.IsInEditor())
    Vision::Callbacks.OnReassignShaders += this;

  // Light shafts require to sample the depth buffer.
  for(int i = 0; i < Vision::Renderer.GetRendererNodeCount(); i++)
  {
    if(VRendererNodeCommon* pRendererCommon = vdynamic_cast<VRendererNodeCommon*>(Vision::Renderer.GetRendererNode(i)))
    {
      pRendererCommon->SetRequiresOffscreenContext(this, true);
      pRendererCommon->SetRequiresDepthTexture(this, true, NULL);
    }
  }
}

void VLightShafts::DeInit()
{
  m_spLookupTextureAnim = NULL;
  m_spLookupTexture = NULL;

  m_spMaskTexture = NULL;
  m_spMaskRenderContext = NULL;
  for (unsigned int i=0; i<2; i++)
  {
    m_spBlurTextures[i] = NULL;
    m_spBlurRenderContexts[i] = NULL;
  }
  m_spGenMaskTechnique = NULL;
  m_spRadialBlurTechnique = NULL;
  m_spApplyTechnique = NULL;

  Vision::Callbacks.OnRenderHook -= this;
  Vision::Callbacks.OnRendererNodeChanged -= this;
  Vision::Callbacks.OnRendererNodeSwitching -= this;

  if(Vision::Editor.IsInEditor())
    Vision::Callbacks.OnReassignShaders -= this;

  // Depth buffer sampling not required anymore by this instance. However, to avoid a reinitialization of the renderer nodes,
  // only inform renderer node when inside vForge.
  if (Vision::Editor.IsInEditor())
  {
    for(int i = 0; i < Vision::Renderer.GetRendererNodeCount(); i++)
    {
      if(VRendererNodeCommon* pRendererCommon = vdynamic_cast<VRendererNodeCommon*>(Vision::Renderer.GetRendererNode(i)))
      {
        pRendererCommon->SetRequiresOffscreenContext(this, false);
        pRendererCommon->SetRequiresDepthTexture(this, false, NULL);
      }
    }
  }
}

bool VLightShafts::CreateTextures()
{
  int iRequiredWidth = 0;
  int iRequiredHeight = 0;
  const int iRendererNodeCount = Vision::Renderer.GetRendererNodeCount();
  for (int i=0; i<iRendererNodeCount; i++)
  {
    IVRendererNode *pRendererNode = Vision::Renderer.GetRendererNode(i);
    if (pRendererNode != NULL)
    {
      int iWidth, iHeight;
      VisRenderContext_cl *pContext = pRendererNode->GetTranslucencyReferenceContext();
      if (pContext == NULL)
        continue;
      pContext->GetSize(iWidth, iHeight);
      if (iWidth > iRequiredWidth)
        iRequiredWidth = iWidth;
      if (iHeight > iRequiredHeight)
        iRequiredHeight = iHeight;
    }
  }
  iRequiredHeight = (iRequiredHeight+1) >> 2;
  iRequiredWidth = (iRequiredWidth+1) >> 2; 
  if ((iRequiredWidth == 0) || (iRequiredHeight == 0))
    return false;

  m_spMaskTexture = ScratchTexturePool_cl::GlobalManager().GetScratchTexture(iRequiredWidth*2, iRequiredHeight*2, VTextureLoader::DEFAULT_RENDERTARGET_FORMAT_32BPP, 0, 0, false, 0, 0, false);
  m_spMaskRenderContext->SetRenderTarget(0, m_spMaskTexture);
  m_spBlurTextures[0] = ScratchTexturePool_cl::GlobalManager().GetScratchTexture(iRequiredWidth, iRequiredHeight, VTextureLoader::DEFAULT_RENDERTARGET_FORMAT_32BPP, 0, 0, false, 0, 0, false);
  m_spBlurRenderContexts[0]->SetRenderTarget(0, m_spBlurTextures[0]);
  m_spBlurTextures[1] = ScratchTexturePool_cl::GlobalManager().GetScratchTexture(iRequiredWidth, iRequiredHeight, VTextureLoader::DEFAULT_RENDERTARGET_FORMAT_32BPP, 1, 0, false, 0, 0, false);
  m_spBlurRenderContexts[1]->SetRenderTarget(0, m_spBlurTextures[1]);
  ScratchTexturePool_cl::GlobalManager().PurgeUnusedTextures();

  return true;
}

bool VLightShafts::CreateShaders(bool bForce)
{
  if((!bForce) && (m_spGenMaskTechnique != NULL) && (m_spRadialBlurTechnique != NULL) && (m_spApplyTechnique != NULL))
  {
    return true;
  }

  bool bResult = Vision::Shaders.LoadShaderLibrary("\\Shaders\\LightShafts.ShaderLib", SHADERLIBFLAG_HIDDEN) != NULL;
  VVERIFY_OR_RET_VAL(bResult, false); 

  m_spGenMaskTechnique = Vision::Shaders.CreateTechnique("GenMask", NULL);
  VVERIFY_OR_RET_VAL(m_spGenMaskTechnique != NULL, false);
  m_regColorDepthThreshold.Init(m_spGenMaskTechnique->GetShader(0), "ColorDepthThreshold");
  m_regBounds.Init(m_spGenMaskTechnique->GetShader(0), "Bounds");
#ifdef _VR_DX9
  m_regHalfTexelOffset.Init(m_spGenMaskTechnique->GetShader(0), "HalfTexelOffset");
#endif

  m_spRadialBlurTechnique = Vision::Shaders.CreateTechnique("RadialBlur", NULL);
  VVERIFY_OR_RET_VAL(m_spRadialBlurTechnique != NULL, false);
  m_regPositionLength.Init(m_spRadialBlurTechnique->GetShader(0), "PositionLength");
   
  m_spApplyTechnique = Vision::Shaders.CreateTechnique("Apply", NULL);
  VVERIFY_OR_RET_VAL(m_spApplyTechnique!=NULL, false); 

  return true;
}

VisLightSource_cl* VLightShafts::GetLightSource() const
{
  VisTypedEngineObject_cl *pOwner = GetOwner();
  if (pOwner == NULL)
    return NULL;

  VisLightSource_cl *pLight = vdynamic_cast<VisLightSource_cl*>(pOwner);
  VASSERT((pLight != NULL) && ((pLight->GetType() == VIS_LIGHT_POINT) || (pLight->GetType() == VIS_LIGHT_SPOTLIGHT) || (pLight->GetType() == VIS_LIGHT_DIRECTED)));
  
  return pLight;
}

void VLightShafts::SetLookupTexture(VTextureObject *pTex) 
{
  m_spLookupTexture = pTex;
  m_spLookupTextureAnim = Vision::TextureManager.RegisterTextureAnimation(m_spLookupTexture);
}

void VLightShafts::SetLookupTexture(const char *szFilename) 
{
  SetLookupTexture(Vision::TextureManager.Load2DTexture(szFilename));
}

void VLightShafts::OnRender(VisRenderContext_cl *pContext)
{
  if ((!m_bIsInitialized) || (Enabled == FALSE))
    return;

  IVRendererNode *pRendererNode = Vision::Renderer.GetCurrentRendererNode();
  if ((pRendererNode == NULL) || (pRendererNode->GetTranslucencyReferenceContext() != pContext))
    return;
  if ((pContext->GetRenderFilterMask() & m_iVisibleBitmask) == 0)
    return;

  VRendererNodeCommon *pRenderNodeCommon = NULL;
  bool bNativeDepthBuffer = false;
  if (pRendererNode->IsOfType(VRendererNodeCommon::GetClassTypeId()))
  {
    pRenderNodeCommon = static_cast<VRendererNodeCommon*>(pRendererNode);

    // For VMobileForwardRenderingSystem the native depth buffer is used.
    bNativeDepthBuffer = (pRenderNodeCommon->IsOfType(VMobileForwardRenderingSystem::GetClassTypeId()) == TRUE);
  }
  else 
  {
    return;
  }

  if (Update(pContext, bNativeDepthBuffer))
  {
    VTextureObject *pDepthTexture = GetDepthTexture(pRenderNodeCommon, bNativeDepthBuffer);
    if (pDepthTexture != NULL)
    {
      INSERT_PERF_MARKER_SCOPE("VLightShafts");

      VisRenderContextPtr spContextBackup = Vision::Contexts.GetCurrentContext();

      GenerateMaskPass(pContext, pDepthTexture);
      RadialBlurPass();

      spContextBackup->Activate();
      spContextBackup->PerformPendingContextSwitch();
      spContextBackup = NULL;

      ApplyPass();
    }
  }  
  else
  {
    pRendererNode->SetRequiresDepthTexture(this, false, NULL);
  }
}

VTextureObject* VLightShafts::GetDepthTexture(VRendererNodeCommon *pRendererNode, bool bNativeDepthBuffer) 
{
  if (bNativeDepthBuffer)
  {
    VTextureObject *pTexture = pRendererNode->GetPostProcessDepthStencilTarget(VRTV_RESOLVED);
    if (pTexture == NULL)
      return NULL;
    if (VVideo::IsTextureFormatSupported(pTexture->GetTextureFormat(), *Vision::Video.GetCurrentConfig()))
    {
#ifdef _VR_GLES2
      // Ensure that depth-stencil buffer is not discarded on OpenGL ES 2.0 devices.
      VisRenderableTexture_cl* pRenderableTexture = static_cast<VisRenderableTexture_cl*>(pTexture);
      pRenderableTexture->EnableDepthStencilBufferDiscard(false);
#endif 
      return pTexture;
    }
    return NULL;
  }
  else
  {
    if (!pRendererNode->SetRequiresDepthTexture(this, true, NULL))
      return NULL;
    return pRendererNode->GetDepthTexture(VRTV_RESOLVED);
  }
}

bool VLightShafts::Update(VisRenderContext_cl *pContext, bool bNativeDepthBuffer) 
{
  VisLightSource_cl *pLight = GetLightSource();
  if (pLight == NULL)
    return false; 

  // calculate virtual light position
  hkvVec3 vLightPosition;
  pLight->GetVirtualPosition(vLightPosition, pContext);

  // cull effect when camera is back facing
  hkvVec3 vLightVec = pContext->GetCamera()->GetPosition()-vLightPosition;
  vLightVec.normalizeIfNotZero();
  if (vLightVec.dot(-pContext->GetCamera()->GetDirection()) < 0.0f)
    return false;
  
  // calculate effect center in texture space
  hkvMat4 projMatrix = pContext->GetViewProperties()->getProjectionMatrix(hkvClipSpaceYRange::MinusOneToOne);
  hkvMat4 viewMatrix = pContext->GetCamera()->GetWorldToCameraTransformation();
  hkvVec4 vLightPositionVS = viewMatrix*vLightPosition.getAsVec4(1.0f);
  hkvVec4 vLightPositionTS = projMatrix*vLightPositionVS;
  vLightPositionTS =  (vLightPositionTS.w != 0.0f) ? (vLightPositionTS/ vLightPositionTS.w) : vLightPositionTS;
  m_vPosition.x = vLightPositionTS.x * 0.5f + 0.5f;
  m_vPosition.y = -vLightPositionTS.y * 0.5f + 0.5f;

  // get half size screen space extents
  int iWidth, iHeight;
  pContext->GetSize(iWidth, iHeight);
  m_vHalfSizeExtents.x = float((iWidth + 1) >> 1);
  m_vHalfSizeExtents.y = float((iHeight + 1) >> 1);

  // calculate bounds of effect in texture space and account for aspect ratio
  float fAspectRatio = m_vHalfSizeExtents.x/ m_vHalfSizeExtents.y;
  hkvVec2 mins(m_vPosition.x-Radius, m_vPosition.y-Radius*fAspectRatio);
  hkvVec2 maxes(m_vPosition.x+Radius, m_vPosition.y+Radius*fAspectRatio);

  // set bounds that is used to fetch the lookup texture
  m_bounds.set(mins.x, mins.y, 1.0f/(maxes.x-mins.x), 1.0f/(maxes.y-mins.y));

  // calculate scissor rectangle
  mins.clampTo(hkvVec2(0.0f, 0.0f), hkvVec2(1.0f, 1.0f));
  maxes.clampTo(hkvVec2(0.0f, 0.0f), hkvVec2(1.0f, 1.0f));
  m_scissorRect.m_vMin = mins.compMul(m_vHalfSizeExtents);
  m_scissorRect.m_vMax = maxes.compMul(m_vHalfSizeExtents);

  // cull effect when outside of screen
  if(!m_scissorRect.Touches(VRectanglef(HKVMATH_DEFAULT_EPSILON, HKVMATH_DEFAULT_EPSILON, m_vHalfSizeExtents.x-HKVMATH_DEFAULT_EPSILON, 
     m_vHalfSizeExtents.y-HKVMATH_DEFAULT_EPSILON)))
  {
    return false;
  }

  // calculate color of effect pre-multiplied with intensity
  hkvVec3 vColor;
  pLight->GetColor().ToFloat(vColor);
  vColor *= Intensity;

  // calculate depth threshold in native, respectively linear depth range
  VisRenderContext_cl *pRefContext = Vision::Renderer.GetCurrentRendererNode()->GetTranslucencyReferenceContext();
  float fNearClipDist, fFarClipDist;
  pRefContext->GetClipPlanes(fNearClipDist, fFarClipDist);
  float fDepthThreshold = vLightPositionVS.z-DepthThreshold;
  if (bNativeDepthBuffer)
    fDepthThreshold = fFarClipDist/(fFarClipDist-fNearClipDist) - (fNearClipDist/fDepthThreshold);
  else
    fDepthThreshold = fDepthThreshold/(fFarClipDist-fNearClipDist);

  m_vColorDepthThreshold.set(vColor.x, vColor.y, vColor.z, fDepthThreshold);

  return true;
}

void VLightShafts::GenerateMaskPass(VisRenderContext_cl *pContext, VTextureObject *pDepthTexture)
{
  VisRenderContext_cl* pMainContext = Vision::Contexts.GetCurrentContext();
  VisLightSource_cl *pLight = GetLightSource();
  VASSERT(pLight != NULL);

  VCompiledShaderPass *pGenMaskPass = NULL;
  VisMeshBuffer_cl *pMeshBuffer = NULL;
  hkvMat4 mMeshTransform(hkvNoInitialization);
  if (pLight->GetType() == VIS_LIGHT_SPOTLIGHT) 
  {
    pMeshBuffer = Vision::Renderer.GetCurrentRendererNode()->GetRendererNodeHelper()->GetConeMeshBuffer();
    hkvMat3 mRot = pLight->GetRotationMatrix();
    hkvVec3 vPos = pLight->GetPosition();
    mMeshTransform.set(mRot, vPos);

    // Cone length must be large enough so that cone spans the entire screen to avoid harsh cut-offs of
    // light shafts. However it should not be unnecessary exaggerated to take advantage of guard-band
    // clipping. Therefor the distance of viewer to a far frustum corner will be taken as approximation.
    hkvVec3 vFarCorners[4];
    Vision::Renderer.GetCurrentRendererNode()->GetRendererNodeHelper()->GetFrustumFarCorners(vFarCorners);
    float fConeLength = vFarCorners[0].getLength();

    float fConeRadius = fConeLength * hkvMath::tanDeg(pLight->GetProjectionAngle()*0.5f);
    mMeshTransform.m_ElementsCM[0] *= fConeLength;
    mMeshTransform.m_ElementsCM[1] *= fConeLength;
    mMeshTransform.m_ElementsCM[2] *= fConeLength;
    mMeshTransform.m_ElementsCM[4] *= fConeRadius;
    mMeshTransform.m_ElementsCM[5] *= fConeRadius;
    mMeshTransform.m_ElementsCM[6] *= fConeRadius;
    mMeshTransform.m_ElementsCM[8] *= fConeRadius;
    mMeshTransform.m_ElementsCM[9] *= fConeRadius;
    mMeshTransform.m_ElementsCM[10] *= fConeRadius;

    // If camera is in front of spot light cone use front cull, otherwise back cull. In this way each screen pixel will
    // be only processed once by the corresponding shader. Furthermore when camera is behind spot light disable masking
    // in order to avoid false light blocking.
    hkvVec3 vLightVec = pContext->GetCamera()->GetPosition()-vPos;
    vLightVec.normalizeIfNotZero();
    bool bUseBackCull = (pLight->GetDirection().dot(vLightVec) < 0.0f);
    pGenMaskPass = bUseBackCull ? m_spGenMaskTechnique->GetShader(2) : m_spGenMaskTechnique->GetShader(1);
  }
  else
  {
    pGenMaskPass = m_spGenMaskTechnique->GetShader(0);
    pMeshBuffer = Vision::Renderer.GetCurrentRendererNode()->GetRendererNodeHelper()->GetFrustumMeshBuffer();
    mMeshTransform.setIdentity();
  }

  m_regColorDepthThreshold.SetRegisterValueSafeF(pGenMaskPass, m_vColorDepthThreshold.data);
  m_regBounds.SetRegisterValueSafeF(pGenMaskPass, m_bounds.data);
#ifdef _VR_DX9 
  hkvVec4 vHalfTexelOffset(0.5f/m_vHalfSizeExtents.x, 0.5f/m_vHalfSizeExtents.y, 0.0f, 0.0f);
  m_regHalfTexelOffset.SetRegisterValueSafeF(pGenMaskPass, vHalfTexelOffset.data);
#endif

  m_spMaskRenderContext->SetCamera(pMainContext->GetCamera());
  m_spMaskRenderContext->SetViewProperties(pMainContext->GetViewProperties());
  m_spMaskRenderContext->Activate();
  m_spMaskRenderContext->PerformPendingContextSwitch();

  Vision::RenderLoopHelper.ClearScreen(VisRenderLoopHelper_cl::VCTF_Color, VColorRef(0, 0, 0, 0));
  Vision::RenderLoopHelper.BeginMeshRendering();
  Vision::RenderLoopHelper.ResetMeshStreams(); 
  Vision::RenderLoopHelper.SetMeshTransformationMatrix(mMeshTransform);
  Vision::RenderLoopHelper.AddMeshStreams(pMeshBuffer, pGenMaskPass->GetStreamMask () | VERTEX_STREAM_INDEXBUFFER);
 
  VTextureObject *pLookupTexture = (m_spLookupTextureAnim!=NULL) ? m_spLookupTextureAnim->GetCurrentFrame() : m_spLookupTexture.GetPtr();
  Vision::RenderLoopHelper.BindMeshTexture(pLookupTexture, 0, (VStateGroupSampler*)VisRenderStates_cl::GetSamplerStateGroup((VisSamplerState_e)(VIS_SAMPLER_CLAMP|VIS_SAMPLER_FILTERING)));
  Vision::RenderLoopHelper.BindMeshTexture(pDepthTexture, 1, (VStateGroupSampler *)VisRenderStates_cl::GetSamplerStateGroup((VisSamplerState_e)(VIS_SAMPLER_CLAMP|VIS_SAMPLER_FILTERING)));
  
  Vision::RenderLoopHelper.SetScissorRect(&m_scissorRect);

  Vision::RenderLoopHelper.RenderMeshes(pGenMaskPass, VisMeshBuffer_cl::MB_PRIMTYPE_INDEXED_TRILIST, 0, pMeshBuffer->GetIndexCount()/3, pMeshBuffer->GetVertexCount());

  Vision::RenderLoopHelper.SetScissorRect(NULL);

  Vision::RenderLoopHelper.EndMeshRendering();
}

void VLightShafts::RadialBlurPass()
{
  for (unsigned int i=0; i<4; i++)
  {
    VCompiledShaderPass *pRadialBlurPass = m_spRadialBlurTechnique->GetShader(i);
    hkvVec4 vPositionLength(m_vPosition.x, m_vPosition.y, LightShaftLength, 0.0f);
    m_regPositionLength.SetRegisterValueSafeF(pRadialBlurPass, vPositionLength.data);

    unsigned int iContext = (i < 2) ? 0 : 1;
    m_spBlurRenderContexts[iContext]->Activate();
    m_spBlurRenderContexts[iContext]->PerformPendingContextSwitch();

    hkvVec2 vTexCoordsMin(0.0f, 0.0f);
    hkvVec2 vTexCoordsMax(1.0f, 1.0f);
#if defined(_VR_GLES2) 
    vTexCoordsMin.y = 1.0f;
    vTexCoordsMax.y = 0.0f;
#endif
#ifdef _VR_DX9
   hkvVec2 vHalfTexelOffset(1.0f/m_vHalfSizeExtents.x, 1.0f/m_vHalfSizeExtents.y);
   vTexCoordsMin += vHalfTexelOffset;
   vTexCoordsMax += vHalfTexelOffset;
#endif

    VTextureObject *pTexture = (i < 2) ? m_spMaskTexture : m_spBlurTextures[0];
    IVRender2DInterface* pRI = Vision::RenderLoopHelper.BeginOverlayRendering();
    pRI->DrawTexturedQuadWithShader(hkvVec2(0.0f, 0.0f), m_vHalfSizeExtents*0.5f, pTexture, vTexCoordsMin, vTexCoordsMax, 
                                    V_RGBA_WHITE, *pRadialBlurPass);
    Vision::RenderLoopHelper.EndOverlayRendering();
  }
}

void VLightShafts::ApplyPass()
{
  VCompiledShaderPass *pApplyPass = m_spApplyTechnique->GetShader(0);

  int iWidth, iHeight;
  VisRenderContext_cl *pContext = Vision::Contexts.GetCurrentContext();
  pContext->GetSize(iWidth, iHeight);

  hkvVec2 vTexCoordsMin(0.0f, 0.0f);
  hkvVec2 vTexCoordsMax(1.0f, 1.0f);
#if defined(_VR_GLES2) 
  vTexCoordsMin.y = 1.0f;
  vTexCoordsMax.y = 0.0f;
#endif
#ifdef _VR_DX9
  hkvVec2 vHalfTexelOffset(0.5f/(float)iWidth, 0.5f/(float)iWidth);
  vTexCoordsMin += vHalfTexelOffset;
  vTexCoordsMax += vHalfTexelOffset;
#endif

  IVRender2DInterface* pRI = Vision::RenderLoopHelper.BeginOverlayRendering();
  pRI->DrawTexturedQuadWithShader(hkvVec2(0.0f, 0.0f), hkvVec2((float)iWidth, (float)iHeight), m_spBlurTextures[1], 
                                  vTexCoordsMin, vTexCoordsMax, V_RGBA_WHITE, *pApplyPass);
  Vision::RenderLoopHelper.EndOverlayRendering();
}

void VLightShafts::SetOwner(VisTypedEngineObject_cl *pOwner)
{
  IVObjectComponent::SetOwner(pOwner);

  if (pOwner != NULL)
  {
    Init();
  }
  else
  {
    DeInit();
  }
}

BOOL VLightShafts::CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut)
{
  if (!IVObjectComponent::CanAttachToObject(pObject, sErrorMsgOut))
    return FALSE;

  if (!pObject->IsOfType(V_RUNTIME_CLASS(VisLightSource_cl)))
  {
    sErrorMsgOut = "Component can only be added to VisLightSource_cl or derived classes.";
    return FALSE;
  }

  return TRUE;
}

void VLightShafts::OnVariableValueChanged(VisVariable_cl *pVar, const char * value)
{
  if (!strcmp(pVar->name, "TextureFilename"))
  {
    VTextureObject *pTexture = (value && value[0]) ? Vision::TextureManager.Load2DTexture(value) : Vision::TextureManager.Load2DTexture("plainwhite.dds");
    SetLookupTexture(pTexture);
  }
}

void VLightShafts::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if (pData->m_pSender == &Vision::Callbacks.OnRenderHook)
  {
    if (((VisRenderHookDataObject_cl *)pData)->m_iEntryConst == VRH_CORONAS_AND_FLARES)
    {
      VisRenderContext_cl *pContext = Vision::Contexts.GetCurrentContext();
      OnRender(pContext);
    }
    return;
  } 

  if (pData->m_pSender == &Vision::Callbacks.OnRendererNodeChanged)
  {
    // Light shafts require to sample the depth buffer.
    if(VRendererNodeCommon* pRendererCommon = vdynamic_cast<VRendererNodeCommon*>(static_cast<VisRendererNodeChangedDataObject_cl *>(pData)->m_spAddedNode))
    {
      pRendererCommon->SetRequiresOffscreenContext(this, true);
      pRendererCommon->SetRequiresDepthTexture(this, true, NULL);
    }

    m_bIsInitialized = CreateTextures() && CreateShaders();

    return;
  }

  // When render node changes, the camera has to be synchronized with the camera of the active renderer node.
  if (pData->m_pSender == &Vision::Callbacks.OnRendererNodeSwitching) 
  {
    VisRendererNodeDataObject_cl &data = *((VisRendererNodeDataObject_cl*)pData);
    if (data.m_pRendererNode)
    {
      VisRenderContext_cl *pRefContext = data.m_pRendererNode->GetReferenceContext();
      if (pRefContext != NULL)
      {
        const VisContextCamera_cl *pCamera = pRefContext->GetCamera();
        m_spMaskRenderContext->SetCamera(pCamera);
        m_spBlurRenderContexts[0]->SetCamera(pCamera);
        m_spBlurRenderContexts[1]->SetCamera(pCamera);
      }
    }
    return;
  }

  if(pData->m_pSender == &Vision::Callbacks.OnReassignShaders)
  {
    CreateShaders(true);
    return; 
  }
}

void VLightShafts::Serialize(VArchive &ar)
{
  IVObjectComponent::Serialize(ar);

  char iLocalVersion = VLIGHT_SHAFTS_VERSION_CURRENT;
  if (ar.IsLoading())
  {
    ar >> iLocalVersion;
    VASSERT_MSG((iLocalVersion >= VLIGHT_SHAFTS_VERSION_0) && (iLocalVersion <= VLIGHT_SHAFTS_VERSION_CURRENT), "Invalid local version. Please re-export");
    ar >> Enabled;
    ar >> Intensity;
    ar >> Radius;
    ar >> DepthThreshold;
    ar >> LightShaftLength;
    SetLookupTexture((VTextureObject*)ar.ReadProxyObject());
  } 
  else
  {
    ar << iLocalVersion;
    ar << Enabled;
    ar << Intensity;
    ar << Radius;
    ar << DepthThreshold;
    ar << LightShaftLength;
    ar.WriteProxyObject(m_spLookupTexture);
  }
}

START_VAR_TABLE(VLightShafts, IVObjectComponent, "Light Shafts component", VVARIABLELIST_FLAGS_NONE, "Light Shafts")
  DEFINE_VAR_BOOL(VLightShafts, Enabled, "En-/Disable component", "TRUE", 0, NULL);
  DEFINE_VAR_FLOAT(VLightShafts, Intensity, "Intensity of the effect", "1.0", 0, "Min(0.0)");
  DEFINE_VAR_FLOAT(VLightShafts, Radius, "Radius of effect (texture space)", "0.2", 0, "Clamp(0.00001, 1.0)");
  DEFINE_VAR_FLOAT(VLightShafts, DepthThreshold, "Depth threshold (view space)", "10.0", 0, "Min(0.0)");
  DEFINE_VAR_FLOAT(VLightShafts, LightShaftLength, "Length of light shafts (texture space)", "0.5", 0, "Clamp(0.0, 1.0)");
  DEFINE_VAR_STRING_CALLBACK(VLightShafts, TextureFilename, "Filename of lookup texture", DEFAULT_LOOKUP_TEXTURE, DISPLAY_HINT_NONE, "assetpicker(Texture | 2D,Animated Texture)");
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
