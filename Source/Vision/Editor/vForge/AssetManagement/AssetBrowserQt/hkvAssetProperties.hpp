/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvAssetProperties.hpp

#ifndef VASSETPROPERTIES_HPP_INCLUDED
#define VASSETPROPERTIES_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/AssetBrowserQtPCH.h> 

ANALYSIS_IGNORE_ALL_START
#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/GeneratedFiles/ui_hkvAssetProperties.h>
#include <QScrollArea>
ANALYSIS_IGNORE_ALL_END
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvCallback.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Assets/hkvAsset.hpp>

class hkvAssetQuery;
class hkvPropertyBrowserWidget;
class QMenu;
class QRegExpValidator;

/// \brief
///   Scroll area that shows the currently selected asset in the asset property floater.
class hkvThumbnailArea : public QScrollArea
{
  Q_OBJECT

public:
  hkvThumbnailArea(QWidget* parent = NULL);
  void setAsset(hkvAsset::RefCPtr pAsset);
  const hkvAsset* getAsset() const { return m_pAsset.val(); }

private slots:
  void OnThumbnailLoaded(int iRow, const QString& asset);

protected:
  virtual void resizeEvent(QResizeEvent* event) HKV_OVERRIDE;
  void updatePixMap();

private:
  QPixmap m_thumbnail;
  QLabel* m_pLabel;
  hkvAsset::RefCPtr m_pAsset;
};


/// \brief
///   Asset property widget
class hkvAssetProperties : public QWidget, public Ui_hkvAssetProperties, public hkvCallbackHandler
{
  Q_OBJECT
public:
  hkvAssetProperties(QWidget* parent, hkvAssetQuery* pQuery);
  ~hkvAssetProperties();

  virtual void handleCallback(hkvCallbackData* pData) HKV_OVERRIDE;
  virtual bool event(QEvent* e) HKV_OVERRIDE;

  const hkvAsset* getSelectedAsset() const;

  static hkvAssetProperties* getCurrentInstance() { return s_pCurrentInstance; }

public slots:
  void onUpdateAssetProperties();
  void onAddTagButton_clicked();
  void OnTagName_textChanged(const QString& newName);
  void onAvailableTags_itemDoubleClicked(QListWidgetItem* item);
  void onUsedTags_itemDoubleClicked(QListWidgetItem* item);
  void onTags_customContextMenuRequested(const QPoint& pos);
  void deleteCurrentTag();
  virtual void on_ButtonTransformAsset_clicked();
  virtual void on_ButtonMoveTagRight_clicked();
  virtual void on_ButtonMoveTagLeft_clicked();

signals:
  void updateAssetProperties();

  //private functions
private:
  void SetupUI(QWidget* pParent);
  void updateTags();

  //private data
private:
  hkvAssetQuery* m_pQuery;
  QRegExpValidator* m_pValidator;
  QMenu* m_pContextMenu;
  QIcon m_tagIcon;
  hkvThumbnailArea* m_pThumbnailView;
  
  static hkvAssetProperties* s_pCurrentInstance;
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
