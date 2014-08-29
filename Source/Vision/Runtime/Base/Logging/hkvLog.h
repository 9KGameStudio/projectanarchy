/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file 

#ifndef VLOGGER_H_INCLUDED
#define VLOGGER_H_INCLUDED

#include <Vision/Runtime/Base/Container/VArray.hpp>
#include <Vision/Runtime/Base/Container/VScopedPtr.hpp>


// Insert this define into a cpp file to map the VerboseDebugMsg function to some other logging function (e.g. 'Debug' or 'Dev'). Otherwise that logging call will always be ignored.
// In shipping code VerboseDebugMsg should never be defined to anything such that that logging code is always stripped out.
//#define VerboseDebugMsg Debug


/// \brief This macro allows to convert a Managed Code 'LogInterface' into a native C++ 'hkvLogInterface'.
/// It should be used in managed code to pass through a LogInterface to C++. The macro is placed in vBase, because Managed libraries to not export macros.
#define LogManagedTohkvLog(log) ((hkvLogInterface*) (void*) (LogBridgeManaged(log).GetLogInterface()))

/// \brief Creates a uniquely named instance of hkvLogBlock.\n
///   Usage example: HKV_LOG_BLOCK("My Log Block");\n
///   Usage example: HKV_LOG_BLOCK("My Log Block", szFileNameForContextInfo);\n
///   Usage example: HKV_LOG_BLOCK("My Log Block", szFileNameForContextInfo, true);\n
///   Usage example: HKV_LOG_BLOCK(pMyOwnLoggingSystem, "My Log Block");\n
///   Usage example: HKV_LOG_BLOCK(pMyOwnLoggingSystem, "My Log Block", true);\n
/// \sa hkvLogBlock
#define HKV_LOG_BLOCK hkvLogBlock V_CAT(Block, __LINE__)

class hkvLogBlock;


/// \brief Describes the different types of messages that are sent through hkvLogInterface::HandleLogMessage
struct hkvLogMsgType
{
  /// \brief Describes the different types of messages that are sent through hkvLogInterface::HandleLogMessage
  enum Enum
  {
    BeginBlock      =-2,  ///< A logging block has been opened. szText contains its name, szTag sometimes contains some 'context information', such as a file name
    EndBlock        =-1,  ///< Sent when a logging block is closed again.
    None            = 0,  ///< Placeholder, not used, except when setting the log level.
    FatalError      = 1,  ///< A fatal error is logged. Unless hkvGlobalLog is configured to ignore these, this will trigger an engine shutdown afterwards.
    Error           = 2,  ///< Error message.
    SeriousWarning  = 3,  ///< A warning that is serious, so it should get fixed asap.
    Warning         = 4,  ///< A warning that might indicate a problem, but is not fatal when it is ignored.
    Success         = 5,  ///< Something went well for a change.
    Info            = 6,  ///< Some general information about the current status.
    Dev             = 7,  ///< More detailed information. Will be compiled out in release builds.
    Debug           = 8,  ///< Even more detailed information, will be compiled out in dev builds and above.
    AllMessages     = 8,  ///< Placeholder, not used, except when setting the log level.
  };
};


/// \brief The base class for all logging systems.
///
/// A logging system is a class that can be passed to the functions in hkvLog and will then receive the formatted output through its
/// HandleLogMessage() function.
class hkvLogInterface
{
public:
  /// \brief Constructor.
  VBASE_IMPEXP hkvLogInterface() { m_pCurrentLogBlock = NULL; m_LogLevel = hkvLogMsgType::AllMessages; }

  /// \brief Virtual destructor.
  VBASE_IMPEXP virtual ~hkvLogInterface() {}


  /// \brief Override this function to implement custom logging functionality.
  ///
  /// \param MsgType
  ///   The type of message that is passed to the log interface. See hkvLogMsgType for details.
  /// \param szText
  ///   In case of a message (FatalError to Debug) this is the message text.
  ///   In case of a log block message (BeginBlock / EndBlock), this is the name of the block.
  /// \param iIndentation
  ///   The amount of indentation for the message or block. This is computed automatically, using the hkvLogBlock's.
  ///   Each log block will increase the indentation for following messages. Of course the logging system can ignore this information.
  /// \param szTag
  ///   In case of a message this is the message tag. A tag is the sub string in brackets right at the start of a message, ie.
  ///   a message like "[tag]Message Text" will result in szText being "Message Text" and szTag being "tag".
  ///   This can be used to indicate certain types of messages (e.g. from some sub system).
  ///   In most cases this can and should be ignored, as it is a way to communicate additional information to a specific kind of sub system.
  ///   In case of a log block, this is the extra information, that was passed to it. Often this contains a file name or some other information
  ///   that gives more context to the log block.
  VBASE_IMPEXP virtual void HandleLogMessage(hkvLogMsgType::Enum MsgType, const char* szText, int iIndentation, const char* szTag) = 0;

  /// \brief This function can be overridden to filter out certain messages. The filtering is done before any log blocks are written.
  ///
  /// The function should return true to let messages through, false to filter a message out. Note that it is more efficient to filter messages by
  /// log level, however this function can filter by more complicated criteria.
  ///
  /// \param MsgType
  ///   The type of message that is passed to the log interface. See hkvLogMsgType for details.
  /// \param szText
  ///   In case of a message (FatalError to Debug) this is the message text.
  ///   In case of a log block message (BeginBlock / EndBlock), this is the name of the block.
  /// \param szTag
  ///   In case of a message this is the message tag. A tag is the sub string in brackets right at the start of a message, ie.
  ///   a message like "[tag]Message Text" will result in szText being "Message Text" and szTag being "tag".
  ///   This can be used to indicate certain types of messages (e.g. from some sub system).
  ///   In most cases this can and should be ignored, as it is a way to communicate additional information to a specific kind of sub system.
  ///   In case of a log block, this is the extra information, that was passed to it. Often this contains a file name or some other information
  ///   that gives more context to the log block.
  VBASE_IMPEXP virtual bool CustomMessageFilter(hkvLogMsgType::Enum MsgType, const char* szText, const char* szTag) { return true; }

  /// \brief Specifies the log level for this log interface. All messages below this level will be filtered out by hkvLog already and will not be passed to HandleLogMessage().
  ///
  /// \param Level
  ///   The new log level used for filtering out messages.
  VBASE_IMPEXP void SetLogLevel(hkvLogMsgType::Enum Level) { m_LogLevel = Level; }

  /// \brief Returns the current log level.
  VBASE_IMPEXP hkvLogMsgType::Enum GetLogLevel() { return m_LogLevel; }

private:
  friend class hkvLog;
  friend class hkvLogBlock;

  // This filter is directly read by hkvLog to filter out messages even before the format string needs to be processed.
  hkvLogMsgType::Enum m_LogLevel;

  // The latest log block (top of the stack). hkvLogBlock instances will read and modify this variable to keep track of who is on top.
  hkvLogBlock* m_pCurrentLogBlock;

  /// \brief This function can be overridden, such that classes like hkvGlobalLog can implement this in a more thread safe way (e.g. with thread local variables).
  VBASE_IMPEXP virtual hkvLogBlock* GetCurrentLogBlock() const { return m_pCurrentLogBlock; }
  VBASE_IMPEXP virtual void SetCurrentLogBlock(hkvLogBlock* pCurrentLogBlock) { m_pCurrentLogBlock = pCurrentLogBlock; }
};


/// \brief This class is the central piece of Vision's logging system. All log message go through here and are then dispatched to some hkvLogInterface instance.
///
/// hkvLog provides functions for logging messages of different importance. It already decides whether a message can be filtered out, depending on which system
/// the message would be sent to (\see hkvLogInterface::SetLogLevel()).
/// All logging functions are available in two variants, one where the target hkvLogInterface instance can be specified, and one without that parameter. The latter
/// version will always dispatch messages to the 'default' logging system, which can be set using SetDefaultLogSystem(), but which will default to hkvGlobalLog.
/// If you want to log messages from a specific process into another logging system, for example you want to gather all messages from some mesh-conversion step
/// to output in a GUI, you should create a custom hkvLogInterface-derived class and pass that as the target logging system into that specific code path.
/// If, however, all that you want to do is to write all the standard log messages to some other output (e.g. to some specific file, in a different format than what
/// Vision provides), you should check out hkvGlobalLog, which allows to attach custom 'log writers', such that all messages that are received by hkvGlobalLog can
/// be written to different outputs. By default Vision already writes all global log messages out via printf, OutputDebugString, the ResourceViewer panel and an HTML file.
class hkvLog
{
public:
  /// \brief If some logging system handles a message put wants to pass it over to be handled by some other logging system (e.g. hkvGlobalLog), this function can be used to do so more easily.
  ///
  /// Just pass in the target interface, the message type, text and tag and the message will be dispatched properly (including writing log blocks in the target system).
  /// The only correct alternative for this function is to call hkvLog::Info/Error/etc. with the target interface as the first parameter, but this would result in
  /// needless parsing and a large switch statement to handle all the different message types.
  /// \note It is NOT correct to directly call HandleLogMessage() on the target log interface, as that would not correctly write out log blocks and would ignore the log level filtering.
  ///
  /// \param pInterface
  ///   The interface to which to pass the log message to.
  /// \param MsgType
  ///   The message type.
  /// \param szText
  ///   The message text.
  /// \param szTag
  ///   An optional tag that is passed along.
  VBASE_IMPEXP static void PassToOtherLog(hkvLogInterface* pInterface, hkvLogMsgType::Enum MsgType, const char* szText, const char* szTag);

  /// \brief Switches to which log system to pass all messages to, when no target system is provided manually.
  /// This should NEVER be required to be used. It is possible to completely replace the default logging system, but there is no real use-case for this.
  VBASE_IMPEXP static void SetDefaultLogSystem(hkvLogInterface* pInterface);

  /// \brief Returns which logging system all messages are passed to that are not specifically passed to some target logging system.
  VBASE_IMPEXP static hkvLogInterface* GetDefaultLogSystem();


  /// \brief Outputs a 'fatal error' log message to the default logging system. Allows message formatting using printf notation.
  ///
  /// \attention FatalError() has a special meaning, namely being FATAL. By default the logging system will still output this message and then SHUT DOWN the engine.
  ///   This is a remnant from the old Vision 'logging system' and should be considered deprecated. Simply use Error() instead for real error messages and use VASSERT_MSG
  ///   for all errors that are indeed fatal.
  /// \sa hkvGlobalLog::SetOnFatalErrorCallback
  /// \sa hkvGlobalLog::EnableEngineShutdownOnFatalError
  ///
  /// \param szMessage
  ///   The log message text.
  VBASE_IMPEXP static void FatalError(const char* szMessage, ...);

  /// \brief Outputs a 'fatal error' log message to the provided logging system. Allows message formatting using printf notation.
  VBASE_IMPEXP static void FatalError(hkvLogInterface* pTarget, const char* szMessage, ...);


  /// \brief Outputs an 'error' log message to the default logging system. Allows message formatting using printf notation.
  VBASE_IMPEXP static void Error(const char* szMessage, ...);

  /// \brief Outputs an 'error' log message to the provided logging system. Allows message formatting using printf notation.
  VBASE_IMPEXP static void Error(hkvLogInterface* pTarget, const char* szMessage, ...);


  /// \brief Outputs a 'serious warning' log message to the default logging system. Allows message formatting using printf notation.
  VBASE_IMPEXP static void SeriousWarning(const char* szMessage, ...);

  /// \brief Outputs a 'serious warning' log message to the provided logging system. Allows message formatting using printf notation.
  VBASE_IMPEXP static void SeriousWarning(hkvLogInterface* pTarget, const char* szMessage, ...);


  /// \brief Outputs a 'warning' log message to the default logging system. Allows message formatting using printf notation.
  VBASE_IMPEXP static void Warning(const char* szMessage, ...);

  /// \brief Outputs a 'warning' log message to the provided logging system. Allows message formatting using printf notation.
  VBASE_IMPEXP static void Warning(hkvLogInterface* pTarget, const char* szMessage, ...);

  
  /// \brief Outputs a 'success' log message to the default logging system. Allows message formatting using printf notation.
  VBASE_IMPEXP static void Success(const char* szMessage, ...);

  /// \brief Outputs a 'success' log message to the provided logging system. Allows message formatting using printf notation.
  VBASE_IMPEXP static void Success(hkvLogInterface* pTarget, const char* szMessage, ...);


  /// \brief Outputs an 'info' log message to the default logging system. Allows message formatting using printf notation.
  VBASE_IMPEXP static void Info(const char* szMessage, ...);

  /// \brief Outputs an 'info' log message to the provided logging system. Allows message formatting using printf notation.
  VBASE_IMPEXP static void Info(hkvLogInterface* pTarget, const char* szMessage, ...);


#ifdef HK_DEBUG

  /// \brief Outputs a 'Dev' log message to the default logging system. Allows message formatting using printf notation.
  /// Note: 'Dev' messages are compiled out in release builds. They will thus only appear in Debug and Dev builds.
  VBASE_IMPEXP static void Dev(const char* szMessage, ...);

  /// \brief Outputs a 'Dev' log message to the provided logging system. Allows message formatting using printf notation.
  /// Note: 'Dev' messages are compiled out in release builds. They will thus only appear in Debug and Dev builds.
  VBASE_IMPEXP static void Dev(hkvLogInterface* pTarget, const char* szMessage, ...);

#else

  /// \brief Outputs a 'Dev' log message to the default logging system. Allows message formatting using printf notation.
  /// Note: 'Dev' messages are compiled out in release builds. They will thus only appear in Debug and Dev builds.
  HKV_FORCE_INLINE static void Dev(const char* szMessage, ...) { }


  /// \brief Outputs a 'Dev' log message to the provided logging system. Allows message formatting using printf notation.
  /// Note: Dev' messages are compiled out in release builds. They will thus only appear in Debug and Dev builds.
  HKV_FORCE_INLINE static void Dev(hkvLogInterface* pTarget, const char* szMessage, ...) { }

#endif


#ifdef HK_DEBUG_SLOW

  /// \brief Outputs a 'Debug' log message to the default logging system. Allows message formatting using printf notation.
  /// Note: 'Debug' messages are compiled out in dev and release builds. They will thus only appear in Debug builds.
  VBASE_IMPEXP static void Debug(const char* szMessage, ...);

  /// \brief Outputs a 'Debug' log message to the provided logging system. Allows message formatting using printf notation.
  /// Note: 'Debug' messages are compiled out in dev and release builds. They will thus only appear in Debug builds.
  VBASE_IMPEXP static void Debug(hkvLogInterface* pTarget, const char* szMessage, ...);

#else

  /// \brief Outputs a 'Debug' log message to the default logging system. Allows message formatting using printf notation.
  /// Note: 'Debug' messages are compiled out in dev and release builds. They will thus only appear in Debug builds.
  HKV_FORCE_INLINE static void Debug(const char* szMessage, ...) { }


  /// \brief Outputs a 'Debug' log message to the provided logging system. Allows message formatting using printf notation.
  /// Note: 'Debug' messages are compiled out in dev and release builds. They will thus only appear in Debug builds.
  HKV_FORCE_INLINE static void Debug(hkvLogInterface* pTarget, const char* szMessage, ...) { }

#endif

  /// \brief This is a special type of message that can be used for very verbose logging during debugging some piece of code.
  ///
  /// By default this function does not have an implementation and thus will not do anything. It is only a stub so that one can insert
  /// these kinds of logging messages into code without actually doing anything (and thus without spamming the log or reducing performance).
  ///
  /// The idea is to use a \code #define \endcode to redirect this function to some other logging function, for example the 'Dev' or 'Debug' log functions.
  /// That can be done on a per cpp-file basis and thus this function can be 'enabled' only very locally and only during some debugging session.
  /// Still afterwards you can commit all code with the logging functionality included, you only need to make sure that VerboseDebugMsg is not defined
  /// to redirect to anything else anymore.\n
  ///
  /// \code
  /// // comment out the following line before committing !
  /// #define VerboseDebugMsg Debug
  /// ...
  /// hkvLog::VerboseDebugMsg("This will actually write to hkvLog::Debug");
  /// \endcode
  ///
  /// \param szMessage
  ///   The log message text.
  HKV_FORCE_INLINE static void VerboseDebugMsg(const char* szMessage, ...) { }

  /// \brief Same as the other variant of VerboseDebugMsg, only that it allows to specify a target log interface.
  HKV_FORCE_INLINE static void VerboseDebugMsg(hkvLogInterface* pTarget, const char* szMessage, ...) { }

private:
  friend class hkvLogBlock;

  static void WriteAllLogBlocks(hkvLogInterface* pInterface, hkvLogBlock* pBlock);

  static hkvLogInterface* s_pDefaultLogSystem;
};

/// \brief hkvGlobalLog is a singleton class that acts as the default logging system, when messages are not specifically sent to some other system.
/// 
/// hkvGlobalLog broadcasts all messages to different 'log writers' which can then output the messages in different ways.
/// The hkvGlobalLog class itself does not output messages in any way.
/// By default the log writers hkvLogWriter::Printf, hkvLogWriter::VisualStudio, hkvLogWriter::HTML are already attached (in VBaseInit) and will
/// write all log message to the console, the Visual Studio console, the ResourceViewer log panel and the HTML file "Log.htm" in the binary directory.
class hkvGlobalLog : public hkvLogInterface
{
public:

  /// \brief The function signature for log writers.
  typedef void(*LogWriter)(hkvLogMsgType::Enum MsgType, const char* szText, int iIndentation, const char* szTag, void* pPassThrough);

  /// \brief The function signature for message filters.
  typedef bool(*MessageFilter)(hkvLogMsgType::Enum MsgType, const char* szText, const char* szTag, void* pPassThrough);

  /// \brief The function signature for fatal error callbacks.
  typedef void (*ON_FATALERROR_CALLBACK)(const char* szMessage);

  /// \brief Constructor.
  VBASE_IMPEXP hkvGlobalLog();

  /// \brief Destructor.
  VBASE_IMPEXP virtual ~hkvGlobalLog();

  /// \brief Returns one global instance of hkvGlobalLog (singleton). The instance will be created when none exists yet.
  VBASE_IMPEXP static hkvGlobalLog* GetInstance();

  /// \brief Adds a 'log writer' to the hkvGlobalLog instance. Every time a log message is sent to the global log, all registered log writers will be notified.
  ///
  /// \param Handler
  ///   The function that will be called  when a log message is handled by hkvGlobalLog.
  /// \param pPassThrough
  ///   An additional pointer that will be passed unmodified to the LogWriter function.
  VBASE_IMPEXP void AddLogWriter(LogWriter Handler, void* pPassThrough = NULL);

  /// \brief Removes a previously registered log writer. The Handler and pPassThrough parameter need to be identical to what was passed to AddLogWriter.
  /// This function will assert when the given log writer cannot be found.
  VBASE_IMPEXP void RemoveLogWriter(LogWriter Handler, void* pPassThrough = NULL);

  /// \brief Allows to query whether a given log writer with pass through value has been registered earlier. Use this to prevent calling RemoveLogWriter
  /// when it is not certain whether a log writer was added earlier, or has already been removed, because RemoveLogWriter will crash the application
  /// when a non-existing log writer shall be removed.
  VBASE_IMPEXP bool WasLogWriterAdded(LogWriter Handler, void* pPassThrough = NULL);

  /// \brief Overridden function from hkvLogInterface. Will broadcast all incoming messages to the registered log writers.
  VBASE_IMPEXP virtual void HandleLogMessage(hkvLogMsgType::Enum MsgType, const char* szText, int iIndentation, const char* szTag) HKV_OVERRIDE;

  /// \brief Adds a 'message filter' to the hkvGlobalLog instance. Every time a log message is sent to the global log, all message filters must let it pass (by returning true).
  ///
  /// If any filter function returns false, the log message is not output.
  ///
  /// \param Handler
  ///   The function that will be called  when a log message is handled by hkvGlobalLog.
  /// \param pPassThrough
  ///   An additional pointer that will be passed unmodified to the LogWriter function.
  VBASE_IMPEXP void AddCustomMessageFilter(MessageFilter Handler, void* pPassThrough = NULL);

  /// \brief Removes a previously registered message filter. The Handler and pPassThrough parameter need to be identical to what was passed to AddCustomMessageFilter.
  /// This function will assert when the given message filter cannot be found.
  VBASE_IMPEXP void RemoveCustomMessageFilter(MessageFilter Handler, void* pPassThrough = NULL);

  /// \brief Overridden function from hkvLogInterface. Will broadcast all incoming messages to the registered log writers.
  VBASE_IMPEXP virtual bool CustomMessageFilter(hkvLogMsgType::Enum MsgType, const char* szText, const char* szTag) HKV_OVERRIDE;

  /// \brief Enables whether a FatalError logged through hkvGlobalLog will trigger an Engine shutdown. On by default.
  ///
  /// This feature can be regarded as deprecated. You should never use hkvLog::FatalError, instead use hkvLog::Error for not-so-fatal errors and VASSERT_MSG for others.
  /// Although you can disable the default behavior (the engine will be shut down when a fatal error arrives), this is not advised, as the engine will be in an
  /// inconsistent state then (the code expects that the engine gets shut down in FatalError calls, so everything after that might not work properly).
  /// \sa SetOnFatalErrorCallback
  ///
  /// \param bFatalErrorsShouldReallyBeFatal
  ///   If true, fatal erros will shutdown the engine with an error message. Otherwise they are just logged like any other error.
  VBASE_IMPEXP void EnableEngineShutdownOnFatalError(bool bFatalErrorsShouldReallyBeFatal);

  /// \brief Allows to switch the default handler for fatal errors.
  ///
  /// This feature is deprecated.
  /// \sa EnableEngineShutdownOnFatalError
  ///
  /// \param cb
  ///   The function to call when a fatal error is logged.
  VBASE_IMPEXP void SetOnFatalErrorCallback(ON_FATALERROR_CALLBACK cb) { m_OnFatalErrorCallback = cb; }

private:
  static VISION_THREADLOCAL_DECL(hkvLogBlock*, s_pCurrentLogBlock);

  VBASE_IMPEXP virtual hkvLogBlock* GetCurrentLogBlock() const HKV_OVERRIDE
  {
    return s_pCurrentLogBlock;
  }

  VBASE_IMPEXP virtual void SetCurrentLogBlock(hkvLogBlock* pCurrentLogBlock) HKV_OVERRIDE
  {
    s_pCurrentLogBlock = pCurrentLogBlock;
  }

  static VScopedPtr<hkvGlobalLog> g_spInstance;

  struct LogWriterData
  {
    LogWriter m_Callback;
    void* m_pPassThrough;
  };

  struct MessageFilterData
  {
    MessageFilter m_Callback;
    void* m_pPassThrough;
  };

  ON_FATALERROR_CALLBACK m_OnFatalErrorCallback;
  int m_iShutdownOnFatalError;
  VArray<LogWriterData> m_LogWriterArray;
  VArray<MessageFilterData> m_MessageFilterArray;
};


/// \brief Log blocks are used for structured, well formatted logging output. Use the HKV_LOG_BLOCK macro to create them more comfortably.
///
/// By creating instances of hkvLogBlock all following messages will get grouped and indented. Once the lifetime of the log block ends,
/// the logging group is closed and the indentation is reverted for following messages.
/// Log blocks are never written directly to the log, but only once a message is written (and makes it through the log level filter).
/// So you can create a vast number of log blocks, even in frequently executed code, but will not get any unnecessary output in the log,
/// unless some message is actually written.
/// The creation of log blocks is light weight. Whether it is thread-safe depends on the logging system that the log block is connected to.
/// With hkvGlobalLog thread local variables are used and thus output is thread safe. Other logging systems will not be thread safe, unless
/// they re-implement hkvLogInterface::GetCurrentLogBlock differently.
class hkvLogBlock
{
public:
  /// \brief Creates a log block that will group messages in hkvGlobalLog.
  ///
  /// \param szBlockName
  ///   The name of the block, will be passed as szText to the hkvLogInterface::HandleLogMessage
  ///   szBlockName should be a constant string.
  /// \param szExtraInfo
  ///   Additional (optional) context information. This can be used to pass a file name or some other string along.\n
  ///   Usage example: HKV_LOG_BLOCK("Loading Texture" , szFilename);\n
  ///   Note szExtraInfo must be available throughout the whole lifetime of the log block, as it might only get accessed
  ///   on destruction of the log block. Prefer to pass in strings that are available anyway and do not need to get created only
  ///   for the log block.
  /// \param bWriteAlways
  ///   If set to true, the log block text will always be output, regardless of whether any logging message is written while it is active.
  ///   If set to false (the default), the log block text will only be output, if any logging message follows (and is not filtered out)
  ///   while the log block is active, thus reducing unnecessary logging output.
  VBASE_IMPEXP hkvLogBlock(const char* szBlockName, const char* szExtraInfo = "", bool bWriteAlways = false);

  /// \brief Same as the other constructor, only that instead of outputting to hkvGlobalLog, one can specify to which log interface to write.
  VBASE_IMPEXP hkvLogBlock(hkvLogInterface* pLocalLog, const char* szBlockName, const char* szExtraInfo = "", bool bWriteAlways = false);

  /// \brief The destructor will close the log block and, if necessary, send the 'EndBlock' message to the log interface.
  VBASE_IMPEXP ~hkvLogBlock();

private:
  friend class hkvLog;

  // the block that was active before us
  hkvLogBlock* m_pParentBlock;

  // the interface to which to send the messages to
  hkvLogInterface* m_pLogInterface;

  // our own name, needs to be valid memory throughout the whole lifetime of the block
  const char* m_szBlockName;

  // additional context information, needs to be valid memory throughout the whole lifetime of the block
  const char* m_szExtraInfo;

  // whether the block header was written; it will be written only once, and if it was, it will be written again on closure
  bool m_bWritten;

  // the parent lock indentation plus one, will be computed when the block is written
  short m_iIndentation;
};


/// \brief This is a helper class to temporarily filter out certain log messages
class hkvScopedLogFilter
{
public:

  /// \brief Adds a log filter on hkvGlobalLog, that will filter out all messages that contain the given text.
  hkvScopedLogFilter(const char* szMessageContains) : m_szMessageContains(szMessageContains)
  {
    m_iFilteredMessages = 0;
    hkvGlobalLog::GetInstance()->AddCustomMessageFilter(MessageFilter, this);
  }

  /// \brief Resets the log filter.
  ~hkvScopedLogFilter()
  {
    hkvGlobalLog::GetInstance()->RemoveCustomMessageFilter(MessageFilter, this);
  }

  /// \brief Returns the number of messages that were filtered out by this filter.
  /// Can be used to detect whether an expected error message arrived.
  int GetNumFilteredMessages() const { return m_iFilteredMessages; }
  

private:
  static bool MessageFilter(hkvLogMsgType::Enum MsgType, const char* szText, const char* szTag, void* pPassThrough)
  {
    hkvScopedLogFilter* pFilter = (hkvScopedLogFilter*)pPassThrough;

    // text not found -> let it pass
    if (strstr(szText, pFilter->m_szMessageContains) == NULL)
      return true;

    pFilter->m_iFilteredMessages++;
    return false;
  }

  const char* m_szMessageContains;
  int m_iFilteredMessages;
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
