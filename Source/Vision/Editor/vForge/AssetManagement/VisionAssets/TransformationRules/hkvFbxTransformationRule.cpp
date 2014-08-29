/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/VisionAssetsPCH.h>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/TransformationRules/hkvFbxTransformationRule.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/TransformationRules/hkvTransformationRuleTypeManager.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Assets/hkvAssetTrackedFile.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetLibrary.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetManager.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/ExternalTools/hkvExternalToolFbxImporter.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/Assets/hkvFbxAsset.hpp>
#include <Common/Base/System/Io/Writer/Crc/hkCrcStreamWriter.h>
#include <Common/Base/System/Io/Reader/hkStreamReader.h>
#include <Common/Base/Container/String/hkUtf8.h>
#include <vector>
#include <string>

hkUint32 hkvFbxTransformationRule::s_ruleTypeIndex = HKV_INVALID_INDEX;
CRITICAL_SECTION hkvFbxTransformationRule::s_protect;

/////////////////////////////////////////////////////////////////////////////
// hkvFbxTransformationRule::FbxContext functions
/////////////////////////////////////////////////////////////////////////////

hkvFbxTransformationRule::FbxContext::FbxContext(const hkvTransformationInput& input, hkvTransformationOutput& output)
  : hkvTransformationRule::Context(input, output), m_noTakes(false)
{
  if (m_input.m_controlHost != NULL)
  {
    m_input.m_controlHost->setTransformationControl(this);
  }
}

hkvFbxTransformationRule::FbxContext::~FbxContext()
{
  if (m_input.m_controlHost != NULL)
  {
    m_input.m_controlHost->setTransformationControl(NULL);
  }
}

void hkvFbxTransformationRule::FbxContext::cancelTransformation()
{
  hkvCriticalSectionLock lock(s_protect);
  if (m_currentTransformationStep != NULL)
  {
    m_currentTransformationStep->cancel();
  }
}


/////////////////////////////////////////////////////////////////////////////
// hkvFbxTransformationRule static functions
/////////////////////////////////////////////////////////////////////////////

void hkvFbxTransformationRule::staticInit()
{
  VASSERT(s_ruleTypeIndex == HKV_INVALID_INDEX);

  hkvTransformationRuleTypeInfo ti;
  ti.m_name = "FbxToTagfileTransformationRule";
  ti.m_createFunc = &createTransformationRule;
  ti.m_supportsAllTypes = false;
  ti.m_supportedAssetTypes.pushBack("FBX");
  ti.m_priority = 100;

  // register at the hkvTransformationRuleTypeManager and store the type index in static variable.
  s_ruleTypeIndex = hkvTransformationRuleTypeManager::getGlobalInstance()->addType(ti);

  InitializeCriticalSection(&s_protect);
}


void hkvFbxTransformationRule::staticDeInit()
{
  if (s_ruleTypeIndex == HKV_INVALID_INDEX)
    return;

  // de-register at the hkvTransformationRuleTypeManager
  hkvTransformationRuleTypeManager::getGlobalInstance()->removeType(s_ruleTypeIndex);
  s_ruleTypeIndex = HKV_INVALID_INDEX;

  DeleteCriticalSection(&s_protect);
}


hkvTransformationRule* hkvFbxTransformationRule::createTransformationRule(hkvTargetPlatform targetPlatform)
{
  return new hkvFbxTransformationRule(targetPlatform);
}


/////////////////////////////////////////////////////////////////////////////
// hkvFbxTransformationRule public functions
/////////////////////////////////////////////////////////////////////////////

hkvFbxTransformationRule::hkvFbxTransformationRule(hkvTargetPlatform platform)
  : hkvFilterManagerTransformationRule(platform)
{
}


hkvFbxTransformationRule::~hkvFbxTransformationRule()
{
}


void hkvFbxTransformationRule::getProperties(hkvPropertyList& properties, hkvProperty::Purpose purpose) const
{
}


void hkvFbxTransformationRule::setProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, unsigned int iStackIndex, hkvProperty::Purpose purpose)
{
}

unsigned int hkvFbxTransformationRule::getTypeIndex() const
{
  return s_ruleTypeIndex;
}


const char* hkvFbxTransformationRule::getTypeName() const
{
  return "FbxToTagfileTransformationRule";
}

bool hkvFbxTransformationRule::assetNeedsTransformation(const hkvAsset& asset) const
{
  const hkvFbxAsset& fbxAsset = static_cast<const hkvFbxAsset&>(asset);
  if (fbxAsset.getFbxTarget() == HKV_FBX_TARGET_NONE)
    return false;

  hkStringBuf filename;
  fbxAsset.makeFbxTargetName(filename);
  hkStringBuf relativeFileName(hkvAssetLibrary::s_assetDataDirectory, "/", hkvAssetLibrary::s_assetCacheDirectory);
  relativeFileName.pathAppend(filename);

  bool bNeedFbxStep = needFbxStep(&fbxAsset, relativeFileName);
  if (bNeedFbxStep)
    return true;

  return hkvFilterManagerTransformationRule::assetNeedsTransformation(asset);
}

bool hkvFbxTransformationRule::executeTransformation(const hkvTransformationInput& input, hkvTransformationOutput& output) const
{
  FbxContext context(input, output);
  const hkvFbxAsset* fbxAsset = static_cast<const hkvFbxAsset*>(context.m_input.m_pAsset);
  hkvFbxTarget fbxTarget = fbxAsset->getFbxTarget();

  if (fbxTarget == HKV_FBX_TARGET_NONE)
    return true;
  
  bool ok = true;
  // FBX steps
  {
    // Make path to asset transform cache
    hkStringBuf filename;
    fbxAsset->makeFbxTargetName(filename);
    
    hkStringBuf relativeFileName(hkvAssetLibrary::s_assetDataDirectory, "/", hkvAssetLibrary::s_assetCacheDirectory);
    relativeFileName.pathAppend(filename);

    bool bNeedFbxStep = needFbxStep(fbxAsset, relativeFileName);

    if (bNeedFbxStep)
    {
      relativeFileName.append(".hkt");
      hkStringBuf absoluteFileName(fbxAsset->getOwnerLibrary()->getPath());
      absoluteFileName.pathAppend(relativeFileName);

      context.m_relativeTargetPath = relativeFileName;
      context.m_absoluteTargetPath = absoluteFileName;
      context.m_noTakes = fbxTarget == HKV_FBX_TARGET_SINGLE_OUTPUT;
      ok = ok && (runFbxConversion(context) == HK_SUCCESS);

      if (!ok)
      {
        output.m_messages.pushBack(hkvAssetLogMessage(HKV_MESSAGE_CATEGORY_ASSET_TRANSFORMATION, HKV_MESSAGE_SEVERITY_ERROR, "Running FBX-Importer failed!"));
        return false;
      }
    }
  }

  // Tagfile steps
  {
    hkvFilterManagerTransformableAsset::TagfileArray& tagfiles = fbxAsset->getTagfileArray();
    const int tagfileCount = tagfiles.size();
    for (int tagfileIdx = 0; tagfileIdx < tagfileCount; ++tagfileIdx)
    {
      hkvFilterManagerTransformableAsset::TagfileEntry& entry = tagfiles[tagfileIdx];
      TagfileContext context(input, output, entry);
      ok = ok && executeTagfileEntryTransformation(context) == HK_SUCCESS;
    }

    cleanUpTempFiles(context);
    return ok;
  }

  return false;
}


bool hkvFbxTransformationRule::needFbxStep(const hkvFbxAsset* fbxAsset, const char* relativeFileName) const
{
  const char* absLibraryPath = fbxAsset->getOwnerLibrary()->getPath();
  hkvFilterManagerTransformableAsset::TagfileArray& tagfiles = fbxAsset->getTagfileArray();

  const int tagfileCount = tagfiles.size();
  if (tagfileCount == 0)
    return true;

  // All FBXImporter generated tagfiles must start with the asset hash name.
  for (int tagfileIdx = 0; tagfileIdx < tagfileCount; ++tagfileIdx)
  {
    hkvFilterManagerTransformableAsset::TagfileEntry& entry = tagfiles[tagfileIdx];
    if (!entry.m_sourcePath.startsWith(relativeFileName))
    {
      return true;
    }

    hkStringBuf absPath = absLibraryPath;
    absPath.pathAppend(entry.m_sourcePath);
    if (!hkvFileHelper::fileExists(absPath))
    {
      return true;
    }
  }
  return false;
}


hkResult hkvFbxTransformationRule::runFbxConversion(FbxContext& context) const
{
  hkvExternalToolFbxImporter step(context.m_input.m_sourceFile, context.m_absoluteTargetPath, context.m_noTakes);
  {
    hkvCriticalSectionLock lock(s_protect);
    context.m_currentTransformationStep = &step;
  }
  step.m_checkTargetFile = false;
  hkResult res = step.run();
  {
    hkvCriticalSectionLock lock(s_protect);
    context.m_currentTransformationStep = NULL;
  }
  if (res == HK_FAILURE)
    return HK_FAILURE;

  const hkvFbxAsset* fbxAsset = static_cast<const hkvFbxAsset*>(context.m_input.m_pAsset);
  hkArray<hkStringPtr> outputFiles;
  extractOutputFilenamesFromStdOutMessages(context, step.getMessages(), outputFiles);
  convertFbxOutputToTagfileEntries(context, outputFiles);
  const_cast<hkvAsset*>(context.m_input.m_pAsset)->touch();
  return res;
}


void hkvFbxTransformationRule::extractOutputFilenamesFromStdOutMessages(FbxContext& context, const hkArray<hkvAssetLogMessage>& messages, hkArray<hkStringPtr>& out_outputFiles) const
{
  const hkvFbxAsset* fbxAsset = static_cast<const hkvFbxAsset*>(context.m_input.m_pAsset);
  // This text is printed for each output file followed by the relative filename.
  static const char* outputLineStart = "Saved tag file: ";
  static const hkInt32 outputLineStartLength = (hkInt32)strlen(outputLineStart);

  hkStringBuf relativeFolder = context.m_relativeTargetPath;
  relativeFolder.pathDirname();

  // Extract imported filenames from the stdout stream of the FBXImporter.
  const hkInt32 messageCount = messages.getSize();
  for (hkInt32 messageIdx = 0; messageIdx < messageCount; ++messageIdx)
  {
    const char* foundPos = hkString::strStr(messages[messageIdx].m_message, outputLineStart);
    if (foundPos != NULL)
    {
      const char* fileName = foundPos + outputLineStartLength;

      hkStringBuf relativeFilePath = relativeFolder;
      relativeFilePath.pathAppend(fileName);
      relativeFilePath.pathNormalize();
      out_outputFiles.pushBack(relativeFilePath.cString());
    }
  }
}


void hkvFbxTransformationRule::convertFbxOutputToTagfileEntries(FbxContext& context, const hkArray<hkStringPtr>& outputFiles) const
{
  const hkvFbxAsset* fbxAsset = static_cast<const hkvFbxAsset*>(context.m_input.m_pAsset);
  hkvFilterManagerTransformableAsset::TagfileArray& tagfiles = fbxAsset->getTagfileArray();
  // basename containing Hash
  hkStringBuf targetBasename = context.m_relativeTargetPath;
  targetBasename.pathBasename();
  hkvStringHelper::pathNoExtension(targetBasename);
  int basenameLength = targetBasename.getLength();

  // Original basename
  hkStringBuf sourceBasename;
  fbxAsset->getRelativeFilePath(sourceBasename);
  sourceBasename.pathBasename();
  hkvStringHelper::pathNoExtension(sourceBasename);

  hkStringBuf sourceBaseDirectory;
  fbxAsset->getRelativeFilePath(sourceBaseDirectory);
  sourceBaseDirectory.pathDirname();

  hkvFilterManagerTransformableAsset::TagfileArray newTagfiles;
  const int outputFileCount = outputFiles.getSize();
  newTagfiles.reserve(outputFileCount);
  
  for (int fileIdx = 0; fileIdx < outputFileCount; fileIdx++)
  {
    // New tagfile name
    const hkStringPtr& relativePath = outputFiles[fileIdx];
    hkStringBuf entryName = relativePath;
    entryName.pathBasename();
    entryName.chompStart(basenameLength);
    entryName.prepend(sourceBasename);
    hkvStringHelper::pathNoExtension(entryName);

    hkvFilterManagerTransformableAsset::TagfileEntry* entry = fbxAsset->getTagfileByName(entryName);
    if (entry)
    {
      entry->m_sourcePath = relativePath;
      newTagfiles.push_back(*entry);
    }
    else
    {
      newTagfiles.push_back(hkvFilterManagerTransformableAsset::TagfileEntry());
      hkvFilterManagerTransformableAsset::TagfileEntry& entry = newTagfiles.back();
      entry.m_name = entryName;
      entry.m_sourcePath = outputFiles[fileIdx];
    }
    
  }
  tagfiles.swap(newTagfiles);
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
