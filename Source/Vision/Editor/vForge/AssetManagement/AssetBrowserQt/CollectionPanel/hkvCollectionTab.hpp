/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvCollectionTab.hpp

#ifndef VKVCOLLECTIONTAB_HPP_INCLUDED
#define VKVCOLLECTIONTAB_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/AssetBrowserQtPCH.h> 

ANALYSIS_IGNORE_ALL_START
#include <QWidget>
ANALYSIS_IGNORE_ALL_END

class hkvCollectionModel;
class hkvCollectionQt;
class hkvCollectionView;

class hkvCollectionTab : public QWidget
{
  Q_OBJECT
public:
  ASSETBROWSER_IMPEXP hkvCollectionTab(QWidget* parent);
  ASSETBROWSER_IMPEXP ~hkvCollectionTab();

  ASSETBROWSER_IMPEXP const char* GetAbsolutePath() const;
  ASSETBROWSER_IMPEXP hkStringPtr GetTabName() const;
  ASSETBROWSER_IMPEXP hkResult LoadCollection(const char* szAbsolutePath);
  ASSETBROWSER_IMPEXP hkResult SaveCollection(const char* szAbsolutePath = NULL);
  ASSETBROWSER_IMPEXP hkResult TransformCollection(const char* szAbsolutePath) const;

  ASSETBROWSER_IMPEXP hkUint32 addEntries(const std::vector<hkStringPtr>& relativePaths);

  ASSETBROWSER_IMPEXP const hkvCollectionQt* GetCollection() const { return m_pCollection; }

private: 
  void SetupUI();

private:
  hkvCollectionQt* m_pCollection;
  hkvCollectionModel* m_pModel;
  hkvCollectionView* m_pView;
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
