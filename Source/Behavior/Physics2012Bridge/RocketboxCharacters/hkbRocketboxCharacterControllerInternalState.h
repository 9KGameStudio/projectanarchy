/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// WARNING: THIS FILE IS GENERATED. EDITS WILL BE LOST.
// Generated from 'Behavior/Physics2012Bridge/RocketboxCharacters/hkbRocketboxCharacterController.h'

#ifndef HKB_ROCKETBOX_CHARACTER_CONTROLLER_INTERNAL_STATE_H
#define HKB_ROCKETBOX_CHARACTER_CONTROLLER_INTERNAL_STATE_H

#include <Behavior/Physics2012Bridge/RocketboxCharacters/hkbRocketboxCharacterController.h>

class hkbRocketboxCharacterControllerInternalState : public hkReferencedObject
{
	//+vtable(1)
	//+version(0)
	public:

		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );

		hkbRocketboxCharacterControllerInternalState() {}
		hkbRocketboxCharacterControllerInternalState( hkFinishLoadedObjectFlag flag )
			: hkReferencedObject(flag)
		{
		}

		bool				 m_rapidTurnRequest;
		int					 m_currPose;
		int					 m_prevPose;
		hkReal				 m_noVelocityTimer;
		hkReal				 m_linearSpeedModifier;
		hkReal				 m_characterAngle;
		int					 m_plantedFootIdx;
		hkReal				 m_timeStep;
};

#endif

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
