/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef __V_APP_BASE_TIZEN_HPP
#define __V_APP_BASE_TIZEN_HPP

#include <Vision/Runtime/Framework/VisionApp/Helper/VAppMobile.hpp>

/// \brief
///   Tizen specific application behavior.
///
/// \ingroup VisionAppFramework
class VAppTizen : public VAppMobile
                , public Tizen::App::UiApp
                , public Tizen::Base::Runtime::ITimerEventListener
                , public Tizen::Ui::IOrientationEventListener
                , public Tizen::Ui::Controls::IFrameEventListener
{
public:
  static Tizen::App::UiApp* CreateInstance(void) { return new VAppTizen; }
  static void SetAppImpl(VAppImpl* pImpl) { s_pAppImpl = pImpl; }

	VAppTizen();
	virtual ~VAppTizen();

  virtual bool IsCallbackBased() const HKV_OVERRIDE { return true; }

  virtual void PlatformInitSettings() HKV_OVERRIDE;
	virtual void PlatformMapInput() HKV_OVERRIDE;

  virtual void UpdateApplicationState() HKV_OVERRIDE;
  
  virtual void SetupPlatformRootFileSystem() HKV_OVERRIDE;
  virtual VString GetApplicationDataDirectory() HKV_OVERRIDE;
  virtual VString GetPlatformStorageDirectory() HKV_OVERRIDE;
  virtual VString GetPlatformCacheDirectory() HKV_OVERRIDE;

  /// \brief
  /// Called when the app is initializing. (via UiApp)
  virtual bool OnAppInitializing(Tizen::App::AppRegistry& appRegistry) HKV_OVERRIDE;

  /// \brief
  /// Called when the app initialization is finished. (via UiApp)
  virtual bool OnAppInitialized(void) HKV_OVERRIDE; 

  /// \brief
  /// Called when the app is terminating. (via UiApp)
  virtual bool OnAppTerminating(Tizen::App::AppRegistry& appRegistry, bool forcedTermination = false) HKV_OVERRIDE;

  /// \brief
  /// Called when the UiApp's frame moves to the top of the screen. (from UiApp)
  virtual void OnForeground(void) HKV_OVERRIDE;

  /// \brief
  /// Called when this UiApp's frame is moved from top of the screen to the background. (from UiApp)
  virtual void OnBackground(void) HKV_OVERRIDE;

  /// \brief
  /// Called when the system memory is not sufficient to run the UiApp any further. (via UiApp)
  virtual void OnLowMemory(void) HKV_OVERRIDE;

  /// \brief
  /// Called when the battery level changes. (via UiApp)
  virtual void OnBatteryLevelChanged(Tizen::System::BatteryLevel batteryLevel) HKV_OVERRIDE;

  /// \brief
  /// Timer callback (called as frequent as possible). (from ITimerEventListener)
  virtual void OnTimerExpired(Tizen::Base::Runtime::Timer& timer) HKV_OVERRIDE;

  /// \brief
  /// Called when the device orientation changes. (from IOrientationEventListener)
  virtual void OnOrientationChanged(const Tizen::Ui::Control &source, Tizen::Ui::OrientationStatus orientationStatus) HKV_OVERRIDE;

  /// \brief
  /// Called when the main application frame gets activated. (from IFrameEventListener)
  virtual void OnFrameActivated(const Tizen::Ui::Controls::Frame &source) HKV_OVERRIDE;

  /// \brief
  /// Called when the main application frame gets terminated. (from IFrameEventListener)
  virtual void OnFrameTerminating(const Tizen::Ui::Controls::Frame &source) HKV_OVERRIDE;

  /// \brief
  /// The main draw call - calling the Vision run function.
  void Draw(void);

private:
  bool ProcessStartupModule(int iIndex);

  static VAppImpl* s_pAppImpl;

  VString m_sAppDirectory;
  VString m_sAppDataDirectory;
  VString m_sCacheDirectory;
  VString m_sSdCardDirectory;

  Tizen::Base::Runtime::Timer* m_pTimer;
};

#endif //__V_APP_BASE_TIZEN_HPP

/*
 * Havok SDK - Base file, BUILD(#20140328)
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
