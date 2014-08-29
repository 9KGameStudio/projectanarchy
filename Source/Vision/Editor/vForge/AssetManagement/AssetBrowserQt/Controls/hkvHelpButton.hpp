/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKV_HELP_ACTION_HPP_INCLUDED
#define HKV_HELP_ACTION_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/AssetBrowserQtPCH.h> 
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/UIService/hkvUiService.hpp>

ANALYSIS_IGNORE_ALL_START
#include <QToolButton>
#include <QAction>
ANALYSIS_IGNORE_ALL_END

class QMenu;

/// \brief
///   A help button for toolbars that calls into vForge's c# help system to show artist manual pages.
class hkvHelpButton : public QToolButton
{
  Q_OBJECT
public:
  ASSETBROWSER_IMPEXP hkvHelpButton(QWidget* parent, const char* szKey, const char* szContext = NULL);
  ASSETBROWSER_IMPEXP ~hkvHelpButton();

  ASSETBROWSER_IMPEXP void setHelpKey(const char* szHelpKey);
  ASSETBROWSER_IMPEXP const char* getHelpKey() const;

  ASSETBROWSER_IMPEXP void setDynamicHelpKeys(hkArray<hkStringPtr>& dynamicHelpKeys);
  ASSETBROWSER_IMPEXP const hkArray<hkStringPtr>& getDynamicHelpKeys() const;

  ASSETBROWSER_IMPEXP void setContext(const char* szContext);
  ASSETBROWSER_IMPEXP const char* getContext() const;

protected:
  virtual void mousePressEvent(QMouseEvent* e) HKV_OVERRIDE;

private:
  hkStringPtr m_sKey;
  hkArray<hkStringPtr> m_dynamicKeys;
  hkStringPtr m_sContext;
  QMenu* m_pMenu;
};

/// \brief
///   Helper menu item for hkvHelpAction. Describes one clickable help entry in the menu.
class hkvHelpAction : public QAction
{
  Q_OBJECT
public:
  hkvHelpAction(QObject* pParent, const char* szKey, const char* szContextName);

private slots:
  void onTriggered();

private:
  const char* m_szKey;
  const char* m_szContextName;
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
