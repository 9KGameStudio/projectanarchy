/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/VPostProcessUpscale.hpp>

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/MobileForwardRenderer/VMobileForwardRenderer.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RendererNode/VRendererNodeCommon.hpp>

V_IMPLEMENT_DYNAMIC(VPostProcessUpscale, VPostProcessingBaseComponent, &g_VisionEngineModule);

VPostProcessUpscale::VPostProcessUpscale( bool bUseFiltering ) : m_bUseFiltering(bUseFiltering)
{
  m_bufferUsageFlags = SAMPLES_ACCUMULATION_BUFFER | DRAWS_FULLSCREEN_QUAD;

  // Upscaling replaces simple copy, so we can use the same priority
  m_fPriority = VPOSTPROCESSOR_PRIORITY_SIMPLE_COPY;

  // The post processor is added dynamically based on screen resolution
  m_iComponentFlags = VIS_OBJECTCOMPONENTFLAG_NOSERIALIZE;  
}

void VPostProcessUpscale::Execute()
{
  INSERT_PERF_MARKER_SCOPE("VPostProcessUpscale::Execute");

  int iWidth, iHeight;
  VisRenderContext_cl *pContext = VisRenderContext_cl::GetCurrentContext();
  pContext->GetSize(iWidth, iHeight);

#if defined(_VR_DX9)
  const hkvVec2 texelShift(1.0f / (float)(iWidth*2), 1.0f / (float)(iHeight*2));
#else
  const hkvVec2 texelShift(0.0f, 0.0f);
#endif

  VSimpleRenderState_t iState(VIS_TRANSP_NONE,RENDERSTATEFLAG_FRONTFACE|RENDERSTATEFLAG_ALWAYSVISIBLE|RENDERSTATEFLAG_NOWIREFRAME|RENDERSTATEFLAG_NOMULTISAMPLING);
  if (m_bUseFiltering)
    iState.SetFlag(RENDERSTATEFLAG_FILTERING);
  IVRender2DInterface *pRI = Vision::RenderLoopHelper.BeginOverlayRendering();
  pRI->DrawTexturedQuad(hkvVec2(0.f,0.f), hkvVec2((float)iWidth, (float)iHeight), m_spSourceTextures[0], hkvVec2(0.0f,0.0f) + texelShift, hkvVec2(1.0f,1.0f) + texelShift, V_RGBA_WHITE, iState);
  Vision::RenderLoopHelper.EndOverlayRendering();

  // Only render 2d overlay geometry. 3d requires a depth buffer, which we don't have at this resolution.
  VRendererNodeCommon::RenderOverlays(true, false);
}

VPostProcessUpscale::~VPostProcessUpscale()
{

}

void VPostProcessUpscale::InitializePostProcessor()
{
  if(m_bIsInitialized)
  {
    return;
  }

  m_spSourceTextures[0] = GetOwner()->GetGBuffer(IVRendererNode::VGBT_Accumulation);

  GetOwner()->AddContext(GetTargetContext());

  m_bIsInitialized = true;
}

void VPostProcessUpscale::DeInitializePostProcessor()
{
  if(!m_bIsInitialized)
  {
    return;
  }

  m_spSourceTextures[0] = NULL;

  GetOwner()->RemoveContext(GetTargetContext());

  m_bIsInitialized = false;
}

BOOL VPostProcessUpscale::CanAttachToObject( VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut )
{
  if(!pObject->IsOfType(VMobileForwardRenderingSystem::GetClassTypeId()))
  {
    sErrorMsgOut = "Can only attach this component to instances of VMobileForwardRenderingSystem";
    return FALSE;
  }

  return VPostProcessingBaseComponent::CanAttachToObject(pObject, sErrorMsgOut);
}

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
