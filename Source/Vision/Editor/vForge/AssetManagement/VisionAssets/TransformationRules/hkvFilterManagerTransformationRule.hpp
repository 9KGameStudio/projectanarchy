/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKV_FILTER_MANAGER_TRANSFORMATION_RULE_HPP_INCLUDED
#define HKV_FILTER_MANAGER_TRANSFORMATION_RULE_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvBase.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/TransformationRules/hkvTransformationRule.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/Assets/hkvFilterManagerTransformableAsset.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Transformation/hkvFileTransformationStep.hpp>

class hkvProcessRunner;
struct hkvTransformationInput;
struct hkvTransformationOutput;

class hkvFilterManagerTransformationRule : public hkvTransformationRule
{
protected:
  /// \brief Context for converting a tagfile with the filter manager
  class TagfileContext : public hkvTransformationRule::Context, public hkvITransformationControl
  {
  public:
    TagfileContext(const hkvTransformationInput& input, hkvTransformationOutput& output, hkvFilterManagerTransformableAsset::TagfileEntry& entry);
    ~TagfileContext();
    virtual void cancelTransformation() HKV_OVERRIDE;

  public:
    hkvFilterManagerTransformableAsset::TagfileEntry& m_entry;
    hkStringPtr m_absSourcePath;
    hkStringPtr m_absTargetPath;
    hkStringPtr m_relTargetPath;
    hkStringPtr m_absSettingsPath;
    const char* m_filterSettings;
    hkRefPtr<hkvFileTransformationStep> m_currentTransformationStep;
  };


public:
  HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_TOOLS);

public: // static functions
  static void staticInit();
  static void staticDeInit();

public:
  hkvFilterManagerTransformationRule(hkvTargetPlatform platform);
  ~hkvFilterManagerTransformationRule();

public: // Interface
  virtual bool queryOutputFileSpecs(const hkvTransformationInput& input, hkvTransformationOutput& output, bool forLookUpTable) const HKV_OVERRIDE;
  virtual bool assetNeedsTransformation(const hkvAsset& asset) const HKV_OVERRIDE;

protected:
  hkResult executeTagfileEntryTransformation(TagfileContext& context) const;
  hkResult copySettingsToTempFile(TagfileContext& context) const;
  hkResult determineFilterManagerInput(TagfileContext& context) const;
  hkResult runTagfileConversion(TagfileContext& context, hkUint32 entryHash) const;

protected:
  static CRITICAL_SECTION s_protect;
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
