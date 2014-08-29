/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_BASE_TYPES
#define HKAI_BASE_TYPES

#include <Ai/Internal/hkaiExport.h>
#include <Common/Base/Config/hkConfigPackedKey.h> // Needed for HK_CONFIG_PACKED_KEY define

#if (HK_CONFIG_PACKED_KEY == HK_CONFIG_PACKED_KEY_64_BIT)
// DO NOT change this manually, make sure to change hkConfigPackedKey.h or add preprocessor defines for HK_CONFIG_PACKED_KEY instead.
#	define HKAI_64BIT_PACKED_KEYS
#endif

#ifdef HKAI_64BIT_PACKED_KEYS
typedef hkUint64 hkaiPackedKeyStorage;
typedef hkInt32 hkaiRuntimeIndex;
#else
typedef hkUint32 hkaiPackedKeyStorage;
typedef int hkaiRuntimeIndex;
#endif

typedef hkaiPackedKeyStorage hkaiPackedKey;
typedef hkUint32 hkaiSectionUid;
typedef hkUint32 hkaiTreeHandle;

typedef hkInt32 hkaiNavMeshFaceIndex;
typedef hkInt32 hkaiNavMeshEdgeIndex;
typedef hkInt32 hkaiNavMeshVertexIndex;
typedef hkInt32 hkaiNavMeshFaceData;
typedef hkInt32 hkaiNavMeshEdgeData;

typedef hkInt32 hkaiNavVolumeCellIndex;
typedef hkInt32 hkaiNavVolumeEdgeIndex;

typedef hkUint32 hkaiLayer;

#ifndef HKAI_64BIT_PACKED_KEYS
	/// Enums to determine hkaiPackedKey layout.
enum hkaiIndices
{ 
	HKAI_NUM_BITS_FOR_SECTION = 10,
	HKAI_NUM_BITS_FOR_INDEX = 22,

	HKAI_MAX_NUM_SECTIONS = 1<<HKAI_NUM_BITS_FOR_SECTION,
	HKAI_MAX_NUM_EDGES = 1<<HKAI_NUM_BITS_FOR_INDEX,
	HKAI_MAX_NUM_FACES = 1<<HKAI_NUM_BITS_FOR_INDEX,

	HKAI_INVALID_RUNTIME_INDEX = -1,

	// Special section ID to mark starting pseudo-nodes in the search graphs.
	HKAI_START_NODE_SECTION_ID = ((1<<HKAI_NUM_BITS_FOR_SECTION) - 2),
	// Special section ID to mark goal pseudo-nodes in the search graphs.
	HKAI_GOAL_NODE_SECTION_ID = ((1<<HKAI_NUM_BITS_FOR_SECTION) - 3),

	// Same as hkaiNavMesh::EDGE_EXTERNAL_OPPOSITE
	HKAI_EDGE_EXTERNAL_OPPOSITE_FLAG = 64,
};

#define HKAI_INDEX_FROM_KEY_MASK ((1<<HKAI_NUM_BITS_FOR_INDEX) - 1)

#else
enum hkaiIndices
{ 
	HKAI_NUM_BITS_FOR_SECTION = 32,
	HKAI_NUM_BITS_FOR_INDEX = 32,


	// Same as hkaiNavMesh::EDGE_EXTERNAL_OPPOSITE
	HKAI_EDGE_EXTERNAL_OPPOSITE_FLAG = 64,
};

#define HKAI_MAX_NUM_SECTIONS (HK_INT32_MAX)
#define HKAI_MAX_NUM_EDGES (HK_INT32_MAX)
#define HKAI_MAX_NUM_FACES (HK_INT32_MAX)
#define HKAI_INVALID_RUNTIME_INDEX (-1)
#define HKAI_START_NODE_SECTION_ID (HKAI_MAX_NUM_SECTIONS - 2)
#define HKAI_GOAL_NODE_SECTION_ID (HKAI_MAX_NUM_SECTIONS - 3)

#define HKAI_INDEX_FROM_KEY_MASK ((1LL<<HKAI_NUM_BITS_FOR_INDEX) - 1)

#endif

#define HKAI_INVALID_SECTION_UID (hkaiSectionUid(-1))
#define HKAI_INVALID_PACKED_KEY (hkaiPackedKeyStorage(-1))	
#define HKAI_DEFAULT_LAYER ( hkaiLayer(1) )
#define HKAI_ALL_LAYERS ( hkaiLayer(-1) )



HK_FORCE_INLINE hkaiRuntimeIndex HK_CALL hkaiGetRuntimeIdFromPacked(hkaiPackedKey key);
HK_FORCE_INLINE int HK_CALL hkaiGetIndexFromPacked(hkaiPackedKey key);
HK_FORCE_INLINE hkaiPackedKey HK_CALL hkaiGetPackedKey(hkaiRuntimeIndex sectionId, int index);

HK_FORCE_INLINE hkaiPackedKey HK_CALL hkaiGetOppositePackedKey( hkUint8 flags, hkaiRuntimeIndex thisRuntimeId, hkaiPackedKey storedKey);
HK_FORCE_INLINE hkaiRuntimeIndex HK_CALL hkaiGetOppositeRuntimeIndex( hkUint8 flags, hkaiRuntimeIndex thisRuntimeId, hkaiPackedKey storedKey);

// Handy macro for checking return values during generation
#define HKAI_CHECK_SUCCESS( RES ){ if ( HK_VERY_UNLIKELY((RES) != HK_SUCCESS)) {return HK_FAILURE;} }

#ifndef HK_PLATFORM_SPU
typedef class hkaiNavMeshInstance hkaiNavMeshAccessor;
#else
typedef class hkaiSpuNavMeshAccessor hkaiNavMeshAccessor;
#endif

#include <Ai/Pathfinding/hkaiBaseTypes.inl>

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
