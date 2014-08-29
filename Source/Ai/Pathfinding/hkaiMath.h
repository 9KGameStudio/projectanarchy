/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_MATH_H
#define HKAI_MATH_H

#include <Ai/Internal/hkaiExport.h>

	/// Wrappers around selected hkMath methods, to allow hkSimdReal->hkSimdReal operations.
namespace hkaiMath
{
	HK_FORCE_INLINE static hkSimdReal HK_CALL interpolate2d( hkSimdRealParameter x, hkSimdRealParameter x0, hkSimdRealParameter x1, hkSimdRealParameter y0, hkSimdRealParameter y1 )
	{
		//HK_MATH_ASSERT(0x2342ab9,(x<=x0)||(x>=x1),"x is not from interval <x0,x1>!");
		HK_MATH_ASSERT(0x2342ab9,(x0 != x1), "no proper interval defined!");
		hkSimdReal dyOverDx; dyOverDx.setDiv<HK_ACC_FULL, HK_DIV_SET_ZERO>(y1-y0, x1-x0);
		return y0 + (x-x0)*dyOverDx;
	}

#define HKAI_MATH_WRAPPER1( FUNC ) HK_FORCE_INLINE static hkSimdReal HK_CALL FUNC( hkSimdRealParameter x){ return hkSimdReal::fromFloat( hkMath::FUNC(x.getReal()) ); }
#define HKAI_MATH_WRAPPER2( FUNC ) HK_FORCE_INLINE static hkSimdReal HK_CALL FUNC( hkSimdRealParameter x, hkSimdRealParameter y){ return hkSimdReal::fromFloat( hkMath::FUNC(x.getReal(), y.getReal()) ); }

	HKAI_MATH_WRAPPER1(acos);
	HKAI_MATH_WRAPPER1(sin);
	HKAI_MATH_WRAPPER1(cos);

	HKAI_MATH_WRAPPER2(atan2);
#undef HKAI_MATH_WRAPPER1
#undef HKAI_MATH_WRAPPER2

		/// Hash the xyz coordinates of the vector.
	inline hkUint64 spatialHash(hkVector4fParameter p);

		/// Hash the xyz coordinates of the vector.
	hkUint64 spatialHash(hkVector4dParameter p);

}


#include <Ai/Pathfinding/hkaiMath.inl>

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
