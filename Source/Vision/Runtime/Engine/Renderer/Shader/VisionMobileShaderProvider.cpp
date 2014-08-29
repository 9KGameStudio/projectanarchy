/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/Engine/System/Vision.hpp>
#include <Vision/Runtime/Engine/Renderer/Shader/VisionMobileShaderProvider.hpp>
#include <Vision/Runtime/Base/Graphics/Shader/vShaderConstantHelper.hpp>

V_IMPLEMENT_SERIAL(VMobileDynamicLightShader, VDynamicLightShaderBase, 0, Vision::GetEngineModule());

void VMobileDynamicLightShader::PostCompileFunction(VShaderEffectResource *pSourceFX,VShaderPassResource *pSourceShader)
{
  VDynamicLightShaderBase::PostCompileFunction(pSourceFX,pSourceShader); // base

  m_RegLightProjection.Init(this, "LightProjectionTex");
  m_RegShadowParams.Init(this, "ShadowParameters");
  m_RegShadowMapSize.Init(this, "ShadowMapSize");
  m_RegShadowColor.Init(this, "ShadowColor");
}


V_IMPLEMENT_DYNCREATE(VisionMobileShaderProvider, VisionShaderProvider_cl, Vision::GetEngineModule());

VisionMobileShaderProvider::VisionMobileShaderProvider()
  : VisionShaderProvider_cl()
  , m_bLastReassignUsedGlobalAmbient(UseGlobalAmbientColor())
{
  m_bUseMobileParticleShader = true;

  Vision::Callbacks.OnGlobalRenderSettingsChanged += this;
}

VisionMobileShaderProvider::~VisionMobileShaderProvider()
{
  Vision::Callbacks.OnGlobalRenderSettingsChanged -= this;
}

void VisionMobileShaderProvider::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if (pData->m_pSender==&Vision::Callbacks.OnGlobalRenderSettingsChanged)
  {
    VisGlobalRendererSettingsDataObject_cl *pCallbackObj = static_cast<VisGlobalRendererSettingsDataObject_cl*>(pData);

    if ((pCallbackObj->m_eChangedFlags & VGRP_FOG_TOGGLE) != 0 ||
      (pCallbackObj->m_eChangedFlags & VGRP_NEW_TIMEOFDAY_HANDLER) != 0)
    {
      RecreateAllLightingShaders();
    }  
    // Only react to ambient color changes if time of day is disabled,
    // otherwise the global ambient color is already taken into account.
    else if (Vision::Renderer.GetTimeOfDayHandler() == NULL && (pCallbackObj->m_eChangedFlags & VGRP_AMBIENT_COLOR) != 0)
    {
      if (UseGlobalAmbientColor() != m_bLastReassignUsedGlobalAmbient)
      {
        RecreateAllLightingShaders();
      }
    }
  }

  VisionShaderProvider_cl::OnHandleCallback(pData);
}

void VisionMobileShaderProvider::GetParticleEffectConfig(VisParticleGroup_cl::ParticleShaderFlags_e iFlags, VTechniqueConfig &config) const
{
  VStaticString<128> szInclusion;

#ifdef SUPPORTS_HARDWARESPANNING
  szInclusion += "HWSPANNING;";
  if (iFlags & VisParticleGroup_cl::PARTICLESHADERFLAGS_LOCALSPACE)
    szInclusion += "LOCALSPACE;";
#endif

  if ((iFlags & VisParticleGroup_cl::PARTICLESHADERFLAGS_LIGHTING_STATIC) || 
      (iFlags & VisParticleGroup_cl::PARTICLESHADERFLAGS_LIGHTING_DYNAMIC))
  {
    szInclusion += "LIGHTING_MOBILE;";

    if(iFlags & VisParticleGroup_cl::PARTICLESHADERFLAGS_LIGHTING_STATIC)
      szInclusion += "LIGHTING_STATIC;";
    if(iFlags & VisParticleGroup_cl::PARTICLESHADERFLAGS_LIGHTING_DYNAMIC)
      szInclusion += "LIGHTING_DYNAMIC;";
  }

#ifndef SUPPORTS_ALPHATEST_BLENDSTATE
  if (iFlags & VisParticleGroup_cl::PARTICLESHADERFLAGS_ALPHATESTINSHADER)
    szInclusion += "ALPHATESTINSHADER;";
#endif  

  config.SetInclusionTags(szInclusion);
}

void VisionMobileShaderProvider::RecreateAllLightingShaders()
{
  m_bLastReassignUsedGlobalAmbient = UseGlobalAmbientColor();
  CreateLightingShaderForWorld(true);
  CreateLightingShaderForAllModels(true);
}

void VisionMobileShaderProvider::SetEffect(VisSurface_cl *pSurface, 
  VCompiledEffect *pFX, VTechniqueConfig* pDefaultConfig)
{
  VTechniqueConfig globalConfig = *Vision::Shaders.GetGlobalTechniqueConfig();
  if (!pFX)
    pFX = CreateMaterialEffect(pSurface);

  // store references to relevant effect resources so they are also refcounted 
  // and don't accidentally get cleaned up before saving to XML
  pSurface->m_spCurrentEffect = pFX;
  pSurface->m_spCurrentEffectResource = (pFX!=NULL) ? pFX->m_pSourceEffect : NULL;
  pSurface->m_spCurrentEffectLib = (pSurface->m_spCurrentEffectResource!=NULL) ? 
    pSurface->m_spCurrentEffectResource->GetOwnerEffectLib() : NULL;

  VTechniqueConfig defaultConfig;
  if (pDefaultConfig != NULL)
    defaultConfig = *pDefaultConfig;

  if (!pFX) // should not happen though
  {
    hkvLog::Warning("failed to assign surface effect or create default effect. Missing material shader libs?");
    pSurface->m_spCurrentTechnique = NULL;
    pSurface->m_spDynPointLight = NULL;
    pSurface->m_spDynSpotLight = NULL;
    pSurface->m_spDynProjPointLight = NULL;
    pSurface->m_spMirror = NULL;
    pSurface->m_spMirrorSimple = NULL;
    pSurface->m_spDepthFill = NULL;
    return;
  }

  VTechniqueConfig *pConfigs[2] = { &defaultConfig, &globalConfig };  
  pFX->GetSourceEffect()->AddEffectParameterInclusionTags(pConfigs, 2, pFX->GetParameterString());

  const bool bUseNormalMap = pSurface->GetNormalMapTextureObject() != NULL;
  const bool bUseSpecular = pSurface->GetSpecularMultiplier() > 0.f;
  const bool bUseSpecularMap = pSurface->GetSpecularMapTextureObject() != NULL;

  bool bUseDot3Lightmapping = 
    (Vision::Renderer.GetLightingMode() & VIS_LIGHTING_BUMPLIGHTMAPS) > 0;
  bUseDot3Lightmapping = 
    bUseDot3Lightmapping && (bUseNormalMap || bUseSpecular || bUseSpecularMap);

  //default and mirror technique
  for (int i = 0; i < 2; i++)
  { 
    VTechniqueConfig usageConfig = defaultConfig;
    char szFallbackMode[128];
    char szMode[128];
    bool bTestFallback = false;

    if (pSurface->IsFullbright())
    {
      strcpy(szMode, i == 0 ? "FULLBRIGHT" : "MIRROR;FULLBRIGHT");
    }
    else if (pSurface->IsLightMapped())
    {
      // All shaders in the mobile lib have also set LIGHTMAPDOT3 when LIGHTMAP is set. Therefore also when 
      // the LIGHTMAPDOT3 inclusion tag is set the corresponding mobile shader can be found.
      if (bUseDot3Lightmapping)
        strcpy(szMode, i == 0 ? "LIGHTMAPDOT3" : "MIRROR;LIGHTMAPDOT3");
      else
        strcpy(szMode, i == 0 ? "LIGHTMAP" : "MIRROR;LIGHTMAP");
    }
    else if (pSurface->GetLightingMode () == VIS_LIGHTING_LIGHTGRID)
    {
      strcpy(szMode, i == 0 ? "LIGHTGRIDSIMPLE" : "MIRROR;LIGHTGRIDSIMPLE");
    }
    else 
    {
      strcpy(szMode, i == 0 ? "" : "MIRROR");
    }

    // Since non mobile lib shaders do not provide the inclusion tag USE_GLOBAL_AMBIENT, for those
    // shaders a fall-back without those inclusion tags will be checked, too.
    strcpy(szFallbackMode, szMode);

    // Only use global ambient color for non full-bright surfaces and when Time of Day is active.
    if (!pSurface->IsFullbright() && UseGlobalAmbientColor())
    {
      if (szMode[0]) 
        strcat(szMode,";");
      strcat(szMode,"USE_GLOBAL_AMBIENT");
      bTestFallback = true;
    }

    // on mobile devices use the alphatest shader only if the surface explicitly wants alphatest, 
    // since the shader uses a clip instruction which is expensive on mobile devices
    if (pSurface->GetTransparencyType () == VIS_TRANSP_ALPHATEST)
    {
      if (szMode[0]) 
        strcat(szMode,";");
      strcat(szMode,"ALPHATEST");
    }

    usageConfig.AddInclusionTag(szMode);

#if defined (HK_DEBUG) && defined (_VISION_WIN32)
    if (i == 0)
    {
      pSurface->SetDefaultInclusionTags (usageConfig);
    }
#endif
    
    VCompiledTechnique *pTech = pFX->FindCompatibleTechnique(&usageConfig, &globalConfig);
    if (!pTech && bTestFallback)
    {
      usageConfig = defaultConfig;
      usageConfig.AddInclusionTag(szFallbackMode);
      pTech = pFX->FindCompatibleTechnique(&usageConfig, &globalConfig);
    }
    if (!pTech)
    {
      // find default technique
      pTech = pFX->FindCompatibleTechnique(&globalConfig);
      usageConfig = globalConfig;
    }

    // dynamic base pass lighting only supported for default technique
    if (i==0 && !pSurface->IsFullbright() && !pSurface->IsMaterialFlagSet(VisSurface_cl::VSF_NoDynamicLightShaders))
    {
      SetDynamicLightingTechniques(pSurface, pFX, usageConfig, VMOBILE_LIGHTING_FLAG_BASEPASS | VMOBILE_LIGHTING_FLAG_DYNAMIC); 
      SetDynamicLightingTechniques(pSurface, pFX, usageConfig, VMOBILE_LIGHTING_FLAG_BASEPASS | VMOBILE_LIGHTING_FLAG_DYNAMIC | VMOBILE_LIGHTING_FLAG_SHADOW); 
      SetDynamicLightingTechniques(pSurface, pFX, usageConfig, VMOBILE_LIGHTING_FLAG_BASEPASS | VMOBILE_LIGHTING_FLAG_SHADOW); 
    }

    if (pTech)
    {
      if (i == 0)
      {
        pSurface->SetTechnique(pTech);
      }
      else
      {
        pSurface->m_spMirror = pTech;
        pSurface->m_spMirrorSimple = pTech;
      }
    }
  }

#ifdef HK_DEBUG_SLOW
  {
    if (!pSurface->GetTechnique())
    {
      hkvLog::Warning ("Mobile-Renderer: Surface \"%s\": Could not find compatible DEFAULT technique from Effect \"%s\" from EffectLib \"%s\".", 
        pSurface->GetName (), pFX->m_pSourceEffect->m_sName.AsChar (), 
        pFX->m_pSourceEffect->GetOwnerEffectLib ()->GetFilename ());
    }
  }
#endif

  // DepthFill technique
  {
    VTechniqueConfig usageConfig = defaultConfig;
    char szMode[64];
    strcpy(szMode,"DEPTHFILL");
    if (pSurface->GetTransparencyType () == VIS_TRANSP_ALPHATEST)
    {
      strcat(szMode,";ALPHATEST");
    }
    usageConfig.AddInclusionTag(szMode);
    pSurface->m_spDepthFill = pFX->FindCompatibleTechnique(&usageConfig, &globalConfig);
  }

  // Dynamic additive lighting
  if (!pSurface->IsMaterialFlagSet(VisSurface_cl::VSF_NoDynamicLightShaders))
  {
    SetDynamicLightingTechniques(pSurface, pFX, defaultConfig, VMOBILE_LIGHTING_FLAG_DYNAMIC);
    SetDynamicLightingTechniques(pSurface, pFX, defaultConfig, VMOBILE_LIGHTING_FLAG_DYNAMIC | VMOBILE_LIGHTING_FLAG_SHADOW);
  }  

  //Trigger resolving of the final pass type on the surface
  pSurface->ResolvePassType();
}

VCompiledTechnique* VisionMobileShaderProvider::CreateLightingTechnique(VisSurface_cl *pSurface,VCompiledEffect *pFX, const VTechniqueConfig* pConfigChain, 
                                                                        int iNumConfigsInChain, const VTechniqueConfig* pGlobalConfig, bool bBasepass)
{
  // Do not create fall-back technique for base-pass lighting shaders.
  if (!bBasepass)
    return SafeCreateTechnique(pSurface, pFX, pConfigChain, iNumConfigsInChain, pGlobalConfig);
  else
    return pFX->FindCompatibleTechnique(&pConfigChain[0], pGlobalConfig);
}

void VisionMobileShaderProvider::SetDynamicLightingTechniques(VisSurface_cl *pSurface, VCompiledEffect *pFX, const VTechniqueConfig &defaultConfig, 
  int iLightingFlags)
{
  VASSERT(pSurface && pFX);

  const bool bUseNormalMap = pSurface->GetNormalMapTextureObject() != NULL;

  bool bBasepass = (iLightingFlags & VMOBILE_LIGHTING_FLAG_BASEPASS) > 0;
  bool bDynamicLighting = (iLightingFlags & VMOBILE_LIGHTING_FLAG_DYNAMIC) > 0;
  bool bShadow = (iLightingFlags & VMOBILE_LIGHTING_FLAG_SHADOW) > 0;

  // depth stencil states are set manually 
  int iDisableDepthStencilStateGroupMask = STATEGROUP_ALL;
  if (!bBasepass)
    iDisableDepthStencilStateGroupMask &= ~STATEGROUP_DEPTHSTENCIL;

  // technique config chain
  VTechniqueConfig tcPresetChain[2] = { defaultConfig, defaultConfig };
  VTechniqueConfig tcChain[2];
  int iNumConfigsInChain = 1;

  if (bShadow)
  {
    tcPresetChain[0].AddInclusionTag("USE_SHADOWMAP");
    iNumConfigsInChain = 2;
  }

  if (bBasepass)
  {
    tcPresetChain[0].AddInclusionTag(bDynamicLighting ? "MOBILE_DYNAMICBASEPASSLIGHTING" : "MOBILE_SUBTRACTIVE_SHADOWMAP");
    iNumConfigsInChain = 2;
  }

  VTechniqueConfig *pGlobalConfig = Vision::Shaders.GetGlobalTechniqueConfig();
  VCompiledTechnique *pPointLight = NULL;
  VCompiledTechnique *pSpotLight = NULL;
  VCompiledTechnique *pDirectionalLight = NULL;

  // dynamic point light
  {
    for (int i = 0; i < iNumConfigsInChain; i++)
    {
      tcChain[i] = tcPresetChain[i];
      tcChain[i].AddInclusionTag("POINTLIGHT");
    }

    pPointLight = CreateLightingTechnique(pSurface, pFX, tcChain, iNumConfigsInChain, pGlobalConfig, bBasepass);
    if (pPointLight != NULL)
      pPointLight->SetStateGroupMaskForAllPasses(iDisableDepthStencilStateGroupMask);
  }

  // dynamic spot light
  {
    for (int i = 0; i < iNumConfigsInChain; i++)
    {
      tcChain[i] = tcPresetChain[i];
      tcChain[i].AddInclusionTag("SPOTLIGHT");
    }

    pSpotLight = CreateLightingTechnique(pSurface, pFX, tcChain, iNumConfigsInChain, pGlobalConfig, bBasepass);
    if (pSpotLight != NULL)
      pSpotLight->SetStateGroupMaskForAllPasses(iDisableDepthStencilStateGroupMask);
  }

  // dynamic directional light
  {
    for (int i = 0; i < iNumConfigsInChain; i++)
    {
      tcChain[i] = tcPresetChain[i];
      tcChain[i].AddInclusionTag("DIRECTIONALLIGHT");
    }

    pDirectionalLight = CreateLightingTechnique(pSurface, pFX, tcChain, iNumConfigsInChain, pGlobalConfig, bBasepass);
    if (pDirectionalLight != NULL)
      pDirectionalLight->SetStateGroupMaskForAllPasses(iDisableDepthStencilStateGroupMask);
  }

  switch (iLightingFlags)
  {
  case VMOBILE_LIGHTING_FLAG_DYNAMIC:
    pSurface->m_spDynPointLight = pPointLight;
    pSurface->m_spDynSpotLight = pSpotLight;
    pSurface->m_spDynDirectionalLight = pDirectionalLight;
    break;

  case (VMOBILE_LIGHTING_FLAG_DYNAMIC | VMOBILE_LIGHTING_FLAG_SHADOW):
    pSurface->m_spDynSpotLightShadow = pSpotLight;
    pSurface->m_spDynDirectionalLightShadow = pDirectionalLight;
    break;

  case (VMOBILE_LIGHTING_FLAG_BASEPASS | VMOBILE_LIGHTING_FLAG_DYNAMIC):
    pSurface->m_spCustomTechniques[VMOBILE_BASEPASS_TECHNIQUE_DYN_POINTLIGHT] = pPointLight;
    pSurface->m_spCustomTechniques[VMOBILE_BASEPASS_TECHNIQUE_DYN_SPOTLIGHT] = pSpotLight;
    pSurface->m_spCustomTechniques[VMOBILE_BASEPASS_TECHNIQUE_DYN_DIRLIGHT] = pDirectionalLight;
    break;

  case (VMOBILE_LIGHTING_FLAG_BASEPASS | VMOBILE_LIGHTING_FLAG_DYNAMIC | VMOBILE_LIGHTING_FLAG_SHADOW):
    pSurface->m_spCustomTechniques[VMOBILE_BASEPASS_TECHNIQUE_DYN_SPOTLIGHT_SHADOW] = pSpotLight;
    pSurface->m_spCustomTechniques[VMOBILE_BASEPASS_TECHNIQUE_DYN_DIRLIGHT_SHADOW] = pDirectionalLight;
    break;

  case (VMOBILE_LIGHTING_FLAG_BASEPASS | VMOBILE_LIGHTING_FLAG_SHADOW):
    pSurface->m_spCustomTechniques[VMOBILE_BASEPASS_TECHNIQUE_DYN_SPOTLIGHT_SUBTRACTIVE_SHADOW] = pSpotLight;
    pSurface->m_spCustomTechniques[VMOBILE_BASEPASS_TECHNIQUE_DYN_DIRLIGHT_SUBTRACTIVE_SHADOW] = pDirectionalLight;
    break;

  default:
    VASSERT_MSG(false, "Unsupported lighting flags");
  }
}

VCompiledEffect* VisionMobileShaderProvider::CreateMaterialEffect(VisSurface_cl* pSurface, int iFlags)
{
  // default colors for model resp. world
  if (!pSurface->IsFullbright() && !(iFlags & VisSurface_cl::VSF_NoDefaultShaders))
  {
    if ((pSurface->IsLightMapped() && !Vision::RenderLoopHelper.HasLightmaps())
      || (pSurface->GetLightingMode() == VIS_LIGHTING_LIGHTGRID && !Vision::RenderLoopHelper.HasLightGrid()))
    {
      /*hkvLog::Warning("Material '%s' wants %s, which is not used in this scene. Falling back to default material."
      "Use 'fullbright' or 'dynamic only' lighting method to prevent this warning.", pSurface->GetName(),
      pSurface->IsLightMapped() ? "lightmaps" : "lightgrid");*/

      bool bUseAlphaTest = 
        pSurface->GetTransparencyType()==VIS_TRANSP_ALPHATEST || 
        pSurface->GetTransparencyType()==VIS_TRANSP_ALPHA;
      return GetDefaultLightingColorEffect(bUseAlphaTest, pSurface->GetAlphaTestThreshold(), 
        pSurface->IsDoubleSided(), pSurface->IsDepthWriteEnabled());
    }
  }

  char szEffectName[256];
  szEffectName[0] = 0;
  char szParamStr[256];
  szParamStr[0] = 0;

  // valid for all types
  sprintf(szParamStr, "AlphaThreshold=%.3f;", pSurface->GetAlphaTestThreshold());

  // normal map
  if (pSurface->GetNormalMapTextureObject() != NULL)
  {
    strcpy_s(szEffectName, "DiffuseBump");
  }
  // diffuse only
  else 
  {  
    strcpy_s(szEffectName, "Diffuse");
  }

  if (Vision::World.GetFogParameters().depthMode != VFogParameters::Off)
  {
    strcat(szEffectName, "Fog");
  }

  return CreateEffect(pSurface, szEffectName, szParamStr, EFFECTCREATEFLAG_NONE, false);
}

VCompiledEffect* VisionMobileShaderProvider::CreateEffect(const VisSurface_cl* pSurface, 
  const char* szEffectName, char* szParamStr, int iCreateFlags, bool bDynamicLighting)
{
  VASSERT(!VStringUtil::IsEmpty(szEffectName));

  char szAdditionalParam[1024];
  sprintf(szAdditionalParam,"CullMode=%s;DepthWrite=%s;DepthBias=%.6f;",
    pSurface->IsDoubleSided() ? "none" : "back",
    pSurface->IsDepthWriteEnabled() ? "true" : "false",
    pSurface->GetDepthBias());
  strcat(szParamStr, szAdditionalParam);

  // Load shader library
  const char* szLibName = NULL;
  if (Vision::World.GetFogParameters().depthMode != VFogParameters::Off)
    szLibName = "\\Shaders\\MobileShadersFog.ShaderLib";
  else 
    szLibName = "\\Shaders\\MobileShaders.ShaderLib";

  VShaderEffectLib* pFXLib = Vision::Shaders.LoadShaderLibrary(szLibName, SHADERLIBFLAG_HIDDEN);
  if (!pFXLib)
  {
    hkvLog::Warning("Failed to load mobile shader library: '%s'", szLibName);
  }

  // create effect
  VCompiledEffect *pFX = Vision::Shaders.CreateEffect(szEffectName, szParamStr,
    iCreateFlags, pFXLib);
  if (!pFX)
  {
    hkvLog::Warning("Failed to create effect '%s'", szEffectName);
  }
  return pFX;
}

VCompiledTechnique *VisionMobileShaderProvider::GetDynamicLightShader(
  const VisLightSource_cl *pLight, const VisSurface_cl *pSurface, bool bForEntity)
{
  VASSERT(pLight != NULL && pSurface != NULL);

  // light faded out?
  const hkvVec3 vCamPos = VisRenderContext_cl::GetCurrentContext()->GetCamera()->GetPosition();
  float fFade = pLight->GetMultiplier() * pLight->GetFadeWeight(vCamPos);
  if (fFade <= HKVMATH_LARGE_EPSILON)
    return NULL;

  VCompiledTechnique *pTech = NULL;
  VisLightSourceType_e lightType = pLight->GetType();

  VTextureObject *pProjTexture = NULL;
  hkvPlane plane_x(hkvNoInitialization);
  hkvPlane plane_y(hkvNoInitialization);
  hkvPlane plane_z(hkvNoInitialization);
 
  // 1: pick light shader from material
  switch (lightType)
  {
  case VIS_LIGHT_POINT:
    pTech = pSurface->m_spDynPointLight; 
    break;
  case VIS_LIGHT_SPOTLIGHT: 
    pTech = pSurface->m_spDynSpotLight;
    pProjTexture = pLight->GetProjectionTexture();
    if (!pProjTexture)
      pProjTexture = GetDefaultSpotlightTexture();
    pLight->GetProjectionPlanes(plane_x, plane_y, plane_z); // this has the cone angle encoded already
    break;
  case VIS_LIGHT_DIRECTED: 
    pTech = pSurface->m_spDynDirectionalLight; 
    break;
  default:
    VASSERT_MSG(false, "Unsupported light type");
    return NULL;
  }
  if (!pTech)
    return NULL;

  // 2: modify light shader
  const hkvVec3 vColor = pLight->GetColor().ToFloat();
  hkvVec3 lightDir;
  if (lightType == VIS_LIGHT_DIRECTED || lightType == VIS_LIGHT_SPOTLIGHT)
  {
    lightDir = pLight->GetDirection();
    lightDir.normalize();
  }
  // Attenuation texture not used by MobileShaders.ShaderLib, but may be used
  // by manually assigned shader libs.
  VTextureObject *pAttTexture = pLight->GetAttenuationTexture();
  if (pAttTexture == NULL)
    pAttTexture = GetDefaultAttenuationTexture();

  const int iPassCount = pTech->GetShaderCount();
  for (int i=0;i<iPassCount;i++)
  {
    VDynamicLightShaderBase *pPass = vdynamic_cast<VDynamicLightShaderBase*>(pTech->GetShader(i));
    VASSERT_MSG(pPass != NULL, "Dynamic light shaders must be of class VDynamicLightShaderBase");   
    if (!pPass)
    {
      hkvLog::Warning("Dynamic light shader is not of class VDynamicLightShaderBase; dynamic light will have no effect.");
      continue;
    }

    // set light source properties
    pPass->SetPosition(pLight->GetPosition());
    pPass->SetRadius(pLight->GetRadius());
    pPass->SetColor(vColor, fFade);
    pPass->SetAttenuationTexture(pAttTexture);

    // set light direction for directed lights
    if (lightType == VIS_LIGHT_DIRECTED)
    {
      pPass->SetDirection(lightDir);
    }
    // set light direction and cone angle for spot lights
    else if (lightType == VIS_LIGHT_SPOTLIGHT)
    {
      pPass->SetDirection(lightDir);
      pPass->SetProjectionAngle(pLight->GetProjectionAngle());
    }

    if (pProjTexture)
    {
      pPass->SetProjectedTexture(pProjTexture);
      pPass->SetProjectionPlanes(plane_x,plane_y,plane_z);
    }

    pPass->m_bModified = true;
  }

  return pTech;
}

bool VisionMobileShaderProvider::UseGlobalAmbientColor() const
{
  return (Vision::Renderer.GetTimeOfDayHandler() != NULL || 
    !Vision::Renderer.GetGlobalAmbientColor().getAsVec3().isZero(0.001f));
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
