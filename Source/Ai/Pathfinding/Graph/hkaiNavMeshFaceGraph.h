/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKAI_NAV_MESH_FACE_GRAPH_H
#define HKAI_NAV_MESH_FACE_GRAPH_H

#include <Ai/Pathfinding/NavMesh/hkaiNavMesh.h>
#include <Ai/Pathfinding/NavMesh/hkaiNavMeshTraversalUtils.h>
#include <Ai/Pathfinding/Graph/hkaiAgentTraversalInfo.h>
#include <Ai/Pathfinding/Astar/CostModifier/hkaiAstarCostModifier.h>
#include <Ai/Pathfinding/Astar/EdgeFilter/hkaiAstarEdgeFilter.h>
#include <Ai/Pathfinding/NavMesh/hkaiNavMeshUtils.h>

#include <Ai/Pathfinding/Astar/SearchState/hkaiHashSearchState.h>
#include <Ai/Pathfinding/Astar/Heuristic/hkaiGraphDistanceHeuristic.h>
#include <Common/Base/Monitor/hkMonitorStream.h>


/// A simple graph on a nav mesh, that treats the faces as nodes in the graph.
/// This isn't used for traversal, but is used during cluster generation for hierarchical pathfinding.
struct hkaiNavMeshFaceGraph
{
public:
HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,hkaiNavMeshFaceGraph);
	typedef hkaiPackedKey SearchIndex;
	typedef hkaiPackedKey EdgeKey;
	typedef hkReal EdgeCost;
	typedef hkVector4 Position;

	typedef hkaiGraphMultiDistanceHeuristic<hkaiNavMeshFaceGraph> Heuristic;
	typedef hkaiHashSearchState SearchState;

	HK_FORCE_INLINE hkaiNavMeshFaceGraph();
	inline void setCollection( const hkaiStreamingCollection* collection );
	const hkaiStreamingCollection* getCollection() const { return m_collection; }

	HK_FORCE_INLINE int getNumNodes() const;


	//
	// Begin hkaiAstar interface
	//

	HK_FORCE_INLINE hkBool32 isGoal( SearchIndex nit ) const;
	HK_FORCE_INLINE int getMaxNeighborCount( SearchIndex nit ) const;
	HK_FORCE_INLINE void getNeighbors( SearchIndex nit, hkArrayBase< EdgeKey >& neighbors ) const;
	HK_FORCE_INLINE SearchIndex edgeTarget( SearchIndex nit, EdgeKey eit ) const;

	HK_FORCE_INLINE EdgeCost getTotalCost(SearchIndex nit, SearchIndex adj, EdgeKey eit, const EdgeCost costToParent) const;

	HK_FORCE_INLINE hkBool32 isEdgeTraversable(SearchIndex nit, SearchIndex adj, EdgeKey eit);

	HK_FORCE_INLINE hkBool32 isValidEdgeTarget( SearchIndex eit ) const;

	HK_FORCE_INLINE void getPosition( SearchIndex a, hkVector4& vecOut) const;
	HK_FORCE_INLINE void getPositionForHeuristic( SearchIndex a, hkVector4& vecOut) const;

	HK_FORCE_INLINE hkSimdReal distance(SearchIndex A, SearchIndex B) const;

	HK_FORCE_INLINE hkSimdReal distanceFromPosition(hkVector4Parameter posA, hkVector4Parameter posB) const;

	HK_FORCE_INLINE void getFaceCentroid( hkaiNavMesh::FaceIndex faceIndex, hkVector4& vecOut) const;
	HK_FORCE_INLINE void getFaceNormal( hkaiNavMesh::FaceIndex faceIndex, hkVector4& vecOut) const;

	//
	// Begin hkaiAster Listener interface
	//
	HK_FORCE_INLINE void nextNode(SearchIndex nid);
	HK_FORCE_INLINE void nextEdge(SearchIndex nid, EdgeKey ekey);
	HK_FORCE_INLINE void edgeAccepted(SearchIndex nid, EdgeKey ekey)  {}
	HK_FORCE_INLINE void setParent(SearchIndex cur, SearchIndex newParent, EdgeKey newEdge) { }
	HK_FORCE_INLINE void keepParent(SearchIndex cur, SearchIndex parent, EdgeKey edge) { }
	HK_FORCE_INLINE void nodeOpened(SearchIndex nid) {}
	HK_FORCE_INLINE void openNodeAdjusted(SearchIndex nid) {}
	HK_FORCE_INLINE void nodeClosed(SearchIndex) {}

protected:
		/// Nav mesh to be searched
	const hkaiStreamingCollection* m_collection;
	const hkaiNavMesh* m_navMesh;

public:

		/// Traversal info for the current search
	hkaiAgentTraversalInfo m_info;

		/// Cost modifier and edge filter for the graph.
		/// hkaiAstarCostModifier was designed for use with the usual hkaiNavMeshGraph; as such, the callbacks consider 2 edges and the
		/// face that is "between" them. hkaiNavMeshFaceGraph only has the "outgoing" edge at a given time, though, so callbacks using the
		/// incoming edge might not work.
	const hkaiAstarCostModifier* m_costModifier;
	const hkaiAstarEdgeFilter* m_edgeFilter;

	SearchState* m_searchState;

	// Leashing
	hkVector4 m_startPoint;
	const hkVector4* m_endPoints;
	int m_numGoals;
	hkSimdReal m_searchSphereRadius;
	hkSimdReal m_searchCapsuleRadius;

	// Cached info
	hkArray< hkVector4 > m_cachedCentroids;
	hkArray< hkVector4 > m_cachedNormals;
};

#include <Ai/Pathfinding/Graph/hkaiNavMeshFaceGraph.inl>
#endif // HKAI_NAV_MESH_FACE_GRAPH_H

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
