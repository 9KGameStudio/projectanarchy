/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvAssetModel.hpp

#ifndef VASSETMODEL_HPP_INCLUDED
#define VASSETMODEL_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/AssetBrowserQtPCH.h> 

ANALYSIS_IGNORE_ALL_START
#include <QPixmap>
#include <QAbstractTableModel>
ANALYSIS_IGNORE_ALL_END

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/AssetFramework.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvCallback.hpp>

class hkvAsset;
class hkvAssetQuery;
class hkvThumbnailCache;

/// \brief
///   Custom model that represents the results of the current query in the asset browser.
class hkvAssetModel : public QAbstractTableModel
{
  Q_OBJECT
public:
  ASSETBROWSER_IMPEXP hkvAssetModel(QObject* pParent, hkvAssetQuery* pQuery, hkvThumbnailCache* pThumbnailCache);

  ASSETBROWSER_IMPEXP virtual int rowCount(const QModelIndex& parent) const HKV_OVERRIDE;
  ASSETBROWSER_IMPEXP virtual int columnCount(const QModelIndex& parent) const HKV_OVERRIDE;
  ASSETBROWSER_IMPEXP virtual QVariant data(const QModelIndex& index, int role) const HKV_OVERRIDE;
  ASSETBROWSER_IMPEXP virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const HKV_OVERRIDE;
  ASSETBROWSER_IMPEXP virtual Qt::ItemFlags flags(const QModelIndex& index) const HKV_OVERRIDE;

  ASSETBROWSER_IMPEXP virtual QStringList mimeTypes() const HKV_OVERRIDE;
  ASSETBROWSER_IMPEXP virtual QMimeData* mimeData(const QModelIndexList &indexes) const HKV_OVERRIDE;

  ASSETBROWSER_IMPEXP Qt::DropActions supportedDropActions() const HKV_OVERRIDE;

public slots:
  ASSETBROWSER_IMPEXP void OnResetModel();

private slots:
  void OnAssetsChanged(const std::vector<hkvAsset::RefCPtr>& assets);
  void OnThumbnailLoaded(int iRow, const QString& asset);

public:
  enum { NumSortingCategories = 5 };
  static const char* const s_SortingCategories[NumSortingCategories];

private:
  hkvAssetQuery* m_pQuery;

  hkvThumbnailCache* m_pThumbnailCache;
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
