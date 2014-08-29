/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_OVERLAPPING_TRIANGLES_H
#define HKAI_OVERLAPPING_TRIANGLES_H

struct hkGeometry;
struct hkaiNavMeshGenerationSettings;

extern HK_EXPORT_AI const class hkClass hkaiOverlappingTrianglesSettingsClass;


	// Try to detect groups of overlapping coplanar triangles, and replace them with equivalent geometry
class  hkaiOverlappingTriangles
{
public:
	HK_DECLARE_REFLECTION();
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_NAVMESH, hkaiOverlappingTriangles);

	enum WalkableTriangleSettings
	{
		/// Only walkable triangles (as determined by hkaiNavMeshGenerationSettings::isWalkableBySlope) are 
		/// considered for retriangulation.
		ONLY_FIX_WALKABLE,

		/// Triangles that are walkable or that would be walkable when flipped are considered for retriangulation.
		/// Where a walkable triangle overlaps with an unwalkable one, the result will be walkable.
		/// This is the default behavior.
		PREFER_WALKABLE,

		/// Triangles that are walkable or that would be walkable when flipped are considered for retriangulation.
		/// Where a walkable triangle overlaps with an unwalkable one, the result will be unwalkable.
		PREFER_UNWALKABLE,
	};

	struct Settings
	{
		HK_DECLARE_REFLECTION();
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_NAVMESH, Settings);
		Settings();
		Settings(hkFinishLoadedObjectFlag flag) { }

			/// Triangles are only considered coplanar if their planes are within this tolerance (based on Manhattan distance)
		hkReal m_coplanarityTolerance; //+default(1e-3f)

		hkReal m_raycastLengthMultiplier; //+default(5.0f)

		hkEnum<WalkableTriangleSettings, hkUint8> m_walkableTriangleSettings; //+default(hkaiOverlappingTriangles::PREFER_WALKABLE)
	};

		/// Fixup the geometry using the specified coplanar settings. The coplanar settings from hkaiNavMeshGenerationSettings are unused.
	static hkResult HK_CALL fix( const Settings& coplanarSettings, const hkaiNavMeshGenerationSettings& genSettings, const hkGeometry& geomIn, hkGeometry& geomOut );
};

	/// Calls hkaiOverlappingTriangles::fix() with hkaiNavMeshGenerationSettings's coplanar settings.
hkResult HK_CALL hkaiFixOverlappingTriangles( const hkaiNavMeshGenerationSettings& genSettings, const hkGeometry& geomIn, hkGeometry& geomOut );

#endif // HKAI_OVERLAPPING_TRIANGLES_H

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
