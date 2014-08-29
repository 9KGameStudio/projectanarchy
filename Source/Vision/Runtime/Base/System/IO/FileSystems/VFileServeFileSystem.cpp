/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/Base/BasePCH.h>

#include <Vision/Runtime/Base/System/IO/FileSystems/VFileServeFileSystem.hpp>

#include <Vision/Runtime/Base/System/IO/FileSystems/VFileServeDaemon.hpp>

VFileServeFileSystem::VFileServeFileSystem(const char* szRootName)
{
  m_sRootName.Format(":%s", szRootName);
}

IVFileInStream* VFileServeFileSystem::Open(const char* szFileName, unsigned int uiOpenFlags)
{
  if(!VFileServeDaemon::IsInitialized())
  {
    return NULL;
  }

  VStaticString<FS_MAX_PATH> szAbsolutePath;
  VPathHelper::CombineDirAndFile(szAbsolutePath, m_sRootName, szFileName);

  if(VFileAccessManager::CanonicalizePath(szAbsolutePath) == HKV_FAILURE)
  {
    return NULL;
  }

  // Forward to the daemon
  return VFileServeDaemon::GetInstance()->Open(szAbsolutePath, uiOpenFlags);
}

IVFileOutStream* VFileServeFileSystem::Create( const char* szFileName, unsigned int uiCreateFlags)
{
  // Not supported
  return NULL;
}

VFileRemoveResult::Value VFileServeFileSystem::Remove( const char* szFileName, unsigned int uiRemoveFlags)
{
  // Not supported
  return VFileRemoveResult::FAILED;
}

hkvResult VFileServeFileSystem::ResolveRelativePath( const char* szPath, VPathLookupContext& context, VStaticString<FS_MAX_PATH>& sNativePathOut )
{
  if(!VFileServeDaemon::IsInitialized())
  {
    return HKV_FAILURE;
  }
  
  // Forward to the daemon
  return VFileServeDaemon::GetInstance()->ResolveRelative(szPath, context);
}

hkvResult VFileServeFileSystem::ResolveAbsolutePath( const char* szPath, VPathLookupContext& context, VStaticString<FS_MAX_PATH>& sNativePathOut )
{
  if(!VFileServeDaemon::IsInitialized())
  {
    return HKV_FAILURE;
  }

  VStaticString<FS_MAX_PATH> szAbsolutePath;
  VPathHelper::CombineDirAndFile(szAbsolutePath, m_sRootName, szPath);
  
  if(VFileAccessManager::CanonicalizePath(szAbsolutePath) == HKV_FAILURE)
  {
    return HKV_FAILURE;
  }

  // Forward to the daemon
  return VFileServeDaemon::GetInstance()->ResolveAbsolute(szAbsolutePath, context);
}

hkvResult VFileServeFileSystem::ResolveNativePath( const char* szPath, VFileSystemAccessMode::Value eAccessMode, VFileSystemElementType::Value eElementType, VStaticString<FS_MAX_PATH>& sAbsolutePathFragmentOut )
{
  // Not supported
  return HKV_FAILURE;
}

hkvResult VFileServeFileSystem::GetFileTimeStamp( const char *szFileName, VDateTime& destTime)
{
  if(!VFileServeDaemon::IsInitialized())
  {
    return HKV_FAILURE;
  }

  VStaticString<FS_MAX_PATH> szAbsolutePath;
  VPathHelper::CombineDirAndFile(szAbsolutePath, m_sRootName, szFileName);

  if(VFileAccessManager::CanonicalizePath(szAbsolutePath) == HKV_FAILURE)
  {
    return HKV_FAILURE;
  }

  // Forward to the daemon
  return VFileServeDaemon::GetInstance()->GetFileTimeStamp(szAbsolutePath, destTime);
}

bool VFileServeFileSystem::IsWritable() const
{
  return false;
}

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
