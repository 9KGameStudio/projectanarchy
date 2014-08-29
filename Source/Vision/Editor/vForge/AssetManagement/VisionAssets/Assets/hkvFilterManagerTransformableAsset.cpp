/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/VisionAssetsPCH.h>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/Assets/hkvFilterManagerTransformableAsset.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Assets/hkvAssetTypeManager.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetManager.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetLibrary.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Assets/hkvAssetTrackedFile.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Transformation/hkvTransformationHelper.hpp>
#include <Common/Base/System/Io/Util/hkLoadUtil.h>
#include <Common/Base/System/Io/Reader/hkStreamReader.h>
#include <Common/Base/System/Io/Writer/Crc/hkCrcStreamWriter.h>

const char* const hkvFilterManagerTransformableAsset::s_filterManagerSettingsCustomType = "TagfileFilterSettings";

const char* hkvFilterManagerTargetNames[] =
{
  "None",
  "Model",
  "StaticMesh",
  "Character Asset",
  "Character Animation"
};

HK_COMPILE_TIME_ASSERT(V_ARRAY_SIZE(hkvFilterManagerTargetNames) == HKV_FILTER_MANAGER_TARGET_COUNT);

hkvEnumDefinition hkvFilterManagerTransformableAsset::s_filterManagerTargetDefinition(HKV_FILTER_MANAGER_TARGET_COUNT, hkvFilterManagerTargetNames);

/////////////////////////////////////////////////////////////////////////////
// hkvFilterManagerTransformableAsset static functions
/////////////////////////////////////////////////////////////////////////////

const char* hkvFilterManagerTransformableAsset::getFilterTargetFileExtension(hkvFilterManagerTarget filterTarget)
{
  switch (filterTarget)
  {
  case HKV_FILTER_MANAGER_TARGET_NONE:
    return "";
  case HKV_FILTER_MANAGER_TARGET_MODEL:
    return "model";
  case HKV_FILTER_MANAGER_TARGET_STATICMESH:
    return "vmesh";
  case HKV_FILTER_MANAGER_TARGET_CHARACTER_ASSET:
  case HKV_FILTER_MANAGER_TARGET_CHARACTER_ANIMATION:
    return "hkt";
  default:
    return "";
  }
}


/////////////////////////////////////////////////////////////////////////////
// hkvFilterManagerTransformableAsset::TagfileEntry public functions
/////////////////////////////////////////////////////////////////////////////

hkvFilterManagerTransformableAsset::TagfileEntry::TagfileEntry()
  : m_filterManagerTarget(s_filterManagerTargetDefinition), m_targetHash(0)
{
}

hkvFilterManagerTransformableAsset::TagfileEntry::TagfileEntry(const hkvFilterManagerTransformableAsset::TagfileEntry& rhs)
{
  *this = rhs;
}

void hkvFilterManagerTransformableAsset::TagfileEntry::operator=(const hkvFilterManagerTransformableAsset::TagfileEntry& rhs)
{
  m_name = rhs.m_name.cString();
  m_sourcePath = rhs.m_sourcePath.cString();
  m_filterManagerTarget = rhs.m_filterManagerTarget;
  m_targetDirectory = rhs.m_targetDirectory.cString();
  m_settings = rhs.m_settings.cString();
  m_targetHash = rhs.m_targetHash;
}

const char* hkvFilterManagerTransformableAsset::TagfileEntry::getName() const
{
  return m_name;
}

hkvFilterManagerTarget hkvFilterManagerTransformableAsset::TagfileEntry::getFilterTarget() const
{
  return (hkvFilterManagerTarget)m_filterManagerTarget.getDefinitionId();
}

const char* hkvFilterManagerTransformableAsset::TagfileEntry::getSettings() const
{
  return m_settings;
}

const char* hkvFilterManagerTransformableAsset::TagfileEntry::getRelativeTargetPath(hkStringBuf& out_path, const char* relativeAssetFolder, const char* name) const
{
  out_path.clear();
  if (hkvStringHelper::isEmpty(m_targetDirectory))
  {
    out_path = relativeAssetFolder;
  }
  else
  {
    out_path = m_targetDirectory;
  }
  
  if (name)
  {
    out_path.pathAppend(name);
  }
  else
  {
    out_path.pathAppend(m_name);
  }
  out_path.append(".");
  out_path.append(getFilterTargetFileExtension(getFilterTarget()));
  return out_path;
}

void hkvFilterManagerTransformableAsset::TagfileEntry::getTagfileEntryProperties(hkvPropertyList& properties, hkvProperty::Purpose purpose,
                                                                                 const char* absSourcePath, const char* absTargetPath, const char* absLibraryPath) const
{
  int iFlags = (purpose == hkvProperty::PURPOSE_USER_INTERFACE_READ_ONLY) ? hkvProperty::FLAG_READ_ONLY : 0;

  properties.reserve(properties.size() + 10);
  properties.push_back(hkvProperty::groupStart(m_name));
  properties.back().setDescription("Asset specific properties and settings.");

  if (purpose == hkvProperty::PURPOSE_SERIALIZATION)
  {
    properties.push_back(hkvProperty("Filter Target", m_filterManagerTarget.getString(), hkvProperty::TYPE_STRING));
    properties.back().setDefaultValue(m_filterManagerTarget.getStringForAvailableElementsId(0), false);
    properties.push_back(hkvProperty("Source", m_sourcePath, hkvProperty::TYPE_STRING));
    properties.push_back(hkvProperty("Target Directory", m_targetDirectory, hkvProperty::TYPE_STRING));
    properties.push_back(hkvProperty("Settings", m_settings, hkvProperty::TYPE_STRING));
    properties.push_back(hkvProperty("Target Hash", m_targetHash, hkvProperty::FLAG_INFORMATION_ONLY));
  }
  else
  {
    properties.push_back(hkvProperty("Filter Target", m_filterManagerTarget.getAvailableElementsId(),
      m_filterManagerTarget.getAvailableElementStrings(), iFlags,
      "The filter target defines what the file should be converted to when passed to the HCT Stand-Alone Filter Manager."
      "'Model' will create a '.model' file, 'StaticMesh' a '.vmesh' file and the character options will produce a '.hkt' file usable by Havok Animation Studio."));
    if (getFilterTarget() != HKV_FILTER_MANAGER_TARGET_NONE)
    {
      properties.back().getMetaInfo()[hkvProperty::ENUM_META_CONFIRMATION_MESSAGE] = hkvVariantValue("Changing the 'Filter Target' will reset the filter manager settings, do you want to continue?", false);
    }

    // The settings are stored along with the source and target path as they are required to open the filter manager.
    hkvProperty::VariantValueList list;

    list.push_back(absSourcePath);
    if (getFilterTarget() == HKV_FILTER_MANAGER_TARGET_NONE)
    {
      list.push_back("");
      list.push_back("");
    }
    else
    {
      list.push_back(absTargetPath);
      list.push_back(getSettings());
    }

    properties.push_back(hkvProperty("Settings", list, s_filterManagerSettingsCustomType, (getFilterTarget() == HKV_FILTER_MANAGER_TARGET_NONE) ? hkvProperty::FLAG_READ_ONLY : iFlags,
      "Opens the HCT stand-alone filter manager to change this file's settings."));
    properties.push_back(hkvProperty("Target Directory", m_targetDirectory, hkvProperty::TYPE_FOLDER, (getFilterTarget() == HKV_FILTER_MANAGER_TARGET_NONE) ? hkvProperty::FLAG_READ_ONLY : iFlags,
      "Select a folder where the output of this Tagfile should be stored. The selected path must be relative to the current data directory. If left blank, the output will be placed next to this asset."));
    properties.back().getMetaInfo()[hkvProperty::FOLDER_META_DIALOG_CAPTION] = hkvVariantValue("Select output directory", false);
    properties.back().getMetaInfo()[hkvProperty::FOLDER_META_RESTRICT_TO_FOLDER] = hkvVariantValue(absLibraryPath, true);
  }

  properties.push_back(hkvProperty::groupEnd());
}

void hkvFilterManagerTransformableAsset::TagfileEntry::setTagfileEntryProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, unsigned int stackIndex, hkvProperty::Purpose purpose)
{
  int stackSize = (path.getSize() - stackIndex);

  if ((stackSize == 1) && (hkvStringHelper::safeCompare(path[stackIndex + 0], m_name) == 0))
  {
    if (purpose == hkvProperty::PURPOSE_SERIALIZATION)
    {
      if (hkvStringHelper::safeCompare(prop.getName(), "Source") == 0)
      {
        m_sourcePath = prop.getString();
      }
      else if (hkvStringHelper::safeCompare(prop.getName(), "Target Directory") == 0)
      {
        m_targetDirectory = prop.getString();
      }
      else if (hkvStringHelper::safeCompare(prop.getName(), "Filter Target") == 0)
      {
        if (m_filterManagerTarget.setByString(prop.getString()) != HK_SUCCESS)
        {
          m_filterManagerTarget.setByAvailableElementsId(0);
        }
      }
      else if (hkvStringHelper::safeCompare(prop.getName(), "Settings") == 0)
      {
        m_settings = prop.getString();
      }
      else if (hkvStringHelper::safeCompare(prop.getName(), "Target Hash") == 0)
      {
        m_targetHash = prop.getUint();
      }
    }
    else
    {
      if (hkvStringHelper::safeCompare(prop.getName(), "Settings") == 0)
      {
        const hkvProperty::VariantValueList& params = prop.getArray();
        m_settings = params[2].getString();
      }
      else if (hkvStringHelper::safeCompare(prop.getName(), "Filter Target") == 0)
      {
        if (m_filterManagerTarget.getAvailableElementsId() != prop.getEnumValue())
        {
          m_filterManagerTarget.setByAvailableElementsId(prop.getEnumValue());
          loadDefaultSettingsForFilterTarget((hkvFilterManagerTarget)m_filterManagerTarget.getDefinitionId());
        }
      }
      else if (hkvStringHelper::safeCompare(prop.getName(), "Target Directory") == 0)
      {
        m_targetDirectory = prop.getString();
      }
    }
  }
}

void hkvFilterManagerTransformableAsset::TagfileEntry::loadDefaultSettingsForFilterTarget(hkvFilterManagerTarget filterTarget)
{
  hkStringBuf absSettingsFilePath;
  if(hkvFileHelper::getMainModuleBasePath(absSettingsFilePath) != HK_SUCCESS)
  {
    return;
  }
  absSettingsFilePath.pathAppend("../../../../Data/Vision/Tools/FBXImporter");
  absSettingsFilePath.pathNormalize();

  switch (filterTarget)
  {
  case HKV_FILTER_MANAGER_TARGET_MODEL:
    {
      absSettingsFilePath.pathAppend("VisionModel.hko");
      break;
    }   
  case HKV_FILTER_MANAGER_TARGET_STATICMESH:
    {
      absSettingsFilePath.pathAppend("VisionStaticMesh.hko");
      break;
    }
  case HKV_FILTER_MANAGER_TARGET_CHARACTER_ASSET:
    {
      absSettingsFilePath.pathAppend("VisionCharacterAsset.hko");
      break;
    }
  case HKV_FILTER_MANAGER_TARGET_CHARACTER_ANIMATION:
    {
      absSettingsFilePath.pathAppend("VisionCharacterAnimation.hko");
      break;
    }
  case HKV_FILTER_MANAGER_TARGET_NONE:
  default:
    {
      m_settings = "";
      return;
    }
  }

  hkArray<char> optionsBuf;
  if( hkLoadUtil(absSettingsFilePath.cString()).toArray(optionsBuf) )
  {
    m_settings.set(optionsBuf.begin(), optionsBuf.getSize());
  }
  else
  {
    m_settings = "";
  }
}

hkUint32 hkvFilterManagerTransformableAsset::TagfileEntry::getHash(hkUint32 assetHash) const
{
  if (getFilterTarget() == HKV_FILTER_MANAGER_TARGET_NONE)
  {
    return 0;
  }
  else
  {
    hkvPropertyList allProperties, relevantProperties;
    getTagfileEntryProperties(allProperties, hkvProperty::PURPOSE_SERIALIZATION);
    hkvTransformationHelper::filterRelevantProperties(allProperties, relevantProperties);
    return hkvPropertiesHelper::computeHash(relevantProperties, NULL, assetHash);
  }
}


/////////////////////////////////////////////////////////////////////////////
// hkvFilterManagerTransformableAsset public functions
/////////////////////////////////////////////////////////////////////////////

hkvFilterManagerTransformableAsset::hkvFilterManagerTransformableAsset(const hkvAssetTypeInfo* assetTypeInfo)
  : hkvAsset(assetTypeInfo)
{
}

hkvFilterManagerTransformableAsset::~hkvFilterManagerTransformableAsset()
{
}

void hkvFilterManagerTransformableAsset::clearTagfileArray() const
{
  m_tagfiles.clear();
}

hkvFilterManagerTransformableAsset::TagfileArray& hkvFilterManagerTransformableAsset::getTagfileArray() const
{
  return m_tagfiles;
}

void hkvFilterManagerTransformableAsset::getTagfileArrayProperties(hkvPropertyList& properties, hkvProperty::Purpose purpose) const
{
  const char* absLibraryPath = getOwnerLibrary()->getPath();
  hkStringBuf relAssetFolder;
  getRelativeFilePath(relAssetFolder);
  relAssetFolder.pathDirname();

  const int tagfileCount = m_tagfiles.size();
  for (int tagfileIdx = 0; tagfileIdx < tagfileCount; ++tagfileIdx)
  {
    if (purpose == hkvProperty::PURPOSE_SERIALIZATION)
    {
      m_tagfiles[tagfileIdx].getTagfileEntryProperties(properties, purpose);
    }
    else
    {
      const TagfileEntry& entry = m_tagfiles[tagfileIdx];
      hkStringBuf absSourcePath = absLibraryPath;
      absSourcePath.pathAppend(entry.m_sourcePath);

      hkStringBuf relTargetPath;
      entry.getRelativeTargetPath(relTargetPath, relAssetFolder);

      hkStringBuf absTargetPath = absLibraryPath;
      absTargetPath.pathAppend(relTargetPath);

      entry.getTagfileEntryProperties(properties, purpose, absSourcePath, absTargetPath, absLibraryPath);
    }
  }
}

void hkvFilterManagerTransformableAsset::setTagfileArrayProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, unsigned int stackIndex, hkvProperty::Purpose purpose)
{
  const int stackSize = (path.getSize() - stackIndex);

  if (stackSize >= 1)
  {
    const char* tagfileName = path[stackIndex + 0];
    TagfileEntry* entry = getTagfileByName(tagfileName, purpose == hkvProperty::PURPOSE_SERIALIZATION);

    if (entry != NULL)
    {
      entry->setTagfileEntryProperty(prop, path, stackIndex, purpose);
    }
  }
}

hkvFilterManagerTransformableAsset::TagfileEntry* hkvFilterManagerTransformableAsset::getTagfileByName(const char* name, bool create) const
{
  const int tagfileCount = m_tagfiles.size();
  for (int tagfileIdx = 0; tagfileIdx < tagfileCount; ++tagfileIdx)
  {
    if (hkvStringHelper::safeCompare(m_tagfiles[tagfileIdx].m_name, name) == 0)
    {
      return &m_tagfiles[tagfileIdx];
    }
  }

  if (create)
  {
    m_tagfiles.push_back(TagfileEntry());
    TagfileEntry* entry = &m_tagfiles[tagfileCount];
    entry->m_name = name;
    return entry;
  }

  return NULL;
}

bool hkvFilterManagerTransformableAsset::isTagfileOutputHashOutOfDate(const TagfileEntry& entry) const
{
  const char* libraryPath = getOwnerLibrary()->getPath();

  if (entry.getFilterTarget() == HKV_FILTER_MANAGER_TARGET_NONE)
  {
    return false;
  }

  hkStringBuf relAssetFolder;
  getRelativeFilePath(relAssetFolder);
  relAssetFolder.pathDirname();

  hkStringBuf relativeFilePath;
  entry.getRelativeTargetPath(relativeFilePath, relAssetFolder);

  hkStringBuf absoluteFilePath = libraryPath;
  absoluteFilePath.pathAppend(relativeFilePath);

  bool bCached = false;
  hkUint32 targetHash = 0;
  // Check whether the output is an asset an we thus don't need to hash the file and can instead use the cached value.
  {
    hkvCriticalSectionLock lock(hkvAssetManager::getGlobalInstance()->acquireLock());
    hkvAsset::RefPtr targetAsset = getOwnerLibrary()->getAssetByPath(relativeFilePath);
    if (targetAsset)
    {
      hkRefPtr<hkvAssetTrackedFile> targetFile(targetAsset->getFile());
      if (targetFile)
      {
        targetHash = targetFile->getStoredHash();
        bCached = true;
      }
    }
  }

  if (!bCached)
  {
    // The output is either no asset or is not created yet - get the hash by hashing the file.
    hkRefPtr<hkStreamReader> reader = hkFileSystem::getInstance().openReader(absoluteFilePath);
    if (reader == NULL || !reader->isOk())
    {
      return true;
    }
    targetHash = hkvTrackedFile::hashFile(*reader);
  }

  // Compare the file's hash to the stored hash to determine whether it is up to date.
  hkUint32 assetHash = getAssetHash();
  hkUint32 entryHash = entry.getHash(assetHash);
  hkCrc32StreamWriter crcWriter(entryHash);
  crcWriter.write(&targetHash, sizeof(targetHash));
  if (crcWriter.getCrc() != entry.m_targetHash)
  {
    return true;
  }

  return false;
}

bool hkvFilterManagerTransformableAsset::anyTagfileOutputHashOutOfDate() const
{
  hkRefPtr<hkvAssetTrackedFile> assetFile(getFile());
  // If the file is no longer present we won't need to transform it anymore.
  if (assetFile == NULL)
  {
    return false;
  }

  const int tagfileCount = m_tagfiles.size();
  // Check each output file if it has the correct hash value (the input tagfile entry hash combined with the output file hash).
  for (int tagfileIdx = 0; tagfileIdx < tagfileCount; ++tagfileIdx)
  {
    const TagfileEntry& entry = m_tagfiles[tagfileIdx];
    if (isTagfileOutputHashOutOfDate(entry))
    {
      return true;
    }
  }
  return false;
}

hkUint32 hkvFilterManagerTransformableAsset::getAssetHash() const
{
  hkRefPtr<hkvAssetTrackedFile> assetFile(getFile());
  if (assetFile == NULL)
  {
    return 0;
  }

  hkUint32 assetHash = assetFile->getStoredHash();
  return assetHash;
}

/////////////////////////////////////////////////////////////////////////////
// hkvFilterManagerTransformableAsset private functions
/////////////////////////////////////////////////////////////////////////////

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
