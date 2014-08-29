/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VLUA_APIDOC

%nodefaultctor VScriptFileSystem_wrapper;
%nodefaultdtor VScriptFileSystem_wrapper;

class VScriptFileSystem_wrapper
{
public:
  
  bool Exists(const char * szFilename, const char * szOptionalPath = NULL);
  
  %extend
  {
    void GetAbsDir(const char* szFileName, VCaptureSwigEnvironment* env)
    {
      char pBuffer[FS_MAX_PATH];
      Vision::File.GetAbsoluteDirectory(pBuffer, szFileName);
      env->AddReturnValueString(pBuffer);
    }
  }
};

//Implement GetDataDirs native because it returns a list of strings
%native(GetDataDirs) int VScriptFileSystem_wrapper_GetDataDirs(lua_State *L);
%{
  SWIGINTERN int VScriptFileSystem_wrapper_GetDataDirs(lua_State *L)
  {
    IS_MEMBER_OF(VScriptFileSystem_wrapper) //this will move this function to the method table of the specified class

    lua_newtable(L);                                        //stack: ..., table

    VFileAccessManager* pManager = VFileAccessManager::GetInstance();
    int iCount = pManager->GetNumSearchPaths();

    for(int i=0;i<iCount;++i)
    {
      lua_pushstring(L, pManager->GetSearchPath(i).GetBasePath().AsChar());  //stack: ..., table, string
      lua_rawseti(L, -2, i+1);                         //stack: ..., table
    } 
    
    return 1; //return the table as stack param
  }
%}

#else

/// \brief File system related methods. It appears as member of the \b Application instance with the name \b FileSystem.
/// \par Example
///   \code
///     local dataDirs = Application.FileSystem:GetDataDirs()
///
///     for i=1,#dataDirs do
///       Debug:PrintLine("Data directory "..i..": "..dataDirs[i])
///     end
///   \endcode
class VScriptFileSystem_wrapper {
public:
 
  /// @name File System Access Functions
  /// @{
 
  /// \brief Check whether a file exists or not.
  /// \param filename Name of the file (relative to one of the data directories).
  /// \param optionalPath (\b optional) Optional directory that the filename is relative to. This is prepended to the filename.
  /// \return Returns true if the file exists at the specified location, otherwise false.
  /// \par Example
  ///   \code
  ///     Application.FileSystem:Exists("MyHero.model", "models")
  ///   \endcode
  boolean Exists(string filename, string optionalPath = nil);

  /// \brief Helper function that takes a relative directory name and combines it with the current directory to make an absolute path.
  /// \note If you call this after engine init then the current directory might already have changed and not be the application directory anymore!
  /// \param relativeDirectory (\b optional) The relative directory.
  /// \return Returns the absolute directory as string.
  /// \par Example
  ///   \code
  ///     local absDir = Application.FileSystem:GetAbsDir("models")
  ///   \endcode
  string GetAbsDir(string relativeDirectory);
  
  /// \brief Get all present data directories.
  /// \return A table containing string entries from index 1 to LEN (could be an empty table too).
  /// \par Example
  ///   \code
  ///     local dataDirs = Application.FileSystem:GetDataDirs()
  ///   
  ///     Debug:PrintLine("Detected "..#dataDirs.." data directories.")
  ///
  ///     for i=1,#dataDirs do
  ///       Debug:PrintLine("Data directory "..i..": "..dataDirs[i])
  ///     end
  ///   \endcode
  table GetDataDirs();
  
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
