/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef __VHAVOK_AI_NAVMESH_DEBUG_DISPLAY_HPP
#define __VHAVOK_AI_NAVMESH_DEBUG_DISPLAY_HPP

#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/vHavokAiIncludes.hpp>

class vHavokAiNavMeshDebugDisplayHandler : public hkDebugDisplayHandler
{
public:
	virtual ~vHavokAiNavMeshDebugDisplayHandler() {}

	// Unhandled functions
	virtual hkResult addGeometry(const hkArrayBase<hkDisplayGeometry*>& geometries, const hkTransform& transform, hkUlong id, int tag, hkUlong shapeIdHint, hkGeometry::GeometryType geomType) HKV_OVERRIDE;
	virtual hkResult addGeometry(hkDisplayGeometry* geometry, hkUlong id, int tag, hkUlong shapeIdHint) HKV_OVERRIDE;
	virtual hkResult addGeometryInstance(hkUlong origianalGeomId, const hkTransform& transform, hkUlong id, int tag, hkUlong shapeIdHint) HKV_OVERRIDE;
	virtual hkResult setGeometryColor(hkColor::Argb color, hkUlong id, int tag) HKV_OVERRIDE;
	virtual hkResult setGeometryTransparency(float alpha, hkUlong id, int tag) HKV_OVERRIDE;
	virtual hkResult setGeometryPickable( hkBool isPickable, hkUlong id, int tag ) HKV_OVERRIDE;
	virtual hkResult setGeometryVisibility(int geometryIndex, bool isEnabled, hkUlong id, int tag) HKV_OVERRIDE;
	virtual hkResult updateGeometry(const hkTransform& transform, hkUlong id, int tag) HKV_OVERRIDE;
	virtual hkResult updateGeometry(const hkMatrix4& transform, hkUlong id, int tag) HKV_OVERRIDE;
	virtual hkResult skinGeometry(hkUlong* ids, int numIds, const hkMatrix4* poseModel, int numPoseModel, const hkMatrix4& worldFromModel, int tag) HKV_OVERRIDE;
	virtual hkResult removeGeometry(hkUlong id, int tag, hkUlong shapeIdHint) HKV_OVERRIDE;
	virtual hkResult updateCamera(const hkVector4& from, const hkVector4& to, const hkVector4& up, hkReal nearPlane, hkReal farPlane, hkReal fov, const char* name) HKV_OVERRIDE;
	virtual hkResult displayPoint(const hkVector4& position, hkColor::Argb color, int id, int tag) HKV_OVERRIDE;
	virtual hkResult displayText(const char* text, hkColor::Argb color, int id, int tag) HKV_OVERRIDE;
	virtual hkResult displayGeometry(const hkArrayBase<hkDisplayGeometry*>& geometries, const hkTransform& transform, hkColor::Argb color, int id, int tag) HKV_OVERRIDE;
	virtual hkResult displayGeometry(const hkArrayBase<hkDisplayGeometry*>& geometries, hkColor::Argb color, int id, int tag) HKV_OVERRIDE;
	virtual hkResult sendMemStatsDump(const char* data, int length) HKV_OVERRIDE;
	virtual hkResult holdImmediate() HKV_OVERRIDE;

	// only these functions are implemented
	virtual hkResult displayLine(const hkVector4& start, const hkVector4& end, hkColor::Argb color, int id, int tag) HKV_OVERRIDE;
	virtual hkResult displayTriangle(const hkVector4& a, const hkVector4& b, const hkVector4& c, hkColor::Argb color, int id, int tag) HKV_OVERRIDE;
	virtual hkResult display3dText(const char* text, const hkVector4& pos, hkColor::Argb color, int id, int tag) HKV_OVERRIDE;

	// compatibility with VisionIntegration branch
#if defined(HAVOK_SDK_VERSION_MAJOR) && (HAVOK_SDK_VERSION_MAJOR >= 2012)
	virtual hkResult displayPoint2d(const hkVector4& position, hkColor::Argb color, int id, int tag)  HKV_OVERRIDE { return HK_SUCCESS; }
	virtual hkResult displayLine2d(const hkVector4& start, const hkVector4& end, hkColor::Argb color, int id, int tag)  HKV_OVERRIDE { return HK_SUCCESS; }
	virtual hkResult displayTriangle2d(const hkVector4& a, const hkVector4& b, const hkVector4& c, hkColor::Argb color, int id, int tag)  HKV_OVERRIDE { return HK_SUCCESS; }
	virtual hkResult displayText2d(const char* text, const hkVector4& pos, hkReal sizeScale, hkColor::Argb color, int id, int tag)  HKV_OVERRIDE { return HK_SUCCESS; }
#endif
};


#endif	// __VHAVOK_AI_NAVMESH_DEBUG_DISPLAY_HPP

/*
 * Havok SDK - Base file, BUILD(#20140327)
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
