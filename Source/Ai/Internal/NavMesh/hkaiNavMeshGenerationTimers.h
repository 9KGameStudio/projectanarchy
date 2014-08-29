/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKAI_NAVIGATION_MESH_GENERATION_TIMERS_H
#define HKAI_NAVIGATION_MESH_GENERATION_TIMERS_H

#include <Common/Base/Monitor/hkMonitorStream.h>


struct hkaiCsgTriangulateTimers
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_NAVMESH, hkaiCsgTriangulateTimers );
	void exportTimers(hkMonitorStream* HK_RESTRICT stream);

	hkMonitorStream::MultiTimerCommand m_timer_reset;
	hkMonitorStream::MultiTimerCommand m_timer_setDomain;
	hkMonitorStream::MultiTimerCommand m_timer_addEdges;
	hkMonitorStream::MultiTimerCommand m_timer_partition;
	hkMonitorStream::MultiTimerCommand m_timer_output;
	hkMonitorStream::MultiTimerCommand m_timer_raycastAgainstCuttingGeom;
};

struct hkaiCsgGetNearbyGeomTimers
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_NAVMESH, hkaiCsgGetNearbyGeomTimers );
	void exportTimers(hkMonitorStream* HK_RESTRICT stream);

	hkMonitorStream::MultiTimerCommand m_timer_treeQuery;
	hkMonitorStream::MultiTimerCommand m_timer_boxPrismSetup;
	hkMonitorStream::MultiTimerCommand m_timer_triPrism;
	hkMonitorStream::MultiTimerCommand m_timer_triAabb;
	hkMonitorStream::MultiTimerCommand m_timer_vertexMap;
	hkMonitorStream::MultiTimerCommand m_timer_convertGeom;
};

struct hkaiExtractCarversTimers
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_NAVMESH, hkaiCsgGetNearbyGeomTimers );
	void exportTimers(hkMonitorStream* HK_RESTRICT stream);

	hkMonitorStream::MultiTimerCommand m_timer_treeQuery;
	hkMonitorStream::MultiTimerCommand m_timer_calcGeom;
	hkMonitorStream::MultiTimerCommand m_timer_getGeometry;
	hkMonitorStream::MultiTimerCommand m_timer_edgeGeomSet;
	hkMonitorStream::MultiTimerCommand m_timer_concat;
};

struct hkaiCsgTimers
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_NAVMESH, hkaiCsgTimers );
	hkaiCsgTimers();
	~hkaiCsgTimers();

	void exportTimers();

	hkMonitorStream::MultiTimerCommand m_timer_csg;
		hkMonitorStream::MultiTimerCommand m_timer_isWalkable;
		hkMonitorStream::MultiTimerCommand m_timer_pruneTriangles;
		hkMonitorStream::MultiTimerCommand m_timer_reset;
		hkMonitorStream::MultiTimerCommand m_timer_getNearbyCuttingGeometry;
			hkaiCsgGetNearbyGeomTimers m_nearbyGeomTimers;
		hkMonitorStream::MultiTimerCommand m_timer_extrude;
		hkMonitorStream::MultiTimerCommand m_timer_addInRegion;
			hkaiExtractCarversTimers m_extractCarversTimer;
		hkMonitorStream::MultiTimerCommand m_timer_setFilter;
		hkMonitorStream::MultiTimerCommand m_timer_subtract;
		hkMonitorStream::MultiTimerCommand m_timer_triangulate;
			hkaiCsgTriangulateTimers			m_triangulationTimers;
		
};

struct hkaiRemoveUnreachableTimers
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_NAVMESH, hkaiRemoveUnreachableTimers );
	hkaiRemoveUnreachableTimers();
	~hkaiRemoveUnreachableTimers();

	void clear();
	void exportTimers();
	
	hkMonitorStream::MultiTimerCommand m_timer_RemoveUnreachable;
		hkMonitorStream::MultiTimerCommand m_timer_setup;
		hkMonitorStream::MultiTimerCommand m_timer_rayCast;
		hkMonitorStream::MultiTimerCommand m_timer_rayBundleCast;
		hkMonitorStream::MultiTimerCommand m_timer_carverContains;
		hkMonitorStream::MultiTimerCommand m_timer_removeTriangles;
};

struct hkaiConnectEdgesTimers
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_NAVMESH, hkaiConnectEdgesTimers );
	hkaiConnectEdgesTimers();
	~hkaiConnectEdgesTimers();

	void clear();
	void exportTimers();

	hkMonitorStream::MultiTimerCommand m_timer_findExact;
	hkMonitorStream::MultiTimerCommand m_timer_findConnectable;
		
	hkMonitorStream::MultiTimerCommand m_timer_setup;
	hkMonitorStream::MultiTimerCommand m_timer_calcConnectivity;
};

struct hkaiSimplifySegmentsConvexTimers
{

	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_NAVMESH, hkaiSimplifySegmentsConvexTimers );
	hkaiSimplifySegmentsConvexTimers();
	~hkaiSimplifySegmentsConvexTimers() {} // manually call export

	void clear();
	void exportTimers(hkMonitorStream* HK_RESTRICT stream);

	hkMonitorStream::MultiTimerCommand m_timer_setup;
	hkMonitorStream::MultiTimerCommand m_timer_triKeys;
	hkMonitorStream::MultiTimerCommand m_timer_sort;
	hkMonitorStream::MultiTimerCommand m_timer_sortColinear;
	hkMonitorStream::MultiTimerCommand m_timer_heightError;
	hkMonitorStream::MultiTimerCommand m_timer_intersectPartition;
	hkMonitorStream::MultiTimerCommand m_timer_signedPartitionAreas;
	hkMonitorStream::MultiTimerCommand m_timer_corridorImpassable;
};

struct hkaiSimplifySegmentsTimers
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_NAVMESH, hkaiSimplifySegmentsTimers );
	hkaiSimplifySegmentsTimers();
	~hkaiSimplifySegmentsTimers() {} // manually call export

	void clear();
	void exportTimers(hkMonitorStream* HK_RESTRICT stream);

	hkMonitorStream::MultiTimerCommand m_timer_gatherSegments;
	hkMonitorStream::MultiTimerCommand m_timer_buildTree;
	hkMonitorStream::MultiTimerCommand m_timer_convertToObb;
	hkMonitorStream::MultiTimerCommand m_timer_simplifyConvex;
		hkaiSimplifySegmentsConvexTimers m_convexTimers;
	hkMonitorStream::MultiTimerCommand m_timer_simplifyConcave;
};

struct hkaiPartitionTimers
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_NAVMESH, hkaiPartitionTimers );
	hkaiPartitionTimers();
	~hkaiPartitionTimers() {} // manually call export

	void clear();
	void exportTimers(hkMonitorStream* HK_RESTRICT stream);

	hkMonitorStream::MultiTimerCommand m_timer_reset;
	hkMonitorStream::MultiTimerCommand m_timer_canInclude;
	hkMonitorStream::MultiTimerCommand m_timer_include;
};

struct hkaiSimplificationTimers
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_NAVMESH, hkaiSimplificationTimers );
	hkaiSimplificationTimers();
	~hkaiSimplificationTimers();

	void clear();
	void exportTimers();

	hkMonitorStream::MultiTimerCommand m_timer_mergeShared;
		hkMonitorStream::MultiTimerCommand m_timer_selectiveMergeShared;
		hkMonitorStream::MultiTimerCommand m_timer_mergeDegen;
		hkMonitorStream::MultiTimerCommand m_timer_removeDegen;
	hkMonitorStream::MultiTimerCommand m_timer_edgeFilter;
	hkMonitorStream::MultiTimerCommand m_timer_partition;
		hkaiPartitionTimers m_partitionTimers;
	hkMonitorStream::MultiTimerCommand m_timer_trace;
	hkMonitorStream::MultiTimerCommand m_timer_simplifySegments;
		hkaiSimplifySegmentsTimers m_simplifySegmentsTimers;
	hkMonitorStream::MultiTimerCommand m_timer_addVertices;
	hkMonitorStream::MultiTimerCommand m_timer_triangulate;
	hkMonitorStream::MultiTimerCommand m_timer_hertelMehlhorn;
	
};


#endif // HKAI_NAVIGATION_MESH_GENERATION_TIMERS_H

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
