/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_STATE_CHOOSER_H
#define HKB_STATE_CHOOSER_H

#include <Behavior/Behavior/StateMachine/hkbStateMachine.h>
#include <Behavior/Behavior/Utils/hkbCustomIdSelector.h>

	/// An interface to choose a state (typically the start state) of a state machine.
	/// This class is deprecated, please use hkbCustomIdSelector.
class hkbStateChooser : public hkReferencedObject
{
public:

	HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
	HK_DECLARE_REFLECTION();

	hkbStateChooser()
	{
		HK_WARN_ONCE( 0x22440207, "Using deprecated hkbStateChooser interface. Switch to hkbCustomIdSelector." );
	}

		/// Returns the ID of a state of the given state machine.
	virtual int chooseStateId( const hkbContext& context, const hkbStateMachine& stateMachine ) = 0;

public:

	hkbStateChooser( hkFinishLoadedObjectFlag flag ) : hkReferencedObject(flag) {}
};


	/// A wrapper for the deprecated interface.
	/// This will be removed in a future release.
class hkbStateChooserWrapper : public hkbCustomIdSelector
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

			// Wrap a custom chooser implementation.
			// The wrapper steals the reference for the object and cleans it up upon destruction.
			// This is used during versioning of deprecated hkbStateChooser objects.
		hkbStateChooserWrapper( hkbStateChooser& wrappedChooser ) : m_wrappedChooser(&wrappedChooser)
		{
			HK_WARN_ONCE( 0x22440207, "Using deprecated hkbStateChooser interface. Switch to hkbCustomIdSelector." );
		}

		~hkbStateChooserWrapper()
		{
			m_wrappedChooser->removeReference();
		}

			// hkbCustomIdSelector interface.
		virtual bool canSelect( hkbNodeType nodeType ) HK_OVERRIDE
		{
			// Using this old interface for something other than a state machine doesn't make sense
			HK_ASSERT( 0x22440208, (nodeType == HKB_NODE_TYPE_STATE_MACHINE) );

			// We can select for state machines
			return (nodeType == HKB_NODE_TYPE_STATE_MACHINE);
		}

			// hkbCustomIdSelector interface.
		virtual int selectCustomId( const hkbContext& context, const hkbNode& source, int /*propertyId*/ ) HK_OVERRIDE
		{
			return m_wrappedChooser->chooseStateId( context, static_cast<const hkbStateMachine&>(source) );
		}

	protected:

		hkbStateChooser* m_wrappedChooser;

	public:

		hkbStateChooserWrapper( hkFinishLoadedObjectFlag flag ) : hkbCustomIdSelector(flag)
		{
			if ( flag.m_finishing )
			{
				HK_WARN_ONCE( 0x22440233, "Deserializing a deprecated hkbStateChooser object. Switch to hkbCustomIdSelector." );
			}
		}
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
