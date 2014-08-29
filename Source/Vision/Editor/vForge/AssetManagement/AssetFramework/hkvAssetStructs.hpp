/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvAssetStructs.hpp

#ifndef HKV_ASSET_STRUCTS_HPP_INCLUDED
#define HKV_ASSET_STRUCTS_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetDecl.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Tags/hkvAssetTag.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvBase.hpp>

#include <Common/Base/Types/Color/hkColor.h>

#include <vector>

class hkvAsset;
class hkvIProperties;
class hkvITransformationControlHost;

/////////////////////////////////////////////////////////////////////////////
// hkvTargetPlatform related
/////////////////////////////////////////////////////////////////////////////
enum hkvTargetPlatform
{
  HKV_TARGET_PLATFORM_DX9 = 0,
  HKV_TARGET_PLATFORM_DX11,
  HKV_TARGET_PLATFORM_XBOX360,
  HKV_TARGET_PLATFORM_PS3,
  HKV_TARGET_PLATFORM_PSVITA,
  HKV_TARGET_PLATFORM_IOS,
  HKV_TARGET_PLATFORM_ANDROID,
  HKV_TARGET_PLATFORM_WIIU,
  HKV_TARGET_PLATFORM_TIZEN,
  HKV_TARGET_PLATFORM_WINPHONE,

  HKV_TARGET_PLATFORM_COUNT,

  HKV_TARGET_PLATFORM_ANY,
};

extern const char* hkvTargetPlatformNames[];
extern const char* hkvTargetPlatformShaderBinExtensions[]; // This is for the PackageTool. We need to clean this up and probably remove hkvTargetPlatform altogether, and instead use the vBase defines.

ASSETFRAMEWORK_IMPEXP const hkvEnumDefinition& hkvGetTargetPlatformDefinition();
ASSETFRAMEWORK_IMPEXP const char* hkvGetTargetPlatformShaderBinExtension(hkvTargetPlatform Platform);


/////////////////////////////////////////////////////////////////////////////
// Asset look-up table related
/////////////////////////////////////////////////////////////////////////////

/// \brief Represents a single entry in the lookup table.
struct hkvLookupTableEntry
{
  hkStringPtr m_sSourcePath;  ///< The original file path (should be all lower case)
  hkStringPtr m_sVariantKey;  ///< The variant key of this entry (optional)
  hkStringPtr m_sTargetPath;  ///< The file to which to map the source path
  hkStringPtr m_sMetaInfo;    ///< Meta info string

  /// \brief Combines the entry data into a single string, as it will be stored in the lookup table.
  ASSETFRAMEWORK_IMPEXP const char* getCombinedString(hkStringBuf& sBuffer) const;
};



/////////////////////////////////////////////////////////////////////////////
// hkvAssetTypeInfo related
/////////////////////////////////////////////////////////////////////////////
typedef hkvAsset* (*hkvCreateAssetFunc)(void);
typedef bool (*hkvDetermineDependenciesFunc)(const char* libraryPath, const char* assetPath, std::vector<hkStringPtr>& out_dependencies);
typedef bool (*hkvGenerateThumbnailFunc)(const char* libraryPath, const char* assetPath, const char* thumbnailPath);
typedef bool (*hkvGetPropertyHintFunc)(const char* libraryPath, const char* assetPath, hkStringBuf& out_propertyHint);

/// \brief
///   The hkvAssetTypeInfo struct is used to register a new asset type
///   at the hkvAssetTypeManager.
struct hkvAssetTypeInfo
{
  hkvAssetTypeInfo() 
  : m_createFunc(NULL), 
    m_determineDependenciesFunc(NULL), m_generateThumbnailFunc(NULL), m_getPropertyHintFunc(NULL),
    m_szTypeIconQt(""), m_resourceManagerName(NULL),
    m_useEngineForDependencies(false), m_useEngineForThumbnails(false), m_useEngineForPropertyHint(false),
    m_profileIndependentTransform(false)
  {
  }

private:
  hkvAssetTypeInfo(const hkvAssetTypeInfo& rhs);
  hkvAssetTypeInfo& operator=(const hkvAssetTypeInfo&);

public:
  bool supportsFile(const char* fileName) const
  {
    const char* extension = hkvStringHelper::getExtension(fileName);
    if (extension[0] == '.')
    {
      extension++;
    }

    const int numSupported = m_supportedFileExtensions.getSize();
    for (int supportedIdx = 0; supportedIdx < numSupported; ++supportedIdx)
    {
      if (hkvStringHelper::safeCompare(extension, m_supportedFileExtensions[supportedIdx].cString(), true) == 0)
      {
        return true;
      }
    }

    return false;
  }

  hkStringPtr m_name;
  hkvCreateAssetFunc m_createFunc;
  hkvDetermineDependenciesFunc m_determineDependenciesFunc;
  hkvGenerateThumbnailFunc m_generateThumbnailFunc;
  hkvGetPropertyHintFunc m_getPropertyHintFunc;
  hkArray<hkStringPtr> m_supportedFileExtensions;

  hkArray<const char*> m_subtypes;

  hkStringPtr m_defaultThumbnailPath;
  const char* m_szTypeIconQt;

  const char* m_resourceManagerName;
  bool m_useEngineForDependencies;
  bool m_useEngineForThumbnails;
  bool m_useEngineForPropertyHint;
  bool m_profileIndependentTransform; // If set, each asset has only one trasform rule for all profiles.
};


struct hkvAssetTypeInfoHandle
{
  hkvAssetTypeInfoHandle(const hkvAssetTypeInfo& typeInfo)
    : m_typeInfo(typeInfo), m_typeIndex(0), m_typeEntryOffset(0)
  {
  }

  const hkvAssetTypeInfo& getTypeInfo() const
  {
    return m_typeInfo;
  }

  hkUint32 getTypeIndex() const
  {
    return m_typeIndex;
  }

  void setTypeIndex(hkUint32 index)
  {
    m_typeIndex = index;
  }

  hkUint32 getTypeEntryOffset() const
  {
    return m_typeEntryOffset;
  }

  void setTypeEntryOffset(hkUint32 offset)
  {
    m_typeEntryOffset = offset;
  }

  hkUint32 getNumSubtypes() const
  {
    return (hkvMath::Max(1, m_typeInfo.m_subtypes.getSize()));
  }

private:
  const hkvAssetTypeInfo& m_typeInfo;
  hkUint32 m_typeIndex;
  hkUint32 m_typeEntryOffset;
};


/// \brief
///   Underlying info for entries in asset type lists
struct hkvAssetTypeEntry
{
  hkvAssetTypeEntry()
    : m_typeInfoHandle(NULL), m_subtypeIndex(-1)
  {
  }

  hkvAssetTypeEntry(const hkvAssetTypeInfoHandle* typeInfoHandle, hkInt32 subtypeIndex)
    : m_typeInfoHandle(typeInfoHandle), m_subtypeIndex(subtypeIndex)
  {
  }

  const hkvAssetTypeInfoHandle* getTypeInfoHandle() const
  {
    return m_typeInfoHandle;
  }

  hkInt32 getSubtypeIndex() const
  {
    return m_subtypeIndex;
  }

  const char* makeFullTypeName(hkStringBuf& buffer) const
  {
    buffer = m_typeInfoHandle->getTypeInfo().m_name;
    if (m_subtypeIndex >= 0)
    {
      buffer.append(" | ");
      buffer.append(m_typeInfoHandle->getTypeInfo().m_subtypes[m_subtypeIndex]);
    }
    return buffer;
  }

private:
  const hkvAssetTypeInfoHandle* m_typeInfoHandle; ///< Pointer to a handle for this entry's asset type info
  hkInt32 m_subtypeIndex; ///< The asset subtype of this entry (-1 for default type)
};


struct hkvTransformationInput
{
  hkvTransformationInput()
  : m_pAsset(NULL), m_includeEditorPreview(false), m_controlHost(NULL)
  {
  }

  hkvTransformationInput(const hkvTransformationInput& rhs)  
  {
    *this = rhs;
  }

  hkvTransformationInput& operator=(const hkvTransformationInput& rhs)
  {
    m_pAsset = rhs.m_pAsset;
    m_sourceFile = rhs.m_sourceFile;
    m_targetPath = rhs.m_targetPath;
    m_targetBaseName = rhs.m_targetBaseName;
    m_targetHashString = rhs.m_targetHashString;
    m_includeEditorPreview = rhs.m_includeEditorPreview;
    m_controlHost = rhs.m_controlHost;
    return *this;
  }
  
  const hkvAsset* m_pAsset; // Reference is held by the transform process
  hkStringPtr m_sourceFile;
  hkStringPtr m_targetPath;
  hkStringPtr m_targetBaseName;
  hkStringPtr m_targetHashString;
  bool m_includeEditorPreview;
  hkvITransformationControlHost* m_controlHost;
};


enum hkvMessageCategory
{
  HKV_MESSAGE_CATEGORY_ANY = 0,
  HKV_MESSAGE_CATEGORY_ASSET_UPDATE,
  HKV_MESSAGE_CATEGORY_ASSET_TRANSFORMATION
};


enum hkvMessageSeverity
{
  HKV_MESSAGE_SEVERITY_INFO,
  HKV_MESSAGE_SEVERITY_WARNING,
  HKV_MESSAGE_SEVERITY_ERROR
};


struct hkvAssetLogMessage
{
  hkvAssetLogMessage(hkvMessageCategory category, hkvMessageSeverity severity, const char* message) 
  : m_category(category), m_severity(severity), m_message(message)
  {
  }

  hkvMessageCategory m_category;
  hkvMessageSeverity m_severity;
  hkStringPtr m_message;
};


struct hkvTransformationOutputFileSpec
{
  hkvTransformationOutputFileSpec()
  {
  }

  hkvTransformationOutputFileSpec(const hkvTransformationOutputFileSpec& rhs)
    : m_variantKey(rhs.m_variantKey)
    , m_fileName(rhs.m_fileName)
    , m_extraMetadata(rhs.m_extraMetadata)
  {
  }

  hkvTransformationOutputFileSpec& operator=(const hkvTransformationOutputFileSpec& rhs)
  {
    m_variantKey = rhs.m_variantKey;
    m_fileName = rhs.m_fileName;
    m_extraMetadata = rhs.m_extraMetadata;
    return *this;
  }

  hkStringPtr m_variantKey;
  hkStringPtr m_fileName;
  hkStringPtr m_extraMetadata;
};


struct hkvTransformationOutput
{
  hkvTransformationOutput()
  {
  }
  
  hkvTransformationOutput(const hkvTransformationOutput& rhs)
  {
    m_messages.append(rhs.m_messages);
    m_outputFileSpecs.append(rhs.m_outputFileSpecs);
  }

  hkvTransformationOutput& operator=(const hkvTransformationOutput& rhs)
  {
    m_messages = rhs.m_messages;
    m_outputFileSpecs = rhs.m_outputFileSpecs;
    return *this;
  }

  hkArray<hkvAssetLogMessage> m_messages;
  hkArray<hkvTransformationOutputFileSpec> m_outputFileSpecs;
};

class hkvTransformationRule;

typedef hkvTransformationRule* (*hkvCreateTransformationRuleFunc)(hkvTargetPlatform targetPlatform);

struct hkvTransformationRuleTypeInfo
{
  hkvTransformationRuleTypeInfo() 
  : m_createFunc(NULL), m_supportsAllTypes(false), m_priority(0)
  {
  }

  hkvTransformationRuleTypeInfo(const hkvTransformationRuleTypeInfo& rhs)
  {
    *this = rhs;
  }
  
  ~hkvTransformationRuleTypeInfo() {}
  
  void operator= (const hkvTransformationRuleTypeInfo& rhs)
  {
    m_name = rhs.m_name;
    m_createFunc = rhs.m_createFunc;
    m_supportedAssetTypes = rhs.m_supportedAssetTypes;
    m_supportsAllTypes = rhs.m_supportsAllTypes;
    m_priority = rhs.m_priority;
  }

  hkStringPtr m_name;
  hkvCreateTransformationRuleFunc m_createFunc;
  hkArray<hkStringPtr> m_supportedAssetTypes;
  bool m_supportsAllTypes;
  int m_priority;
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
