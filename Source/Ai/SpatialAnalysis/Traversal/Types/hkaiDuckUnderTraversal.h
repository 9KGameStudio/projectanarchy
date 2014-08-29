/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_DUCK_UNDER_TRAVERSAL_H
#define HKAI_DUCK_UNDER_TRAVERSAL_H

#include <Ai/SpatialAnalysis/Traversal/hkaiTraversalAnalyzer.h>
#include <Ai/SpatialAnalysis/Traversal/hkaiTraversalAnnotationLibrary.h>

struct hkaiTraversalAnalysisContext;
class hkaiTraversalAnnotationLibrary;

class HK_EXPORT_AI hkaiDuckUnderAnalyzer : public hkaiTraversalAnalyzer
{
public:
	//+version(1)
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_SPATIAL);
	HK_DECLARE_REFLECTION();

	hkaiDuckUnderAnalyzer() 
		: m_maxHorizontalDistance(0.5f)
		, m_minClearance(0.5f)
		, m_maxClearance(3.0f)
		, m_maxHeightDifference(0.25f)
	{ }
	hkaiDuckUnderAnalyzer(hkFinishLoadedObjectFlag f)
		: hkaiTraversalAnalyzer(f)
	{ }

	/// The minimum horizontal distance between the two surfaces. Must be 
	/// greater than 0.
	hkReal m_maxHorizontalDistance; //+default(0.5f)
	
	/// The minimum vertical clearance above the ground connecting the two
	/// surfaces. Must be greater than 0.
	hkReal m_minClearance; //+default(0.5f)

	/// The maximum vertical clearance above the ground connecting the two
	/// surfaces. Must be greater than m_minClearance.
	hkReal m_maxClearance; //+default(3.0f)

	/// The maximum vertical height difference between the two surfaces.
	hkReal m_maxHeightDifference; //+default(0.5f)

	virtual void analyze(hkaiTraversalAnalysisContext & context) const HK_OVERRIDE;
};


class hkaiDuckUnderUtils
{
public:
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_SPATIAL, hkaiDuckUnderUtils);

	static hkBool32 HK_CALL getClearance(
		hkaiTraversalAnnotationLibrary const& library,
		hkaiTraversalAnnotationLibrary::Index& annotationIndex,
		hkVector4Parameter startPoint,
		hkReal & clearanceOut);

	static int HK_CALL getNumClearanceIntervals(	
		hkaiTraversalAnnotationLibrary const& library,
		hkaiTraversalAnnotationLibrary::Index& annotationIndex);

	static void HK_CALL getClearanceInterval(
		hkaiTraversalAnnotationLibrary const& library,
		hkaiTraversalAnnotationLibrary::Index& annotationIndex,
		int clearanceIntervalIndex,
		hkVector4 & leftStartPointOut,
		hkVector4 & rightStartPointOut,
		hkVector4 & leftEndPointOut,
		hkVector4 & rightEndPointOut);
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
