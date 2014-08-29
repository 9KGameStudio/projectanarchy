/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


#ifndef HK_AI_BEHAVIOR_CALC_VELOCITIES_TASK_H
#define HK_AI_BEHAVIOR_CALC_VELOCITIES_TASK_H

#include <Ai/Pathfinding/Multithreaded/hkaiDefaultTask.h>

struct hkaiLocalSteeringInput;

class hkaiBehaviorCalcVelocitiesTask : public hkaiDefaultTask
{
public:
	HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI );

	HK_FORCE_INLINE hkaiBehaviorCalcVelocitiesTask()
	: m_behaviors(HK_NULL)
	, m_numBehaviors(-1)
	, m_outputs(HK_NULL)
	, m_numOutputs(-1)
	, m_timestep(-1.0f)
	{
		setDisabledOnSpu();
	}

	void process() HK_OVERRIDE;
	
	/// Pointer to an array of hkaiBehavior pointers to be processed
	class hkaiBehavior** m_behaviors;
	int m_numBehaviors;

	/// Pointer to an array of local steering input structs to hold the outputs.
	/// This array should be sized to hold one output per character managed by the processed behaviors
	hkaiLocalSteeringInput* m_outputs;
	int m_numOutputs;

	hkReal m_timestep;
};

#endif // HK_AI_BEHAVIOR_CALC_VELOCITIES_TASK_H

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
