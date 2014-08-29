/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvManifestSettingsQt.hpp

#ifndef VMANIFESTSETTINGSQT_HPP_INCLUDED
#define VMANIFESTSETTINGSQT_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/AssetBrowserQtPCH.h> 

ANALYSIS_IGNORE_ALL_START
#include <QWidget>
#include <QToolBar>
#include <QAction>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetDecl.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetStructs.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvCallback.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetLibrary.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/GeneratedFiles/ui_hkvManifestSettingsQt.h>
ANALYSIS_IGNORE_ALL_END

/// \brief
///   This dialog allows changing the vForgeManifest.txt that can be placed in the project directory.
class hkvManifestSettingsQt : public QWidget, public Ui_hkvManifestSettingsQt
{
  Q_OBJECT
public:
  ASSETBROWSER_IMPEXP hkvManifestSettingsQt(QWidget* pParent, const char* projectDir, const char* commaSeperatedEditorPlugins);
  ASSETBROWSER_IMPEXP ~hkvManifestSettingsQt ();

  //public data
public:
  hkvCallback OnRequestDialogClose;

public slots:
  void on_AssetLibraries_currentIndexChanged(int iIndex);
  void on_EnginePlugins_dataChanged();
  void on_vForgePlugins_dataChanged();
  void on_AssetLibraries_dataChanged();
  void on_DialogButtonBox_accepted();
  void on_DialogButtonBox_rejected();

private: 
  void setupUI();
  bool loadManifest();
  bool saveManifest();

private:
  QString m_sProjectDir;
  bool m_bManifestDirty;
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
