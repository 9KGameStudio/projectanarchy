/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKVPROPERTYVIEW_HPP_INCLUDED
#define HKVPROPERTYVIEW_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/AssetBrowserQtPCH.h> 
#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/PropertyBrowser/hkvPropertyModelEntry.hpp>

ANALYSIS_IGNORE_ALL_START
#include <QTreeView>
ANALYSIS_IGNORE_ALL_END

class hkvPropertyView : public QTreeView
{
  Q_OBJECT
public:
  ASSETBROWSER_IMPEXP hkvPropertyView(QWidget *parent = 0);

public slots:
  virtual void reset() HKV_OVERRIDE;
  virtual void selectionChanged(const QItemSelection & selected, const QItemSelection & deselected) HKV_OVERRIDE;

signals:
  void selectedRowChanged(const QModelIndex& index);

protected:
  ASSETBROWSER_IMPEXP void drawRow(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
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
