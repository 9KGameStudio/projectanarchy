/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/Framework/VisionApp/iOS/VAppIOS.hpp>
#include <Vision/Runtime/Framework/VisionApp/VAppImpl.hpp>

#include <Vision/Runtime/Base/System/IO/FileSystems/VFileServeDaemon.hpp>

#include <Vision/Runtime/Common/iOS/VisAppSettings.h>

VAppIOS* VAppIOS::s_pAppInstance = NULL;

extern VAppImpl* CreateInstance();

extern "C" bool VisionInitFunction()
{
  VASSERT(VAppIOS::s_pAppInstance == NULL);
  VAppIOS* pApp = new VAppIOS(g_VisionAppSettings.szAppDirectory, g_VisionAppSettings.szDocumentsDirectory, g_VisionAppSettings.szCachesDirectory);
  VAppIOS::s_pAppInstance = pApp;

  pApp->PlatformInit();
  pApp->Execute(CreateInstance());

  if ((VAppIOS::s_pStartupModules != NULL) && (VAppIOS::s_pStartupModules->GetSize() > 0))
  {
    VAppIOS::s_pStartupModules->GetAt(0).m_pModule->Init();
    return true;
  }
  else
  {
    return pApp->AppInit();
  }
}

extern "C" bool VisionRunFunction()
{
  VASSERT(VAppIOS::s_pAppInstance != NULL);
  if ((VAppIOS::s_pStartupModules != NULL) && (VAppIOS::s_pStartupModules->GetSize() > 0))
  {
    if (VAppIOS::ProcessStartupModule(0))
      return true;

    delete VAppIOS::s_pStartupModules->GetAt(0).m_pModule;
    VAppIOS::s_pStartupModules->RemoveAt(0);
    
    
    if(static_cast<VAppIOS*>(VAppBase::Get())->WantsToQuit())
    {
      V_SAFE_DELETE(static_cast<VAppIOS*>(VAppBase::Get())->m_pAppImpl);
      return false;
    }
    
    if (VAppIOS::s_pStartupModules->GetSize() > 0)
      VAppIOS::s_pStartupModules->GetAt(0).m_pModule->Init();
    else
      VAppIOS::s_pAppInstance->AppInit();

    return true;
  }
  else
  {
    return VAppIOS::s_pAppInstance->AppRun();
  }
}

extern "C" bool VisionDeInitFunction()
{
  VASSERT(VAppIOS::s_pAppInstance != NULL);
  return VAppIOS::s_pAppInstance->AppDeInit();
}

VAppIOS::VAppIOS(const char* szAppDirectory, const char* szOutputDirectory, const char* szCacheDirectory)
  : VAppMobile()
  , m_sAppDirectory(szAppDirectory)
  , m_sOutputDirectory(szOutputDirectory)
  , m_sCacheDirectory(szCacheDirectory)
{
  s_pAppInstance = this;
}

VAppIOS::~VAppIOS()
{
}

void VAppIOS::PlatformInitSettings()
{
#if !defined(FORCE_TRILINEAR_FILTERING)
  Vision::Renderer.SetDefaultTextureFilterMode(FILTER_MIN_MAG_LINEAR_MIP_POINT);
#endif

  VAppMobile::PlatformInitSettings();
}

void VAppIOS::SetupPlatformRootFileSystem()
{
  VAppMobile::SetupPlatformRootFileSystem();

  const VString& sRoot = m_appConfig.m_sFileSystemRootName;
  if(VFileServeDaemon::IsInitialized())
    VFileAccessManager::GetInstance()->SetRoot(sRoot, VFileServeDaemon::GetInstance()->CreateFileSystem(sRoot, m_sAppDirectory + "/"));
  else
    VFileAccessManager::GetInstance()->SetRoot(sRoot, m_sAppDirectory + "/");

  VFileAccessManager::GetInstance()->SetRoot("app_data", GetApplicationDataDirectory(), VFileSystemFlags::WRITABLE);
}


VString VAppIOS::GetApplicationDataDirectory()
{
  return m_sOutputDirectory;
}

VString VAppIOS::GetPlatformStorageDirectory()
{
  return m_sOutputDirectory;
}

VString VAppIOS::GetPlatformCacheDirectory()
{
  return m_sCacheDirectory;
}


void VAppIOS::UpdateApplicationState()
{
  VAppMobile::UpdateApplicationState();

  // Refresh screen when loading
  if (GetAppState() == VAppHelper::AS_SCENE_LOADING)
  {
    Vision::Video.UpdateScreen();
  }
}

int VAppIOS::GetStartupModuleCount()
{
  return s_pStartupModules->GetSize();
}

bool VAppIOS::ProcessStartupModule(int iIndex)
{
  VASSERT(iIndex < GetStartupModuleCount());
  VStartupModule* pModule = s_pStartupModules->GetAt(iIndex).m_pModule;
  bool bResult = pModule->Run();

  if(static_cast<VAppIOS*>(VAppBase::Get())->WantsToQuit() || !bResult)
  {
    pModule->DeInit();
    return false;
  }
  
  return true;
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
