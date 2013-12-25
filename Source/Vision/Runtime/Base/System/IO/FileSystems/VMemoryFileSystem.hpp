/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef V_MEMORY_FILE_SYSTEM_HPP_INCLUDED
#define V_MEMORY_FILE_SYSTEM_HPP_INCLUDED

#include <Vision/Runtime/Base/System/IO/System/VFileAccessManager.hpp>

/// \brief
///   Implementation of IVFileSystem that stores all data written to it in memory.
class VMemoryFileSystem : public IVFileSystem
{
public:
  VBASE_IMPEXP VMemoryFileSystem();
private:
  VMemoryFileSystem(const VMemoryFileSystem&);
  VMemoryFileSystem& operator=(const VMemoryFileSystem&);
public:
  VBASE_IMPEXP ~VMemoryFileSystem();

public:
  VBASE_IMPEXP virtual IVFileInStream* Open(const char* szFileName, unsigned int uiOpenFlags) HKV_OVERRIDE;
  VBASE_IMPEXP virtual IVFileOutStream* Create(const char* szFileName, unsigned int uiCreateFlags) HKV_OVERRIDE;
  VBASE_IMPEXP virtual VFileRemoveResult::Value Remove(const char* szFileName, unsigned int uiRemoveFlags) HKV_OVERRIDE;
  VBASE_IMPEXP virtual hkvResult ResolveAbsolutePath(const char* szPath, VPathLookupContext& context, VStaticString<FS_MAX_PATH>& sNativePathOut) HKV_OVERRIDE;
  VBASE_IMPEXP virtual hkvResult ResolveNativePath(const char* szPath, VFileSystemAccessMode::Value eAccessMode, VFileSystemElementType::Value eElementType, VStaticString<FS_MAX_PATH>& sAbsolutePathFragmentOut) HKV_OVERRIDE;
  VBASE_IMPEXP virtual hkvResult GetFileTimeStamp(const char *szFileName, VDateTime& destTime) HKV_OVERRIDE;
  VBASE_IMPEXP virtual bool IsWritable() const HKV_OVERRIDE;

  VBASE_IMPEXP virtual void SetLookupCachingEnabled(bool bValue) HKV_OVERRIDE;
  VBASE_IMPEXP virtual bool IsLookupCachingEnabled() const HKV_OVERRIDE;
  VBASE_IMPEXP virtual void ResetLookupCache() HKV_OVERRIDE;

private:
  VMutex m_IOMutex;
  VMap<VString, VMemoryStream*> m_streams;
};

#endif

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
