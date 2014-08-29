/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_AI_NAV_VOLUME_SEARCH_H
#define HK_AI_NAV_VOLUME_SEARCH_H

#include <Ai/Pathfinding/Astar/hkaiAstarParameters.h>

// GRAPH and HEURISTIC - Volume specific
#include <Ai/Pathfinding/Astar/Heuristic/hkaiGraphDistanceHeuristic.h>
#include <Ai/Pathfinding/Graph/hkaiNavVolumeGraph.h>
#include <Ai/Pathfinding/NavVolume/hkaiNavVolume.h>

#include <Ai/Pathfinding/Astar/SearchState/hkaiHashSearchState.h>

#include <Ai/Pathfinding/Astar/OpenSet/hkaiHeapOpenSet.h>
struct hkaiSearchMemoryInfo;

	/// Utility for performing A* searches on a nav mesh.
	/// This is used by hkaiPathfindingUtility and the multithreaded tasks; it is recommended that you use those instead of
	/// using this directly.
struct hkaiNavVolumeSearch
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,hkaiNavVolumeSearch);
	typedef hkaiNavVolumeGraph Graph;

	//typedef hkaiDirectedGraphVisitor ClusterGraph;

	typedef hkaiGraphMultiDistanceHeuristic<hkaiNavVolumeGraph> Heuristic;
	typedef hkaiHashSearchState SearchState;

	typedef hkaiHeapOpenSet OpenSet;

	/// DMA groups used on the SPU for prefetching data
	enum DmaGroups
	{
		STATE_INFO_PREFETCH,
		CHARACTER_INFO_PREFETCH,
		GOAL_PREFETCH
	};

		/// Start point and goal information
	struct StartGoalInfo
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,hkaiNavVolumeSearch::StartGoalInfo);
		hkVector4 m_startPoint;
		const hkVector4* m_goalPoints;
		hkaiPackedKey m_startCellKey;
		const hkaiPackedKey* m_goalCellKeys;
		int m_numGoals;
	};
	

	hkaiNavVolumeSearch( const hkaiSearchMemoryInfo& memInfo );

		/// Initialize the search
	void init(Graph* graph, const hkaiAgentTraversalInfo& agentInfo, const StartGoalInfo& goalInfo );
	inline hkReal getCost(hkaiPackedKey nid)
	{
		m_state.nextNode( nid );
		return m_state.getCost(nid);
	}

protected:
		/// Set start and end cells.
	void setStartCellAndPosition( hkaiPackedKey  startCellIndex, hkVector4Parameter startPos, const struct hkaiAgentTraversalInfo& agentInfo );
	void setEndCell(  hkaiPackedKey endCellIndex );

public:
		/// Do one iteration of A*
	hkaiAstarOutputParameters::SearchStatus iteration();
		
		/// Pointer to the hkaiNavMeshGraph being searched
	hkPadSpu<Graph*> m_graph;

		/// A* search state
	SearchState m_state;

		/// A* open set
	OpenSet m_openset;

		/// A* Heuristic
	Heuristic m_heuristic;

		/// The most recent nav mesh edge that was processed by A*
	//hkaiNavVolume::EdgeIndex m_lastClosedEdge;

		/// The opposite cell of the last closed edge
	//hkaiNavVolume::CellIndex m_lastOppositeCell;

		/// The last closed node
	hkaiPackedKey m_lastClosedCell;

		/// Hierarchy graph (if applicable)
	//ClusterGraph m_spuClusterGraph;
};

#include <Ai/Pathfinding/Astar/Search/NavVolumeSearch/hkaiNavVolumeSearch.inl>

#endif // HK_AI_NAV_VOLUME_SEARCH_H

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
