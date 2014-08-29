/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvPropertyWidget.hpp

#ifndef HKVPROPERTYWIDGET_HPP_INCLUDED
#define HKVPROPERTYWIDGET_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/AssetBrowserQtPCH.h> 

ANALYSIS_IGNORE_ALL_START
#include <QWidget>
#include <QtPropertyBrowser/qtpropertymanager.h>
#include <QDateTime>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/AssetFramework.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvIProperties.hpp>
ANALYSIS_IGNORE_ALL_END

class QCheckBox;
class QHBoxLayout;
class QDoubleSpinBox;
class QSpinBox;
class QLineEdit;
class QToolButton;
class QDateTimeEdit;
class QComboBox;
class QLabel;

/// \brief
///   Base class for all property widgets
class ASSETBROWSER_IMPEXP hkvPropertyWidget : public QWidget
{
  Q_OBJECT
public:
  hkvPropertyWidget (QWidget* pParent);
  ~hkvPropertyWidget ();

  const hkvProperty& getValue() const;

public slots:
  void setValue(const hkvProperty& p);

signals:
  void valueChanged();

protected:
  virtual void focusInEvent(QFocusEvent *e) HKV_OVERRIDE;
  virtual void focusOutEvent(QFocusEvent *e) HKV_OVERRIDE;

protected:
  virtual void UpdateUI() {}

protected:
  hkvProperty m_propertyValue;
};


/// \brief
///   Property widget class handling bool properties.
class hkvBoolPropertyWidget : public hkvPropertyWidget
{
  Q_OBJECT
public:
  hkvBoolPropertyWidget (QWidget* pParent);
  ~hkvBoolPropertyWidget ();

protected:
  virtual void UpdateUI() HKV_OVERRIDE;

public slots:
  void OnCheckBox_stateChanged(int);

private:
  QCheckBox* m_pCheckBox;
  QHBoxLayout* m_pLayout;
};


/// \brief
///   Property widget class handling int properties.
class hkvIntPropertyWidget : public hkvPropertyWidget
{
  Q_OBJECT
public:
  hkvIntPropertyWidget (QWidget* pParent);
  ~hkvIntPropertyWidget ();

protected:
  virtual void UpdateUI() HKV_OVERRIDE;

  public slots:
    void OnSpinBox_editingFinished(void);

private:
  hkvProperty::Type m_eType;
  QSpinBox* m_pSpinBox;
  QHBoxLayout* m_pLayout;
};

/// \brief
///   Property widget class handling double properties.
class hkvDoublePropertyWidget : public hkvPropertyWidget
{
  Q_OBJECT
public:
  hkvDoublePropertyWidget (QWidget* pParent);
  ~hkvDoublePropertyWidget ();

protected:
  virtual void UpdateUI() HKV_OVERRIDE;

public slots:
  void OnSpinBox_editingFinished(void);

private:
  QDoubleSpinBox* m_pSpinBox;
  QHBoxLayout* m_pLayout;
  bool m_bUseDouble;
};


/// \brief
///   Property widget class handling string properties.
class hkvStringPropertyWidget : public hkvPropertyWidget
{
  Q_OBJECT
public:
  hkvStringPropertyWidget (QWidget* pParent);
  ~hkvStringPropertyWidget ();

protected:
  virtual void UpdateUI() HKV_OVERRIDE;

  public slots:
    void OnLineEdit_editingFinished(void);

private:
  QLineEdit* m_pLineEdit;
  QHBoxLayout* m_pLayout;
};


/// \brief
///   Property widget class handling file properties.
class hkvFilePropertyWidget : public hkvPropertyWidget
{
  Q_OBJECT
public:
  enum SelectionMode
  {
    SelectionModeFile,
    SelectionModeFolder,
  };

  hkvFilePropertyWidget (QWidget* pParent, enum SelectionMode mode);
  ~hkvFilePropertyWidget ();

protected:
  virtual void UpdateUI() HKV_OVERRIDE;

  public slots:
    void OnLineEdit_editingFinished(void);
    void OnButton_clicked(void);

private:
  QString m_sDialogCaption;
  bool m_bRestrictToAssets;
  hkEnum<SelectionMode, hkUint8> m_mode;
  QString m_sFilter;
  QString m_sRestrictToFolder;

  QLineEdit* m_pLineEdit;
  QToolButton* m_pButton;
  QHBoxLayout* m_pLayout;
};

/// \brief
///   Property widget class handling timestamp properties.
class hkvTimeStampPropertyWidget : public hkvPropertyWidget
{
  Q_OBJECT
public:
  hkvTimeStampPropertyWidget (QWidget* pParent);
  ~hkvTimeStampPropertyWidget ();

protected:
  virtual void UpdateUI() HKV_OVERRIDE;

  public slots:
    void OnTimeStamp_editingFinished(void);

private:
  QDateTime m_currentValue;
  QDateTimeEdit* m_pTimeStamp;
  QHBoxLayout* m_pLayout;
};

/// \brief
///   Property widget class handling enum properties.
class hkvEnumPropertyWidget : public hkvPropertyWidget
{
  Q_OBJECT
public:
  hkvEnumPropertyWidget (QWidget* pParent);
  ~hkvEnumPropertyWidget ();

protected:
  virtual void UpdateUI() HKV_OVERRIDE;

public slots:
  void OnComboBox_currentIndexChanged(int iIndex);

private:
  QComboBox* m_pComboBox;
  QHBoxLayout* m_pLayout;
  QString m_sValues;
  QString m_sConfirmationMessage;
};


/// \brief
///   Property widget class handling array properties.
class hkvArrayPropertyWidget : public hkvPropertyWidget
{
  Q_OBJECT
public:
  hkvArrayPropertyWidget(QWidget* parent);
  ~hkvArrayPropertyWidget();

protected:
  virtual void UpdateUI() HKV_OVERRIDE;

private slots:
  void OnButtonClicked();

private:
  QLabel* m_label;
  QToolButton* m_button;
  QHBoxLayout* m_layout;
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
