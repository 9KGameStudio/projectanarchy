/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


#ifndef HKAINAVMESHTRAVERSABILITY_H
#define HKAINAVMESHTRAVERSABILITY_H

#include <Ai/Pathfinding/hkaiBaseTypes.h>

class hkaiGeneralAccessor;

namespace hkaiNavMeshTraversability
{
	int HK_CALL calcNumFaceTraversabilitySlots(
		hkaiGeneralAccessor const& accessor,
		hkaiPackedKey faceKey);

	int HK_CALL calcEdgePairTraversabilityOffset(
		hkaiGeneralAccessor const& accessor,
		hkaiPackedKey faceKey, 
		hkaiPackedKey edgeKeyA, 
		hkaiPackedKey edgeKeyB);

	struct BoundaryEdge
	{
		hkVector4 m_a, m_b;
	};

	void findMaximumClearancePoint(
		hkVector4Parameter edgeStart,
		hkVector4Parameter edgeEnd,
		hkVector4Parameter up,
		hkArrayBase<BoundaryEdge> const& boundaryEdges,
		hkReal & maximumClearancePointOut,
		hkReal & maximumClearanceOut);

	// Internal utilities, exposed for testing
	namespace Util
	{
		struct Quadratic
		{
			hkReal x1, x2;
			hkReal a, h, c;

			HK_FORCE_INLINE hkReal evaluateAt(hkReal x) const { return (a*x + 2*h)*x + c; }
			HK_FORCE_INLINE hkReal evaluateDerivativeAt(hkReal x) const { return 2*(a*x+h); }
		};

		hkVector2 findMaximumAmongQuadratics(hkArrayBase<Quadratic> const& quadratics, hkReal xMin, hkReal xMax);

	}
}

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
