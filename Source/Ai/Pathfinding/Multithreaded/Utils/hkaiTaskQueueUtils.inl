/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Common/Base/Thread/Task/hkTask.h>
#include <Common/Base/Thread/Task/hkTaskGraph.h>
#include <Common/Base/Thread/Task/hkTaskQueue.h>
#include <Common/Base/Thread/Pool/hkThreadPool.h>

template <typename TaskType>
inline void HK_CALL hkaiTaskQueueUtils::runAllTasks( hkArrayBase<TaskType>& tasks, hkTaskQueue* taskQueue, hkThreadPool* threadPool )
{
	hkTaskGraph graph;
	graph.reserve( tasks.getSize(), 0 );
	for (int i=0; i<tasks.getSize(); i++)
	{
		graph.addTask( &tasks[i] );
	}

	processGraph(&graph, taskQueue, threadPool);
}


template <typename TaskTypeA, typename TaskTypeB>
void HK_CALL hkaiTaskQueueUtils::runAllTasks( hkArrayBase<TaskTypeA>& tasksA, hkArrayBase<TaskTypeB>& tasksB, hkTaskQueue* taskQueue, hkThreadPool* threadPool )
{
	hkTaskGraph graph;
	graph.reserve( tasksA.getSize() + tasksB.getSize(), 0 );
	for (int i=0; i<tasksA.getSize(); i++)
	{
		graph.addTask( &tasksA[i] );
	}

	for (int i=0; i<tasksB.getSize(); i++)
	{
		graph.addTask( &tasksB[i] );
	}

	processGraph(&graph, taskQueue, threadPool);
}


template <typename TaskType>
void HK_CALL hkaiTaskQueueUtils::runSingleTask( TaskType& task, hkTaskQueue* taskQueue, hkThreadPool* threadPool )
{
	hkArray< TaskType > tasks;
	tasks.pushBack(task);
	runAllTasks(tasks, taskQueue, threadPool);
}

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
