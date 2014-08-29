/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/AssetManagement/VisionAssets/VisionAssetsPCH.h>

#include <Vision/Editor/vForge/AssetManagement/VisionAssets/ExternalTools/hkvExternalToolPvrTexTool.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvTextureDefinitions.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/TransformationRules/hkvTextureTransformationSettings.hpp>


hkvExternalToolPvrTexTool::hkvExternalToolPvrTexTool(const hkvTextureTransformationSettings& settings, 
  const char* sourceFile, const char* targetFile)
: hkvExternalToolTransformation(sourceFile, targetFile)
{
  determineExecutablePath();
  determineCommandLineParameters(settings, sourceFile, targetFile);
  m_idlePriority = true;
}


const char* hkvExternalToolPvrTexTool::getToolName() const
{
  return "PVRTexTool";
}


void hkvExternalToolPvrTexTool::determineExecutablePath()
{
  hkStringBuf pathBuf;
  if (hkvSystemHelper::getEnvironmentVariable("HAVOK_THIRDPARTY_DIR", pathBuf) != HK_SUCCESS)
  {
    if(hkvFileHelper::getMainModuleBasePath(pathBuf) != HK_SUCCESS)
    {
      return;
    }
    pathBuf.pathAppend("../../../../ThirdParty");
  }
#if defined(_WIN64)
  pathBuf.pathAppend("redistsdks/PVRTexTool/3.0r2/CL/Windows_x86_64/PVRTexToolCL.exe");
#else
  pathBuf.pathAppend("redistsdks/PVRTexTool/3.0r2/CL/Windows_x86_32/PVRTexToolCL.exe");
#endif
  m_executable = pathBuf;
}


void hkvExternalToolPvrTexTool::determineCommandLineParameters(
  const hkvTextureTransformationSettings& settings, const char* sourceFile, const char* targetFile)
{
  // Build the command line argument string
  hkStringBuf sourceFileBuf(sourceFile);
  sourceFileBuf.replace('/', '\\');
  hkStringBuf targetFileBuf(targetFile);
  targetFileBuf.replace('/', '\\');

  hkStringBuf paramBuf;
  paramBuf.appendJoin(" -i \"", sourceFileBuf, "\"");
  paramBuf.appendJoin(" -o \"", targetFileBuf, "\"");

  //TODO: Add ETC2 and PVRTC2 support
  const char* dataFormatStr = NULL;
  switch (settings.getTargetDataFormat())
  {
  case HKV_IMAGE_DATA_FORMAT_PVRTC2:
    dataFormatStr = "PVRTC1_2";
    break;
  case HKV_IMAGE_DATA_FORMAT_PVRTC4:
    dataFormatStr = "PVRTC1_4";
    break;
  case HKV_IMAGE_DATA_FORMAT_ETC1:
    dataFormatStr = "ETC1";
    break;
  }

  if (dataFormatStr != NULL)
  {
    paramBuf.appendJoin(" -f ", dataFormatStr);
  }
  else
  {
    VASSERT_MSG(dataFormatStr != NULL, "Unsupported data format specified!");
  }

  switch (settings.getTargetFileFormat())
  {
  case HKV_IMAGE_FILE_FORMAT_DDS:
    {
      break;
    }
  case HKV_IMAGE_FILE_FORMAT_ETC:
  case HKV_IMAGE_FILE_FORMAT_PVR:
    {
      paramBuf.append(" -legacypvr");
      break;
    }
  }

  if (settings.getCreateMipMaps())
  {
    paramBuf.append(" -m");
    paramBuf.append(" -mfilter cubic");
  }

  if (settings.getTargetWidth() != settings.getSourceWidth() || settings.getTargetHeight() != settings.getSourceHeight())
  {
    paramBuf.appendPrintf(" -r %i,%i", settings.getTargetWidth(), settings.getTargetHeight());
    paramBuf.appendPrintf(" -rfilter cubic");
  }
  
  m_commandLine = paramBuf;
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
