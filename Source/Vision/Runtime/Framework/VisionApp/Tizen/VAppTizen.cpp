/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/Framework/VisionApp/Tizen/VAppTizen.hpp>
#include <Vision/Runtime/Framework/VisionApp/VAppImpl.hpp>

#include <Vision/Runtime/Base/System/IO/FileSystems/VFileServeDaemon.hpp>

#include <FSecurity.h>

extern "C" void VisionEnterBackgroundFunction();
extern "C" void VisionEnterForegroundFunction();

// Custom UI Form implementation for detecting the termination event.
class VTizenForm : public Tizen::Ui::Controls::Form
{
public:
  VTizenForm(VAppTizen* pApp) 
    : Tizen::Ui::Controls::Form()
    , m_pApp(pApp)
  {}

  virtual result OnTerminating() HKV_OVERRIDE
  {
    m_pApp->OnFormTerminating();

    return Tizen::Ui::Controls::Form::OnTerminating();
  }

private:
  VAppTizen* m_pApp;
};

VSmartPtr<VAppTizen> VAppTizen::s_spInstance = NULL;
VAppImpl* VAppTizen::s_pAppImpl = NULL;

Tizen::App::UiApp* VAppTizen::CreateInstance()
{
  if (s_spInstance == NULL)
    s_spInstance = new VAppTizen();

  return s_spInstance;
}

VAppTizen::VAppTizen()
  : VAppMobile()
  , m_pTimer(NULL)
  , m_bIsTerminating(false)
{
  Tizen::Base::String path = Tizen::App::App::GetInstance()->GetAppResourcePath().GetPointer();
  m_sAppDirectory = path.GetPointer();
  
  path = Tizen::App::App::GetInstance()->GetAppDataPath().GetPointer();
  m_sAppDataDirectory = path.GetPointer();
  m_sCacheDirectory = m_sAppDataDirectory;
  
  path = Tizen::System::Environment::GetMediaPath().GetPointer();
  m_sSdCardDirectory = path.GetPointer();
}

VAppTizen::~VAppTizen(void)
{
}

void VAppTizen::PlatformInitSettings()
{
#if !defined(FORCE_TRILINEAR_FILTERING)
  Vision::Renderer.SetDefaultTextureFilterMode(FILTER_MIN_MAG_LINEAR_MIP_POINT);
#endif

  VAppMobile::PlatformInitSettings();
}

void VAppTizen::PlatformMapInput()
{
  VInputMap* pInputMap = m_pAppImpl->GetInputMap();
  pInputMap->MapTrigger(VAPP_EXIT, VInputManagerTizen::GetKeyInput(), CT_KB_TIZEN_BACK, VInputOptions::OncePerFrame(ONCE_ON_RELEASE));
  pInputMap->MapTrigger(VAPP_MENU, VInputManagerTizen::GetKeyInput(), CT_KB_TIZEN_MENU, VInputOptions::OncePerFrame(ONCE_ON_RELEASE));
}

void VAppTizen::UpdateApplicationState()
{
  VAppMobile::UpdateApplicationState();
  
  // Refresh screen when loading
  if (VAppMobile::GetAppState() == VAppHelper::AS_SCENE_LOADING)
  {
    // TODO: Always allow the application to exit on android if the app is loading
    //if (GetInputMap()->GetTrigger(EXIT)) 
    //{
    //  m_sceneLoader.Close();
    //  m_sceneLoader.OnAbort();
    //  m_appState = AS_LOADING_ERROR;
    //  return m_appState;
    //}

    // We have to draw the screen masks here to ensure that we actually fill the 
    // frame buffer every time the API is called since the OS clears the frame 
    // buffer itself.
    Vision::Video.ResumeRendering();

    VisRenderContext_cl::GetMainRenderContext()->Activate();
    Vision::RenderScreenMasks();
    Vision::Video.UpdateScreen();
  }
}

void VAppTizen::SetupPlatformRootFileSystem()
{
  VAppMobile::SetupPlatformRootFileSystem();

  const VString& sRoot = m_appConfig.m_sFileSystemRootName;
  if(VFileServeDaemon::IsInitialized())
    VFileAccessManager::GetInstance()->SetRoot(sRoot, VFileServeDaemon::GetInstance()->CreateFileSystem(sRoot, m_sAppDirectory));
  else
    VFileAccessManager::GetInstance()->SetRoot(sRoot, m_sAppDirectory);

  VFileAccessManager::GetInstance()->SetRoot("app_data", GetApplicationDataDirectory(), VFileSystemFlags::WRITABLE);
}

VString VAppTizen::GetApplicationDataDirectory()
{
  return m_sAppDataDirectory;
}

VString VAppTizen::GetPlatformStorageDirectory()
{
  return m_sSdCardDirectory;
}

VString VAppTizen::GetPlatformCacheDirectory()
{
  return m_sCacheDirectory;
}

bool VAppTizen::OnAppInitializing(Tizen::App::AppRegistry& appRegistry)
{
  m_pTimer = new Tizen::Base::Runtime::Timer();
  m_pTimer->Construct(*this);

  return true;
}

bool VAppTizen::OnAppInitialized(void)
{
  using namespace Tizen::Ui;
  using namespace Tizen::Ui::Controls;

  // Create the app's UI frame.
  Frame* pAppFrame = new Frame();
  pAppFrame->Construct();
  pAppFrame->SetOrientation(ORIENTATION_LANDSCAPE);
  AddFrame(*pAppFrame);

  // Create and register our Tizen form with the app frame.
  Form* pTizenForm = new VTizenForm(this);
  pTizenForm->Construct(FORM_STYLE_NORMAL);
  GetAppFrame()->GetFrame()->AddControl(pTizenForm);

  // Set and handle device orientation.
  pTizenForm->SetOrientation(ORIENTATION_LANDSCAPE);
  pTizenForm->AddOrientationEventListener(*this);

  // Register for handling frame events. We use this only for deactivating the automatic switching off of the
  // screen - trying to do that in OnForeground() doesn't work as expected.
  pAppFrame->AddFrameEventListener(*this);

  // Register our Tizen form with Vision (to allow for hooking up input events etc.).
  VVideo::SetUIForm(pTizenForm);

  // Initialize application
  PlatformInit();
  
  if ((s_pStartupModules != NULL) && (s_pStartupModules->GetSize() > 0))
  {
    s_pStartupModules->GetAt(0).m_pModule->Init();
    return true;
  }
  else
  {
    VAppMobile::Execute(s_pAppImpl);
    s_pAppImpl = NULL;
    return AppInit();
  }
}

bool VAppTizen::OnAppTerminating(Tizen::App::AppRegistry& appRegistry, bool forcedTermination)
{
  if (m_pTimer != NULL)
  {
    m_pTimer->Cancel();
    delete m_pTimer;
    m_pTimer = NULL;
  }

  // Release the application, but let Tizen handle the actual destruction in order to shut down properly.
  VASSERT(s_spInstance.m_pPtr->GetRefCount() == 1);
  s_spInstance.m_pPtr->ReleaseNoDelete();
  s_spInstance.m_pPtr = NULL;
  return true;
}

void VAppTizen::OnFrameActivated(const Tizen::Ui::Controls::Frame &source)
{
  // Try to disable automatic screen locking.
  // Note: The application requires the http://tizen.org/privilege/power privilege for this.
  // Note: We do this here (instead of in OnForeground()) in order to make it work when the application starts up.
  //       Trying this in OnForeground() makes it work *after* the device is woken up again, but doesn't prevent
  //       the device to be put to sleep right after the application has been started (as of SDK 2.2.0b).
  if (Tizen::Security::AccessController::CheckPrivilege("http://tizen.org/privilege/power") == E_SUCCESS)
  {
    result res = Tizen::System::PowerManager::KeepScreenOnState(true, false);
    if (res != E_SUCCESS)
      hkvLog::Warning( "Unable to keep screen on - error: '%s'", (res == E_PRIVILEGE_DENIED) ? "PRIVILEGE DENIED" : "SYSTEM ERROR");
  }
  else
  {
#if defined(DEBUG_VERBOSE)
    hkvLog::Info("Unable to keep screen on - insufficient privileges\n");
#endif
  }
}

void VAppTizen::OnFrameDeactivated(const Tizen::Ui::Controls::Frame &source)
{
}

void VAppTizen::OnFrameTerminating(const Tizen::Ui::Controls::Frame &source)
{
}

void VAppTizen::OnFormTerminating()
{
  VASSERT_MSG(Vision::Video.GetUIForm() != NULL, "Tizen UI form already destroyed.");

  // Shut down the engine as soon as the form is terminating.
  AppDeInit();
  PlatformDeInit();

  Vision::Video.SetUIForm(NULL);
}

void VAppTizen::OnForeground(void)
{
  // ------------------------------------------------------------------------------------------------------
  // Note: Trying to do this here doesn't work as intended as of SDK 2.2.0b.
  //       We still keep this code around - if this works in a later SDK, we can avoid deriving from IFrameEventListener.
  //// Try to disable automatic screen locking. 
  //// Note: The application requires the http://tizen.org/privilege/power privilege for this.
  //hkvLog::Info( "Trying to set screen on state" );
  //result res = Tizen::System::PowerManager::KeepScreenOnState( true, false );
  //hkvLog::Info( "done..." );
  //if ( res != E_SUCCESS )
  //{
  //  hkvLog::Warning( "Unable to keep screen on - error: '%s'",
  //    ( res == E_PRIVILEGE_DENIED ? "PRIVILEGE DENIED" : "SYSTEM ERROR" ) );
  //}
  //else 
  //{
  //  hkvLog::Info( "SUCCESS - PowerManager::KeepScreenOnState( true, false ) returned E_SUCCESS\n" );
  //}
  // ------------------------------------------------------------------------------------------------------

  static bool bStartingUp = true;
  if (bStartingUp)
  {
    // The app receives the OnForeground event when starting up - we want to skip resource restoration in this case.
    bStartingUp = false;
  }
  else if(IsInitialized())
  {
    VisionEnterForegroundFunction();
  }

  if (m_pTimer)
    m_pTimer->Start(1);
}

void VAppTizen::OnBackground(void)
{
  if (m_pTimer)
    m_pTimer->Cancel();

  // No need to trigger a background event if the app gets terminated afterwards anyway.
  if (IsInitialized() && !m_bIsTerminating)
  {
    VisionEnterBackgroundFunction();
  }
}

void VAppTizen::OnLowMemory(void)
{
  Vision::Callbacks.OnMemoryWarning.TriggerCallbacks();
}

void VAppTizen::OnBatteryLevelChanged(Tizen::System::BatteryLevel batteryLevel)
{
  if (batteryLevel == Tizen::System::BATTERY_CRITICAL)
  {
    // Consider terminating the application...
  }
}

void VAppTizen::OnTimerExpired(Tizen::Base::Runtime::Timer& timer)
{
  if ((m_pTimer == NULL) || (&timer != m_pTimer))
    return;
  
  m_pTimer->Start(1);
  Draw();
}

void VAppTizen::OnOrientationChanged(const Tizen::Ui::Control &source, Tizen::Ui::OrientationStatus orientationStatus)
{
#if defined(DEBUG_VERBOSE)
  hkvLog::Info( "### Received new orientation: %d", orientationStatus );
#endif

  if (Vision::Video.IsInitialized())
  {
    Tizen::Ui::Controls::Form* pForm = Vision::Video.GetUIForm();
    if (pForm)
    {
      int iX = 0, iY = 0;
      int iWidth = 0, iHeight = 0;
      pForm->GetBounds(iX, iY, iWidth, iHeight);
      Vision::Video.ChangeScreenResolution(iWidth, iHeight, 0);
    }
  }
}

void VAppTizen::Draw(void)
{
  // The OS may change the current GL context in between callbacks.
  // Call MakeCurrent early to make sure texture uploading etc. works at any point during the frame.
  VVideo::MakeCurrent();

  if ((s_pStartupModules != NULL) && (s_pStartupModules->GetSize() > 0))
  {
    if (ProcessStartupModule(0))
      return;

    delete s_pStartupModules->GetAt(0).m_pModule;
    s_pStartupModules->RemoveAt(0);

    if (WantsToQuit())
    {
      DoQuit();
    }
    else if (s_pStartupModules->GetSize() > 0)
    {
      // Set up next startup module.
      s_pStartupModules->GetAt(0).m_pModule->Init();
    }
    else if (s_pAppImpl != NULL)
    {
      // Install the app implementation.
      VAppMobile::Execute(s_pAppImpl);
      s_pAppImpl = NULL;
      AppInit();
    }
    else
    {
      // Nothing left to do.
      DoQuit();
    }

    return;
  }

  if (!AppRun())
  {
    DoQuit();
  }
}

bool VAppTizen::ProcessStartupModule(int iIndex)
{
  VASSERT(iIndex < s_pStartupModules->GetSize());
  VStartupModule* pModule = s_pStartupModules->GetAt(iIndex).m_pModule;

  bool bResult = pModule->Run();

  if (WantsToQuit() || !bResult)
  {
    pModule->DeInit();
    return false;
  }

  return true;
}

void VAppTizen::DoQuit()
{
  m_bIsTerminating = true;
  Terminate();
}

/*
 * Havok SDK - Base file, BUILD(#20140625)
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
