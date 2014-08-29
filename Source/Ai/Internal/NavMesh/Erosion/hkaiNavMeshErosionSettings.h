/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAINAVMESHEROSIONSETTINGS_H
#define HKAINAVMESHEROSIONSETTINGS_H

namespace hkaiNavMeshErosion
{
	struct ErosionSettings
	{
		ErosionSettings()
			: m_preWeldRadius(1e-4f)
		{ }

		/// The erosion radius. Must be greater than zero.
		hkReal m_radius;

		/// The maximum allowable radius error in the polygons generated at corners;
		/// the remaining area after erosion will never be closer to a corner than
		/// the given radius, but may be artificially far by up to this quantity.
		hkReal m_maxRadiusError;

		/// The radius within which to pre-weld vertices. This is only needed to
		/// prevent numerical problems, so it does not need to be changed unless
		/// nav mesh extents are greater than ~10e14.
		hkReal m_preWeldRadius; //+default(1e-4)

		/// The material to use to mark eroded areas, or 
		/// hkaiSilhouetteGenerator::CUTTING_MATERIAL to cut the eroded
		/// areas from the navmesh.
		int m_erodedMaterial;

		hkVector4 m_up;
	};

	namespace Util
	{
		/// Given the erosion radius and the max radius error, determine the inverse
		/// of the maximum wedge angle for corner extrusions.
		hkReal HK_FORCE_INLINE calcPhiRecip(
			hkReal erosionRadius,
			hkReal maxError)
		{
			return 1.0f/(2*hkMath::acos(erosionRadius/(erosionRadius+maxError)));
		}
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
