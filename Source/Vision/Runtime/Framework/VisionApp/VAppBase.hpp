/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \defgroup VisionAppFramework Vision application framework.
/// \image html VisionAppFramework.jpg
///
/// Vision's application framework is a collection of classes which help to build an applications with the Vision Engine.
/// The core classes are VAppBase and VAppImpl which encapsulate all functionality to run an application on various platforms.
/// Beside this the framework uses so called application modules which represent additional functionality, such as loading screen, debug options, etc.
/// As shown in the image above, a custom application is simply derived from VAppImpl and extends VAppImpl's functionality as needed.

#ifndef __V_APP_BASE_HPP
#define __V_APP_BASE_HPP

#include <Vision/Runtime/Engine/System/Vision.hpp>
#include <Vision/Runtime/Engine/Application/VisionApp.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scene/VSceneLoader.hpp>

#include <Vision/Runtime/Framework/VisionApp/VAppImportExport.hpp>
#include <Vision/Runtime/Framework/VisionApp/Helper/VAppHelper.hpp>

#ifdef HK_DEBUG_SLOW
  #define VISION_INTER_LINKING_CHECK VisionInterDebugReleaseLinkingCheck(); vBaseInterDebugReleaseLinkingCheck();
#else
  #define VISION_INTER_LINKING_CHECK VisionInterReleaseDebugLinkingCheck(); vBaseInterReleaseDebugLinkingCheck();
#endif

class VAppImpl;
class VStartupModule;

#define VAPP_INPUT_CONTROL_USER_SPACE 64
#define VAPP_INPUT_CONTROL_ALTERNATIVES 4

// Vision related input triggers
enum VAPP_INPUT_CONTROL 
{
  VAPP_EXIT = 0,
  VAPP_MENU,
  VAPP_MENU_UP,
  VAPP_MENU_DOWN,
  VAPP_MENU_CONFIRM,
  VAPP_MENU_BACK,

  VAPP_INPUT_CONTROL_LAST_ELEMENT = VAPP_MENU_BACK + 10 // reserve some space for custom module stuff
};

/// \brief
///   This class is provided to help you use Vision features, and to give a unified framework for samples.
///
/// The VAppBase is not directly instanced, instead on each supported platform a derived class is automatically
/// instanced which also takes care about all the platform specific details.
///
/// Usually there is no need to change, derive from or instantiate this class or derived ones. The VAppBase
/// class is designed to encapsulate all platform specific features. The starting point for your own application
/// is the VAppImpl class.
///
/// \sa VAppImpl
/// \ingroup VisionAppFramework
class VAppBase : public VisionApp_cl, public IVisCallbackHandler_cl
{
public:
  typedef VSmartPtr<VAppBase> VAppBasePtr;

  ///
  /// @name Static
  /// @{
  ///

  /// \brief
  ///   Gets a pointer to the VAppBase instance.
  ///
  /// \return
  ///   Pointer to the VAppBase instance.
  VAPP_IMPEXP static VAppBase* Get();

  /// \brief
  ///   Callbacks
  VAPP_IMPEXP static VisCallback_cl OnAppStateChanged;

  /// \brief
  ///   Registers a Startup Module which is executed before Vision is initialized.
  ///
  /// Startup Modules are used to perform platform specific task like showing native dialogs for instance.
  /// Startup Modules are executed before the VAppBase is actually created, thus this function can't called
  /// directly, instead use REGISTER_STARTUP_MODULE(MyStartupModule, 123).
  ///
  /// \param pModule
  ///   Startup Module instance.
  ///
  /// \param iPriority
  ///   Startup Module priority. Startup Modules are executed depending on their priority.
  ///
  /// \return
  ///   Pointer to the VAppBase instance.
  VAPP_IMPEXP static bool RegisterStartupModule(VStartupModule* pModule, int iPriority);

  /// \brief
  VAPP_IMPEXP static void ProcessStartupModules();

  ///
  /// @}
  ///

  ///
  /// @name Constructors / Destructor
  /// @{
  ///

  /// \brief
  ///   Constructor. Initializes the basic variables for an application, but does not initialize the engine.
  VAPP_IMPEXP VAppBase();

  /// \brief
  ///   Destructor of the VAppBase class
  VAPP_IMPEXP ~VAppBase();

  ///
  /// @}
  ///

  ///
  /// @name Init / Run / DeInit
  /// @{
  ///

  /// \brief
  ///   Executes the application.
  ///
  /// This function blocks until the application has finished on non callback based platforms.
  ///
  /// \param pImpl
  ///   VAppImpe instance.
  /// \sa VAppImpl
  VAPP_IMPEXP virtual void Execute(VAppImpl* pImpl);

  /// \brief
  ///   Returns whether the platform is callback based or not.
  ///
  /// Callback based platforms, such as iOS for instance, must return to the operating system after each frame.
  ///
  /// \return
  ///  True when platform is callback based, false otherwise.
  virtual bool IsCallbackBased() const { return false; }

  /// \brief
  ///   Initializes the base application.
  ///
  /// \return
  ///   Returns whether initialization was successful or not.
  VAPP_IMPEXP virtual bool AppInit();

  /// \brief
  ///   Initializes the base application.
  ///
  /// \return
  ///   Returns whether initialization was successful or not.
  VAPP_IMPEXP virtual bool AppRun();
  VAPP_IMPEXP virtual bool AppDeInit();

  /// \brief
  ///   Initializes the thread manager.
  VAPP_IMPEXP virtual void AppInitThreadManager();

  /// \brief
  ///   Called when initialization has failed.
  VAPP_IMPEXP virtual void AppInitFailed();

  /// \brief
  ///   Called after the engine has been initialized.
  VAPP_IMPEXP virtual void AppAfterEngineInit();

  /// \brief
  ///   Platform specific initialization.
  VAPP_IMPEXP virtual void PlatformInit();

  /// \brief
  ///   Platform specific settings initialization.
  VAPP_IMPEXP virtual void PlatformInitSettings();

  /// \brief
  ///   Platform specific input initialization.
  VAPP_IMPEXP virtual void PlatformMapInput() {}

  /// \brief
  ///   Platform specific run function.
  VAPP_IMPEXP virtual bool PlatformRun() { return true; }

  /// \brief
  ///   Platform specific de-initialization.
  VAPP_IMPEXP virtual void PlatformDeInit();

  ///
  /// @}
  ///

  ///
  /// @name Helper
  /// @{
  ///

  /// \brief
  ///   Returns platform specific threading model parameters.
  VAPP_IMPEXP virtual VAppHelper::VPlatformThreadingModel GetThreadingModel() { return VAppHelper::VPlatformThreadingModel(); }

  /// \brief
  ///   Returns platform specific prefix.
  VAPP_IMPEXP virtual void SetupPlatformRootFileSystem();

  /// \brief
  ///   Returns the directory where the application can store private data that won't get 
  ///   deleted when the application's cache is cleared.
  virtual VString GetApplicationDataDirectory() = 0;

  /// \brief
  ///   Returns the platform's directory where common data can be stored.
  virtual VString GetPlatformStorageDirectory() = 0;

  /// \brief
  ///   Returns the platform specific cache directory
  virtual VString GetPlatformCacheDirectory() = 0;

  /// \brief
  ///   Returns the number of command-line arguments specified when the application 
  ///   was started (if supported by the underlying platform). 
  ///
  /// The path and name of the executable is not included; i.e., the queryable
  /// list of arguments starts with the first actual argument.
  ///
  /// \return
  ///   The number of command-line arguments
  virtual unsigned int GetNumCommandLineArguments() const { return 0; }

  /// \brief
  ///   Returns a specific command-line arguments.
  ///
  /// The path and name of the executable is not included; i.e., the queryable
  /// list of arguments starts with the first actual argument.
  ///
  /// \param index
  ///   the index of the argument to return
  /// \return
  ///   the requested argument as an UTF-8 encoded string, or \c NULL if no argument 
  ///   with the specified index exists
  virtual const char* GetCommandLineArgument(unsigned int index) const { return NULL; }

  /// \brief
  ///   Setup data directories which includes the base data directory and all data directories
  ///   given by the application implementation.
  VAPP_IMPEXP virtual void SetupBaseDataDirectories();

  VAPP_IMPEXP virtual void OnHandleCallback(IVisCallbackDataObject_cl* pData) HKV_OVERRIDE;

  VAPP_IMPEXP virtual int64 GetCallbackSortingKey(VCallback *pCallback) HKV_OVERRIDE;

  VAPP_IMPEXP virtual void OnLoadSceneStatus(int iStatus, float fPercentage) HKV_OVERRIDE;

  VAPP_IMPEXP virtual IVRendererNode* CreateRendererNode() HKV_OVERRIDE;

  /// \brief
  ///   Application status update.
  VAPP_IMPEXP virtual void UpdateApplicationState();

  /// \brief
  ///   Returns the application status.
  inline VAppStateRef GetAppState() const { return m_eAppState; }

  /// \brief
  ///   Sets the application status.
  VAPP_IMPEXP void SetAppState(VAppHelper::VApplicationState eAppState);

  /// \brief
  ///   Returns the application implementation.
  inline VAppImpl* GetAppImpl() { return m_pAppImpl; }

  /// \brief
  ///   Returns the path the application was started from.
  inline const char* GetStartupPath() const { return m_sStartupPath; }

  ///
  /// @}
  ///

protected:
  /// \brief
  ///   Helper structure used for storing startup modules.
  struct VStartupElement 
  {
    VStartupElement() : m_pModule(NULL), m_iPriority(0) {}
    VStartupElement(VStartupModule* pModule, int iPriority) : m_pModule(pModule), m_iPriority(iPriority) {}
    VStartupModule* m_pModule;
    int m_iPriority;
  };
  static VArray<VStartupElement>* s_pStartupModules;

  VAppImpl * m_pAppImpl;
  IVisBackgroundResourceRestorer_cl* m_pResourceRestorer;

private:
  static VAppBase* s_instance;
  VAppHelper::VApplicationState m_eAppState;
  VString m_sStartupPath;
};

#endif //__V_APP_BASE_HPP

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
