/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_CLIMB_UP_TRAVERSAL_H
#define HKAI_CLIMB_UP_TRAVERSAL_H

#include <Ai/SpatialAnalysis/Traversal/hkaiTraversalAnalyzer.h>
#include <Ai/SpatialAnalysis/Traversal/hkaiTraversalAnnotationLibrary.h>

struct hkaiTraversalAnalysisContext;
class hkaiTraversalAnnotationLibrary;

class HK_EXPORT_AI hkaiClimbUpAnalyzer : public hkaiTraversalAnalyzer
{
public:
	//+version(2)
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_SPATIAL);
	HK_DECLARE_REFLECTION();

	hkaiClimbUpAnalyzer() 
		: m_maxUnderhang(1.0f)
		, m_minUpHeight(0.5f)
		, m_maxUpHeight(3.0f)
		, m_grabAngle(1.0f)
		, m_grabScanDepth(1.0f)
		, m_verticalLipHeight(1.0f)
	{ }
	hkaiClimbUpAnalyzer(hkFinishLoadedObjectFlag f)
		: hkaiTraversalAnalyzer(f)
	{ }

	/// The maximum horizontal gap between the edge of the lower surface and the
	/// edge of the upper surface.
	hkReal m_maxUnderhang; //+default(1.0f)

	/// The minimum height difference between the lower and upper surfaces.
	/// Must be greater than zero.
	hkReal m_minUpHeight; //+default(0.5f)

	/// The maximum height difference between the lower and upper surfaces.
	hkReal m_maxUpHeight; //+default(3.0f)

	/// The angle to use when extracting a grab profile for the climb, in
	/// radians. Should be between 0 and pi/2, with a higher angle representing
	/// a more nearly vertical grab.
	hkReal m_grabAngle; //+default(1.0f)

	/// The extents of the bounding volume to search for a grab profile. Should
	/// be greater than m_minUpHeight, and less than the character height.
	hkReal m_grabScanDepth; //+default(1.0f)

	/// The maximum height above the navmesh of a vertical lip on the edge of
	/// the upper surface. A lip greater than this height will block traversal.
	hkReal m_verticalLipHeight; //+default(1.0f)

	virtual void analyze(hkaiTraversalAnalysisContext & context) const HK_OVERRIDE;
};

class hkaiClimbUpUtils
{
public:
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_SPATIAL, hkaiClimbUpUtils);

	static hkBool32 HK_CALL getGrabPoint(
		hkaiTraversalAnnotationLibrary const& library,
		hkaiTraversalAnnotationLibrary::Index& annotationIndex,
		hkVector4Parameter handPos,
		hkVector4 & grabPointOut,
		hkVector4 & grabFwdDirOut);

	static int HK_CALL getNumGrabSegments(	
		hkaiTraversalAnnotationLibrary const& library,
		hkaiTraversalAnnotationLibrary::Index& annotationIndex);

	static void HK_CALL getGrabSegment(
		hkaiTraversalAnnotationLibrary const& library,
		hkaiTraversalAnnotationLibrary::Index& annotationIndex,
		int handPlantSegmentIndex,
		hkVector4 & leftPointOut,
		hkVector4 & rightPointOut);
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
