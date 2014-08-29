/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKAI_HIERARCHY_UTILS_H
#define HKAI_HIERARCHY_UTILS_H

#include <Ai/Pathfinding/hkaiBaseTypes.h>
#include <Ai/Pathfinding/Astar/Heuristic/hkaiGraphDistanceHeuristic.h>
#include <Ai/Pathfinding/NavMesh/hkaiNavMeshPathSearchParameters.h>
#include <Ai/Pathfinding/Graph/hkaiAgentTraversalInfo.h>

class hkaiDirectedGraphExplicitCost;
class hkaiDirectedGraphInstance;
class hkPseudoRandomGenerator;
class hkaiNavMesh;
class hkaiNavMeshInstance;
struct hkaiNavMeshFaceGraph;
class hkaiAstarCostModifier;
class hkaiStreamingCollection;
class hkaiAstarEdgeFilter;


	/// Utilities for constructing clusters of graph nodes (e.g., nav mesh faces).
	/// These clusters can the be used as a good heuristic for A* searches.
class HK_EXPORT_AI hkaiHierarchyUtils
{
public:
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI, hkaiHierarchyUtils);
	HK_DECLARE_REFLECTION();

		/// Cluster generation settings.
	struct HK_EXPORT_AI ClusterSettings
	{
		//+version(2)
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI, ClusterSettings);
		HK_DECLARE_REFLECTION();
		ClusterSettings();
		ClusterSettings( hkFinishLoadedObjectFlag f );

			/// The desired number of faces per cluster.
			/// This is approximate and will vary with the actual nav mesh.
		int m_desiredFacesPerCluster; //+default(20)

			/// Search parameters used when determining the final costs for graph edges.
			/// You can specifiy e.g. an hkaiAstarCostModifier to make the costs as close to possible to the cost of an
			/// actual nav mesh search.
			/// It is recommended that you set the global up vector with hkaiNavMeshPathSearchParameters::setUp() - if
			/// this is not done, the up vector for the search will be computed as the average of the face normals.
		hkaiNavMeshPathSearchParameters m_searchParameters;

			/// Agent information used when determining the final costs for graph edges.
			/// Setting the minimum agent size here will give more accurate costs in the graph.
		hkaiAgentTraversalInfo m_agentInfo;
	};

		/// Creates a cluster of the input nav mesh using the specified ClusterSettings.
	static void HK_CALL clusterNavMesh( hkaiNavMesh& mesh, hkaiDirectedGraphExplicitCost& graphOut, const ClusterSettings& settings);
	

	//
	// All other structs and methods are internal to the cluster builder.
	//

		/// Sortable pair of representative and group.
	struct HK_EXPORT_AI RepGroupPair
	{
		int m_representative;
		int m_group;

		HK_FORCE_INLINE hkBool32 operator<( const RepGroupPair& other ) const
		{
			return m_representative < other.m_representative;
		}
	};

		/// A 2d array which is sparse in one direction and direct in the other.
	struct HK_EXPORT_AI SemiSparse2dArraySorted
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI, SemiSparse2dArraySorted );
		int getSize() { return m_n; }

			/// Cost of travelling to edge t.
		struct HK_EXPORT_AI Cost
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI, Cost );
			int t;
			hkReal r;
		};

		typedef hkArray<Cost> Array;

		int indexOf(const Array& a, int y, int& greaterThanIndex) const; // returns index of the element with t == y
		int indexOf_binarySearch(const Array& a, int y, int& greaterThanIndex) const; // returns index of the element with t == y
		int indexOfGreaterThan(const Array& a, int y) const; // returns index of the first element with t > y
		void setSize(int n, hkReal r);
		hkReal get(int x, int y) const;
		void set(int x, int y, hkReal r);

		void findMinValFromSet( int x, const hkArrayBase<struct RepGroupPair>& groupReps, int& minIndex, hkReal& minCost ) const;
		
		void validate();
		void validateSub(int i);

		hkReal m_default;
		hkArray< Array > m_costs;

		int m_n;
	};

	typedef SemiSparse2dArraySorted SemiSparse2dArray;

		/// A simple structure for caching costs between nodes in a graph.
		/// This is used during graph clustering to get the costs between several nearest neighbors for a graph node.
	struct HK_EXPORT_AI CostAdaptor
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI, CostAdaptor );
		typedef hkReal PathCost;
		typedef hkaiPackedKey SearchIndex;

		typedef hkaiGraphMultiDistanceHeuristic<hkaiNavMeshFaceGraph>  Heuristic;
		CostAdaptor(SemiSparse2dArray& costs, int sourceNid);

		PathCost getCost( SearchIndex nid ) const;
		void setCost( Heuristic* h_unused, SearchIndex nid, PathCost d );
		bool isCostTooHigh( PathCost c) { return false; }

		PathCost estimatedCost( SearchIndex a ) const;
		PathCost estimatedCostCurrent( SearchIndex a ) const;

		SemiSparse2dArray& m_costs;
		int m_sourceNid;
	};
	
	static void HK_CALL buildAbstractGraphFromClusterInfo( hkaiNavMeshFaceGraph& graph, hkaiDirectedGraphExplicitCost& graphOut, hkArray<int>&, hkArray<int>&, const hkArray<hkVector4>&, const hkaiNavMeshPathSearchParameters& searchParams, const hkaiAgentTraversalInfo& agentInfo );
};

#endif // HKAI_HIERARCHY_UTILS_H

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
