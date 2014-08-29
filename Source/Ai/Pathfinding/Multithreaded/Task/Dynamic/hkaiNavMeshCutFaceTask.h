/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_NAVMESH_CUT_FACE_TASK_H
#define HKAI_NAVMESH_CUT_FACE_TASK_H

#include <Ai/Pathfinding/hkaiBaseTypes.h>
#include <Ai/Pathfinding/Multithreaded/hkaiDefaultTask.h>

struct hkaiSilhouetteGeneratorSectionContext;
class hkaiNavMeshInstance;

class hkaiNavMeshCutFaceTask : public hkaiDefaultTask
{
public:
	HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI );
	hkaiNavMeshCutFaceTask()
	{
		m_meshInstance = HK_NULL;
		m_faceKey = HKAI_INVALID_PACKED_KEY;
		m_generatorContexts = HK_NULL;
		m_numGenerators = -1;
		m_firstNonCuttingGeneratorIndex = -1;
		m_worldUp.setZero();
		m_extrusionAmount = 0.0f;
		m_radiusExpansionAmount = 0.0f;
		m_erosionRadius = 0.0f;
		m_isWallClimbing = false;
		m_results = HK_NULL;
		m_generationParams = HK_NULL;
	}

	void process() HK_OVERRIDE;

#if defined(HK_PLATFORM_HAS_SPU)
	virtual void* getSpuElf() HK_OVERRIDE;
#endif

	enum
	{
		
		MAX_MATERIALS = 14,
	};

	/// Pointer to nav mesh instance (in main memory)
	const hkaiNavMeshInstance* m_meshInstance;
	/// Key of the face to be cut
	hkaiPackedKey m_faceKey;

	const hkaiSilhouetteGeneratorSectionContext* const* m_generatorContexts;
	int m_numGenerators;

	/// Index of the first non-cutting material.
	/// Any generator before this will be treated as cutting even if it has an non-cutting material type.
	/// This can happen in cases where we have too many distinct materials and have to force some to be cutting.
	/// In order to reduce the impact of artifacts from triangulation, we move all the cutting silhouettes to
	/// the front of the array.
	int m_firstNonCuttingGeneratorIndex;

	hkVector4 m_worldUp;
	hkReal m_extrusionAmount;
	hkReal m_radiusExpansionAmount;
	hkReal m_erosionRadius;

	hkBool m_isWallClimbing; 
	hkBool m_useNewCutter;

	// location of hkaiFaceCutResults
	struct hkaiFaceCutResults* m_results;

	const struct hkaiSilhouetteGenerationParameters* m_generationParams;
};

#endif // HKAI_NAVMESH_CUT_FACE_TASK_H

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
