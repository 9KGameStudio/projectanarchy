/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKAI_VALIDATE_RANGE_H
#define HKAI_VALIDATE_RANGE_H


	/// A utility for checking that members are in valid range as defined by hk.RangeInt32 and hk.RangeReal.
class hkaiValidateRange
{
public:
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_NAVMESH, hkaiValidateRange);

		/// Check the float and integer members of the specified object, assuming it is of the type described by the hkClass.
	static void HK_CALL checkMembers( const void* obj, const hkClass& klass );

		/// Check the members of the specified object.
	template< typename KLASS >
	static void checkMembers( const KLASS* obj )
	{
		checkMembers( obj, KLASS::staticClass() );
	}

};
#endif // HKAI_VALIDATE_RANGE_H

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
