/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_AI_VOLUME_PATHFINDING_TASK_H
#define HK_AI_VOLUME_PATHFINDING_TASK_H


#include <Ai/Pathfinding/Astar/CostModifier/hkaiDefaultAstarCostModifier.h>
#include <Ai/Pathfinding/NavVolume/hkaiNavVolumePathSearchParameters.h>
#include <Ai/Pathfinding/Graph/hkaiAgentTraversalInfo.h>
#include <Ai/Pathfinding/Utilities/hkaiVolumePathfindingUtil.h>
#include <Ai/Pathfinding/Multithreaded/hkaiDefaultTask.h>

struct hkaiAstarOutputParameters;

/// A task that runs multiple A* searches on a single nav volume.
/// The searches are represented as an array of hkaiNavVolumeAStarCommands.
struct hkaiNavVolumePathfindingTask : public hkaiDefaultTask
{
	HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI );

	enum TaskType
	{
		TASK_VOLUME_ASTAR,
		TASK_VOLUME_LINE_OF_SIGHT,
	};

	HK_FORCE_INLINE hkaiNavVolumePathfindingTask( TaskType tt, const hkaiStreamingCollection& collection );

	/// Initializes the relevant fields in the task from the pathfinding input
	HK_FORCE_INLINE void set( const hkaiVolumePathfindingUtil::FindPathInput& input);

	/// Returns true if all the relevant fields in the task equal to the pathfinding input
	HK_FORCE_INLINE hkBool32 isEquivalent( const hkaiVolumePathfindingUtil::FindPathInput& input) const;

#if defined(HK_PLATFORM_HAS_SPU)
	virtual void* getSpuElf() HK_OVERRIDE;
#endif

	/// Search parameters
	hkaiNavVolumePathSearchParameters m_searchParameters;

	/// The maximum number of search iterations that should be performed over the whole set of search commands.
	/// This defaults to a very large value, but can be reduced to avoid spending too much time per frame.
	int m_maxTotalIterations;

	/// Pointer to streaming collection's information
	const hkaiStreamingCollection::InstanceInfo* m_streamingSectionInfo;

	/// Array of hkaiNavVolumeAStarCommand or hkaiNavVolumeLineOfSightCommand commands to be processed
	void* m_commands;

	/// Number of search commands
	int m_numCommands;

	hkEnum<TaskType, hkUint8> m_taskType;
};


/// Command for performing an A* search on a nav volume.
/// Each command corresponds to a single path; several commands can be performed in a single task.
struct hkaiNavVolumeAStarCommand
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,hkaiNavVolumeAStarCommand);
	enum
	{
		/// Maximum number of goals allowed
		MAX_GOALS = 16
	};

	/// Initializes data to sane values
	HK_FORCE_INLINE void init();

	/// Set the start point and start cell for the search
	HK_FORCE_INLINE void setStartPointAndCell( hkVector4Parameter startPoint, hkaiPackedKey startCellKey );

	/// Set a single goal for the search.
	/// To search with multiple goals, use setMultipleGoalPointsAndCells
	HK_FORCE_INLINE void setGoalPointAndCell( hkVector4Parameter goalPoint, hkaiPackedKey goalCellKey );

	/// Set multiple goals for the search.
	/// To search with a single goal, use setGoalPointAndCell
	/// \param goalPoints Array of goal points. This memory must remain valid until the command is processed.
	/// \param goalCells Array of goal cell keys. This memory must remain valid until the command is processed.
	/// \param numGoal Number of elements in both the goalPoints and goalCells arrays
	HK_FORCE_INLINE void setMultipleGoalPointsAndCells( const hkVector4* goalPoints, const hkaiPackedKey* goalCellKeys, int numGoals );


	/// Start point
	hkVector4 m_startPoint;

	/// Goal point.
	/// If there are multiple goals, the goal points are stored in m_multipleGoalPoints
	hkVector4 m_goalPoint;

	/// Key of the start cell
	hkaiPackedKey m_startCellKey;

	/// Key of the goal cell.
	/// If there are multiple goals, the goal cells are stored in m_multipleGoalCellKeys
	hkaiPackedKey m_goalCellKey;

	/// Array of goal points, for multiple-goal queries.
	/// Size of the array is given by m_numGoals.
	const hkVector4* m_multipleGoalPoints;

	/// Array of goal cells, for multiple-goal queries.
	/// Size of the array is given by m_numGoals.
	const hkaiPackedKey* m_multipleGoalCellKeys;

	/// Total number of goals, equal to 1 for single-goal queries, greater than 1 for multiple-goal queries.
	/// For multiple-goal queries, this is the size of both the m_multipleGoalPoints and m_multipleGoalCellKeys arrays.
	int m_numGoals;

	/// The maximum number of A* iterations that should be performed.
	/// This defaults to a very large value, but can be reduced to avoid spending too much time per frame.
	int m_maxNumberOfIterations;

	/// Traversal information to control edge filtering, cost modification, etc.
	hkaiAgentTraversalInfo m_agentInfo;

	//
	// Output
	//

	/// Optional output parameters (to query number of iterations, search status, etc.)
	hkaiAstarOutputParameters* m_AStarOutput;

	/// List of cells visited along the final path. The last edge in the list is followed by HKAI_INVALID_PACKED_KEY.
	hkaiPackedKey* m_cellsOut;

	/// Maximum length of the m_cellsOut array. Note that limiting the size of the output will not early-out of the search.
	int m_maxCellsOut;

	/// Results of path smoothing. The last point in the list is followed by a terminator point.
	/// This final point can be identified with hkaiPath::PathPoint::isTerminator()
	hkaiPath::PathPoint* m_pointsOut; 

	/// Maximum length of the smoothed path. Note that limiting the size of the output will not early-out of the search.
	int  m_maxPointsOut;

};



/// Command for performing a line of sight check on a nav volume.
/// Each command corresponds to a single check; several commands can be performed in a single task.
struct hkaiNavVolumeLineOfSightCommand
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,hkaiNavVolumeLineOfSightCommand);
	/// Initializes data to sane values
	HK_FORCE_INLINE void init();

	/// Start point.
	hkVector4 m_startPoint;

	/// Goal point.
	hkVector4 m_endPoint;

	/// Key of the starting cell. m_startPoint should be contained in this cell.
	hkaiPackedKey m_startCellKey;

	/// Key of the goal cell. m_endPoint should be contained in this cell.
	hkaiPackedKey m_endCellKey;

	/// Traversal information to control edge filtering, cost modification, etc.
	hkaiAgentTraversalInfo m_agentInfo;

	/// Edges will be rejected if they do not intersect a sphere of this radius, centered on the start point.
	/// If this value is negative, the check will be skipped
//	hkReal m_searchSphereRadius;

	/// Edges will be rejected if they do not intersect a capsule of this radius, whose axis extends from the start point to the end point.
	/// If this value is negative, the check will be skipped
//	hkReal m_searchCapsuleRadius;

	//
	// Output
	//

	/// Main memory pointer where the result of the line of sight check is written out
	hkBool* m_result;

	/// List of cell visited along the final path. The last cell in the list is followed by a HKAI_INVALID_PACKED_KEY.
	hkaiPackedKey* m_cellsOut;

	/// Maximum length of the m_cellsOut array.
	int m_maxCellsOut;


};



#include <Ai/Pathfinding/Multithreaded/Task/Pathfinding/Volume/hkaiNavVolumePathfindingTask.inl>

#endif // HK_AI_VOLUME_PATHFINDING_TASK_H

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
