/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvAssetView.hpp

#ifndef HKVCOLLECTIONDATA_HPP_INCLUDED
#define HKVCOLLECTIONDATA_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/AssetBrowserQtPCH.h> 

ANALYSIS_IGNORE_ALL_START
#include <QTreeView>
#include <QDragEnterEvent>
#include <QDropEvent>

#include <deque>
#include <vector>
ANALYSIS_IGNORE_ALL_END

class dummy1 : QObject
{
  Q_OBJECT
};

// The base class for the collection hierarchy.
class hkvCollectionModelEntryBase
{
public:
  struct EntryType
  {
    enum Enum
    {
      ET_COLLECTION      = V_BIT(0),
      ET_COLLECTIONENTRY = V_BIT(1),
      ET_DEPENDENCYENTRY = V_BIT(2),
      ET_ALL             = ET_COLLECTION | ET_COLLECTIONENTRY | ET_DEPENDENCYENTRY,
    };
  };

  ASSETBROWSER_IMPEXP hkvCollectionModelEntryBase(EntryType::Enum type);
  ASSETBROWSER_IMPEXP virtual ~hkvCollectionModelEntryBase();

  ASSETBROWSER_IMPEXP void ClearEntries(void);
  ASSETBROWSER_IMPEXP hkUint32 GetNumSubEntries(hkUint32 uiEntryTypes = EntryType::ET_ALL) const;
  ASSETBROWSER_IMPEXP hkUint32 MapFilteredIndexToReal(hkUint32 uiIndex, hkUint32 uiEntryTypes) const;
  ASSETBROWSER_IMPEXP hkUint32 MapRealIndexToFiltered(hkUint32 uiIndex, hkUint32 uiEntryTypes) const;
  ASSETBROWSER_IMPEXP hkvCollectionModelEntryBase* GetSubEntry(hkUint32 uiIndex, hkUint32 uiEntryTypes) const;
  ASSETBROWSER_IMPEXP void AddSubEntry(hkvCollectionModelEntryBase* pEntry);
  ASSETBROWSER_IMPEXP virtual hkvCollectionModelEntryBase* RemoveSubEntry(hkUint32 uiIndex, hkUint32 uiEntryTypes);

  ASSETBROWSER_IMPEXP EntryType::Enum GetType() const { return m_Type; };
  ASSETBROWSER_IMPEXP hkvCollectionModelEntryBase* GetParentEntry() const;
  ASSETBROWSER_IMPEXP hkUint32 GetParentsNthEntry(hkUint32 uiEntryTypes) const;

  // interface for derived classes
public:
  ASSETBROWSER_IMPEXP virtual QVariant data(int iColumn, int iRole) const {return QVariant();}

protected:
  hkvCollectionModelEntryBase* m_pParentEntry;
  hkUint32 m_uiParentsNthEntry;
  hkArray<hkvCollectionModelEntryBase*> m_SubEntries;
  hkEnum<EntryType::Enum, hkUint8> m_Type;
};

//// All dependencies that are built on the fly on top of hkvCollectionEntrys.
//class hkvDependencyEntry : public hkvCollectionEntry
//{
//
//};

// All entries that are saved within the collection
class hkvCollectionEntry : public hkvCollectionModelEntryBase
{
public:
  ASSETBROWSER_IMPEXP hkvCollectionEntry();
  ASSETBROWSER_IMPEXP virtual ~hkvCollectionEntry();

  ASSETBROWSER_IMPEXP virtual QVariant data(int iColumn, int iRole) const HKV_OVERRIDE;

public:
  bool m_bIsAsset;
  hkStringPtr m_sName;
};

// The root of the collection model
class hkvCollectionQt : public QObject, public hkvCollectionModelEntryBase
{
  Q_OBJECT
public:
  ASSETBROWSER_IMPEXP hkvCollectionQt();
  ASSETBROWSER_IMPEXP virtual ~hkvCollectionQt();

  ASSETBROWSER_IMPEXP const char* GetAbsolutePath() { return m_sAbsoluteCollectionPath; }
  ASSETBROWSER_IMPEXP hkUint32 AddEntries(const std::vector<hkStringPtr>& entries);
  ASSETBROWSER_IMPEXP virtual hkvCollectionModelEntryBase* RemoveSubEntry(hkUint32 uiIndex, hkUint32 uiEntryTypes) HKV_OVERRIDE;
  ASSETBROWSER_IMPEXP void UpdateDependencies();

  ASSETBROWSER_IMPEXP hkResult SaveCollection(const char* szAbsolutePath);
  ASSETBROWSER_IMPEXP hkResult LoadCollection(const char* szAbsolutePath);
  ASSETBROWSER_IMPEXP hkResult TransformCollection(const char* szAbsolutePath) const;
  ASSETBROWSER_IMPEXP void SetModified(bool bModified = true);
  ASSETBROWSER_IMPEXP bool IsModified() const { return m_bModified; };

signals:
  void CollectionModifiedStateChanged();

private:
  void BuildCollection(std::vector<hkStringPtr>& out_collection) const;
  bool m_bModified;
  hkStringPtr m_sAbsoluteCollectionPath;
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
