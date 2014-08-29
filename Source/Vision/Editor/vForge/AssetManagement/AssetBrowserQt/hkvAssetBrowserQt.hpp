/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvAssetBrowserQt.hpp

#ifndef VASSETBROWSERQT_HPP_INCLUDED
#define VASSETBROWSERQT_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/AssetBrowserQtPCH.h> 

ANALYSIS_IGNORE_ALL_START
#include <QDialog>
#include <QDialogButtonBox>
#include <QMainWindow>
#include <QDockWidget>
#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/AssetBrowser/hkvSearchWidget.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/AssetBrowser/hkvTypeWidget.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/AssetBrowser/hkvTagWidget.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/AssetBrowser/hkvLibraryWidget.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/hkvThumbnailCache.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetDecl.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetQuery.hpp>
ANALYSIS_IGNORE_ALL_END

class hkvAssetViewWidget;
class hkvAssetModel;
class hkvAssetProperties;
class hkvAssetTransformStatusPanel;
class QPushButton;
class QDockWidget;


/// \brief
///   The main widget of the asset browser. It holds the different query dock widgets and the 
///   asset browser view. This widget is embedded into a vForge DockableForm by passing the window handle
///   between the two.
class hkvAssetBrowserQt : public QMainWindow, public hkvCallbackHandler//, public Ui_AssetBrowser
{
  Q_OBJECT
public:
  enum Mode
  {
    M_Panel,
    M_Dialog_OnlyAssets,
    M_Dialog_AllowExternalFiles,
  };

  ASSETBROWSER_IMPEXP hkvAssetBrowserQt (QWidget *parent, Mode eMode, const char* szAssetTypes = NULL, const char* szPreviousUid = NULL, bool multiSelection = false);
  ASSETBROWSER_IMPEXP ~hkvAssetBrowserQt();

  ASSETBROWSER_IMPEXP VOVERRIDE void handleCallback(hkvCallbackData* pData) HKV_OVERRIDE;

  ASSETBROWSER_IMPEXP const char* getSelectedAssets() const;

  ASSETBROWSER_IMPEXP void OnlyShowOneAsset(const char* szPath);

  ASSETBROWSER_IMPEXP hkvAssetProperties* getAssetPropertiesPanel() const { return m_pAssetProperties; }

public slots:
  void OnDialogButtonClicked(QDialogButtonBox::StandardButton button);
  void OnWidgetEmbedded(QWidget* widget);

private slots:
  void OnAcceptDialog();
  void OnCancelDialog();
  void OnCancelDialogAndSelectFile();
  void OnShowProperties();
  void OnWindowLoaded();

signals:
  void QueryChanged();
  void AssetViewDataChanged();
  void AvailableButtonsChanged(QDialogButtonBox::StandardButtons buttons);
  void ButtonStateChanged(QDialogButtonBox::StandardButton button, bool enabled, QString text);
  void RequestDialogClose(int returnCode);

  // private functions
private:
  void SetupUI();
  void ShutdownUI();
  void SaveUIState();
  void LoadUIState();

  // static functions
public:
  static void DeInit();
  static hkvAssetQuery* GetDialogQuery(const QString& sAssetTypes);
  static hkvAssetBrowserQt* GetPanel() { return s_pPanel; }

  //public data
public:
  hkvCallback OnRequestDialogClose;

  //private data
private:
  Mode m_eMode;
  hkStringPtr m_sAllowedAssetTypes;
  bool m_multiSelection;
  bool m_bUsePersistentAsset;

  // Search DockWidget
  QDockWidget* m_pSearchDockWidget;
  hkvSearchWidget* m_pSearchWidget;

  // Type DockWidget
  QDockWidget* m_pTypeDockWidget;
  hkvTypeWidget* m_pTypeWidget;

  // Tag DockWidget
  QDockWidget* m_pTagDockWidget;
  hkvTagWidget* m_pTagWidget;

  // Library DockWidget
  QDockWidget* m_pLibraryDockWidget;
  hkvLibraryWidget* m_pLibraryWidget;

  // Dialog selection
  hkStringPtr m_selectedAssets;

  // Main widget
  hkvAssetViewWidget* m_pAssetView;
  hkvAssetModel* m_pAssetModel;

  // Status panels
  hkvAssetTransformStatusPanel* m_pAssetTransformStatusPanel;

  hkvAssetProperties* m_pAssetProperties;

  hkvAssetQuery* m_pQuery;
  hkvThumbnailCache* m_pThumbnailCache;

  //static data
private:
  static QMap<QString, hkvAssetQuery*> s_querySettings;

  static hkvAssetBrowserQt* s_pPanel;
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
