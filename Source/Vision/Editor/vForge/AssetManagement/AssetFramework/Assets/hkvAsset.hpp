/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvAsset.hpp

#ifndef HKV_ASSET_HPP_INCLUDED
#define HKV_ASSET_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetDecl.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetStructs.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Tags/hkvAssetTag.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvIProperties.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/TransformationRules/hkvTransformationRuleContainer.hpp>

#include <Common/Base/Container/StringMap/hkStorageStringMap.h>

#include <deque>

class hkStreamReader;
class hkvAssetLibrary;
class hkvAssetLibraryReader;
class hkvAssetTrackedFile;
class hkvProfile;
class hkvTransformationRule;
struct hkvTransformationRuleTypeInfo;



/// \brief
///   This is the base class of all asset types. It provides an interface that needs
///   to be implemented by any derived asset type.
///
/// Each derived class needs to provide a static function that passes its hkvAssetTypeInfo
/// to the hkvAssetTypeManager that is called on plugin load and another function should
/// be called on plugin unload to remove the asset type from the hkvAssetTypeManager.
class hkvAsset : public hkReferencedObject, public hkvIProperties
{
  friend class hkvTransformationRule;

public:
  HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_TOOLS);

public:
  typedef hkRefNew<hkvAsset> RefNew;
  typedef hkRefPtr<hkvAsset> RefPtr;
  typedef hkRefPtr<const hkvAsset> RefCPtr;

public:
  /// \brief
  ///   storage type for the asset flags bitfield
  typedef hkUint8 FlagsType;

  /// \brief
  ///   Possible asset flags and flag masks
  enum Flags {
    FLAG_UPDATE_PENDING    = V_BIT(0), ///< The asset is queued for updating
    FLAG_UPDATE_ERROR      = V_BIT(1), ///< The asset has been updated, but updating failed
    FLAG_UPTODATE          = V_BIT(2), ///< The asset has been successfully updated
    FLAG_TRANSFORM_PENDING = V_BIT(3), ///< The asset is queued for transformation
    FLAG_TRANSFORM_ERROR   = V_BIT(4), ///< The asset has beend transformed, but transformation has failed
    FLAG_TRANSFORMED       = V_BIT(5), ///< The asset has been successfully transformed

    /// \brief
    ///   Mask including all update-related flags
    FLAG_MASK_UPDATE = FLAG_UPDATE_PENDING | FLAG_UPDATE_ERROR | FLAG_UPTODATE,

    /// \brief
    ///   Mask including all transformation-related flags
    FLAG_MASK_TRANSFORM = FLAG_TRANSFORM_PENDING | FLAG_TRANSFORM_ERROR | FLAG_TRANSFORMED
  };

public:
  /// \brief
  ///   An asset variant definition.
  ///
  /// Asset variants are user-defined redirections from one asset to another, identified by
  /// a variant key that is used to denote a certain run-time condition.
  class Variant
  {
  public:
    /// \brief
    ///   Constructor; initializes this instance.
    /// \param key
    ///   the variant key for this variant
    /// \param target
    ///   the asset id (relative path) this variant should redirect to
    Variant(const char* key, const char* target);

    /// \brief
    ///   Comparison operator; compares by variant key
    ASSETFRAMEWORK_IMPEXP bool operator<(const Variant& other) const;

    /// \brief
    ///   Returns the variant key of this variant.
    ASSETFRAMEWORK_IMPEXP const char* getKey() const;

    /// \brief
    ///   Returns the target path of this variant
    ASSETFRAMEWORK_IMPEXP const char* getTarget() const;

    /// \brief
    ///   Returns whether this variant is valid.
    ASSETFRAMEWORK_IMPEXP bool isValid() const;

    /// \brief
    ///   Sets the variant key of this variant.
    void setKey(const char* key);

    /// \brief
    ///   Sets the relative asset path this variant redirects to.
    void setTarget(const char* target);

    /// \brief
    ///   Sets whether this variant is valid.
    void setValid(bool valid);

  private:
    hkStringPtr m_key;
    hkStringPtr m_target;
    bool m_valid;
  };


  /// \brief
  ///   A collection of asset variant definitions.
  class Variants
  {
  public:
    /// \brief
    ///   Clears all entries from this collection.
    ASSETFRAMEWORK_IMPEXP void clear();

    /// \brief
    ///   Returns the number of asset variants contained in this collection.
    ASSETFRAMEWORK_IMPEXP int getCount() const;

    /// \brief
    ///   Returns a variant stored in this collection.
    /// \param index
    ///   index of the variant to return
    /// \return
    ///   the requested variant, or \c NULL if \c index was not a valid index into this
    ///   collection.
    ASSETFRAMEWORK_IMPEXP const Variant* getVariant(hkUint32 index) const;

    /// \brief
    ///   Returns the index of the variant definition with the specified key.
    /// \param key
    ///   the variant key to search for
    /// \return
    ///   the index of the variant with the specified key, or \c -1, if no such variant
    ///   exists.
    ASSETFRAMEWORK_IMPEXP int indexOf(const char* key) const;

    /// \brief
    ///   Sets an asset variant.
    /// \param key
    ///   the key of the variant to set. If a variant with this key does not exist in this
    ///   collection, it is added. Otherwise, the target of the existing variant entry is
    ///   set to the new target.
    /// \param target
    ///   the target to set for the variant
    /// \return
    ///   the index of the variant that has just been set
    ASSETFRAMEWORK_IMPEXP int setVariant(const char* key, const char* target);

    /// \brief
    ///   Changes the variant key of an existing variant.
    /// \param index
    ///   the index of the variant to rename
    /// \param newName
    ///   the new key for the variant
    /// \return
    ///   \c true if the variant has been successfully renamed; \c false if not. Reasons for
    ///   failure can be: invalid index; new name is empty; new name is not unique.
    ASSETFRAMEWORK_IMPEXP bool renameVariant(hkUint32 index, const char* newName);

    /// \brief
    ///   Sets the target of an existing variant.
    /// \param index
    ///   the index of the variant to change
    /// \param target
    ///   the target to set for the variant
    /// \param valid
    ///   whether the new target is valid
    ASSETFRAMEWORK_IMPEXP void setVariantTarget(hkUint32 index, const char* target, bool valid);

    /// \brief
    ///   Removes the specified variant entry from this collection
    /// \param index
    ///   index of the entry to remove
    ASSETFRAMEWORK_IMPEXP void remove(hkUint32 index);

    /// \brief
    ///   Sorts the variant entries in this collection by variant key.
    ASSETFRAMEWORK_IMPEXP void sort();

    /// \brief
    ///   Removes all variant keys that do not have a target set.
    ASSETFRAMEWORK_IMPEXP void removeEmptyTargets();

    /// \brief
    ///   Checks all variants in this collection, and sets their valid state based on whether
    ///   or not the target exists.
    ASSETFRAMEWORK_IMPEXP void validate();

    /// \brief
    ///   Sets the entries of this collection from a property.
    ASSETFRAMEWORK_IMPEXP void setFromProperty(const hkvProperty& prop);

    /// \brief
    ///   Returns the contents of this collection as a property.
    /// \param propertyName
    ///   the name of the property to return
    /// \param flags
    ///   the flags to apply to the returned property
    /// \return
    ///   a property representing this collection
    ASSETFRAMEWORK_IMPEXP hkvProperty toProperty(const char* propertyName, hkUint32 flags) const;

  public:
    ASSETFRAMEWORK_IMPEXP static const char* s_propTypeVariants; ///< The property subtype for asset variant collections

  private:
    hkArray<Variant> m_variants;
  };


public:
  ASSETFRAMEWORK_IMPEXP hkvAsset(const hkvAssetTypeInfo* assetTypeInfo);
  ASSETFRAMEWORK_IMPEXP virtual ~hkvAsset();

public:
  /// \brief
  ///   Returns the number of hkvAsset instances currently in existence (for debugging purposes).
  static int getNumInstances() { return s_numInstances; }

public: // Interface
  /// \brief
  ///   Returns whether this asset matches a string query for \c query.
  ASSETFRAMEWORK_IMPEXP virtual bool queryByString(const char* query) const;

  /// \brief
  ///   Returns whether this asset matches a query for the specified property.
  ASSETFRAMEWORK_IMPEXP virtual bool queryByProperty(const char* propertyName, const char* propertyValue) const;

  /// \brief
  ///   Returns the index of the asset type of this instance.
  ASSETFRAMEWORK_IMPEXP virtual const hkvAssetTypeInfoHandle& getTypeInfoHandle() const = 0;

  /// \brief
  ///   Returns the asset type of this instance.
  ASSETFRAMEWORK_IMPEXP const hkvAssetTypeInfo& getType() const;

  /// \brief
  ///   Returns the asset subtype (usually only determinable by examining the asset). A return value
  ///   of \c -1 (default) means the subtype is unknown; this should also be the return value if there
  ///   are no subtypes defined for an asset type.
  ASSETFRAMEWORK_IMPEXP virtual hkInt32 getSubtype() const;

  /// \brief
  ///   Returns the name of this asset's subtype, if a subtype is known (\c NULL otherwise)
  ASSETFRAMEWORK_IMPEXP virtual const char* getSubtypeName() const;

  /// \brief
  ///   Sets the subtype of this asset by name. The base class implementation does nothing and should
  ///   be overwritten in all subclasses that support asset subtypes. If the subtype name is not valid, 
  ///   the subtype should be set to \c -1.
  ASSETFRAMEWORK_IMPEXP virtual void setSubtypeByName(const char* name);

  /// \brief
  ///   Returns the Vision resource subtype of this asset.
  ASSETFRAMEWORK_IMPEXP virtual hkUint32 getResourceSubtype() const;

public: // Member functions
  /// \brief
  ///   Returns the time stamp of when this asset's information was last changed.
  ASSETFRAMEWORK_IMPEXP virtual hkFileSystem::TimeStamp getLastEditTime() const;

  /// \brief
  ///   Sets the last-change time of this asset to the current time.
  ASSETFRAMEWORK_IMPEXP virtual void touch();

  /// \brief
  ///   Returns the asset library that owns this asset (may be \c NULL).
  ASSETFRAMEWORK_IMPEXP hkvAssetLibrary* getOwnerLibrary() const { return m_ownerLibrary; }

  /// \brief
  ///   Returns the index this asset has in its owner library.
  /// \return
  ///   the index of this asset in its owner library; or \c HKV_INVALID_INDEX if
  ///   that index is not known.
  ASSETFRAMEWORK_IMPEXP int getIndexInLibrary() const { return m_indexInLibrary; }

  /// \brief
  ///   Sets the owner library of this asset, and optionally the index of this asset
  ///   in its owner library.
  /// \param library
  ///   the new owner library. May be \c NULL, in which case the index will always be
  ///   set to \c HKV_INVALID_INDEX.
  /// \param indexInLibrary
  ///   the index of this asset in its owner library. Only effective if \c library is
  ///   not \c NULL.
  ASSETFRAMEWORK_IMPEXP void setOwnerLibrary(hkvAssetLibrary* library, int indexInLibrary = HKV_INVALID_INDEX);

  /// \brief
  ///   Returns the underlying file object of this asset (may be \c NULL).
  ASSETFRAMEWORK_IMPEXP hkvAssetTrackedFile* getFile() const { return m_file; }

  /// \brief
  ///   Sets the underlying file object of this asset.
  ///
  /// This also sets the asset back pointer in the passed file object (if it's not \c NULL).
  ///
  /// \param file
  ///   the new underlying file to set for this asset. May be \c NULL.
  ASSETFRAMEWORK_IMPEXP void setFile(hkvAssetTrackedFile* file);

  /// \brief
  ///   Determines the absolute file path of this asset, and writes it into the passed
  ///   buffer.
  /// \param out_path
  ///   the buffer to write the path to
  /// \return
  ///   the path (pointer to the buffer in \c out_path) on success; \c NULL or the empty
  ///   string on failure. In the latter case, \c out_buf is cleared as well.
  ASSETFRAMEWORK_IMPEXP const char* getAbsoluteFilePath(hkStringBuf& out_path) const;

  /// \brief
  ///   Determines the relative file path of this asset, and writes it into the passed
  ///   buffer.
  /// \param out_path
  ///   the buffer to write the path to
  /// \return
  ///   the path (pointer to the buffer in \c out_path) on success; \c NULL or the empty
  ///   string on failure. In the latter case, \c out_buf is cleared as well.
  ASSETFRAMEWORK_IMPEXP const char* getRelativeFilePath(hkStringBuf& out_path) const;

  /// \brief
  ///   Returns whether this asset matches a query by the passed set of asset tags.
  ASSETFRAMEWORK_IMPEXP bool queryByTags(const hkvAssetTags& tags) const;
  /// \brief
  ///   Returns a reference to the set of asset tags of this asset.
  ASSETFRAMEWORK_IMPEXP hkvAssetTags& getTags();
  /// \brief
  ///   Returns a reference to the set of asset tags of this asset.
  ASSETFRAMEWORK_IMPEXP const hkvAssetTags& getTags() const;

  /// \brief
  ///   Clears, then fills \c out_list with the relative paths to all known dependencies of this asset.
  ASSETFRAMEWORK_IMPEXP virtual void makeDependencyList(std::vector<hkStringPtr>& out_list) const;

  /// \brief
  ///   Sets an external dependency list on this asset.
  ///
  /// Setting this list only stores it within the asset, but does not change the asset's dependency 
  /// configuration yet. To actually realize the dependency configuration passed in this function,
  /// call updateDependenciesFromList() at a point where all asset libraries are loaded and thus cross-
  /// library dependencies can be resolved as well.
  ///
  /// \param dependencies
  ///   the list of dependencies (relative file paths) to set
  ASSETFRAMEWORK_IMPEXP void setDependencyList(const std::vector<hkStringPtr>& dependencies);

  /// \brief
  ///   For each dependency set by setDependencyList() earlier, tries to resolve that dependency
  ///   and make the connection between this asset and the dependent file or asset.
  ASSETFRAMEWORK_IMPEXP virtual void updateDependenciesFromList();

  /// \brief
  ///   Method to be called by the asset framework when a project has finished loading all of
  ///   its data.
  ///
  /// The base implementation simply calls updateDependenciesFromList().
  ASSETFRAMEWORK_IMPEXP virtual void handleProjectLoaded();

  /// \brief
  ///   Processes a property hint string.
  ///
  /// Property hints are a way of letting an asset know about reasonable property
  /// default values and other information that can only be retrieved by closer examination
  /// of the actual asset.
  ///
  /// \param propertyHint
  ///   the property hint to process
  ASSETFRAMEWORK_IMPEXP virtual void processPropertyHint(const char* propertyHint) {}

  /// \brief
  ///   Returns the name of this asset.
  ASSETFRAMEWORK_IMPEXP const char* getName() const;

  /// \brief
  ///   Returns whether the asset system knows any way of creating a thumbnail for this asset.
  ASSETFRAMEWORK_IMPEXP bool canCreateThumbnail() const;

  /// \brief
  ///   Creates a key for uniquely identifying the thumbnail for this assets, and stores that
  ///   key in the passed buffer.
  ///
  /// The key is also a file name (incorporating the hash of the asset file's contents), so it
  /// can be appended to a thumbnail path to conveniently determine where to store the thumbnail
  /// picture.
  ///
  /// \param out_key
  ///   the buffer to store the thumbnail key in
  /// \return
  ///   the thumbnail key (pointer to the buffer of \c out_key) on success; \c NULL otherwise.
  ASSETFRAMEWORK_IMPEXP const char* makeThumbnailKey(hkStringBuf& out_key) const;

  /// \brief
  ///   Determines the absolute path for storing this thumbnail's asset, and stores that path
  ///   in the passed buffer.
  ///
  /// The thumbnail path incorporates the absolute path to the thumbnail directory of this
  /// asset's parent asset library, as well as the thumbnail key computed by makeThumbnailKey().
  ///
  /// \param out_path
  ///   the buffer to store the thumbnail path in
  /// \return
  ///   the thumbnail path (pointer to the buffer of \c out_path) on success; \c NULL otherwise.
  ASSETFRAMEWORK_IMPEXP const char* makeAbsoluteThumbnailPath(hkStringBuf& out_path) const;

  /// \brief
  ///   Notifies the asset system that the thumbnail for this asset is missing.
  ///
  /// By using this method, it is possible to notify whenever the thumbnail is found missing 
  /// without triggering an undue amount of thumbnail generation actions. This method takes care
  /// that thumbnail regeneration is only retried once after each change of the underlying asset.
  ASSETFRAMEWORK_IMPEXP void notifyMissingThumbnail();

  /// @name Asset-specific Metadata
  /// @{

  /// \brief
  ///   Clears the metadata that is specific to this asset type and this asset instance.
  ASSETFRAMEWORK_IMPEXP virtual void clearAssetSpecificData() const {}

  /// \brief
  ///   Updates metadata specific to this asset's type and instance.
  ///
  /// The data to be updated here is data which should be displayed and/or edited as the 
  /// asset's properties, and which can be retrieved by examining the asset's file data.
  ///
  /// \param fileData
  ///   a stream from which the file's data can be read
  /// \return
  ///   the operation's result; i.e., whether it was successful, and if not, whether it
  ///   should be retried.
  ASSETFRAMEWORK_IMPEXP virtual hkvAssetOperationResult updateAssetSpecificData(hkStreamReader& fileData) const { return HKV_AOR_SUCCESS; }

  /// @}

  /// @name Status flags
  /// @{

  /// \brief
  ///   Returns the status flags of this asset.
  /// \return
  ///   the status flags of this asset
  /// \see hkvAsset::Flags
  ASSETFRAMEWORK_IMPEXP FlagsType getStatusFlags() const;

  /// \brief
  ///   Clears all update-related status flags of this asset.
  ASSETFRAMEWORK_IMPEXP void setUpdateUnknown();

  /// \brief
  ///   Sets the update pending flag and clears all other update-related status flags of this asset.
  ASSETFRAMEWORK_IMPEXP void setUpdatePending();

  /// \brief
  ///   Sets the update error flag and clears all other update-related status flags of this asset.
  ASSETFRAMEWORK_IMPEXP void setUpdateError();

  /// \brief
  ///   Sets the update success flag and clears all other update-related status flags of this asset.
  ASSETFRAMEWORK_IMPEXP void setUpdateSuccess();

  /// \brief
  ///   Clears all transformation-related status flags of this asset.
  ASSETFRAMEWORK_IMPEXP void setTransformUnknown();

  /// \brief
  ///   Sets the transform pending flag and clears all other transformation-related status flags of this asset.
  ASSETFRAMEWORK_IMPEXP void setTransformPending();

  /// \brief
  ///   Sets the transform error flag and clears all other transformation-related status flags of this asset.
  ASSETFRAMEWORK_IMPEXP void setTransformError();

  /// \brief
  ///   Sets the transform success flag and clears all other transformation-related status flags of this asset.
  ASSETFRAMEWORK_IMPEXP void setTransformSuccess();

  /// @}

  /// @name Log Messages
  /// @{

  ASSETFRAMEWORK_IMPEXP void addLogMessage(const hkvAssetLogMessage& message);
  ASSETFRAMEWORK_IMPEXP void clearLogMessages(hkvMessageCategory category = HKV_MESSAGE_CATEGORY_ANY);
  ASSETFRAMEWORK_IMPEXP hkUint32 getLogMessageCount() const;
  ASSETFRAMEWORK_IMPEXP const hkvAssetLogMessage& getLogMessage(hkUint32 index) const;

  /// @}

  // Transformation
  ASSETFRAMEWORK_IMPEXP void queueForTransformation();

  // Variants
  ASSETFRAMEWORK_IMPEXP Variants& getVariants() { return m_variants; }
  ASSETFRAMEWORK_IMPEXP const Variants& getVariants() const { return m_variants; }
  ASSETFRAMEWORK_IMPEXP void appendVariantLookupEntries(std::vector<hkvLookupTableEntry>& out_entries) const;

  // IProperties implementation
  ASSETFRAMEWORK_IMPEXP virtual const char* getTypeName() const HKV_OVERRIDE;
  ASSETFRAMEWORK_IMPEXP virtual void getProperties(hkvPropertyList& properties, hkvProperty::Purpose purpose) const HKV_OVERRIDE;
  ASSETFRAMEWORK_IMPEXP virtual void setProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, hkUint32 stackIndex, hkvProperty::Purpose purpose) HKV_OVERRIDE;

  ASSETFRAMEWORK_IMPEXP virtual void getCommonProperties(hkvPropertyList& properties, hkvProperty::Purpose purpose) const;
  ASSETFRAMEWORK_IMPEXP virtual void setCommonProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, hkUint32 stackIndex, hkvProperty::Purpose purpose);

  ASSETFRAMEWORK_IMPEXP virtual void getFileProperties(hkvPropertyList& properties, hkvProperty::Purpose purpose) const;
  ASSETFRAMEWORK_IMPEXP virtual void setFileProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, hkUint32 stackIndex, hkvProperty::Purpose purpose);

  virtual void getSpecificProperties(hkvPropertyList& properties, hkvProperty::Purpose purpose) const {}
  virtual void setSpecificProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, hkUint32 stackIndex, hkvProperty::Purpose purpose) {}

  virtual void getInternalProperties(hkvPropertyList& properties, hkvProperty::Purpose purpose) const {}
  virtual void setInternalProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, hkUint32 stackIndex, hkvProperty::Purpose purpose) {}

  ASSETFRAMEWORK_IMPEXP void makeSpecificPropertiesString(const char* entrySeparator, 
    const char* nameValueSeparator, bool flatten, hkStringBuf& out_string) const;

  ASSETFRAMEWORK_IMPEXP const hkvTransformationRule* getTransformRule(const char* profileName) const;
  ASSETFRAMEWORK_IMPEXP bool hasCustomTransformRule() const;
  ASSETFRAMEWORK_IMPEXP hkUint32 getTransformTemplate() const;
  ASSETFRAMEWORK_IMPEXP void setTransformTemplate(const char* name, bool allowCreateTemplate);
  ASSETFRAMEWORK_IMPEXP void setTransformTemplate(hkUint32 newTemplateIndex);

  ASSETFRAMEWORK_IMPEXP void dropUnusedProfiles();

private:
  hkvTransformationRule* getCustomTransformRule(const char* profileName) const;
  void createCustomTransformRules(hkUint32 sourceTemplateIndex);
  void deleteCustomTransformRules();

  // Property handling
  void getTransformationsProperties(hkvPropertyList& properties, hkvProperty::Purpose purpose) const;
  void setTransformationsProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, hkUint32 stackIndex, hkvProperty::Purpose purpose);


protected:
  hkvAssetTags m_tags;

  Variants m_variants;
  std::vector<hkStringPtr> m_dependencyList;

  hkvAssetLibrary* m_ownerLibrary;
  hkUint32 m_indexInLibrary;

  hkRefPtr<hkvAssetTrackedFile> m_file;
  mutable hkStringPtr m_templateName;

  hkvTransformRuleContainer m_transformRules;

  bool m_updateAfterLoad; // If true, the asset should be placed in the update queue after its library has been opened.

  std::vector<hkvAssetLogMessage> m_logMessages;

  hkUint32 m_lastMissingThumbnailHash;

private:
  static int s_numInstances;
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
