/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/VisionAssetsPCH.h>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/Assets/hkvPrefabAsset.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Assets/hkvAssetTypeManager.hpp>

#include <Vision/Runtime/Base/ThirdParty/tinyXML/TinyXMLHelper.hpp>

#include <Common/Base/System/Io/IStream/hkIStream.h>
#include <Common/Serialize/Serialize/Xml/hkXmlObjectReader.h>


hkvAssetTypeInfo* hkvPrefabAsset::s_typeInfo = NULL;
const hkvAssetTypeInfoHandle* hkvPrefabAsset::s_typeInfoHandle = NULL;

/////////////////////////////////////////////////////////////////////////////
// hkvPrefabAsset static functions
/////////////////////////////////////////////////////////////////////////////

void hkvPrefabAsset::StaticInit()
{
  s_typeInfo = new hkvAssetTypeInfo();
  s_typeInfo->m_name = "Prefab";
  s_typeInfo->m_createFunc = &CreateAsset;
  s_typeInfo->m_generateThumbnailFunc = &GenerateThumbnail;
  s_typeInfo->m_supportedFileExtensions.pushBack("prefab");
  s_typeInfo->m_useEngineForThumbnails = false;
  s_typeInfo->m_szTypeIconQt = ":/Icons/Icons/PrefabAsset.png";

  // register at the hkvAssetTypeManager and store the asset type handle in static variable.
  s_typeInfoHandle = hkvAssetTypeManager::getGlobalInstance()->addAssetType(*s_typeInfo);
}


void hkvPrefabAsset::StaticDeInit()
{
  // de-register at the hkvAssetTypeManager
  hkvAssetTypeManager::getGlobalInstance()->removeAssetType(*s_typeInfoHandle);
  s_typeInfoHandle = NULL;

  delete s_typeInfo;
  s_typeInfo = NULL;
}


hkvAsset* hkvPrefabAsset::CreateAsset()
{
  hkvAsset* pAsset = new hkvPrefabAsset;
  VASSERT(pAsset)

    return pAsset;
}


bool hkvPrefabAsset::GenerateThumbnail(const char* libraryPath, const char* assetPath, const char* thumbnailPath)
{
  hkStringBuf sFilename(libraryPath);
  sFilename.pathAppend(assetPath);

  TiXmlDocument doc;
  if (!doc.LoadFile(sFilename.cString()))
  {
    return true;
  }

  if (doc.RootElement() == NULL)
    return true;

  // Proceed if any only if there is actually a thumbnail node
  TiXmlElement* pThumbnailNode = doc.RootElement()->FirstChildElement("thumbnail");
  if (pThumbnailNode == NULL)
    return true;

  IVFileOutStream* pFile = VFileAccessManager::GetInstance()->Create(thumbnailPath);
  if (pFile == NULL)
    return true;

  int iSize = 0;
  pThumbnailNode->QueryIntAttribute("size", &iSize);

  // Convert base64 encoded data back to binary form
  char* imageData = new char[iSize];
  hkXmlObjectReader::base64read(hkIstream(pThumbnailNode->GetText(), (int)strlen(pThumbnailNode->GetText())).getStreamReader(), imageData, iSize);

  // Write image to disk
  pFile->Write(imageData, iSize);
  pFile->Close();

  delete[] imageData;

  return true;
}


/////////////////////////////////////////////////////////////////////////////
// hkvPrefabAsset public functions
/////////////////////////////////////////////////////////////////////////////

hkvPrefabAsset::hkvPrefabAsset() : hkvAsset(s_typeInfo)
{

}


hkvPrefabAsset::~hkvPrefabAsset()
{

}


/////////////////////////////////////////////////////////////////////////////
// hkvPrefabAsset public override functions
/////////////////////////////////////////////////////////////////////////////

const hkvAssetTypeInfoHandle& hkvPrefabAsset::getTypeInfoHandle() const
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
