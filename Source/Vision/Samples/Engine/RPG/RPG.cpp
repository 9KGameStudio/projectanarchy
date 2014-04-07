/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Samples/Engine/RPG/RPGPCH.h>
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>
#include <Vision/Samples/Engine/RPGPlugin/GameManager.h>

#include <Vision/Runtime/Framework/VisionApp/VAppImpl.hpp>

#include <Vision/Runtime/Framework/VisionApp/Modules/VExitHandler.hpp>
#include <Vision/Runtime/Framework/VisionApp/Modules/VDefaultMenu.hpp>
#include <Vision/Runtime/Framework/VisionApp/Modules/VLoadingScreen.hpp>
#include <Vision/Runtime/Framework/VisionApp/Modules/VRestoreScreen.hpp>
#include <Vision/Runtime/Framework/VisionApp/Modules/VDebugOptions.hpp>
#include <Vision/Runtime/Framework/VisionApp/Modules/VDebugShadingModes.hpp>
#include <Vision/Runtime/Framework/VisionApp/Modules/VDebugProfiling.hpp>
#include <Vision/Runtime/Framework/VisionApp/Modules/VHelp.hpp>

enum GUI_DEMO_CONTROL 
{
  GUI_DEMO_CONTINUE = VAPP_INPUT_CONTROL_LAST_ELEMENT + 1,
  GUI_DEMO_EXIT
};

enum GUIState
{
  GS_INIT,
  GS_GAME
};

#define LOGOHAVOK_FILENAME "GUI/Textures/Logo_Havok_128x64.tga"
#define LOGOWMD_FILENAME "GUI/Textures/Logo_WMD_128x256.tga"
#define INTROSCREEN_FILENAME "GUI/Textures/Anarchy_Intro_512x512.tga"
#define GAMEOVERSCREEN_FILENAME "GUI/Textures/Anarchy_GameOver_512x512.tga"
#define LOADINGSCREEN_FILENAME "GUI/Textures/Anarchy_Splash_512x512.tga"
#define SCENE_FILENAME "Scenes/Temple/Temple.vscene"

class RPGApp : public VAppImpl
{
public:
  RPGApp();
  virtual ~RPGApp() {}

  virtual void PreloadPlugins() HKV_OVERRIDE { VISION_PLUGIN_ENSURE_LOADED(RPGPlugin); }

  virtual void Init() HKV_OVERRIDE;
  virtual void AfterEngineInit() HKV_OVERRIDE;
  virtual void AfterSceneLoaded(bool bLoadingSuccessful) HKV_OVERRIDE;
  virtual bool Run() HKV_OVERRIDE;
  virtual void DeInit() HKV_OVERRIDE;

private:
  void LoadSplashScreen(const char* szFilename, const bool bShowLogo);
  void ClearSplashScreen();

  GUIState m_state;

  VisScreenMaskPtr m_spSplashScreen;
  VisScreenMaskPtr m_spBackground;
  VisScreenMaskPtr m_spLogoHavok;
  VisScreenMaskPtr m_spLogoWMD;

#if defined(_VISION_MOBILE)
  VTouchAreaPtr m_spDemoTapArea;
#endif
};

VAPP_IMPLEMENT_SAMPLE(RPGApp);

RPGApp::RPGApp()
  : m_state(GS_INIT)
  , m_spSplashScreen(NULL)
  , m_spBackground(NULL)
  , m_spLogoHavok(NULL)
  , m_spLogoWMD(NULL)
#if defined(_VISION_MOBILE)
  , m_spDemoTapArea(NULL)
#endif
{

}

void RPGApp::Init()
{
  m_state = GS_INIT;

  // Init the application
  SetupScene(VisAppLoadSettings("", ":havok_sdk/Data/Vision/Samples/Engine/RPG"));
    
  // Disable automatic use of 2X assets on smaller screens even if the density is pretty high. 
  // For instance, on the HTC One X (720x1280) the buttons are just too big when using the 
  // 2x version.
  if (Vision::Video.GetYRes() < 1000)
	  Vision::Video.SetAllowAutomaticUseOf2xAssets(false);

  // Intro screen
  LoadSplashScreen(INTROSCREEN_FILENAME, true);

#if defined(SUPPORTS_KEYBOARD)
  GetInputMap()->MapTrigger(GUI_DEMO_CONTINUE, V_KEYBOARD, CT_KB_ANYKEY);
  GetInputMap()->MapTrigger(GUI_DEMO_EXIT, V_KEYBOARD, CT_KB_ESC);

# if defined(SUPPORTS_MOUSE)
  GetInputMap()->MapTrigger(GUI_DEMO_CONTINUE, V_MOUSE, CT_MOUSE_LEFT_BUTTON);
# endif

  VArray<const char*> help;
  help.Append("Controls for this demo (PC-Version):");
  help.Append("");
  help.Append("[LEFT MOUSE BUTTON] : Move character or do a simple attack");
  help.Append("[LEFT MOUSE BUTTON] + [SHIFT] : Throw a magic axe");
  help.Append("[RIGHT MOUSE BUTTON] : Hit the axe to the ground (consumes Mana)");
  help.Append("[MIDDLE MOUSE BUTTON] : Swing the axe around character (consumes Mana)");

  VHelp* pHelp = GetAppModule<VHelp>();
  if (pHelp == NULL)
    RegisterAppModule(new VHelp(help));
  else
    pHelp->SetText(help);

#elif defined(_VISION_MOBILE)
  m_spDemoTapArea = new VTouchArea(VInputManager::GetTouchScreen(), VRectanglef(0.0f, 0.0f, Vision::Video.GetXRes(), Vision::Video.GetYRes()));
  GetInputMap()->MapTrigger(GUI_DEMO_CONTINUE, m_spDemoTapArea, CT_TOUCH_ANY);
#endif
}

void RPGApp::AfterEngineInit()
{
  VLoadingScreenBase::Settings loadSettings(LOADINGSCREEN_FILENAME);
  loadSettings.m_fFadeOutTime = 2.0f;
  loadSettings.m_backgroundColor = V_RGBA_BLACK;
  loadSettings.m_eAspectRatioAlignment = VLoadingScreenBase::ALIGN_VERTICAL;
  RegisterAppModule(new VLoadingScreen(loadSettings));
  RegisterAppModule(new VRestoreScreen(0.7f, 0.2f));

  RegisterAppModule(new VDefaultMenu);
  RegisterAppModule(new VDebugOptions);
  RegisterAppModule(new VDebugShadingModes);
  RegisterAppModule(new VDebugProfiling);
}

void RPGApp::AfterSceneLoaded(bool bLoadingSuccessful)
{
}

bool RPGApp::Run()
{
  if  (m_state == GS_INIT)
  {
    const bool bCont = GetInputMap()->GetTrigger(GUI_DEMO_CONTINUE);
    const bool bExit = GetInputMap()->GetTrigger(GUI_DEMO_EXIT);

    VExitHandler* pExitHandler = GetAppModule<VExitHandler>();
    if ((pExitHandler == NULL || !pExitHandler->IsExitDialogVisible()) && !bExit && bCont)
    {
      // Reset intro screen
      ClearSplashScreen();
      
      // Load scene
      VisAppLoadSettings settings(SCENE_FILENAME);
      settings.m_bReplaceSearchPaths = false;
      LoadScene(settings);
      m_state = GS_GAME;
    }
  }
  else if (m_state == GS_GAME)
  {
    if (RPG_GameManager::s_instance.IsGameOver())
    {
      // Unload scene
      GetSceneLoader().ClearScene();

      // Show end load screen  
      LoadSplashScreen(GAMEOVERSCREEN_FILENAME, true);
      m_state = GS_INIT;
    }
  }

  return true;
}

void RPGApp::DeInit()
{
  ClearSplashScreen();

#if defined(_VISION_MOBILE)
  m_spDemoTapArea = NULL;
#endif

  GetSceneLoader().UnloadScene();
}

void RPGApp::LoadSplashScreen(const char* szFilename, const bool bShowLogo)
{
  const float fWidth = (float) Vision::Video.GetXRes();
  const float fHeight = (float) Vision::Video.GetYRes();

  m_spBackground = new VisScreenMask_cl("plainwhite.dds");
  m_spBackground->SetTargetSize(fWidth, fHeight);
  m_spBackground->SetPos(0.f, 0.f, 0.f);
  m_spBackground->SetColor(V_RGBA_BLACK);
  m_spBackground->SetOrder(500);

  m_spSplashScreen = new VisScreenMask_cl(szFilename);
  m_spSplashScreen->SetDepthWrite(FALSE);
  m_spSplashScreen->SetTransparency (VIS_TRANSP_ALPHA);
  m_spSplashScreen->SetOrder(200);

  if (fWidth > fHeight)
  {
    m_spSplashScreen->SetTargetSize(fHeight, fHeight);
    m_spSplashScreen->SetPos((fWidth - fHeight) * 0.5f, 0.f);
  }
  else
  {
    m_spSplashScreen->SetTargetSize(fWidth, fWidth);
    m_spSplashScreen->SetPos(0.f, (fHeight - fWidth) * 0.5f);
  }

  // Display logos
  if (bShowLogo)
  {
    int xSize, ySize;
    m_spLogoHavok = new VisScreenMask_cl(LOGOHAVOK_FILENAME);
    m_spLogoHavok->SetDepthWrite(FALSE);
    m_spLogoHavok->GetTextureSize(xSize, ySize);
    m_spLogoHavok->SetTargetSize((float) xSize,(float)  ySize);
    m_spLogoHavok->SetPos(fWidth - xSize - 20, fHeight - ySize - 10);

    m_spLogoWMD = new VisScreenMask_cl(LOGOWMD_FILENAME);
    m_spLogoWMD->SetDepthWrite(FALSE);
    m_spLogoWMD->GetTextureSize(xSize, ySize);
    m_spLogoWMD->SetTargetSize((float) xSize,(float)  ySize);
    m_spLogoWMD->SetPos(20, fHeight - ySize - 10);
  }
}

void RPGApp::ClearSplashScreen()
{
  m_spSplashScreen = NULL;
  m_spBackground = NULL;
  m_spLogoHavok = NULL;
  m_spLogoWMD = NULL;
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
