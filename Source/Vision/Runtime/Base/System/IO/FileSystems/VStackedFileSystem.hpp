/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VBASE_VSTACKEDFILESYSTEM_HPP
#define VBASE_VSTACKEDFILESYSTEM_HPP

#include <Vision/Runtime/Base/System/IO/System/VFileAccessManager.hpp>

/// \brief This file system combines multiple file systems into a single one.
///
/// For each request a linear search is done through all file systems until one 
/// file system fulfills the request. File systems are queried in descending order; i.e., 
/// the most recently added file system is queried first.
class VStackedFileSystem : public IVFileSystem
{
public:
  VBASE_IMPEXP VStackedFileSystem();

  /// \brief returns the number of file systems stacked
  inline unsigned int GetNumFileSystems() const { return m_fileSystems.Count(); }

  /// \brief returns the file system at the given index
  inline IVFileSystem* GetFileSystem(unsigned int uiIndex) { return m_fileSystems.GetAt(uiIndex); }

  /// \brief adds a file system at the end of the list
  VBASE_IMPEXP void AddFileSystem(IVFileSystem* pFileSystem);

  /// \brief insert a file system at a certain position
  VBASE_IMPEXP void InsertFileSystem(IVFileSystem* pFileSystem, unsigned int uiInsertBefore);

  /// \brief removes the file system at the given index
  VBASE_IMPEXP inline void RemoveFileSystem(unsigned int uiIndex) { m_fileSystems.RemoveAt(uiIndex); }

  // IVFileSystem interface
  virtual IVFileInStream* Open(const char* szFileName, unsigned int uiOpenFlags) HKV_OVERRIDE;

  virtual IVFileOutStream* Create(const char* szFileName, unsigned int uiCreateFlags) HKV_OVERRIDE;

  virtual VFileRemoveResult::Value Remove(const char* szFileName, unsigned int uiRemoveFlags) HKV_OVERRIDE;

  virtual hkvResult ResolveRelativePath(const char* szPath, VPathLookupContext& context, VStaticString<FS_MAX_PATH>& sNativePathOut) HKV_OVERRIDE;

  virtual hkvResult ResolveAbsolutePath(const char* szPath, VPathLookupContext& context, VStaticString<FS_MAX_PATH>& sNativePathOut) HKV_OVERRIDE;

  virtual hkvResult ResolveNativePath(const char* szPath, VFileSystemAccessMode::Value eAccessMode, VFileSystemElementType::Value eElementType, VStaticString<FS_MAX_PATH>& sAbsolutePathFragmentOut) HKV_OVERRIDE;

  virtual hkvResult GetFileTimeStamp(const char *szFileName, VDateTime& destTime) HKV_OVERRIDE;

  virtual bool IsWritable() const HKV_OVERRIDE;

  virtual void SetLookupCachingEnabled(bool bValue) HKV_OVERRIDE;

  virtual bool IsLookupCachingEnabled() const HKV_OVERRIDE;

  virtual void ResetLookupCache() HKV_OVERRIDE;

private:
  VRefCountedCollection<IVFileSystem> m_fileSystems;
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
