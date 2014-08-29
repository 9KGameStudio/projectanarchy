/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/Framework/VisionApp/Modules/VDebugShadingModes.hpp>
#include <Vision/Runtime/Framework/VisionApp/VAppImpl.hpp>

#include <Vision/Runtime/Engine/Renderer/VisApiSimpleRendererNode.hpp>

V_IMPLEMENT_DYNCREATE(VDebugShadingModes, VAppModule, Vision::GetEngineModule());

VDebugShadingModes::VDebugShadingModes()
  : VAppModule()
  , m_iCurrentDebugShadingMode(-1)
  , m_bParticleDebugShading(true)
{
}

void VDebugShadingModes::Init()
{
  Vision::Callbacks.OnAfterSceneLoaded += this;
  Vision::Callbacks.OnBeforeSceneUnloaded += this;

  VAppMenu* pMainMenu = GetParent()->GetAppModule<VAppMenu>();
  if (pMainMenu == NULL)
    return;
  

  // Load geometry debug shading effects and add menu items.
  VAppMenuItems items;
  VisShaderFXLibManager_cl &manager(Vision::Shaders.GetShaderFXLibManager());

  m_debugGeometryShadingEffects.Clear();
  m_spGeometryDebugShadingLib = NULL;

  m_spGeometryDebugShadingLib = Vision::Shaders.LoadShaderLibrary("Shaders\\DebugShadingEffects.ShaderLib");
  if ((m_spGeometryDebugShadingLib == NULL) || (m_spGeometryDebugShadingLib && !m_spGeometryDebugShadingLib->IsLoaded()))
    return;
  
  // Enumerate all available effects and add them to a collection for later usage
  for (int i = 0; i<m_spGeometryDebugShadingLib->m_Effects.Count(); i++)
  {
    VCompiledEffect *pFX = m_spGeometryDebugShadingLib->m_Effects.GetAt(i)->CompileEffect(NULL, manager.m_ShaderInstances);

    // Skip the "NotAvailable" shader, because it is only used for fallback reasons in vForge
    if ((pFX==NULL) || (pFX && VStringHelper::SafeCompare(pFX->GetSourceEffect()->GetName(), "NotAvailable") == 0))
      continue;

    // Use the effect's same as a description
    pFX->SetUserData((void *)pFX->GetSourceEffect()->GetName());

    int iIndex = m_debugGeometryShadingEffects.Add(pFX);

    // Geometry shading effects determines the number of items.
    items.Add(VAppMenuItem(pFX->GetSourceEffect()->GetName(), iIndex, 0, true));
  }

  m_callbacks = pMainMenu->RegisterGroup("Debug Shading", items, NULL, VAPP_DEFAULT_SORTING_2, true);
  m_callbacks.Add(pMainMenu->RegisterItem(VAppMenuItem("Particle Debug Shading", s_iParticleShadingOnOffAction, 1, true, m_bParticleDebugShading), "Debug Shading"));
  RegisterCallbacks();

  // Load particle shading effects.
  m_debugParticleShadingEffects.Clear();
  m_spParticleDebugShadingLib = NULL;

  m_spParticleDebugShadingLib = Vision::Shaders.LoadShaderLibrary("Shaders\\DebugShadingEffects_Particles.ShaderLib");
  if ((m_spParticleDebugShadingLib == NULL) || (m_spParticleDebugShadingLib && !m_spParticleDebugShadingLib->IsLoaded()))
    return;

  // For each geometry shading effect, there must be a corresponding particle shading effect.
  m_debugParticleShadingEffects.EnsureCount(m_debugGeometryShadingEffects.Count());
  for (int i = 0; i < m_spParticleDebugShadingLib->m_Effects.Count() && i < m_debugParticleShadingEffects.Count(); i++)
  {
    VCompiledEffect *pFX = m_spParticleDebugShadingLib->m_Effects.GetAt(i)->CompileEffect(NULL, manager.m_ShaderInstances);
    m_debugParticleShadingEffects.SetAt(i, pFX);
  }
}

void VDebugShadingModes::DeInit()
{
  Vision::Callbacks.OnAfterSceneLoaded -= this;
  Vision::Callbacks.OnBeforeSceneUnloaded -= this;
  DeRegisterCallbacks();

  m_spBackupRendererNode = NULL;
  Vision::RenderLoopHelper.SetReplacementRenderLoop(NULL);
  m_spDebugShadingRenderLoop = NULL;
  m_debugGeometryShadingEffects.Clear();
  m_spGeometryDebugShadingLib = NULL;
  m_debugParticleShadingEffects.Clear();
  m_spParticleDebugShadingLib = NULL;
}

void VDebugShadingModes::OnHandleCallback(IVisCallbackDataObject_cl* pData)
{
  if (pData->m_pSender == &Vision::Callbacks.OnAfterSceneLoaded)
  {
    Vision::RenderLoopHelper.SetReplacementRenderLoop(NULL);
    m_iCurrentDebugShadingMode = -1;
  }
  else if (pData->m_pSender == &Vision::Callbacks.OnBeforeSceneUnloaded)
  {
    Vision::RenderLoopHelper.SetReplacementRenderLoop(NULL);
    m_iCurrentDebugShadingMode = -1;
    m_spBackupRendererNode = NULL;
  }

  int iIndex = GetCallbackIndex(pData);
  if (iIndex == s_iParticleShadingOnOffAction)
  {
    m_bParticleDebugShading = !m_bParticleDebugShading;
    if (m_iCurrentDebugShadingMode != -1)
      SetDebugShadingMode(m_iCurrentDebugShadingMode);
  }
  else if (iIndex >= 0)
  {
    if (m_iCurrentDebugShadingMode != -1)
    {
      GetParent()->GetAppModule<VAppMenu>()->SetItemCheckState(
        static_cast<char*>(m_debugGeometryShadingEffects.GetAt(m_iCurrentDebugShadingMode)->GetUserData()), false);
    }

    if (m_iCurrentDebugShadingMode == iIndex)
      SetDebugShadingMode(-1);
    else
      SetDebugShadingMode(iIndex);
  }
}

void VDebugShadingModes::SetDebugShadingMode(int iDebugShadingModeIndex)
{
  if (m_iCurrentDebugShadingMode == -1)
  {
    // Backup render node so that the debug rendering can be restored to the original rendering
    m_spBackupRendererNode = Vision::Renderer.GetRendererNode(0);
  }

  // Disable when currently active shading mode is triggered again
  if (iDebugShadingModeIndex == -1)
  {
    m_iCurrentDebugShadingMode = -1;
    Vision::RenderLoopHelper.SetReplacementRenderLoop(NULL);
    Vision::Renderer.SetRendererNode(0, m_spBackupRendererNode);
  }
  else
  {
    m_iCurrentDebugShadingMode = iDebugShadingModeIndex;
    VSmartPtr<IVisShaderProvider_cl> spPreviousShaderProvider = Vision::GetApplication()->GetShaderProvider();

    // The debug shading works by replacing the current render loop by a debug shading one which applies the different effects
    if (m_spDebugShadingRenderLoop == NULL)
      m_spDebugShadingRenderLoop = new VisDebugShadingRenderLoop_cl();

    VCompiledEffect* pParticleShader = NULL;
    if (m_bParticleDebugShading)
      pParticleShader = m_debugParticleShadingEffects.GetAt(m_iCurrentDebugShadingMode);

    m_spDebugShadingRenderLoop->SetEffects(m_debugGeometryShadingEffects.GetAt(m_iCurrentDebugShadingMode), pParticleShader);

    // Simple render node base on the main render context which encapsulates the debug shading render loop
    VSimpleRendererNode* pSimpleRendererNode = new VSimpleRendererNode(Vision::Contexts.GetMainRenderContext());
    pSimpleRendererNode->InitializeRenderer();

    Vision::Renderer.SetRendererNode(0, pSimpleRendererNode);
    Vision::RenderLoopHelper.SetReplacementRenderLoop(m_spDebugShadingRenderLoop);

    // Keep old shader provider.
    Vision::GetApplication()->SetShaderProvider(spPreviousShaderProvider);
  }
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
