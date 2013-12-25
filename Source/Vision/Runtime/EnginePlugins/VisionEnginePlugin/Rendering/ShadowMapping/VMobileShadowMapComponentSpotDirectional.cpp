/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ShadowMapping/VMobileShadowMapComponentSpotDirectional.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ShadowMapping/VShadowMapGenSpotDir.hpp>
#include <Vision/Runtime/Engine/Renderer/Shader/VisionMobileShaderProvider.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

V_IMPLEMENT_SERIAL(VMobileShadowMapComponentSpotDirectional, VBaseShadowMapComponentSpotDirectional, 0, &g_VisionEngineModule);

VMobileShadowMapComponentSpotDirectional::VMobileShadowMapComponentSpotDirectional() :
  VBaseShadowMapComponentSpotDirectional()
{
  m_fFadeOutStart = 0.0f;
  m_fFadeOutEnd = 0.0f;
}

VMobileShadowMapComponentSpotDirectional::VMobileShadowMapComponentSpotDirectional(int iRendererNodeIndex) : 
  VBaseShadowMapComponentSpotDirectional(iRendererNodeIndex)
{
  m_fFadeOutStart = 0.0f;
  m_fFadeOutEnd = 0.0f;
}

VMobileShadowMapComponentSpotDirectional::~VMobileShadowMapComponentSpotDirectional()
{ 
}

bool VMobileShadowMapComponentSpotDirectional::InitializeRenderer()
{
  if (m_bIsInitialized)
    return true;

  if (!VBaseShadowMapComponentSpotDirectional::InitializeRenderer())
    return false;
  
  // ensure that shadow maps for spot and directional lights only have 1 cascade on mobile 
  CascadeCount = 1;
  CascadeSelection = CSM_NO_SELECTION;

  SetCascadeCount(CascadeCount);
  m_spShadowMapGenerator = new VShadowMapGenSpotDir(m_pRendererNode, m_pLightSource, GetShadowMapFormat(),
    this, CascadeCount, MOBILE_MAX_SHADOW_PARTS_COUNT);
  if (!m_spShadowMapGenerator->Initialize())
  {
    m_spShadowMapGenerator = NULL;
    return false;
  }
  m_spShadowMapGenerator->SetUpdateEnabled(true);

  if (m_pLightSource->GetType() == VIS_LIGHT_DIRECTED)
    m_pLightVolumeMeshBuffer = m_pRendererNode->GetRendererNodeHelper()->GetFrustumMeshBuffer();
  else if (m_pLightSource->GetType() == VIS_LIGHT_SPOTLIGHT)
    m_pLightVolumeMeshBuffer = m_pRendererNode->GetRendererNodeHelper()->GetConeMeshBuffer();

  m_bIsInitialized = true;
  return true;
}

void VMobileShadowMapComponentSpotDirectional::Render(bool bUseLightClippingVolume, bool bClearTexture)
{
  hkvVec4 vCascadeScale, vCascadeEnd;
  m_spShadowMapGenerator->ComputeCascadeParams(m_LightProjection, vCascadeScale, vCascadeEnd);
  ((VShadowMapGenSpotDir*)m_spShadowMapGenerator.GetPtr())->ComputeFadeOutParams(m_fFadeOutStart, m_fFadeOutEnd);
}

#if defined(WIN32) || defined(_VISION_DOC)

void VMobileShadowMapComponentSpotDirectional::GetVariableAttributes(VisVariable_cl* pVariable, VVariableAttributeInfo& destInfo)
{
  VBaseShadowMapComponentSpotDirectional::GetVariableAttributes(pVariable, destInfo);

  if (!strcmp(pVariable->GetName(), "ShadowMappingMode"))
    destInfo.m_bHidden = true;
  else if (!strcmp(pVariable->GetName(), "CascadeCount"))
    destInfo.m_bHidden = true;
  else if (!strcmp(pVariable->GetName(), "CascadeSelection"))
    destInfo.m_bHidden = true;
  else if (!strncmp(pVariable->GetName(), "CascadeRange", 12) && pVariable->GetName()[13] != '0') 
    destInfo.m_bHidden = true; 
  else if (!strncmp(pVariable->GetName(), "Bias", 4) && pVariable->GetName()[5] != '0')
    destInfo.m_bHidden = true;   
  else if (!strncmp(pVariable->GetName(), "SlopeScaled", 11) && pVariable->GetName()[12] != '0')
    destInfo.m_bHidden = true; 
  else if (!strcmp(pVariable->GetName(), "OverestimateCascades"))
    destInfo.m_bHidden = true; 
  else if (!strcmp(pVariable->GetName(), "SampleRadius"))
    destInfo.m_bHidden = true; 
  else if (!strcmp(pVariable->GetName(), "SampleRadiusScaleWithDistance"))
    destInfo.m_bHidden = true; 
  else if (!strcmp(pVariable->GetName(), "AmbientColor") && m_pLightSource->IsDynamic())
    destInfo.m_bHidden = true; 
  else if (!strcmp(pVariable->GetName(), "ShadowBoxExtrudeMultiplier"))
    destInfo.m_bHidden = true;

  if (m_pLightSource->GetType() == VIS_LIGHT_SPOTLIGHT)
  {
    if (!strcmp(pVariable->GetName(), "CameraUpdateInterval"))
      destInfo.m_bHidden = true; 
    else if (!strcmp(pVariable->GetName(), "CameraUpdateAngle"))
      destInfo.m_bHidden = true;
    else if (!strcmp(pVariable->GetName(), "FadeOutStart"))
      destInfo.m_bHidden = true;
    else if (!strcmp(pVariable->GetName(), "FadeOutEnd"))
      destInfo.m_bHidden = true;
  }
  else if (m_pLightSource->GetType() == VIS_LIGHT_DIRECTED)
  {
    if (!strcmp(pVariable->GetName(), "NearClip"))
      destInfo.m_bHidden = true;
  }
}

#endif

void VMobileShadowMapComponentSpotDirectional::SetCascadeCount(unsigned int iCount)
{
  VASSERT(iCount > 0);
  if (iCount > MOBILE_MAX_SHADOW_PARTS_COUNT)
    hkvLog::Warning("VMobileShadowMapComponentSpotDirectional: Currently only %d cascade is supported!", MOBILE_MAX_SHADOW_PARTS_COUNT);
}

void VMobileShadowMapComponentSpotDirectional::SetCascadeSelection(VCascadeSelectionMethod_e cascadeSelection)
{
  if (cascadeSelection != CSM_NO_SELECTION)
    hkvLog::Warning("VMobileShadowMapComponentSpotDirectional: Currently only CSM_NO_SELECTION is supported!");
}

void VMobileShadowMapComponentSpotDirectional::SetCascadeRange(int iCascade, float fRange)
{
  VASSERT(iCascade >= 0);
  if (iCascade > (MOBILE_MAX_SHADOW_PARTS_COUNT-1))
  {
    hkvLog::Warning("VMobileShadowMapComponentSpotDirectional: Currently only %d cascade is supported!", MOBILE_MAX_SHADOW_PARTS_COUNT);
    return;
  }

  CascadeRange[iCascade] = fRange;
  if (m_bIsInitialized)
  {
    float fNear, fFar;
    m_pRendererNode->GetViewProperties()->getClipPlanes(fNear, fFar);
    float fStart = (iCascade == 0) ? fNear : CascadeRange[iCascade - 1] + fNear;
    m_spShadowMapGenerator->GetCascadeInfo(iCascade).ComputeOffset(fStart, fRange + fNear);
    m_spShadowMapGenerator->Update(true);
  }
}

void VMobileShadowMapComponentSpotDirectional::SetOverestimateCascades(BOOL bOverestimate)
{
  if (!bOverestimate)
    hkvLog::Warning("VMobileShadowMapComponentSpotDirectional: Currently overestimation is always enabled!");
}

void VMobileShadowMapComponentSpotDirectional::SetCascadeRangePtr(float* range, int size)
{
  VASSERT(size > 0);

  // Clamp value instead of warn, since this method gets called from the base implementation with size = MAX_SHADOW_PARTS_COUNT
  size = hkvMath::Max(size, MOBILE_MAX_SHADOW_PARTS_COUNT);
  
  memcpy(CascadeRange, range, sizeof(float) * size);
  if (m_bIsInitialized)
  {
    float fNear, fFar;
    m_pRendererNode->GetViewProperties()->getClipPlanes(fNear, fFar);
    float start = fNear;
    for (int i = 0; i < (int)CascadeCount; i++)
    {
      m_spShadowMapGenerator->GetCascadeInfo(i).ComputeOffset(start, CascadeRange[i] + fNear);
      start = CascadeRange[i] + fNear;
    }
    m_spShadowMapGenerator->Update(true);
  }
}

void VMobileShadowMapComponentSpotDirectional::Serialize(VArchive &ar)
{
  VBaseShadowMapComponentSpotDirectional::Serialize(ar);

  char cLocalVersion = SHADOWMAP_COMPONENT_MOBILE_SPOTDIR_CURRENT_VERSION; 
  if (ar.IsLoading())
  {
    ar >> cLocalVersion; 
    VASSERT(cLocalVersion <= SHADOWMAP_COMPONENT_MOBILE_SPOTDIR_CURRENT_VERSION);

    if (cLocalVersion == SHADOWMAP_COMPONENT_MOBILE_SPOTDIR_VERSION_0)
    {
      float fMaxIntensity;
      ar >> fMaxIntensity;

      // For backward compatibility convert MaxIntensity into AmbientColor (overwriting thereby serialized AmbientColor value from base class).
      AmbientColor.FromFloat(hkvVec3(fMaxIntensity, fMaxIntensity, fMaxIntensity));
    }
  }
  else
  {
    ar << cLocalVersion;
  }
}

void VMobileShadowMapComponentSpotDirectional::UpdateLightShader(VMobileDynamicLightShader *pPass)
{
  VASSERT(pPass!=NULL && GetLightSource()!=NULL); 

  pPass->SetLightProjection(CascadeCount, m_LightProjection);
  pPass->SetShadowParameters(m_fFadeOutStart, m_fFadeOutEnd);
  pPass->SetShadowTexture(GetShadowTexture());
  pPass->SetShadowColor(AmbientColor);
  
  int iLightMaskSampler = pPass->GetSamplerIndexByName(VSS_PixelShader, "LightMask");
  if (iLightMaskSampler >= 0)
  {
    VStateGroupTexture *pStateGroupTexture = pPass->GetStateGroupTexture(VSS_PixelShader, iLightMaskSampler);
    if (pStateGroupTexture != NULL)
    {
      // specify this white alternative texture if no SM is available. The default would be a plain black texture
      pStateGroupTexture->m_spCustomTex = Vision::TextureManager.GetPlainWhiteTexture();
      pStateGroupTexture->m_iTextureIndex = GetLightSource()->GetNumber();
    }
  }
}

START_VAR_TABLE(VMobileShadowMapComponentSpotDirectional, VBaseShadowMapComponentSpotDirectional, "Mobile ShadowMap Component for Spotlights or directional lights", VCOMPONENT_ALLOW_MULTIPLE, "Mobile Shadow Map Component Spot/Directional")  
END_VAR_TABLE

/*
 * Havok SDK - Base file, BUILD(#20131218)
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2013
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
