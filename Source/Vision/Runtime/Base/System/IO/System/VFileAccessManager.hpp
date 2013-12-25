/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef V_FILE_ACCESS_MANAGER_HPP_INCLUDED
#define V_FILE_ACCESS_MANAGER_HPP_INCLUDED

#include <Vision/Runtime/Base/Container/VArray.hpp>
#include <Vision/Runtime/Base/Container/vstrlist.hpp>

#include <Vision/Runtime/Base/String/VString.hpp>

#include <Vision/Runtime/Base/System/VDateTime.hpp>
#include <Vision/Runtime/Base/System/Threading/SyncPrimitive/VMutex.hpp>
#include <Vision/Runtime/Base/System/VRefCounter.hpp>

#define V_PATH_ROOT_STR ":"
#define V_PATH_ROOT_CHAR ':'

#define V_ASSETMGMT_DATA_PATH "AssetMgmt_data"

// forward declarations
class IVFileInStream;
class IVFileOutStream;

class IVFileSystem;
class VFileAccessManager;

struct VPathLookupContext;

/// \brief Possible results for file removal
struct VFileRemoveResult
{
  /// \brief Enumeration of possible values
  enum Value
  {
    SUCCESS    = 0, ///< The file was successfully removed
    NOT_FOUND  = 1, ///< The specified file didn't exist
    FAILED     = 2, ///< An error occurred while removing the file, or file deletion is not suppported
  };
};

/// \brief File system access modes
struct VFileSystemAccessMode
{
  // IMPORTANT:
  // - When changing this enum, also change the "FileSystemAccessMode" enum in IEngineManager.cs accordingly.
  // - These values are serialized and send via the FileServe connection, so please make sure to keep any changes in sync
  //   with the corresponding enum in FileServe.
  /// \brief Enumeration of possible values
  enum Value
  {
    READ             = 0, ///< Read access
    READ_NO_REDIRECT = 1, ///< Read access without performing asset lookup
    WRITE            = 2, ///< Write access (file/directory must be resolvable on a writable file system/search path, but doesn't need to exist)
    REMOVE           = 3  ///< Delete access (file/directory must exist and must be writable)
  };
};

/// \brief File system element types (file or directory)
struct VFileSystemElementType
{
  // IMPORTANT: 
  // - When changing this enum, also change the "FileSystemElementType" enum in IEngineManager.cs accordingly.
  // - These values are serialized and send via the FileServe connection, so please make sure to keep any changes in sync
  //   with the corresponding enum in FileServe.
  /// \brief Enumeration of possible values
  enum Value
  {
    FILE      = 0, ///< File element
    DIRECTORY = 1  ///< Directory element
  };
};

/// \brief Flags that can be specified when adding file systems to a file access manager
struct VFileSystemFlags
{
  // IMPORTANT: When changing this enum, also change the "FileSystemFlags" enum in IEngineManager.cs accordingly.
  /// \brief Enumeration of possible values
  enum Values
  {
    WRITABLE              = V_BIT(0), ///< Try to make the file system added as a new root writable. Not all file systems support this.
    ADD_SEARCH_PATH       = V_BIT(1), ///< After successfully adding a file system, also add a search path to its root.
  };
};

/// \brief Flags that can be specified when adding search paths to a file access manager
struct VSearchPathFlags
{
  // IMPORTANT: When changing this enum, also change the "SearchPathFlags" enum in IEngineManager.cs accordingly.
  /// \brief Enumeration of possible values
  enum Values
  {
    PATH_MUST_EXIST = V_BIT(0), ///< Fail if the path to add doesn't exist in the underlying file system
    WRITABLE        = V_BIT(1), ///< Mark the added search path as writable
  };
};


/// \brief Flags for controlling how files are created or opened for writing
struct VFileCreateFlags
{
  /// \brief Enumeration of possible values
  enum Values
  {
    APPEND                = V_BIT(0),                           ///< Opens a file for writing, but keeps existing data and instead appends new data at its end
    SHARE_DENY_WRITE      = V_BIT(1),                           ///< Denies concurrent write access while the file is open. Currently only supported on Windows plattforms.
    SHARE_DENY_READ       = V_BIT(2),                           ///< Denies concurrent read access while the file is open. Currently only supported on Windows plattforms.
    SHARE_DENY_READWRITE  = SHARE_DENY_WRITE | SHARE_DENY_READ  ///< Denies concurrent read and write access while the file is open. Currently only supported on Windows plattforms.
  };
};


/// \brief Flags for controlling how files are opened for reading
struct VFileOpenFlags
{
  /// \brief Enumeration of possible values
  enum Values
  {
    NO_CACHE           = V_BIT(0),  ///< Disables the read cache for files. The read cache is only used if its size was specified via VDiskFileSystem::SetReadCacheSize, though.
    SHARE_ALLOW_WRITE  = V_BIT(1)   ///< Allow the file to be opened for writing by others. Only supported on Windows platforms.
  };
};

/// \brief Flags for controlling how files are removed
struct VFileRemoveFlags
{
  /// \brief Enumeration of possible values
  enum Values
  {
    FORCE = V_BIT(0)   ///< Also remove the file if it's read-only
  };
};


/// \brief
///   Interface for file system implementations used by VFileAccessManager.
///
/// A file system implementation allows access to files stored on local drives, within
/// packages or archives, or on a file server.
class IVFileSystem : public VRefCounter, public VTypedObject
{
public:
  /// \brief
  ///   Opens an existing file for reading.
  ///
  /// \param szFileName
  ///   name of the file to open. This must be a path within the underlying file system; i.e., a path
  ///   relative to whatever root the file system is based upon. Both slashes and backslashes are acceptable
  ///   as path separators.
  ///
  /// \param uiOpenFlags
  ///   flags determining how the file should be opened. See VFileAccessManager::OpenFlags.
  ///
  /// \return
  ///   a file stream for reading the file if successful; \c NULL otherwise
  virtual IVFileInStream* Open(const char* szFileName, unsigned int uiOpenFlags) = 0;

  /// \brief
  ///   Creates a new file or opens an existing file for writing. If the file already exists, the 
  ///   \c uiCreateFlags determine whether the file gets overwritten or appended to.
  /// \param szFileName
  ///   name of the file to open. This must be a path within the underlying file system; i.e., a path
  ///   relative to whatever root the file system is based upon. Both slashes and backslashes are acceptable
  ///   as path separators.
  /// \param uiCreateFlags
  ///   flags determining how the file should be created. See VFileAccessManager::CreateFlags.
  ///
  /// \return
  ///   a file stream for writing to the file if successful; \c NULL otherwise (e.g., if the file system is
  ///   read-only)
  virtual IVFileOutStream* Create(const char* szFileName, unsigned int uiCreateFlags) = 0;

  /// \brief
  ///   Removes a file.
  /// \param szFileName
  ///   name of the file to open. This must be a path within the underlying file system; i.e., a path
  ///   relative to whatever root the file system is based upon. Both slashes and backslashes are acceptable
  ///   as path separators.
  /// \param uiRemoveFlags
  ///   flags controlling the behavior of this method. See VFileAccessManager::RemoveFlags.
  ///
  /// \return
  ///   an enumeration value describing the outcome of the operation.
  virtual VFileRemoveResult::Value Remove(const char* szFileName, unsigned int uiRemoveFlags) = 0;

  /// \brief
  ///   Resolves the passed relative (file or directory) path; i.e., determines whether the path can be 
  ///   accessed with the desired mode, and, if applicable, determines the native path of the file or directory.
  ///
  /// When the VFileAccessManager needs to resolve a relative path, it calls this function for each search path
  /// it checks on the file system of the respective search path. The context contains a reference to the calling
  /// VFileAccessManager, a sequence number of the request, and the index of the search path to check.
  ///
  /// The default implementation uses this implementation to compose the absolute path to check, and calls
  /// ResolveAbsolutePath with this information.
  ///
  /// Optimizations, such as checking multiple candidate paths at once, can be realized by overriding
  /// this function.
  ///
  /// \param szPath
  ///   the path to resolve. This must be a relative path which may be resolvable against a search path. Both 
  ///   slashes and backslashes are acceptable as path separators.
  /// \param context
  ///   a context object describing the modalities and containing auxiliary data for the lookup.
  ///
  /// \param sNativePathOut
  ///   A buffer receiving the native resolved path.
  ///   If this function fails, the buffer contents are not
  ///   defined. If this function succeeds, the buffer is either empty (if the file system does not map to
  ///   native paths), or it contains the resolved native path.
  /// \return
  ///   whether the path could be successfully resolved
  virtual hkvResult ResolveRelativePath(const char* szPath, VPathLookupContext& context, VStaticString<FS_MAX_PATH>& sNativePathOut);

  /// \brief
  ///   Resolves the passed absolute (file or directory) path; i.e., determines whether the path can be 
  ///   accessed with the desired mode, and, if applicable, determines the native path of the file or directory.
  ///
  /// Please note that the absolute path passed into this function must not include the root part; only the
  /// part below the root should be included.
  ///
  /// \param szPath
  ///   the path to resolve. This must be a path within the underlying file system; i.e., a path
  ///   relative to whatever root the file system is based upon. Both slashes and backslashes are acceptable
  ///   as path separators.
  /// \param context
  ///   a context object describing the modalities and containing auxiliary data for the lookup.
  /// \param sNativePathOut
  ///   a buffer receiving the native resolved path. If this function fails, the buffer contents are not
  ///   defined. If this function succeeds, the buffer is either empty (if the file system does not map to
  ///   native paths), or it contains the resolved native path.
  /// \return
  ///   whether the path could be successfully resolved
  virtual hkvResult ResolveAbsolutePath(const char* szPath, VPathLookupContext& context, VStaticString<FS_MAX_PATH>& sNativePathOut) = 0;

  /// \brief
  ///   Resolves the passed native (file or directory) path to a path within this file system. This operation
  ///   is the opposite of ResolvePath, and is not supported on all types of file systems.
  /// \param szPath
  ///   the native path to resolve
  /// \param eAccessMode
  ///   the intended access mode of the path to be resolved. If \c AM_WRITE, the file or directory need not
  ///   exist for this function to succeed, but still, the file system must be writable.
  /// \param eElementType
  ///   the expected element type (file or directory) the path to resolve points to. This must be explicitly 
  ///   specified in order to catch name collisions between files and directories.
  /// \param sAbsolutePathFragmentOut
  ///   the path below the root of this file system \c szPath resolves to. If this function succeeds, the content
  ///   of this buffer is always valid; otherwise, the content is undefined.
  /// \return
  ///   whether the path could be successfully resolved
  virtual hkvResult ResolveNativePath(const char* szPath, VFileSystemAccessMode::Value eAccessMode, VFileSystemElementType::Value eElementType, VStaticString<FS_MAX_PATH>& sAbsolutePathFragmentOut) = 0;

  /// \brief
  ///   Queries the time stamp of a file.
  /// \param szFileName
  ///   name of the file to query the timestamp of. This must be a path within the underlying file system; i.e., a path
  ///   relative to whatever root the file system is based upon. Both slashes and backslashes are acceptable
  ///   as path separators.
  /// \param destTime
  ///   receives the file time stamp if this function succeeds.
  ///
  /// \return
  ///   whether the time stamp could be successfully queried
  virtual hkvResult GetFileTimeStamp(const char *szFileName, VDateTime& destTime) = 0;

  /// \brief
  ///   Returns whether this file system is writable.
  virtual bool IsWritable() const = 0;

  /// \brief
  ///   If supported by this file system, enables caching of information to allow for faster file look-ups.
  ///
  /// Not all file systems support this feature; others may even rely on it for internal operation, so
  /// it is always enabled and can't be switched off. Call IsLookupCachingEnabled to find out what the
  /// current state is.
  ///
  /// \param bValue
  ///   Whether look-up caching should be enabled.
  virtual void SetLookupCachingEnabled(bool bValue);

  /// \brief
  ///   Returns whether caching of file look-up information is enabled.
  virtual bool IsLookupCachingEnabled() const;

  /// \brief
  ///   If supported by this file system, resets the information for file look-up caching; i.e., 
  ///   rebuilds the look-up cache.
  virtual void ResetLookupCache();
};


/// \brief
///   Helper struct containing data relevant during asset lookup and path resolution.
struct VPathLookupContext
{
  /// \brief
  ///   Constructor; initializes this context struct.
  /// \param owner
  ///   the owner and creator of this instance
  VPathLookupContext(const VFileAccessManager& owner)
    : m_owner(owner), m_uiSearchPathIndex(0), m_assetInfo(NULL), m_szInitialSearchPath(NULL), 
    m_szPathToOpen(NULL), m_eAccessMode(VFileSystemAccessMode::READ), m_eElementType(VFileSystemElementType::FILE)
  {
    static int iNextSequenceNo = 0;
    m_iSequenceNo = VAtomic::Increment(iNextSequenceNo);
  }

  const VFileAccessManager& m_owner; ///< The owner and creator of this instance
  unsigned int m_iSequenceNo;        ///< Instance counter uniquely identifying this instance
  unsigned int m_uiSearchPathIndex;  ///< Index of the search path to be examined

  const VAssetInfo* m_assetInfo;     ///< Information about the asset if one has been resolved; \c NULL otherwise

  const char* m_szInitialSearchPath;        ///< The search path in which the file has been found (or, if an asset lookup has taken place, in which the asset was first looked up)
  const char* m_szPathToOpen;               ///< The path to actually open. If asset lookup was successful, this is the redirected asset path; else, the input path.

  VFileSystemAccessMode::Value m_eAccessMode;   ///< The mode in which the path to look up is going to be accessed
  VFileSystemElementType::Value m_eElementType; ///< The type of file system element to look up (file/directory)
};


/// \brief
///   Interface for file system factory implementations.
///
/// A file system factory creates a file system object suitable for accessing files under
/// a specified native path.
class IVFileSystemFactory: public VRefCounter
{
public:
  IVFileSystemFactory() {}
private:
  IVFileSystemFactory(const IVFileSystemFactory&);
  IVFileSystemFactory& operator=(const IVFileSystemFactory&);
public:
  virtual ~IVFileSystemFactory() {}

public:
  /// \brief
  ///   Creates a file system instance for the specified native path.
  /// \param szNativePath
  ///   the path for which to create the file system instance
  /// \param bWritable
  ///   whether the file system should be writable. Note that not all file system
  ///   implementations support writing, so this parameter is only a hint.
  /// \return
  ///   the file system implementation, or \c null, if this factory couldn't create a 
  ///   file system implementation for the specified path.
  virtual IVFileSystem* CreateFileSystem(const char* szNativePath, bool bWritable) = 0;
};


/// \brief
///   Represents a single search path.
///
/// Search paths are a layer above file systems within the VFileAccessManager. They are expressed as
/// platform independent paths, and provide the base paths against which the VFileAccessManager tries
/// to resolve relative paths. Each search paths may also be the root of an asset library, in which case
/// an asset look-up table will be loaded and asset redirections may be performed as part of the path
/// resolution process.
///
/// Instances of this class are only created by the VFileAccessManager.
class VSearchPath : public VRefCounter
{
  friend class VFileAccessManager;

private:
  /// \brief
  ///   Constructor; initializes a new VSearchPath.
  /// \param owner
  ///   the VFileAccessManager that owns this search path object
  /// \param szBasePath
  ///   the platform-independent, absolute base path
  /// \param bWritable
  ///   whether this search path allows writing. Note that if the search path is to be writable, 
  ///   the underlying file system must be writable as well.
  VSearchPath(VFileAccessManager& owner, const char* szBasePath, bool bWritable = false);
  VSearchPath(const VSearchPath&);
  VSearchPath& operator=(const VSearchPath&);
public:
  virtual ~VSearchPath();

public:
  /// \brief
  ///   Returns whether this search path is writable.
  inline bool IsWritable() const { return m_bWritable; }

  /// \brief
  ///   Initializes the asset library represented by this search path; i.e., clears the current asset
  ///   look-up table, and reloads it based on the current asset profile information.
  /// \return
  ///   whether the asset library was successfully initialized
  VBASE_IMPEXP hkvResult InitializeAssetLibrary();

  /// \brief
  ///   Looks up the specified asset in this search path's asset look-up table.
  /// \param pszAsset
  ///   the name of the asset to look up
  /// \param pszVariant
  ///   the name of the asset variant for which to look up the asset. May be \c NULL to look up the default
  ///   variant.
  /// \return
  ///   a pointer to an asset information struct if successful; \c NULL otherwise
  VBASE_IMPEXP const VAssetInfo* LookUpAsset(const char* pszAsset, const char* pszVariant);

  /// \brief
  ///   Returns the base path of this search path.
  inline const VString& GetBasePath() const { return m_sBasePath; }

  /// \brief
  ///   Returns the root part of the base path.
  inline const char* GetRootName() const { return m_sRootName.AsChar(); }

  /// \brief
  ///   Returns the part of the base path below the root component.
  inline const char* GetPathBelowRoot() const { return m_szPathBelowRoot; }

private:
  bool m_bWritable;
  VString m_sBasePath;
  VString m_sRootName;
  const char* m_szPathBelowRoot;
  VFileAccessManager& m_owner;

  /// The lookup table used to resolve assets to files that can be opened by the file managers.
  hkvAssetLookUpTable m_AssetLookUpTable;
};


class VFileAccessManager;
typedef VSmartPtr<VFileAccessManager> VFileAccessManagerPtr;

/// \brief 
///   Manages an infrastructure for file access within Vision.
///
/// Files can be accessed by three kinds of paths:
/// - Native paths: these paths follow the syntax of the native file-access functions of the platform
///   Vision is running on. Only paths that are absolute in terms of the native file system are
///   recognized as native paths.
///   Not all of the registered file systems may support native paths, meaning that these files can
///   only be accessed by relative or absolute paths.
/// - Absolute paths: these paths start with a colon and a root name, and point directly into
///   one of the file systems registered with a VFileAccessManager. Unlike relative paths,
///   absolute paths are unambiguous, but when accessing files through them, no asset look-up
///   or redirection takes place.
/// - Relative paths: these are all paths which are neither absolute nor native. Relative paths
///   are resolved against all search paths, and asset look-up (and possibly redirection) takes
///   place during this process.
///
/// The layer responsible for actual file access consists of the file systems (see IVFileSystem) 
/// registered with the VFileAccessManager. Each file system is registered under a uniquely named 
/// root, which also constitutes the first component of absolute paths.
///
/// Above the file systems, search paths may be added to provide base paths against which relative
/// paths can be resolved. Each search path can also represent an asset library, in which case it
/// may provide alternative file names based on the currently active asset profile and the asset variant
/// keys set at runtime.
///
/// When relative paths are resolved, search paths are checked back-to-front, i.e., the most recently
/// added search path is checked first.
class VFileAccessManager : public VRefCounter
{
  friend class IVFileSystem;

public:
  /// Constants used within the VFileAccessManager class
  enum Constants {
    MAX_ROOT_NAME_LENGTH = 64 ///< Maximum length of a root name for registered file systems
  };

public:
  /// \brief
  ///   Base structure for path conversion results
  struct PathResult
  {
    bool m_bLookedUpAsAsset; ///< An asset look-up was performed when converting the path
    bool m_bTargetIsShadowedByReadOnly; ///< A writable path was successfully determined, but when resolving the path for reading, a different path would be returned.

    PathResult()
    : m_bLookedUpAsAsset(false), m_bTargetIsShadowedByReadOnly(false)
    {
    }
  };

  /// \brief
  ///   Result structure for conversions to relative paths
  struct RelativePathResult : public PathResult
  {
    VStaticString<FS_MAX_PATH> m_sSearchPath; ///< The search path to which the resulting path is relative
    VStaticString<FS_MAX_PATH> m_sRelativePath; ///< The resulting relative path
  };

  /// \brief
  ///   Result structure for conversions to absolute paths
  struct AbsolutePathResult : public PathResult
  {
    VStaticString<FS_MAX_PATH> m_sInitialSearchPath; ///< The initial search path that succeeded in resolving the input path. Due to asset redirection, this may be a different path than the absolute path returned as result.
    VStaticString<FS_MAX_PATH> m_sAbsolutePath; ///< The resulting absolute path
  };

  /// \brief
  ///   Result structure for conversions to native paths
  struct NativePathResult : public PathResult
  {
    VStaticString<FS_MAX_PATH> m_sNativePath; ///< The resulting native path
  };


public:
  VBASE_IMPEXP VFileAccessManager();
  VBASE_IMPEXP virtual ~VFileAccessManager();
private:
  VFileAccessManager(const VFileAccessManager&);
  VFileAccessManager& operator=(const VFileAccessManager&);


public:
  /// \brief
  ///   Splits off the root part of an absolute path, and returns both the root part and the part
  ///   below the root.
  /// \param szPath
  ///   the path to split
  /// \param sRootName
  ///   buffer for receiving the root name
  /// \return
  ///   the part below the root, or \c NULL in case of an error. Note that a non-\c NULL, but empty string 
  ///   may be returned if \c szPath consists of only the root.
  VBASE_IMPEXP static const char* SplitOffRoot(const char* szPath, VStaticString<MAX_ROOT_NAME_LENGTH>& sRootName);

  /// \brief
  ///   Returns the global instance of the VFileAccessManager; asserts if the instance has not been created
  ///   by a call to Init() yet.
  VBASE_IMPEXP static VFileAccessManager* GetInstance();

  /// \brief
  ///   Creates the global VFileAccessManager instance.
  VBASE_IMPEXP static void Init();

  /// \brief
  ///   Destroys the global VFileAccessManager instance. Note that calls to Init() and DeInit() are not
  ///   reference counted and thus need not be balanced.
  VBASE_IMPEXP static void DeInit();

  /// \brief
  ///   Returns whether the global VFileAccessManager instance has been created.
  VBASE_IMPEXP static bool IsInitialized();

public:
  /// @name File System Factory Management
  /// @{

  /// \brief
  ///   Registers an IVFileSystemFactory with this VFileAccessManager.
  /// \param pFactory
  ///   the IVFileSystemFactory to register. If it is already registered, it won't be
  ///   registered again.
  VBASE_IMPEXP void AddFileSystemFactory(IVFileSystemFactory* pFactory);

  /// \brief
  ///   Removes the specified IVFileSystemFactory from this VFileAccessManager.
  /// \param iIndex
  ///   index of the entry to remove; must be within the valid range
  VBASE_IMPEXP void RemoveFileSystemFactory(unsigned int iIndex);

  /// \brief
  ///   Removes all file system factories currently registered with this VFileAccessManager.
  VBASE_IMPEXP void ClearFileSystemFactories();

  /// \brief
  ///   Returns the index of \c pFactory, or \c -1 if \c pFactory is not registered with this
  ///    VFileAccessManager.
  VBASE_IMPEXP int IndexOfFileSystemFactory(IVFileSystemFactory* pFactory) const;

  /// \brief
  ///   Returns the number of registered file system factories.
  VBASE_IMPEXP unsigned int GetNumFileSystemFactories() const;

  /// \brief
  ///   Returns the requested file system factory.
  /// \param iIndex
  ///   index of the entry to remove; must be within the valid range
  /// \return
  ///   the requested file system factory
  VBASE_IMPEXP IVFileSystemFactory* GetFileSystemFactory(unsigned int iIndex) const;

  /// @}


  /// @name Root Management
  /// @{

  /// \brief
  ///   Adds or replaces a named root with the specified file system.
  /// \param szRootName
  ///   name of the root to set
  /// \param pFileSystem
  ///   the file system to set. If this is \c NULL, the root will be removed if it exists
  /// \param uiFlags
  ///   flags for controlling file system creation. See VFileSystemFlags
  VBASE_IMPEXP hkvResult SetRoot(const char* szRootName, IVFileSystem* pFileSystem, unsigned int uiFlags = 0);

  /// \brief
  ///   Adds or replaces a named root with a file system created from a native path.
  ///
  /// You can install file system factories (see IVFileSystemFactory) to hook into the file
  /// system creation process and create your own file system implementations. The registered 
  /// file system factories are called in inverse order (most recently added first).
  ///
  /// If no user-registered file system factories exist, or if these did not create a file 
  /// system, the default file system factory is called, which can create file systems for
  /// native file access, zip-based archives, and Vision packages (.varc).
  ///
  /// \param szRootName
  ///   name of the root to set
  /// \param szNativePath
  ///   the native path for which to create the file system.
  /// \param uiFlags
  ///   flags for controlling file system creation. See VFileSystemFlags
  /// \return
  ///   whether the root was successfully set
  VBASE_IMPEXP hkvResult SetRoot(const char* szRootName, const char* szNativePath, unsigned int uiFlags = 0);

  /// \brief
  ///   Removes all registered file systems from this file access manager.
  VBASE_IMPEXP void ClearRoots();

  /// \brief
  ///   Removes the file system registered with the specified root name.
  /// \param szRootName
  ///   the root name of the file system to remove
  VBASE_IMPEXP void RemoveRoot(const char* szRootName);

  /// \brief
  ///   Returns whether any file systems are registered with this file access manager.
  VBASE_IMPEXP bool HasRoots() const;

  /// \brief
  ///   Fills the passed string list with the root names of all registered file systems.
  /// \param destStrings
  ///   list to receive the root names
  VBASE_IMPEXP void GetRootNames(VStrList& destStrings) const;

  /// \brief
  ///   Returns the file system registered with a root name.
  /// \param szRootName
  ///   the name of the root to return the file system for
  /// \return
  ///   the file system registered with \c szRootName
  VBASE_IMPEXP IVFileSystem* GetRoot(const char* szRootName) const;

  /// \brief
  ///   Sets whether lookup caching should be enabled for file systems that support it. 
  ///
  /// The value set here affects the file systems already present in this VFileAccessManager, and
  /// sets the default for newly added file systems.
  ///
  /// \param bEnabled
  ///   whether lookup caching should be enabled
  VBASE_IMPEXP void SetLookupCachingEnabled(bool bEnabled);

  /// \brief
  ///   Returns whether lookup caching is enabled.
  ///
  /// Note that the returned value does not mean that the file systems support lookup caching; some
  /// file systems don't support caching at all; on some it is always enabled because of the way they
  /// are implemented; and finally some file systems (e.g., the VDiskFileSystem) allow enabling and
  /// disabling this feature. 
  ///
  /// The returned value does tell, however, the state of lookup caching for file systems that support
  /// enabling and disabling it, and the state that will be set on any file systems added to this
  /// VFileAccessManager.
  ///
  /// \return
  ///   whether lookup caching is enabled.
  VBASE_IMPEXP bool IsLookupCachingEnabled() const;

  /// @}


  /// @name Search Path Management
  /// @{

  /// \brief 
  ///   Adds a search path.
  /// \param szPath
  ///   the absolute path to add as search path. If the search path already exists, 
  ///   it is moved to the end of the search path list.
  /// \param uiFlags
  ///   flags for search path addition; see VFileAccessManager::AddSearchPathFlags
  /// \return
  ///   the index of the added or existing search path, or \c -1, if the search path did
  ///   not exist and could not be added
  VBASE_IMPEXP int AddSearchPath(const char* szPath, unsigned int uiFlags = 0);

  /// \brief
  ///   Inserts a search path at a specific position in the list.
  /// \param szPath
  ///   the absolute path to add as search path
  /// \param uiInsertBefore
  ///   index before which to insert the search path
  /// \param uiFlags
  ///   flags for search path addition; see VFileAccessManager::AddSearchPathFlags
  /// \return
  ///   whether the search path was successfully inserted
  VBASE_IMPEXP hkvResult InsertSearchPath(const char* szPath, unsigned int uiInsertBefore, unsigned int uiFlags = 0);

  /// \brief
  ///   Removes the search path at the specified position.
  /// \param uiIndex
  ///   index of the search path to remove
  VBASE_IMPEXP void RemoveSearchPath(unsigned int uiIndex);

  /// \brief
  ///   Removes the specified search path
  /// \param szPath
  ///   the search path to remove
  VBASE_IMPEXP void RemoveSearchPath(const char* szPath);

  /// \brief
  ///   Removes all search paths
  VBASE_IMPEXP void ClearSearchPaths();

  /// \brief
  ///   Determines the index of the specified search path
  /// \param szPath
  ///   the path to determine the index of
  /// \return
  ///   the index of the search path \c szPath, or \c -1, if this search path is not registered with
  ///   this file access manager.
  VBASE_IMPEXP int IndexOfSearchPath(const char* szPath);
  
  /// \brief
  ///   Returns the number of search paths registered with this file access manager.
  VBASE_IMPEXP unsigned int GetNumSearchPaths() const;
  
  /// \brief
  ///   Returns the object representing the specified search path
  /// \param uiIndex
  ///   the index of the search path to return
  /// \return
  ///   a reference to the object representing the specified search path
  VBASE_IMPEXP const VSearchPath& GetSearchPath(unsigned int uiIndex) const;

  /// @}


  /// @name File Handling
  /// @{

  /// \brief
  ///   Opens a file for reading.
  /// \param szPath
  ///   path of the file to open. This can be a native, an absolute, or a relative path.
  /// \param uiOpenFlags
  ///   flags to influence the behavior of this function; see VFileAccessManager::OpenFlags
  /// \return
  ///   a stream to read from the file if successful; \c NULL otherwise
  VBASE_IMPEXP IVFileInStream* Open(const char* szPath, unsigned int uiOpenFlags = 0);

  /// \brief
  ///   Opens a file for writing. If the file doesn't exist, it is created.
  /// \param szPath
  ///   path of the file to open/create. This can be a native, an absolute, or a relative path.
  /// \param uiCreateFlags
  ///   flags to influence the behavior of this function; see VFileAccessManager::CreateFlags
  /// \return
  ///   a stream to write to the file if successful; \c NULL otherwise
  VBASE_IMPEXP IVFileOutStream* Create(const char* szPath, unsigned int uiCreateFlags = 0);

  /// \brief
  ///   Removes (deletes) a file.
  /// \param szPath
  ///   path of the file to remove. This can be a native, an absolute, or a relative path.
  /// \param uiRemoveFlags
  ///   flags to influence the behavior of this function; see VFileAccessManager::RemoveFlags
  /// \return
  ///   a FileRemoveResult return code
  VBASE_IMPEXP VFileRemoveResult::Value Remove(const char* szPath, unsigned int uiRemoveFlags = 0);

  /// \brief
  ///   Checks whether a file exists.
  /// \param szPath
  ///   path of the file to check. This can be a native, an absolute, or a relative path.
  /// \return
  ///   \c true if the file exists; \c false if an error occurred during look-up or the file 
  ///   doesn't exist
  VBASE_IMPEXP bool FileExists(const char* szPath);

  /// \brief
  ///   Checks whether a directory exists.
  /// \param szPath
  ///   path of the directory to check. This can be a native, an absolute, or a relative path.
  /// \return
  ///   \c true if the directory exists; \c false if an error occurred during look-up or the directory 
  ///   doesn't exist
  VBASE_IMPEXP bool DirectoryExists(const char* szPath);

  /// \brief
  ///   Retrieves the timestamp of a file (usually the last-modified time).
  /// \param szPath
  ///   path of the file to retrieve the timestamp of. This can be a native, an absolute, or a 
  ///   relative path.
  /// \param destTime
  ///   receives the timestamp value
  /// \return
  ///   whether the time stamp could be successfully retrieved
  VBASE_IMPEXP hkvResult GetFileTimeStamp(const char *szPath, VDateTime& destTime);

  /// @}


  /// @name Path handling
  /// @{

  /// \brief
  ///   Returns whether a path is relative.
  VBASE_IMPEXP static bool IsPathRelative(const char* szPath);
  
  /// \brief
  ///   Returns whether a path is an absolute Vision path, ie. the path is of the form ":root/path".
  VBASE_IMPEXP static bool IsPathAbsolute(const char* szPath);
  
  /// \brief
  ///   Returns whether a path is an absolute native path.
  VBASE_IMPEXP static bool IsPathNative(const char* szPath);

  /// \brief
  ///   Returns whether a path is canonical. Note that this method is slow and intended mainly for debugging.
  VBASE_IMPEXP static bool IsPathCanonical(const char* szPath);

  /// \brief
  ///   Canonicalizes a path.
  ///
  /// Canonicalization removes empty or neutral ('.') path components, and resolves upward 
  /// references ('..') if possible.
  ///
  /// The canonicalized path never ends with a slash; casing is preserved.
  ///
  /// The separators of absolute paths (:root/path) and relative paths are changed to
  /// (forward) slashes; the separators of native paths are set to the current platform's
  /// standard separator.
  ///
  /// \param sPath
  ///   the path to canonicalize
  /// \return
  ///   whether the canonicalization succeeded. If it failed, \c sPath is set to an empty string.
  VBASE_IMPEXP static hkvResult CanonicalizePath(VStaticString<FS_MAX_PATH>& sPath);

  /// \brief
  ///   Appends one path to another.
  ///
  /// The base path may be an absolute vision path (:root/path), a native path or a relative path. The
  /// path to append must be empty or a relative path, with the exception that a leading slash or
  /// backslash is allowed for legacy reasons.
  ///
  /// The resulting path will be in canonical form; i.e., all redundant slashes and parent references
  /// are removed, and the components are all separated by the type of separator (slash or backslash)
  /// appropriate for the type of path being handled.
  ///
  /// \param sBasePath
  ///   the buffer containing the base path. \c szAppend will be appended to this buffer.
  /// \param szAppend
  ///   the path to append. Note that this must not point into the buffer of \c sBasePath, or the
  ///   result will be undefined.
  /// \return
  ///   whether appending the path succeeded.
  VBASE_IMPEXP static hkvResult AppendPath(VStaticString<FS_MAX_PATH>& sBasePath, const char* szAppend);

  /// \brief
  ///   Prepends one path to another.
  ///
  /// The path to prepend may be an absolute vision path (:root/path), a native path or a relative path. 
  /// The base path must be a relative path, with the exception that a leading slash or
  /// backslash is allowed for legacy reasons.
  ///
  /// The resulting path will be in canonical form; i.e., all redundant slashes and parent references
  /// are removed, and the components are all separated by the type of separator (slash or backslash)
  /// appropriate for the type of path being handled.
  ///
  /// \param sPath
  ///   the buffer containing the path to which to prepend \c szPrepend. Can be the empty string.
  /// \param szPrepend
  ///   the path to prepend. Note that this must not point into the buffer of \c sPath, or the
  ///   result will be undefined.
  /// \return
  ///   whether prepending the path succeeded.
  VBASE_IMPEXP static hkvResult PrependPath(VStaticString<FS_MAX_PATH>& sPath, const char* szPrepend);

  /// \brief
  ///   Joins two paths, and writes the result into a buffer.
  ///
  /// The first path may be an absolute vision path (:root/path), a native path or a relative path (NULL or
  /// the empty string is allowed, too). The second path must be empty or a relative path, with the exception 
  /// that a leading slash or backslash is allowed for legacy reasons.
  ///
  /// The resulting path will be in canonical form; i.e., all redundant slashes and parent references
  /// are removed, and the components are all separated by the type of separator (slash or backslash)
  /// appropriate for the type of path being handled.
  ///
  /// \param sResult
  ///   the buffer to which the joined path will be written.
  /// \param szPath1
  ///   the first partial path to join. Must not point into sResult, or the result of this function
  ///   will be undefined.
  /// \param szPath2
  ///   the second partial path to join. Must not point into sResult, or the result of this function
  ///   will be undefined.
  /// \return
  ///   whether joining the paths succeeded
  VBASE_IMPEXP static hkvResult JoinPaths(VStaticString<FS_MAX_PATH>& sResult, const char* szPath1, const char* szPath2);

  /// \brief
  ///   Tries to make the path passed into this function relative, based on the specified criteria.
  ///
  /// The path returned on success meets the specified criteria, so accessing the path with the specified
  /// access mode will likely succeed.
  ///
  /// \param szPath
  ///   the path to make relative. This can be a relative, an absolute, or a native path.
  /// \param result
  ///   the result of the path conversion
  /// \param eAccessMode
  ///   the intended access to the path
  /// \param eElementType
  ///   the type of file system element (file/directory) expected at \c szPath
  /// \return
  ///   whether \c szPath could successfully be converted
  VBASE_IMPEXP hkvResult MakePathRelative(const char* szPath, RelativePathResult& result, VFileSystemAccessMode::Value eAccessMode, VFileSystemElementType::Value eElementType) const;

  /// \brief
  ///   Tries to make the path passed into this function absolute, based on the specified criteria.
  ///
  /// The path returned on success meets the specified criteria, so accessing the path with the specified
  /// access mode will likely succeed.
  ///
  /// \param szPath
  ///   the path to make absolute. This can be a relative, an absolute, or a native path.
  /// \param result
  ///   the result of the path conversion
  /// \param eAccessMode
  ///   the intended access to the path
  /// \param eElementType
  ///   the type of file system element (file/directory) expected at \c szPath
  /// \param pContext
  ///   (optional) a lookup context in which the result of any asset lookup is stored
  /// \return
  ///   whether \c szPath could successfully be converted
  VBASE_IMPEXP hkvResult MakePathAbsolute(const char* szPath, AbsolutePathResult& result, VFileSystemAccessMode::Value eAccessMode, VFileSystemElementType::Value eElementType, VPathLookupContext* pContext = NULL) const;

  /// \brief
  ///   Tries to make the path passed into this function native, based on the specified criteria.
  ///
  /// The path returned on success meets the specified criteria, so accessing the path with the specified
  /// access mode will likely succeed.
  ///
  /// \param szPath
  ///   the path to make native. This can be a relative, an absolute, or a native path.
  /// \param result
  ///   the result of the path conversion
  /// \param eAccessMode
  ///   the intended access to the path
  /// \param eElementType
  ///   the type of file system element (file/directory) expected at \c szPath
  /// \return
  ///   whether \c szPath could successfully be converted
  VBASE_IMPEXP hkvResult MakePathNative(const char* szPath, NativePathResult& result, VFileSystemAccessMode::Value eAccessMode, VFileSystemElementType::Value eElementType) const;
  /// @}



  /// @name Asset management 
  /// @{

  /// \brief
  ///   Returns whether \c szSearchPath is an asset library and whether \c szProfileName 
  ///   is a supported asset profile within this library.
  /// \param szProfileName
  ///   the asset profile to check
  /// \param szSearchPath
  ///   the absolute path in which to look. This doesn't need to be a registered search path
  ///   within this file access manager; however, at least the underlying file system must be
  ///   registered for this function to return a meaningful result.
  /// \return
  ///   \c true if \c szSearchPath is a valid absolute path and is the root of an asset library
  ///   which supports the specified profile; \c false otherwise
  VBASE_IMPEXP bool IsAssetProfileSupported(const char* szProfileName, const char* szSearchPath);

  /// \brief
  ///   Looks up an asset in all registered search paths.
  /// \param szAsset
  ///   The asset to look up
  /// \return NULL if not found, pointer to the data struct otherwise
  VBASE_IMPEXP const VAssetInfo* LookUpAsset(const char* szAsset) const;

  /// \brief
  ///   Looks up an asset in all registered search paths.
  /// \param szAsset
  ///   The asset to look up
  /// \param context
  ///   The context used to track the current look-up
  VBASE_IMPEXP void LookUpAsset(const char* szAsset, VPathLookupContext& context) const;

  /// \brief
  ///   Reloads the asset lookup tables for either one specific, or for all search paths 
  ///   currently registered with this manager.
  /// \param szSearchPath
  ///   The search path for which to reload the asset lookup table. If the search path
  ///   is \c NULL, the asset lookup tables are reloaded for all search paths.
  VBASE_IMPEXP void ReloadAssetLookupTables(const char* szSearchPath = NULL);

  /// @}

private:
  void SetRootInternal(const char* szRootName, IVFileSystem* pRoot);

  // Checks the path for illegal characters
  bool CheckPathIsSane(const char* szPath) const;

  // Fills a path lookup context; performs asset lookup if necessary
  void PreparePathLookupContext(const char* pszFileName, VFileSystemAccessMode::Value eAccessMode, VFileSystemElementType::Value eElementType, VPathLookupContext& context) const;

  // Clears the cache that's keeping the most recently looked-up asset
  void ClearAssetLookupCache();

  // Path conversion functions form one path type to another
  hkvResult MakePathRelativeFromAbsolute(const char* szPath, RelativePathResult& result, VFileSystemAccessMode::Value eAccessMode, VFileSystemElementType::Value eElementType) const;
  hkvResult MakePathRelativeFromNative(const char* szPath, RelativePathResult& result, VFileSystemAccessMode::Value eAccessMode, VFileSystemElementType::Value eElementType) const;
  hkvResult MakePathRelativeFromRelative(const char* szPath, RelativePathResult& result, VFileSystemAccessMode::Value eAccessMode, VFileSystemElementType::Value eElementType) const;
  hkvResult MakePathAbsoluteFromRelative(const char* szPath, AbsolutePathResult& result, VFileSystemAccessMode::Value eAccessMode, VFileSystemElementType::Value eElementType, VPathLookupContext& lookupContext) const;
  hkvResult MakePathAbsoluteFromAbsolute(const char* szPath, AbsolutePathResult& result, VFileSystemAccessMode::Value eAccessMode, VFileSystemElementType::Value eElementType) const;
  hkvResult MakePathAbsoluteFromNative(const char* szPath, AbsolutePathResult& result, VFileSystemAccessMode::Value eAccessMode, VFileSystemElementType::Value eElementType) const;
  hkvResult MakePathNativeFromAbsolute(const char* szPath, NativePathResult& result, VFileSystemAccessMode::Value eAccessMode, VFileSystemElementType::Value eElementType) const;

private:
  static VFileAccessManagerPtr s_spInstance;

private:
  mutable VMutex m_protect;

  VSmartPtr<IVFileSystemFactory> m_spDefaultFileSystemFactory;
  VArray< VSmartPtr<IVFileSystemFactory> > m_fileSystemFactories;

  VMap<VString, IVFileSystem*> m_roots;
  VArray< VSmartPtr<VSearchPath> > m_searchPaths;
  bool m_bLookupCachingEnabled;

  // These are not part of any observable state, thus may be mutable.
  mutable VStaticString<FS_MAX_PATH> m_sLastAsset;
  mutable VStaticString<FS_MAX_PATH> m_sLastAssetDataDir;
  mutable const VAssetInfo* m_pLastAssetInfo;
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
