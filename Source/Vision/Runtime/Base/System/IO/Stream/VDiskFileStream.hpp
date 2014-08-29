/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VDiskFileStream.hpp

#ifndef DC_VBASE_VDISKFILESTREAM_HPP
#define DC_VBASE_VDISKFILESTREAM_HPP

#include <Vision/Runtime/Base/System/Threading/SyncPrimitive/VMutex.hpp>
#include <Vision/Runtime/Base/System/IO/Asset/hkvAssetLookUpTable.hpp>


/// \brief
///   Stream that reads from a file on disk.
class VDiskFileInStream : public IVFileInStream
{
public:
  /// \brief 
  ///   VDiskFileInStream instances are created by VDiskFileSystem.
  VDiskFileInStream(IVFileSystem* pFileSystem = NULL)
  : IVFileInStream(pFileSystem)
  {
    m_sFilename[0] = 0;
    m_iPoolIndex = -1;
    m_iCacheEntry = -1;
    m_iReadCachePos = 0;
    m_iBytesInCache = 0;
    m_iFileSize = -1;
    m_iFilePos = 0;
  }


  /// \internal Called by VDiskFileSystem.
  bool Open(const char *pszFile, int iFlags = 0);

  void SetFileSystem(IVFileSystem* pFileSystem)
  {
    m_spFileSystem = pFileSystem;
  }

  /// \internal Called by VDiskFileSystem.
  inline void SetCacheEntry(int iEntry) { m_iCacheEntry = iEntry; }

  /// \internal Called by VDiskFileSystem.
  inline int GetCacheEntry() const { return m_iCacheEntry; }

  /// \internal Called by VDiskFileSystem.
  inline void InvalidateReadCache()
  {  
    m_iReadCachePos = 0;
    m_iBytesInCache = 0;
    m_iFilePos = 0;
  }


  // implement IVFileInStream

  VBASE_IMPEXP virtual size_t Read(void* pBuffer,int iLen) HKV_OVERRIDE;
  VBASE_IMPEXP virtual void Close() HKV_OVERRIDE;
  VBASE_IMPEXP virtual BOOL SetPos(LONG iPos, int iMode) HKV_OVERRIDE;
  VBASE_IMPEXP virtual LONG GetPos() HKV_OVERRIDE;
  VBASE_IMPEXP virtual LONG GetSize() HKV_OVERRIDE;
  VBASE_IMPEXP virtual const char* GetFileName() HKV_OVERRIDE;

  int m_iPoolIndex;
  VFileHandle m_handle;
  VStaticString<FS_MAX_PATH> m_sFilename;

  int m_iCacheEntry;

  long m_iReadCachePos;
  long m_iBytesInCache;
  long m_iFilePos;
  long m_iFileSize;
};

/// \brief
///   Stream that writes directly to a file
class VDiskFileOutStream : public IVFileOutStream
{
public:
  /// \brief 
  ///   VDiskFileOutStream instances are created by VDiskFileSystem.
  VDiskFileOutStream(IVFileSystem* pFileSystem = NULL)
  : IVFileOutStream(pFileSystem), m_iPoolIndex(-1)
  {
  }

  void SetFileSystem(IVFileSystem* pFileSystem)
  {
    m_spFileSystem = pFileSystem;
  }

  /// \internal Called by VDiskFileInStream.
  bool Create(const char *pszFile, int iFlags = 0);


  //Implement IVFileOutStream

  VBASE_IMPEXP virtual void Close() HKV_OVERRIDE;
  VBASE_IMPEXP virtual size_t Write(const void* pBuffer,size_t iLen) HKV_OVERRIDE;
  VBASE_IMPEXP virtual const char* GetFileName() HKV_OVERRIDE;
  VBASE_IMPEXP virtual void Flush() HKV_OVERRIDE;

  int m_iPoolIndex;
  VFileHandle m_handle;
  VStaticString<FS_MAX_PATH> m_sFilename;
};


/// \brief
///   Class for managing the read cache of the disk file stream managers.
class VDiskFileStreamCacheManager
{
public:
  enum Constants
  {
    NumReadCacheEntries = 4
  };

public:
  /// \brief
  ///   Sets the read cache size to be used for each opened file.
  ///
  /// Note that the specified size is the size of the cache for each opened file, so if two files are opened and
  /// being read from using a VDiskFileInStream, 2*iSize bytes are reserved for the cache. The memory will remain
  /// allocated until FreeMemory() is called.
  ///
  /// Up to four files can be cached at the same time. All further simultaneously opened files will be read from
  /// directly without going through the read cache.
  ///
  /// It is allowed to change this value at any time. Note, however, that read caches already allocated to currently open
  /// files will not be resized.
  ///
  /// By default, read caching is disabled. 
  ///
  /// \param iSize
  ///   Size of the cache per file. Setting 0 will disable read caching. The read cache size must be a multiple of 32 bytes.
  VBASE_IMPEXP static void SetReadCacheSize(int iSize);

  /// \brief
  ///   Returns the read cache size to be used for each opened file.
  VBASE_IMPEXP static int GetReadCacheSize();

  /// \brief
  ///   Frees all unused memory currently allocated to file read caches. Call this when loading has finished. Note
  ///   that this function will not free memory allocated to files which are still open. Furthermore, this function
  ///   is implicitly called by the destructor of VDiskFileStreamManager.
  VBASE_IMPEXP static void FreeUnusedEntries();

  /// \brief
  ///   Internal function. Do not call.
  static int AllocateEntry();

  /// \brief
  ///   Internal function. Do not call.
  static void FreeEntry(int iEntry);

  /// \brief
  ///   Internal function. Do not call.
  static void EnsureReadCacheAllocated(int iEntry);

  /// \brief
  ///   Internal function. Do not call.
  static void *GetCacheEntry(int iEntry, int &iSize);

private:
  static int s_iReadCacheSizePerFile;
  static unsigned int s_uiEntryInUse;

  static void *s_pReadCache[NumReadCacheEntries];
  static int s_iReadCacheSize[NumReadCacheEntries];

  static VMutex s_CacheMutex;
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
