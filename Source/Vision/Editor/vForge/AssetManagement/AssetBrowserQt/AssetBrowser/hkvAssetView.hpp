/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvAssetView.hpp

#ifndef VASSETVIEW_HPP_INCLUDED
#define VASSETVIEW_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/AssetBrowserQtPCH.h> 

ANALYSIS_IGNORE_ALL_START
#include <QListView>
#include <QTableView>
#include <QDragEnterEvent>
#include <QDropEvent>
ANALYSIS_IGNORE_ALL_END

class hkvQListView : public QListView
{
  Q_OBJECT
public:
  ASSETBROWSER_IMPEXP hkvQListView(QWidget* parent);

signals:
  void OnMouseWheelZoom(int delta);

protected:
  virtual bool event(QEvent* event) HKV_OVERRIDE;
  virtual void wheelEvent(QWheelEvent* event) HKV_OVERRIDE;
};


class hkvQTableView : public QTableView
{
  Q_OBJECT
public:
  ASSETBROWSER_IMPEXP hkvQTableView(QWidget* parent);

protected:
  virtual bool event(QEvent* event) HKV_OVERRIDE;
};


class hkvAssetViewDropHandler
{
public:
  ASSETBROWSER_IMPEXP static void dragEnterEvent(QDragEnterEvent* pEvent);
  ASSETBROWSER_IMPEXP static void dropEvent(QDropEvent* pEvent);
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
