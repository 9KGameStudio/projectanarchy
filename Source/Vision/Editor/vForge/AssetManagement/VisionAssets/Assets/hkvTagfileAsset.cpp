/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/VisionAssetsPCH.h>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/Assets/hkvTagfileAsset.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Assets/hkvAssetTypeManager.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetLibrary.hpp>
#include <Common/Base/System/Io/Util/hkLoadUtil.h>

hkvAssetTypeInfo* hkvTagfileAsset::s_typeInfo = NULL;
const hkvAssetTypeInfoHandle* hkvTagfileAsset::s_typeInfoHandle = NULL;

/////////////////////////////////////////////////////////////////////////////
// hkvTagfileAsset static functions
/////////////////////////////////////////////////////////////////////////////

void hkvTagfileAsset::StaticInit()
{
  s_typeInfo = new hkvAssetTypeInfo();
  s_typeInfo->m_name = "Havok Tagfile";
  s_typeInfo->m_createFunc = &CreateAsset;
  s_typeInfo->m_supportedFileExtensions.pushBack("hkt");
  s_typeInfo->m_szTypeIconQt = ":/Icons/Icons/HKT.png";
  s_typeInfo->m_profileIndependentTransform = true;

  // register at the hkvAssetTypeManager and store the asset type handle in static variable.
  s_typeInfoHandle = hkvAssetTypeManager::getGlobalInstance()->addAssetType(*s_typeInfo);
}


void hkvTagfileAsset::StaticDeInit()
{
  // de-register at the hkvAssetTypeManager
  hkvAssetTypeManager::getGlobalInstance()->removeAssetType(*s_typeInfoHandle);
  s_typeInfoHandle = NULL;

  delete s_typeInfo;
  s_typeInfo = NULL;
}


hkvAsset* hkvTagfileAsset::CreateAsset()
{
  return new hkvTagfileAsset;
}


/////////////////////////////////////////////////////////////////////////////
// hkvTagfileAsset public functions
/////////////////////////////////////////////////////////////////////////////

hkvTagfileAsset::hkvTagfileAsset()
  : hkvFilterManagerTransformableAsset(s_typeInfo)
{
  m_tagfiles.push_back(TagfileEntry());
  TagfileEntry& entry = m_tagfiles.back();
  entry.m_name = "Tagfile";
}

hkvTagfileAsset::~hkvTagfileAsset()
{
}


/////////////////////////////////////////////////////////////////////////////
// hkvTagfileAsset public override functions
/////////////////////////////////////////////////////////////////////////////

const hkvAssetTypeInfoHandle& hkvTagfileAsset::getTypeInfoHandle() const
{
  return *s_typeInfoHandle;
}

void hkvTagfileAsset::getInternalProperties(hkvPropertyList& properties, hkvProperty::Purpose purpose) const
{
  int iFlags = (purpose == hkvProperty::PURPOSE_USER_INTERFACE_READ_ONLY) ? hkvProperty::FLAG_READ_ONLY : 0;
  if (purpose == hkvProperty::PURPOSE_SERIALIZATION)
  {
    m_tagfiles[0].getTagfileEntryProperties(properties, purpose);
  }
  else
  {
    const TagfileEntry& entry = m_tagfiles[0];
    
    hkStringBuf absSourcePath;
    getAbsoluteFilePath(absSourcePath);

    hkStringBuf relAssetFolder;
    getRelativeFilePath(relAssetFolder);
    relAssetFolder.pathDirname();

    hkStringBuf name = getName();
    hkvStringHelper::pathNoExtension(name);

    hkStringBuf relTargetPath;
    entry.getRelativeTargetPath(relTargetPath, relAssetFolder, name);

    const char* absLibraryPath = getOwnerLibrary()->getPath();
    hkStringBuf absTargetPath = absLibraryPath;
    absTargetPath.pathAppend(relTargetPath);

    entry.getTagfileEntryProperties(properties, purpose, absSourcePath, absTargetPath, absLibraryPath);
  }
}

void hkvTagfileAsset::setInternalProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, unsigned int stackIndex, hkvProperty::Purpose purpose)
{
  int stackSize = (path.getSize() - stackIndex);

  if ((stackSize >= 1) && (hkvStringHelper::safeCompare(path[stackIndex + 0], "Tagfile") == 0))
  {
    setTagfileArrayProperty(prop, path, stackIndex, purpose);
  }
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
