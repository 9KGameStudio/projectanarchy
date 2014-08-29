/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_TEST_ID_SELECTOR_H
#define HKB_TEST_ID_SELECTOR_H

#include <Behavior/Behavior/Utils/hkbCustomIdSelector.h>

	/// An hkbCustomIdSelector for testing purposes only.
class hkbTestIdSelector : public hkbCustomIdSelector
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbTestIdSelector() : m_int(0), m_real(0.0f) {}

		virtual bool canSelect( hkbNodeType nodeType ) HK_OVERRIDE
		{
			return
				// is a state machine
				( nodeType == HKB_NODE_TYPE_STATE_MACHINE ) ||
				// or is a transition effect
				( ( nodeType >= HKB_NODE_TYPE_FIRST_TRANSITION_EFFECT ) &&
				  ( nodeType < HKB_NODE_TYPE_FIRST_MODIFIER ) );
		}

			/// Returns the ID of the start state for the given state machine.
		virtual int selectCustomId( const hkbContext& context, const hkbNode& source, int /*propertyId*/ ) HK_OVERRIDE
		{
			HK_ASSERT( 0x22440219, ( source.getType() == HKB_NODE_TYPE_STATE_MACHINE ) || source.isTransitionEffect() );
			return 0;
		}

	public:

		int m_int;
		hkReal m_real;
		hkStringPtr m_string;

	public:

		hkbTestIdSelector( hkFinishLoadedObjectFlag flag ) : hkbCustomIdSelector(flag), m_string(flag) {}

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
