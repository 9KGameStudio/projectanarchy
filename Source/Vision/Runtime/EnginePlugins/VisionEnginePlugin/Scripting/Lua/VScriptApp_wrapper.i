/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VLUA_APIDOC

class VScriptApp_wrapper
{
public:

  VScriptApp_wrapper();
  ~VScriptApp_wrapper();
  
  VScriptFileSystem_wrapper FileSystem;
  
  bool LoadScene(const char * szFileName);
  void UnloadScene();
  void ClearScene();

  bool IsInEditor();
  int GetEditorMode();
  
  const char * GetPlatformName();

  %extend
  {
    bool LoadScript(const char* szFileName, VCaptureSwigEnvironment* env)
    {
      return VScriptApp_wrapper::LoadScript(env->GetLuaState(), szFileName);
    }
  }
};

#else

/// \brief Base application related methods. Present in Lua as the global instance \b Application.
/// \par Example
///   \code
///     local mode = Application:GetEditorMode()
///     if mode ~= Vision.EDITOR_PLAY then
///       Debug:PrinLine("Please run in 'Play the Game' mode for full keyboard access!");
///     end
///   \endcode
class VScriptApp_wrapper {
public:

 
  /// @name Scene Control
  /// @{
 
  /// \brief Load the specified scene.
  /// \param sceneName The scene file name.
  /// \return true on success, otherwise false.
  boolean LoadScene(string sceneName);
  
  /// \brief Unload the current scene - perfom only at application exit.
  void UnloadScene();
  
  /// \brief Vclear the scene content, and prepare empty scene for use.	
  void ClearScene();

  /// \brief Check if the application is running in the vForge editor.
  /// \return true if the application runs in the editor, otherwise false.
  /// \see IsInEditor
  boolean IsInEditor();
  
  /// \brief Checks the current operating mode of the editor.
  /// \return The edot mode as one of the following values:
  ///  - EDITOR_NONE: No editor active.
  ///  - EDITOR_ANIMATING: Editor is in 'Animation' mode.
  ///  - EDITOR_RUN: Editor is in 'Run in Editr' mode.
  ///  - EDITOR_PLAY: Editor is in 'Play the Game' mode.
  /// \see IsInEditor
  /// \par Example
  ///   \code
  ///     local mode = Application:GetEditorMode()
  ///     if mode == Vision.EDITOR_ANIMATING then
  ///       Debug:PrinLine("Please run this sample at least in the 'Run in Editor' mode...");
  ///     end
  ///   \endcode
  number GetEditorMode();
 
  /// @} 
  /// @name General Purpose
  /// @{
  
  /// \brief Returns the name of the current platform as string.
  /// \return The name of the platform ("WIN32DX9", "ANDROID", "IOS", "TIZEN", ...)
  /// \par Example
  ///   \code
  ///     local isWindows = Application:GetPlatformName()=="WIN32DX9"
  ///     GUI:SetCursorVisible(isWindows)
  ///   \endcode
  string GetPlatformName();

  /// \brief Loads a script file. Similar to LUA's dofile, but this one uses the vision file system. 
  /// \param filename The script file to load.
  void LoadScript(string filename);
 
  /// @} 
  /// @name Public Members
  /// @{
  
  /// \brief Use this member for file system access.
  /// \par Example
  ///   \code
  ///     local dataDirs = Application.FileSystem:GetDataDirs()
  ///   \endcode
  VScriptFileSystem_wrapper FileSystem;
  
  /// @}
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
