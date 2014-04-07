/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvAssetProject.hpp

#ifndef HKV_ASSET_PROJECT_HPP_INCLUDED
#define HKV_ASSET_PROJECT_HPP_INCLUDED

#include <Vision/Runtime/Base/Math/hkvMathConfig.h>

/// \brief
///   Helper class for handling asset projects in stand-alone applications
class hkvAssetProject : public hkReferencedObject
{
public:
  HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_TOOLS);

public:
  /// \brief
  ///   Constructor; initializes this asset project instance.
  /// \param fileAccessManager
  ///   the file access manager to use for setting the file systems and search paths associated
  ///   with this asset project
  ASSETFRAMEWORK_IMPEXP hkvAssetProject(VFileAccessManager& fileAccessManager);
private:
  hkvAssetProject(const hkvAssetProject&);
  hkvAssetProject& operator=(const hkvAssetProject&);
public:
  ASSETFRAMEWORK_IMPEXP ~hkvAssetProject();

public:
  /// \brief
  ///   Loads an asset project and its asset libraries.
  /// \param projectPath
  ///   the root path of the asset project
  /// \return
  ///   whether loading succeeded. On failure, this class is reset, so no partially loaded
  ///   data will be present.
  ASSETFRAMEWORK_IMPEXP hkvResult load(const char* projectPath);

  /// \brief
  ///   Unloads the currently loaded asset project.
  ASSETFRAMEWORK_IMPEXP void unload();

  /// \brief
  ///   Returns the name of the asset library file that is considered the local
  ///   asset library. This may be empty; if so, a local library file name will be determined based
  ///   on the name of the computer and the current user.
  const char* getLocalLibraryFileHint() const
  {
    return m_localLibraryFileHint;
  }

  /// \brief
  ///   Sets the name of the asset library file that is considered the local
  ///   asset library. This is also the name under which the asset library will be saved. If 
  ///   empty, a local library file name will be determined based on the name of the computer 
  ///   and the current user.
  void setLocalLibraryFileHint(const char* name)
  {
    m_localLibraryFileHint = name;
  }

  /// \brief
  ///   Returns the name of the library file to load. If non-empty, only this file will be 
  ///   considered when loading the asset library. If not set, all library files for this asset
  ///   library will be loaded and merged together.
  const char* getInputLibraryFile() const
  {
    return m_inputLibraryFile;
  }

  /// \brief
  ///   Sets the name of the library file to load. If non-empty, only this file will be 
  ///   considered when loading the asset library. If not set, all library files for this asset
  ///   library will be loaded and merged together.
  void setInputLibraryFile(const char* name)
  {
    m_inputLibraryFile = name;
  }

  /// \brief
  ///   Returns the native root directory of the Havok Vision SDK installation in which the current
  ///   executable is running. (Only valid if a project has successfully been opened)
  const char* getSdkRoot() const
  {
    return m_sdkRoot;
  }

  /// \brief
  ///   Returns the native root folder of the currently loaded project.
  const char* getProjectRoot() const
  {
    return m_projectRoot;
  }

  /// \brief
  ///   Returns the native path to the asset project file of the currently loaded project.
  const char* getProjectFile() const
  {
    return m_projectFile;
  }

  /// \brief
  ///   Returns the native path of the workspace to which the currently loaded project belongs.
  const char* getWorkspaceRoot() const
  {
    return m_workspaceRoot;
  }

  /// \brief
  ///   Returns the search path of the project folder within its workspace file system.
  const char* getProjectSearchPath() const
  {
    return m_projectSearchPath;
  }

private:
  hkvResult importProjectManifest();
  hkvResult addDataDirectory(const char* path);
  hkvResult loadAssetLibraries();

private:
  VFileAccessManagerPtr m_fileAccessManager;

  hkStringPtr m_localLibraryFileHint;
  hkStringPtr m_inputLibraryFile;

  hkStringPtr m_sdkRoot;
  hkStringPtr m_projectRoot;
  hkStringPtr m_projectFile;
  hkStringPtr m_workspaceRoot;
  hkStringPtr m_projectSearchPath;
};

#endif

/*
 * Havok SDK - Base file, BUILD(#20140328)
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
