/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

template <typename T>
/*static*/ T hkbCustomIdSelector::computeId(
	hkbCustomIdSelector* customSelector,
	const hkbContext& context,
	const hkbNode& source,
	int propertyId,
	T currentIdx,
	int maxIdx )
{
	T effSelectionIndex = currentIdx;
	if ( customSelector != HK_NULL )
	{
		HK_ASSERT2( 0x22440217, customSelector->canSelect( source.getType() ), "hkbCustomIdSelector should be capable of selecting an index for this source type" );
		int userSelectedId = customSelector->selectCustomId( context, source );
		if ( userSelectedId >= 0 )
		{
			effSelectionIndex = T(userSelectedId);
		}
	}

	HK_ASSERT2( 0x22440218, ( int(effSelectionIndex) + 1 >= 1 ) && ( int(effSelectionIndex) <= maxIdx ), "The selection index is outside the bounds of valid indices" );

	return hkMath::clamp( effSelectionIndex, T(0), T(maxIdx) );
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
