/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvTransformationRuleContainer.hpp

#ifndef HKVTRANSFORMATIONRULECONTAINER_HPP_INCLUDED
#define HKVTRANSFORMATIONRULECONTAINER_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/TransformationRules/hkvTransformationRule.hpp>
#include <Common/Base/Container/StringMap/hkStorageStringMap.h>

/// \brief
///   Manages a map of profiles to transform rules for a specific asset type.
///   Used by the transform templates and assets to store custom per profile rules.
class hkvTransformRuleContainer
{
public:
  /// \brief
  ///   Creates a transform rule container for the given asset type identified by its name.
  ASSETFRAMEWORK_IMPEXP hkvTransformRuleContainer(const char* assetType, bool bCreateRules);

  /// \brief
  ///   Creates a transform rule container for the given asset type identified by its type info.
  ASSETFRAMEWORK_IMPEXP hkvTransformRuleContainer(const hkvAssetTypeInfo* assetTypeInfo, bool bCreateRules);

  /// \brief
  ///   Destructor.
  ASSETFRAMEWORK_IMPEXP ~hkvTransformRuleContainer();

  /// \brief
  ///   Copies all rules from the given container to this one.
  ///   Both container's asset type must match.
  ASSETFRAMEWORK_IMPEXP void operator= (const hkvTransformRuleContainer& rhs);

  /// \brief
  ///   Returns whether any rules are contained within the map.
  ASSETFRAMEWORK_IMPEXP bool hasRules() const;

  /// \brief
  ///   Clears all rules from the map.
  ASSETFRAMEWORK_IMPEXP void clearRules();

  /// \brief
  ///   Returns the asset type of the rules this container manages.
  ASSETFRAMEWORK_IMPEXP const char* getAssetType() const;

  /// \brief
  ///   Returns whether the asset type of this container is profile independent.
  ///
  /// If true, there is only one transform rule under the key hkvTransformRuleContainer::s_profileIndependentName.
  /// If false, each profile in the current project will have an entry in the map.
  ASSETFRAMEWORK_IMPEXP bool isProfileIndependent() const;

  /// \brief
  ///   Returns the transformation rule for the given profile name. If the profile does not exist,
  ///   NULL is returned unless bCreate is true, then a new default rule is created.
  ASSETFRAMEWORK_IMPEXP hkvTransformationRule* getTransformRule(const char* profileName, bool bCreate = true) const;
  
  /// \brief
  ///   Inserts the given rule under the given name in the map.
  ASSETFRAMEWORK_IMPEXP void insertTransformRule(const char* profileName, hkvTransformationRule* transformRule);

  /// \brief
  ///   hkvIProperties compatible getProperties function.
  ASSETFRAMEWORK_IMPEXP void getProperties(hkvPropertyList& properties, hkvProperty::Purpose purpose) const;
  
  /// \brief
  ///   hkvIProperties compatible setProperty function.
  ASSETFRAMEWORK_IMPEXP void setProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, hkUint32 stackIndex, hkvProperty::Purpose purpose);
  
  /// \brief
  ///   Drops all entries in the map that don't have a matching profile in the project.
  ASSETFRAMEWORK_IMPEXP void dropUnusedProfiles();

private:
  ASSETFRAMEWORK_IMPEXP void getRuleProperties(const hkvTransformationRule* rule, const char* profileName, const char* description,
    hkvPropertyList& properties, hkvProperty::Purpose purpose) const;

  /// \brief
  ///   Creates default settings entry for each profile.
  ASSETFRAMEWORK_IMPEXP void init();

public:
  ASSETFRAMEWORK_IMPEXP static const char* const s_profileIndependentName;

private:
  hkStringPtr m_assetType;
  mutable hkStorageStringMap<hkvTransformationRule*> m_profileRules; // Maps profiles to hkvTransformationRule
  bool m_profileIndependent;
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
