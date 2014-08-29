/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/AssetManagement/VisionAssets/VisionAssetsPCH.h>

#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/Dialogs/hkvVForgeDlg.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/Dialogs/hkvWaitForProcessDlg.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/Wrapper/hkvQtDialogWrapper.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetManager.hpp>

#include <Vision/Editor/vForge/AssetManagement/VisionAssets/PropertyEditors/hkvLODDistancesDlg.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/PropertyEditors/hkvVisionPropertyEditingSupport.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/Assets/hkvFilterManagerTransformableAsset.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/ExternalTools/hkvExternalToolFilterManager.hpp>

#include <Common/Base/System/Io/Util/hkLoadUtil.h>

ANALYSIS_IGNORE_ALL_START
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QProcess>
#include <QMessageBox>
ANALYSIS_IGNORE_ALL_END

// ----------------------------------------------------------------------------
// hkvVisionPropertyEditingProvider
// ----------------------------------------------------------------------------

bool hkvVisionPropertyEditingProvider::canHandleProperty(const hkvProperty& prop) const
{
  if (prop.getType() == hkvProperty::TYPE_ARRAY && hkvStringHelper::safeCompare(prop.getCustomType(), "MeshLODDistances", false) == 0)
  {
    return true;
  }
  else if (prop.getType() == hkvProperty::TYPE_ARRAY && hkvStringHelper::safeCompare(prop.getCustomType(), hkvFilterManagerTransformableAsset::s_filterManagerSettingsCustomType, false) == 0)
  {
    return true;
  }
  return false;
}


void hkvVisionPropertyEditingProvider::makeDisplayString(const hkvProperty& prop, hkStringBuf& out_displayString) const
{
  if (prop.getType() == hkvProperty::TYPE_ARRAY && hkvStringHelper::safeCompare(prop.getCustomType(), "MeshLODDistances", false) == 0)
  {
    hkvLODDistancesPropertyWidget::makeDisplayString(prop, out_displayString);
  }
  else if (prop.getType() == hkvProperty::TYPE_ARRAY && hkvStringHelper::safeCompare(prop.getCustomType(), hkvFilterManagerTransformableAsset::s_filterManagerSettingsCustomType, false) == 0)
  {
    hkvTagfileFilterSettingsPropertyWidget::makeDisplayString(prop, out_displayString);
  }
}


hkvPropertyWidget* hkvVisionPropertyEditingProvider::createEditor(QWidget* parent, const hkvProperty& prop) const
{
  if (prop.getType() == hkvProperty::TYPE_ARRAY && hkvStringHelper::safeCompare(prop.getCustomType(), "MeshLODDistances", false) == 0)
  {
    return createEditorLOD(parent, prop);
  }
  else if (prop.getType() == hkvProperty::TYPE_ARRAY && hkvStringHelper::safeCompare(prop.getCustomType(), hkvFilterManagerTransformableAsset::s_filterManagerSettingsCustomType, false) == 0)
  {
    return createEditorTagfileFilterSettings(parent, prop);
  }

  return NULL;
}




hkvPropertyWidget* hkvVisionPropertyEditingProvider::createEditorLOD(QWidget* parent, const hkvProperty& prop) const
{
  const hkvProperty::VariantValueList& lodValues = prop.getArray();
  const size_t numLodValues = lodValues.size();
  return (numLodValues > 0) ? new hkvLODDistancesPropertyWidget(parent) : NULL;
}


hkvPropertyWidget* hkvVisionPropertyEditingProvider::createEditorTagfileFilterSettings(QWidget* parent, const hkvProperty& prop) const
{
  return new hkvTagfileFilterSettingsPropertyWidget(parent);
}

// ----------------------------------------------------------------------------
// hkvLODDistancesPropertyWidget
// ----------------------------------------------------------------------------
hkvLODDistancesPropertyWidget::hkvLODDistancesPropertyWidget(QWidget* parent)
: hkvPropertyWidget(parent)
{
  m_label = new QLabel(this);
  m_button = new QToolButton(this);
  m_button->setText(tr("..."));
  m_layout = new QHBoxLayout(this);
  m_layout->setContentsMargins(4, 0, 0, 0);
  m_layout->addWidget(m_label);
  m_layout->addWidget(m_button);
  setLayout(m_layout);

  connect(m_button, SIGNAL(clicked(bool)), this, SLOT(OnButtonClicked()));
  setFocusProxy(m_button);
}


hkvLODDistancesPropertyWidget::~hkvLODDistancesPropertyWidget()
{
}


void hkvLODDistancesPropertyWidget::makeDisplayString(const hkvProperty& prop, hkStringBuf& out_displayString)
{
  const hkvProperty::VariantValueList& lodValues = prop.getArray();
  const size_t numLodValues = lodValues.size();
  if (numLodValues < 1)
  {
    out_displayString = "No LOD support";
  }
  else if (numLodValues == 1)
  {
    out_displayString = "1 LOD distance";
  }
  else
  {
    out_displayString.printf("%d LOD distances", numLodValues);
  }
}


void hkvLODDistancesPropertyWidget::UpdateUI()
{
  const hkvProperty::VariantValueList& lodValues = m_propertyValue.getArray();

  if ((m_propertyValue.getFlags() & hkvProperty::FLAG_MULTIPLE_VALUES) == 0)
  {
    QString itemsStr;
    const size_t numLodValues = m_propertyValue.getArray().size();
    if (numLodValues == 1)
    {
      itemsStr = tr("1 LOD distance");
    }
    else
    {
      itemsStr.sprintf("%d LOD distances", numLodValues);
    }
    m_label->setText(itemsStr);
  }
  else
  {
    m_label->setText(QString::fromUtf8("<Multiple Values>"));
  }
}


void hkvLODDistancesPropertyWidget::OnButtonClicked()
{
  if (m_propertyValue.getType() != hkvProperty::TYPE_ARRAY)
  {
    return;
  }

  hkvProperty::VariantValueList lodList = m_propertyValue.getArray();

  hkvQtDialogWrapper dlgWrapper(this);
  hkvVForgeDlg hostDlg(tr("Edit LOD Distances"),
    tr("Edit the distances at which LOD levels are switched. Detail decreases with increasing LOD level. A distance of -1 means 'Infinity'. Changes take effect after re-transformation."),
    tr(":/Icons/Icons/LODDistances32.png"),
    &dlgWrapper);
  hkvLODDistancesDlg* lodDlg = new hkvLODDistancesDlg(lodList, &hostDlg);
  hostDlg.embedWidget(lodDlg);
  if (hostDlg.exec() == hkvVForgeDlg::Accepted)
  {
    m_propertyValue = hkvProperty(m_propertyValue.getName(), lodList, m_propertyValue.getCustomType());
    emit valueChanged();
  }
}


/////////////////////////////////////////////////////////////////////////////
// hkvTagfileFilterSettingsPropertyWidget
/////////////////////////////////////////////////////////////////////////////

hkvTagfileFilterSettingsPropertyWidget::hkvTagfileFilterSettingsPropertyWidget(QWidget* parent)
  : hkvPropertyWidget(parent)
{
  m_label = new QLabel(this);
  m_button = new QToolButton(this);
  m_button->setText(tr("..."));
  m_layout = new QHBoxLayout(this);
  m_layout->setContentsMargins(4, 0, 0, 0);
  m_layout->addWidget(m_label);
  m_layout->addWidget(m_button);
  setLayout(m_layout);

  connect(m_button, SIGNAL(clicked(bool)), this, SLOT(OnButtonClicked()));
  setFocusProxy(m_button);
}


hkvTagfileFilterSettingsPropertyWidget::~hkvTagfileFilterSettingsPropertyWidget()
{
}


void hkvTagfileFilterSettingsPropertyWidget::makeDisplayString(const hkvProperty& prop, hkStringBuf& out_displayString)
{
  const hkvProperty::VariantValueList& params = prop.getArray();

  if ((prop.getFlags() & hkvProperty::FLAG_MULTIPLE_VALUES) == 0)
  {
    if (params.size() != 3 || hkvStringHelper::isEmpty(params[1].getString()))
    {
      out_displayString = "No filter target set.";
    }
    else
    {
      out_displayString = "Change settings in filter manager...";
    }
  }
  else
  {
    out_displayString = "<Multiple Values>";
  }
}


void hkvTagfileFilterSettingsPropertyWidget::UpdateUI()
{
  hkStringBuf displayString;
  makeDisplayString(m_propertyValue, displayString);
  m_label->setText(tr(displayString));

  const hkvProperty::VariantValueList& params = m_propertyValue.getArray();

  if ((m_propertyValue.getFlags() & hkvProperty::FLAG_MULTIPLE_VALUES) == 0)
  {
    if (params.size() != 3 || hkvStringHelper::isEmpty(params[1].getString()))
    {
      m_button->setEnabled(false);
    }
    else
    {
      m_button->setEnabled(true);
    }
  }
  else
  {
    m_button->setEnabled(false);
  }
}


void hkvTagfileFilterSettingsPropertyWidget::OnButtonClicked()
{
  if (m_propertyValue.getType() != hkvProperty::TYPE_ARRAY)
  {
    return;
  }

  const hkvProperty::VariantValueList& params = m_propertyValue.getArray();
  if (params.size() != 3)
  {
    return;
  }

  hkStringBuf settingsFile;
  if (hkvFileHelper::getSystemTempPath(settingsFile) != HK_SUCCESS)
  {
    return;
  }

  if (!hkvFileHelper::fileExists(params[0].getString()))
  {
    QString sError = QString::fromUtf8("The tagfile could not be found, please transform the asset first to be able to make changes.");
    QMessageBox::warning(this, QString::fromUtf8("Opening Stand-Alone Filter Manager failed!"), sError, QMessageBox::Ok, 0);
    return;
  }
  

  settingsFile.pathNormalize();
  settingsFile.pathAppend("TagfileSettings");
  settingsFile.appendPrintf("%08x%08x.hko", GetCurrentProcessId(), GetCurrentThreadId());

  // Write filter settings to file.
  {
    hkRefPtr<hkStreamWriter> optionsWriter = hkFileSystem::getInstance().openWriter(settingsFile);
    if (optionsWriter != HK_NULL)
    {
      optionsWriter->write(params[2].getString(), strlen(params[2].getString()));
    }
    else
    {
      QString sError = QString::fromUtf8("The temporary settings file '%1' for the stand-alone filter manager could not be written!").arg(QString::fromUtf8(settingsFile));
      QMessageBox::critical(this, QString::fromUtf8("Opening Stand-Alone Filter Manager failed!"), sError, QMessageBox::Ok, 0);
      return;
    }
  }
  
  // Build command line
  hkStringBuf arguments;
  hkvExternalToolFilterManager::getFilterManagerCmdArguments(params[0].getString(), params[1].getString(), settingsFile, true, arguments);
  hkStringBuf hctPath;
  if (!hkvExternalToolFilterManager::getHctPath(hctPath))
  {
    QString sError = QString::fromUtf8("The path to the HCT stand-alone filter manager could not be found! Make sure the HCT is installed correctly.");
    QMessageBox::critical(this, QString::fromUtf8("Opening stand-alone filter manager failed!"), sError, QMessageBox::Ok, 0);
    return;
  }

  hctPath.pathAppend("hctStandAloneFilterManager.exe");
  hctPath.prepend("\"");
  hctPath.append("\"");

  // Start process
  QProcess process(this);
  process.setNativeArguments(QString::fromUtf8(arguments));

  hkvQtDialogWrapper dlgWrapper(this);
  QString sDialogText = QString::fromUtf8("The external process '%1' is currently being executed and vForge is waiting for it to return."
    " This window will close as soon as the external process exits.").arg(QString::fromUtf8("Stand-Alone Filter Manager"));
  hkvVForgeDlg hostDlg(tr("Waiting for external process..."), sDialogText, tr(":/Icons/Icons/trafficlight_red.png"), &dlgWrapper);
  hkvWaitForProcessDlg* waitDlg = new hkvWaitForProcessDlg(&hostDlg, process, QString::fromUtf8(hctPath), QString::fromUtf8("Stand-Alone Filter Manager"));
  hostDlg.embedWidget(waitDlg);

  hkvAssetManager::getGlobalInstance()->setMainThreadQueryEvalEnabled(false);
  auto scopeGuard = onScopeExit([&]
  {
    hkvAssetManager::getGlobalInstance()->setMainThreadQueryEvalEnabled(true);
  });
  hostDlg.exec();


  // Read back filter settings from file.
  hkArray<char>::Temp optionsBuf;
  if(!hkLoadUtil(settingsFile).toArray(optionsBuf))
  {
    QString sError = QString::fromUtf8("The temporary settings file '%1' for the stand-alone filter manager could not be read back!").arg(QString::fromUtf8(settingsFile));
    QMessageBox::critical(this, QString::fromUtf8("Processing Stand-Alone Filter Manager settings failed!"), sError, QMessageBox::Ok, 0);
    return;
  }
  hkFileSystem::getInstance().remove(settingsFile);

  hkStringPtr newSettings(optionsBuf.begin(), optionsBuf.getSize());
  
  // Set new properties
  hkvProperty::VariantValueList newParams;
  newParams.push_back("");
  newParams.push_back("");
  newParams.push_back(newSettings.cString());

  // Check for modifications
  if (hkvStringHelper::safeCompare(params[2].getString(), newSettings.cString(), false) == 0)
    return;

  m_propertyValue = hkvProperty(m_propertyValue.getName(), newParams, m_propertyValue.getCustomType());
  emit valueChanged();
}

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
