/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef __V_APP_BASE_ANDROID_HPP
#define __V_APP_BASE_ANDROID_HPP

#include <Vision/Runtime/Framework/VisionApp/Helper/VAppMobile.hpp>

/// \brief
///   Android specific application behavior.
///
/// \ingroup VisionAppFramework
class VAppAndroid : public VAppMobile
{
public:
  VAppAndroid(struct android_app* state);
  virtual ~VAppAndroid();

  virtual void PlatformInit() HKV_OVERRIDE;
  virtual void PlatformInitSettings() HKV_OVERRIDE;
  virtual void PlatformMapInput() HKV_OVERRIDE;
  virtual bool PlatformRun() HKV_OVERRIDE;
  virtual void PlatformDeInit() HKV_OVERRIDE;

  virtual void UpdateApplicationState() HKV_OVERRIDE;
  virtual void SetupPlatformRootFileSystem() HKV_OVERRIDE;
  virtual VString GetApplicationDataDirectory() HKV_OVERRIDE;
  virtual VString GetPlatformStorageDirectory() HKV_OVERRIDE;
  virtual VString GetPlatformCacheDirectory() HKV_OVERRIDE;

private:
  VString m_sCacheDirectory;
  VString m_sApkDirectory;
  VString m_sAppDataDirectory;
  VString m_sSdCardDirectory;
};

#endif //__V_APP_BASE_ANDROID_HPP

/*
 * Havok SDK - Base file, BUILD(#20131218)
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2013
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
