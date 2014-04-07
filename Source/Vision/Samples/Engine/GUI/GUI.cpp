/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// ***********************************************************************************************
// GUI Demo
// Copyright (C) Havok.com Inc. All rights reserved.
// ***********************************************************************************************
// This is a demo which provides a completely object oriented Graphical User Interface system
// based on Vision screenmasks.
// This system is reusable and extendable. For instance, a new class can be derived from the item class to
// make a custom type of menu items, such as scrollbars, etc.
// ***********************************************************************************************
#include <Vision/Samples/Engine/GUI/GUIPCH.h>
#include <Vision/Runtime/Framework/VisionApp/VAppImpl.hpp>
#include <Vision/Runtime/Framework/VisionApp/Modules/VExitHandler.hpp>

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VMenuIncludes.hpp>
#include <Vision/Samples/Engine/GUI/MonitorContext.hpp>
#include <Vision/Runtime/Framework/VisionApp/Helper/VDataDirectoryHelper.hpp>

// Sample flags, and filename configurations
#if defined(_VISION_MOBILE) || defined(HK_ANARCHY) || defined( _VISION_APOLLO ) || defined( _VISION_METRO )   // TODO: Define _VISION_MOBILE on Apollo.
  #define MENU_SYSTEM_FILENAME "GUI\\Dialogs\\MenuSystemMobile.xml"
  #define MAIN_MENU_FILENAME "GUI\\Dialogs\\MainMenuMobile.xml"
#else
  #define MENU_SYSTEM_FILENAME "GUI\\Dialogs\\MenuSystem.xml"
  #define MAIN_MENU_FILENAME "GUI\\Dialogs\\MainMenu.xml"
#endif

#define MAP_DATA_DIR  ":havok_sdk/Data/Vision/Samples/Engine/Maps/SciFi"
#define SCENE_NAME "Crossing"
#define MESH_NAME "Crossing.vmesh"

// Optional callback listener that can translate all text labels in the GUI.
// Note: Does not do anything in this version.
class TextLabelTranslator : public IVisCallbackHandler_cl
{
public:
  virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData)
  {
    VTextLabelDataObject *pTextData = (VTextLabelDataObject *)pData;
    if (!pTextData->m_szLabel)
      return;

    // Optionally do some localization here...
    //pTextData->m_bTranslated = true;
    //pTextData->m_szLabel = "Translated version";
  }
};
TextLabelTranslator translator;



class GUISampleApp : public VAppImpl
{
public:
  enum GUIState
  {
    GS_MAIN_MENU,
    GS_GAME
  };

  GUISampleApp() 
    : m_state(GS_MAIN_MENU)
    , m_spMonitor(NULL)
    , m_pMainMenuDialog(NULL)
    , m_pMessageBoxInGame(NULL) 
    , m_spBackMask(NULL)
  {}
  virtual ~GUISampleApp() {}

  virtual void Init() HKV_OVERRIDE;
  virtual void SetupAppConfig(VisAppConfig_cl& config) HKV_OVERRIDE;
  virtual void AfterSceneLoaded(bool bLoadingSuccessful) HKV_OVERRIDE;
  virtual bool Run() HKV_OVERRIDE;
  virtual void DeInit() HKV_OVERRIDE;
  
private:
  GUIState m_state;

  VMonitorGUIContextPtr m_spMonitor;

  VDialog* m_pMainMenuDialog;
  VDialog* m_pMessageBoxInGame;
  VisScreenMaskPtr m_spBackMask;
};

VAPP_IMPLEMENT_SAMPLE(GUISampleApp);

void GUISampleApp::Init()
{
#if defined(_VISION_MOBILE) || defined(HK_ANARCHY)
  Vision::Renderer.SetUseSingleBufferedStaticMeshes(false);
#endif

  SetupScene(VisAppLoadSettings(SCENE_NAME, MAP_DATA_DIR));
  

  Vision::Error.SetReportStatus(FALSE);
  Vision::ResourceSystem.SetReportSurvivingResources(FALSE);

  VAppMenuContextPtr spContext = GetContext();

  // Load some GUI resources:
  VGUIManager::GlobalManager().LoadResourceFile(MENU_SYSTEM_FILENAME);

  VGUIManager::OnTextLabelCallback += translator;

  // Use a modified default tooltip
  VTooltip *pCustomTooltip = new VTooltip();
  pCustomTooltip->SetDelay(0.5f); // shorter delay than usual
  spContext->SetTooltipTemplate(pCustomTooltip);
  m_pMainMenuDialog = spContext->ShowDialog(MAIN_MENU_FILENAME);

  // Prepare the world in order to have skinning shaders for the render to texture menu
  Vision::InitWorld();

#ifdef SUPPORTS_MOUSE
  GetInputMap()->MapTrigger(GUI_DEMO_ACTION_1, V_MOUSE, CT_MOUSE_LEFT_BUTTON);
  GetInputMap()->MapTrigger(GUI_DEMO_ACTION_2, V_MOUSE, CT_MOUSE_RIGHT_BUTTON);
  GetInputMap()->MapTrigger(GUI_DEMO_ACTION_3, V_MOUSE, CT_MOUSE_MIDDLE_BUTTON);
#endif

#if defined (_VISION_XENON) || (defined(_VISION_WINRT) && !defined(_VISION_METRO) && !defined(_VISION_APOLLO))
  GetInputMap()->MapTrigger(GUI_DEMO_ACTION_1, V_XENON_PAD(0), CT_PAD_A);
  GetInputMap()->MapTrigger(GUI_DEMO_ACTION_2, V_XENON_PAD(0), CT_PAD_B);
  GetInputMap()->MapTrigger(GUI_DEMO_ACTION_3, V_XENON_PAD(0), CT_PAD_X);

#elif defined (_VISION_PS3)
  GetInputMap()->MapTrigger(GUI_DEMO_ACTION_1, V_PS3_PAD(0), CT_PAD_CROSS);
  GetInputMap()->MapTrigger(GUI_DEMO_ACTION_2, V_PS3_PAD(0), CT_PAD_CIRCLE);
  GetInputMap()->MapTrigger(GUI_DEMO_ACTION_3, V_PS3_PAD(0), CT_PAD_SQUARE);

#elif defined (_VISION_PSP2)
  GetInputMap()->MapTrigger(GUI_DEMO_ACTION_1, V_PSP2_PAD(0), CT_PAD_CROSS);
  GetInputMap()->MapTrigger(GUI_DEMO_ACTION_2, V_PSP2_PAD(0), CT_PAD_CIRCLE);
  GetInputMap()->MapTrigger(GUI_DEMO_ACTION_3, V_PSP2_PAD(0), CT_PAD_SQUARE);

#elif defined (_VISION_WIIU)
  GetInputMap()->MapTrigger(GUI_DEMO_ACTION_1, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_B);
  GetInputMap()->MapTrigger(GUI_DEMO_ACTION_2, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_A);
  GetInputMap()->MapTrigger(GUI_DEMO_ACTION_3, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_Y);

#endif

  // Add additional back touch area on mobile
#if defined(_VISION_MOBILE)
  if (m_spBackMask == NULL)
  {
    const int iScreenWidth = Vision::Video.GetXRes();
    const int iScreenHeight = Vision::Video.GetYRes();
    const float fUiScale = VAppHelper::GetUIScalingFactor();

    m_spBackMask = new VisScreenMask_cl("GUI/Dialogs/ButtonQuit_Normal.TGA");
    
    int iTextureWidth, iTextureHeight;
    m_spBackMask->GetTextureSize(iTextureWidth, iTextureHeight);
    const hkvVec2 vTargetSize = hkvVec2(iTextureWidth, iTextureHeight) * fUiScale;
    m_spBackMask->SetTargetSize(vTargetSize.x, vTargetSize.y);
    
    const hkvVec2 vPos = hkvVec2(iScreenWidth - vTargetSize.x - 16.0f, 16.0f);
    m_spBackMask->SetPos(vPos.x, vPos.y);
    m_spBackMask->SetTransparency(VIS_TRANSP_ALPHA);
    m_spBackMask->SetVisible(TRUE);

    VTouchArea* pDemoExitArea = new VTouchArea(VInputManager::GetTouchScreen(), 
      VRectanglef(vPos.x, vPos.y, vPos.x + vTargetSize.x, vPos.y + vTargetSize.y), 0.0f);
    GetInputMap()->MapTrigger(VAPP_EXIT, pDemoExitArea, CT_TOUCH_ANY);
  }
#endif
}

void GUISampleApp::SetupAppConfig(VisAppConfig_cl& config)
{
  config.m_videoConfig.m_bWaitVRetrace = true;

#if defined(_VISION_PSP2)
  config.m_videoConfig.m_iXRes = 960;
  config.m_videoConfig.m_iYRes = 544;
#endif
}

void GUISampleApp::AfterSceneLoaded(bool bLoadingSuccessful)
{
  if (m_state == GS_GAME)
  {
    VExitHandler* pExit = GetAppModule<VExitHandler>();
    if (pExit)
      pExit->SetEnabled(false);

    VisBaseEntity_cl* pCamera = Vision::Game.CreateEntity("VFreeCamera", hkvVec3::ZeroVector());
    pCamera->SetPosition(150, 0, 200);
    pCamera->SetOrientation(180, -15, 0);

    VisSurface_cl *pScreenSrf = NULL;
    VMeshManager* MeshManager = VMeshManager::GetMeshManager();
    VisStaticMesh_cl* CinemaMesh = (VisStaticMesh_cl*)MeshManager->GetResourceByName(MESH_NAME);

    if( CinemaMesh )
      pScreenSrf = CinemaMesh->GetSurfaceByName("MonitorScreen01_Mat");

    // Put GUI on one of the scene's monitors.
    m_spMonitor = new VMonitorGUIContext(NULL, pScreenSrf);
    m_spMonitor->SetActivate(true);
    m_spMonitor->ShowDialog("GUI\\Dialogs\\MonitorDialog.xml");
  }
}

bool GUISampleApp::Run()
{
  switch (m_state)
  {
  case GS_MAIN_MENU:
    {
      VASSERT(m_pMainMenuDialog);

      int iDlgResult = m_pMainMenuDialog->GetDialogResult();
      if (iDlgResult)
      {
        if (iDlgResult == VGUIManager::GetID("CANCEL"))
        {
          GetContext()->CloseDialog(m_pMainMenuDialog);
          m_pMainMenuDialog = NULL;
          return false;
        }
        else if (iDlgResult == VGUIManager::GetID("NEWGAME"))
        {
          GetContext()->CloseDialog(m_pMainMenuDialog);
          m_pMainMenuDialog = NULL;

          m_state = GS_GAME;

          // Load a map with free camera...
          LoadScene(VisAppLoadSettings(SCENE_NAME, MAP_DATA_DIR));

          return true;
        }
      }

      return true;
    }

  case GS_GAME:
    {
      if (GetInputMap()->GetTrigger(VAPP_EXIT) && (m_pMessageBoxInGame == NULL))
      {
        VAppMenu* pMenu = GetAppModule<VAppMenu>();
        if (pMenu)
          pMenu->Reset();

        // Show in-game menu, allowing the user to exit to main menu
        m_pMessageBoxInGame = GetContext()->ShowDialog("GUI/Dialogs/MessageBox_InGame.xml");
        VInputMap::LockInputMaps(true);
      }

      if (m_pMessageBoxInGame)
      {
        int iDlgResult = m_pMessageBoxInGame->GetDialogResult();
        if (iDlgResult)
        {
          VInputMap::LockInputMaps(false);

          // Close the dialog when a result is available
          GetContext()->CloseDialog(m_pMessageBoxInGame);
          m_pMessageBoxInGame = NULL;

          // In case of MAINMENU, unload scene and go to main menu again
          if (iDlgResult == VGUIManager::GetID("MAINMENU"))
          {
            m_spMonitor->SetActivate(false);
            m_spMonitor = NULL;

            if (Vision::World.IsWorldInitialized())
              Vision::DeInitWorld();
            Vision::InitWorld();
            m_state = GS_MAIN_MENU;

            VExitHandler* pExit = GetAppModule<VExitHandler>();
            if (pExit)
              pExit->SetEnabled(true);

            m_pMainMenuDialog = GetContext()->ShowDialog(MAIN_MENU_FILENAME);
          }
        }
      }

      return true;
    }

  default:
    VASSERT(false);
    return false;
  }

  return false;
}

void GUISampleApp::DeInit()
{
  // Cleanup GUI related stuff just before sample de-initialization
  VGUIManager::GlobalManager().CleanupResources();
  VGUIManager::OnTextLabelCallback -= translator;

  if (m_spMonitor)
  {
    m_spMonitor->SetActivate(false);
    m_spMonitor = NULL;
  }

  m_pMainMenuDialog = NULL;
  m_pMessageBoxInGame = NULL;
  m_spBackMask = NULL;
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
