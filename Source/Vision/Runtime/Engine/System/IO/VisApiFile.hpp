/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiFile.hpp

#ifndef DEFINE_VISAPIFILE
#define DEFINE_VISAPIFILE



/// \brief
///   VisFile_cl class for file operations. Can be accessed via Vision::File
/// 
/// All engine data reading and writing is handled by the installed file manager.
/// 
/// Custom file managers can be installed; e.g., to load encrypted data files.
/// 
/// \sa IVFileInStream
/// \sa IVFileOutStream
class VisFile_cl
{
public:
  VisFile_cl();



  ///
  /// @name File Access Configuration Functions
  /// @{
  ///

  /// \brief
  ///   Adds a file system to be used by the engine for file operations.
  ///
  /// This function operates on the global VFileAccessManager.
  ///
  /// A special notation is required for paths that are located within an archive or a package (.zip, .v, 
  /// .vpk, .zip.sdat, .v.sdat, .vpk.sdat). In this case, the question mark character \c ? is used
  /// to separate the path to the package from the path within the package. 
  /// For example:
  /// \verbatim main.zip?baseData \endverbatim
  /// 
  /// \param szRootName
  ///   name of the root under which to add the file system
  /// \param szBasePath
  ///   base path of the file system to add. This can be an absolute path, or a path relative to the
  ///   current directory.
  /// \param uiFlags
  ///   flags controlling the detailed behavior of this function. See VFileSystemFlags.
  /// \return
  ///   whether adding the file system was successful
  /// \see VFileAccessManager::SetRoot
  VISION_APIFUNC bool AddFileSystem(const char* szRootName, const char *szBasePath, unsigned int uiFlags = 0);

  /// \brief
  ///   Adds a file system to be used by the engine for file operations.
  ///
  /// This function operates on the global VFileAccessManager.
  ///
  /// \param szRootName
  ///   name of the root under which to add the file system
  /// \param pFileSystem
  ///   the file system to add. May be NULL, in which case the call is equivalent to RemoveFileSystem.
  /// \param uiFlags
  ///   flags controlling the detailed behavior of this function. See VFileSystemFlags.
  /// \return
  ///   whether adding the file system was successful
  /// \see VFileAccessManager::SetRoot
  VISION_APIFUNC bool AddFileSystem(const char* szRootName, IVFileSystem* pFileSystem, unsigned int uiFlags = 0);

  /// \brief
  ///   Removes a file system root.
  ///
  /// This function operates on the global VFileAccessManager.
  ///
  /// \param szRootName
  ///   name of the file system root to remove
  VISION_APIFUNC void RemoveFileSystem(const char* szRootName);

  /// \brief
  ///   Removes all installed file systems from the global file access manager.
  ///
  /// Along with the file systems, all search paths are removed as well, since they are not
  /// useful without their underlying file system.
  VISION_APIFUNC void RemoveAllFileSystems();

  /// \brief
  ///   Adds a search path to be used by the engine for file operations.
  ///
  /// This function operates on the global VFileAccessManager.
  ///
  /// All search paths must be specified as absolute paths, starting with the a colon (:) followed
  /// by the root name of an installed file system, and optionally followed by a path below that
  /// root.
  ///
  /// A valid search path might look like this:
  /// \verbatim :havok_sdk/Data/Vision/Base \endverbatim
  ///
  /// \param szPath
  ///   the search path to add
  /// \param uiFlags
  ///   flags controlling the detailed behavior of this function. See VSearchPathFlags.
  /// \return
  ///   the index of the added or existing search path, or \c -1, if the search path did
  ///   not exist and could not be added
  /// \see VFileAccessManager::AddSearchPath
  VISION_APIFUNC int AddSearchPath(const char* szPath, unsigned int uiFlags = 0);

  /// \brief
  ///   Removes a specific search path.
  /// \param szPath
  ///   the path to remove. This path must specified in the same format as for AddSearchPath().
  VISION_APIFUNC void RemoveSearchPath(const char* szPath);

  /// \brief
  ///   Removes all search paths form the global file access manager.
  VISION_APIFUNC void RemoveAllSearchPaths();

  /// \brief
  ///   Returns the number of search paths registered with the global file access manager.
  VISION_APIFUNC unsigned int GetNumSearchPaths() const;

  /// \brief
  ///   Returns the base path of a search path.
  /// \param uiIndex
  ///   the index of the search path whose base path to get
  /// \return
  ///   the base path of the search path, or \c NULL, if \c uiIndex is not valid
  VISION_APIFUNC const char* GetSearchPath(unsigned int uiIndex) const;


  /// \brief
  ///   Helper function that takes a relative directory name and combines it with the current
  ///   directory (getcwd) to make an absolute path.
  /// 
  /// \param pszOutDir
  ///   buffer to store the result
  /// 
  /// \param pszRelDir
  ///   relative directory name
  /// 
  /// \note
  ///   If you call this after engine init then the current directory might  already have changed
  ///   and not be the application directory anymore!
  /// 
  /// \example
  ///   \code
  ///   char tmp[FS_MAX_PATH];
  ///   Vision::File.GetAbsoluteDirectory(tmp,"data");
  ///   Vision::File.AddDataDirectory(tmp);
  ///   \endcode
  VISION_APIFUNC void GetAbsoluteDirectory(char *pszOutDir, const char *pszRelDir);


  ///
  /// @}
  ///


  ///
  /// @name File Functions
  /// @{
  ///
  
  /// \brief
  ///   Open a file for input using the current engine file manager.
  /// 
  /// This function uses the default engine file manager. All engine internal file read 
  /// accesses use this function.
  /// 
  /// \param pszFileName
  ///   Name of the file to open (relative to one of the data directories)
  /// 
  /// \param pszDirectory
  ///   Optional directory that the filename is relative to. This is prepended to the filename.
  /// 
  /// \param iFlags
  ///   Optional flags for opening the file. See VFileOpenFlags.
  /// 
  /// \return
  ///   a file stream for reading the requested file, or \c NULL if the file could not be opened.
  /// 
  /// \example
  ///   \code
  ///   IVFileInStream* pIn = Vision::File.Open("myfile.txt", "data");
  ///   \endcode
  VISION_APIFUNC IVFileInStream* Open(const char* pszFileName, const char* pszDirectory = NULL, int iFlags = 0);

  /// \brief
  ///   Create a file for output using the current engine file manager.
  /// 
  /// This function uses the default engine file manager.
  /// 
  /// Before testing the data directories, this function tries to create a file via the manager
  /// installed with  SetOutputDirectory resp. SetOutputManager.
  /// 
  /// Streams cannot be created inside archives or packages files.
  /// 
  /// \param pszFileName
  ///   Name of the file to create (relative to the data directories)
  /// 
  /// \param pszDirectory
  ///   Optional directory that the filename is relative to. This is prepended to the filename.
  ///
  /// \param iFlags
  ///   Optional flags for creating the file. See VFileCreateFlags.
  /// 
  /// \return
  ///   a stream for writing to the created file, or \c NULL if the file could not be created.
  /// 
  /// \example
  ///   \code
  ///   IVFileOutStream* pOut = Vision::File.Create("myfile.txt", "data");
  ///   \endcode
  VISION_APIFUNC IVFileOutStream* Create(const char* pszFileName, const char* pszDirectory = NULL, int iFlags = 0);


  /// \brief
  ///   Returns whether a file exists.
  /// 
  /// This function uses the default file access manager.
  /// 
  /// \param pszFileName
  ///   Name of the file. This may be an absolute native path, an absolut path in
  ///   ':root/path' notation, or a relative path to be resolved agains the search paths.
  /// 
  /// \param pszDirectory
  ///   Optional directory that the filename is relative to. This is prepended to the filename.
  /// 
  /// \return
  ///   \c TRUE if the file exists; \c false if not.
  /// 
  /// \example
  ///   \code
  ///   if (Vision::File.Exists("myfile.txt", "data"))
  ///   \endcode
  VISION_APIFUNC BOOL Exists(const char* pszFileName, const char* pszDirectory = NULL);

  ///
  /// @}
  ///


  ///
  /// @name Pre-Caching
  /// @{
  ///

  /// \brief
  ///   Creates a loading background task that loads a file into memory.
  /// 
  /// The next time Vision::File.Open is called with a cached file, the file system returns a
  /// memory stream.
  ///
  /// \param szFilename
  ///   File to precache.
  ///
  /// \return
  ///   the loading task for loading the file into cache, or \c NULL on error.
  VISION_APIFUNC VLoadingTask* PrecacheFile(const char *szFilename);


  /// \brief
  ///   Creates a loading task that has the passed data already loaded into memory
  ///   for the specified file.
  ///
  /// \param szFilename
  ///   File to which the passed data should belong
  /// \param pData
  ///   Pointer to the file data
  /// \param iDataSize
  ///   Size of the file data
  ///
  /// \return
  ///   The loading task for the specified file. In case a loading task for this file 
  ///   already existed, the existing task is returned. Note that in this case, the task
  ///   may not have finished loading yet!
  VISION_APIFUNC VLoadingTask* PrecacheFileFromMemory(const char *szFilename,
    unsigned char *pData, int iDataSize);


  /// \brief
  ///   Returns a reference to the global background loading manager.
  VISION_APIFUNC VMemoryStreamManager& GetMemoryStreamManager();

  /// \brief
  ///   Sets whether to log warnings whenever an uncached file is loaded.
  ///
  /// This feature is useful for optimizing zone streaming. If enabled, a warning will be printed to 
  /// the log for every file that is opened but not cached through a memory stream.
  ///
  /// \param bStatus
  ///   \c true to print log warnings about uncached file accesses
  inline void SetWarnOnUncachedFiles(bool bStatus)
  {
    m_bWarnOnUncachedFiles = bStatus;
  }

  /// \brief
  ///   Returns whether warnings about uncached file accesses are logged.
  inline bool GetWarnOnUncachedFiles() const
  {
    return m_bWarnOnUncachedFiles;
  }

  ///
  /// @}
  ///


  /// \brief
  ///   Sets if absolute paths should be passed to the file manager or not
  ///
  /// \param bUseAbsolutePaths
  ///   true for using absolute paths, false otherwise
  inline void SetUseAbsolutePaths(bool bUseAbsolutePaths) 
  {
    m_bUseAbsolutePaths = bUseAbsolutePaths;
  }

  ///
  /// @name Asset Related
  /// @{
  ///

  /// \brief
  ///   Sets the asset profile to be used. This is usually done by the engine automatically.
  ///
  /// \param pszAssetProfile
  ///   the asset profile to be used, if NULL the default asset profile for the platform is used
  ///
  /// \param pszAssetLookUpTable
  ///   the name of the look up table to be used. If NULL, pszAssetProfile will be used instead. Setting a different value than pszAssetProfile is only done in vForge.
  VISION_APIFUNC void SetAssetProfile(const char* pszAssetProfile, const char* pszAssetLookUpTable = NULL);

  /// \brief
  ///   Gets the currently active asset profile
  VISION_APIFUNC const char* GetAssetProfile() const;

  /// \brief
  ///  Sets whether or not asset mappings should be logged.
  /// \param bLog
  ///   \c true if asset mappings should be logged; \c false if not
  /// \note
  ///   This functionality is currently not supported; setting this state has no effect.
  inline void SetLogAssetMappings(bool bLog)
  {
    m_bLogAssetMappings = bLog;
  }

  /// \brief
  ///  Returns whether asset mappings are being logged.
  /// \return
  ///   \c true if asset mappings should be logged; \c false if not
  /// \note
  ///   This functionality is currently not supported.
  inline bool GetLogAssetMappings() const
  {
    return m_bLogAssetMappings;
  }

  /// \brief
  ///   Reloads the asset lookup tables for either one specific, or for all search paths 
  ///   currently registered with Vision.
  /// \param szSearchPath
  ///   The search path for which to reload the asset lookup table. If the search path
  ///   is \c NULL, the asset lookup tables are reloaded for all search paths.
  VISION_APIFUNC void ReloadAssetLookupTables(const char* szSearchPath = NULL);

  ///
  /// @}
  ///


private:
  bool m_bWarnOnUncachedFiles;
  bool m_bUseAbsolutePaths;
  bool m_bLogAssetMappings;
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
