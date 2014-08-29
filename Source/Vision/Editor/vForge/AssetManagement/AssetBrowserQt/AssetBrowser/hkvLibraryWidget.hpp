/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvAssetBrowserQt.hpp

#ifndef VLIBRARYWIDGET_HPP_INCLUDED
#define VLIBRARYWIDGET_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/AssetBrowserQtPCH.h> 

ANALYSIS_IGNORE_ALL_START

#include <QWidget>
#include <QDockWidget>
#include <QToolBar>
#include <QTreeWidget>
#include <QAction>
#include <QLayout>

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/AssetFramework.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetManager.hpp>
ANALYSIS_IGNORE_ALL_END

class hkvAssetQuery;

enum VFolderType_e
{
  VFT_Root = 0,
  VFT_Lib = 1,
  VFT_SubFolder = 2,
};


/// \brief
///   Library and folder selection widget for the asset browser.
class hkvLibraryWidget : public QWidget, public hkvCallbackHandler
{
  Q_OBJECT
public:
  ASSETBROWSER_IMPEXP hkvLibraryWidget (QWidget* pParent, hkvAssetQuery* pQuery);
  ASSETBROWSER_IMPEXP ~hkvLibraryWidget ();

  ASSETBROWSER_IMPEXP VOVERRIDE void handleCallback(hkvCallbackData* pData);

signals:
  void LibraryDataChanged();
  void TransformAssets();

public slots:
  virtual void OnLibraryDataChanged();
  virtual void OnQueryChanged();
  virtual void LibraryTreeWidget_OnCurrentItemChanged( QTreeWidgetItem * current, QTreeWidgetItem * previous );
  virtual void CleanUpCache_OnTriggered(void);
  virtual void RefreshLibrary_OnTriggered(void);
  virtual void TransformAssets_OnTriggered(void);
  virtual void on_customContextMenuRequested(const QPoint&);
  virtual void on_ExploreTo_triggered();
  virtual void on_ShowAllSubItems_triggered();

private:
  void AddChildren (QTreeWidgetItem* pItem, const hkvTrackedDirectory* pLibDir);
  QTreeWidgetItem* GetRootLibrary(QTreeWidgetItem* pItem);
  void UpdateButtons(QTreeWidgetItem* pCurrent = NULL);
  void SetupUI();

private:
  QToolBar* m_pToolBar;

  QAction* m_pShowAllSubItems;
  QAction* m_pRefreshLibrary;
  QAction* m_pTransformAssets;
  QAction* m_pCleanUpCache;
  QAction* m_pActionExploreTo;
  QTreeWidget* m_pTreeWidget; ///< The data model rarely changes and is rather simple, so a model/view approach would be overkill.
  QLayout* m_pLayout;
  bool m_bPreventCallbacks;
  hkvAssetQuery* m_pQuery;
  QIcon m_folderIcon;
  QIcon m_AssetLibrariesIcon;
  QIcon m_library;
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
