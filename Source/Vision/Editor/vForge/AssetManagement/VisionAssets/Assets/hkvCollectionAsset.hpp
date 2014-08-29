/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
/// \file hkvCollectionAsset.hpp

#ifndef VCOLLECTIONASSETTYPE_HPP_INCLUDED
#define VCOLLECTIONASSETTYPE_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetDecl.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Assets/hkvAsset.hpp>

#include <vector>

class hkvAssetTypeManager;

class hkvCollectionAsset : public hkvAsset
{
public:
  HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_TOOLS);

public: // static functions
  static void StaticInit();
  static void StaticDeInit();
  static hkvAsset* CreateAsset();
  static bool DetermineDependencies(const char* libraryPath, const char* assetPath, std::vector<hkStringPtr>& out_dependencies);

public: // public functions
  hkvCollectionAsset();
  ~hkvCollectionAsset();

public: // overrides
  virtual const hkvAssetTypeInfoHandle& getTypeInfoHandle() const HKV_OVERRIDE;

private:
  static hkvAssetTypeInfo* s_typeInfo;
  static const hkvAssetTypeInfoHandle* s_typeInfoHandle;
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
