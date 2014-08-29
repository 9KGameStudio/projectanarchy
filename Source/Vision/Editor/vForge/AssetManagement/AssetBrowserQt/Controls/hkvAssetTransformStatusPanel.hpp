/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKV_ASSET_TRANSFORM_STATUS_PANEL_HPP_INCLUDED
#define HKV_ASSET_TRANSFORM_STATUS_PANEL_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/AssetBrowserQtPCH.h> 

ANALYSIS_IGNORE_ALL_START
#include <QWidget>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetManager.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/GeneratedFiles/ui_hkvAssetTransformStatusPanel.h>
ANALYSIS_IGNORE_ALL_END

class hkvAssetTransformStatusPanel : public QWidget, public hkvCallbackHandler, public Ui_hkvAssetTransformStatusPanel
{
  Q_OBJECT

public:
  ASSETBROWSER_IMPEXP hkvAssetTransformStatusPanel(QWidget* parent);
  ASSETBROWSER_IMPEXP ~hkvAssetTransformStatusPanel();

private slots:
  virtual void on_ProgressBar_doubleClicked();
  virtual void on_ButtonAutomaticTransform_clicked();

private:
   void UpdateButtonState();
   void UpdateToolTip();
   void UpdateProgressBar();
   void OnTransformStatusChanged();

public:
  ASSETBROWSER_IMPEXP virtual void handleCallback(hkvCallbackData* pData) HKV_OVERRIDE;

private:
  hkvTransformStatus m_status;
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
