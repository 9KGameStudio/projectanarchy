/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


#ifndef HKAI_GRAPH_CLUSTERING_TASK_H
#define HKAI_GRAPH_CLUSTERING_TASK_H

#include <Ai/Pathfinding/Multithreaded/hkaiDefaultTask.h>
#include <Ai/Pathfinding/Graph/hkaiDirectedGraphExplicitCost.h>

class hkaiGraphClusterUpdateTask : public hkaiDefaultTask
{
public:
	HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI );
	hkaiGraphClusterUpdateTask(const hkaiDirectedGraphExplicitCost* graph, const hkArrayBase<hkaiDirectedGraphExplicitCost::NodeIndex>& nodeIndices, hkArrayBase<int>& clusterIndices, const hkArrayBase<hkVector4>& centers, const class hkcdDynamicAabbTree* tree);

	void process() HK_OVERRIDE;

	
	const hkaiDirectedGraphExplicitCost* m_graph;
	const class hkcdDynamicAabbTree* m_tree;
	const hkArrayBase<hkaiDirectedGraphExplicitCost::NodeIndex>* m_nodeIndices;
	hkArrayBase<int>* m_clusterIndices;
	const hkArrayBase<hkVector4>* m_centers;

	int m_startIndex;
	int m_endIndex;
};


#endif // HKAI_GRAPH_CLUSTERING_TASK_H

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
