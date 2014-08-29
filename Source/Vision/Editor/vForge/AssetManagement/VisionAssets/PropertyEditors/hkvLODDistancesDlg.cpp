/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/AssetManagement/VisionAssets/VisionAssetsPCH.h>

#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/Dialogs/hkvVForgeDlg.hpp>

#include <Vision/Editor/vForge/AssetManagement/VisionAssets/PropertyEditors/hkvLODDistancesDlg.hpp>

ANALYSIS_IGNORE_ALL_START
#include <QStyledItemDelegate>
ANALYSIS_IGNORE_ALL_END


// ----------------------------------------------------------------------------
// hkvLODDistancesDlg
// ----------------------------------------------------------------------------
hkvLODDistancesDlg::hkvLODDistancesDlg(hkvProperty::VariantValueList& distances, QWidget* parent, Qt::WindowFlags)
: QWidget(parent), m_distances(distances)
{
  setupUi(this);

  m_distanceTable->verticalHeader()->setResizeMode(QHeaderView::Fixed);
  m_distanceTable->verticalHeader()->setDefaultSectionSize(fontMetrics().lineSpacing() + 7);

  m_distanceTable->setModel(new TableModel(*this));
}


hkvLODDistancesDlg::~hkvLODDistancesDlg()
{
}


void hkvLODDistancesDlg::OnDialogButtonClicked(QDialogButtonBox::StandardButton button)
{
  switch (button)
  {
  case QDialogButtonBox::Ok:
    {
      emit RequestDialogClose(hkvVForgeDlg::Accepted);
      break;
    }
  case QDialogButtonBox::Cancel:
    {
      emit RequestDialogClose(hkvVForgeDlg::Rejected);
      break;
    }
  }
}


void hkvLODDistancesDlg::OnWidgetEmbedded(QWidget* widget)
{
  emit AvailableButtonsChanged(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
}


// ----------------------------------------------------------------------------
// hkvLODDistancesDlg::TableModel
// ----------------------------------------------------------------------------

hkvLODDistancesDlg::TableModel::TableModel(hkvLODDistancesDlg& owner)
: QAbstractTableModel(&owner), m_owner(owner)
{
}


int hkvLODDistancesDlg::TableModel::rowCount(const QModelIndex& parent) const
{
  return (int)m_owner.m_distances.size();
}


int hkvLODDistancesDlg::TableModel::columnCount(const QModelIndex& parent) const
{
  return 2;
}


QVariant hkvLODDistancesDlg::TableModel::data(const QModelIndex& index, int role) const
{
  switch (index.column())
  {
  case 0:
    {
      if (role == Qt::DisplayRole)
      {
        return QString::number(index.row());
      }
      break;
    }
  case 1:
    {
      switch (role)
      {
      case Qt::DisplayRole:
        return QString::number(m_owner.m_distances[index.row()].getDouble());
      case Qt::EditRole:
        return m_owner.m_distances[index.row()].getDouble();
      }
      break;
    }
  }

  return QVariant();
}


bool hkvLODDistancesDlg::TableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  double val = value.toDouble();
  if (!(val >= 0))
  {
    val = -1;
  }

  m_owner.m_distances[index.row()] = hkvVariantValue(val);

  return true;
}


Qt::ItemFlags hkvLODDistancesDlg::TableModel::flags(const QModelIndex& index) const
{
  Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
  if (index.column() == 1)
  {
    flags |= Qt::ItemIsEditable;
  }
  return flags;
}


QVariant hkvLODDistancesDlg::TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
  {
    switch (section)
    {
    case 0:
      return tr("LOD Level");
    case 1:
      return tr("Distance");
    }
  }

  return QVariant();
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
