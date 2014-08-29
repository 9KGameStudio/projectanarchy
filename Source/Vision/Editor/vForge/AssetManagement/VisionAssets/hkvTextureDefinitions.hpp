/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKV_TEXTURE_DEFINITIONS_HPP_INCLUDED
#define HKV_TEXTURE_DEFINITIONS_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetStructs.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/ImageFile/hkvImageFile.hpp>

enum hkvTextureUsage
{
  HKV_TEXTURE_USAGE_UNDEFINED,
  HKV_TEXTURE_USAGE_DIFFUSE_MAP,
  HKV_TEXTURE_USAGE_NORMAL_MAP,
  //HKV_TEXTURE_USAGE_SPECULAR_MAP,
  HKV_TEXTURE_USAGE_COUNT
};
extern const char* hkvTextureUsageNames[];


enum hkvTextureCompression
{
  HKV_TEXTURE_COMPRESSION_NONE,
  HKV_TEXTURE_COMPRESSION_QUALITY,
  //HKV_TEXTURE_COMPRESSION_SIZE,
  HKV_TEXTURE_COMPRESSION_COUNT
};
extern const char* hkvTextureCompressionNames[];


struct hkvConversionToolFormatString
{
  hkvImageDataFormat m_format;
  const char* m_string;
};


typedef hkvEnumMapping<hkvTargetPlatform, HKV_TARGET_PLATFORM_COUNT, hkvImageDataFormat, HKV_IMAGE_DATA_FORMAT_COUNT> hkvPlatformToTextureDataFormatMappingBase;
class hkvPlatformToTextureDataFormatMapping
: public hkReferencedObject,
  public hkSingleton<hkvPlatformToTextureDataFormatMapping>,
  public hkvPlatformToTextureDataFormatMappingBase
{
public:
  HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_TOOLS);

public:
  hkvPlatformToTextureDataFormatMapping();
private:
  hkvPlatformToTextureDataFormatMapping(const hkvPlatformToTextureDataFormatMapping&);
  hkvPlatformToTextureDataFormatMapping& operator=(const hkvPlatformToTextureDataFormatMapping&);
};


typedef hkvEnumMapping<hkvTargetPlatform, HKV_TARGET_PLATFORM_COUNT, hkvImageFileFormat, HKV_IMAGE_FILE_FORMAT_COUNT> hkvPlatformToTextureFileFormatMappingBase;
class hkvPlatformToTextureFileFormatMapping
: public hkReferencedObject,
  public hkSingleton<hkvPlatformToTextureFileFormatMapping>,
  public hkvPlatformToTextureFileFormatMappingBase
{
public:
  HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_TOOLS);

public:
  hkvPlatformToTextureFileFormatMapping();
private:
  hkvPlatformToTextureFileFormatMapping(const hkvPlatformToTextureFileFormatMapping&);
  hkvPlatformToTextureFileFormatMapping& operator=(const hkvPlatformToTextureFileFormatMapping&);
};


typedef hkvEnumMapping<hkvImageFileFormat, HKV_IMAGE_FILE_FORMAT_COUNT, hkvImageDataFormat, HKV_IMAGE_DATA_FORMAT_COUNT> hkvTextureFileToDataFormatMappingBase;
class hkvTextureFileToDataFormatMapping
: public hkReferencedObject,
  public hkSingleton<hkvTextureFileToDataFormatMapping>,
  public hkvTextureFileToDataFormatMappingBase
{
public:
  HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_TOOLS);

public:
  hkvTextureFileToDataFormatMapping();
private:
  hkvTextureFileToDataFormatMapping(const hkvTextureFileToDataFormatMapping&);
  hkvTextureFileToDataFormatMapping& operator=(const hkvTextureFileToDataFormatMapping&);
};
#endif

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
