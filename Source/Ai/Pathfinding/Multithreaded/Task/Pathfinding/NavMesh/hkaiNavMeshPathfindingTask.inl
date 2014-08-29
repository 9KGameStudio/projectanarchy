/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

HK_FORCE_INLINE hkaiNavMeshPathfindingTask::hkaiNavMeshPathfindingTask( TaskType tt, const hkaiStreamingCollection& collection)
:	m_maxTotalIterations(100000),
	m_commands(HK_NULL),
	m_numCommands(0),
	m_taskType(tt)
{
	m_streamingSectionInfo = collection.getInstanceInfoPtr();
}

HK_FORCE_INLINE void hkaiNavMeshPathfindingTask::set( const hkaiPathfindingUtil::FindPathInput& input)
{
	m_searchParameters	= input.m_searchParameters;
}

HK_FORCE_INLINE hkBool32 hkaiNavMeshPathfindingTask::isEquivalent( const hkaiPathfindingUtil::FindPathInput& input) const
{
	return( m_searchParameters == input.m_searchParameters );
}

inline void hkaiNavMeshAStarCommand::init()
{
	m_startPoint.setZero();
	m_goalPoint.setZero();
	m_startFaceKey = HKAI_INVALID_PACKED_KEY;
	m_goalFaceKey = HKAI_INVALID_PACKED_KEY;

	m_multipleGoalPoints = HK_NULL;
	m_multipleGoalFaceKeys = HK_NULL;
	m_numGoals = 0;

	m_maxNumberOfIterations = 100000;
	m_agentInfo.init();

	m_AStarOutput = HK_NULL;
	m_edgesOut = HK_NULL;
	m_maxEdgesOut = 0;
	m_pointsOut = HK_NULL; 
	m_maxPointsOut = 0;
}

inline void hkaiNavMeshAStarCommand::setStartPointAndFace( hkVector4Parameter startPoint, hkaiPackedKey startFaceKey )
{
	m_startPoint = startPoint;
	m_startFaceKey = startFaceKey;
}

inline void hkaiNavMeshAStarCommand::setGoalPointAndFace( hkVector4Parameter goalPoint, hkaiPackedKey goalFaceKey )
{
	// Copy the single goal by value. This scheme avoids an extra DMA in the single-goal case.
	m_goalPoint = goalPoint;
	m_goalFaceKey = goalFaceKey;
	m_multipleGoalPoints = HK_NULL;
	m_multipleGoalFaceKeys = HK_NULL;
	m_numGoals = 1;
}

inline void hkaiNavMeshAStarCommand::setMultipleGoalPointsAndFaces( const hkVector4* goalPoints, const hkaiPackedKey* goalFaceKeys, int numGoals )
{
	HK_ASSERT(0x346b517a, numGoals < hkaiNavMeshAStarCommand::MAX_GOALS);
	
	if ( numGoals == 1 )
	{
		setGoalPointAndFace( goalPoints[0], goalFaceKeys[0] );
	}
	else if ( numGoals > 1)
	{
		HK_CHECK_ALIGN16(goalPoints);	// Address must be aligned for DMA
		HK_CHECK_ALIGN16(goalFaceKeys);	// Address must be aligned for DMA

		// Copy the first goal by value. This scheme avoids an extra DMA in the single-goal case.
		m_goalPoint = goalPoints[0];
		m_goalFaceKey = goalFaceKeys[0];
		m_multipleGoalPoints = goalPoints;
		m_multipleGoalFaceKeys = goalFaceKeys;
		m_numGoals = numGoals;
	}
	else
	{
		m_multipleGoalPoints = HK_NULL;
		m_multipleGoalFaceKeys = HK_NULL;
		m_numGoals = 0;
	}
}


inline void hkaiNavMeshLineOfSightCommand::init()
{
	m_startPoint.setZero();
	m_endPoint.setZero();

	m_startFaceKey = HKAI_INVALID_PACKED_KEY;
	m_endFaceKey = HKAI_INVALID_PACKED_KEY;

	m_agentInfo.init();

	m_searchSphereRadius = -1.f;

	m_result = HK_NULL;
	m_edgesOut = HK_NULL;
	m_maxEdgesOut = 0;

	m_outputEdgesOnFailure = false;
	m_exactInternalVertexHandling = true;
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
