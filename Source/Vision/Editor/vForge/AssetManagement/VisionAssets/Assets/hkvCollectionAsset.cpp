/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/VisionAssetsPCH.h>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/Assets/hkvCollectionAsset.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Collection/hkvCollection.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Assets/hkvAssetTypeManager.hpp>

hkvAssetTypeInfo* hkvCollectionAsset::s_typeInfo = NULL;
const hkvAssetTypeInfoHandle* hkvCollectionAsset::s_typeInfoHandle = NULL;

/////////////////////////////////////////////////////////////////////////////
// hkvCollectionAsset static functions
/////////////////////////////////////////////////////////////////////////////

void hkvCollectionAsset::StaticInit()
{
  s_typeInfo = new hkvAssetTypeInfo();
  s_typeInfo->m_name = "Collection";
  s_typeInfo->m_createFunc = &CreateAsset;
  s_typeInfo->m_determineDependenciesFunc = &DetermineDependencies;
  s_typeInfo->m_supportedFileExtensions.pushBack("vcollection");
  s_typeInfo->m_szTypeIconQt = ":/Icons/Icons/CollectionAsset.png";

  // register at the hkvAssetTypeManager and store the asset type handle in static variable.
  s_typeInfoHandle = hkvAssetTypeManager::getGlobalInstance()->addAssetType(*s_typeInfo);
}


void hkvCollectionAsset::StaticDeInit()
{
  // de-register at the hkvAssetTypeManager
  hkvAssetTypeManager::getGlobalInstance()->removeAssetType(*s_typeInfoHandle);
  s_typeInfoHandle = NULL;

  delete s_typeInfo;
  s_typeInfo = NULL;
}


hkvAsset* hkvCollectionAsset::CreateAsset()
{
  hkvAsset* pAsset = new hkvCollectionAsset;
  VASSERT(pAsset)

  return pAsset;
}

bool hkvCollectionAsset::DetermineDependencies(const char* libraryPath, const char* assetPath, std::vector<hkStringPtr>& out_dependencies)
{
  hkvCollection collection;
  hkStringBuf sAbsolutePath(libraryPath);
  sAbsolutePath.pathAppend(assetPath);

  hkResult res = collection.loadCollection(sAbsolutePath.cString());
  if (res == HK_FAILURE)
    return false;

  std::map<hkStringBuf, hkUint32> entryMap;
  collection.buildEntryMap(entryMap, assetPath);

  std::map<hkStringBuf, hkUint32>::iterator it = entryMap.begin();
  for (; it != entryMap.end(); ++it)
  {
    out_dependencies.push_back(it->first.cString());
  }

  return true;
}


/////////////////////////////////////////////////////////////////////////////
// hkvCollectionAsset public functions
/////////////////////////////////////////////////////////////////////////////

hkvCollectionAsset::hkvCollectionAsset() : hkvAsset(s_typeInfo)
{

}


hkvCollectionAsset::~hkvCollectionAsset()
{

}


/////////////////////////////////////////////////////////////////////////////
// hkvCollectionAsset public override functions
/////////////////////////////////////////////////////////////////////////////

const hkvAssetTypeInfoHandle& hkvCollectionAsset::getTypeInfoHandle() const
{
  return *s_typeInfoHandle;
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
