/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_DEFAULT_TASK_H
#define HKAI_DEFAULT_TASK_H

#include <Common/Base/Thread/Task/hkTask.h>

class hkaiDefaultTask : public hkTask
{
public:

	HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI );
	HK_FORCE_INLINE hkaiDefaultTask();
	
	enum SpuType
	{
		HKAI_TASK_SPU_TYPE_INVALID,
		HKAI_TASK_SPU_TYPE_ENABLED,
		HKAI_TASK_SPU_TYPE_DISABLED
	};

		/// Called at the end of the task. Currently just calls atomicIncrementFlag();
	void taskDone();

		/// Called at the end of the task.
		/// The m_taskDoneFlag will be incremented if it is non-NULL
	void atomicIncrementFlag() const;

	inline void setEnabledOnSpu();
	inline void setDisabledOnSpu();
	inline bool isEnabledOnSpu() const;

		/// The variable at this location will be incremented (atomically) when the task is done.
	hkUint32* m_taskDoneFlag;

	hkEnum<SpuType, hkUint8> m_taskSpuType;

};

#include <Ai/Pathfinding/Multithreaded/hkaiDefaultTask.inl>

#endif // HKAI_DEFAULT_TASK_H

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
