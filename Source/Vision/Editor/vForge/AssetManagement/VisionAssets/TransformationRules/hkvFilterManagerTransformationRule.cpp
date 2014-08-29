/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/VisionAssetsPCH.h>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/TransformationRules/hkvFilterManagerTransformationRule.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/Assets/hkvFilterManagerTransformableAsset.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/TransformationRules/hkvTransformationRuleTypeManager.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Assets/hkvAssetTrackedFile.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetLibrary.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/ExternalTools/hkvExternalToolFilterManager.hpp>

#include <Common/Base/System/Io/Writer/Crc/hkCrcStreamWriter.h>
#include <Common/Base/System/Io/Reader/hkStreamReader.h>
#include <Common/Base/Container/String/hkUtf8.h>
#include <vector>
#include <string>

CRITICAL_SECTION hkvFilterManagerTransformationRule::s_protect;

/////////////////////////////////////////////////////////////////////////////
// hkvFilterManagerTransformationRule::TagfileContext functions
/////////////////////////////////////////////////////////////////////////////

hkvFilterManagerTransformationRule::TagfileContext::TagfileContext(const hkvTransformationInput& input, hkvTransformationOutput& output, hkvFilterManagerTransformableAsset::TagfileEntry& entry)
  : hkvTransformationRule::Context(input, output), m_entry(entry), m_filterSettings(NULL)
{
  if (m_input.m_controlHost != NULL)
  {
    m_input.m_controlHost->setTransformationControl(this);
  }
}

hkvFilterManagerTransformationRule::TagfileContext::~TagfileContext() 
{
  if (m_input.m_controlHost != NULL)
  {
    m_input.m_controlHost->setTransformationControl(NULL);
  }
}

void hkvFilterManagerTransformationRule::TagfileContext::cancelTransformation()
{
  hkvCriticalSectionLock lock(s_protect);
  if (m_currentTransformationStep != NULL)
  {
    m_currentTransformationStep->cancel();
  }
}


/////////////////////////////////////////////////////////////////////////////
// hkvTagfileTransformationRule static functions
/////////////////////////////////////////////////////////////////////////////

void hkvFilterManagerTransformationRule::staticInit()
{
  InitializeCriticalSection(&s_protect);
}


void hkvFilterManagerTransformationRule::staticDeInit()
{
  DeleteCriticalSection(&s_protect);
}


/////////////////////////////////////////////////////////////////////////////
// hkvFilterManagerTransformationRule public functions
/////////////////////////////////////////////////////////////////////////////

hkvFilterManagerTransformationRule::hkvFilterManagerTransformationRule(hkvTargetPlatform platform)
: hkvTransformationRule(platform)
{
}


hkvFilterManagerTransformationRule::~hkvFilterManagerTransformationRule()
{
}


bool hkvFilterManagerTransformationRule::queryOutputFileSpecs(const hkvTransformationInput& input, hkvTransformationOutput& output, bool forLookUpTable) const
{
  if (forLookUpTable)
  {
    return false;
  }

  const hkvFilterManagerTransformableAsset* filterManagerAsset = static_cast<const hkvFilterManagerTransformableAsset*>(input.m_pAsset);
  const hkvFilterManagerTransformableAsset::TagfileArray& tagfiles = filterManagerAsset->getTagfileArray();
  const int tagfileCount = (int)tagfiles.size();
  for (int tagfileIdx = 0; tagfileIdx < tagfileCount; ++tagfileIdx)
  {
    const hkvFilterManagerTransformableAsset::TagfileEntry& entry = tagfiles[tagfileIdx];
    
    if (!hkvStringHelper::isEmpty(entry.m_sourcePath))
    {
      hkvTransformationOutputFileSpec outSpec;
      hkStringBuf fileName = entry.m_sourcePath;
      fileName.pathBasename();

      outSpec.m_fileName = fileName;
      output.m_outputFileSpecs.pushBack(outSpec);
    }
    
  }
  return true;
}


bool hkvFilterManagerTransformationRule::assetNeedsTransformation(const hkvAsset& asset) const
{
  const hkvFilterManagerTransformableAsset& filterManagerAsset = static_cast<const hkvFilterManagerTransformableAsset&>(asset);
  return filterManagerAsset.anyTagfileOutputHashOutOfDate();
}

hkResult hkvFilterManagerTransformationRule::executeTagfileEntryTransformation(TagfileContext& context) const
{
  const hkvFilterManagerTransformableAsset* filterManagerAsset = static_cast<const hkvFilterManagerTransformableAsset*>(context.m_input.m_pAsset);

  if (context.m_entry.getFilterTarget() == HKV_FILTER_MANAGER_TARGET_NONE)
  {
    return HK_SUCCESS;
  }

  bool ok = true;
  if (filterManagerAsset->isTagfileOutputHashOutOfDate(context.m_entry))
  {
    ok = (determineFilterManagerInput(context) == HK_SUCCESS);
    ok = ok && (makeIntermediateBaseName(context) == HK_SUCCESS);
    ok = ok && (copySettingsToTempFile(context) == HK_SUCCESS);
    ok = ok && (runTagfileConversion(context, context.m_entry.getHash(filterManagerAsset->getAssetHash())) == HK_SUCCESS);

    if (ok)
    {
      notifyOutputWritten(context, NULL);
      context.m_output.m_messages.pushBack(hkvAssetLogMessage(HKV_MESSAGE_CATEGORY_ASSET_TRANSFORMATION, HKV_MESSAGE_SEVERITY_INFO,
        "Running Stand-Alone Filter Manager succeeded."));
    }
    else
    {
      hkStringBuf message;
      message.printf("Running Stand-Alone Filter Manager failed on target: '%s'!", context.m_entry.m_name.cString());
      context.m_output.m_messages.pushBack(hkvAssetLogMessage(HKV_MESSAGE_CATEGORY_ASSET_TRANSFORMATION, HKV_MESSAGE_SEVERITY_ERROR,
        message.cString()));
    }
  }
  else
  {

  }
  return ok ? HK_SUCCESS : HK_FAILURE;
}


hkResult hkvFilterManagerTransformationRule::copySettingsToTempFile(TagfileContext& context) const
{
  hkStringBuf tempBuf;
  context.m_absSettingsPath = makeIntermediateFileName(context, "hko", tempBuf);

  if (hkvStringHelper::isEmpty(context.m_filterSettings))
  {
    context.m_output.m_messages.pushBack(hkvAssetLogMessage(HKV_MESSAGE_CATEGORY_ASSET_TRANSFORMATION, HKV_MESSAGE_SEVERITY_ERROR, "Filter manager settings are empty!"));
    return HK_FAILURE;
  }

  hkRefPtr<hkStreamWriter> settingsWriter = hkFileSystem::getInstance().openWriter(context.m_absSettingsPath);
  if (settingsWriter == HK_NULL)
  {
    context.m_output.m_messages.pushBack(hkvAssetLogMessage(HKV_MESSAGE_CATEGORY_ASSET_TRANSFORMATION, HKV_MESSAGE_SEVERITY_ERROR, "Failed to write filter manager settings to temporary file!"));
    return HK_FAILURE;
  }

  settingsWriter->write(context.m_filterSettings, strlen(context.m_filterSettings));
  context.m_tempFileNames.pushBack(context.m_absSettingsPath);

  return HK_SUCCESS;
}


hkResult hkvFilterManagerTransformationRule::determineFilterManagerInput(TagfileContext& context) const
{
  const hkvFilterManagerTransformableAsset* filterManagerAsset = static_cast<const hkvFilterManagerTransformableAsset*>(context.m_input.m_pAsset);
  if (context.m_entry.getFilterTarget() == HKV_FILTER_MANAGER_TARGET_NONE)
  {
    return HK_SUCCESS;
  }

  // Determine the target file extension and settings.
  const char* fileExt = hkvFilterManagerTransformableAsset::getFilterTargetFileExtension(context.m_entry.getFilterTarget());
  context.m_filterSettings = context.m_entry.getSettings();
  if (hkvStringHelper::isEmpty(context.m_filterSettings))
  {
    context.m_output.m_messages.pushBack(hkvAssetLogMessage(HKV_MESSAGE_CATEGORY_ASSET_TRANSFORMATION, HKV_MESSAGE_SEVERITY_ERROR, "Filter settings are empty! Try changing the filter target to restore the default settings."));
    return HK_FAILURE;
  }

  if (hkvStringHelper::isEmpty(fileExt))
  {
    context.m_output.m_messages.pushBack(hkvAssetLogMessage(HKV_MESSAGE_CATEGORY_ASSET_TRANSFORMATION, HKV_MESSAGE_SEVERITY_ERROR, "The filter target is invalid and needs to be changed!"));
    return HK_FAILURE;
  }

  // Generate source and target paths.
  const char* absLibraryPath = context.m_input.m_pAsset->getOwnerLibrary()->getPath();
  hkStringBuf relAssetFolder;
  filterManagerAsset->getRelativeFilePath(relAssetFolder);
  relAssetFolder.pathDirname();

  hkStringBuf absSourcePath(absLibraryPath);
  absSourcePath.pathAppend(context.m_entry.m_sourcePath);
  context.m_absSourcePath = absSourcePath;

  hkStringBuf relTargetPath;
  context.m_entry.getRelativeTargetPath(relTargetPath, relAssetFolder);
  context.m_relTargetPath = relTargetPath;

  hkStringBuf absTargetPath(absLibraryPath);
  absTargetPath.pathAppend(context.m_relTargetPath);
  context.m_absTargetPath = absTargetPath;
  return HK_SUCCESS;
}


hkResult hkvFilterManagerTransformationRule::runTagfileConversion(TagfileContext& context, hkUint32 entryHash) const
{
  const hkvFilterManagerTransformableAsset* tagfileAsset = static_cast<const hkvFilterManagerTransformableAsset*>(context.m_input.m_pAsset);
  if (context.m_entry.getFilterTarget() == HKV_FILTER_MANAGER_TARGET_NONE)
  {
    return HK_SUCCESS;
  }

  hkStringBuf hctPath;
  if (!hkvExternalToolFilterManager::getHctPath(hctPath))
  {
    context.m_output.m_messages.pushBack(hkvAssetLogMessage(HKV_MESSAGE_CATEGORY_ASSET_TRANSFORMATION, HKV_MESSAGE_SEVERITY_ERROR,
      "The path to the Havok Content Tools could not be found, please make sure the HCT are installed correctly."));
  }

  // Run HCT standalone filter manager.
  hkvExternalToolFilterManager step(context.m_absSourcePath, context.m_absTargetPath, context.m_absSettingsPath);
  {
    hkvCriticalSectionLock lock(s_protect);
    context.m_currentTransformationStep = &step;
  }
  step.m_checkTargetFile = true;
  hkResult res = step.run();
  {
    hkvCriticalSectionLock lock(s_protect);
    context.m_currentTransformationStep = NULL;
  }

  // Hash output file.
  hkRefPtr<hkStreamReader> reader = hkFileSystem::getInstance().openReader(context.m_absTargetPath);
  if (reader != NULL && reader->isOk())
  {
    hkUint32 targetHash = hkvTrackedFile::hashFile(*reader);
    hkCrc32StreamWriter crcWriter(entryHash);
    crcWriter.write(&targetHash, sizeof(targetHash));
    context.m_entry.m_targetHash = crcWriter.getCrc();
  }
  else
  {
    context.m_entry.m_targetHash = 0;
  }
  const_cast<hkvAsset*>(context.m_input.m_pAsset)->touch();
  return res;
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
