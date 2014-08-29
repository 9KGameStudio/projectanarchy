/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvAssetViewWidget.hpp

#ifndef VASSETVIEWWIDGET_HPP_INCLUDED
#define VASSETVIEWWIDGET_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/AssetBrowserQtPCH.h> 

ANALYSIS_IGNORE_ALL_START
#include <QItemDelegate>
#include <QWidget>
ANALYSIS_IGNORE_ALL_END
#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/AssetBrowser/hkvAssetView.hpp>

class QAbstractItemModel;
class QComboBox;
class QMenu;
class QToolBar;
class QPushButton;
class QListView;
class QTableView;
class QLayout;
class QSettings;
class QLabel;

class hkvAssetQuery;

/// \brief
///   Asset View widget
class hkvAssetViewWidget : public QWidget, public hkvCallbackHandler
{
  Q_OBJECT
public:
  ASSETBROWSER_IMPEXP hkvAssetViewWidget(QWidget* parent, hkvAssetQuery* pQuery, bool bIsDialog, bool bMultiSelection);
  ASSETBROWSER_IMPEXP ~hkvAssetViewWidget();

  ASSETBROWSER_IMPEXP virtual void handleCallback(hkvCallbackData* pData) HKV_OVERRIDE;

  void setModel(QAbstractItemModel* model);
  /// \brief
  ///   Sets the selection to the given row and scroll to make it visible.
  void selectRow(hkUint32 row);

  void SaveUIState(QSettings* pSettings) const;
  void LoadUIState(const QSettings* pSettings);

signals:
  void ShowProperties();
  void AcceptDialog();

public slots:
  void OnQueryChanged();

private slots:
  void OnChangeViewTriggered(bool checked);
  void OnThumbnailSizeSliderValueChanged(int size);
  void OnListViewZoom(int delta);
  void OnSortCriterionBoxCurrentIndexChanged(int index);
  void OnSortOrderClicked();
  void OnTableViewSortIndicatorChanged(int logicalIndex, Qt::SortOrder order);

  void OnShowFailedAssetsTriggered(bool checked);
  void OnTransformAssetsTriggered();
  void OnShowPropertiesTriggered();
  void OnShowInExplorerTriggered();
  void OnOpenFileTriggered();
  void OnPerforceCheckoutTriggered();
  void OnShowLogMessagesTriggered();
  void OnCopyFullPathTriggered();

  void OnSelectionChanged();
  void OnShowContextMenu(const QPoint& pos);

protected:
  //virtual void keyPressEvent(QKeyEvent* pEvent);

private:
  void OnQueryViewSettingsChanged();
  void ChangeView(bool listView);
  void SetupToolBar();
  void SetupContextMenu();
  void SetupUI();
  void GetAbsPathListOfSelection(QStringList& out_AbsPathList) const;

private:
  QToolBar* m_pToolBar;
  QLabel* m_pStatusLabel;
  QAction* m_pListViewAction;
  QAction* m_pTableViewAction;
  QAction* m_pShowFailedAssetsAction;
  QAction* m_pTransformAssets;
  QAction* m_pShowInExporer;
  QAction* m_pOpenFileAction;
  QAction* m_pShowPropertiesAction;
  QAction* m_pPerforceCheckoutAction;
  QAction* m_pShowLogMessagesAction;
  QAction* m_pCopyFullPathAction;

  QSlider* m_pThumbnailSizeSlider;

  QComboBox* m_pSortCriterionBox;
  QPushButton* m_pSortOrder;
  
  hkvQListView* m_pListView;
  hkvQTableView* m_pTableView;

  QMenu* m_pContextMenu;
  QMenu* m_pContextMenuDialogMode;

  hkvAssetQuery* m_pQuery;
  bool m_bIsDialog;
  bool m_bMultiSelection;
};

class hkvListViewDelegate : public QItemDelegate
{
  friend class hkvAssetViewWidget;
  Q_OBJECT
private:
  hkvListViewDelegate(QObject* parent = 0);
  ~hkvListViewDelegate();

  virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const HKV_OVERRIDE;
  virtual QSize	sizeHint(const QStyleOptionViewItem & option, const QModelIndex& index) const HKV_OVERRIDE;

  QSize gridSize(const QFont& font) const;
  float fontPointSize() const;

  enum 
  {
    ItemSelectionFrameWidth = 4,
    ItemBorderPadding = 5,
    ItemBorderPaddingSide = 7,
    ItemTextPadding = 5
  };

  int m_iconSize;
};

class hkvTableViewDelegate : public QItemDelegate
{
  friend class hkvAssetViewWidget;
  Q_OBJECT
private:
  hkvTableViewDelegate(QObject* parent = 0);
  ~hkvTableViewDelegate();

  virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const HKV_OVERRIDE;
  virtual QSize	sizeHint(const QStyleOptionViewItem & option, const QModelIndex& index) const HKV_OVERRIDE;

  enum 
  {
    ItemTextPadding = 4,
    ItemIconSize = 24
  };
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
