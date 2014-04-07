/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef __V_APP_IMPL_HPP
#define __V_APP_IMPL_HPP

#include <Vision/Runtime/Engine/System/Vision.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scene/VSceneLoader.hpp>

#include <Vision/Runtime/Framework/VisionApp/VAppBase.hpp>
#include <Vision/Runtime/Framework/VisionApp/VAppModule.hpp>
#include <Vision/Runtime/Framework/VisionApp/VAppMain.inl>

/// \brief
///   This class is the base class for your application.
///
/// To create a new application simply derive from the VAppImpl and override the
/// corresponding base methods to configure/customize your application
/// implementation's behavior.
///
/// \example
///   \code
///   #include <Vision/Runtime/Framework/VisionApp/VAppImpl.hpp>
///
///   class MyApp : public VAppImpl
///   {
///   public:
///     MyApp() {}
///     virtual ~MyApp() {}
///
///     void Init() HKV_OVERRIDE
///     {
///       LoadScene(VisAppLoadSettings("ViewerMap.vscene", ":havok_sdk/Data/Vision/Samples/Engine/Maps/ViewerMap"));
///     }
///
///     void AfterSceneLoaded(bool bLoadingSuccessful) HKV_OVERRIDE
///     {
///       Vision::Game.CreateEntity("VFreeCamera", hkvVec3(0.0f, 0.0f, 0.0f));
///     }
///
///     void GetVideoConfigSettings(VVideoConfig& settings) HKV_OVERRIDE
///     {
///       // Enable vsync
///       settings.m_bWaitVRetrace = true;
///     }
///   };
///
///   VAPP_IMPLEMENT_SAMPLE(MyApp);
///   \endcode
///
/// The VAppImpl base class also features the option to register application
/// modules. These modules are intended for extending the capabilities of an
/// application. All default features like the loading screen, the application
/// menu, or the debug options are realized as such modules and can easily
/// be plugged into the application.
///
/// The method ordering in this class exactly matches the order in which
/// the VAppBase is calling these methods. (see VBaseApp::AppInit for more details).
///
/// \sa VBaseApp::AppInit
/// \sa VAppModule
/// \ingroup VisionAppFramework
class VAppImpl
{
public:
  

	VAppImpl();
	virtual ~VAppImpl();

  /// \brief
  ///   Configures video config.
  ///
  /// \param config
  ///   VisAppConfig_cl structure initialized with default values.
  ///
  /// Simply overrides this method in your derived application class and modify the given
  /// VVideoConfig instance to your needs.
  ///
  /// \sa VisAppConfig_cl
  virtual void SetupAppConfig(VisAppConfig_cl& config) { config.m_videoConfig.m_bWaitVRetrace = true; }

  /// \brief
  ///   Returns engine initialization flags.
  ///
  /// \return
  ///   Engine init flags (default VAPP_INIT_DEFAULTS)
  virtual unsigned int SetupEngineInitFlags();

  /// \brief
  ///   Load plugins.
  ///
  /// Used to load plugins at the beginning of the application's lifetime cycle.
  /// In contrast to VAppImpl::LoadPlugins, this version is only triggered once
  /// before the engine gets initialized.
  virtual void PreloadPlugins() {}

  /// \brief
  ///   Gets called when engine initialization fails.
  virtual void InitFailed() {};

  /// \brief
  ///   Gets called when the engine has successfully initialized.
  ///
  /// The default implementation of this method registers all VAppModules such as the
  /// loading screen, the default menu, debug options, etc. by using RegisterAppModule(...).
  /// When overriding this method ensure to call the base implementation, otherwise no
  /// modules will be in place expect the VExitHandler which is always added.
  /// Another option is to NOT call the base implementation and setup all the VAppModules by
  /// yourself and/or registering your own modules derived from VAppModule.
  virtual void AfterEngineInit();

  /// \brief
  ///   Application initialization.
  ///
  /// This is the best spot to load the initial scene and to also setup/init all
  /// the application stuff that should be persistent over the entire lifetime of
  /// your application.
  virtual void Init() {}

  /// \brief
  ///   Called after loading of a scene has completed.
  ///
  /// All scene specific setup stuff like camera setup, additional entity creation, etc. should
  /// be done in here.
  ///
  /// \param bLoadingSuccessful
  ///   Flags whether loading was successful or not.
  virtual void AfterSceneLoaded(bool bLoadingSuccessful) {}

  /// \brief
  ///   Main Application Run method which is called after the scene has been rendered.
  ///
  /// \return
  ///   False when the application should exit, True otherwise.
  virtual bool Run();

  /// \brief
  ///   Counterpart to Init().
  ///
  /// Everything that has been initialized/created in Init() should be cleaned/destroyed in here.
  ///
  /// \sa VAppImpl::Init
  virtual void DeInit() {}

  /// \brief
  ///   Gets called when the engine has been de-initialized.
  virtual void EngineDeInit() {}

  /// \brief
  ///   Returns the application wide input map.
  ///
  /// \return
  ///   Input map.
  static VInputMap* GetInputMap() { return s_pInputMap; }

  /// \brief
  ///   Return a reference to the scene loader which is responsible for vscene loading.
  ///
  /// The scene loader is mainly exposed to enable the VAppBase to transparently synchronize
  /// the applications state with the scene loader.
  ///
  /// \return
  ///   Reference to the scene loader.
  VSceneLoader& GetSceneLoader() { return *m_pSceneLoader; }

  /// \brief
  ///   Setup scene environment depending on given settings.
  ///
  /// Scene setup includes setting data directories, processing the manifest file, and
  /// loading plugins.
  ///
  /// \param sceneSettings
  ///   Settings describing the required setup.
  void SetupScene(const VisAppLoadSettings& sceneSettings);

  /// \brief
  ///   Loads a scene with the given settings.
  ///
  /// \param sceneSettings
  ///   Settings describing the required setup including a valid scene name.
  ///
  /// \return
  ///   True when loading was successful, false otherwise.
  bool LoadScene(const VisAppLoadSettings& sceneSettings);

  /// \brief
  ///   Register an application module which implements the VAppModule interface.
  ///
  /// \param pModule
  ///   Instance of the application module which should be registered.
  ///
  /// \sa VAppModule
  void RegisterAppModule(VAppModule* pModule);

  /// \brief
  ///   De-register application module.
  ///
  /// \param pModule
  ///   Instance of the application module which should be de-registered.
  void DeRegisterAppModule(VAppModule* pModule);

  /// \brief
  ///   Returns the first registered module of the given type.
  ///
  /// \example
  ///   \code
  ///   VAppMenu* pMainMenu = GetAppModule<VAppMenu>();
  ///   if (pMainMenu != NULL)
  ///   {
  ///     // ...
  ///   }
  ///   \endcode
  ///
  /// \return
  ///   Application module with the given type, or NULL if not found.
  template<typename T>
  inline T* GetAppModule() const
  {
    const int iCount = m_appModules.Count();
    for (int i=0; i<iCount; ++i)
    {
      T* pAppModule = vdynamic_cast<T*>(m_appModules.GetAt(i));
      if (pAppModule != NULL)
        return pAppModule;
    }

    return NULL;
  }

  /// \brief
  ///   Returns the all registered module of the given type.
  ///
  /// \example
  ///   \code
  ///   VArray<VAppMenu*> modules;
  ///   GetAppModules<VAppMenu>(modules);
  ///   \endcode
  ///
  /// \param modules
  ///   Array which is filled with all module instances of the given type.
  template<typename T>
  inline void GetAppModules(VArray<T*>& modules) const
  {
    modules.RemoveAll();

    const int iCount = m_appModules.Count();
    for (int i=0; i<iCount; ++i)
    {
      T* pAppModule = vdynamic_cast<T*>(m_appModules.GetAt(i));
      if (pAppModule != NULL)
        modules.Append(pAppModule);
    }
  }

  /// \brief
  ///   Returns the application wide GUI context.
  const VAppMenuContextPtr GetContext() const { return m_spContext; }

protected:

  VSceneLoader* m_pSceneLoader;
  VAppMenuContextPtr m_spContext;

private:
  static VInputMap* s_pInputMap;
  VRefCountedCollection<VAppModule> m_appModules;
};

#endif //__V_APP_IMPL_HPP

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
