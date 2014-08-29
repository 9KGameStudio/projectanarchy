/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvCollectionPanelQt.hpp

#ifndef VCOLLECTIONPANELQT_HPP_INCLUDED
#define VCOLLECTIONPANELQT_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/AssetBrowserQtPCH.h> 

ANALYSIS_IGNORE_ALL_START
#include <QMainWindow>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetDecl.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetStructs.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvCallback.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/GeneratedFiles/ui_hkvCollectionPanelQt.h>
ANALYSIS_IGNORE_ALL_END

class hkvCollectionTab;

/// \brief
///   
class hkvCollectionPanelQt : public QMainWindow, public Ui_hkvCollectionPanelQt, public hkvCallbackHandler
{
  Q_OBJECT
public:
  ASSETBROWSER_IMPEXP hkvCollectionPanelQt(QWidget* pParent);
  ASSETBROWSER_IMPEXP ~hkvCollectionPanelQt ();

  ASSETBROWSER_IMPEXP void CloseAllTabs();
  ASSETBROWSER_IMPEXP virtual void handleCallback(hkvCallbackData* data) HKV_OVERRIDE;

private slots:
  void on_actionNewCollection_triggered();
  void on_actionOpenCollection_triggered();
  void on_actionSaveCollection_triggered();
  void on_actionSaveCollectionAs_triggered();
  //void on_actionTransformCollection_triggered();
  void on_actionAddFiles_triggered();
  void on_actionAddAssets_triggered();

  void on_CollectionTab_tabCloseRequested(int iTabIndex);
  void on_CollectionTab_currentChanged(int index);
  void OnCollectionModifiedStateChanged();

private: 
  void SetupUI();
  void UpdateTabName(hkInt32 iTabIndex);
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
