/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file AssetBrowserQtHelper.hpp

#ifndef VASSETBROWSERQTHELPER_HPP_INCLUDED
#define VASSETBROWSERQTHELPER_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetDecl.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvCallback.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Assets/hkvAsset.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetLibrary.hpp>

extern const char* szValidJsonRegExp;
extern const char* szValidJsonToolTip;
extern const char* szValidName;

class QWidget;
class QObject;

template <class TA> class hkvScopedSetVariable
{
public:
  hkvScopedSetVariable(TA& pData, TA setValue)
  {
    m_oldValue = pData;
    m_pData = &pData;
    *m_pData = setValue;
  }

  ~hkvScopedSetVariable()
  {
    *m_pData = m_oldValue;
  }

  TA m_oldValue;
  TA* m_pData;
};


/// \brief
///   Helper class that disables the GUI updates of a QWidget
///   and resets the update flag to the previous value once it runs out of scope.
class hkvScopedDisableUpdates
{
public:
  hkvScopedDisableUpdates(QWidget* pWidget);
  ~hkvScopedDisableUpdates();

private:
  QWidget* m_pWidget;
};


/// \brief
///   Helper class that disables the invocation of signals of a QObject
///   and resets the signal flag to the previous value once it runs out of scope.
class hkvScopedBlockSignals
{
public:
  hkvScopedBlockSignals(QObject* pObject);
  ~hkvScopedBlockSignals();

private:
  QObject* m_pObject;
  bool m_bPreviousState;
};


/// \brief
///   Callback data for hkvQtHelper::OnOpenFileDialog. Fill in the public data and call ShowDialog
///   to show vForge's C# OpenFileDialog or CreateFileDialog. The resulting filename is stored in m_sFileName.
class FileDialogData : public hkvCallbackData
{
public:
  ASSETBROWSER_IMPEXP FileDialogData(const hkvCallback* sender)
    : hkvCallbackData(sender), m_bSuccess(false), m_bCreateFile(false), m_bShowBaseData(true) {}

  /// \brief
  ///   Calls hkvCallbackData::trigger() and returns m_bSuccess.
  ASSETBROWSER_IMPEXP bool ShowDialog();

public:
  hkBool m_bSuccess;               ///< Whether the c# dialog returned DialogResult::OK.
  hkBool m_bCreateFile;            ///< True if a create file dialog should be shown, false will result in an open file dialog.
  hkBool m_bShowBaseData;          ///< Whether the base data directory should be listed.
  hkStringPtr m_sCaption;          ///< Text displayed in dialog's title and caption bar.
  hkStringPtr m_sDescription;      ///< Test displayed in the caption bar's description.
  hkStringPtr m_sInitialDirectory; ///< Main root directory of the browser.
  hkStringPtr m_sExt;              ///< Extension to append to the file (e.g. ".txt") in case of a create file dialog.
  hkStringPtr m_sFilter;           ///< Semicolon seperated filter list to use. eg. ".bmp;.jpg" in LOWERCASE!
  hkStringPtr m_sFileName;         ///< The file that has been selected via the dialog.
};


/// \brief
///   Static helper class for handling global Qt things in the asset browser.
class hkvQtHelper
{
private:
  static LRESULT CALLBACK messageFilterProc(int nCode, WPARAM wParam, LPARAM lParam);

public:
  /// \brief
  ///   Initializes the Havok base system for this library
  ASSETBROWSER_IMPEXP static void initHavok();

  /// \brief
  ///   Initializes the Qt application that drives the asset browser
  ASSETBROWSER_IMPEXP static void initQt();

  /// \brief
  ///   Deinitializes the Qt application driving the asset browser
  ASSETBROWSER_IMPEXP static void deInitQt();

  /// \brief
  ///   Triggers forwarding of Windows events to Qt.
  ASSETBROWSER_IMPEXP static void callQtProcessEvents();

public:
  /// \brief
  ///   Callback that allows listening for file open dialog requests.
  ASSETBROWSER_IMPEXP static hkvCallbackWrapper OnOpenFileDialog;

private:
  static HHOOK s_messageHook;
};


class hkvAssetUtils
{
public:
  static hkResult TransformAssetsInForeground(const hkArray<hkvAsset::RefPtr>& AssetsToTransform, QWidget* pParent, hkvAssetLibrary::RefPtr pLibrary = NULL, const char* szRelPath = NULL);
  static hkResult TransformAllAssetsInForeground(QWidget* pParent, hkvAssetLibrary::RefPtr pLibrary = NULL, const char* szRelPath = NULL) { return TransformAssetsInForeground(hkArray<hkvAsset::RefPtr>(), pParent, pLibrary, szRelPath); }
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
