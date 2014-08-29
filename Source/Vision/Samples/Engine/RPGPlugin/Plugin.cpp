/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/GameManager.h>
#include <Vision/Samples/Engine/RPGPlugin/MeshTrailEffectComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/HighlightableComponentManager.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokSync.inl>

#include <Common/Serialize/Util/hkBuiltinTypeRegistry.h>
#include <Common/Base/Reflection/Registry/hkVtableClassRegistry.h>

#include <Ai/Pathfinding/Character/Behavior/hkaiPathFollowingBehavior.h>

VIMPORT IVisPlugin_cl* GetEnginePlugin_vHavokAi();
VIMPORT IVisPlugin_cl* GetEnginePlugin_vHavokBehavior();
VIMPORT IVisPlugin_cl* GetEnginePlugin_vFmodEnginePlugin();

// Plugin class
namespace
{
  RPG_Plugin s_Plugin;

  class RPG_PluginInitializer : public IVisCallbackHandler_cl
  {
  public:
    RPG_PluginInitializer() {}

  protected:
    // IVisCallbackHandler_cl
    void OnHandleCallback(IVisCallbackDataObject_cl *callback_data) HKV_OVERRIDE;
  } 
  s_PluginInitializer;
}

DECLARE_THIS_MODULE(g_RPGPluginModule, MAKE_VERSION(0, 1), "RPGPlugin", "Havok", "Havok RPG Plugin", &s_Plugin);

IVisPlugin_cl *GetEnginePlugin_RPGPlugin()
{
  return &s_Plugin;
}

#ifndef VBASE_LIB
VEXPORT IVisPlugin_cl *GetEnginePlugin()
{
  return GetEnginePlugin_RPGPlugin();
}
#endif

void RPG_Plugin::OnInitEnginePlugin()
{
  VISION_HAVOK_SYNC_STATICS();

  // Module
  hkvLog::Info("RPGPlugin:OnInitEnginePlugin()");
  Vision::RegisterModule(&g_RPGPluginModule);
  RPG_VisionModuleTypes::ForceStaticLink();

  // Havok AI plugin (chains to Havok plugin)
  VISION_PLUGIN_ENSURE_LOADED(vHavokAi);
  VISION_PLUGIN_ENSURE_LOADED(vHavokBehavior);

  // Fmod
  VISION_PLUGIN_ENSURE_LOADED(vFmodEnginePlugin);

  // Hook physics init
  vHavokPhysicsModule::OnBeforeInitializePhysics += &s_PluginInitializer;
  vHavokPhysicsModule::OnAfterInitializePhysics += &s_PluginInitializer;

  // Local systems
  RPG_GameManager::s_instance.OneTimeInit();
  RPG_HighlightableComponentManager::s_instance.OneTimeInit();
  RPG_MeshTrailEffectComponentManager::s_instance.OneTimeInit();
}

#if (RPG_DUMP_CRT_OBJECTS_ON_EXIT)
# include <crtdbg.h>
#endif

void RPG_Plugin::OnDeInitEnginePlugin()
{
#if (RPG_DUMP_CRT_OBJECTS_ON_EXIT)
  _CrtMemDumpAllObjectsSince(NULL);
#endif

  // Local systems
  RPG_GameManager::s_instance.OneTimeDeInit();
  RPG_MeshTrailEffectComponentManager::s_instance.OneTimeDeInit();
  RPG_HighlightableComponentManager::s_instance.OneTimeDeInit();

  // Unhook physics init
  vHavokPhysicsModule::OnBeforeInitializePhysics -= &s_PluginInitializer;
  vHavokPhysicsModule::OnAfterInitializePhysics -= &s_PluginInitializer;

  // Module
  hkvLog::Info("RPGPlugin:OnDeInitEnginePlugin()");
  Vision::UnregisterModule(&g_RPGPluginModule);

  VISION_HAVOK_UNSYNC_STATICS();
}

char const *RPG_Plugin::GetPluginName()
{
  // Must match vPlugin name
  return "RPGPlugin";
}

void RPG_PluginInitializer::OnHandleCallback(IVisCallbackDataObject_cl *callback_data)
{
  if(&vHavokPhysicsModule::OnBeforeInitializePhysics == callback_data->m_pSender)
  {
    // hkaiCharacter created by this dll will have a different vtable than the one expected by the AI module (for vdb viewer)
    // The vtable-class(?) mapping is apparently many-to-one, so re-register here with our vtable
    // (Actually the vtables in this module and the vtables elsewhere are unique, so being many-to-one is a happy accident)
    // TODO redundant in the statically linked case
    hkVtableClassRegistry::getInstance().registerList(hkBuiltinTypeRegistry::StaticLinkedTypeInfos, hkBuiltinTypeRegistry::StaticLinkedClasses);
  }
  else if (&vHavokPhysicsModule::OnAfterInitializePhysics == callback_data->m_pSender)
  {
    vHavokPhysicsModuleCallbackData* pPhysicsData = static_cast<vHavokPhysicsModuleCallbackData*>(callback_data);
    vHavokPhysicsModule* pPhysicsModule = pPhysicsData->GetHavokModule();
    pPhysicsModule->SetEnabledVisualDebugger(true);

    // Disable validation checks (that are also performed in dev build)
    vHavokAiModule::GetInstance()->GetAiWorld()->getNavMeshCutter()->m_performValidationChecks = false;
  }
}

/*
 * Havok SDK - Base file, BUILD(#20140725)
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
