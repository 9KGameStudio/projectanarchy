/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvCollectionModel.hpp

#ifndef HKVCOLLECTIONMODEL_HPP_INCLUDED
#define HKVCOLLECTIONMODEL_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/AssetBrowserQtPCH.h> 

ANALYSIS_IGNORE_ALL_START
#include <QAbstractItemModel>
#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/CollectionPanel/hkvCollectionData.hpp>
ANALYSIS_IGNORE_ALL_END

class hkvCollectionModel : public QAbstractItemModel
{
  Q_OBJECT
public:
  ASSETBROWSER_IMPEXP hkvCollectionModel(QObject* parent = 0);
  ASSETBROWSER_IMPEXP ~hkvCollectionModel();

  ASSETBROWSER_IMPEXP void setCollection(hkvCollectionQt* pCollection);

  ASSETBROWSER_IMPEXP hkUint32 addEntries(const std::vector<hkStringPtr>& relativePaths);
  ASSETBROWSER_IMPEXP hkUint32 deleteEntries(QModelIndexList& indices);


  ASSETBROWSER_IMPEXP virtual QStringList mimeTypes() const HKV_OVERRIDE;
  ASSETBROWSER_IMPEXP virtual QMimeData* mimeData(const QModelIndexList &indexes) const HKV_OVERRIDE;
  ASSETBROWSER_IMPEXP Qt::DropActions supportedDropActions() const HKV_OVERRIDE;

public:
  // Implementation of QAbstractItemModel to provide read access to model
  virtual QVariant data(const QModelIndex& index, int role) const HK_OVERRIDE;
  virtual Qt::ItemFlags flags(const QModelIndex& index) const HK_OVERRIDE;
  virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const HK_OVERRIDE;
  virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const HK_OVERRIDE;
  virtual QModelIndex parent(const QModelIndex& index) const HK_OVERRIDE;
  virtual int rowCount(const QModelIndex& parent = QModelIndex()) const HK_OVERRIDE;
  virtual int columnCount(const QModelIndex& parent = QModelIndex()) const HK_OVERRIDE;

  // Implementation of QAbstractItemModel to provide write access to model
  //virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) HK_OVERRIDE;

private:
  hkUint32 m_uiShowEntries;
  hkvCollectionQt* m_pCollection;
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
