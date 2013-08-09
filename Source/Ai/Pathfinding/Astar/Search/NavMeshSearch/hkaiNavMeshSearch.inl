/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

inline void hkaiNavMeshSearch::MemInfo::setEmpty( )
{
	m_openSetStorage = HK_NULL;
	m_openSetSize = 0;
	m_searchStateStorage = HK_NULL;
	m_searchStateSize = 0;
}

inline void hkaiNavMeshSearch::MemInfo::init( hkArray<char>& openSetArray, hkArray<char>& searchStateArray )
{
	m_openSetStorage = openSetArray.begin();
	m_openSetSize = openSetArray.getCapacity();
	m_searchStateStorage = searchStateArray.begin();
	m_searchStateSize = searchStateArray.getCapacity();
}

#ifndef HK_PLATFORM_SPU
inline void hkaiNavMeshSearch::MemInfo::init( hkArray<char>::Temp& openSetArray, hkArray<char>::Temp& searchStateArray )
{
	m_openSetStorage = openSetArray.begin();
	m_openSetSize = openSetArray.getCapacity();
	m_searchStateStorage = searchStateArray.begin();
	m_searchStateSize = searchStateArray.getCapacity();
}
#endif

inline const hkaiNavMesh::Face& hkaiNavMeshSearch::getFace( hkaiPackedKey faceKey )
{
	return m_graph->setCachedFace( faceKey );
}

inline const hkaiNavMesh::Edge& hkaiNavMeshSearch::getEdge( hkaiPackedKey edgekey )
{
	// This only gets used during initialization. We set the cached face on the graph to avoid asserts.
	return m_graph->setCachedOutgoingEdge(edgekey);
}

/*
 * Havok SDK - Base file, BUILD(#20130723)
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
