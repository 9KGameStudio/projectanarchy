/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef __V_APP_BASE_WIN_HPP
#define __V_APP_BASE_WIN_HPP

#include <Vision/Runtime/Framework/VisionApp/VAppBase.hpp>

/// \brief
///   Windows specific application behavior.
///
/// \ingroup VisionAppFramework
class VAppWin : public VAppBase
{
public:
  VAPP_IMPEXP VAppWin(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
  VAPP_IMPEXP virtual ~VAppWin();

  VAPP_IMPEXP virtual void PlatformInit() HKV_OVERRIDE;
  VAPP_IMPEXP virtual void PlatformInitSettings() HKV_OVERRIDE;
  VAPP_IMPEXP virtual void PlatformMapInput() HKV_OVERRIDE;

  VAPP_IMPEXP virtual void UpdateApplicationState() HKV_OVERRIDE;
  VAPP_IMPEXP virtual void SetupPlatformRootFileSystem() HKV_OVERRIDE;
  VAPP_IMPEXP virtual VString GetApplicationDataDirectory() HKV_OVERRIDE;
  VAPP_IMPEXP virtual VString GetPlatformStorageDirectory() HKV_OVERRIDE;
  VAPP_IMPEXP virtual VString GetPlatformCacheDirectory() HKV_OVERRIDE;

  VAPP_IMPEXP virtual unsigned int GetNumCommandLineArguments() const HKV_OVERRIDE;
  VAPP_IMPEXP virtual const char* GetCommandLineArgument(unsigned int index) const HKV_OVERRIDE;

  VAPP_IMPEXP virtual VAppHelper::VPlatformThreadingModel GetThreadingModel() HKV_OVERRIDE;

protected:
  VAPP_IMPEXP bool CheckFullscreenResolution(int iAdapter, int desiredX, int desiredY, int* selectedX, int* selectedY);

private:
  void RetrieveCommandLine();

private:
  VString m_sInitialWorkingDirectory;
  VStrList m_commandLineArguments;
};

#endif //__V_APP_BASE_WIN_HPP

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
