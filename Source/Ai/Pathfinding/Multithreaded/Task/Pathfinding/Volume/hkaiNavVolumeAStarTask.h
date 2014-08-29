/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_NAVVOLUME_ASTAR_TASK_H
#define HKAI_NAVVOLUME_ASTAR_TASK_H

#include <Ai/Pathfinding/Multithreaded/Task/Pathfinding/Volume/hkaiNavVolumePathfindingTask.h>

class hkaiNavVolumeAStarTask : public hkaiNavVolumePathfindingTask
{
public:
	HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI );

	hkaiNavVolumeAStarTask( const hkaiStreamingCollection& collection )
	:	hkaiNavVolumePathfindingTask( TASK_VOLUME_ASTAR, collection)
	{
	}

	void process() HK_OVERRIDE;
};


/// Search failed
void HK_CALL hkaiNavVolumeAStarTask_failure( const hkaiNavVolumeAStarCommand& command );

/// Search had line-of-sight to the goal
void HK_CALL hkaiNavVolumeAStarTask_lineOfSight( const hkaiNavVolumeAStarCommand& command,
											    hkVector4Parameter startPoint, hkaiPackedKey startCellKey, 
												hkVector4Parameter goalPoint, hkaiPackedKey goalCellKey );

#endif // HKAI_NAVVOLUME_ASTAR_TASK_H

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
