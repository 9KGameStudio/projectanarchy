/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


#ifndef VTHUMBNAILCACHE_HPP_INCLUDED
#define VTHUMBNAILCACHE_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/AssetBrowserQtPCH.h> 

ANALYSIS_IGNORE_ALL_START
#include <QAbstractItemModel>
#include <QMutex>
#include <QList>
#include <QPixmap>
#include <QThread>
#include <QCache>
ANALYSIS_IGNORE_ALL_END

class hkvAsset;
struct hkvAssetTypeInfo;

class hkvThumbnailLoader : public QObject
{
  friend class hkvThumbnailCache;
  Q_OBJECT
private:
  hkvThumbnailLoader();
  ~hkvThumbnailLoader();

signals:
  void ThumbnailPixmapReady(int iRow, const QString& assetPath, const QString& thumbnailKey, QImage* pThumbnail);

private slots:
  void OnLoadThumbnail(const QModelIndex& index, const QString& assetLibraryPath, const QString& assetPath, unsigned int typeIndex);

private:
  void doLoadThumbnail(const QString& thumbnailFile, const QString& assetName, const QString& thumbnailKey, int tableRow);

  QPixmap m_LoadingIcon;
  QPixmap m_NoThumbnailIcon;
};

class hkvThumbnailCache : public QObject
{
  Q_OBJECT
public:
  hkvThumbnailCache();
  ~hkvThumbnailCache();
  static hkvThumbnailCache* getGlobalInstance();

  QPixmap GetThumbnail(const QModelIndex& index, const hkvAsset* pAsset);

signals:
  void LoadThumbnail(const QModelIndex& index, const QString& assetLibraryPath, const QString& assetPath, unsigned int typeIndex);
  void ThumbnailLoaded(int iRow, const QString& asset);

private slots:
  void OnThumbnailPixmapReady(int iRow, const QString& assetPath, const QString& thumbnailKey, QImage* pThumbnail);
  void OnThreadStarted();
  void OnThreadFinished();

private:
  hkvThumbnailLoader* m_loader;
  QThread* m_thread;
  QCache<QString, QPixmap> m_cache;

private: // static data
  static hkvThumbnailCache* s_pGlobalInstance;
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
