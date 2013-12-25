/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef __V_APP_BASE_IOS_HPP
#define __V_APP_BASE_IOS_HPP

#include <Vision/Runtime/Framework/VisionApp/Helper/VAppMobile.hpp>

extern "C" bool VisionInitFunction();
extern "C" bool VisionRunFunction();
extern "C" bool VisionDeInitFunction();

/// \brief
///   iOS specific application behavior.
///
/// \ingroup VisionAppFramework
class VAppIOS : public VAppMobile
{
  friend bool VisionInitFunction();
  friend bool VisionRunFunction();
  friend bool VisionDeInitFunction();

public:
	VAppIOS(const char* szAppDirectory, const char* szOutputDirectory, const char* szCacheDirectory);
	virtual ~VAppIOS();

  virtual void PlatformInitSettings() HKV_OVERRIDE;

  virtual bool IsCallbackBased() const HKV_OVERRIDE { return true; }

  virtual void SetupPlatformRootFileSystem() HKV_OVERRIDE;
  virtual VString GetApplicationDataDirectory() HKV_OVERRIDE;
  virtual VString GetPlatformStorageDirectory() HKV_OVERRIDE;
  virtual VString GetPlatformCacheDirectory() HKV_OVERRIDE;

  void UpdateApplicationState();

private:
  static int GetStartupModuleCount();
  static bool ProcessStartupModule(int iIndex);

  VString m_sAppDirectory;
  VString m_sOutputDirectory;
  VString m_sCacheDirectory;

  static VAppIOS* s_pAppInstance;
};

#endif //__V_APP_BASE_IOS_HPP

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
