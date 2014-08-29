/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvPropertyBrowserWidget.hpp

#ifndef HKVPROPERTYBROWSERWIDGET_HPP_INCLUDED
#define HKVPROPERTYBROWSERWIDGET_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/AssetBrowserQtPCH.h> 

ANALYSIS_IGNORE_ALL_START
#include <QWidget>
#include <QSplitter>
#include <QLabel>
ANALYSIS_IGNORE_ALL_END

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/AssetFramework.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvIProperties.hpp>

class QLayout;
class QtAbstractPropertyBrowser;
class hkvPropertyEditorFactory;

class QModelIndex;
class hkvPropertyModel;
class hkvPropertyView;
class hkvPropertyDelegate;

class hkvPropertyBrowserWidget : public QWidget
{
  Q_OBJECT
public:
  ASSETBROWSER_IMPEXP hkvPropertyBrowserWidget (QWidget* pParent);
  ASSETBROWSER_IMPEXP ~hkvPropertyBrowserWidget ();

  void SetInterface (hkvIProperties* pPropertyInterface);
  void Update();

private slots:
  void OnSelectedRowChanged(const QModelIndex& index);

private:
  void SetupUI();

private:
  QLabel* m_pDescription;

  hkvPropertyModel* m_pPropertyTreeModel;
  hkvPropertyView* m_pPropertyTreeView;
  hkvPropertyDelegate* m_pPropertyDelegate;
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
