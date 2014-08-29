/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/AssetManagement/VisionAssets/VisionAssetsPCH.h>

#include <Vision/Editor/vForge/AssetManagement/VisionAssets/ExternalTools/hkvExternalToolFilterManager.hpp>
#include <Common/Base/Container/String/hkUtf8.h>

std::string hkvExternalToolFilterManager::s_hctPath = "";

/////////////////////////////////////////////////////////////////////////////
// hkvExternalToolFilterManager public functions
/////////////////////////////////////////////////////////////////////////////

hkvExternalToolFilterManager::hkvExternalToolFilterManager(const char* sourceFile, const char* targetFile, const char* filterSet)
: hkvExternalToolTransformation(sourceFile, targetFile)
{
  hkStringBuf tempBuf;
  getFilterManagerCmdArguments(sourceFile, targetFile, filterSet, false, tempBuf);
  m_commandLine = tempBuf;
}

hkResult hkvExternalToolFilterManager::beforeRun()
{
  hkStringBuf pathTemp;

  if (!getHctPath(pathTemp))
  {
    hkStringBuf msgBuf("[", getToolName(), "] The path to the Havok Content Tools could not be found! Make sure it is installed correctly.");
    addMessage(hkvAssetLogMessage(HKV_MESSAGE_CATEGORY_ASSET_TRANSFORMATION, HKV_MESSAGE_SEVERITY_ERROR, msgBuf.cString()));
    return HK_FAILURE;
  }

  pathTemp.pathAppend("hctStandAloneFilterManager.exe");
  if (!hkvFileHelper::fileExists(pathTemp))
  {
    hkStringBuf msgBuf("[", getToolName(), "] The 'hctStandAloneFilterManager.exe' was not found in the HCT directory: '", s_hctPath.c_str(), "'.");
    addMessage(hkvAssetLogMessage(HKV_MESSAGE_CATEGORY_ASSET_TRANSFORMATION, HKV_MESSAGE_SEVERITY_ERROR, msgBuf.cString()));
  }
  m_executable = pathTemp;
  return HK_SUCCESS;
}

bool hkvExternalToolFilterManager::isExitCodeFailure(hkUint32 exitCode) const
{
  // The stand-alone filter manager returns 1 if warnings were present, which we don't care about.
  return !(exitCode == 0 || exitCode == 1);
}


/////////////////////////////////////////////////////////////////////////////
// hkvExternalToolFilterManager static functions
/////////////////////////////////////////////////////////////////////////////

bool hkvExternalToolFilterManager::getHctPath(hkStringBuf& out_hctPath)
{
  if (!determineExecutablePath())
  {
    out_hctPath.clear();
    return false;
  }
  else
  {
    out_hctPath = s_hctPath.c_str();
    return true;
  }
}

void hkvExternalToolFilterManager::getFilterManagerCmdArguments(const char* sourceFile, const char* targetFile, const char* filterSet, bool interactive, hkStringBuf& out_cmdArguments)
{
  hkStringBuf sourceDir(sourceFile);
  sourceDir.pathDirname();
  out_cmdArguments.printf("-p \"%s\" -o \"%s\" -s \"%s\" %s\"%s\"", sourceDir.cString(), targetFile, filterSet, interactive ? "-i " : "", sourceFile);
}


/////////////////////////////////////////////////////////////////////////////
// hkvExternalToolFilterManager overrides
/////////////////////////////////////////////////////////////////////////////

const char* hkvExternalToolFilterManager::getToolName() const
{
  return "Stand-Alone Filter Manager";
}


/////////////////////////////////////////////////////////////////////////////
// hkvExternalToolFilterManager private functions
/////////////////////////////////////////////////////////////////////////////

static bool checkHCTPath(const char* path, hkStringBuf& tempBuffer)
{
  if (hkvStringHelper::isEmpty(path))
    return false;

  const char* exeName = "hctStandAloneFilterManager.exe";

  tempBuffer = path;
  tempBuffer.pathAppend(exeName);
  return hkvFileHelper::fileExists(tempBuffer);
}

bool hkvExternalToolFilterManager::determineExecutablePath()
{
  if (!s_hctPath.empty())
    return true;

  hkStringBuf hctPath;
  hkStringBuf filterManagerPath;
  
  if (hkvSystemHelper::getRegistryStringVariable("Software\\Havok\\hkFilters_x64", "FilterPath", hctPath) == HK_SUCCESS && checkHCTPath(hctPath, filterManagerPath))
  {
    s_hctPath = hctPath;
    return true;
  }

  if (hkvSystemHelper::getRegistryStringVariable("Software\\Havok\\hkFilters", "FilterPath", hctPath) == HK_SUCCESS && checkHCTPath(hctPath, filterManagerPath))
  {
    s_hctPath = hctPath;
    return true;
  }

  if (hkvSystemHelper::getEnvironmentVariable("HAVOK_TOOLS_ROOT_X64", hctPath) == HK_SUCCESS && checkHCTPath(hctPath, filterManagerPath))
  {
    s_hctPath = hctPath;
    return true;
  }

  if (hkvSystemHelper::getEnvironmentVariable("HAVOK_TOOLS_ROOT", hctPath) == HK_SUCCESS && checkHCTPath(hctPath, filterManagerPath))
  {
    s_hctPath = hctPath;
    return true;
  }

  if (hkvSystemHelper::getProgramFiles64Dir(hctPath))
  {
    hctPath.pathAppend("Havok/HavokContentTools");
    if (checkHCTPath(hctPath, filterManagerPath))
    {
      s_hctPath = hctPath;
      return true;
    }
  }

  if (hkvSystemHelper::getProgramFiles32Dir(hctPath))
  {
    hctPath.pathAppend("Havok/HavokContentTools");
    if (checkHCTPath(hctPath, filterManagerPath))
    {
      s_hctPath = hctPath;
      return true;
    }
  }

  return false;
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
