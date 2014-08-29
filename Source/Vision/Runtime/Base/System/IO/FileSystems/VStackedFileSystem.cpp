/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/Base/BasePCH.h>
#include <Vision/Runtime/Base/System/IO/FileSystems/VStackedFileSystem.hpp>

VStackedFileSystem::VStackedFileSystem()
{
}

void VStackedFileSystem::AddFileSystem(IVFileSystem* pFileSystem)
{ 
  m_fileSystems.Add(pFileSystem); 
}

void VStackedFileSystem::InsertFileSystem(IVFileSystem* pFileSystem, unsigned int uiInsertBefore)
{ 
  m_fileSystems.Insert(pFileSystem, uiInsertBefore); 
}

IVFileInStream* VStackedFileSystem::Open(const char* szFileName, unsigned int uiOpenFlags)
{
  IVFileInStream* pResult = NULL;
  for (int i = m_fileSystems.Count() - 1; i >= 0; --i)
  {
    pResult = m_fileSystems.GetAt(i)->Open(szFileName, uiOpenFlags);
    if(pResult)
      break;
  }
  return pResult;
}

IVFileOutStream* VStackedFileSystem::Create(const char* szFileName, unsigned int uiCreateFlags)
{
  IVFileOutStream* pResult = NULL;
  for (int i = m_fileSystems.Count() - 1; i >= 0; --i)
  {
    IVFileSystem* pFileSystem = m_fileSystems.GetAt(i);
    if(pFileSystem->IsWritable())
    {
      pResult = pFileSystem->Create(szFileName, uiCreateFlags);
      if(pResult)
        break;
    }
  }
  return pResult;
}

VFileRemoveResult::Value VStackedFileSystem::Remove(const char* szFileName, unsigned int uiRemoveFlags)
{
  bool bHaveFailure = false;
  bool bHaveDeletion = false;

  for (int i = m_fileSystems.Count() - 1; i >= 0; --i)
  {
    IVFileSystem* pFileSystem = m_fileSystems.GetAt(i);
    if (!pFileSystem->IsWritable())
    {
      continue;
    }

    switch (pFileSystem->Remove(szFileName, uiRemoveFlags))
    {
    case VFileRemoveResult::SUCCESS:
      bHaveDeletion = true;
      break;
    case VFileRemoveResult::FAILED:
      bHaveFailure = true;
      break;
    }
  }

  if (bHaveDeletion)
  {
    return VFileRemoveResult::SUCCESS;
  }
  else if (bHaveFailure)
  {
    return VFileRemoveResult::FAILED;
  }

  return VFileRemoveResult::NOT_FOUND;
}

hkvResult VStackedFileSystem::ResolveRelativePath(const char* szPath, VPathLookupContext& context, VStaticString<FS_MAX_PATH>& sNativePathOut)
{
  for (int i = m_fileSystems.Count() - 1; i >= 0; --i)
  {
    if (m_fileSystems.GetAt(i)->ResolveRelativePath(szPath, context, sNativePathOut) == HKV_SUCCESS)
      return HKV_SUCCESS;
  }
  return HKV_FAILURE;
}

hkvResult VStackedFileSystem::ResolveAbsolutePath(const char* szPath, VPathLookupContext& context, VStaticString<FS_MAX_PATH>& sNativePathOut)
{
  for (int i = m_fileSystems.Count() - 1; i >= 0; --i)
  {
    if (m_fileSystems.GetAt(i)->ResolveAbsolutePath(szPath, context, sNativePathOut) == HKV_SUCCESS)
      return HKV_SUCCESS;
  }
  return HKV_FAILURE;
}

hkvResult VStackedFileSystem::ResolveNativePath(const char* szPath, VFileSystemAccessMode::Value eAccessMode, VFileSystemElementType::Value eElementType, VStaticString<FS_MAX_PATH>& sAbsolutePathFragmentOut)
{
  for (int i = m_fileSystems.Count() - 1; i >= 0; --i)
  {
    if (m_fileSystems.GetAt(i)->ResolveNativePath(szPath, eAccessMode, eElementType, sAbsolutePathFragmentOut) == HKV_SUCCESS)
      return HKV_SUCCESS;
  }
  return HKV_FAILURE;
}

hkvResult VStackedFileSystem::GetFileTimeStamp(const char *szFileName, VDateTime& destTime)
{
  for (int i = m_fileSystems.Count() - 1; i >= 0; --i)
  {
    if (m_fileSystems.GetAt(i)->GetFileTimeStamp(szFileName, destTime) == HKV_SUCCESS)
      return HKV_SUCCESS;
  }
  return HKV_FAILURE;
}

bool VStackedFileSystem::IsWritable() const
{
  for (int i = m_fileSystems.Count() - 1; i >= 0; --i)
  {
    if (m_fileSystems.GetAt(i)->IsWritable())
      return true;
  }
  return false;
}

void VStackedFileSystem::SetLookupCachingEnabled(bool bValue)
{
  for (int i = m_fileSystems.Count() - 1; i >= 0; --i)
  {
    m_fileSystems.GetAt(i)->SetLookupCachingEnabled(bValue);
  }
}

bool VStackedFileSystem::IsLookupCachingEnabled() const
{
  for (int i = m_fileSystems.Count() - 1; i >= 0; --i)
  {
    if (m_fileSystems.GetAt(i)->IsLookupCachingEnabled())
      return true;
  }
  return false;
}

void VStackedFileSystem::ResetLookupCache() 
{
  for (int i = m_fileSystems.Count() - 1; i >= 0; --i)
  {
    m_fileSystems.GetAt(i)->ResetLookupCache();
  }
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
