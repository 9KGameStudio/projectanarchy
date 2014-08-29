/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef V_PACKAGE_FILE_SYSTEM_HPP_INCLUDED
#define V_PACKAGE_FILE_SYSTEM_HPP_INCLUDED

class VPackagedFileInStream;

/// \brief Class that encapsulates a complete package file.
class VPackage
{
public:
  /// \brief Constructor.
  VPackage () {}

  /// \brief File Version of a .vArc file
  enum VVersion_e
  {
    VPV_Version_1 = 1,
    VPV_Current = VPV_Version_1
  };

  /// \brief Represents one file in the archive
  struct VPackageFile
  {
    /// \brief The filename of the file in the archive (including path)
    const char* m_szFilename;
    /// \brief The uncompressed size of the file. Identical to m_uiFileSizeOnDisk, if the file is stored uncompressed.
    unsigned int m_uiFileSizeUncompressed;
    /// \brief The compressed size of the file as stored on disk. Identical to m_uiFileSizeUncompressed, if the file is stored uncompressed.
    unsigned int m_uiFileSizeOnDisk;
    /// \brief The position of the (compressed) file in the archive. Seek to this position to start reading from the file.
    unsigned int m_uiFileSeekPosition;
  };

public:
  /// \brief Opens the archive and reads all the important information for bookkeeping.
  bool ReadArchive (const char* szFile);

  /// \brief Checks at which index the file exists in the archive. Returns -1 if it does not exist at all.
  int FindFile (const char* szFile);

  /// \brief Returns the information about the file with the given index.
  const VPackageFile* GetPackagedFile (unsigned int uiFile) const { return &m_Files[uiFile]; }

private:
  /// \brief Deactivated copy constructor.
  VPackage (const VPackage& cc);
  /// \brief Deactivated assignment operator.
  void operator= (const VPackage& rhs);

  /// \brief Compares two VPackageFileIndex CRC values for equality. Used for sorting and searching.
  static int CRCCompare (const void* p1, const void* p2);

  /// \brief struct used for sorting and searching files by CRC values.
  struct VPackageFileIndex
  {
    unsigned int m_uiFileNameCRC1;
    unsigned int m_uiFileNameCRC2;
    unsigned int m_uiFileIndex;
  };

  /// \brief The VPackageFileIndex for every file in the archive. Sorted by CRCs for binary searches.
  VArray<VPackageFileIndex> m_FileIndices;
  /// \brief The information about every file in the archive.
  VArray<VPackageFile> m_Files;
  /// \brief One big array for all filename strings. The VPackageFile objects just point into this array.
  VArray<char> m_FileNameStrings;
};


/// \brief
///   Implementation of IVFileSystem for accessing data in Vision package (*.varc) files.
class VPackageFileSystem : public IVFileSystem
{
  friend class VPackagedFileInStream;

public:
  /// \brief
  ///   Initializes a new VPackageFileSystem.
  /// \param szPackageFile
  ///   the package file to operate on. This should be a path usable with native file functions
  ///   of the target platform. It is possible to limit access to a path within the package; such
  ///   a path can be appended to the package file name, separated with a question mark.
  VBASE_IMPEXP VPackageFileSystem(const char* szPackageFile);
private:
  VPackageFileSystem(const VPackageFileSystem&);
  VPackageFileSystem& operator=(const VPackageFileSystem&);
public:
  VBASE_IMPEXP ~VPackageFileSystem();

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
  void ResetDirectoryManager();
  void CloseFile(VPackagedFileInStream* pFile);

private:
  VStaticString<FS_MAX_PATH> m_LastExistsCheck;
  int m_iLastExistsCheckResult;

  VMutex m_IOMutex;
  VPackage m_Package;

  int m_iArchiveSubDirectoryStringLength;
  VStaticString<FS_MAX_PATH> m_sArchiveSubDirectory;

  VArray<VPackagedFileInStream*> m_pArchiveFilePool;

  VString m_sPackageFile;
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
