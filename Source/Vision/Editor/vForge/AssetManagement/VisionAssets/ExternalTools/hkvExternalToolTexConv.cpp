/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/AssetManagement/VisionAssets/VisionAssetsPCH.h>

#include <Vision/Editor/vForge/AssetManagement/VisionAssets/ExternalTools/hkvExternalToolTexConv.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvTextureDefinitions.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/TransformationRules/hkvTextureTransformationSettings.hpp>

#include <Common/Base/Container/String/hkUtf8.h>


hkvExternalToolTexConv::hkvExternalToolTexConv(const hkvTextureTransformationSettings& settings, 
  const char* sourceFile, const char* targetFile, bool forceDxt10)
: hkvExternalToolTransformation(sourceFile, targetFile), m_forceDxt10(forceDxt10)
{
  determineExecutablePath();
  determineCommandLineParameters(settings, sourceFile, targetFile);
}


const char* hkvExternalToolTexConv::getToolName() const
{
  return "TexConv";
}


void hkvExternalToolTexConv::determineExecutablePath()
{
  hkStringBuf pathBuf;
  if (hkvFileHelper::getMainModuleBasePath(pathBuf) == HK_SUCCESS)
  {
    pathBuf.pathAppend("texconv.exe");
    m_executable = pathBuf;
  }
}


void hkvExternalToolTexConv::determineCommandLineParameters(
  const hkvTextureTransformationSettings& settings, const char* sourceFile, const char* targetFile)
{
  // Build the command line argument string
  hkStringBuf sourceFileBuf(sourceFile);
  sourceFileBuf.replace('/', '\\');

  hkStringBuf outFileDir(sourceFile);
  outFileDir.pathDirname();
  outFileDir.replace('/', '\\');

  hkStringBuf sourceFileBaseName(sourceFile);
  sourceFileBaseName.pathBasename();
  hkvStringHelper::pathNoExtension(sourceFileBaseName);

  hkStringBuf outFileName(outFileDir);
  outFileName.appendJoin("/", sourceFileBaseName, "_out.dds");
  setTargetFile(outFileName);

  hkStringBuf paramBuf;
  paramBuf.append(" -nologo");

  if(m_forceDxt10)
  {
    paramBuf.append(" -dx10");
  }

  if (settings.getTargetWidth() != settings.getSourceWidth())
  {
    paramBuf.appendPrintf(" -w %i", settings.getTargetWidth());
  }
  if (settings.getTargetHeight() != settings.getSourceHeight())
  {
    paramBuf.appendPrintf(" -h %i", settings.getTargetHeight());
  }

  if (settings.getCreateMipMaps())
  {
    paramBuf.append(" -m 0");
  }
  else
  {
    paramBuf.append(" -m 1");
  }

  paramBuf.append(" -sepalpha");

  const char* dataFormatStr = NULL;
  switch (settings.getTargetDataFormat())
  {
  case HKV_IMAGE_DATA_FORMAT_DXT1:
    dataFormatStr = "BC1_UNORM";
    break;
  case HKV_IMAGE_DATA_FORMAT_DXT3:
    dataFormatStr = "BC2_UNORM";
    break;
  case HKV_IMAGE_DATA_FORMAT_DXT5:
    dataFormatStr = "BC3_UNORM";
    break;
  case HKV_IMAGE_DATA_FORMAT_A1R5G5B5:
    dataFormatStr = "B5G5R5A1_UNORM";
    break;
  case HKV_IMAGE_DATA_FORMAT_R5G6B5:
    dataFormatStr = "B5G6R5_UNORM";
    break;
  case HKV_IMAGE_DATA_FORMAT_A8R8G8B8:
    dataFormatStr = "B8G8R8A8_UNORM";
    break;
  case HKV_IMAGE_DATA_FORMAT_X8R8G8B8:
    dataFormatStr = "B8G8R8X8_UNORM";
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

  paramBuf.append(" -if CUBIC");

  paramBuf.append(" -sx _out");

  paramBuf.appendJoin(" -o \"", outFileDir.cString(), "\"");

  paramBuf.append(" -ft DDS");

  paramBuf.appendJoin(" \"", sourceFileBuf.cString(), "\"");

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
