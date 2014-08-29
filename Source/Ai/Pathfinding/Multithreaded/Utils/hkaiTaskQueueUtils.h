/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_TASK_QUEUE_UTILS
#define HKAI_TASK_QUEUE_UTILS

#include <Common/Base/Thread/Task/hkTaskGraph.h>

class hkTaskQueue;
class hkThreadPool;


	/// Helper class for running tasks
class hkaiTaskQueueUtils
{
public:

		/// Add the tasks to the queue, start running, and wait until completion.
	template <typename TaskType>
	static void HK_CALL runAllTasks( hkArrayBase<TaskType>& tasks, hkTaskQueue* taskQueue, hkThreadPool* threadPool );

		/// Add the tasks to the queue, start running, and wait until completion.
	template <typename TaskTypeA, typename TaskTypeB>
	static void HK_CALL runAllTasks( hkArrayBase<TaskTypeA>& tasksA, hkArrayBase<TaskTypeB>& tasksB, hkTaskQueue* taskQueue, hkThreadPool* threadPool );

		/// Add one task to the queue, start running, and wait until completion.
		/// Not very efficient, mainly for testing and demos.
	template <typename TaskType>
	static void HK_CALL runSingleTask( TaskType& task, hkTaskQueue* taskQueue, hkThreadPool* threadPool );

		/// Run all the tasks. The dependency array is initially specified by the order in the taskPtrs array.
	static void HK_CALL runTasksWithDependencies( hkArrayBase<hkTask*>& taskPtrs, const hkArrayBase<hkTaskGraph::Dependency>& dependencies, hkTaskQueue* taskQueue, hkThreadPool* threadPool );

protected:
	static void HK_CALL processGraph( hkTaskGraph* graph, hkTaskQueue* taskQueue, hkThreadPool* threadPool );
};

#include <Ai/Pathfinding/Multithreaded/Utils/hkaiTaskQueueUtils.inl>


#endif // HKAI_TASK_QUEUE_UTILS

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
