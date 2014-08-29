/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKV_WAIT_FOR_PROCESS_DLG_HPP
#define HKV_WAIT_FOR_PROCESS_DLG_HPP

#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/AssetBrowserQtPCH.h> 

ANALYSIS_IGNORE_ALL_START
#include <QProcess>
#include <QDialogButtonBox>
#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/GeneratedFiles/ui_hkvWaitForProcessDlg.h>
ANALYSIS_IGNORE_ALL_END

class ASSETBROWSER_IMPEXP hkvWaitForProcessDlg : public QWidget, public Ui_hkvWaitForProcessDlg
{
  Q_OBJECT

public:
  hkvWaitForProcessDlg(QWidget* parent, QProcess& process, const QString& sExecutable, const QString& sName);

signals:
  void AvailableButtonsChanged(QDialogButtonBox::StandardButtons buttons);
  void ButtonStateChanged(QDialogButtonBox::StandardButton button, bool enabled, QString text);
  void RequestDialogClose(int returnCode);

public slots:
  void OnCloseEventReceived(QCloseEvent* event);

private slots:
  void on_ShowProcess_clicked();
  void ProcessFinished();
  void ProcessError(QProcess::ProcessError error);
  void ProcessStateChanged(QProcess::ProcessState newState);
  void OnWindowLoaded();

private:
  QProcess& m_Process;
  QString m_sExecutable;
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
