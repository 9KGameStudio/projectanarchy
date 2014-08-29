/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKV_VFORGE_DLG_HPP
#define HKV_VFORGE_DLG_HPP

#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/AssetBrowserQtPCH.h> 

ANALYSIS_IGNORE_ALL_START
#include <QDialog>
#include <QDialogButtonBox>
ANALYSIS_IGNORE_ALL_END

class QAbstractButton;
class QHBoxLayout;
class QLabel;
class QPushButton;

class ASSETBROWSER_IMPEXP hkvVForgeDlg : public QDialog
{
  Q_OBJECT

public:
  enum DialogReturnValue
  {
    Rejected = 0,
    Accepted = 1,
    Custom = 2
  };

  hkvVForgeDlg(const QString& caption, const QString& description, const QString& iconResource, QWidget* parent = 0, Qt::WindowFlags f = 0);
  ~hkvVForgeDlg();

  void embedWidget(QWidget* widget, int paddingLeft = 0, int paddingTop = 0, int paddingRight = 0, int paddingBottom = 0);
  void setHelp(const char* szHelpKey, const char* szHelpContext);
  virtual void closeEvent(QCloseEvent* event) HKV_OVERRIDE;
  
signals:
  void DialogButtonClicked(QDialogButtonBox::StandardButton button);
  void WidgetEmbedded(QWidget* widget);
  void CloseEventReceived(QCloseEvent* event);

public slots:
  virtual void reject() HKV_OVERRIDE;

private slots:
  void OnAvailableButtonsChanged(QDialogButtonBox::StandardButtons buttons);
  void OnButtonStateChanged(QDialogButtonBox::StandardButton button, bool enabled, QString text);
  void OnStandardButtonClicked(QAbstractButton* button);
  void OnRequestClose(int returnCode);
  void OnHelpClicked();

private:
  void setupUI();

private:
  hkStringPtr m_sHelpKey;
  hkStringPtr m_sHelpContext;
  QWidget* m_titleFrame;

  QLabel* m_titleCaption;
  QLabel* m_titleDescription;
  QLabel* m_titleImage;
  QPushButton* m_helpButton;

  QHBoxLayout* m_clientLayout;

  QDialogButtonBox* m_dialogButtons;
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
