/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvProfileManagerQt.hpp

#ifndef VPROFILEMANAGERQT_HPP_INCLUDED
#define VPROFILEMANAGERQT_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/AssetBrowserQtPCH.h> 

ANALYSIS_IGNORE_ALL_START
#include <QWidget>
#include <QToolBar>
#include <QAction>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetDecl.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetStructs.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/GeneratedFiles/ui_hkvProfileManager.h>
ANALYSIS_IGNORE_ALL_END

/// \brief
///   This widget allows you to manage profiles and transform templates.
class hkvProfileManagerQt : public QWidget, public IVisCallbackHandler_cl, public Ui_hkvProfileManager
{
  Q_OBJECT
public:
  ASSETBROWSER_IMPEXP hkvProfileManagerQt (QWidget *parent);
  ASSETBROWSER_IMPEXP ~hkvProfileManagerQt ();

  ASSETBROWSER_IMPEXP VOVERRIDE void OnHandleCallback(IVisCallbackDataObject_cl *pData);

private slots:
  ASSETBROWSER_IMPEXP void on_LockButton_clicked();
  ASSETBROWSER_IMPEXP void on_UnlockButton_clicked();

  ASSETBROWSER_IMPEXP void ProfilesTable_OnCellActivated(int iRow, int iColumn);
  ASSETBROWSER_IMPEXP void ProfilesAction_OnAddProfileTriggered();
  ASSETBROWSER_IMPEXP void ProfilesAction_OnDeleteProfileTriggered();

  ASSETBROWSER_IMPEXP void TemplatesTree_OnItemSelectionChanged();
  ASSETBROWSER_IMPEXP void TemplatesAction_OnAddTemplateTriggered();
  ASSETBROWSER_IMPEXP void TemplatesAction_OnDeleteTemplateTriggered();
  ASSETBROWSER_IMPEXP void TemplatesAction_OnRenameTemplateTriggered();

private:
  void UpdateProfiles();
  void UpdateTemplates(const QString& sSelectTemplateAssetType = QString(), hkUint32 iSelectTemplateIndex = HKV_INVALID_INDEX);
  void UpdateTransformRuleWidget();
  void UpdateLockStatus();
  void SetupUI();
  void ShutdownUI();

private:
  unsigned int m_iSelectedProfile;
  QTreeWidgetItem* m_pSelectedTemplate;
  bool m_bOwnsLock;

  // Qt member
  QToolBar* m_pProfileToolBar;
  QAction* m_pAddProfileAction;
  QAction* m_pDeleteProfileAction;

  QToolBar* m_pTemplateToolBar;
  QAction* m_pAddTemplateAction;
  QAction* m_pDeleteTemplateAction;
  QAction* m_pRenameTemplateAction;
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
