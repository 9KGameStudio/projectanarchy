/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Tools/vPlayer/vPlayerPCH.h>
#include <Vision/Tools/vPlayer/vPlayerApp.hpp>
#include <Vision/Tools/vPlayer/vPlayerAppModule.hpp>
#include <Vision/Tools/vPlayer/vLoader.hpp>
#include <Vision/Tools/vPlayer/resource.h>

#include <Vision/Runtime/Framework/VisionApp/Modules/VCameraHandling.hpp>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp> 

// Include key-codes last so they are not disabled by some other product includes
#include <Common/Base/KeyCode.h>

VAPP_IMPLEMENT_SAMPLE(VPlayerApp);

#if defined(USE_FILESERVE)
  #include <Vision/Runtime/Framework/VisionApp/Modules/VFileServe.hpp>
  REGISTER_STARTUP_MODULE(VFileServeStartupModule, 0);
#endif

VPlayerApp::VPlayerApp()
{
  ParseCommandLine();
}

VPlayerApp::~VPlayerApp()
{

}

void VPlayerApp::SetupAppConfig(VisAppConfig_cl& config)
{
  config.m_videoConfig.m_szWindowTitle = "vPlayer";
  config.m_videoConfig.m_iXRes = m_Config.iResX;
  config.m_videoConfig.m_iYRes = m_Config.iResY;
  config.m_videoConfig.m_bFullScreen = m_Config.bFullscreen;
  config.m_videoConfig.m_bWaitVRetrace = m_Config.bWaitForVSync;
  config.m_videoConfig.m_iAdapter = m_Config.iAdapter;

  // only supported on desktop windows
#if defined(WIN32) && !defined(_VISION_WINRT)
  if (m_Config.bBorderlessFullscreen)
  {
    DEVMODEA deviceMode;
    deviceMode = Vision::Video.GetAdapterMode(m_Config.iAdapter);
    config.m_videoConfig.m_iXRes = deviceMode.dmPelsWidth;
    config.m_videoConfig.m_iYRes = deviceMode.dmPelsHeight;
    config.m_videoConfig.m_bFullScreen = false;
    config.m_windowConfig.SetBorderlessWindow(true);
  }
#endif
}

void VPlayerApp::PreloadPlugins()
{
  VISION_PLUGIN_ENSURE_LOADED(vFmodEnginePlugin);
  VISION_PLUGIN_ENSURE_LOADED(vHavok);

  PreloadHavokPlugins();
  PreloadThirdPartyPlugins();

#if defined(_DLL)
  //Always load the remote input plugin to enable starting the input server from scene scripts
  VISION_PLUGIN_ENSURE_LOADED(vRemoteInput);
#endif
}

void VPlayerApp::AfterEngineInit()
{
  VAppImpl::AfterEngineInit();

  VPlayerAppModule* pModule = new VPlayerAppModule(&m_Config);

  pModule->RegisterLoader(new SceneLoader());
  pModule->RegisterLoader(new MeshLoader());
  pModule->RegisterLoader(new PrefabLoader());
  pModule->RegisterLoader(new ZoneLoader());
  pModule->RegisterLoader(new LitLoader());

  RegisterAppModule(pModule);

  RegisterAppModule(new VCameraHandling());

  SetWindowIcon();
}

void VPlayerApp::Init()
{
  // always enable visual debugger
  if (vHavokPhysicsModule* pPhysicsModule = static_cast<vHavokPhysicsModule*>(Vision::GetApplication()->GetPhysicsModule()))
  {
    pPhysicsModule->SetEnabledVisualDebugger(true);
  }
}

void VPlayerApp::DeInit()
{

}

void VPlayerApp::AfterSceneLoaded(bool bLoadingSuccessful)
{
  VASSERT(GetAppModule<VPlayerAppModule>() != NULL);
  GetAppModule<VPlayerAppModule>()->FinalizeLoading(bLoadingSuccessful);
}

bool VPlayerApp::Run()
{
  VPlayerAppModule* pModule = GetAppModule<VPlayerAppModule>();
  VASSERT(pModule != NULL);
  pModule->ProcessPendingLoadingRequest();
  pModule->ProcessMessages();

  return true;
}

void VPlayerApp::ParseCommandLine()
{
  // Command line is only supported on desktop windows
#if defined(WIN32) && !defined(_VISION_WINRT)
    
  int argc;
  LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

  // skip the first argument since it contains the executable
  for (int i = 1; i < argc; ++i)
  {
    bool bScenefileRequired = false;

    LPWSTR pArg = argv[i];
    VASSERT(pArg);

    if (pArg[0]=='-' || pArg[0]=='/') // the argument is a command
    {
      pArg++;

      if (_wcsicmp(pArg, L"res") == 0)
      {
        if (i >= argc-2) // next two ones are x resp. y res
          continue;

        m_Config.iResX = _wtoi(argv[i+1]);
        m_Config.iResY = _wtoi(argv[i+2]);
        
        hkvMath::clamp(m_Config.iResX, 0, 4096);
        hkvMath::clamp(m_Config.iResY, 0, 4096);

        i+=2;
      }

      else if (_wcsicmp(pArg, L"fullscreen") == 0)
      {
        m_Config.bFullscreen = true;
      }

      else if (_wcsicmp(pArg, L"borderlessfullscreen") == 0)
      {
        m_Config.bBorderlessFullscreen = true;
      }

      else if (_wcsicmp(pArg, L"novsync") == 0)
      {
        m_Config.bWaitForVSync = false;
      }
      
      else if (_wcsicmp(pArg, L"adapter") == 0)
      {
        if (i >= argc-1) // next one is adapter index
          continue;

        i++;
        m_Config.iAdapter = _wtoi(argv[i]);
        hkvMath::clamp(m_Config.iAdapter, 0, 16);        
      }

      else if (_wcsicmp(pArg, L"varkeys") == 0)
      {
        if (i >= argc-1) // next one is variant key list
          continue;

        i++;
        m_Config.sVariantKeys = argv[i];
      }
    }
    else // interpret the argument as a filename
    {
      VString sPath(argv[i]);
      if (VFileAccessManager::IsPathRelative(sPath))
      {
        m_Config.sFileToLoad = VPathHelper::CombineDirAndFile(VAppBase::Get()->GetStartupPath(), sPath);
      }
      else
      {
        m_Config.sFileToLoad = sPath;
      }
    }
  }

  LocalFree(argv);
#endif
}

void VPlayerApp::PreloadHavokPlugins()
{
#if defined ( HAVOK_AI_KEYCODE )
  VISION_PLUGIN_ENSURE_LOADED(vHavokAi);
#else
  #pragma message ( "No Havok AI Keycode found. If you do not have a license for Havok AI, please remove the following library dependencies (and this line): vHavokAi*.lib, hkaiPathfinding.lib, hkaiInternal.lib, hkaiVisualize.lib, hkaiSpatialAnalysis.lib, hkaiPhysics2012Bridge.lib, as well as any platform-specific hkai* libraries.")
#endif

#if defined ( HAVOK_BEHAVIOR_KEYCODE )
  VISION_PLUGIN_ENSURE_LOADED(vHavokBehavior);
#else
  #pragma message ( "No Havok Behavior Keycode found. If you do not have a license for Havok Behavior, please remove the following library dependencies (and this line): vHavokBehavior*.lib, hkbBehavior.lib, hkbInternal.lib, hkbUtilities.lib, hkbPhysics2012Bridge.lib, hkbScript.lib, as well as any platform-specific hkb* libraries.")
#endif

#if !defined( HK_ANARCHY )
  #if defined ( HAVOK_DESTRUCTION_2012_KEYCODE )
    VISION_PLUGIN_ENSURE_LOADED(vHavokDestruction);
  #else
    #pragma message ( "No Havok Destruction Keycode found. If you do not have a license for Havok Destruction, please remove the following library dependencies (and this line): vHavokDestruction*.lib, hkdDestruction.lib, hkdInternal.lib, hkgpConvexDecomposition.lib, as well as any platform-specific hkd* libraries.")
  #endif

  #if defined ( HAVOK_CLOTH_KEYCODE )
    VISION_PLUGIN_ENSURE_LOADED(vHavokCloth);
  #else
    #pragma message ( "No Havok Cloth Keycode found. If you do not have a license for Havok Cloth, please remove the following library dependencies (and this line): vHavokCloth*.lib, hclCloth.lib, hclInternal.lib, hclSetup.lib, hclAnimationBridge.lib, hclPhysics2012Bridge.lib, as well as any platform-specific hcl* libraries.")
  #endif

  #if defined(WIN32) && !defined( _VISION_WINRT )
    #if defined ( HAVOK_SIMULATION_KEYCODE )
      VISION_PLUGIN_ENSURE_LOADED(vHavokVehicle);
      VISION_PLUGIN_ENSURE_LOADED(vHavokCharacter);
    #else
      #pragma message ( "No Havok Simulation found. If you do not have a license for Havok Simulation, please remove the following library dependencies (and this line): hkmsCharacter.lib, hkmsVehicle.lib, hkmsCommon.lib" )
    #endif
  #endif  
#endif
}

void VPlayerApp::PreloadThirdPartyPlugins()
{
#if !defined(HK_ANARCHY)
  // Plugins currently not available on Vision Mobile and WinRT
  #if !defined(_VISION_MOBILE) && !defined(_VISION_WINRT) && !defined(_VISION_PSP2)
    VISION_PLUGIN_ENSURE_LOADED(Speedtree5EnginePlugin);
    #pragma message ( "If you do not have a license for SpeedTree, please remove the following library dependencies (and this line): Speedtree5EnginePlugin*.lib" )
  #endif   
#endif // !defined(HK_ANARCHY)

#if defined(HK_ANARCHY)
  #if !defined(_VISION_TIZEN)
    VISION_PLUGIN_ENSURE_LOADED(vScaleformPlugin);
  #endif
#endif // defined(HK_ANARCHY)
}

void VPlayerApp::SetWindowIcon()
{
#if defined(WIN32) && !defined(_VISION_WINRT)
  HWND hWnd = Vision::Video.GetCurrentConfig()->m_hWnd;
  HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON));
  VASSERT(hIcon != NULL);
 
  SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
  SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
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
