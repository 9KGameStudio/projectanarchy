/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKAI_USERDATA_UTILS_H
#define HKAI_USERDATA_UTILS_H

#include <Ai/Pathfinding/hkaiBaseTypes.h>

/// Utilities for manipulating user data arrays
class hkaiUserDataUtils 
{
public:
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE, hkaiUserDataUtils);

	template<typename DATA>
	static HK_FORCE_INLINE void copyUserData(DATA* dst, const DATA* src, int striding);

	template<typename DATA>
	static HK_FORCE_INLINE void copyUserData( hkArrayBase<DATA>& dataArray, int dstIndex, int srcIndex, int striding );

	template<typename DATA>
	static inline hkResult restride( hkArray<DATA>& data, int oldStride, int newStride, int N, int fillValue );
};

#include <Ai/Pathfinding/Utilities/hkaiUserDataUtils.inl>

#endif //HKAI_NAVMESH_GEOMETRY_H

/*
 * Havok SDK - Base file, BUILD(#20140327)
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
