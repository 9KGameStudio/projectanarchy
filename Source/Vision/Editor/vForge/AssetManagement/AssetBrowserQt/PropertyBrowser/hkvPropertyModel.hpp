/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKVPROPERTYMODEL_HPP_INCLUDED
#define HKVPROPERTYMODEL_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/AssetBrowserQtPCH.h> 
#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/PropertyBrowser/hkvPropertyModelEntry.hpp>

ANALYSIS_IGNORE_ALL_START
#include <QAbstractItemModel>
ANALYSIS_IGNORE_ALL_END

class hkvPropertyModel : public QAbstractItemModel
{
  Q_OBJECT
public:
  ASSETBROWSER_IMPEXP hkvPropertyModel(QObject* parent = 0);
  ASSETBROWSER_IMPEXP ~hkvPropertyModel();

  ASSETBROWSER_IMPEXP void SetInterface(hkvIProperties* pPropertyInterface);  
  ASSETBROWSER_IMPEXP void Update();

private:
  void Clear();
  bool BuildPropertyPath(hkvPropertyModelEntry* pEntry, hkvPropertyModelEntry* pTarget, hkArray<hkStringPtr>& path);

public:
  // virtual functions required to implement for QAbstractItemModel
  virtual QVariant data(const QModelIndex& index, int role) const HK_OVERRIDE;
  virtual Qt::ItemFlags flags(const QModelIndex& index) const HK_OVERRIDE;
  virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const HK_OVERRIDE;
  virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const HK_OVERRIDE;
  virtual QModelIndex parent(const QModelIndex& index) const HK_OVERRIDE;
  virtual hkInt32 rowCount(const QModelIndex& parent = QModelIndex()) const HK_OVERRIDE;
  virtual hkInt32 columnCount(const QModelIndex& parent = QModelIndex()) const HK_OVERRIDE;

  virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

private:
  hkvPropertyModelEntry* m_pRoot;
  hkvIProperties* m_pPropertyInterface;
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
