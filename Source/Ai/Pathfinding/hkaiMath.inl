/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

hkUint64 hkaiMath::spatialHash( hkVector4fParameter p )
{
	
	
	
	

	union
	{
		float floatVal[3];
		hkUint32 intVal[3];
	} u;

	u.floatVal[0] = p(0);
	u.floatVal[1] = p(1);
	u.floatVal[2] = p(2);
	hkUint64 x = u.intVal[0];
	hkUint64 y = u.intVal[1];
	hkUint64 z = u.intVal[2];
	const hkUint64 p1 = 73856093;
	const hkUint64 p2 = 19349663;
	const hkUint64 p3 = 83492791;

	return (x*p1) ^ (y*p2) ^ (z*p3);
}

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
