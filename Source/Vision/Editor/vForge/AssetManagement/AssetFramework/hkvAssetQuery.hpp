/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKVASSETQUERY_HPP_INCLUDED
#define HKVASSETQUERY_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetDecl.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Tags/hkvAssetTag.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvCallback.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvIProperties.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Assets/hkvAsset.hpp>

#include <Common/Base/Thread/CriticalSection/hkCriticalSection.h>

#include <vector>

class hkvAsset;
class hkvAssetManager;
class hkvAssetLibrary;
class hkvAssetQuery;

/// \brief
///   Data object for the OnQueryChanged callback in the hkvAssetQuery.
class hkvQueryChangedData : public hkvCallbackData
{
public:
  ASSETFRAMEWORK_IMPEXP hkvQueryChangedData(const hkvCallback* sender) : hkvCallbackData(sender)
  {
    m_query = NULL;
  }
  const hkvAssetQuery* m_query;
};


/// \brief
///   This struct defines a query to the hkvAssetManager.
class hkvAssetQuery : public hkvCallbackHandler, public hkvIProperties
{
  friend class hkvAssetManager;
  friend class hkvAssetLibrary;

public:
  ASSETFRAMEWORK_IMPEXP hkvAssetQuery();
  ASSETFRAMEWORK_IMPEXP hkvAssetQuery(const hkvAssetQuery& rhs);
  ASSETFRAMEWORK_IMPEXP ~hkvAssetQuery();
  ASSETFRAMEWORK_IMPEXP void operator=(const hkvAssetQuery& rhs);
  ASSETFRAMEWORK_IMPEXP void clearAll();

  /// Sets a bitfield that acts as a filter when 'all types' are selected (to make only a subset of all types part of such a query)
  ASSETFRAMEWORK_IMPEXP void setAllTypesFilter(const hkBitField& AllowedTypes);

  // Asset Library Query
  ASSETFRAMEWORK_IMPEXP void clearLibrary();
  ASSETFRAMEWORK_IMPEXP void setIncludeLibrarySubfolders(bool includeSubfolder);
  ASSETFRAMEWORK_IMPEXP hkBool isIncludeLibrarySubfoldersSet() const;
  ASSETFRAMEWORK_IMPEXP void setCurrentLibrary(const char* libraryName);
  ASSETFRAMEWORK_IMPEXP void setCurrentFolder(const char* relativePath);
  ASSETFRAMEWORK_IMPEXP const char* getCurrentLibrary() const;
  ASSETFRAMEWORK_IMPEXP const char* getCurrentFolder() const;

  // Asset Type Query
  ASSETFRAMEWORK_IMPEXP void clearAssetTypes();
  ASSETFRAMEWORK_IMPEXP bool setAssetType(hkUint32 typeIndex, bool isQueried);
  ASSETFRAMEWORK_IMPEXP bool isAssetTypeSet(hkUint32 typeEntryIndex) const;
  ASSETFRAMEWORK_IMPEXP bool isAnyTypeSet() const;
  ASSETFRAMEWORK_IMPEXP bool isAssetTypeInQuery(const hkvAssetTypeInfoHandle& typeInfo, hkInt32 subtype) const;

  // Tag Query
  ASSETFRAMEWORK_IMPEXP void clearTags();
  ASSETFRAMEWORK_IMPEXP hkBool setTag(hkUint32 tagIndex, bool isQueried);
  ASSETFRAMEWORK_IMPEXP hkBool isTagSet(hkUint32 tagIndex) const;
  ASSETFRAMEWORK_IMPEXP hkBool isAnyTagSet() const;

  // String Query
  ASSETFRAMEWORK_IMPEXP void clearStringQuery();
  ASSETFRAMEWORK_IMPEXP void setStringQuery(const char* stringQuery);
  ASSETFRAMEWORK_IMPEXP const char* getStringQuery() const;

  // Failed Asset Query
  ASSETFRAMEWORK_IMPEXP void setShowOnlyFailedAssets(bool showFailed);
  ASSETFRAMEWORK_IMPEXP bool shouldShowOnlyFailedAssets() const;

  // Queries
  ASSETFRAMEWORK_IMPEXP void evaluateQuery();

  // Library view
  /// \brief
  ///   The asset with the given path is always present in the view (used by the asset browser dialog).
  ///   Returns false if the path could not be resolved.
  ASSETFRAMEWORK_IMPEXP bool setPersistentAsset(const char* szPath);
  /// \brief
  ///   Returns the index of the persistent asset in the view.
  ///   Returns HKV_INVALID_INDEX if no persistent asset is present.
  ASSETFRAMEWORK_IMPEXP hkUint32 getPersistentAssetIndexInView() const;

  ASSETFRAMEWORK_IMPEXP hkUint32 getNumberOfAssetsInView() const;
  ASSETFRAMEWORK_IMPEXP hkvAsset::RefCPtr getAssetInView(hkUint32 viewIndex) const;
  ASSETFRAMEWORK_IMPEXP hkInt32 indexOfAssetInView(const hkvAsset& asset) const;

  // View Settings
  ASSETFRAMEWORK_IMPEXP void sortByCategory(hkvAssetSortingCriterion assetSortingCriterion, bool ascending);
  inline hkvAssetSortingCriterion getSortingCriterion() const { return m_sortingCriterion; }
  inline bool getAscending() const { return m_ascending; }

  ASSETFRAMEWORK_IMPEXP void setUseListView(bool listView);
  inline bool getUseListView() const { return m_listView; }

  ASSETFRAMEWORK_IMPEXP void setThumbnailSize(hkInt32 thumbnailSize);
  inline hkInt32 getThumbnailSize() const { return m_thumbnailSize; }

  // Selection
  ASSETFRAMEWORK_IMPEXP void clearSelection();
  ASSETFRAMEWORK_IMPEXP void addToSelection(hkUint32 viewIndex);
  ASSETFRAMEWORK_IMPEXP hkUint32 getNumberOfAssetsInSelection() const;
  ASSETFRAMEWORK_IMPEXP const hkArray<hkvAsset::RefPtr>& getSelection() const;
  ASSETFRAMEWORK_IMPEXP hkvAsset::RefPtr getAssetInSelection(hkUint32 selectionIndex) const;
  ASSETFRAMEWORK_IMPEXP const char* getSelectedAssetPaths(const char* separator, hkStringBuf& out_buffer) const;
  /// \brief
  ///   Returns false if no selection is present. 
  ASSETFRAMEWORK_IMPEXP hkBool getSelectionTags(hkvAssetTags& out_tagsLogicalOr, hkvAssetTags& out_tagsLogicalAnd) const;
  ASSETFRAMEWORK_IMPEXP void callOnSelectionChanged() const;


  // hkvCallbackHandler
  ASSETFRAMEWORK_IMPEXP VOVERRIDE void handleCallback(hkvCallbackData* data) HKV_OVERRIDE;

  // hkvIProperties
  ASSETFRAMEWORK_IMPEXP VOVERRIDE const char* getTypeName() const;
  ASSETFRAMEWORK_IMPEXP VOVERRIDE void getProperties(hkvPropertyList& properties, 
    hkvProperty::Purpose purpose) const HKV_OVERRIDE;
  ASSETFRAMEWORK_IMPEXP VOVERRIDE void setProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, 
    hkUint32 stackIndex, hkvProperty::Purpose purpose) HKV_OVERRIDE;

public:
  hkvCallback OnQueryChanged;           ///< Called when a query's settings have changed. Called by 'evaluateQuery'.
  hkvCallback OnAssetViewDataChanged;   ///< Called when a query's result is ready. Called by 'sortByCategory'.
  hkvCallback OnSelectionChanged;       ///< Called when the selection in the query has been modified.
  hkvCallback OnViewSettingsChanged;    ///< Called when view settings have changed, like sort order, thumbnail size, use of list view etc.

private:
  void checkPlausibility();

  void callOnQueryChanged() const;
  void callOnAssetViewDataChanged() const;
  void callOnViewSettingsChanged() const;

private:
  // Library
  hkStringPtr m_libraryName;
  hkStringPtr m_relativePath;
  bool m_includeSubdirectories;
  // Asset Types
  hkBitField m_types;
  hkBitField m_AllTypesFilter;
  // Tags
  hkvAssetTags m_tags;
  // Name
  hkStringPtr m_stringQuery;
  // Failed Assets
  bool m_showOnlyFailedAssets;

  // Library view
  hkvAsset::RefPtr m_persistentAsset;
  hkUint32 m_persistentAssetIndexInView;

  std::vector<hkvAsset::RefPtr> m_query;
  hkvAssetSortingCriterion m_sortingCriterion;
  bool m_ascending;
  bool m_listView;
  int m_thumbnailSize;

  // Selection
  hkArray<hkvAsset::RefPtr> m_selection;
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
