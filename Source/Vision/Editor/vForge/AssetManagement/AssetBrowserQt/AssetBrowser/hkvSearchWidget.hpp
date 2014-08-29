/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvSearchWidget.hpp

#ifndef VSearchWidget_HPP_INCLUDED
#define VSearchWidget_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/AssetBrowserQtPCH.h> 

ANALYSIS_IGNORE_ALL_START
#include <QWidget>
#include <QDockWidget>
#include <QToolBar>
#include <QLineEdit>
#include <QAction>
#include <QLayout>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/AssetFramework.hpp>
ANALYSIS_IGNORE_ALL_END

class hkvAssetQuery;

/// \brief
///   Text search widget for the asset browser.
class hkvSearchWidget : public QWidget
{
  Q_OBJECT
public:
  ASSETBROWSER_IMPEXP hkvSearchWidget (QWidget* pParent, hkvAssetQuery* pQuery);
  ASSETBROWSER_IMPEXP ~hkvSearchWidget ();

public slots:
  ASSETBROWSER_IMPEXP void OnQueryChanged();
  ASSETBROWSER_IMPEXP void SearchLineEdit_OnTextChanged ( const QString & text );
  ASSETBROWSER_IMPEXP void ClearSearchAction_OnClicked ();

private:
  void SetupUI();

private:
  QToolBar* m_pToolBar;
  QLineEdit* m_pLineEdit;
  QAction* m_pClearSearchAction;
  QLayout* m_pLayout;
  bool m_bPreventCallbacks;
  hkvAssetQuery* m_pQuery;
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
