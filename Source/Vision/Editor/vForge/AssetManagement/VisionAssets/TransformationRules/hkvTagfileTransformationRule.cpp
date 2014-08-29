/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/VisionAssetsPCH.h>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/TransformationRules/hkvTagfileTransformationRule.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/Assets/hkvTagfileAsset.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/TransformationRules/hkvTransformationRuleTypeManager.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Assets/hkvAssetTrackedFile.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/ExternalTools/hkvExternalToolFilterManager.hpp>

#include <Common/Base/System/Io/Writer/Crc/hkCrcStreamWriter.h>
#include <Common/Base/System/Io/Reader/hkStreamReader.h>
#include <Common/Base/Container/String/hkUtf8.h>
#include <vector>
#include <string>

hkUint32 hkvTagfileTransformationRule::s_ruleTypeIndex = HKV_INVALID_INDEX;

/////////////////////////////////////////////////////////////////////////////
// hkvTagfileTransformationRule static functions
/////////////////////////////////////////////////////////////////////////////

void hkvTagfileTransformationRule::staticInit()
{
  VASSERT(s_ruleTypeIndex == HKV_INVALID_INDEX);

  hkvTransformationRuleTypeInfo ti;
  ti.m_name = "TagfileTransformationRule";
  ti.m_createFunc = &createTransformationRule;
  ti.m_supportsAllTypes = false;
  ti.m_supportedAssetTypes.pushBack("Havok Tagfile");
  ti.m_priority = 100;

  // register at the hkvTransformationRuleTypeManager and store the type index in static variable.
  s_ruleTypeIndex = hkvTransformationRuleTypeManager::getGlobalInstance()->addType(ti);
}


void hkvTagfileTransformationRule::staticDeInit()
{
  if (s_ruleTypeIndex == HKV_INVALID_INDEX)
    return;

  // de-register at the hkvTransformationRuleTypeManager
  hkvTransformationRuleTypeManager::getGlobalInstance()->removeType(s_ruleTypeIndex);
  s_ruleTypeIndex = HKV_INVALID_INDEX;
}


hkvTransformationRule* hkvTagfileTransformationRule::createTransformationRule(hkvTargetPlatform targetPlatform)
{
  return new hkvTagfileTransformationRule(targetPlatform);
}


/////////////////////////////////////////////////////////////////////////////
// hkvTagfileTransformationRule public functions
/////////////////////////////////////////////////////////////////////////////

hkvTagfileTransformationRule::hkvTagfileTransformationRule(hkvTargetPlatform platform)
: hkvFilterManagerTransformationRule(platform)
{
}


hkvTagfileTransformationRule::~hkvTagfileTransformationRule()
{
}

void hkvTagfileTransformationRule::getProperties(hkvPropertyList& properties, hkvProperty::Purpose purpose) const
{

}

void hkvTagfileTransformationRule::setProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, unsigned int iStackIndex, hkvProperty::Purpose purpose)
{

}

unsigned int hkvTagfileTransformationRule::getTypeIndex() const
{
  return s_ruleTypeIndex;
}


const char* hkvTagfileTransformationRule::getTypeName() const
{
  return "TagfileTransformationRule";
}

bool hkvTagfileTransformationRule::assetNeedsTransformation(const hkvAsset& asset) const
{
  const hkvFilterManagerTransformableAsset& filterManagerAsset = static_cast<const hkvFilterManagerTransformableAsset&>(asset);
  hkRefPtr<hkvAssetTrackedFile> assetFile(asset.getFile());
  // If the file is no longer present we won't need to transform it anymore.
  if (assetFile == NULL)
  {
    return false;
  }

  hkvFilterManagerTransformableAsset::TagfileEntry& entry = filterManagerAsset.getTagfileArray()[0];
  // We create a new TagfileEntry here for the transformation and set the source and target
  // as there is no point at which we can set those on the asset as we don't know when
  // a file is actually attached to the asset.
  hkvFilterManagerTransformableAsset::TagfileEntry transformEntry;
  transformEntry = entry;

  // Name
  hkStringBuf name = filterManagerAsset.getName();
  hkvStringHelper::pathNoExtension(name);
  transformEntry.m_name = name;

  // Source Path
  hkStringBuf relSourcePath;
  transformEntry.m_sourcePath = filterManagerAsset.getRelativeFilePath(relSourcePath);

  return filterManagerAsset.isTagfileOutputHashOutOfDate(transformEntry);
}

bool hkvTagfileTransformationRule::executeTransformation(const hkvTransformationInput& input, hkvTransformationOutput& output) const
{
  const hkvFilterManagerTransformableAsset* filterManagerAsset = static_cast<const hkvFilterManagerTransformableAsset*>(input.m_pAsset);
  hkvFilterManagerTransformableAsset::TagfileEntry& entry = filterManagerAsset->getTagfileArray()[0];

  // We create a new TagfileEntry here for the transformation and set the source and target
  // as there is no point at which we can set those on the asset as we don't know when
  // a file is actually attached to the asset.
  hkvFilterManagerTransformableAsset::TagfileEntry transformEntry;
  transformEntry = entry;

  // Name
  hkStringBuf name = filterManagerAsset->getName();
  hkvStringHelper::pathNoExtension(name);
  transformEntry.m_name = name;

  // Source Path
  hkStringBuf relSourcePath;
  transformEntry.m_sourcePath = filterManagerAsset->getRelativeFilePath(relSourcePath);

  TagfileContext context(input, output, transformEntry);
  bool ok = executeTagfileEntryTransformation(context) == HK_SUCCESS;
  if (ok)
  {
    entry.m_targetHash = transformEntry.m_targetHash;
  }

  cleanUpTempFiles(context);
  return ok;
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
