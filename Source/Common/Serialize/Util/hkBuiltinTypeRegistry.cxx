/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_CLASSES_FILE
#	error Please define HK_CLASSES_FILE before including this file
#endif

// The default is to register all typeinfos and classes

#ifndef HK_REGISTER_TYPEINFOS
#	define HK_REGISTER_TYPEINFOS 1
#endif

#ifndef HK_REGISTER_CLASSES
#	define HK_REGISTER_CLASSES 1
#endif

#include <Common/Serialize/Util/hkBuiltinTypeRegistry.h>
#include <Common/Base/Reflection/hkTypeInfo.h>

class hkClass;
class hkTypeInfo;

#ifndef HK_TYPE_REG_NAMESPACE
#define HK_TYPE_REG_NAMESPACE hkBuiltinTypeRegistry
#endif

// Forward declarations of all used classes and typeinfos
#if HK_REGISTER_TYPEINFOS || HK_REGISTER_CLASSES
#	define HK_CLASS(E, A) extern E const hkClass A##Class; extern E const hkTypeInfo A##TypeInfo;
#	define HK_STRUCT(E, A) extern E const hkClass A##Class; extern E const hkTypeInfo A##TypeInfo;
#	define HK_ABSTRACT_CLASS(E, A) extern E const hkClass A##Class;
#		include HK_CLASSES_FILE
#	undef HK_STRUCT
#	undef HK_CLASS
#	undef HK_ABSTRACT_CLASS
#endif

#if HK_REGISTER_TYPEINFOS
	// nonvirtual type infos
#	define HK_CLASS(E, A) // nothing
#	define HK_STRUCT(E, A) // nothing
#	define HK_ABSTRACT_CLASS(E, A) // nothing
#	include HK_CLASSES_FILE
#	undef HK_STRUCT
#	undef HK_CLASS
#	undef HK_ABSTRACT_CLASS

	// List of all typeinfos
	const hkTypeInfo* const HK_TYPE_REG_NAMESPACE::StaticLinkedTypeInfos[] =
	{
#		define HK_CLASS(E, A) &A##TypeInfo,
#		define HK_STRUCT(E, A) // nothing
#		define HK_ABSTRACT_CLASS(E, A) // nothing
#		include HK_CLASSES_FILE

#		undef HK_CLASS
#		define HK_CLASS(E, A) // nothing
#		undef HK_STRUCT
#		define HK_STRUCT(E, A) &A##TypeInfo,
#		include HK_CLASSES_FILE

#		undef HK_STRUCT
#		undef HK_CLASS
#		undef HK_ABSTRACT_CLASS
		HK_NULL
	};
#else

	// empty typeinfo list
	const hkTypeInfo* const HK_TYPE_REG_NAMESPACE::StaticLinkedTypeInfos[] =
	{
		HK_NULL
	};

#endif

#if HK_REGISTER_CLASSES

	// List of all classes
	// The strange construction is to ensure that the classes
	// are in the same order of the typeinfo list.


	const hkClass* const HK_TYPE_REG_NAMESPACE::StaticLinkedClasses[] =
	{
#		define HK_CLASS(E, A) &A##Class,
#		define HK_STRUCT(E, A)			// nothing
#		define HK_ABSTRACT_CLASS(E, A) // nothing
#		include HK_CLASSES_FILE
#		undef HK_STRUCT
#		undef HK_CLASS
#		undef HK_ABSTRACT_CLASS

#		define HK_CLASS(E, A)			// nothing
#		define HK_ABSTRACT_CLASS(E, A) // nothing
#		define HK_STRUCT(E, A) &A##Class,
#		include HK_CLASSES_FILE
#		undef HK_STRUCT
#		undef HK_CLASS
#		undef HK_ABSTRACT_CLASS

#		define HK_CLASS(E, A)			// nothing
#		define HK_STRUCT(E, A)			// nothing
#		define HK_ABSTRACT_CLASS(E, A) &A##Class,
#		include HK_CLASSES_FILE
#		undef HK_STRUCT
#		undef HK_CLASS
#		undef HK_ABSTRACT_CLASS

		HK_NULL
	};

#else

	// empty classes list
	const hkClass* const HK_TYPE_REG_NAMESPACE::StaticLinkedClasses[] =
	{
		HK_NULL
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
