/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_CUSTOM_TEST_GENERATOR_H
#define HKB_CUSTOM_TEST_GENERATOR_H

#include <Behavior/Behavior/Generator/ReferencePose/hkbReferencePoseGenerator.h>
#include <Behavior/Behavior/Modifier/hkbModifier.h>
#include <Behavior/Behavior/Character/hkbBoneIndexArray.h>
#include <Behavior/Behavior/Character/hkbBoneWeightArray.h>
#include <Behavior/Behavior/Utils/hkbCustomIdSelector.h>
#include <Behavior/Utilities/Physics/hkbPhysicsBaseTypes.h>
#include <Common/Base/Container/String/hkStringPtr.h>

extern const class hkClass hkbCustomTestGeneratorClass;

struct hkbCustomTestGeneratorHiddenTypes : public hkbReferencePoseGenerator
{
	HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
	HK_DECLARE_REFLECTION();
	hkbCustomTestGeneratorHiddenTypes() {}
	hkbCustomTestGeneratorHiddenTypes( const hkbCustomTestGeneratorHiddenTypes& gen );
	hkbCustomTestGeneratorHiddenTypes( hkFinishLoadedObjectFlag flag );
	bool m_inheritedHiddenMember; //+hk.Description("This should not be visible")
protected:
	bool m_protectedInheritedHiddenMember; //+hk.Description("This should not be visible")
private:
	bool m_privateInheritedHiddenMember; //+hk.Description("This should not be visible")
};

struct hkbCustomTestGeneratorSimpleTypes : public hkbCustomTestGeneratorHiddenTypes
{
	HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
	HK_DECLARE_REFLECTION();
	hkbCustomTestGeneratorSimpleTypes() :
		m_simpleTypeHkInt64(0),
		m_simpleTypeHkUint64(0), 
		m_simpleTypeBool(false),
		m_simpleTypeHkBool(false),
		m_simpleTypeCString(HK_NULL),
		m_simpleTypeHkInt8(0),
		m_simpleTypeHkInt16(0),
		m_simpleTypeHkInt32(0),
		m_simpleTypeHkUint8(0),
		m_simpleTypeHkUint16(0),
		m_simpleTypeHkUint32(0),
		m_simpleTypeHkReal(0),
		
		
		m_simpleTypeHkInt8Clamp(0),
		m_simpleTypeHkInt16Clamp(0),
		m_simpleTypeHkInt32Clamp(0),
		m_simpleTypeHkUint8Clamp(0),
		m_simpleTypeHkUint16Clamp(0),
		m_simpleTypeHkUint32Clamp(0),
		m_simpleTypeHkRealClamp(0)
		{}
	hkbCustomTestGeneratorSimpleTypes( const hkbCustomTestGeneratorSimpleTypes& gen );
	hkbCustomTestGeneratorSimpleTypes( hkFinishLoadedObjectFlag flag );

	HK_ALIGN8(hkInt64) m_simpleTypeHkInt64; //+hk.Description("This should use a spinner with an initial value of 1 and min/max of -100/100")
	hkUint64 m_simpleTypeHkUint64; //+hk.Description("This should use a spinner with an initial value of 1 and min/max of 0/100")

	bool m_simpleHiddenTypeCopyStart; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_HIDDEN")
		//+hk.Description("This should not be visible")

	bool m_simpleTypeBool; //+hk.Description("This should have an unchecked checkbox; it can be assigned to a variable or character property")
	hkBool m_simpleTypeHkBool; //+hk.Description("This should have an unchecked checkbox; it can be assigned to a variable or character property")

	const char* m_simpleTypeCString; //+hk.Description("This should use an ellipses string editor")
	hkStringPtr m_simpleTypeHkStringPtr; //+hk.Description("This should use an ellipses string editor")

	hkInt8 m_simpleTypeHkInt8; //+hk.Description("This should use a spinner with an initial value of 0 and min/max of -100/100; it can be assigned to a variable or character property")
	hkInt16 m_simpleTypeHkInt16; //+hk.Description("This should use a spinner with an initial value of 0 and min/max of -100/100; it can be assigned to a variable or character property")
	hkInt32 m_simpleTypeHkInt32; //+hk.Description("This should use a spinner with an initial value of 0 and min/max of -100/100; it can be assigned to a variable or character property")
	hkUint8 m_simpleTypeHkUint8; //+hk.Description("This should use a spinner with an initial value of 0 and min/max of 0/100; it can be assigned to a variable or character property")
	hkUint16 m_simpleTypeHkUint16; //+hk.Description("This should use a spinner with an initial value of 0 and min/max of 0/100; it can be assigned to a variable or character property")
	hkUint32 m_simpleTypeHkUint32; //+hk.Description("This should use a spinner with an initial value of 0 and min/max of 0/100; it can be assigned to a variable or character property")
	hkReal m_simpleTypeHkReal; //+hk.Description("This should use a spinner with an initial value of 0 and min/max of 0/1.0; it can be assigned to a variable or character property")
	
	
		
	
		
	
		
	
		
	hkInt8 m_simpleTypeHkInt8Default; //+default(1)
		//+hk.Description("This should use a spinner with any initial value which can be reset to 1 with a right click on up/down arrows and min/max of -100/100; it can be assigned to a variable or character property")
	hkInt16 m_simpleTypeHkInt16Default; //+default(1)
		//+hk.Description("This should use a spinner with any initial value which can be reset to 1 with a right click on up/down arrows and min/max of -100/100; it can be assigned to a variable or character property")
	hkInt32 m_simpleTypeHkInt32Default; //+default(1)
		//+hk.Description("This should use a spinner with any initial value which can be reset to 1 with a right click on up/down arrows and min/max of -100/100; it can be assigned to a variable or character property")
	hkUint8 m_simpleTypeHkUint8Default; //+default(1)
		//+hk.Description("This should use a spinner with any initial value which can be reset to 1 with a right click on up/down arrows and min/max/100; it can be assigned to a variable or character property")
	hkUint16 m_simpleTypeHkUint16Default; //+default(1)
		//+hk.Description("This should use a spinner with any initial value which can be reset to 1 with a right click on up/down arrows and min/max/100; it can be assigned to a variable or character property")
	hkUint32 m_simpleTypeHkUint32Default; //+default(1)
		//+hk.Description("This should use a spinner with any initial value which can be reset to 1 with a right click on up/down arrows and min/max/100; it can be assigned to a variable or character property")
	hkReal m_simpleTypeHkRealDefault; //+default(1.0)
		//+hk.Description("This should use a spinner with any initial value which can be reset to 1 with a right click on up/down arrows and min/max of 0/1.0; it can be assigned to a variable or character property")

	hkInt8 m_simpleTypeHkInt8Clamp; //+hk.RangeInt32(absmax=2,softmax=1,softmin=-1,absmin=-2)
		//+hk.Description("This should use a spinner with an initial value of 0 and min/max of -1/1; it can be assigned to a variable or character property")
	hkInt16 m_simpleTypeHkInt16Clamp; //+hk.RangeInt32(absmax=2,softmax=1,softmin=-1,absmin=-2)
		//+hk.Description("This should use a spinner with an initial value of 0 and min/max of -1/1; it can be assigned to a variable or character property")
	hkInt32 m_simpleTypeHkInt32Clamp; //+hk.RangeInt32(absmax=2,softmax=1,softmin=-1,absmin=-2)
		//+hk.Description("This should use a spinner with an initial value of 0 and min/max of -1/1; it can be assigned to a variable or character property")
	hkUint8 m_simpleTypeHkUint8Clamp; //+hk.RangeInt32(absmax=2,softmax=1)
		//+hk.Description("This should use a spinner with an initial value of 0 and min/max of 0/1; it can be assigned to a variable or character property")
	hkUint16 m_simpleTypeHkUint16Clamp; //+hk.RangeInt32(absmax=2,softmax=1)
		//+hk.Description("This should use a spinner with an initial value of 0 and min/max of 0/1; it can be assigned to a variable or character property")
	hkUint32 m_simpleTypeHkUint32Clamp; //+hk.RangeInt32(absmax=2,softmax=1)
		//+hk.Description("This should use a spinner with an initial value of 0 and min/max of 0/1; it can be assigned to a variable or character property")
	hkReal m_simpleTypeHkRealClamp; //+hk.RangeReal(absmax=2.0,softmax=1.0,softmin=-1.0,absmin=-2.0)
		//+hk.Description("This should use a spinner with an initial value of 0 and min/max of -1.0/1.0; it can be assigned to a variable or character property")

	bool m_simpleHiddenTypeCopyEnd; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_HIDDEN")
		//+hk.Description("This should not be visible")
};

struct hkbCustomTestGeneratorComplexTypes : hkbCustomTestGeneratorSimpleTypes
{
	HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
	HK_DECLARE_REFLECTION();
	hkbCustomTestGeneratorComplexTypes() :
		m_complexTypeEnumHkInt8(CUSTOM_ENUM_ALA),
		m_complexTypeEnumHkInt16(CUSTOM_ENUM_ALA),
		m_complexTypeEnumHkInt32(CUSTOM_ENUM_ALA),
		m_complexTypeEnumHkUint8(CUSTOM_ENUM_ALA),
		m_complexTypeEnumHkUint16(CUSTOM_ENUM_ALA),
		m_complexTypeEnumHkUint32(CUSTOM_ENUM_ALA),
		m_complexTypeEnumHkInt8InvalidCheck((CustomEnum)7),
		m_complexTypeEnumHkInt16InvalidCheck((CustomEnum)7),
		m_complexTypeEnumHkInt32InvalidCheck((CustomEnum)7),
		m_complexTypeEnumHkUint8InvalidCheck((CustomEnum)7),
		m_complexTypeEnumHkUint16InvalidCheck((CustomEnum)7),
		m_complexTypeEnumHkUint32InvalidCheck((CustomEnum)7),
		m_complexTypeFlagsHkInt8(CUSTOM_FLAG_NONE),
		m_complexTypeFlagsHkInt16(CUSTOM_FLAG_NONE),
		m_complexTypeFlagsHkInt32(CUSTOM_FLAG_NONE),
		m_complexTypeFlagsHkUint8(CUSTOM_FLAG_NONE),
		m_complexTypeFlagsHkUint16(CUSTOM_FLAG_NONE),
		m_complexTypeFlagsHkUint32(CUSTOM_FLAG_NONE),
		m_complexTypeFlagsHkInt8InvalidCheck(-1),
		m_complexTypeFlagsHkInt16InvalidCheck(-1),
		m_complexTypeFlagsHkInt32InvalidCheck(-1),
		m_complexTypeFlagsHkUint8InvalidCheck((hkUint8)-1),
		m_complexTypeFlagsHkUint16InvalidCheck((hkUint16)-1),
		m_complexTypeFlagsHkUint32InvalidCheck((hkUint32)-1)
	{
		m_complexTypeHkQuaternion.setIdentity();
		m_complexTypeHkVector4.setZero();
	}
	hkbCustomTestGeneratorComplexTypes( const hkbCustomTestGeneratorComplexTypes& gen );
	hkbCustomTestGeneratorComplexTypes( hkFinishLoadedObjectFlag flag );

	hkbRigidBodyHandleRef m_complexTypeHkObjectPtr; //+hk.Description("This should use an ellipses select objects form dialog")

	bool m_complexHiddenTypeCopyStart; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_HIDDEN")
		//+hk.Description("This should not be visible")

	hkQuaternion m_complexTypeHkQuaternion; //+hk.Description("This should use a rotation editor with an initial identity value; it can be assigned to a variable or character property")
	hkVector4 m_complexTypeHkVector4; //+hk.Description("This should use a vector editor with an initial zero vector value; it can be assigned to a variable or character property")

	enum CustomEnum
	{
		CUSTOM_ENUM_ALA = 0,
		CUSTOM_ENUM_DEPECHE = 1,
		CUSTOM_ENUM_FURIOUS = 5,
	};

	hkEnum<CustomEnum, hkInt8> m_complexTypeEnumHkInt8; //+hk.Description("This should use have a combo box with an initial value of \"Ala\"")
	hkEnum<CustomEnum, hkInt16> m_complexTypeEnumHkInt16; //+hk.Description("This should use have a combo box with an initial value of \"Ala\"")
	hkEnum<CustomEnum, hkInt32> m_complexTypeEnumHkInt32; //+hk.Description("This should use have a combo box with an initial value of \"Ala\"")
	hkEnum<CustomEnum, hkUint8> m_complexTypeEnumHkUint8; //+hk.Description("This should use have a combo box with an initial value of \"Ala\"")
	hkEnum<CustomEnum, hkUint16> m_complexTypeEnumHkUint16; //+hk.Description("This should use have a combo box with an initial value of \"Ala\"")
	hkEnum<CustomEnum, hkUint32> m_complexTypeEnumHkUint32; //+hk.Description("This should use have a combo box with an initial value of \"Ala\"")

	
		
	
		
	
		
	
		
	
		
	
		

	hkEnum<CustomEnum, hkInt8> m_complexTypeEnumHkInt8InvalidCheck; //+hk.Description("This should use have a combo box with no initial value")
	hkEnum<CustomEnum, hkInt16> m_complexTypeEnumHkInt16InvalidCheck; //+hk.Description("This should use have a combo box with no initial value")
	hkEnum<CustomEnum, hkInt32> m_complexTypeEnumHkInt32InvalidCheck; //+hk.Description("This should use have a combo box with no initial value")
	hkEnum<CustomEnum, hkUint8> m_complexTypeEnumHkUint8InvalidCheck; //+hk.Description("This should use have a combo box with no initial value")
	hkEnum<CustomEnum, hkUint16> m_complexTypeEnumHkUint16InvalidCheck; //+hk.Description("This should use have a combo box with no initial value")
	hkEnum<CustomEnum, hkUint32> m_complexTypeEnumHkUint32InvalidCheck; //+hk.Description("This should use have a combo box with no initial value")

	enum CustomFlag
	{
		//+defineflags(true)
		CUSTOM_FLAG_NONE = 0x0,
		CUSTOM_FLAG_UNO = 0x1,
		CUSTOM_FLAG_ZWEI = 0x2,
		CUSTOM_FLAG_SHI_OR_YON = 0x4,
		CUSTOM_FLAG_LOTS_O_BITS = 0xf0,
	};

	hkFlags<CustomFlag, hkInt8> m_complexTypeFlagsHkInt8; //+hk.Description("This should use have a custom flag selection box with an initial value of \"None\"")
	hkFlags<CustomFlag, hkInt16> m_complexTypeFlagsHkInt16; //+hk.Description("This should use have a custom flag selection box with an initial value of \"None\"")
	hkFlags<CustomFlag, hkInt32> m_complexTypeFlagsHkInt32; //+hk.Description("This should use have a custom flag selection box with an initial value of \"None\"")
	hkFlags<CustomFlag, hkUint8> m_complexTypeFlagsHkUint8; //+hk.Description("This should use have a custom flag selection box with an initial value of \"None\"")
	hkFlags<CustomFlag, hkUint16> m_complexTypeFlagsHkUint16; //+hk.Description("This should use have a custom flag selection box with an initial value of \"None\"")
	hkFlags<CustomFlag, hkUint32> m_complexTypeFlagsHkUint32; //+hk.Description("This should use have a custom flag selection box with an initial value of \"None\"")

	
		
	
		
	
		
	
		
	
		
	
		

	hkFlags<CustomFlag, hkInt8> m_complexTypeFlagsHkInt8InvalidCheck; //+hk.Description("This should use have a custom flag selection box with an initial value of -1")
	hkFlags<CustomFlag, hkInt16> m_complexTypeFlagsHkInt16InvalidCheck; //+hk.Description("This should use have a custom flag selection box with an initial value of -1")
	hkFlags<CustomFlag, hkInt32> m_complexTypeFlagsHkInt32InvalidCheck; //+hk.Description("This should use have a custom flag selection box with an initial value of -1")
	hkFlags<CustomFlag, hkUint8> m_complexTypeFlagsHkUint8InvalidCheck; //+hk.Description("This should use have a custom flag selection box with an initial value of -1")
	hkFlags<CustomFlag, hkUint16> m_complexTypeFlagsHkUint16InvalidCheck; //+hk.Description("This should use have a custom flag selection box with an initial value of -1")
	hkFlags<CustomFlag, hkUint32> m_complexTypeFlagsHkUint32InvalidCheck; //+hk.Description("This should use have a custom flag selection box with an initial value of -1")

	bool m_complexHiddenTypeCopyEnd; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_HIDDEN")
		//+hk.Description("This should not be visible")
};

struct hkbCustomTestGeneratorNestedTypesBase : public hkbCustomTestGeneratorComplexTypes
{
	HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
	HK_DECLARE_REFLECTION();
	hkbCustomTestGeneratorNestedTypesBase() : m_nestedTypeHkbGeneratorPtr( HK_NULL ), m_nestedTypeHkbModifierPtr( HK_NULL ), m_nestedTypeHkbCustomIdSelectorPtr( HK_NULL ) {}
	hkbCustomTestGeneratorNestedTypesBase( const hkbCustomTestGeneratorNestedTypesBase& gen );
	hkbCustomTestGeneratorNestedTypesBase( hkFinishLoadedObjectFlag flag );

	// nested pointers
	hkbGenerator* m_nestedTypeHkbGeneratorPtr; //+hk.Description("This should use an ellipses select objects form dialog")
	hkRefPtr<hkbGenerator> m_nestedTypeHkbGeneratorRefPtr; //+hk.Description("This should use an ellipses select objects form dialog")
	hkbModifier* m_nestedTypeHkbModifierPtr; //+hk.Description("This should use an ellipses select objects form dialog")
	hkRefPtr<hkbModifier> m_nestedTypeHkbModifierRefPtr; //+hk.Description("This should use an ellipses select objects form dialog")
	hkbCustomIdSelector* m_nestedTypeHkbCustomIdSelectorPtr; //+hk.Description("This should use an ellipses select objects form dialog")
	hkRefPtr<hkbCustomIdSelector> m_nestedTypeHkbCustomIdSelectorRefPtr; //+hk.Description("This should use an ellipses select objects form dialog")

	// nested arrays
	hkArray<bool> m_nestedTypeArrayBool; //+hk.Description("This should have \"Double-Click to Edit\" which drills down into the array; then you can add rows with the proper control for the type")
	hkArray<hkBool> m_nestedTypeArrayHkBool; //+hk.Description("This should have \"Double-Click to Edit\" which drills down into the array; then you can add rows with the proper control for the type")
	hkArray<const char*> m_nestedTypeArrayCString; //+hk.Description("This should have \"Double-Click to Edit\" which drills down into the array; then you can add rows with the proper control for the type")
	hkArray<hkStringPtr> m_nestedTypeArrayHkStringPtr; //+hk.Description("This should have \"Double-Click to Edit\" which drills down into the array; then you can add rows with the proper control for the type")
	hkArray<hkInt8> m_nestedTypeArrayHkInt8; //+hk.Description("This should have \"Double-Click to Edit\" which drills down into the array; then you can add rows with the proper control for the type")
	hkArray<hkInt16> m_nestedTypeArrayHkInt16; //+hk.Description("This should have \"Double-Click to Edit\" which drills down into the array; then you can add rows with the proper control for the type")
	hkArray<hkInt32> m_nestedTypeArrayHkInt32; //+hk.Description("This should have \"Double-Click to Edit\" which drills down into the array; then you can add rows with the proper control for the type")
	hkArray<hkUint8> m_nestedTypeArrayHkUint8; //+hk.Description("This should have \"Double-Click to Edit\" which drills down into the array; then you can add rows with the proper control for the type")
	hkArray<hkUint16> m_nestedTypeArrayHkUint16; //+hk.Description("This should have \"Double-Click to Edit\" which drills down into the array; then you can add rows with the proper control for the type")
	hkArray<hkUint32> m_nestedTypeArrayHkUint32; //+hk.Description("This should have \"Double-Click to Edit\" which drills down into the array; then you can add rows with the proper control for the type")
	hkArray<hkReal> m_nestedTypeArrayHkReal; //+hk.Description("This should have \"Double-Click to Edit\" which drills down into the array; then you can add rows with the proper control for the type")
	hkArray<hkbGenerator*> m_nestedTypeArrayHkbGeneratorPtr; //+hk.Description("This should have \"Double-Click to Edit\" which drills down into the array; then you can add rows with the proper control for the type")
	hkArray<hkRefPtr<hkbGenerator> > m_nestedTypeArrayHkbGeneratorRefPtr; //+hk.Description("This should have \"Double-Click to Edit\" which drills down into the array; then you can add rows with the proper control for the type")
	hkArray<hkbModifier*> m_nestedTypeArrayHkbModifierPtr; //+hk.Description("This should have \"Double-Click to Edit\" which drills down into the array; then you can add rows with the proper control for the type")
	hkArray<hkRefPtr<hkbModifier> > m_nestedTypeArrayHkbModifierRefPtr; //+hk.Description("This should have \"Double-Click to Edit\" which drills down into the array; then you can add rows with the proper control for the type")
	hkArray<hkbCustomIdSelector*> m_nestedTypeArrayHkbCustomIdSelectorPtr; //+hk.Description("This should have \"Double-Click to Edit\" which drills down into the array; then you can add rows with the proper control for the type")
	hkArray<hkRefPtr<hkbCustomIdSelector> > m_nestedTypeArrayHkbCustomIdSelectorRefPtr; //+hk.Description("This should have \"Double-Click to Edit\" which drills down into the array; then you can add rows with the proper control for the type")
};

struct hkbCustomTestGeneratorNestedTypes : public hkbCustomTestGeneratorNestedTypesBase
{
	HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
	HK_DECLARE_REFLECTION();
	hkbCustomTestGeneratorNestedTypes() {}
	hkbCustomTestGeneratorNestedTypes( const hkbCustomTestGeneratorNestedTypes& gen );
	hkbCustomTestGeneratorNestedTypes( hkFinishLoadedObjectFlag flag );

	// nested structs
	struct hkbCustomTestGeneratorNestedTypesBase m_nestedTypeStruct; //+hk.Description("This should have \"Double-Click to Edit\" which drills down into the struct; populating with the correct controls for its contents")
	hkArray<struct hkbCustomTestGeneratorNestedTypesBase> m_nestedTypeArrayStruct; //+hk.Description("This should have \"Double-Click to Edit\" which drills down into the array; then you can add rows with nested structs")
};

struct hkbCustomTestGeneratorBoneTypes : public hkbCustomTestGeneratorNestedTypes
{
	HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
	HK_DECLARE_REFLECTION();
	hkbCustomTestGeneratorBoneTypes() :
		m_oldBoneIndex(-1),
		m_oldBoneIndexNoVar(-1),
		m_boneIndex(-1),
		m_boneIndexNoVar(-1),
		m_boneChainIndex0(-1),
		m_boneChainIndex1(-1),
		m_boneChainIndex2(-1),
		m_boneContractIndex0(-1),
		m_boneContractIndex1(-1),
		m_boneContractIndex2(-1) {}
	hkbCustomTestGeneratorBoneTypes( const hkbCustomTestGeneratorBoneTypes& gen );
	hkbCustomTestGeneratorBoneTypes( hkFinishLoadedObjectFlag flag );

	bool m_boneHiddenTypeCopyStart; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_HIDDEN")
		//+hk.Description("This should not be visible")

	hkInt16 m_oldBoneIndex; //+hkb.RoleAttribute("ROLE_BONE_INDEX")
	hkInt16 m_oldBoneIndexNoVar; //+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_NOT_CHARACTER_PROPERTY")
	hkInt16 m_boneIndex; //+hkb.RoleAttribute("ROLE_BONE_INDEX")
		//+hk.Ui(group="Single Bone Index")
	hkInt16 m_boneIndexNoVar; //+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_NOT_CHARACTER_PROPERTY")
		//+hk.Ui(group="Single Bone Index No Var")

	hkInt16 m_boneChainIndex0; //+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_CHAIN")
		//+hk.Ui(group="Chain Indices")
	hkInt16 m_boneChainIndex1; //+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_CHAIN")
		//+hk.Ui(group="Chain Indices")
	hkInt16 m_boneChainIndex2; //+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_CHAIN")
		//+hk.Ui(group="Chain Indices")

	hkInt16 m_boneContractIndex0; //+hkb.RoleAttribute("ROLE_BONE_INDEX")
		//+hk.Ui(group="Contract Indices")
	hkInt16 m_boneContractIndex1; //+hkb.RoleAttribute("ROLE_BONE_INDEX")
		//+hk.Ui(group="Contract Indices")
	hkInt16 m_boneContractIndex2; //+hkb.RoleAttribute("ROLE_BONE_INDEX")
		//+hk.Ui(group="Contract Indices")

	bool m_boneHiddenTypeCopyEnd; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_HIDDEN")
		//+hk.Description("This should not be visible")

	hkRefPtr<hkbBoneWeightArray> m_boneWeightArray;
	hkRefPtr<hkbBoneIndexArray> m_boneIndexArray;
	
	
};

struct hkbCustomTestGeneratorAnnotatedTypes : public hkbCustomTestGeneratorBoneTypes
{
	HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
	HK_DECLARE_REFLECTION();
	hkbCustomTestGeneratorAnnotatedTypes() :
		m_annotatedTypeCStringFilename(HK_NULL),
		m_annotatedTypeHkStringPtrFilename(HK_NULL),
		m_annotatedTypeCStringScript(HK_NULL),
		m_annotatedTypeHkStringPtrScript(HK_NULL),
		m_annotatedTypeCStringBoneAttachment(HK_NULL),
		m_annotatedTypeHkStringPtrBoneAttachment(HK_NULL),
		m_annotatedTypeCStringLocalFrame(HK_NULL),
		m_annotatedTypeHkStringPtrLocalFrame(HK_NULL),
		m_annotatedTypeHkInt32EventID(-1),
		m_annotatedTypeHkInt32VariableIndex(-1),
		m_annotatedTypeHkInt32AttributeIndex(-1),
		m_annotatedTypeHkRealTime(-1.0f),
		m_annotatedTypeBoolNoVar(false),
		m_annotatedTypeHkBoolNoVar(false),
		m_annotatedTypeHkInt8NoVar(0),
		m_annotatedTypeHkInt16NoVar(0),
		m_annotatedTypeHkInt32NoVar(0),
		m_annotatedTypeHkUint8NoVar(0),
		m_annotatedTypeHkUint16NoVar(0),
		m_annotatedTypeHkUint32NoVar(0),
		m_annotatedTypeHkRealNoVar(0),
		m_annotatedTypeBoolOutput(false),
		m_annotatedTypeHkBoolOutput(false),
		m_annotatedTypeHkInt8Output(0),
		m_annotatedTypeHkInt16Output(0),
		m_annotatedTypeHkInt32Output(0),
		m_annotatedTypeHkUint8Output(0),
		m_annotatedTypeHkUint16Output(0),
		m_annotatedTypeHkUint32Output(0),
		m_annotatedTypeHkRealOutput(0),
		m_annotatedHiddenTypeCString1(HK_NULL),
		m_annotatedHiddenTypeCString2(HK_NULL) {}
	hkbCustomTestGeneratorAnnotatedTypes( const hkbCustomTestGeneratorAnnotatedTypes& gen );
	hkbCustomTestGeneratorAnnotatedTypes( hkFinishLoadedObjectFlag flag );

	const char* m_annotatedTypeCStringFilename; //+hkb.RoleAttribute("ROLE_FILE_NAME")
		//+hk.Description("This should use an ellipses file name selection dialog")
	hkStringPtr m_annotatedTypeHkStringPtrFilename; //+hkb.RoleAttribute("ROLE_FILE_NAME")
		//+hk.Description("This should use an ellipses file name selection dialog")
	const char* m_annotatedTypeCStringScript; //+hkb.RoleAttribute("ROLE_SCRIPT")
		//+hk.Description("This should use an ellipses script control (opens Scripts debugger)")
	hkStringPtr m_annotatedTypeHkStringPtrScript; //+hkb.RoleAttribute("ROLE_SCRIPT")
		//+hk.Description("This should use an ellipses script control (opens Scripts debugger)")
	const char* m_annotatedTypeCStringBoneAttachment; //+hkb.RoleAttribute("ROLE_BONE_ATTACHMENT")
		//+hk.Description("This should use an ellipses bone attachment dialog")
	hkStringPtr m_annotatedTypeHkStringPtrBoneAttachment; //+hkb.RoleAttribute("ROLE_BONE_ATTACHMENT")
		//+hk.Description("This should use an ellipses bone attachment dialog")
	const char* m_annotatedTypeCStringLocalFrame; //+hkb.RoleAttribute("ROLE_LOCAL_FRAME")
		//+hk.Description("This should use an ellipses local frame dialog")
	hkStringPtr m_annotatedTypeHkStringPtrLocalFrame; //+hkb.RoleAttribute("ROLE_LOCAL_FRAME")
		//+hk.Description("This should use an ellipses local frame dialog")

	bool m_annotatedHiddenTypeCopyStart; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_HIDDEN")
		//+hk.Description("This should not be visible")

	hkInt32 m_annotatedTypeHkInt32EventID; //+hkb.RoleAttribute("ROLE_EVENT_ID")
		//+hk.Description("This should use an ellipses local frame dialog")
	hkInt32 m_annotatedTypeHkInt32VariableIndex; //+hkb.RoleAttribute("ROLE_VARIABLE_INDEX")
		//+hk.Description("This should use an ellipses local frame dialog")
	hkInt32 m_annotatedTypeHkInt32AttributeIndex; //+hkb.RoleAttribute("ROLE_ATTRIBUTE_INDEX")
		//+hk.Description("This should use an ellipses local frame dialog")
	hkReal m_annotatedTypeHkRealTime; //+hkb.RoleAttribute("ROLE_TIME")
		//+hk.Description("This should use an ellipses local frame dialog")

	bool m_annotatedTypeBoolNoVar; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NOT_VARIABLE | FLAG_NOT_CHARACTER_PROPERTY")
		//+hk.Description("This should have an unchecked checkbox")
	hkBool m_annotatedTypeHkBoolNoVar; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NOT_VARIABLE | FLAG_NOT_CHARACTER_PROPERTY")
		//+hk.Description("This should have an unchecked checkbox")
	hkInt8 m_annotatedTypeHkInt8NoVar; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NOT_VARIABLE | FLAG_NOT_CHARACTER_PROPERTY")
		//+hk.Description("This should use a spinner with an initial value of 0 and min/max of -100/100")
	hkInt16 m_annotatedTypeHkInt16NoVar; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NOT_VARIABLE | FLAG_NOT_CHARACTER_PROPERTY")
		//+hk.Description("This should use a spinner with an initial value of 0 and min/max of -100/100")
	hkInt32 m_annotatedTypeHkInt32NoVar; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NOT_VARIABLE | FLAG_NOT_CHARACTER_PROPERTY")
		//+hk.Description("This should use a spinner with an initial value of 0 and min/max of -100/100")
	hkUint8 m_annotatedTypeHkUint8NoVar; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NOT_VARIABLE | FLAG_NOT_CHARACTER_PROPERTY")
		//+hk.Description("This should use a spinner with an initial value of 0 and min/max of 0/100")
	hkUint16 m_annotatedTypeHkUint16NoVar; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NOT_VARIABLE | FLAG_NOT_CHARACTER_PROPERTY")
		//+hk.Description("This should use a spinner with an initial value of 0 and min/max of 0/100")
	hkUint32 m_annotatedTypeHkUint32NoVar; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NOT_VARIABLE | FLAG_NOT_CHARACTER_PROPERTY")
		//+hk.Description("This should use a spinner with an initial value of 0 and min/max of 0/100")
	hkReal m_annotatedTypeHkRealNoVar; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NOT_VARIABLE | FLAG_NOT_CHARACTER_PROPERTY")
		//+hk.Description("This should use a spinner with an initial value of 0 and min/max of 0/1.0")

	bool m_annotatedTypeBoolOutput; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
		//+hk.Description("This should have \"<==OUTPUT==>\" listed in the property name and \"n/a\" for the control; it can be assigned to a variable or character property")
	hkBool m_annotatedTypeHkBoolOutput; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
		//+hk.Description("This should have \"<==OUTPUT==>\" listed in the property name and \"n/a\" for the control; it can be assigned to a variable or character property")
	hkInt8 m_annotatedTypeHkInt8Output; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
		//+hk.Description("This should have \"<==OUTPUT==>\" listed in the property name and \"n/a\" for the control; it can be assigned to a variable or character property")
	hkInt16 m_annotatedTypeHkInt16Output; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
		//+hk.Description("This should have \"<==OUTPUT==>\" listed in the property name and \"n/a\" for the control; it can be assigned to a variable or character property")
	hkInt32 m_annotatedTypeHkInt32Output; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
		//+hk.Description("This should have \"<==OUTPUT==>\" listed in the property name and \"n/a\" for the control; it can be assigned to a variable or character property")
	hkUint8 m_annotatedTypeHkUint8Output; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
		//+hk.Description("This should have \"<==OUTPUT==>\" listed in the property name and \"n/a\" for the control; it can be assigned to a variable or character property")
	hkUint16 m_annotatedTypeHkUint16Output; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
		//+hk.Description("This should have \"<==OUTPUT==>\" listed in the property name and \"n/a\" for the control; it can be assigned to a variable or character property")
	hkUint32 m_annotatedTypeHkUint32Output; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
		//+hk.Description("This should have \"<==OUTPUT==>\" listed in the property name and \"n/a\" for the control; it can be assigned to a variable or character property")
	hkReal m_annotatedTypeHkRealOutput; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
		//+hk.Description("This should have \"<==OUTPUT==>\" listed in the property name and \"n/a\" for the control; it can be assigned to a variable or character property")
	
	bool m_annotatedHiddenTypeBool; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_HIDDEN")
		//+hk.Description("This should not be visible")
	hkBool m_annotatedHiddenTypeHkBool; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_HIDDEN")
		//+hk.Description("This should not be visible")
	const char* m_annotatedHiddenTypeCString1; //+hkb.RoleAttribute("ROLE_SCRIPT","FLAG_HIDDEN")
		//+hk.Description("This should not be visible")
	hkStringPtr m_annotatedHiddenTypeHkStringPtr1; //+hkb.RoleAttribute("ROLE_FILE_NAME","FLAG_HIDDEN")
		//+hk.Description("This should not be visible")
	const char* m_annotatedHiddenTypeCString2; //+hkb.RoleAttribute("ROLE_BONE_ATTACHMENT","FLAG_HIDDEN")
		//+hk.Description("This should not be visible")
	hkStringPtr m_annotatedHiddenTypeHkStringPtr2; //+hkb.RoleAttribute("ROLE_LOCAL_FRAME","FLAG_HIDDEN")
		//+hk.Description("This should not be visible")
	hkInt8 m_annotatedHiddenTypeHkInt8; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_HIDDEN")
		//+hk.Description("This should not be visible")
	hkInt16 m_annotatedHiddenTypeHkInt16; //+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_HIDDEN")
		//+hk.Description("This should not be visible")
	hkInt32 m_annotatedHiddenTypeHkInt32; //+hkb.RoleAttribute("ROLE_EVENT_ID","FLAG_HIDDEN")
		//+hk.Description("This should not be visible")
	hkUint8 m_annotatedHiddenTypeHkUint8; //+hkb.RoleAttribute("ROLE_VARIABLE_INDEX","FLAG_HIDDEN")
		//+hk.Description("This should not be visible")
	hkUint16 m_annotatedHiddenTypeHkUint16; //+hkb.RoleAttribute("ROLE_ATTRIBUTE_INDEX","FLAG_HIDDEN")
		//+hk.Description("This should not be visible")
	hkUint32 m_annotatedHiddenTypeHkUint32; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_HIDDEN")
		//+hk.Description("This should not be visible")
	
		

	bool m_annotatedHiddenTypeCopyEnd; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_HIDDEN")
		//+hk.Description("This should not be visible")
};

	/// This custom generator demonstrates all of the public properties that HAT is capable of exposing.
class hkbCustomTestGenerator : public hkbCustomTestGeneratorAnnotatedTypes
{
	//+hk.Ui(hideBaseClassMembers="inheritedHiddenMember")
	//+version(4)
	// m_ is prepended to these member names when resolved by HAT.
	
	// Note: The attribute is looked for in the *very* specific format of:
	// class Klass : public KlassBase /*can't be next line*/
	// {
	//     //+hk.Ui(...)

	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbCustomTestGenerator() {}

	protected:

		hkbCustomTestGenerator( const hkbCustomTestGenerator& gen );

	public:

			// packfile constructor
		hkbCustomTestGenerator( hkFinishLoadedObjectFlag flag );

			// hkbBindable interface implementation.
		virtual void collectBindables( hkbBindableCollector& collector ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

protected:
	bool m_protectedHiddenMember; //+hk.Description("This should not be visible")
private:
	bool m_privateHiddenMember; //+hk.Description("This should not be visible")
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
