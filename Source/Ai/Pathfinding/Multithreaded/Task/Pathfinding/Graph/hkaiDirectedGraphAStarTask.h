/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


#ifndef HKAI_DIRECTED_GRAPH_ASTAR_TASK_H
#define HKAI_DIRECTED_GRAPH_ASTAR_TASK_H

#include <Ai/Pathfinding/Multithreaded/hkaiDefaultTask.h>
#include <Ai/Pathfinding/Graph/hkaiDirectedGraphExplicitCost.h>
#include <Ai/Pathfinding/Graph/hkaiAgentTraversalInfo.h>
#include <Ai/Pathfinding/Graph/hkaiGraphPathSearchParameters.h>

struct hkaiAstarOutputParameters;
class hkaiStreamingCollection;
struct hkaiStreamingCollectionInstanceInfo;

	/// Command for performing an A* search on a directed graph.
	/// Each command corresponds to a single path; several commands can be performed in a single task.
struct hkaiDirectedGraphAStarCommand
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,hkaiDirectedGraphAStarCommand);

	/// Initializes data to sane values
	void init();


	/// Key of the starting node.
	hkaiPackedKey m_startNodeKey;

	/// Key of the goal node.
	hkaiPackedKey m_goalNodeKey;

	/// The maximum number of A* iterations that should be performed.
	/// This defaults to a very large value, but can be reduced to avoid spending too much time per frame.
	int m_maxNumberOfIterations;

	/// Width and filter information for a character.
	/// If an hkaiAstarCostModifier is specified, these can determine which edges are traversable,
	/// or change the cost of crossing certain faces
	hkaiAgentTraversalInfo m_agentInfo;

	//
	// Output
	//

	/// Optional output parameters (to query number of iterations, search status, etc.)
	hkaiAstarOutputParameters* m_AStarOutput;

	/// Main memory pointer where the search results are written out.
	/// The last node is followed by a -1 to indicate the end of the results
	hkaiPackedKey* m_nodesOut;

	/// Size of the output array.
	int m_maxNodesOut;
};

	/// A task that runs multiple A* searches on a single directed graph collection.
	/// The individual path requests are represented as an array of hkaiDirectedGraphAStarCommands.
	/// Note that this task is PPU only.
class hkaiDirectedGraphAStarTask : public hkaiDefaultTask
{
public:
	HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI );

	hkaiDirectedGraphAStarTask( const hkaiStreamingCollection& collection, const hkaiStreamingCollection* hierarchyCollection = HK_NULL);

	void process() HK_OVERRIDE;

	/// Search parameters
	hkaiGraphPathSearchParameters m_searchParameters;

	/// Pointer to streaming collection's information
	const hkaiStreamingCollectionInstanceInfo* m_streamingSectionInfo;

	/// Pointer to hierarchy collection's information
	const hkaiStreamingCollectionInstanceInfo* m_hierarchySectionInfo;

	/// Array of search commands to be processed
	hkaiDirectedGraphAStarCommand* m_commands;

	/// Number of search commands
	int m_numCommands;


};


//
// Output functions called by the task
//

/// Search failed
void HK_CALL hkaiDirectedGraphAStarTask_failure( const hkaiDirectedGraphAStarCommand& command );

#endif // HKAI_DIRECTED_GRAPH_ASTAR_TASK_H

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
