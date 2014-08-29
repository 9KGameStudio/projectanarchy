/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKV_ASSET_LOG_MESSAGES_DLG_HPP
#define HKV_ASSET_LOG_MESSAGES_DLG_HPP

#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/AssetBrowserQtPCH.h> 

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Assets/hkvAsset.hpp>

ANALYSIS_IGNORE_ALL_START
#include <QDialogButtonBox>
#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/GeneratedFiles/ui_hkvAssetLogMessagesDlg.h>
ANALYSIS_IGNORE_ALL_END

class hkvAssetLogMessagesDlg : public QWidget, public Ui_hkvAssetLogMessagesDlg
{
  Q_OBJECT

public:
  hkvAssetLogMessagesDlg(const hkvAsset& asset, QWidget* parent = 0);

signals:
  void AvailableButtonsChanged(QDialogButtonBox::StandardButtons buttons);
  void ButtonStateChanged(QDialogButtonBox::StandardButton button, bool enabled, QString text);
  void RequestDialogClose(int returnCode);

public slots:
  void OnDialogButtonClicked(QDialogButtonBox::StandardButton button);
  void OnWidgetEmbedded(QWidget* widget);

private slots:
  void OnCopyMessagesClicked();
  void OnCloseClicked();
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
