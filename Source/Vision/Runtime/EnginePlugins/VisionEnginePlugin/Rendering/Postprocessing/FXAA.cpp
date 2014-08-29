/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/FXAA.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RenderingHelpers/RenderingOptimizationHelpers.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RendererNode/VRendererNodeCommon.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/MobileForwardRenderer/VMobileForwardRenderer.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RenderingHelpers/ScratchTexturePool.hpp>


VPostProcessFXAA::VPostProcessFXAA() : 
    VPostProcessingBaseComponent(),
    tempMasks(1,1)
{
  m_iRequiredBufferFlags = VBUFFERFLAG_FINALCOLOR;
  m_fPriority = VPOSTPROCESSOR_PRIORITY_SCREENMASKS - 100;
  m_bufferUsageFlags = SAMPLES_ACCUMULATION_BUFFER | DRAWS_FULLSCREEN_QUAD;
  Quality = QL_MediumDither_2;
}

VPostProcessFXAA::~VPostProcessFXAA()
{
}

void VPostProcessFXAA::RemoveContext()
{
  GetOwner()->RemoveContext(GetTargetContext());
}

void VPostProcessFXAA::SetupContext()
{
  VRendererNodeCommon *pRendererNode = GetOwner();
  VASSERT(pRendererNode != NULL && "Postprocessing effects require a valid renderer node!");
  VisRenderContext_cl *pRefContext = pRendererNode->GetReferenceContext();

  GetOwner()->AddContext(GetTargetContext());

  GetTargetContext()->SetName("FXAA:Output");

  m_spSourceTextures[0] = pRendererNode->GetGBuffer(IVRendererNode::VGBT_Accumulation);
  m_iNumSourceTextures = 1;
  VASSERT(m_spSourceTextures[0] != NULL);
}

// Returns true if we are the last component who reads from accumulation buffer
bool VPostProcessFXAA::IsLastComponent()
{
  if ( !GetOwner() )
    return false;

  const int count = GetOwner()->Components().Count();
  for ( int i = 0; i < count; ++i )
  {
    IVObjectComponent* comp = GetOwner()->Components().GetAt(i);
    if ( !comp || comp == this ) 
      continue;
    if ( comp->IsOfType(V_RUNTIME_CLASS(VPostProcessingBaseComponent)) )
    {
      VPostProcessingBaseComponent* ppComp = static_cast<VPostProcessingBaseComponent*>(comp);
      if ( (ppComp->GetBufferUsageFlags()&SAMPLES_ACCUMULATION_BUFFER)!=0 && ppComp->GetPriority() >= GetPriority() )
      {
        // we found a PP that is after us (bigger priority number) and reads from accum buffer, so we're not the last one
        return false;
      }
    }
  }
  return true;
}

void VPostProcessFXAA::InitializePostProcessor()
{
  if (m_bIsInitialized || !m_bActive)
    return;

  SetupContext();

  // Load glow shader library
  BOOL bResult = Vision::Shaders.LoadShaderLibrary("\\Shaders\\FXAA.ShaderLib", SHADERLIBFLAG_HIDDEN) != NULL;
  VASSERT(bResult); // file not found?

  GetTargetContext()->GetSize(m_iWidth, m_iHeight);

  m_spMask = new VisScreenMask_cl();
  m_spMask->SetPos(0,0);
  m_spMask->SetTargetSize((float)m_iWidth,(float)m_iHeight);
  m_spMask->SetTextureRange(0.0f, 0.0f, (float)m_iWidth, (float)m_iHeight);
#ifdef _VR_DX9
  m_spMask->SetUseOpenGLTexelShift(TRUE);
#else
  m_spMask->SetUseOpenGLTexelShift(FALSE);
#endif
  //m_spMask->SetUseOpenGLTexelShift(FALSE);

  m_spMask->SetTransparency(VIS_TRANSP_NONE);
  m_spMask->SetVisible(FALSE);
  m_spMask->SetDepthWrite(FALSE);
  m_spMask->SetWrapping(FALSE, FALSE);
  m_spMask->SetVisibleBitmask(0); // this mask is rendered manually via a collection

  // no wireframe for this mask
  VSimpleRenderState_t s = m_spMask->GetRenderState();
  s.SetFlag(RENDERSTATEFLAG_NOWIREFRAME);
  m_spMask->SetRenderState(s);

  VTechniqueConfig vc;

  VString tags;
  tags.Format("FXAA_PRESET=%d", (int)Quality);
  vc.SetInclusionTags(tags);

  VCompiledTechnique *pTech = Vision::Shaders.CreateTechnique("FXAA", NULL, &vc, EFFECTFLAGS_FORCEUNIQUE);
  VASSERT(pTech!=NULL && "Could not create technique for FXAA postprocessor!");
  m_spMask->SetTechnique(pTech);
  m_spMask->SetTransparency(VIS_TRANSP_NONE);

  VShaderConstantBuffer *pPS = pTech->GetShader(0)->GetConstantBuffer(VSS_PixelShader);
  m_iRegScreenSize = pPS->GetRegisterByName("rcpFrame");
  
  // make frame copy only if this is not the last PP
  bool bFrameCopy = !IsLastComponent();
  if (bFrameCopy && GetTargetContext()->GetRenderTarget() == m_spSourceTextures[0])
  {
    m_spFrameCopyTexture = ScratchTexturePool_cl::GlobalManager().GetScratchTexture(m_iWidth, m_iHeight, m_spSourceTextures[0]->GetTextureFormat(), 0);
    m_spMask->SetTextureObject(m_spFrameCopyTexture);
  }
  else
  {
    m_spFrameCopyTexture = NULL;
    m_spMask->SetTextureObject(m_spSourceTextures[0]);
  }

  m_bIsInitialized = true;
}

// ----------------------------------------------------------------------------------------
// Free all resources again
// ----------------------------------------------------------------------------------------
void VPostProcessFXAA::DeInitializePostProcessor()
{
  if (!m_bIsInitialized)
    return;

  m_spMask = NULL;
  RemoveContext();

  m_bIsInitialized = false;
}

void VPostProcessFXAA::Execute()
{
  if (!IsActive() || !m_bIsInitialized)
    return;
 
  INSERT_PERF_MARKER_SCOPE("FXAA");

  RenderingOptimizationHelpers_cl::SetShaderPreference(112);

  if (m_spFrameCopyTexture != NULL)
  {
    Vision::Renderer.CopyToTexture(m_spFrameCopyTexture, 0, 0, m_iWidth, m_iHeight);
  }

  VCompiledShaderPass *pPass = m_spMask->GetTechnique()->GetShader(0);
  VShaderConstantBuffer *pPS = pPass->GetConstantBuffer(VSS_PixelShader);

  hkvVec4 invScreenSize(1.0f / m_iWidth, 1.0f / m_iHeight, 0, 0);

  if (m_iRegScreenSize >= 0)
  {
    pPS->SetSingleRegisterF(m_iRegScreenSize, invScreenSize.data);
  }

  tempMasks.Clear();
  tempMasks.AppendEntryFast(m_spMask);
  Vision::RenderLoopHelper.RenderScreenMasks(tempMasks);
}

V_IMPLEMENT_SERIAL( VPostProcessFXAA, VPostProcessingBaseComponent, 0, &g_VisionEngineModule );

void VPostProcessFXAA::Serialize( VArchive &ar )
{
  if (ar.IsLoading())
  {
    ar >> m_bActive;

    unsigned int iQuality;
    ar >> iQuality;
    Quality = QualityLevel(iQuality);
  }
  else
  {
    ar << m_bActive;
    ar << (unsigned int)Quality;
  }
}

START_VAR_TABLE(VPostProcessFXAA, VPostProcessingBaseComponent, "VPostProcessFXAA", 0, "FXAA")
  DEFINE_VAR_ENUM(VPostProcessFXAA, Quality, "The quality setting is divided into three groups (medium dither, low dither, extreme), whereas the number indicates the quality within a group (higher number - better quality, lower number - better performance). Note: on consoles a specially optimized shader version is used for each platform, thus the quality setting does NOT apply (Off - Off, Others - On).", "MediumDither2", "Off,MediumDither0,MediumDither1,MediumDither2,MediumDither3,MediumDither4,MediumDither5,LowDither0,LowDither1,LowDither2,LowDither3,LowDither4,LowDither5,LowDither6,LowDither7,LowDither8,LowDither9,Extreme", 0, 0);
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
