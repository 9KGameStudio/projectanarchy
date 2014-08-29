/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_TRAVERSAL_ANALYSIS_H
#define HKAI_TRAVERSAL_ANALYSIS_H

#include <Ai/SpatialAnalysis/Traversal/hkaiPairedEdgeFinder.h>
#include <Ai/Internal/SegmentCasting/hkaiIntervalPartition.h>
#include <Ai/Internal/SegmentCasting/hkaiGeometrySegmentCaster.h>
#include <Ai/Internal/SegmentCasting/hkaiNavMeshInstanceSegmentCaster.h>
#include <Common/Base/Types/Geometry/hkGeometry.h>
#include <Common/Base/Container/ObjectCache/hkObjectCache.h>
#include <Ai/SpatialAnalysis/Traversal/hkaiTraversalAnnotationLibrary.h>
#include <Ai/SpatialAnalysis/Traversal/hkaiTraversalAnalysisSettings.h>
#include <Common/Base/Container/BitField/hkBitField.h>

struct hkaiTraversalAnalysisContext;
struct hkKeyPair;
class hkaiNavMeshInstance;
class hkaiTraversalAnalyzer;

struct HK_EXPORT_AI hkaiTraversalAnalysisInput : public hkReferencedObject
{
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_SPATIAL);
	HK_DECLARE_REFLECTION();

	hkaiTraversalAnalysisInput() {}
	hkaiTraversalAnalysisInput(hkFinishLoadedObjectFlag flag)
		: hkReferencedObject(flag)
		, m_sections(flag) { }

	struct HK_EXPORT_AI Section
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_SPATIAL, Section);
		HK_DECLARE_REFLECTION();

		Section() { }
		Section(hkFinishLoadedObjectFlag flag)
			: m_navMeshInstance(flag)
			, m_geometry(flag)
			, m_walkableBitfield(flag)
			, m_cuttingBitfield(flag) { }

		hkRefPtr<const hkaiNavMeshInstance> m_navMeshInstance;
		hkRefPtr<const hkGeometry> m_geometry;
		hkBitField m_walkableBitfield;
		hkBitField m_cuttingBitfield;
	};

	hkArray<Section> m_sections;

	Section const& getSectionByUid(hkaiSectionUid uid) const;
};

struct HK_EXPORT_AI hkaiTraversalAnalysisOutput : public hkReferencedObject
{
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_SPATIAL);
	HK_DECLARE_REFLECTION();

	hkaiTraversalAnalysisOutput() { }
	hkaiTraversalAnalysisOutput(hkFinishLoadedObjectFlag flag) 
		: hkReferencedObject(flag)
		, m_sections(flag) { }

	struct HK_EXPORT_AI Section
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_SPATIAL, Section);
		HK_DECLARE_REFLECTION();

		Section() { }
		Section(hkFinishLoadedObjectFlag flag) : m_annotationLibrary(flag) { }

		hkaiSectionUid m_uid;
		hkRefPtr<hkaiTraversalAnnotationLibrary> m_annotationLibrary;
	};

	hkArray<Section> m_sections;

	Section const& getSectionByUid(hkaiSectionUid uid) const;
};

/// Utility for automatically creating user edges corresponding to jumps and vaults.
/// This feature is in beta. Behavior or interface may change in future releases.
class HK_EXPORT_AI hkaiTraversalAnalysis : public hkReferencedObject
{
public:
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_SPATIAL);
	HK_DECLARE_REFLECTION();

	hkaiTraversalAnalysis();
	hkaiTraversalAnalysis(hkFinishLoadedObjectFlag f);
	~hkaiTraversalAnalysis();

	/// The common settings to use for detecting traversals
	hkRefPtr<hkaiTraversalAnalysisSettings> m_settings;

	/// The set of analyzers to run
	hkArray<hkRefPtr<hkaiTraversalAnalyzer> > m_analyzers;

	/// The set of navmeshes and associated geometries
	hkRefPtr<hkaiTraversalAnalysisInput> m_input;

	/// The output of traversal analysis
	hkRefPtr<hkaiTraversalAnalysisOutput> m_output;

	void doTraversalAnalysis();

	static const hkUint32 MAX_ACCELERATION_DATA_SIZE = 0xFFFFFFFFu;

private:
	struct InternalContext;

	void initInternalContext(InternalContext & internalContext);
	void initContext(hkaiTraversalAnalysisContext & context);
	void initOutput();
	void findOverlaps(hkArray<hkKeyPair>::Temp & overlaps);

	void doTraversalAnalysisBetweenSections(
		int fromSectionIndex, 
		int toSectionIndex, 
		InternalContext & internalContext, 
		hkaiTraversalAnalysisContext & context);

	struct SectionAccelerationData;

	SectionAccelerationData* buildSectionAccelerationData(hkaiTraversalAnalysisInput::Section const& section);

	void setupContextForSections(
		int fromSectionIndex,
		int toSectionIndex,
		InternalContext & internalContext, 
		hkaiTraversalAnalysisContext & context);

	struct HK_EXPORT_AI SectionAccelerationData : public hkReferencedObject
	{
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_SPATIAL);
		hkRefPtr<hkaiPairedEdgeFinder::AccelerationData> m_pairedEdgeData;
		hkRefPtr<hkaiGeometrySegmentCaster::AccelerationData> m_walkableGeometryData;
		hkRefPtr<hkaiGeometrySegmentCaster::AccelerationData> m_cuttingGeometryData;
		hkRefPtr<hkaiNavMeshInstanceSegmentCaster::AccelerationData> m_navMeshData;

		hkUint32 getSize() const;
	};

	struct HK_EXPORT_AI InternalContext
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_SPATIAL, InternalContext);
		hkRefPtr<hkObjectCache<hkaiSectionUid> > m_sectionAccelerationDatas;
	};

};


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
