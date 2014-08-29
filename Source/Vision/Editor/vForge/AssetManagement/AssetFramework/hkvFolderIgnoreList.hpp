/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvFolderIgnoreList.hpp

#ifndef HKV_FOLDER_IGNORE_LIST_HPP_INCLUDED
#define HKV_FOLDER_IGNORE_LIST_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetDecl.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvBase.hpp>
#include <Common/Base/Container/StringMap/hkStorageStringMap.h>

/// \brief
///   This class decides whether a path below its given root folder should be ignored using wildcard matching,
///   given the accumulated ignore lists in the path's parent folders.
///
/// In any folder below the given root directory a file can be placed that contains entries that extend the ignore list
/// for this folder and all folders below it.
/// The name of the file is defined in the constructor. Each non-empty line of the file is added to the ignore list and
/// the syntax of each line is a wildcard match. The ignore lists are then applied to to the relative path whenever the function
/// 'shouldIgnorePath' is called.
/// To ignore all files of a certain type, add the line "*.ext" to an ignore list, which will be applied to all files in and below the
/// folder containing the ignore list.
/// To ignore a certain folder name, add "*/folderName/*" to an ignore list.
class hkvFolderIgnoreList
{
public:
  hkvFolderIgnoreList(const char* rootDirectory, const char* ignoreListName);
  ~hkvFolderIgnoreList();

private:
  hkvFolderIgnoreList(const hkvFolderIgnoreList&);
  hkvFolderIgnoreList& operator=(const hkvFolderIgnoreList&);

public:
  /// \brief
  ///   Whether the given path should be ignored according to the gathered ignore lists. Only the relative 
  ///   portion of the path is wildcard matched to the ignore list entries.
  ///
  /// \param path
  ///   Relative or absolute path to the file oder folder that should be checked against the ignore lists.
  ///
  /// \param isFolder
  ///   Whether the path is a folder or file.
  ///
  /// \return
  ///   Whether the file should be ignored or not.
  bool shouldIgnorePath(const char* path, bool isFolder) const;

  /// \brief
  ///   Clears the cached ignore lists and reloads them from file whenever 'shouldIgnorePath' gets called.
  void clearIgnoreCache();

private:
  const hkArray<hkStringPtr>& getMostRelevantIgnoreList(const char* relativePath) const;
  const hkArray<hkStringPtr>& getMostRelevantIgnoreListRecursive(const char* relativePath) const;

  bool isMatchedByList(const char* relativePath, const hkArray<hkStringPtr>& ignoreList, bool isFolder) const;

private:
  hkStringPtr m_ignoreListName;
  hkStringPtr m_rootDirectory;
  hkArray<hkStringPtr> m_rootIgnoreList;

  mutable hkStorageStringMap<const hkArray<hkStringPtr>*> m_relPathToIgnoreList;
};

#endif

/*
 * Havok SDK - Base file, BUILD(#20140624)
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
