/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

inline hkaiNavMeshPathSearchParameters::hkaiNavMeshPathSearchParameters() 
:	m_costModifier( HK_NULL ),
	m_edgeFilter( HK_NULL ),
	m_validateInputs( true ),
	m_outputPathFlags( OUTPUT_PATH_SMOOTHED | OUTPUT_PATH_PROJECTED ),
	m_lineOfSightFlags( EARLY_OUT_IF_NO_COST_MODIFIER ),
	m_useHierarchicalHeuristic( false ),
	m_projectedRadiusCheck(true),
	m_userEdgeTraversalTestType( USER_EDGE_TRAVERSAL_TEST_DISABLED ),
	m_useGrandparentDistanceCalculation(true),
	m_heuristicWeight( 1.0f ),
	m_simpleRadiusThreshold( 0.01f ),
	m_maximumPathLength( HK_REAL_MAX ),
	m_searchSphereRadius( -1.0f ),
	m_searchCapsuleRadius( -1.0f )
{
	m_up.setZero();
}

inline hkaiNavMeshPathSearchParameters::hkaiNavMeshPathSearchParameters(hkFinishLoadedObjectFlag f)
:	m_bufferSizes(f),
	m_hierarchyBufferSizes(f)
{ }

inline hkBool32 hkaiNavMeshPathSearchParameters::operator ==( const hkaiNavMeshPathSearchParameters& o ) const
{
	// NOTE: m_up vector is not checked!
	return
		(	m_costModifier						== o.m_costModifier						&&
			m_edgeFilter						== o.m_edgeFilter						&&
			m_validateInputs					== o.m_validateInputs					&&
			m_outputPathFlags					== o.m_outputPathFlags					&&
			m_lineOfSightFlags					== o.m_lineOfSightFlags					&&
			m_useHierarchicalHeuristic			== o.m_useHierarchicalHeuristic			&&
			m_projectedRadiusCheck				== o.m_projectedRadiusCheck				&&
			m_heuristicWeight					== o.m_heuristicWeight					&&
			m_userEdgeTraversalTestType			== o.m_userEdgeTraversalTestType		&&
			m_useGrandparentDistanceCalculation == o.m_useGrandparentDistanceCalculation && 
			m_simpleRadiusThreshold				== o.m_simpleRadiusThreshold			&&
			m_maximumPathLength					== o.m_maximumPathLength				&&
			m_searchSphereRadius				== o.m_searchSphereRadius				&&
			m_searchCapsuleRadius				== o.m_searchCapsuleRadius				&&
			m_bufferSizes						== o.m_bufferSizes						&&
			m_hierarchyBufferSizes				== o.m_hierarchyBufferSizes
		);
}

inline hkBool32 hkaiNavMeshPathSearchParameters::shouldSmoothPath() const
{
	return m_outputPathFlags.anyIsSet( OUTPUT_PATH_SMOOTHED );
}

inline hkBool32 hkaiNavMeshPathSearchParameters::shouldProjectPath() const
{
	return m_outputPathFlags.anyIsSet( OUTPUT_PATH_PROJECTED );
}

inline hkBool32 hkaiNavMeshPathSearchParameters::shouldComputePathNormals() const
{
	return m_outputPathFlags.anyIsSet( OUTPUT_PATH_COMPUTE_NORMALS );
}

inline void hkaiNavMeshPathSearchParameters::setUp( hkVector4Parameter up )
{
	m_up = up;
}

hkBool32 hkaiNavMeshPathSearchParameters::shouldPerformLineOfSightCheck() const
{
	return m_lineOfSightFlags.anyIsSet(EARLY_OUT_IF_NO_COST_MODIFIER | EARLY_OUT_ALWAYS);
}
hkBool32 hkaiNavMeshPathSearchParameters::canEarlyOutFromLineOfSight() const
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
