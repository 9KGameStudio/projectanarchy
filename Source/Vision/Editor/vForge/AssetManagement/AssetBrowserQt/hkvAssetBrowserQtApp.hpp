/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvAssetBrowserQtApp.hpp

#ifndef HKV_ASSET_BROWSER_QT_APP_HPP_INCLUDED
#define HKV_ASSET_BROWSER_QT_APP_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/AssetBrowserQtPCH.h> 

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvCallback.hpp>

ANALYSIS_IGNORE_ALL_START
#include <QApplication>
#include <QMutex>
#include <QTimer>
ANALYSIS_IGNORE_ALL_END

#include <vector>

/// \brief
///   Qt application descendant that drives the asset browser.
class hkvAssetBrowserQtApplication : public QApplication, public hkvCallbackHandler
{
  Q_OBJECT

public:
  hkvAssetBrowserQtApplication(int& argc, char** argv);
  virtual ~hkvAssetBrowserQtApplication();

public:
  virtual void handleCallback(hkvCallbackData* data) HKV_OVERRIDE;

private:
  static bool windowsEventFilter(void *message, long *result);

signals:
  /// \brief
  ///   Signal sent when one or more assets have changed and need to be refreshed in any
  ///   view showing them.
  /// \param assets
  ///   the list of changed assets
  void assetsChanged(const std::vector<hkvAsset::RefCPtr>& assets);

private slots:
  void onUpdateHeartbeat();

private:
  QMutex m_changedAssetsProtect;
  QTimer m_updateHeartbeat;

  std::vector<hkvAsset::RefCPtr> m_changedAssets;
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
