/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKVPROPERTYDELEGATE_HPP_INCLUDED
#define HKVPROPERTYDELEGATE_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/AssetBrowserQtPCH.h> 

ANALYSIS_IGNORE_ALL_START
#include <QWidget>
#include <QAbstractItemModel>
#include <QStyledItemDelegate>
ANALYSIS_IGNORE_ALL_END

class hkvPropertyDelegate : public QStyledItemDelegate
{
  Q_OBJECT

public:
  ASSETBROWSER_IMPEXP hkvPropertyDelegate(QObject* parent = 0);
  ASSETBROWSER_IMPEXP ~hkvPropertyDelegate();

  ASSETBROWSER_IMPEXP void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
  ASSETBROWSER_IMPEXP QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
  ASSETBROWSER_IMPEXP QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;
  ASSETBROWSER_IMPEXP void setEditorData(QWidget* editor, const QModelIndex& index) const;
  ASSETBROWSER_IMPEXP void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;
  ASSETBROWSER_IMPEXP bool eventFilter(QObject *object, QEvent *event) HKV_OVERRIDE;
private slots:
  void slotSetValue();
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
