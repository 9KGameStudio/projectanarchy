/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKV_COLLECTION_TRANSFORMATION_RULE_HPP_INCLUDED
#define HKV_COLLECTION_TRANSFORMATION_RULE_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvBase.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/TransformationRules/hkvTransformationRule.hpp>


class hkvProcessRunner;
struct hkvTransformationInput;
struct hkvTransformationOutput;

class hkvCollectionTransformationRule : public hkvTransformationRule
{
public:
  HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_TOOLS);

public: // static functions
  static void staticInit();
  static void staticDeInit();
  static hkvTransformationRule* createTransformationRule(hkvTargetPlatform targetPlatform);

public:
  hkvCollectionTransformationRule (hkvTargetPlatform platform);
  ~hkvCollectionTransformationRule();

  // IProperties implementation
  virtual void getProperties(hkvPropertyList& properties, hkvProperty::Purpose purpose) const HKV_OVERRIDE;
  virtual void setProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, hkUint32 stackIndex, hkvProperty::Purpose purpose) HKV_OVERRIDE;

public: // Interface
  virtual hkUint32 getTypeIndex() const HKV_OVERRIDE;
  virtual const char* getTypeName() const HKV_OVERRIDE;

  virtual bool executeTransformation(const hkvTransformationInput& input, hkvTransformationOutput& output) const HKV_OVERRIDE;
  virtual bool queryOutputFileSpecs(const hkvTransformationInput& input, hkvTransformationOutput& output) const HKV_OVERRIDE;

private:
  hkResult runConversion(Context& context) const;

  static hkUint32 s_ruleTypeIndex;
};


#endif

/*
 * Havok SDK - Base file, BUILD(#20131218)
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2013
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
