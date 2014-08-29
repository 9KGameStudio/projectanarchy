/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKV_ASSET_VARIANTS_DLG_HPP
#define HKV_ASSET_VARIANTS_DLG_HPP

#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/AssetBrowserQtPCH.h> 

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Assets/hkvAsset.hpp>

ANALYSIS_IGNORE_ALL_START
#include <QDialog>
#include <QDialogButtonBox>
#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/GeneratedFiles/ui_hkvAssetVariantsDlg.h>
#include <set>
ANALYSIS_IGNORE_ALL_END

#include <QStyledItemDelegate>

class ComboBoxItemDelegate : public QStyledItemDelegate
{
  Q_OBJECT

public:
  ComboBoxItemDelegate(QObject *parent, std::set<QString>* pContent);

  virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
  virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
  virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

private:
  std::set<QString>* m_pContent;
};

class hkvAssetVariantsDlg : public QWidget, public Ui_hkvAssetVariantsDlg
{
  Q_OBJECT

private:
  class TableModel: public QAbstractTableModel
  {
  public:
    TableModel(hkvAssetVariantsDlg& owner);

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const HKV_OVERRIDE;
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const HKV_OVERRIDE;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const HKV_OVERRIDE;
    virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) HKV_OVERRIDE;
    virtual Qt::ItemFlags flags(const QModelIndex& index) const HKV_OVERRIDE;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const HKV_OVERRIDE;

    void setVariantTarget(int row, const char* target);

    using QAbstractTableModel::reset;

  private:
    hkvAssetVariantsDlg& m_owner;
  };


public:
  hkvAssetVariantsDlg(hkvAsset::Variants& variants, QWidget* parent = 0, Qt::WindowFlags f = 0);
  virtual ~hkvAssetVariantsDlg();


signals:
  void AvailableButtonsChanged(QDialogButtonBox::StandardButtons buttons);
  void ButtonStateChanged(QDialogButtonBox::StandardButton button, bool enabled, QString text);
  void RequestDialogClose(int returnCode);

public slots:
  void OnDialogButtonClicked(QDialogButtonBox::StandardButton button);
  void OnWidgetEmbedded(QWidget* widget);

private slots:
  void OnAddButtonClicked();
  void OnRemoveButtonClicked();
  void OnSelectButtonClicked();
  void OnTableSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

private:
  int getSelectedVariantIndex() const;

  void selectTableRow(int row);

  void setElidedText(QLabel& label, const QString& text, int explicitWidth = -1, Qt::TextElideMode mode = Qt::ElideRight);

  void updateControlState();
  void updateSelectedAsset();

  void FindAllUsedVariantKeys();

private:
  hkvAsset::Variants& m_variants;
  TableModel* m_tableModel;

  hkvAsset::RefCPtr m_selectedAsset;
  QString m_assetTypeText;
  QString m_assetFileText;
  QString m_formatInfoText;
  std::set<QString> m_KnownVariantKeys;

  QIcon m_IconVariant;
  QIcon m_IconAssetType;
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
