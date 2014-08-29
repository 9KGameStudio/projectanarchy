/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
/// \file hkvFilterManagerTransformableAsset.hpp

#ifndef HKV_FILTER_MANAGER_TRANSFORMABLE_ASSET_HPP_INCLUDED
#define HKV_FILTER_MANAGER_TRANSFORMABLE_ASSET_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetDecl.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Assets/hkvAsset.hpp>
enum hkvFilterManagerTarget
{
  HKV_FILTER_MANAGER_TARGET_NONE,
  HKV_FILTER_MANAGER_TARGET_MODEL,
  HKV_FILTER_MANAGER_TARGET_STATICMESH,
  HKV_FILTER_MANAGER_TARGET_CHARACTER_ASSET,
  HKV_FILTER_MANAGER_TARGET_CHARACTER_ANIMATION,
  HKV_FILTER_MANAGER_TARGET_COUNT
};

class hkvFilterManagerTransformableAsset : public hkvAsset
{
public:
  HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_TOOLS);
  struct TagfileEntry
  {
    TagfileEntry();
    TagfileEntry(const TagfileEntry& rhs);
    void operator=(const TagfileEntry& rhs);

    const char* getName() const;
    hkvFilterManagerTarget getFilterTarget() const;
    const char* getSettings() const;
    const char* getRelativeTargetPath(hkStringBuf& out_path, const char* relativeAssetFolder, const char* name = NULL) const;
    void getTagfileEntryProperties(hkvPropertyList& properties, hkvProperty::Purpose purpose,
      const char* absSourcePath = "", const char* absTargetPath = "", const char* absLibraryPath = "") const;
    void setTagfileEntryProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, unsigned int stackIndex, hkvProperty::Purpose purpose);
    void loadDefaultSettingsForFilterTarget(hkvFilterManagerTarget filterTarget);
    hkUint32 getHash(hkUint32 assetHash) const;

    hkStringPtr m_name;
    hkStringPtr m_sourcePath;
    hkvEnumInstance m_filterManagerTarget;
    hkStringPtr m_targetDirectory;
    hkStringPtr m_settings;
    hkUint32 m_targetHash;
  };
  typedef std::vector<TagfileEntry> TagfileArray;

public: // static functions
  static const char* getFilterTargetFileExtension(hkvFilterManagerTarget filterTarget);

public: // public functions
  hkvFilterManagerTransformableAsset(const hkvAssetTypeInfo* assetTypeInfo);
  ~hkvFilterManagerTransformableAsset();

  void clearTagfileArray() const;
  TagfileArray& getTagfileArray() const;

public:
  void getTagfileArrayProperties(hkvPropertyList& properties, hkvProperty::Purpose purpose) const;
  void setTagfileArrayProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, unsigned int stackIndex, hkvProperty::Purpose purpose);
  hkvFilterManagerTransformableAsset::TagfileEntry* getTagfileByName(const char* name, bool create = false) const;
  bool isTagfileOutputHashOutOfDate(const TagfileEntry& entry) const;
  bool anyTagfileOutputHashOutOfDate() const;
  hkUint32 getAssetHash() const;

protected:
  mutable TagfileArray m_tagfiles;

public:
  static const char* const s_filterManagerSettingsCustomType;

protected:
  static hkvEnumDefinition s_filterManagerTargetDefinition;
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
