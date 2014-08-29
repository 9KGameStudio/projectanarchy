/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VGL.hpp

#ifndef _VGL_HPP
#define _VGL_HPP

/// \brief Class containing various helper functions for applications.
class VBASE_IMPEXP VBaseAppHelpers
{
public:

  /// \brief
  ///   Retrieves the directory of the currently running application
  ///
  /// \param pszAppDir
  ///   Application directory.
  /// 
  /// \return
  ///   Whether the function succeeded
  static bool GetApplicationDir(char *pszAppDir);

  /// \brief
  ///   Retrieves a platform dependent non-unique human readable identifier of the machine or device the application is running on.
  static void GetDeviceName(char* buffer, int maxLength);

  /// \brief
  ///   Retrieves a platform dependent non-unique human readable identifier of the machine or device the application is running on.
  template<int N> static void GetDeviceName(char (&buffer)[N])
  {
    return GetDeviceName(buffer, N);
  }

  /// \brief
  ///   Helper function to install a file system that allows access to the Havok SDK.
  ///
  /// This function is intended to be used in applications that run in the context of the Havok
  /// SDK, and use the standard base data and possibly other data directories within the SDK.
  ///
  /// It should not be used in applications that do not share the directory layout of the
  /// Havok SDK.
  ///
  /// \param szName
  ///   name of the file system to install; default: 'havok_sdk'
  /// \param bWritable
  ///   whether the SDK file system should be writable; default: false
  /// \return
  ///   whether the file system was added successfully
  static hkvResult InstallSDKFileSystem(const char* szName = "havok_sdk", bool bWritable = false)
  {
    return InstallSDKFileSystem(*VFileAccessManager::GetInstance(), szName, bWritable);
  }

  /// \brief
  ///   Helper function to install a file system that allows access to the Havok SDK.
  ///
  /// This function is intended to be used in applications that run in the context of the Havok
  /// SDK, and use the standard base data and possibly other data directories within the SDK.
  ///
  /// It should not be used in applications that do not share the directory layout of the
  /// Havok SDK.
  ///
  /// \param fileAccessManager
  ///   the file access manager in which to install the SDK file system
  /// \param szName
  ///   name of the file system to install; default: 'havok_sdk'
  /// \param bWritable
  ///   whether the SDK file system should be writable; default: false
  /// \return
  ///   whether the file system was added successfully
  static hkvResult InstallSDKFileSystem(VFileAccessManager& fileAccessManager, const char* szName = "havok_sdk", bool bWritable = false);

  /// \brief
  ///   Searches for a vForge workspace marker upwards from (and including) the passed
  ///   project directory.
  /// \param szProjectDir
  ///   The project dir in which to start the search.
  /// \param szWorkspaceRoot
  ///   The name of the root under which the workspace file system is going to be installed. 
  ///   This is used for building the project search path (which always lies within this
  ///   file system).
  /// \param sWorkspaceDir
  ///   The buffer into which to write the workspace directory. Only valid if the function succeeds.
  /// \param sProjectSearchPath
  ///   The buffer into which to write the project search path. Only valid if the function succeeds.
  /// \return
  ///   Whether the vForge workspace for the passed project could be successfully found.
  static hkvResult FindVForgeWorkspace(const char* szProjectDir, const char* szWorkspaceRoot,
    VStaticString<FS_MAX_PATH>& sWorkspaceDir, VStaticString<FS_MAX_PATH>& sProjectSearchPath);
};


#ifndef _VISION_DOC

//
// The Vision General Library
//
// Now integrated into VBase

#if defined(_VISION_WIN32) && !defined(_VISION_WINRT)

class VVideoConfig;
void SetVGLWindowParameters(VVideoConfig &vc);

/// \brief Converts a HRESULT to a human readable error string. Supply a VString or VStaticString for stringBuffer.
template<typename T>
const char* VErrorMessageFromHresult(HRESULT result, T& stringBuffer)
{
  LPVOID lpMsgBuf = NULL;
  FormatMessageW(
    FORMAT_MESSAGE_ALLOCATE_BUFFER |
    FORMAT_MESSAGE_FROM_SYSTEM |
    FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL,
    result,
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    (LPWSTR)&lpMsgBuf,
    0, NULL);

  VString string;

  if(lpMsgBuf != NULL)
  {
    char errorBuffer[4096];
    V_WCHAR_TO_UTF8((LPWSTR)lpMsgBuf, errorBuffer, V_ARRAY_SIZE(errorBuffer));

    stringBuffer.Format("(0x%08X) %s", result, errorBuffer);
    LocalFree(lpMsgBuf);
  }
  else
  {
    stringBuffer.Format("0x%08X", result);
  }

  return stringBuffer;
}

#endif

#define VGL_WINDOWSINPUT  1
#define VGL_DIRECTINPUT   2

#define VGL_DEFAULTINPUT  (VGL_DIRECTINPUT)

/*
VGL_WINDOWSINPUT - Windows input notes:

When using Windows input there are some limitiations because Windows
still interprets some keys specially:
-ALT
-F10
-F12 (while in debugger causes a kernal breakpoint)
-PrintScreen 
*/

#define VERR_VGLALREADYINITIALIZED -19001



class VResourceManager;
class VConnection;

//
//                                  ### STRUCTS ###
//

/// \brief
///   Struct for mouse data
struct VGLMouse_t
{
  SLONG deltaX;         ///< Difference in X position since last call
  SLONG deltaY;         ///< Difference in Y position since last call
  
  SLONG buttonL;        ///< TRUE if left mouse button is pressed
  SLONG buttonM;        ///< TRUE if middle mouse button is pressed
  SLONG buttonR;        ///< TRUE if right mouse button is pressed
  
  SLONG wheelUp;        ///< Mouse wheel up movement      (notice that both up and down movement can happen at once)
  SLONG wheelDown;      ///< Mouse wheel down movement    (notice that both up and down movement can happen at once) 
  
  SLONG buttonX0;       ///< TRUE if first extra button is pressed
  SLONG buttonX1;       ///< TRUE if second extra button is pressed
  
  SSHORT cursorX;       ///< Client window cursor position for GUI. Windows: From cursor, Console: accumulated from deltas
  SSHORT cursorY;       ///< Client window cursor position for GUI. Windows: From cursor, Console: accumulated from deltas

};


/// \brief
///   Languages that the keyboard layout can be
enum VGLLanguage
{
 VGL_LANG_ENGLISH,
 VGL_LANG_GERMAN,
 VGL_LANG_FRENCH
};


/// \brief
///   Struct for keys and key recognition
struct VGLKey_t
{
  SLONG vglKey;
  char  m_chLower; ///<lower case character
  char  m_chUpper; ///<upper case character
  const char* m_pszName;
};


///
/// @name Main Functions
/// @{
///


#if defined(_VISION_WIN32) && !defined(_VISION_WINRT)

/// \brief
/// deprecated. use VGLInitialize with window config.
VBASE_IMPEXP RETVAL VGLInitialize();

/// \brief
///   Initialize VGL
/// 
/// VGLInitialize initializes does the one time setup for windowing and input
///
/// \param windowConfig
///   Windows Config(window specific, other it should be null). 
///   It contains window specific information. e.g) custom window procedure, window creation flags.
/// 
/// \return
///   RETVAL
/// 
/// \errcodes
///   \li \c VERR_NOERROR:  Everything is fine and dandy
/// 
///   \li \c VERR_NOWINDOW:  Could not register window class
/// 
///   \li \c VERR_NOOPENGL:  Could not load OpenGL32.dll
/// 
///   \li \c VERR_NOD3D:     Could not load d3d9.dll
/// 
///   \li \c VERR_VGLALREADYINITIALIZED:  Vgl is already initialized
/// 
/// \note
///   If window size in window mode exceeds the boundaries of the screen, the window will not get
///   correctly sized. If you need higher resolutions, consider doing it in full screen mode
///   instead.
VBASE_IMPEXP RETVAL VGLInitialize(const VWindowConfig& windowConfig);

#elif  defined(_VISION_XENON) || defined(_VISION_PS3) || defined(_VISION_LINUX) || defined(_VISION_WINRT)

/// \brief
///   Initialize VGL
/// 
/// VGLInitialize initializes does the one time setup for windowing and input
/// 
/// \return
///   RETVAL
/// 
/// \errcodes
///   \li \c VERR_NOERROR:  Everything is fine and dandy
/// 
///   \li \c VERR_NOWINDOW:  Could not register window class
/// 
///   \li \c VERR_NOOPENGL:  Could not load OpenGL32.dll
/// 
///   \li \c VERR_NOD3D:     Could not load d3d8.dll
/// 
///   \li \c VERR_VGLALREADYINITIALIZED:  Vgl is already initialized
VBASE_IMPEXP RETVAL VGLInitialize();

#endif


/// \brief
///   DeInitialize VGL
/// 
/// VGLDeInitialize will take care of the deinitialization of VGL, when the program quits. It will
/// shut down OpenGL and related contexts, restore the screen, keyboard and mouse.
/// 
/// \note
///   Always call this function last, but preferrably make sure that the mouse and keyboard are
///   deinitialized first. 
VBASE_IMPEXP void VGLDeInitialize(void);

/// \brief
///   Runs the Windows message pump and updates mouse and keyboard status
/// 
/// \errcodes
///   \li \c VERR_NOERROR:  No problem, continue as normal
/// 
///   \li \c VERR_QUIT:     The application has received the WM_QUIT message and should be
///     terminated immediately. 
/// 
/// \note
///   This function should be called _at least_ once per game simulation loop.
VBASE_IMPEXP RETVAL VGLProcessMessage();

/// \brief
///   Runs the Windows message pump
/// 
/// \return
///   false if the application wants to quit
/// 
/// \note
///   This function should be called _at least_ once per game simulation loop.
VBASE_IMPEXP BOOL VGLRunMessagePump();

#ifdef SUPPORTS_MOUSE

///
/// @}
///

///
/// @name Mouse Functions
/// @{
///

/// \brief
///   Initialize mouse 
/// 
/// VGLInitializeMouse will select the proper way of getting mouse input, make sure that the mouse
/// is properly initialized for the set screen mode, and make the mouse cursor invisible
/// 
///
/// \param bHideCursor
///   Whether the cursor will be hidden
///
/// \param bExclusive
///   If exclusive mode is required. 
///   Relevant for DirectInput and will hide the cursor
///
/// \return
///   RETVAL
/// 
/// \errcodes
///   \li \c VERR_NOERROR:   Everything is OK
/// 
///   \li \c VERR_NOWINDOW:  There was no window with which the mouse could be associated
/// 
/// \note
///   Only call this after the display has been initialized
VBASE_IMPEXP RETVAL VGLInitializeMouse(bool bHideCursor = true, bool bExclusive = true);


/// \brief
///   Deinitialize mouse
/// 
/// VGLDeInitializeMouse releases the resources needed for mouse handling and displays the mouse
/// cursor again
/// 
/// \return
///   RETVAL
/// 
/// \errcodes
///   \li \c VGL_NOERROR:   Everything is OK
/// 
///   \li \c VGL_NOWINDOW:  Ther was no window with which the mouse could have been associated.
/// 
/// \note
///   Call this before calling VGLRestoreScreen
VBASE_IMPEXP RETVAL VGLDeInitializeMouse(void);



/// \brief
///   Is mouse initialized
/// 
/// Inquires whether the mouse is initialized by VGL
/// 
/// \return
///   RETVAL
///   \li TRUE if initialized
/// 
///   \li FALSE if not
VBASE_IMPEXP RETVAL VGLIsMouseInit(void);

/// \brief
///   Sets the new on-screen position of the mouse
VBASE_IMPEXP void VGLSetMousePosition(int x, int y);


/// \brief
///   Get mouse data
/// 
/// VGLGetMouse will summarize all the mouse data, movements and button presses that have occurred
/// since the last call and return the logical movement and status of the mouse. Use this to get
/// the data and coordinates that you want.
/// 
/// \param pData
///   Pointer to a struct to be filled in with the data.
/// 
VBASE_IMPEXP void VGLGetMouse(VGLMouse_t *pData);



/// \brief
///   Disable mouse handling
/// 
/// VGLDisableMouse will disable the mouse handling of VGL, for example if you want to do something
/// else.
VBASE_IMPEXP void VGLDisableMouse(void);
 


/// \brief
///   Enables mouse handling
/// 
/// VGLEnableMouse will enable the mouse handling again after having been disabled by
/// VGLDisableMouse.
VBASE_IMPEXP void VGLEnableMouse(void);


#endif

#ifdef SUPPORTS_KEYBOARD

///
/// @}
///

///
/// @name Keyboard Functions
/// @{
///
VBASE_IMPEXP RETVAL VGLInitializeKeyboard(SLONG method = VGL_DEFAULTINPUT, bool bExclusive = true);

/// \brief
///   Deinitialize keyboard
/// 
/// VGLDeInitializeKeyboard releases the resources needed for keyboard handling
/// 
/// \return
///   RETVAL
/// 
/// \errcodes
///   \li \c VGL_NOERROR:   Everything is fine
/// 
///   \li \c VGL_NOWINDOW:  There was no window with which the keyboard could have been associated.
/// 
/// \note
///   Call this before calling VGLRestoreScreen
VBASE_IMPEXP RETVAL VGLDeInitializeKeyboard(void);



/// \brief
///   Is keyboard initialized
/// 
/// Inquires whether the keyboard is initialized by VGL
/// 
/// \return
///   RETVAL
///   \li TRUE if initialized,
/// 
///   \li FALSE if not
VBASE_IMPEXP RETVAL VGLIsKeyboardInit(void);



/// \brief
///   Get keyboard input method
/// 
/// This function will return the method used by VGL to handle the keyboard status and key
/// presses.
/// 
/// \return
///   RETVAL
///   \li VGL_DIRECTINPUT   if DirectInput is used for keyboard input
/// 
///   \li VGL_WINDOWSINPUT  if Windows standard functions are used for keyboard input
/// 
/// \errcodes
///   \li \c VERR_DEVICENOTINIT:  The keyboard was not at all initialized
VBASE_IMPEXP RETVAL VGLGetKeyboardMethod(void);



/// \brief
///   See whether any key is pressed
/// 
/// This functions returns the status of any key being pressed
/// 
/// \return
///   RETVAL: TRUE if any key is pressed, FALSE if not
VBASE_IMPEXP RETVAL VGLIsAnyKeyPressed(void);


/// \brief
///   This is the scan code version of VGLIsAnyKeyPressed. Requires DirectInput mode
VBASE_IMPEXP RETVAL VGLIsAnyKeyPressed_SC(void);


/// \brief
///   Find out whether a key is pressed
/// 
/// VGLIsKeyPressed returns the (logical) status of a key; whether it is pressed or not.
/// 
/// \param key
///   The key, the status of which is to be retrieved.
/// 
/// \return
///   RETVAL
///   \li TRUE if key is pressed,
/// 
///   \li FALSE if not
/// 
/// \errcodes
///   \li \c VERR_NOERROR:       Successful
/// 
///   \li \c VERR_ILLEGALVALUE:  You sent an invalid key value (<0 or >255)
/// 
/// \note
///   Will reset the key to logical unpressed status if it is registered with the VGLKeyOnce
///   function
VBASE_IMPEXP RETVAL VGLIsKeyPressed(SLONG key);


/// \brief
///   Find out whether a key is pressing
/// 
/// VGLIsKeyDownBitSet returns the (logical) status of a key; whether it is pressing or not.
/// 
/// \param key
///   The key, the status of which is to be retrieved.
/// 
/// \return
///   RETVAL
///   \li TRUE if key is pressing,
/// 
///   \li FALSE if not
/// 
/// \errcodes
///   \li \c VERR_NOERROR:       Successful
/// 
///   \li \c VERR_ILLEGALVALUE:  You sent an invalid key value (<0 or >255)
/// 
/// \note
///   Will 'NOT' reset the key to logical unpressed status if it is registered with the VGLKeyOnce
///   function
VBASE_IMPEXP RETVAL VGLIsKeyDownBitSet(SLONG key);



/// \brief
///   This is the scan code version of VGLIsKeyPressed. Requires DirectInput mode
VBASE_IMPEXP RETVAL VGLIsKeyPressed_SC(SLONG key);



/// \brief
///   Set (or reset) KeyOnce status of a key
/// 
/// VGLKeyOnce will make sure that a certain key will only be recognized as pressed one time, and
/// cannot be held down. It can also undo this state.
/// 
/// \param key
///   Key code of the key
/// 
/// \param oper
///   \li TRUE if KeyOnce is set (only recognized once per press)
/// 
///   \li FALSE if KeyOnce is not set (always recognized when pressed)
/// 
/// \return
///   RETVAL
/// 
/// \errcodes
///   \li \c VERR_NOERROR:       Successful
/// 
///   \li \c VERR_ILLEGALVALUE:  You sent an invalid key value (<0 or >255)
VBASE_IMPEXP RETVAL VGLKeyOnce(SLONG key, SLONG oper);

/// \brief
///   This is the scancode version of VGLKeyOnce. Requires DirectInput mode
VBASE_IMPEXP RETVAL VGLKeyOnce_SC(SLONG key, SLONG oper);



/// \brief
///   Find out wehther a key is KeyOnce
/// 
/// VGLIsKeyOnce will find out whether a key is KeyOnce or not. For reference, see VGLKeyOnce
/// 
/// \param key
///   Key code of the key
/// 
/// \return
///   RETVAL
///   \li TRUE if KeyOnce,
/// 
///   \li FALSE if not
/// 
/// \errcodes
///   \li \c VERR_ILLEGALVALUE:  You sent an invalid key value (<0 or >255)
VBASE_IMPEXP RETVAL VGLIsKeyOnce(SLONG key);

/// \brief
///   This is the scancode version of VGLIsKeyOnce. Requires DirectInput mode
VBASE_IMPEXP RETVAL VGLIsKeyOnce_SC(SLONG key);


/// \brief
///   Returns a pointer to an array of 256 key entries
/// 
/// The key entries can be used to translate virtual keys into printable key names and characters
/// ex  pTable[VGLK_ESC].m_pszName gives you the string "Esc"
/// 
/// \return
///   Pointer to a VGLKey_t[256] array
VBASE_IMPEXP VGLKey_t *VGLGetKeyCharMap();

//TODO: Deprecate
VBASE_IMPEXP const VGLKey_t *VGLGetKeyTable();

/// \brief
///   Returns a pointer to a table that maps Direct Input keys to our virtual keys
/// 
/// The key entries can be used to translate Direct Input keys into virtual keys ex.
/// pTable[DIK_ESCAPE] = VGLK_ESC
/// 
/// \return
///   Pointer to a int[256] array
VBASE_IMPEXP int *VGLGetKeyTable_DI();


/// \brief
///   Returns a pointer to a table that maps Windows keys to our virtual keys
/// 
/// The key entries can be used to translate Windows keys into virtual keys ex.  pTable[VK_ESCAPE]
/// = VGLK_ESC
/// 
/// \return
///   Pointer to a int[256] array
VBASE_IMPEXP int *VGLGetKeyTable_WI();

/// \brief
///   Returns a pointer to a table that maps extended Windows keys to our virtual keys
/// 
/// The key entries can be used to translate extended Windows keys into virtual keys ex.
/// pTable[VK_INSERT] = VGLK_INS;
/// 
/// \return
///   Pointer to a int[256] array
VBASE_IMPEXP int *VGLGetKeyTableExt_WI();


/// \brief
///   Returns the current language that the keyboard layout uses
/// 
/// If the language is unrecognized, it will default to English
/// 
/// \return
///   Language identifier
VBASE_IMPEXP VGLLanguage VGLGetKeyBoardLanguage();

#endif

///
/// @}
///

///
/// @name Log Window Functions
/// @{
///

/// \brief
///   Updates a list of resource manager in the resource viewer
/// 
/// \param pManagerList
///   list of pointers to resource manager that should be updated
/// 
/// \param iListCount
///   number of resource manager in the list
/// 
/// \param pRemoteConnection
///   if a remote connection exists it can be supplied here
///
/// \return
///   BOOL bResult: TRUE, if the update was successful
VBASE_IMPEXP BOOL VGLUpdateResourceViewer(const VResourceManager **pManagerList, int iListCount, VConnection* pRemoteConnection = NULL);

/// \brief
///   Returns if a resource viewer is attached over a HWND handle
VBASE_IMPEXP bool VGLIsResourceViewerConnnected();


///
/// @}
///

#define VGL_TIMER_MODE_INVALID      (0)
#define VGL_TIMER_MODE_PERFORMANCE  (1)
#define VGL_TIMER_MODE_SAFETY       (2)

/// \brief
///   You can use this function to set the accuracy mode the timer uses
/// 
/// You can use this function to set the accuracy the timer uses.
/// 
/// The timer will automatically switch to safety mode during initialization if multiple CPUs are
/// detected.
/// 
/// Can only be called after the engine has been initialized
/// 
/// \param iAccuracy
///   The current timer accuracy mode VGL_TIMER_MODE_PERFORMANCE  : Higher performance mode
///   VGL_TIMER_MODE_SAFETY       : Higher safety mode Automatically used on multi-CPU machines
/// 
/// \return
///   bool: whether setting the accuracy was successful
VBASE_IMPEXP bool VGLSetTimerAccuracy(int iAccuracy);

/// \brief
///   You can use this function to retrieve the current accuracy mode of the timer
/// 
/// Can only be called after the engine has been initialized
/// 
/// \return
///   int: The current timer accuracy mode VGL_TIMER_MODE_INVALID      : Not initialized yet
///   VGL_TIMER_MODE_PERFORMANCE  : Higher performance mode VGL_TIMER_MODE_SAFETY       : Higher
///   safety mode Automatically used on multi-CPU machines
VBASE_IMPEXP int VGLGetTimerAccuracy();

// todo: documentation
VBASE_IMPEXP double VGLGetCurrentTimeStamp();


/// \brief
///   Get timer resolution
/// 
/// VGLGetTimerResolution gets the timer value count per second (e.g. 1000000 for microseconds,
/// etc.)
/// 
/// \return
///   uint64: Resolution of the timer used by VGLGetTimer and VGLSetTimer
VBASE_IMPEXP uint64 VGLGetTimerResolution();


/// \brief
///   Get timer value
/// 
/// VGLGetTimer will get the current VGL timer value
/// 
/// \return
///   uint64: Time since VGLSetTimer was called (in seconds * VGLGetTimerResolution())
/// 
/// \note
///   If VGLSetTimer is never called, this function will return the elapsed time since the
///   initialization of VGL
VBASE_IMPEXP uint64 VGLGetTimer();


/// \brief
///   Set timer value
/// 
/// VGLGetTimer will set the current VGL timer value
/// 
/// \param timeValue
///   Starting count of the timer (in seconds * VGLGetTimerResolution())
/// 
/// \note
///   This function is automatically called with the parameter 0 when VGL is initialized. Use
///   VGLGetTimer to get the time.
VBASE_IMPEXP void VGLSetTimer(uint64 timeValue);

#define VQB_HANDLE_WINDOW_CLOSE  1  ///< The window will be closed if the user clicks on the X buton (ALT-F4). If not specified the application has to check VGLWantClose manually
#define VQB_QUIT_ON_CLOSE        2  ///< The application will quit if the window is closed
#define VQB_DEFAULT (VQB_HANDLE_WINDOW_CLOSE)

/// \brief
///   Sets the current behaviour for quiting the application when the Window is closed.
/// 
/// Has no effect on consoles.
/// 
/// \param iFlags
///   Flags specifying current behaviour
///   \li VQB_HANDLE_WINDOW_CLOSE  - The window will be closed if the user clicks in the corner. If
///     not specified the application has to check VGLWantClose manually
/// 
///   \li VQB_QUIT_ON_CLOSE        - The application will quit if the window is closed
/// 
///   \li VQB_DEFAULT (VQB_HANDLE_WINDOW_CLOSE) 
/// 
/// \return
///   Nothing.
/// 
/// \note
///   This is useful for instance when you don't want to quit the user application when the window
///   is closed.
/// 
/// \sa VGLGetQuitBehaviour
/// 
/// \example
///   \code
///   Some usage examples:
///   -Standalone applications use VQB_DEFAULT | VQB_QUIT_ON_CLOSE
///   -Exporter preview windows use VQB_DEFAULT
///   -The engine uses 0 because it checks for the window close using VGLWantClose
///   \endcode
VBASE_IMPEXP void VGLSetQuitBehaviour(int iFlags);

/// \brief
///   Returns the current behaviour for quiting the application when the Window is closed.
/// 
/// Has no effect on consoles.
/// 
/// \return
///   Flags specifying current behaviour
/// 
/// \note
///   This is useful for instance when you don't want to quit the user application when the window
///   is closed.
/// 
/// \sa VGLSetQuitBehaviour
VBASE_IMPEXP int  VGLGetQuitBehaviour();

///
/// @}
///

///
/// @name Vgl Key Codes
/// @{
///

// Generic
#define VGLK_UNKNOWN    0 ///< Unknown key

// Control keys
#define VGLK_LSHIFT     2 ///< Left shift (or shift, if no right shift exists)
#define VGLK_RSHIFT     3 ///< Right shift
#define VGLK_LCTRL      4 ///< Left control (or control, if no right control exists)
#define VGLK_RCTRL      5 ///< Right control

#define VGLK_LALT       6 ///< Left alt (or alt, if no right alt exists)
#define VGLK_RALT       7 ///< Right Alt
#define VGLK_ALTGR      7 ///< AltGr (Right Alt on German keyboards)

#define VGLK_BACKSP     8 ///< Backspace (ASCII compliant)
#define VGLK_TAB        9 ///< Tab (ASCII compliant)

#define VGLK_LWIN      10 ///< Left Windows key
#define VGLK_RWIN      11 ///< Right Windows key
#define VGLK_APPS      12 ///< Windows menu (application) key  //Used to be called VGLK_MENU incorrectly
#define VGLK_ENTER     13 ///< Guess what :) (ASCII compliant)
#define VGLK_LCOM      14 ///< Left command (Mac)
#define VGLK_RCOM      15 ///< Right command (Mac)
#define VGLK_MAC       16 ///< Special mac key (will come later :)
#define VGLK_LMETA     17 ///< Left meta key
#define VGLK_RMETA     18 ///< Right meta key

// Non-numerical edit and cursor movement keys
#define VGLK_INS       21 ///< Insert
#define VGLK_DEL       22 ///< Delete
#define VGLK_HOME      23 ///< Home
#define VGLK_END       24 ///< End
#define VGLK_PGUP      25 ///< PageUp
#define VGLK_PGDN      26 ///< PageDown

#define VGLK_ESC       27 ///< Escape (ASCII compliant)

#define VGLK_UP        28 ///< Up key
#define VGLK_DOWN      29 ///< Down key
#define VGLK_LEFT      30 ///< Left key
#define VGLK_RIGHT     31 ///< Right key

#define VGLK_SPACE     32 ///< Space bar (ASCII compliant)

//////////////////////////////////////////////////////////////////////////
//The following keys might not be available on all keyboard layouts:
// (They work on US and German layouts)
//////////////////////////////////////////////////////////////////////////

#define VGLK_COMMA  33 ///< Comma "," on american keyboard
#define VGLK_PERIOD 34 ///< Period "." on american keyboard
#define VGLK_MINUS  35 ///< Minus on american keyboard

#define VGLK_GRAVE  36 ///< Grave ` on american keyboard (same key as tilde)

/////////////////////////////////////////////
// Standard number codes comply with ASCII codes
/////////////////////////////////////////////
#define VGLK_0         48
#define VGLK_1         49
#define VGLK_2         50
#define VGLK_3         51
#define VGLK_4         52
#define VGLK_5         53
#define VGLK_6         54
#define VGLK_7         55
#define VGLK_8         56
#define VGLK_9         57

/////////////////////////////////////////////
// Special keys
/////////////////////////////////////////////
#define VGLK_CAPS      58 ///< Caps Lock
#define VGLK_SCROLL    59 ///< Scroll Lock
#define VGLK_NUM       60 ///< Num Lock
#define VGLK_PRSCR     61 ///< Print screen
#define VGLK_PAUSE     62 ///< Pause key

/////////////////////////////////////////////
// Standard letter codes comply with ASCII codes
/////////////////////////////////////////////
#define VGLK_A         65
#define VGLK_B         66
#define VGLK_C         67
#define VGLK_D         68
#define VGLK_E         69
#define VGLK_F         70
#define VGLK_G         71
#define VGLK_H         72
#define VGLK_I         73
#define VGLK_J         74
#define VGLK_K         75
#define VGLK_L         76
#define VGLK_M         77
#define VGLK_N         78
#define VGLK_O         79
#define VGLK_P         80
#define VGLK_Q         81
#define VGLK_R         82
#define VGLK_S         83
#define VGLK_T         84
#define VGLK_U         85
#define VGLK_V         86
#define VGLK_W         87
#define VGLK_X         88
#define VGLK_Y         89
#define VGLK_Z         90

/////////////////////////////////////////////////////////////////////
// Numeric keypad keys. Some of these keys are doubled for your leisure :)
/////////////////////////////////////////////////////////////////////
#define VGLK_KP_SLASH  91 ///< Slash (or divide) key on the numeric keypad
#define VGLK_KP_MUL    92 ///< Multiply (or asterisk) key on the numeric keypad
#define VGLK_KP_MINUS  93 ///< Minus on the numeric keypad
#define VGLK_KP_PLUS   94 ///< Plus...
#define VGLK_KP_ENTER  95 
#define VGLK_KP_PERIOD 96 ///< Period (or comma or del) on the numeric keypad
#define VGLK_KP_DEL    96
#define VGLK_KP_0      97 ///< Insert or 0
#define VGLK_KP_INS    97
#define VGLK_KP_1      98 ///< End or 1
#define VGLK_KP_END    98
#define VGLK_KP_2      99 ///< Down or 2
#define VGLK_KP_DOWN   99
#define VGLK_KP_3     100 ///< PgDn or 3
#define VGLK_KP_PGDN  100
#define VGLK_KP_4     101 ///< Left or 4
#define VGLK_KP_LEFT  101
#define VGLK_KP_5     102 ///< Keypad 5
#define VGLK_KP_6     103 ///< Right or 6
#define VGLK_KP_RIGHT 103
#define VGLK_KP_7     104 ///< Home or 7
#define VGLK_KP_HOME  104
#define VGLK_KP_8     105 ///< Up or 8
#define VGLK_KP_UP    105
#define VGLK_KP_9     106 ///< PgUp or 9
#define VGLK_KP_PGUP  106

//////////////////////
// Function keys
//////////////////////
#define VGLK_F1       110
#define VGLK_F2       111
#define VGLK_F3       112
#define VGLK_F4       113
#define VGLK_F5       114
#define VGLK_F6       115
#define VGLK_F7       116
#define VGLK_F8       117
#define VGLK_F9       118
#define VGLK_F10      119
#define VGLK_F11      120
#define VGLK_F12      121


////////////////////////////////
// Keys only available on specific layouts
////////////////////////////////

//US (and English)
#define VGLK_EQUAL    130 ///< Equal "=" on american keyboard
#define VGLK_LSQBRK   131 ///< Left square bracket "[" on american keyboard
#define VGLK_RSQBRK   132 ///< Right square bracket "]" on american keyboard
#define VGLK_SEMICL   133 ///< Semicolon ";" on american keyboard
#define VGLK_APOSTR   134 ///< Apostrophe key "'" on american keyboard
#define VGLK_BACKSL   135 ///< Backslash "\" on american keyboard
#define VGLK_SLASH    136 ///< Slash "/" on american keyboard


//German keyboard
#define VGLK_DE_SS     140 ///< Double-S (or Beta) on german keyboard
#define VGLK_DE_ACCENT 141 ///< Accent key on german keyboard
#define VGLK_DE_UE     142 ///< U with umlaut on german keyboard
#define VGLK_DE_PLUS   143 ///< Plus on german keyboard
#define VGLK_DE_OE     144 ///< O with umlaut on german keyboard
#define VGLK_DE_AE     145 ///< A with umlaut on german keyboard
#define VGLK_DE_HASH   146 ///< Hash "#" on german keyboard
#define VGLK_DE_LT     147 ///< Less than "<" key on german keyboard
#define VGLK_DE_CIRC   148 ///< Circumflex "^"on german keyboard

//French keyboard
#define VGLK_FR_UGRAVE 149 ///< U with grave
#define VGLK_FR_MULTIPLY 150 ///< multiply
#define VGLK_FR_COMMA  151
#define VGLK_FR_SEMICL 152
#define VGLK_FR_COLON  153
#define VGLK_FR_SS   154 ///< Double-S
#define VGLK_FR_RBRACKET 155
#define VGLK_FR_EQUAL   156
#define VGLK_FR_DOLL    157 ///< '$'

/////////////////////////////////////////////
// OLD keys for source backwards compatibility:
/////////////////////////////////////////////

#define VGLK_TILDE  VGLK_GRAVE     ///< Tilde ~ is just here because some people call the grave key the tilde key

//US
#define VGLK_US_EQUAL  VGLK_EQUAL  ///< Equal "=" on american keyboard
#define VGLK_US_LSQBRK VGLK_LSQBRK ///< Left square bracket "[" on american keyboard
#define VGLK_US_RSQBRK VGLK_RSQBRK ///< Right square bracket "]" on american keyboard
#define VGLK_US_SEMICL VGLK_SEMICL ///< Semicolon ";" on american keyboard
#define VGLK_US_APOSTR VGLK_APOSTR ///< Apostrophe key "'" on american keyboard
#define VGLK_US_BACKSL VGLK_BACKSL ///< Backslash "\" on american keyboard
#define VGLK_US_SLASH  VGLK_SLASH  ///< Slash "/" on american keyboard

#define VGLK_US_COMMA  VGLK_COMMA  ///< Comma "," on american keyboard
#define VGLK_US_PERIOD VGLK_PERIOD ///< Period "." on american keyboard
#define VGLK_US_MINUS  VGLK_MINUS  ///< Minus on american keyboard

#define VGLK_US_GRAVE  VGLK_TILDE  ///< Grave ` on american keyboard
#define VGLK_US_TILDE  VGLK_TILDE  ///< Tilde ~ is just because some people call the grave key the tilde key

//German
#define VGLK_DE_COMMA  VGLK_COMMA  ///< Comma "," on german keyboard
#define VGLK_DE_PERIOD VGLK_PERIOD ///< Period "." on german keyboard
#define VGLK_DE_MINUS  VGLK_MINUS  ///< Minus on most keyboards

#define VGLK_DE_Y      VGLK_Y    ///< The keys for Y and Z were exchanged on german keyboards. Not be required anymore
#define VGLK_DE_Z      VGLK_Z

//Misc
#define VGLK_MENU VGLK_APPS        ///< Windows menu (application) key

/////////////////////////////////////////////
/////////////////////////////////////////////

#endif // _VISION_DOC

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
