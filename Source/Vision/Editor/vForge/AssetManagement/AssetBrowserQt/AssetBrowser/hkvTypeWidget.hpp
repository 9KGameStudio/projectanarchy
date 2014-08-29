/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvTypeWidget.hpp

#ifndef VTYPEWIDGET_HPP_INCLUDED
#define VTYPEWIDGET_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/AssetBrowserQtPCH.h> 

ANALYSIS_IGNORE_ALL_START
#include <QWidget>
#include <QDockWidget>
#include <QToolBar>
#include <QListWidget>
#include <QAction>
#include <QLayout>
ANALYSIS_IGNORE_ALL_END

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/AssetFramework.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Assets/hkvAssetTypeManager.hpp>

class hkvAssetQuery;

/// \brief
///   Type selection widget for the asset browser.
class hkvTypeWidget : public QWidget, public hkvCallbackHandler
{
  Q_OBJECT
public:
  ASSETBROWSER_IMPEXP hkvTypeWidget (QWidget* pParent, hkvAssetQuery* pQuery, const char* szTypesList);
  ASSETBROWSER_IMPEXP ~hkvTypeWidget ();

  ASSETBROWSER_IMPEXP VOVERRIDE void handleCallback(hkvCallbackData* pData);
  ASSETBROWSER_IMPEXP void OnTypeDataChanged(const hkArray<hkvAssetTypeEntry>& types);

public slots:
  ASSETBROWSER_IMPEXP void OnQueryChanged();
  ASSETBROWSER_IMPEXP void TypeListWidget_OnItemChanged(QListWidgetItem* pItem);
  ASSETBROWSER_IMPEXP void ClearSelectionAction_OnClicked();

private:
  void SetupUI();

private:
  //QToolBar* m_pToolBar;
  //QAction* m_pClearSelectionAction;
  QListWidget* m_pListWidget; ///< The data model rarely changes and is rather simple, so a model/view approach would be overkill.
  QLayout *m_pLayout;
  bool m_bPreventCallbacks;
  hkvAssetQuery* m_pQuery;
  hkStringPtr m_OnlyEnableTheseTypes;
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
