/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef V_DISK_FILE_SYSTEM_HPP_INCLUDED
#define V_DISK_FILE_SYSTEM_HPP_INCLUDED

#include <Vision/Runtime/Base/System/IO/System/VFileAccessManager.hpp>
#include <Vision/Runtime/Base/System/IO/Stream/VDiskFileStream.hpp>

/// \brief
///   Implementation of IVFileSystem for accessing data using the current platform's
///   file API.
class VDiskFileSystem : public IVFileSystem
{
  friend class VDiskFileInStream;
  friend class VDiskFileOutStream;

private:
  enum Constants
  {
    PoolSizeInStream  = 4,
    PoolSizeOutStream = 1
  };

public:
  /// \brief
  //    Constructor; initializes the file system instance.
  /// \param szBasePath
  ///   The base path below which this file system can access files. This path must be
  ///   in a notation suitable for the platform's native file functions.
  /// \param bWritable
  ///   If possible, make this file system writable. Based on the writability of the
  ///   underlying file system, platform-specific implementations may choose to make this
  ///   file system read-only, even if \c bWritable is \c true.
  VBASE_IMPEXP VDiskFileSystem(const char* szBasePath, bool bWritable = false);
private:
  VDiskFileSystem(const VDiskFileSystem&);
  VDiskFileSystem& operator=(const VDiskFileSystem&);
public:
  VBASE_IMPEXP ~VDiskFileSystem();

public:
  virtual IVFileInStream* Open(const char* szFileName, unsigned int uiOpenFlags) HKV_OVERRIDE;
  virtual IVFileOutStream* Create(const char* szFileName, unsigned int uiCreateFlags) HKV_OVERRIDE;
  virtual VFileRemoveResult::Value Remove(const char* szFileName, unsigned int uiRemoveFlags) HKV_OVERRIDE;
  virtual hkvResult ResolveAbsolutePath(const char* szPath, VPathLookupContext& context, VStaticString<FS_MAX_PATH>& sNativePathOut) HKV_OVERRIDE;
  virtual hkvResult ResolveNativePath(const char* szPath, VFileSystemAccessMode::Value eAccessMode, VFileSystemElementType::Value eElementType, VStaticString<FS_MAX_PATH>& sAbsolutePathFragmentOut) HKV_OVERRIDE;
  virtual hkvResult GetFileTimeStamp(const char *szFileName, VDateTime& destTime) HKV_OVERRIDE;
  virtual bool IsWritable() const HKV_OVERRIDE;

  virtual void SetLookupCachingEnabled(bool bValue) HKV_OVERRIDE;
  virtual bool IsLookupCachingEnabled() const HKV_OVERRIDE;
  virtual void ResetLookupCache() HKV_OVERRIDE;

private:
  bool BuildNativePath(const char* szRelativePath, VStaticString<FS_MAX_PATH>& sNativePathOut) const;
  VDiskFileInStream *CreateNewInStream(bool enableCaching = true);
  VDiskFileOutStream *CreateNewOutStream();
  void CloseFile(VDiskFileInStream *pInStream);
  void CloseFile(VDiskFileOutStream *pOutStream);

  void DumpOpenStreams();

  void BuildDirectory();
  bool TextExistsLookup(const char *szFilename, bool bAddIfMissing);
  void RemoveLookupEntry(const char *szFilename);

private:
  VMutex m_IOMutex;

  VString m_sBasePath;
  bool m_bReadOnly;

  int m_iInPoolUsedMask;
  int m_iOutPoolUsedMask;
  VDiskFileInStream m_InPool[PoolSizeInStream];
  VDiskFileOutStream m_OutPool[PoolSizeOutStream];

  int m_iDirectoryFileCount;
  DynArray_cl<__int64> m_FilesInDirectory;
  bool m_bEnableDirectoryCaching;
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
