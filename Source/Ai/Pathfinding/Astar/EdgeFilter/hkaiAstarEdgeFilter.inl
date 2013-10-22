/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


inline hkaiAstarEdgeFilter::NavMeshIsEnabledCallbackContext::NavMeshIsEnabledCallbackContext(
	const hkaiNavMeshAccessor* meshAccessor, const hkaiAgentTraversalInfo& agentInfo, const hkaiNavMeshEdgePairInfo& edgeFaceInfo, hkUint8 queryTypeFlags )
:	m_meshAccessor(meshAccessor),
	m_agentInfo(agentInfo),
	m_edgeFaceInfo(edgeFaceInfo),
	m_queryTypeFlags(queryTypeFlags)
{

}

inline hkaiAstarEdgeFilter::NavVolumeIsEnabledCallbackContext::NavVolumeIsEnabledCallbackContext(
	const hkaiNavVolumeAccessor* volumeAccessor, const hkaiAgentTraversalInfo& agentInfo, const hkaiNavVolumeCellPairInfo& cellEdgeInfo, hkUint8 queryTypeFlags )
	:	m_volumeAccessor(volumeAccessor),
	m_agentInfo(agentInfo),
	m_cellEdgeInfo(cellEdgeInfo),
	m_queryTypeFlags(queryTypeFlags)
{

}

inline hkaiAstarEdgeFilter::DirectedGraphIsEnabledCallbackContext::DirectedGraphIsEnabledCallbackContext(
	const hkaiStreamingCollection::InstanceInfo* streamingInfo, const hkaiAgentTraversalInfo& agentInfo, 
	const hkaiDirectedGraphNodePairInfo& nodeEdgeInfo)
	: m_streamingInfo(streamingInfo),
	m_agentInfo(agentInfo),
	m_nodeEdgeInfo(nodeEdgeInfo)
{
}

/*
 * Havok SDK - Base file, BUILD(#20131019)
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2013
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
