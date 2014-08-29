/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_AVOIDANCE_SOLVER_TASK_H
#define HKAI_AVOIDANCE_SOLVER_TASK_H

#include <Ai/Pathfinding/Multithreaded/hkaiDefaultTask.h>
#include <Ai/Pathfinding/Character/LocalSteering/hkaiLocalSteeringInput.h>
#include <Ai/Pathfinding/hkaiBaseTypes.h>
#include <Ai/Pathfinding/Character/LocalSteering/hkaiAvoidanceSolver.h>


struct hkaiLocalSteeringOutput;
struct hkaiStreamingCollectionInstanceInfo;

/// Avoidance solving information for a single character.
struct hkaiAvoidanceSolverCommand
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI, hkaiAvoidanceSolverCommand );

	HK_FORCE_INLINE hkaiAvoidanceSolverCommand( ) {}

	hkaiLocalSteeringInput m_localSteeringInput;

	const class hkaiCharacter** m_nearbyCharacters;
	const class hkaiObstacleGenerator** m_obstacleGenerators;

	int m_numNearbyCharacters;
	int m_numObstacleGenerators;
	hkaiPackedKey m_currentNavMeshFaceKey;
	hkReal m_erosionRadius;

	hkaiLocalSteeringOutput* m_output;
};

class hkaiAvoidanceSolverTask : public hkaiDefaultTask
{
public:
	HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI );
	HK_FORCE_INLINE hkaiAvoidanceSolverTask()
	:	m_sectionInfo(HK_NULL),
		m_avoidancePairProps(HK_NULL),
		m_numAvoidancePairProps(0),
		m_commands(HK_NULL),
		m_numCommands(-1)
	{
	}

	void process() HK_OVERRIDE;

#if defined(HK_PLATFORM_HAS_SPU)
	virtual void* getSpuElf() HK_OVERRIDE;
#endif

	hkReal m_timestep;
	const hkaiStreamingCollectionInstanceInfo* m_sectionInfo;

	const hkaiAvoidancePairProperties::PairData*	m_avoidancePairProps;
	int												m_numAvoidancePairProps;

	hkaiAvoidanceSolverCommand* m_commands;
	int							m_numCommands;
};


#endif // HKAI_AVOIDANCE_SOLVER_TASK_H

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
