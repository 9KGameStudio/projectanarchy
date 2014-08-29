/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvManifestEntryList.hpp

#ifndef HKVMANIFESTENTRYLIST_HPP_INCLUDED
#define HKVMANIFESTENTRYLIST_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/AssetBrowserQtPCH.h> 

ANALYSIS_IGNORE_ALL_START
#include <QWidget>
#include <QToolBar>
#include <QAction>
#include <QIcon>
#include <QString>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetDecl.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetStructs.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvCallback.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetManager.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/GeneratedFiles/ui_hkvManifestEntryList.h>
#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/GeneratedFiles/ui_hkvManifestEditorEntryList.h>
ANALYSIS_IGNORE_ALL_END

class QTableWidgetItem;

/// \brief
///   This is the base class for hkvManifestEntryListEngine and hkvManifestEntryListLibrary
///   which handles their common functionality like managing the data and gui updates.
class hkvManifestEntryList : public QWidget, public Ui_hkvManifestEntryList
{
  Q_OBJECT
public:
  ASSETBROWSER_IMPEXP hkvManifestEntryList(QWidget* pParent);
  ASSETBROWSER_IMPEXP ~hkvManifestEntryList ();

  ASSETBROWSER_IMPEXP void setCaption(const char* szCaption);
  ASSETBROWSER_IMPEXP void setEntryName(const char* szEntry);
  ASSETBROWSER_IMPEXP void setEntryIcon(const QIcon& icon);
  ASSETBROWSER_IMPEXP void SetProjectDir(const QString& projectDir);

  ASSETBROWSER_IMPEXP int findOrAddEntry(const char* szValue);
  ASSETBROWSER_IMPEXP bool removeEntry(unsigned int iIndex);
  ASSETBROWSER_IMPEXP void clearEntries();
  ASSETBROWSER_IMPEXP bool moveEntryUp(unsigned int iIndex);
  ASSETBROWSER_IMPEXP bool moveEntryDown(unsigned int iIndex);

  ASSETBROWSER_IMPEXP int getNumberOfEntries() const;
  ASSETBROWSER_IMPEXP bool getEntry(unsigned int iIndex, QString& out_value) const;
  ASSETBROWSER_IMPEXP int getCurrentIndex() const;

  ASSETBROWSER_IMPEXP virtual void setItem(QTableWidgetItem* pItem, const QString& entry, int iRow, int iColumn){};

  /// \brief
  ///   Updates all stored entries, pass -1 to not change the currently selected row.
  ASSETBROWSER_IMPEXP void UpdateList(int iNewRow = -1);

public slots:
  void on_EntriesList_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);
  virtual void on_EntryDown_clicked(void);
  virtual void on_EntryUp_clicked(void);
  virtual void on_AddEntry_clicked(void) {};
  virtual void on_RemoveEntry_clicked(void);
  virtual void on_UpdateButtons(void);

signals:
  void currentIndexChanged(int);
  void dataChanged();

protected: 
  void setupUI();
  int indexOfEntry(const char* szValue);

protected:
  struct ManifestEntry
  {
    ManifestEntry(){}
    ManifestEntry(const char* szValue)
    {
      m_sValue = QString::fromUtf8(szValue);
    }

    QString m_sValue;
  };
  QVector<ManifestEntry> m_entries;
  QIcon m_entryIcon;
  QString m_entryName;
  QString m_projectDir;
};

/// \brief
///   This class handles manifest entries of the type: 'LoadEnginePlugin'
class hkvManifestEntryListEngine : public hkvManifestEntryList
{
  Q_OBJECT
public:
  ASSETBROWSER_IMPEXP hkvManifestEntryListEngine(QWidget* pParent);
  ASSETBROWSER_IMPEXP ~hkvManifestEntryListEngine ();
  ASSETBROWSER_IMPEXP virtual void setItem(QTableWidgetItem* pItem, const QString& entry, int iRow, int iColumn) HK_OVERRIDE;
  ASSETBROWSER_IMPEXP void SetBinaryDir(const QString& binaryDir);

public slots:
  virtual void on_AddEntry_clicked(void) HK_OVERRIDE;

protected:
  QString getAbsolutePath(const char* szPluginName) const;

private:
  QString m_binaryDir;
};

/// \brief
///   This class handles manifest entries of the type: 'AddDataDir'
class hkvManifestEntryListLibrary : public hkvManifestEntryList
{
  Q_OBJECT
public:
  ASSETBROWSER_IMPEXP hkvManifestEntryListLibrary(QWidget* pParent);
  ASSETBROWSER_IMPEXP ~hkvManifestEntryListLibrary ();

  ASSETBROWSER_IMPEXP virtual void setItem(QTableWidgetItem* pItem, const QString& entry, int iRow, int iColumn) HK_OVERRIDE;
  
  ASSETBROWSER_IMPEXP int findOrAddStaticEntry(const char* szValue);
  ASSETBROWSER_IMPEXP hkvAssetLibrary::RefPtr getAssetLibraryBySearchPath(const QString& sSearchPath) const;
  ASSETBROWSER_IMPEXP int getNumberOfStaticLibraries() const;

public slots:
  virtual void on_EntryDown_clicked(void);
  virtual void on_EntryUp_clicked(void);
  virtual void on_AddEntry_clicked(void);
  virtual void on_UpdateButtons(void);

protected:
  QIcon m_library;
  QIcon m_libraryError;
  int m_iStaticEntries;
};


/// \brief
///   This class handles manifest entries of the type: 'LoadEditorPlugin'
class hkvManifestEntryListEditor : public QWidget, public Ui_hkvManifestEditorEntryList
{
  Q_OBJECT
public:
  ASSETBROWSER_IMPEXP hkvManifestEntryListEditor(QWidget* pParent);
  ASSETBROWSER_IMPEXP ~hkvManifestEntryListEditor ();

  ASSETBROWSER_IMPEXP void setPlugins(const QString& commaSeparatedPluginNames);
  ASSETBROWSER_IMPEXP bool setPluginState(const char* szValue, bool bState);

  ASSETBROWSER_IMPEXP int getNumberOfEntries() const;
  ASSETBROWSER_IMPEXP bool getEntry(unsigned int iIndex, QString& out_plugin, bool& out_bState) const;
  ASSETBROWSER_IMPEXP int getCurrentIndex() const;

public slots:
  void on_EntriesList_currentRowChanged(int currentRow);
  void on_EntriesList_itemChanged (QListWidgetItem* item);
  void on_EntryDown_clicked(void);
  void on_EntryUp_clicked(void);
  void on_UpdateButtons(void);

protected:
  void SetupUI();
  void UpdateList(int iNewRow = -1);

signals:
  void dataChanged();

private:
  QIcon m_entryIcon;

  struct editorPlugin
  {
    editorPlugin() : m_bState(false), m_bFound(false) {}
    editorPlugin(const QString& sName, bool bState, bool bFound) : 
      m_sName(sName), m_bState(bState), m_bFound(bFound) {}

    QString m_sName;
    bool m_bState;
    bool m_bFound;
  };
  QVector<editorPlugin> m_plugins;
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
