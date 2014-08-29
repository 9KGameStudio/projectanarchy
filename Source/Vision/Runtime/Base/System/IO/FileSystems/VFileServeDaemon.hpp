/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#if !defined(VFileServeDaemon_h__)
#define VFileServeDaemon_h__

#include <Vision/Runtime/Base/System/Threading/Thread/VBackgroundThread.hpp>
#include <Vision/Runtime/Base/Container/VScopedPtr.hpp>

class IVNativeDialog;
class VMessage;

/// \brief A helper that runs in the background and takes care of setting up and maintaining a FileServe connection.
class VFileServeDaemon : private IVisCallbackHandler_cl, private VBackgroundThread
{
public:

  /// \brief Options that control how how to handle connecting to a server.
  enum VFileServeOptions_e
  {
    VFSO_ALLOW_CACHE_FALLBACK         = V_BIT(0), ///< Fall back to the data cached on the device if no connection could be achieved.
    VFSO_ALLOW_PACKAGE_FALLBACK       = V_BIT(1), ///< Fall back to the data stored inside the application package if no connection could be achieved.
    VFSO_FORCE_CONNECTION_FROM_FILE   = V_BIT(2), ///< Force using the IP stored in the settings file. This disables advertising via network broadcasts and does not show a user dialog.
  };

  /// \brief
  ///   Returns the global instance of the VFileServeDaemon; asserts if the instance has not been created
  ///   by a call to Init() yet.
  VBASE_IMPEXP static VFileServeDaemon* GetInstance();

  /// \brief
  ///   Creates the global VFileServeDaemon instance.
  VBASE_IMPEXP static void Init(const char* szNativeSettingsFile, const char* szNativeCacheDir, unsigned int uiOptions = VFSO_ALLOW_CACHE_FALLBACK | VFSO_ALLOW_PACKAGE_FALLBACK);

  /// \brief
  ///   Destroys the global VFileServeDaemon instance.
  VBASE_IMPEXP static void DeInit();

  /// \brief
  ///   Returns whether the global VFileServeDaemon instance has been created.
  VBASE_IMPEXP static bool IsInitialized();

  /// \brief Describes the FileServe setup status.
  enum SetupResult
  {
    SR_NOT_DONE_YET,
    SR_FINISHED,
    SR_FAILED,
  };

  /// \brief Runs a setup loop that shows a user dialog until a network connection has been achieved.
  VBASE_IMPEXP SetupResult RunSetup();

  /// \brief Factory helper to create a filesystem instance backed by FileServe.
  ///
  /// \param szRootName
  ///   The name of the root under which the filesystem will be mounted.
  ///
  /// \param szNativePath
  ///   The native package path to use as a fallback if VFSO_ALLOW_PACKAGE_FALLBACK was specified.
  VBASE_IMPEXP IVFileSystem* CreateFileSystem(const char* szRootName, const char* szNativePath);

  /// \brief Clears the directory that is used to cache the files in \c szAbsolutePath.
  VBASE_IMPEXP hkvResult ClearCacheDirectory(const char* szAbsolutePath) const;

  /// \brief Checks if the file given by \c szAbsolutePath exists in the cache.
  VBASE_IMPEXP bool FileExistsInCache(const char* szAbsolutePath) const;

  /// \brief Checks if the directory given by \c szAbsolutePath exists in the cache.
  VBASE_IMPEXP bool DirExistsInCache(const char* szAbsolutePath) const;

  /// \brief Returns the remote host that is used.
  VBASE_IMPEXP const char* GetRemoteHost()
  {
    return m_szRemoteHost;
  }

  /// \brief Returns the name of the remote host that is used.
  VBASE_IMPEXP const char* GetRemoteMachineName()
  {
    return m_szRemoteMachineName;
  }

  /// \brief Returns the port that is used to re-establish the connection.
  VBASE_IMPEXP unsigned short GetReconnectionPort()
  {
    return m_usReconnectionPort;
  }

  /// \brief Returns true if the connection is currently established.
  VBASE_IMPEXP bool IsConnected()
  {
    return m_bConnectionInitialized;
  }

  /// \brief Instructs the remote host to stop the FileServe tool with the given exit code.
  ///   The remote host will only handle this request if it was started with --allowexit.
  VBASE_IMPEXP void SendExitCode(int iExitCode);

  /// \brief Internal, do not use
  void Log(hkvLogMsgType::Enum MsgType, const char* szText, int iIndentation, const char* szTag);
  
  /// \brief Internal, do not use
  VBASE_IMPEXP void LogBlocking(hkvLogMsgType::Enum MsgType, const char* szText, int iIndentation, const char* szTag);

private:
  friend class VFileServeFileSystem;

  static VFileServeDaemon* s_pInstance;

  /// \brief Constructor.
  VFileServeDaemon(const char* szNativeSettingsFile, const char* szNativeCacheDir, unsigned int uiOptions);

  /// \brief Destructor.
  virtual ~VFileServeDaemon();

  // Disable copying
  VFileServeDaemon(const VFileServeDaemon&);
  VFileServeDaemon& operator=(const VFileServeDaemon&);

  virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData) HKV_OVERRIDE;
  virtual void Run() HKV_OVERRIDE;

  void LoadSettingsFromFile();
  void SetupInitialDialog();
  SetupResult ShowFailDialog();
  hkvResult InitConnection();

  bool EnsureConnected();

  hkvResult ResolveAbsolute(const char* szAbsolutePath, VPathLookupContext& context);
  hkvResult ResolveRelative(const char* szPathBelowRoot, VPathLookupContext& context);

  IVFileInStream* Open(const char* szAbsolutePath, unsigned int uiOpenFlags);
  hkvResult GetFileTimeStamp(const char* szAbsolutePath, VDateTime& destTime);

  hkvResult ResolveFromHostAbsolute(const char* szAbsolutePath, VPathLookupContext& context);
  hkvResult ResolveFromCacheAbsolute(const char* szAbsolutePath, VPathLookupContext& context);

  hkvResult ResolveFromHostRelative(const char* szPathBelowRoot, VPathLookupContext& context);
  hkvResult ResolveFromCacheRelative(const char* szPathBelowRoot, VPathLookupContext& context);

  hkvResult DownloadFile(const char* szAbsolutePath, VStaticString<FS_MAX_PATH> &szCachePath, int iRequestIndex);

  hkvResult InitiateFileTransfer(int iRequestIndex, const char* szAbsolutePath, int& iFileSize);

  void Stop();

  void GetCachePath(const char* szAbsolutePath, VStaticString<FS_MAX_PATH>& sCachePathOut) const;
  void GetAttributePath(const char* szAbsolutePath, VStaticString<FS_MAX_PATH>& sAttributePathOut) const;

  void GetCachedModifyTime(const char* szAbsolutePath, VDateTime& time) const;
  void SetCachedModifyTime(const char* szAbsolutePath, const VDateTime& time) const;

  void LogErrorFromMessage(VMessage* pMessage) const;

  enum InitState
  {
    FIRST_RUN,
    SHOWING_DIALOG,
    SHOWING_FAIL_DIALOG,
    SETUP_FINISHED,
    FAILED,
  };

  InitState m_initState;
  VScopedPtr<IVNativeDialog> m_spDialog;
  VConnection* m_pRemoteConnection;

  int m_iUseCacheButton;
  int m_iUseLastButton;
  int m_iQuitButton;

  VString m_szCacheDir;
  VString m_szCachePrefix;
  VString m_szSettingsFile; 

  VString m_szRemoteHost;
  VString m_szRemoteMachineName;
  unsigned short m_usReconnectionPort;

  VString m_szRemoteHostFromFile;
  VString m_szRemoteMachineNameFromFile;
  unsigned short m_usRemotePortFromFile;

  unsigned int m_uiOptions;

  VMutex m_mutex;
  VMutex m_logMutex;
  VEvent m_connectEvent;

  bool m_bRunning;
  bool m_bBroadcastFailed;
  bool m_bConnectionInitialized;
  bool m_bAllowWaitForReconnect;
  bool m_bAllowLogging;
  VEvent m_stopEvent;

  VMap<VString, VDateTime> m_resolveCache;

  static int s_iNextRequestIndex;
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
