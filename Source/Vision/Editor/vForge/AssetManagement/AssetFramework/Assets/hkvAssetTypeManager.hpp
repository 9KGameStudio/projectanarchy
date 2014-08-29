/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvAssetTypeManager.hpp

#ifndef HKV_ASSET_TYPE_MANAGER_HPP_INCLUDED
#define HKV_ASSET_TYPE_MANAGER_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetDecl.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetStructs.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvCallback.hpp>

#include <Common/Base/Container/Queue/hkQueue.h>
#include <Common/Base/Container/StringMap/hkStorageStringMap.h>

/// \brief
///   Data object for the OnTypeDataChanged callback in the hkvAssetTypeManager.
class hkvTypeDataChangedData : public hkvCallbackData
{
public:
  ASSETFRAMEWORK_IMPEXP hkvTypeDataChangedData(hkvCallback* sender, const hkArray<hkvAssetTypeEntry>& typeEntries)
    : hkvCallbackData(sender)
    , m_typeEntries(typeEntries)
  {
  }

  const hkArray<hkvAssetTypeEntry>& m_typeEntries;
};


/// \brief
///   The hkvAssetTypeManager handles all types of assets that are either provided by Vision or
///   dynamically loaded via a plugin. An asset type is defined via the hkvAssetTypeInfo struct and
///   a class derived from IAsset.
///
/// Adding or removing types will unload all libraries in the hkvAssetManager so this should only be done during
/// project loading and unloading.
class hkvAssetTypeManager
{
public:
  ASSETFRAMEWORK_IMPEXP static hkvAssetTypeManager* getGlobalInstance();

public:
  ASSETFRAMEWORK_IMPEXP hkvAssetTypeManager();
  ASSETFRAMEWORK_IMPEXP ~hkvAssetTypeManager();

  // Asset Type Handling
  ASSETFRAMEWORK_IMPEXP hkUint32 getTypeCount() const;
  ASSETFRAMEWORK_IMPEXP hkUint32 getTypeEntryCount() const;
  ASSETFRAMEWORK_IMPEXP const hkvAssetTypeInfoHandle* addAssetType(const hkvAssetTypeInfo& assetTypeInfo);
  ASSETFRAMEWORK_IMPEXP void removeAssetType(const hkvAssetTypeInfoHandle& typeHandle);
  ASSETFRAMEWORK_IMPEXP const hkvAssetTypeInfo* getAssetTypeInfo(hkUint32 typeIndex) const;
  ASSETFRAMEWORK_IMPEXP const hkvAssetTypeInfo* getAssetTypeInfo(const char* name) const;
  ASSETFRAMEWORK_IMPEXP hkUint32 getAssetTypeInfoIndexByName(const char* name) const;
  ASSETFRAMEWORK_IMPEXP const hkArray<hkvAssetTypeEntry>& getAssetTypeEntries() const;

  ASSETFRAMEWORK_IMPEXP hkvResult resolveAssetTypeString(const char* typeString, const hkvAssetTypeInfoHandle*& out_typeHandle, hkInt32& out_subtypeIndex) const;

  ASSETFRAMEWORK_IMPEXP const hkArray<hkUint32>* getExtensionTypeList(const char* extension) const;
  ASSETFRAMEWORK_IMPEXP hkvCreateAssetFunc findCreateAssetFunction(const char* fileName) const;

public: // Data
  /// \brief
  ///   Changes to the TypeData will result in all libraries being invalidated. The asset manager
  ///   has to listen to this to tear down all open libraries. This will only happen on plugin unloading,
  ///   which should only happen on project unloading.
  hkvCallback OnBeforeTypeDataChanged; 
  hkvCallback OnTypeDataChanged;

private:
  void clearExtensionLookup();
  void addToExtensionLookup (const hkvAssetTypeInfo& ti, hkUint32 assetTypeIndex);
  void callOnBeforeTypeDataChanged();
  void callOnTypeDataChanged();
  void rebuildTypeEntryList();

private: // Data
  struct TypeList
  {
    hkArray<hkUint32> m_assetTypeIndices;
  };

  hkArray<hkvAssetTypeInfoHandle*> m_assetTypes;
  hkArray<hkvAssetTypeEntry> m_assetTypeEntries;

  hkStorageStringMap<TypeList*> m_extensionLookup;
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
