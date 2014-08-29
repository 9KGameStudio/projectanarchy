/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvNewProfileDlg.hpp

#ifndef HKVNEWPROFILEDLG_HPP_INCLUDED
#define HKVNEWPROFILEDLG_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/AssetBrowserQtPCH.h> 

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetStructs.hpp>

ANALYSIS_IGNORE_ALL_START
#include <QDialog>
#include <QDialogButtonBox>
#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/GeneratedFiles/ui_hkvNewProfileDlg.h>
ANALYSIS_IGNORE_ALL_END


/// \brief
///   Library and folder selection widget for the asset browser.
class hkvNewProfileDlg : public QWidget, public Ui_hkvNewProfileDlg
{
  Q_OBJECT
public:
  ASSETBROWSER_IMPEXP hkvNewProfileDlg(QWidget* pParent);

  ASSETBROWSER_IMPEXP hkvTargetPlatform GetPlatform() const { return m_eSelectedPlatform; }
  ASSETBROWSER_IMPEXP const QString& GetName() const { return m_sName; }
  ASSETBROWSER_IMPEXP unsigned int GetSourceProfile() const { return m_uiSourceProfile; }

public slots:
  void OnProfileNameEdited();
  virtual void on_m_pPlatformComboBox_currentIndexChanged(int index);
  void OnDialogButtonClicked(QDialogButtonBox::StandardButton button);
  void OnWidgetEmbedded(QWidget* widget);

signals:
  void AvailableButtonsChanged(QDialogButtonBox::StandardButtons buttons);
  void ButtonStateChanged(QDialogButtonBox::StandardButton button, bool enabled, QString text);
  void RequestDialogClose(int returnCode);

private:
  void SetupUI();

private:
  hkvTargetPlatform m_eSelectedPlatform;
  QString m_sName;
  hkUint32 m_uiSourceProfile;
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
