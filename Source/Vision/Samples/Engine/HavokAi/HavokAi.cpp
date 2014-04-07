/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

//***********************************************************************************************
// HavokAi : Havok AI Binding Sample
// Copyright (C) Havok.com Inc. All rights reserved.
//
// On PC the command line argument '-streaming' enables the streaming version of the sample,
// which streams in parts of the nav mesh.
//***********************************************************************************************

// TODO: WiiU: ~VSampleFlags::VSAMPLE_WIIU_DRCDEMO

#include <Vision/Samples/Engine/HavokAi/HavokAiPCH.h>
#include <Vision/Samples/Engine/HavokAi/PathPickDlg.hpp>

#include <Vision/Runtime/Framework/VisionApp/VAppImpl.hpp>
#include <Vision/Runtime/Framework/VisionApp/Modules/VHelp.hpp>

#include <Vision/Runtime/EnginePlugins/EnginePluginsImport.hpp>

#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/vHavokAiModule.hpp>
#include <Ai/Pathfinding/World/hkaiWorld.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/vHavokAiNavMeshResource.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/vHavokAiNavMeshResourceManager.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/vHavokAiNavMeshInstance.hpp>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokSync.hpp>

enum HAVOKAI_CONTROL 
{
  HAVOKAI_SHOW_MENU = VAPP_INPUT_CONTROL_LAST_ELEMENT + 1
};

class HavokAiApp : public VAppImpl
{
public:
  HavokAiApp() 
    : m_bUseStreaming(false) // Set to true to enable streaming the NavMesh.
    , m_spPickDlg(NULL)
    , m_bLastShowGUI(false)
    , m_pCamera(NULL)
  {
    for (int i = 0; i < NumEntities; i++)
      m_pEntities[i] = NULL;
  }
  virtual ~HavokAiApp() {}

  virtual void Init() HKV_OVERRIDE
  {
    ParseCommandLine();

    const char* szSceneName = m_bUseStreaming ? "Scenes/HavokAiStreaming.vscene" : "Scenes/HavokAi.vscene";

    VisAppLoadSettings settings(szSceneName);
    settings.m_customSearchPaths.Append(":havok_sdk/Data/Vision/Samples/Engine/HavokAi");
    settings.m_customSearchPaths.Append(":havok_sdk/Data/Vision/Samples/Engine/Common");
    LoadScene(settings);
  }

  virtual void PreloadPlugins() HKV_OVERRIDE
  {
    VISION_PLUGIN_ENSURE_LOADED(vHavokAi);
    VISION_HAVOK_SYNC_ALL_STATICS();
  }

  virtual void AfterEngineInit() HKV_OVERRIDE
  {
    VAppImpl::AfterEngineInit();

    InitHelp();

    // Show streaming zone debug rendering.
    if (m_bUseStreaming)
      Vision::Profiling.ToggleDebugRenderFlags(DEBUGRENDERFLAG_ZONES);
  }

  virtual void AfterSceneLoaded(bool bLoadingSuccessful) HKV_OVERRIDE;

  virtual bool Run() HKV_OVERRIDE;

  virtual void DeInit() HKV_OVERRIDE
  {
    m_spPickDlg = NULL;
  }

  virtual void EngineDeInit() HKV_OVERRIDE
  {
    VISION_HAVOK_UNSYNC_ALL_STATICS();
  }

private:
  void InitHelp();
  void InitInput();
  void ParseCommandLine();

  bool m_bUseStreaming;

  VSmartPtr<PathPickDlg> m_spPickDlg;
  bool m_bLastShowGUI;
  VisBaseEntity_cl* m_pCamera;

  static const unsigned int NumEntities = 20;
  VisBaseEntity_cl* m_pEntities[NumEntities];
};

VAPP_IMPLEMENT_SAMPLE(HavokAiApp);

void HavokAiApp::AfterSceneLoaded(bool bLoadingSuccessful)
{
  // For HavokAiStreaming, the NavMesh gets streamed in automatically.
  if (!m_bUseStreaming)
  {
    vHavokAiModule::GetInstance()->LoadNavMeshDeprecated("Scenes/HavokAi.NavMeshData/navmesh_0.hkt");
  }

  // Override scene settings.
  vHavokAiModule::GetInstance()->SetConnectToPhysicsWorld(true); 

  // Create a GUI context
  m_spPickDlg = new PathPickDlg();
  m_spPickDlg->InitDialog(GetContext(), NULL, NULL);

  m_pCamera = Vision::Game.CreateEntity("VFreeCamera", hkvVec3(484.0f, 753.8f, 920.7f));
  m_pCamera->SetOrientation(-118.5f, 49.5f, 0.f);

  InitInput();
}

bool HavokAiApp::Run()
{
  // Show / Hide GUI
  const bool bShowGUI = (GetInputMap()->GetTrigger(HAVOKAI_SHOW_MENU) != 0);
  if (m_bLastShowGUI != bShowGUI)
  {
    if (bShowGUI)
      GetContext()->ShowDialog(m_spPickDlg);
    else
      GetContext()->CloseDialog(m_spPickDlg);

    m_pCamera->SetThinkFunctionStatus(!bShowGUI);
    m_bLastShowGUI = bShowGUI;
  }

  // Process rigid bodies
  static unsigned int uiNextEntityIndex = 0;
  static float baseTime = Vision::GetTimer()->GetCurrentTime();
  float currTime = Vision::GetTimer()->GetCurrentTime();

  if ((currTime - baseTime) > 1.0f)
  {
    const hkvVec3 vInitialPos(0.0f, 0.0f, 1500.0f);

    vHavokRigidBody *pComponent;
    if (m_pEntities[uiNextEntityIndex] == NULL)
    { 
      m_pEntities[uiNextEntityIndex] = Vision::Game.CreateEntity("VisBaseEntity_cl", vInitialPos, "Models/AmmoBox/ammobox3.model");
      m_pEntities[uiNextEntityIndex]->SetScaling(2.0f);
      m_pEntities[uiNextEntityIndex]->SetCastShadows(true);

      pComponent = new vHavokRigidBody();
      m_pEntities[uiNextEntityIndex]->AddComponent(pComponent);
      pComponent->SetRestitution(0.3f);
    }
    else
    {
      pComponent = m_pEntities[uiNextEntityIndex]->Components().GetComponentOfType<vHavokRigidBody>();
      pComponent->SetPosition(vInitialPos);
      m_pEntities[uiNextEntityIndex]->SetPosition(vInitialPos);
    }
    pComponent->SetLinearVelocity(hkvVec3::ZeroVector());
    pComponent->SetAngularVelocity(hkvVec3(
      static_cast<float>(rand() % 10), static_cast<float>(rand() % 10), static_cast<float>(rand() % 10)) * 50.0f);

    baseTime = currTime;
    if (++uiNextEntityIndex == NumEntities)
      uiNextEntityIndex = 0;
  }

  vHavokAiModule::GetInstance()->DebugRender(0.1f, false);

  IVRenderInterface *pRI = Vision::Contexts.GetMainRenderContext()->GetRenderInterface();
  hkvVec3 newStart = m_spPickDlg->GetStartPoint();
  hkvVec3 newEnd = m_spPickDlg->GetEndPoint();
  vHavokAiModule::GetInstance()->ComputeAndDrawPath(pRI, &newStart, &newEnd, 20.0f, 1.75f, 0.2f, hkColor::YELLOW);

  return true;
}

void HavokAiApp::InitHelp()
{
  VArray<const char*> helpText;

#if defined(SUPPORTS_KEYBOARD)

  helpText.Append("");
  helpText.Append("KEYBOARD - ALT : Show the cursor");
  helpText.Append("LEFT MOUSE CLICK : Set start position");
  helpText.Append("RIGHT MOUSE CLICK : Set end position");
#endif

#if defined (_VISION_XENON)|| (defined(_VISION_WINRT) && !defined(_VISION_METRO) && !defined(_VISION_APOLLO))

  helpText.Append("");
  helpText.Append("PAD1 - Left Shoulder : Show the cursor");
  helpText.Append("PAD1 - Right Thumbstick : Move camera/cursor");
  helpText.Append("PAD1 - A : Set start position");
  helpText.Append("PAD1 - B : Set end position");

#elif defined (_VISION_PS3)

  helpText.Append("");
  helpText.Append("PAD1 - Left Shoulder : Show the cursor");
  helpText.Append("PAD1 - Right Thumbstick : Move camera/cursor");
  helpText.Append("PAD1 - CROSS : Set start position");
  helpText.Append("PAD1 - CIRCLE : Set end position");

#elif defined (_VISION_PSP2)

  helpText.Append("");
  helpText.Append("PAD - Left Shoulder : Show the cursor");
  helpText.Append("PAD - Right Thumbstick : Move camera/cursor");
  helpText.Append("PAD - CROSS : Set start position");
  helpText.Append("PAD - CIRCLE : Set end position");

#elif defined (_VISION_WIIU)

  helpText.Append("");
  helpText.Append("DRC - Left Shoulder : Show the cursor");
  helpText.Append("DRC - Right Thumbstick : Move camera/cursor");
  helpText.Append("DRC - B : Set start position");
  helpText.Append("DRC - A : Set end position");

#endif
  if(helpText.GetSize() > 0)
    RegisterAppModule(new VHelp(helpText));
}

void HavokAiApp::InitInput()
{
#if defined(SUPPORTS_KEYBOARD)
  GetInputMap()->MapTrigger(HAVOKAI_SHOW_MENU, V_KEYBOARD, CT_KB_LALT);
  GetInputMap()->MapTrigger(HAVOKAI_SHOW_MENU, V_KEYBOARD, CT_KB_RALT);
#endif

#if defined (_VISION_XENON)|| (defined(_VISION_WINRT) && !defined(_VISION_METRO) && !defined(_VISION_APOLLO))
  GetInputMap()->MapTrigger(HAVOKAI_SHOW_MENU, V_XENON_PAD(0), CT_PAD_LEFT_SHOULDER);
#elif defined (_VISION_PS3)
  GetInputMap()->MapTrigger(HAVOKAI_SHOW_MENU, V_PS3_PAD(0), CT_PAD_LEFT_SHOULDER);
#elif defined (_VISION_PSP2)
  GetInputMap()->MapTrigger(HAVOKAI_SHOW_MENU, V_PSP2_PAD(0), CT_PAD_LEFT_SHOULDER);
#elif defined (_VISION_WIIU)
  GetInputMap()->MapTrigger(HAVOKAI_SHOW_MENU, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_SHOULDER);
#endif
}

void HavokAiApp::ParseCommandLine()
{
#if defined(WIN32)
  VMemoryTempBuffer<256> tmpString = GetCommandLine();
  if (!VStringUtil::IsEmpty(tmpString.AsChar()))
  {
    VStringTokenizerInPlace tokenizer(tmpString.AsChar(), ' ', true);
    for (unsigned int i = 0; i < tokenizer.GetTokenCount(); i++)
    {
      const VString sArgument = tokenizer.Next();
      if (sArgument == "-streaming")
      {
        m_bUseStreaming = true;
      }
    }
  }
#endif
}

/*
 * Havok SDK - Base file, BUILD(#20140327)
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
