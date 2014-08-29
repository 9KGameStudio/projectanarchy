/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_AI_PATHFINDING_NAVVOLUME_PATH_SEARCH_PARAMETERS_H
#define HK_AI_PATHFINDING_NAVVOLUME_PATH_SEARCH_PARAMETERS_H

#include <Common/Base/Math/Vector/hkPackedVector3.h>
#include <Ai/Pathfinding/Astar/Search/hkaiSearchParams.h>

class hkaiAstarCostModifier;
class hkaiAstarEdgeFilter;


/// Parameters for controlling path searches on nav volumes.
/// This input data may apply to a batch of path requests.
struct hkaiNavVolumePathSearchParameters
{
	// +version(6)
	HK_DECLARE_REFLECTION();
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_ASTAR, hkaiNavVolumePathSearchParameters );

	hkaiNavVolumePathSearchParameters();
	hkaiNavVolumePathSearchParameters(hkFinishLoadedObjectFlag f);

	hkBool32 operator ==( const hkaiNavVolumePathSearchParameters& o ) const;

	inline hkBool32 shouldPerformLineOfSightCheck() const;
	inline hkBool32 canEarlyOutFromLineOfSight() const;

		/// Set the up vector for the search.
	inline void setUp( hkVector4Parameter up );

		/// World up vector.
	hkVector4 m_up; 

		/// Optional pointer to hkaiAstarCostModifier, which can be used to modify costs
		/// based on the hkaiAgentTraversalInfo.
	const hkaiAstarCostModifier* m_costModifier; //+nosave

		/// Optional pointer to hkaiAstarEdgeFilter, which can be used to reject nav mesh edges
		/// based on the hkaiAgentTraversalInfo
	const hkaiAstarEdgeFilter* m_edgeFilter; //+nosave

		/// How line of sight should be checked during the search.
	enum LineOfSightFlags 
	{
			/// Don't perform any line-of-sight check before A*
		NO_LINE_OF_SIGHT_CHECK = 0,
			
			/// Perform a line-of-sight check in order to try to avoid a full A* search.
			/// If the line-of-sight check between the start and end points succeeds and there is no cost modifier,
			/// this is returned as the shortest path.
			/// If the check succeeds and there is a cost modifier, the cost-modified path length is used as an upper
			/// limit for the A* search.
		EARLY_OUT_IF_NO_COST_MODIFIER = 1,

		

			/// Always do a line-of-sight check and early-out if it succeeds, regardless of whether or not a cost
			/// modifier is specified.
			/// In general, this flag should not be enabled when using a cost modifier to increase the cost of certain
			/// path edges, as it will find any straight-line path, even if a non-LOS path is actually preferred due to
			/// modified edge costs.
			/// This may be useful when checking for whether or not a path exists between two points, without actually
			/// needing to compute the path itself.
		EARLY_OUT_ALWAYS = 4
	};

	/// Controls whether a line-of-sight check is performed before A*, and how the check is configured.
	/// See LineOfSightFlags for more details.
	hkFlags<LineOfSightFlags, hkUint8> m_lineOfSightFlags; //+default(hkaiNavVolumePathSearchParameters::EARLY_OUT_IF_NO_COST_MODIFIER)

		/// Weight to apply to the heuristic function. Values greater than 1
		/// result in faster but less accurate searches (non-optimal paths may
		/// be returned). Defaults to 1.
	hkReal m_heuristicWeight; //+default(1.0f)

		/// Maximum path length allowed during A*. Any node beyond this distance will be ignored.
		/// Note that this distance is computed from the midpoint of the nav mesh edges and accounts for the cost modifier.
	hkReal m_maximumPathLength; //+default(HK_REAL_MAX)

	//
	// Memory control parameters
	// Reducing these numbers will reduce the size of memory allocations during the search,
	// but may cause the search to terminate early
	//

		/// Maximum memory size for the search.
	hkaiSearchParameters::BufferSizes m_bufferSizes;

	/// Internal determinism check
	void checkDeterminism() const;
};

#include <Ai/Pathfinding/NavVolume/hkaiNavVolumePathSearchParameters.inl>

#endif // HK_AI_PATHFINDING_NAVVOLUME_PATH_SEARCH_PARAMETERS_H

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
