/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_DROP_DOWN_TRAVERSAL_H
#define HKAI_DROP_DOWN_TRAVERSAL_H

#include <Ai/SpatialAnalysis/Traversal/hkaiTraversalAnalyzer.h>
#include <Ai/SpatialAnalysis/Traversal/hkaiTraversalAnnotationLibrary.h>

struct hkaiTraversalAnalysisContext;
class hkaiTraversalAnnotationLibrary;

class HK_EXPORT_AI hkaiDropDownAnalyzer : public hkaiTraversalAnalyzer
{
public:
	//+version(2)
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_SPATIAL);
	HK_DECLARE_REFLECTION();

	hkaiDropDownAnalyzer() 
		: m_minDropDistance(0.5f)
		, m_maxDropDistance(3.0f)
		, m_maxUnderhang(1.0f)
		, m_verticalLipHeight(1.0f)
	{ }
	hkaiDropDownAnalyzer(hkFinishLoadedObjectFlag f)
		: hkaiTraversalAnalyzer(f)
	{ }

	/// The minimum height difference between the upper and lower surfaces.
	/// Must be greater than 0.
	hkReal m_minDropDistance; //+default(0.5f)

	/// The maximum height difference between the upper and lower surfaces.
	/// Must be greater than m_maxDropDistance.
	hkReal m_maxDropDistance; //+default(3.0f)

	/// The maximum horizontal gap between the upper and lower surfaces.
	/// Must be greater than or equal to 0.
	hkReal m_maxUnderhang; //+default(1.0f)

	/// The maximum height above the navmesh of a vertical lip on the edge of
	/// the upper surface. A lip greater than this height will block traversal.
	hkReal m_verticalLipHeight; //+default(1.0f)

	virtual void analyze(hkaiTraversalAnalysisContext & context) const HK_OVERRIDE;
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
