/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef V_ZIP_FILE_SYSTEM_HPP_INCLUDED
#define V_ZIP_FILE_SYSTEM_HPP_INCLUDED

#include <Vision/Runtime/Base/System/IO/System/VFileAccessManager.hpp>


/// \brief
///   Stores information about a zip file, i.e. one entry in the zip directory
///
/// In order to create an instance of VZipFileInfo, use VZipFileSystem::GetZipFileEntry.
class VZipFileInfo
{
public:
  VZipFileInfo();

  const char *m_szFilename; // points to a name in a name block

  ULONG m_iFilePos; ///< file start
  ULONG m_iFileNum; ///< file number in zip
  ULONG m_iSize;
};



/// \brief
///   Implements a file stream to read from zip archives.
class VZipFileInStream : public IVFileInStream
{
  friend class VZipFileSystem;

public:
  VZipFileInStream(IVFileSystem* pFileSystem = NULL);
private:
  VZipFileInStream(const VZipFileInStream&);
  VZipFileInStream& operator=(const VZipFileInStream&);
public:
  virtual ~VZipFileInStream();

  /// \brief 
  ///   Deletes all internal data.
  void Cleanup();

  //implement IVFileInStream
  VBASE_IMPEXP virtual void Close() HKV_OVERRIDE;
  VBASE_IMPEXP virtual size_t Read(void* pBuffer,int iLen) HKV_OVERRIDE;
  VBASE_IMPEXP virtual BOOL SetPos(LONG iPos, int iMode) HKV_OVERRIDE;
  VBASE_IMPEXP virtual LONG GetPos() HKV_OVERRIDE;
  VBASE_IMPEXP virtual LONG GetSize() HKV_OVERRIDE;
  VBASE_IMPEXP virtual const char* GetFileName() HKV_OVERRIDE;

  /// \brief
  ///   Sets the parent file system.
  inline void SetFileSystem(IVFileSystem *pFileSystem) { m_spFileSystem = pFileSystem; }

  /// \brief 
  ///   Opens the given file in the zip archive.
  bool Open(const char *pszFileName, VZipFileInfo& info);

private:
  void *m_pZipFile;
  ULONG m_iSize;
  int m_iPoolIndex;
  VStaticString<FS_MAX_PATH> m_sFilename;
};


/// \brief
///   IVFileSystem implementation for accessing files within a .zip archive.
class VZipFileSystem : public IVFileSystem
{
  friend class VZipFileInStream;

private:
  enum Constants
  {
    PoolSizeInStream  = 4
  };

public:
  /// \brief
  ///   Initializes a new VZipFileSystem.
  /// \param szZipFile
  ///   the package file to operate on. This should be a path usable with native file functions
  ///   of the target platform. It is possible to limit access to a path within the archive; such
  ///   a path can be appended to the .zip file name, separated with a question mark.
  VBASE_IMPEXP VZipFileSystem(const char* szZipFile);
private:
  VZipFileSystem(const VZipFileSystem&);
  VZipFileSystem& operator=(const VZipFileSystem&);
public:
  virtual ~VZipFileSystem();

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

  /// \brief
  ///   Returns the name of the zip file that this file system operates on. This name does not include
  ///   the subfolder path within the archive.
  VBASE_IMPEXP const char* GetZipFileName() const;

  /// \brief
  ///   Returns the path of the subfolder within this file system's underlying zip file.
  VBASE_IMPEXP const char* GetZipSubFolder() const;

  /// \brief
  ///   Returns the number of files in the underlying zip file
  VBASE_IMPEXP unsigned int GetNumZipFileEntries() const;

private:
  bool BuildFileList(void *pZipFile);
  bool FindFile(const char* pszFilename, VZipFileInfo& info);

  VZipFileInStream *CreateInStream();
  void CloseFile(VZipFileInStream *pStream);

  void Cleanup();
  void DumpOpenStreams();

private:
  VMutex m_IOMutex;

  VMap<VString, VZipFileInfo> m_FileInfos;
  char *m_pNameBuffer;

  VString m_sCompletePath;
  VString m_sZipFileName;
  VString m_sZipSubfolderName;

  int m_iInPoolUsedMask;
  VZipFileInStream m_InPool[PoolSizeInStream];
};

#endif

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
