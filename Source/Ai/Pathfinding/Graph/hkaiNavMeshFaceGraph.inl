/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Geometry/Internal/Algorithms/Distance/hkcdDistancePointLine.h>
#include <Geometry/Internal/Algorithms/Distance/hkcdDistanceSegmentSegment.h>

hkaiNavMeshFaceGraph::hkaiNavMeshFaceGraph()
:	m_collection(HK_NULL),
	m_costModifier(HK_NULL),
	m_edgeFilter(HK_NULL),
	m_searchState(HK_NULL),
	m_searchSphereRadius(-hkSimdReal_1),
	m_searchCapsuleRadius(-hkSimdReal_1)
{
}

void hkaiNavMeshFaceGraph::setCollection( const hkaiStreamingCollection* collection )
{
	HK_ASSERT(0x73370915, collection->getNumInstances() == 1);

	HK_TIME_CODE_BLOCK("setCollection", HK_NULL);
	m_collection = collection;

	const hkaiNavMesh* mesh = collection->getInstanceAt(0)->getOriginalMesh();
	const int numFaces = mesh->getNumFaces();
	m_cachedCentroids.setSize( numFaces );
	m_cachedNormals.setSize( numFaces );

	for (int f=0; f<numFaces; f++)
	{
		hkaiNavMeshUtils::calcFaceCentroid( *mesh, f, m_cachedCentroids[f] );
		hkaiNavMeshUtils::calcFaceNormal( *mesh, f, m_cachedNormals[f] );
	}

	m_navMesh = mesh;
}


inline hkBool32 hkaiNavMeshFaceGraph::isGoal( hkaiPackedKey nit ) const
{
	
	return false;
}

int hkaiNavMeshFaceGraph::getNumNodes() const
{
	
	return m_navMesh->getNumFaces();
}

// Get an upper bound on the number of nodes adjacent to a given node.
int hkaiNavMeshFaceGraph::getMaxNeighborCount( hkaiPackedKey nit ) const
{
	hkaiNavMesh::FaceIndex faceIndex = hkaiGetIndexFromPacked( nit );
	const hkaiNavMesh::Face& face = m_navMesh->getFace(faceIndex);
	const int numRealEdges = (face.m_numEdges);
	return numRealEdges;
}

void hkaiNavMeshFaceGraph::getNeighbors( hkaiPackedKey nit, hkArrayBase< hkaiPackedKey >& neighbors ) const
{
	hkaiNavMesh::FaceIndex faceIndex = hkaiGetIndexFromPacked( nit );
	const hkaiNavMesh::Face& face = m_navMesh->getFace(faceIndex);
	const int numRealEdges = (face.m_numEdges);

	HK_ASSERT(0x66c7197b, neighbors.isEmpty());
	HK_ASSERT2(0x79b7d4c0, neighbors.getCapacity() >= numRealEdges, 
		"For performance, neighbors array should be pre-allocated to contain all possible neighbors.");
	neighbors.setSizeUnchecked(numRealEdges);
	hkaiPackedKey* HK_RESTRICT neighborPtr = neighbors.begin();

	const hkaiRuntimeIndex sectionId = hkaiGetRuntimeIdFromPacked(nit);
	for ( int e=0; e < numRealEdges; e++ )
	{
		const hkaiPackedKey edgeKey = hkaiGetPackedKey( sectionId, face.m_startEdgeIndex + e);
		*neighborPtr++ = edgeKey;
	}
}

hkaiPackedKey hkaiNavMeshFaceGraph::edgeTarget( hkaiPackedKey nit, hkaiPackedKey edgeKey ) const
{
	hkaiNavMesh::EdgeIndex edgeIndex = hkaiGetIndexFromPacked(edgeKey);
	return m_navMesh->getEdge(edgeIndex).m_oppositeFace;
}

hkaiNavMeshFaceGraph::EdgeCost hkaiNavMeshFaceGraph::getTotalCost( hkaiPackedKey nit, hkaiPackedKey adj, hkaiPackedKey eit, const EdgeCost costToParent ) const
{
	Position centroidA; getPosition(nit, centroidA);
	Position centroidB; getPosition(adj, centroidB);

	// The setup looks something like this:
	//		 centroidB
	//			 ^
	//			 |
	//	B < -----+------- A  edge
	//			 |
	//		   <-| plane
	//			 |
	//		 centroidA
	//

	// We could just take the distance between the face centroids
	// But if the segment between the segment doesn't pass though the edge, we should split the segment into 2 parts:
	//	centroidA->edge  + edge->centroidB

	HK_ASSERT(0x6886c84f, hkaiGetRuntimeIdFromPacked(nit) == hkaiGetRuntimeIdFromPacked(eit) );
	const hkaiNavMesh::EdgeIndex edgeIndex = hkaiGetIndexFromPacked(eit);
	const hkaiNavMesh::FaceIndex faceIndex = hkaiGetIndexFromPacked(nit);
	
	hkVector4 edgeA, edgeB;
	m_navMesh->getEdgePoints( edgeIndex, edgeA, edgeB );

	hkVector4 faceNormal = m_cachedNormals[faceIndex];

	hkVector4 dir; dir.setSub(centroidB, centroidA);
	hkVector4 plane; plane.setCross(faceNormal, dir);
	// Don't need to normalize the plane since we're comparing distances against 0
	//plane.normalize<3>();
	plane.setPlaneConstant(centroidA);

	hkSimdReal distanceA = plane.dot4xyz1(edgeA);
	hkSimdReal distanceB = plane.dot4xyz1(edgeB);

	// If the segment passes through the edge, then A is on the negative side of the plane, and B is on the positive
	hkSimdReal euclideanDistance;
	hkVector4 pointOnEdge;
	if (distanceA.isGreaterZero() || distanceB.isLessZero() )
	{
		pointOnEdge.setSelect( distanceA.greaterZero(), edgeA, edgeB );
		euclideanDistance = distanceFromPosition(centroidA, pointOnEdge) +  distanceFromPosition(pointOnEdge, centroidB);
	}
	else
	{
		euclideanDistance = dir.length<3>();
		pointOnEdge.setInterpolate( edgeA, edgeB, hkSimdReal_Half);
	}

	if (!m_costModifier)
	{
		return costToParent + euclideanDistance.getReal();
	}
	else
	{
		const hkaiNavMeshInstance* meshInstance = m_collection->getInstanceAt( hkaiGetRuntimeIdFromPacked(eit) );

		hkaiNavMeshEdgePairInfo edgeInfo;
		edgeInfo.setFace( nit, &meshInstance->getFace( faceIndex ), meshInstance->getFaceDataPtr(faceIndex) );
		edgeInfo.m_outgoingEdgeInfo.setEdge( eit, &meshInstance->getEdge(edgeIndex), meshInstance->getEdgeDataPtr(edgeIndex), pointOnEdge );

		hkSimdReal costToParentSimd = hkSimdReal::fromFloat(costToParent);
		hkaiAstarCostModifier::NavMeshGetModifiedCostCallbackContext callbackContext( meshInstance,
			m_info,
			edgeInfo,
			(hkaiNavMeshEdgePairInfo::QUERY_TYPE_ASTAR | hkaiNavMeshEdgePairInfo::QUERY_INTERMEDIATE_POINT),
			costToParentSimd,
			euclideanDistance
			);
		return (costToParentSimd + m_costModifier->getModifiedCost( callbackContext )).getReal();
	}

}

hkBool32 hkaiNavMeshFaceGraph::isEdgeTraversable( hkaiPackedKey nit, hkaiPackedKey adj, hkaiPackedKey eit )
{
	const hkaiNavMesh::Edge& edge = m_collection->getEdgeFromPacked(eit);
	if (!edge.isTraversable())
	{
		return false;
	}
	
	hkVector4 edgeA, edgeB;

	// NB - this is the instance for nit and eit, but not necessarily adj
	const hkaiNavMesh::EdgeIndex edgeIndex = hkaiGetIndexFromPacked(eit);
	const hkaiNavMesh::FaceIndex faceIndex = hkaiGetIndexFromPacked(nit);
	m_navMesh->getEdgePoints(edgeIndex, edgeA, edgeB);

	// Sphere check
	if (m_searchSphereRadius.isGreaterEqualZero())
	{

		// We want to allow and face that's within the radius of the start point.
		// We could get the closest point on the face, and compare it against the start.
		// But it's equivalent to check the edge that we're crossing now.
		hkSimdReal distanceSqToEdge = hkcdPointSegmentDistanceSquared(m_startPoint, edgeA, edgeB);

		if ( distanceSqToEdge > m_searchSphereRadius*m_searchSphereRadius )
		{
			return false;
		}
	}


	// Capsule check
	if (m_searchCapsuleRadius.isGreaterEqualZero())
	{
		hkVector4 dab; dab.setSub(edgeB, edgeA);

		int i = 0;
		for (; i < m_numGoals; i++)
		{
			hkVector4 dse; dse.setSub(m_endPoints[i], m_startPoint);

			hkSimdReal distanceSquared = hkcdSegmentSegmentDistanceSquared(m_startPoint, dse, edgeA, dab);

			if (distanceSquared <= m_searchCapsuleRadius*m_searchCapsuleRadius)
			{
				break;
			}

		}
		if (i == m_numGoals)
		{
			return false;
		}
	}
	
	if (!m_edgeFilter)
	{
		return true;
	}

	const hkaiNavMeshInstance* meshInstance = m_collection->getInstanceAt( hkaiGetRuntimeIdFromPacked(eit) );


	// We don't fill out the "incoming" edge edgeInfo.
	// The face edgeInfo that is specified is for the current (nit) face.
	hkaiNavMeshEdgePairInfo edgeInfo;
	hkVector4 pos; pos.setInterpolate( edgeA, edgeB, hkSimdReal_Half );

	edgeInfo.setFace( nit, &meshInstance->getFace( faceIndex ), meshInstance->getFaceDataPtr(faceIndex) );
	edgeInfo.m_outgoingEdgeInfo.setEdge( eit, &meshInstance->getEdge(edgeIndex), meshInstance->getEdgeDataPtr(edgeIndex), pos );

	hkaiAstarEdgeFilter::NavMeshIsEnabledCallbackContext callbackContext( meshInstance,
		m_info,
		edgeInfo,
		(hkaiNavMeshEdgePairInfo::QUERY_TYPE_ASTAR | hkaiNavMeshEdgePairInfo::QUERY_INTERMEDIATE_POINT)
		);
	return m_edgeFilter->isEnabled(callbackContext);
}

hkBool32 hkaiNavMeshFaceGraph::isValidEdgeTarget( hkaiPackedKey eit ) const
{
	return (eit != HKAI_INVALID_PACKED_KEY);
}

void hkaiNavMeshFaceGraph::getPositionForHeuristic( hkaiPackedKey a, hkVector4& vecOut ) const
{
	getPosition(a, vecOut);
}

void hkaiNavMeshFaceGraph::getPosition( hkaiPackedKey a, hkVector4& vecOut ) const
{
	vecOut = m_cachedCentroids[ hkaiGetIndexFromPacked(a) ];
}

void hkaiNavMeshFaceGraph::getFaceCentroid( hkaiNavMesh::FaceIndex faceIndex, hkVector4& vecOut ) const
{
	vecOut = m_cachedCentroids[ faceIndex ];
}

void hkaiNavMeshFaceGraph::getFaceNormal( hkaiNavMesh::FaceIndex faceIndex, hkVector4& vecOut ) const
{
	vecOut = m_cachedNormals[ faceIndex ];
}

hkSimdReal hkaiNavMeshFaceGraph::distance( hkaiPackedKey A, hkaiPackedKey B ) const
{
	hkVector4 posA; getPosition(A, posA);
	hkVector4 posB; getPosition(B, posB);
	return posA.distanceTo( posB );
}

hkSimdReal hkaiNavMeshFaceGraph::distanceFromPosition( hkVector4Parameter posA, hkVector4Parameter posB ) const
{
	return posA.distanceTo( posB );
}

void hkaiNavMeshFaceGraph::nextNode( hkaiPackedKey nid )
{
	m_searchState->nextNode( nid );
}

void hkaiNavMeshFaceGraph::nextEdge( hkaiPackedKey nid, hkaiPackedKey ekey )
{
	m_searchState->nextEdge( edgeTarget(nid, ekey) );
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
