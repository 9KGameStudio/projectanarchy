/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_CUSTOM_ID_SELECTOR_H
#define HKB_CUSTOM_ID_SELECTOR_H

#include <Behavior/Behavior/Node/hkbNode.h>

class hkbContext;

	/// An interface to choose a state (typically the start state) of a state machine.
	/// This class is deprecated, please use hkbCustomIdSelector.
class hkbCustomIdSelector : public hkReferencedObject
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbCustomIdSelector() {}

		
			/// Return if the custom id selector can select for a particular node type.
		virtual bool canSelect( hkbNodeType nodeType ) = 0;

			/// Returns the custom ID chosen by the selector.
			/// A value of -1 allows the source to choose.
			/// propertyId may be invalid (-1). Refer to the source you are trying to select for.
		virtual int selectCustomId( const hkbContext& context, const hkbNode& source, int propertyId = -1 ) = 0;
		

			/// Util which simply computes the proper selection index when using a custom selector.
			/// The custom selector may be null.
		template<typename T>
		static T computeId(
			hkbCustomIdSelector* customSelector,
			const hkbContext& context,
			const hkbNode& source,
			int propertyId,
			T currentIdx,
			int maxIdx );

	public:

		hkbCustomIdSelector( hkFinishLoadedObjectFlag flag ) : hkReferencedObject(flag) {}
};

#include <Behavior/Behavior/Utils/hkbCustomIdSelector.inl>

#endif // HKB_CUSTOM_ID_SELECTOR_H

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
