/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
/// \file hkvFbxAsset.hpp

#ifndef VFBXASSETTYPE_HPP_INCLUDED
#define VFBXASSETTYPE_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetDecl.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/Assets/hkvFilterManagerTransformableAsset.hpp>

enum hkvFbxTarget
{
  HKV_FBX_TARGET_NONE,
  HKV_FBX_TARGET_SINGLE_OUTPUT,
  HKV_FBX_TARGET_MULTIPLE_OUTPUTS,
  HKV_FBX_TARGET_COUNT
};

class hkvFbxAsset : public hkvFilterManagerTransformableAsset
{
public:
  HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_TOOLS);

public: // static functions
  static void StaticInit();
  static void StaticDeInit();
  static hkvAsset* CreateAsset();

public: // public functions
  hkvFbxAsset();
  ~hkvFbxAsset();
  hkvFbxTarget getFbxTarget() const;
  hkUint32 getFbxHash() const;
  const char* makeFbxTargetName(hkStringBuf& out_filename) const;

public: // overrides
  virtual const hkvAssetTypeInfoHandle& getTypeInfoHandle() const HKV_OVERRIDE;

  virtual void getInternalProperties(hkvPropertyList& properties, hkvProperty::Purpose purpose) const HKV_OVERRIDE;
  virtual void setInternalProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, unsigned int stackIndex, hkvProperty::Purpose purpose) HKV_OVERRIDE;

private:
  hkvEnumInstance m_fbxTarget;

private:
  static hkvAssetTypeInfo* s_typeInfo;
  static const hkvAssetTypeInfoHandle* s_typeInfoHandle;
  static hkvEnumDefinition s_fbxTargetDefinition;
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
