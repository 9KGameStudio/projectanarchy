/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/VisionAssetsPCH.h>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/Assets/hkvFbxAsset.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Assets/hkvAssetTypeManager.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetLibrary.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Assets/hkvAssetTrackedFile.hpp>

hkvAssetTypeInfo* hkvFbxAsset::s_typeInfo = NULL;
const hkvAssetTypeInfoHandle* hkvFbxAsset::s_typeInfoHandle = NULL;

const char* hkvFbxTargetNames[] =
{
  "None",
  "Single Output",
  "Multiple Outputs",
};

HK_COMPILE_TIME_ASSERT(V_ARRAY_SIZE(hkvFbxTargetNames) == HKV_FBX_TARGET_COUNT);

hkvEnumDefinition hkvFbxAsset::s_fbxTargetDefinition(HKV_FBX_TARGET_COUNT, hkvFbxTargetNames);

/////////////////////////////////////////////////////////////////////////////
// hkvFbxAsset static functions
/////////////////////////////////////////////////////////////////////////////

void hkvFbxAsset::StaticInit()
{
  s_typeInfo = new hkvAssetTypeInfo();
  s_typeInfo->m_name = "FBX";
  s_typeInfo->m_createFunc = &CreateAsset;
  s_typeInfo->m_supportedFileExtensions.pushBack("fbx");
  s_typeInfo->m_szTypeIconQt = ":/Icons/Icons/FBX.png";
  s_typeInfo->m_profileIndependentTransform = true;

  // register at the hkvAssetTypeManager and store the asset type handle in static variable.
  s_typeInfoHandle = hkvAssetTypeManager::getGlobalInstance()->addAssetType(*s_typeInfo);
}


void hkvFbxAsset::StaticDeInit()
{
  // de-register at the hkvAssetTypeManager
  hkvAssetTypeManager::getGlobalInstance()->removeAssetType(*s_typeInfoHandle);
  s_typeInfoHandle = NULL;

  delete s_typeInfo;
  s_typeInfo = NULL;
}


hkvAsset* hkvFbxAsset::CreateAsset()
{
  hkvAsset* pAsset = new hkvFbxAsset;
  VASSERT(pAsset)

  return pAsset;
}


/////////////////////////////////////////////////////////////////////////////
// hkvFbxAsset public functions
/////////////////////////////////////////////////////////////////////////////

hkvFbxAsset::hkvFbxAsset() : hkvFilterManagerTransformableAsset(s_typeInfo), m_fbxTarget(s_fbxTargetDefinition)
{

}


hkvFbxAsset::~hkvFbxAsset()
{

}

hkvFbxTarget hkvFbxAsset::getFbxTarget() const
{
  return (hkvFbxTarget)m_fbxTarget.getDefinitionId();
}

hkUint32 hkvFbxAsset::getFbxHash() const
{
  hkRefPtr<hkvAssetTrackedFile> assetFile(getFile());
  if (assetFile == NULL)
  {
    return 0;
  }

  hkUint32 assetHash = assetFile->getStoredHash();
  return assetHash + (hkUint32)getFbxTarget();
}

const char* hkvFbxAsset::makeFbxTargetName(hkStringBuf& out_filename) const
{
  hkStringBuf hashString;
  hkvStringHelper::encodeNumberBase32Mod(getFbxHash(), hashString);

  hkStringBuf targetBaseName;
  getRelativeFilePath(targetBaseName);
  targetBaseName.pathBasename();
  hkvStringHelper::pathNoExtension(targetBaseName);

  static const int maxNameLen = 40;

  out_filename.clear();
  hkStringBuf variantPartBuf(hashString);
  int availableBasePartLen = maxNameLen - variantPartBuf.getLength();

  if (availableBasePartLen > 0)
  {
    int toCopy = hkvMath::Min(targetBaseName.getLength(), availableBasePartLen);
    out_filename.append(targetBaseName, toCopy);
  }
  out_filename.append(variantPartBuf);

  hkvStringHelper::toLowerCase(out_filename);

  return out_filename;

  
}


/////////////////////////////////////////////////////////////////////////////
// hkvFbxAsset public override functions
/////////////////////////////////////////////////////////////////////////////

const hkvAssetTypeInfoHandle& hkvFbxAsset::getTypeInfoHandle() const
{
  return *s_typeInfoHandle;
}

void hkvFbxAsset::getInternalProperties(hkvPropertyList& properties, hkvProperty::Purpose purpose) const
{
  int iFlags = (purpose == hkvProperty::PURPOSE_USER_INTERFACE_READ_ONLY) ? hkvProperty::FLAG_READ_ONLY : 0;

  properties.push_back(hkvProperty::groupStart("FBX"));
  properties.back().setDescription("Asset specific properties and settings.");
  {
    if (purpose == hkvProperty::PURPOSE_SERIALIZATION)
    {
      properties.push_back(hkvProperty("FBX Target", m_fbxTarget.getString(), hkvProperty::TYPE_STRING));
      properties.back().setDefaultValue(m_fbxTarget.getStringForAvailableElementsId(0), false);
    }
    else
    {
      properties.push_back(hkvProperty("FBX Target", m_fbxTarget.getAvailableElementsId(),
        m_fbxTarget.getAvailableElementStrings(), iFlags,
        "The FBX target defines how animations inside the FBX file are handled. Select 'Single output' if you want all animation takes to be merged into one output. "
        "Also select this option if you want to create a vmesh or a model file with animations.\n"
        "Select 'Multiple Outputs' if each animation take in the FBX file should have its separate transformation as required by Havok Animation Studio."));
      if (getFbxTarget() != HKV_FBX_TARGET_NONE)
      {
        properties.back().getMetaInfo()[hkvProperty::ENUM_META_CONFIRMATION_MESSAGE] = hkvVariantValue("Changing the 'FBX Target' will reset the output targets, do you want to continue?", false);
      }

      if (m_tagfiles.empty())
      {
        properties.push_back(hkvProperty("INFO", "Transform required.", hkvProperty::TYPE_STRING, hkvProperty::FLAG_READ_ONLY,
          "Please transform the FBX Asset once to generate the list of possible targets."));
      }
    }
    getTagfileArrayProperties(properties, purpose);
  }
  properties.push_back(hkvProperty::groupEnd());

  hkvFilterManagerTransformableAsset::getInternalProperties(properties, purpose);
}

void hkvFbxAsset::setInternalProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, unsigned int stackIndex, hkvProperty::Purpose purpose)
{
  int stackSize = (path.getSize() - stackIndex);

  if ((stackSize >= 1) && (hkvStringHelper::safeCompare(path[stackIndex + 0], "FBX") == 0))
  {
    if (stackSize == 1)
    {
      if (purpose == hkvProperty::PURPOSE_SERIALIZATION)
      {
        if (hkvStringHelper::safeCompare(prop.getName(), "FBX Target") == 0)
        {
          if (m_fbxTarget.setByString(prop.getString()) != HK_SUCCESS)
          {
            m_fbxTarget.setByAvailableElementsId(0);
          }
        }
      }
      else
      {
        if (hkvStringHelper::safeCompare(prop.getName(), "FBX Target") == 0)
        {
          if (m_fbxTarget.getAvailableElementsId() != prop.getEnumValue())
          {
            hkvFbxTarget oldTarget = (hkvFbxTarget)m_fbxTarget.getDefinitionId();
            m_fbxTarget.setByAvailableElementsId(prop.getEnumValue());
            m_tagfiles.clear();
          }
        }
      }
    }
    setTagfileArrayProperty(prop, path, stackIndex + 1, purpose);
  }

  hkvFilterManagerTransformableAsset::setInternalProperty(prop, path, stackIndex, purpose);
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
