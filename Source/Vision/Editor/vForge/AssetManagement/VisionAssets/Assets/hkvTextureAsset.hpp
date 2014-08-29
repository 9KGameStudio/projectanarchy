/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvTextureAsset.hpp

#ifndef VTEXTUREASSETTYPE_HPP_INCLUDED
#define VTEXTUREASSETTYPE_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetDecl.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Assets/hkvAsset.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvBase.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/ImageFile/hkvImageFileProperties.hpp>

class hkvAssetTypeManager;

enum hkvTextureAssetSubtype
{
  HKV_TEXTURE_ASSET_SUBTYPE_2D = 0,
  HKV_TEXTURE_ASSET_SUBTYPE_3D,
  HKV_TEXTURE_ASSET_SUBTYPE_ARRAY,
  HKV_TEXTURE_ASSET_SUBTYPE_CUBEMAP,

  HKV_TEXTURE_ASSET_SUBTYPE_COUNT
};

class hkvTextureAsset : public hkvAsset
{
public:
  HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_TOOLS);

public: // static functions
  static void StaticInit();
  static void StaticDeInit();
  static hkvAsset* CreateAsset();

public: // public functions
  hkvTextureAsset();
  ~hkvTextureAsset();

public: // overrides
  virtual const hkvAssetTypeInfoHandle& getTypeInfoHandle() const HKV_OVERRIDE;

  virtual hkInt32 getSubtype() const HKV_OVERRIDE;
  virtual const char* getSubtypeName() const HKV_OVERRIDE;
  virtual void setSubtypeByName(const char* name) HKV_OVERRIDE;
  virtual hkUint32 getResourceSubtype() const HKV_OVERRIDE;

  virtual void getSpecificProperties(hkvPropertyList& properties, hkvProperty::Purpose purpose) const HKV_OVERRIDE;
  virtual void setSpecificProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, unsigned int stackIndex, hkvProperty::Purpose purpose) HKV_OVERRIDE;
  virtual void handleProjectLoaded() HKV_OVERRIDE;

protected:
  virtual void clearAssetSpecificData() const HKV_OVERRIDE;
  virtual hkvAssetOperationResult updateAssetSpecificData(hkStreamReader& fileData) const HKV_OVERRIDE;

private:
  static hkvAssetTypeInfo* s_typeInfo;
  static const hkvAssetTypeInfoHandle* s_typeInfoHandle;

  static hkvEnumDefinition s_subtypeDefinition;
  static hkvEnumDefinition s_imageFormatsDefinition;
  static hkvEnumDefinition s_usageDefinition;

  mutable hkInt32 m_subtype;
  mutable hkvImageFileProperties m_imageProperties;
  hkvEnumInstance m_usageInstance;
  hkBool m_sRgb;
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
