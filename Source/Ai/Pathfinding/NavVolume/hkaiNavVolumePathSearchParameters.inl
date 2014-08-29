/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

inline hkaiNavVolumePathSearchParameters::hkaiNavVolumePathSearchParameters() 
:	m_costModifier( HK_NULL ),
	m_edgeFilter( HK_NULL ),
	m_lineOfSightFlags( EARLY_OUT_IF_NO_COST_MODIFIER ),
	m_heuristicWeight( 1.0f ),
	m_maximumPathLength( HK_REAL_MAX )
{
	m_up.setZero();
}

inline hkaiNavVolumePathSearchParameters::hkaiNavVolumePathSearchParameters(hkFinishLoadedObjectFlag f)
: m_bufferSizes(f)
{ }

inline hkBool32 hkaiNavVolumePathSearchParameters::operator ==( const hkaiNavVolumePathSearchParameters& o ) const
{
	// NOTE: m_up vector is not checked!
	return
		(	m_costModifier						== o.m_costModifier						&&
			m_edgeFilter						== o.m_edgeFilter						&&
			m_lineOfSightFlags					== o.m_lineOfSightFlags					&&
			m_heuristicWeight					== o.m_heuristicWeight					&&
			m_bufferSizes						== o.m_bufferSizes						&&
			m_maximumPathLength					== o.m_maximumPathLength
		);
}


inline void hkaiNavVolumePathSearchParameters::setUp( hkVector4Parameter up )
{
	m_up = up;
}

hkBool32 hkaiNavVolumePathSearchParameters::shouldPerformLineOfSightCheck() const
{
	return m_lineOfSightFlags.anyIsSet(EARLY_OUT_IF_NO_COST_MODIFIER | EARLY_OUT_ALWAYS);
}

hkBool32 hkaiNavVolumePathSearchParameters::canEarlyOutFromLineOfSight() const
{
	return m_lineOfSightFlags.anyIsSet(EARLY_OUT_ALWAYS) ||
		( m_lineOfSightFlags.anyIsSet(EARLY_OUT_IF_NO_COST_MODIFIER) && !m_costModifier);
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
