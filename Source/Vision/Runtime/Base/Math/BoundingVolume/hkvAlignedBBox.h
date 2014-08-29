/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VBASE_HKVMATH_HKVALIGNEDBOUNDINGBOX_H
#define VBASE_HKVMATH_HKVALIGNEDBOUNDINGBOX_H

#include <Vision/Runtime/Base/Math/hkvMath.h>
#include <Vision/Runtime/Base/Math/Vector/hkvVec3.h>
#include <Vision/Runtime/Base/Math/Matrix/hkvMat4.h>

/// \brief
///   Serializes the bbox to/from the archive (reads/writes exactly 6 floats).
VBASE_IMPEXP void SerializeX (VArchive& ar, hkvAlignedBBox& obj);

#ifdef HKVBBOX_CHECK_FOR_NAN
  #define HKVBBOX_INITIALIZATION_CHECK(obj) (obj)->isInitializedCheck()
#else
  #define HKVBBOX_INITIALIZATION_CHECK(obj) 
#endif

/// \brief
///   An axis aligned bounding box class.
class hkvAlignedBBox
{
public:

  ///
  /// @name Constructors
  /// @{
  ///

  /// \brief
  ///   ATTENTION: The object is NOT initialized by the constructor. You MUST initialize it yourself before using it.
  ///
  /// \note In Dev and Debug builds the object will be initialized with NaN values. Member functions that read the values will check that they are not NaN.
  /// If an NaN value is encountered, those functions will trigger an assert. Thus when you run into such an assert, you have not initialized your object
  /// after construction. Make sure you always initialize objects properly before using them.
  HKV_FORCE_INLINE hkvAlignedBBox()
  {
    #ifdef HKVBBOX_INITIALIZE_TO_NAN

      m_vMin.set(hkvMath::generateNaN());
      m_vMax.set(hkvMath::generateNaN());

    #elif defined(HKVBBOX_INITIALIZE_TO_IDENTITY)

      setInvalid();

    #endif
    }

  #ifdef HKVBBOX_CHECK_FOR_NAN
    HKV_FORCE_INLINE void isInitializedCheck() const
    {
      VASSERT_MSG(!hkvMath::isNaN(m_vMin.x) && !hkvMath::isNaN (m_vMin.y) && !hkvMath::isNaN (m_vMin.z) &&
                  !hkvMath::isNaN(m_vMax.x) && !hkvMath::isNaN (m_vMax.y) && !hkvMath::isNaN (m_vMax.z),
        "This object has invalid (NaN) members: Min = (%.2f | %.2f | %.2f) Max = (%.2f | %.2f | %.2f).\nThis happens when you use this object without properly initializing it first, as the default constructor will set all members to NaN in debug builds.", 
        m_vMin.x, m_vMin.y, m_vMin.z, m_vMax.x, m_vMax.y, m_vMax.z);
    }
  #endif

  /// \brief
  ///   Does not initialize this object. Prefer this constructor over the default constructor.
  ///
  /// \param init
  ///   Set this to hkvNoInitialization to actually select this constructor.
  HKV_FORCE_INLINE explicit hkvAlignedBBox (hkvInit_None init /* = hkvNoInitialization */) { }

  /// \brief
  ///   Initializes the box with the given min and max values.
  HKV_FORCE_INLINE hkvAlignedBBox (const hkvVec3& vMin, const hkvVec3& vMax);

  ///
  /// @}
  ///

  ///
  /// @name Setup
  /// @{
  ///

  /// \brief
  ///   Sets the box to all zero.
  ///
  /// \note
  ///   This is a valid bounding box with a volume of zero.
  HKV_FORCE_INLINE void setZero ();

  /// \brief
  ///   Sets the box's min and max values from the given center and half-size extents.
  HKV_FORCE_INLINE void setCenterAndSize (const hkvVec3& vCenter, const hkvVec3& vHalfSizeXYZ);

  /// \brief
  ///   Sets the box's min and max values. In debug mode this will check that vMin <= vMax, to ensure a valid bbox.
  HKV_FORCE_INLINE void set (const hkvVec3& vMin, const hkvVec3& vMax);

  /// \brief
  ///   Sets the box's min and max values. Never checks whether vMin <= vMax, to allow to set invalid boxes. Useful when initializing data from serialized data.
  HKV_FORCE_INLINE void setWithoutValidityCheck (const hkvVec3& vMin, const hkvVec3& vMax);

  /// \brief
  ///   Sets the box to be the bounding box of the given set of points.
  ///
  /// \param pPoints
  ///   Pointer to an array holding a set of points.
  ///
  /// \param uiNumPoints
  ///   Number of points that should be used from the array.
  ///
  /// \param uiStride
  ///   The stride (in bytes) between each point. Must be at least sizeof (hkvVec3) for a pure hkvVec3 array.
  ///   can be larger, if there is other data in between the points.
  VBASE_IMPEXP void setFromPoints (const hkvVec3* HKV_RESTRICT pPoints, hkUint32 uiNumPoints, hkUint32 uiStride = sizeof (hkvVec3));

  ///
  /// @}
  ///

  ///
  /// @name Piecewise Setup
  /// @{
  ///

  /// \brief
  ///
  /// Sets m_vMin to the maximum positive float value, and m_vMax to the maximum negative float value (min > max),
  /// making it an invalid box.
  ///
  /// The various expandToInclude functions can then be used to enlarge the box until it is large enough.
  ///
  /// \sa hkvAlignedBBox::expandToInclude
  HKV_FORCE_INLINE void setInvalid ();

  /// \brief
  ///   Returns true if m_vMin <= m_vMax.
  HKV_FORCE_INLINE bool isValid (void) const;

  /// \brief
  ///   Reduces the box such that only the intersection with the given box remains.
  /// 
  /// Box is invalid afterwards if the two boxes have no overlapping region.
  /// 
  /// \param rhs
  ///   Box to intersect with the callee.
  ///
  /// \sa hkvAlignedBBox::isValid
  HKV_FORCE_INLINE void reduceToIntersection (const hkvAlignedBBox& rhs);

  /// \brief
  ///   Enlarges the box such that v is contained in it (if necessary).
  HKV_FORCE_INLINE void expandToInclude (const hkvVec3& v);

  /// \brief
  ///   Enlarges the box such that the given set of points is contained in it (if necessary).
  ///
  /// \param pPoints
  ///   Pointer to an array holding a set of points.
  ///
  /// \param uiNumPoints
  ///   Number of points that should be used from the array.
  ///
  /// \param uiStride
  ///   The stride (in bytes) between each point. Must be at least sizeof (hkvVec3) for a pure hkvVec3 array.
  ///   can be larger, if there is other data in between the points.
  VBASE_IMPEXP    void expandToInclude (const hkvVec3* HKV_RESTRICT pPoints, hkUint32 uiNumPoints, hkUint32 uiStride = sizeof (hkvVec3));

  /// \brief
  ///   Enlarges the box such that the given object is contained in it (if necessary).
  HKV_FORCE_INLINE void expandToInclude (const hkvAlignedBBox& rhs);

  /// \brief
  ///   Enlarges the box such that the given object is contained in it (if necessary).
  HKV_FORCE_INLINE void expandToInclude (const hkvBoundingSphere& rhs);

  /// \brief
  ///   Makes all sides of the box as long as its longest side, thus making it a cube.
  HKV_FORCE_INLINE void expandToCube ();

  /// \brief
  ///   Changes the size by vChangeXYZ in each direction, e.g. along the positive AND negative axis.
  HKV_FORCE_INLINE void addBoundary (const hkvVec3& vChangeXYZ);

  ///
  /// @}
  ///

  ///
  /// @name Modification
  /// @{
  ///

  /// \brief 
  ///   Transforms the 8 corners in object-space and then recomputes the AABB.
  HKV_FORCE_INLINE void transformFromCenter (const hkvMat4& mTransform);

  /// \brief 
  ///   Transforms the 8 corners in world-space and then recomputes the AABB.
  HKV_FORCE_INLINE void transformFromOrigin (const hkvMat4& mTransform);

  /// \brief
  ///   Translates the box in world-space.
  HKV_FORCE_INLINE void translate (const hkvVec3& vTranslation);

  /// \brief
  ///   Scales the box's size, but does not change its position.
  HKV_FORCE_INLINE void scaleFromCenter (const hkvVec3& vScaleXYZ);

  /// \brief
  ///   Scales the box's min and max extents, thus changing its size and position.
  HKV_FORCE_INLINE void scaleFromOrigin (const hkvVec3& vScaleXYZ);

  ///
  /// @}
  ///

  ///
  /// @name Checks
  /// @{
  ///

  /// \brief
  ///   Checks whether the two boxes are identical.
  HKV_FORCE_INLINE bool isIdentical (const hkvAlignedBBox& rhs) const;

  /// \brief
  ///   Checks whether the two boxes are equal withing some epsilon range.
  HKV_FORCE_INLINE bool isEqual (const hkvAlignedBBox& rhs, float fEpsilon) const;

  /// \brief
  ///   Checks whether the box is all zero within some epsilon range.
  HKV_FORCE_INLINE bool isZero (float fEpsilon = HKVMATH_DEFAULT_EPSILON) const;

  ///
  /// @}
  ///

  ///
  /// @name Retrieving Data
  /// @{
  ///

  /// \brief
  ///   Returns the center point of the box.
  HKV_FORCE_INLINE const hkvVec3 getCenter () const;

  /// \brief
  ///   Returns the box's extent along the X axis.
  HKV_FORCE_INLINE float getSizeX () const;

  /// \brief
  ///   Returns the box's extent along the Y axis.
  HKV_FORCE_INLINE float getSizeY () const;

  /// \brief
  ///   Returns the box's extent along the Z axis.
  HKV_FORCE_INLINE float getSizeZ () const;

  /// \brief
  ///   Writes the 8 corners of the box to the provided array.
  ///
  /// \param out_pVertices
  ///   The array to which the corners are written. Must be large enough to store 8 vectors.
  ///
  /// \sa hkvAlignedBBox::getCorner
  HKV_FORCE_INLINE void getCorners (hkvVec3* HKV_RESTRICT out_pVertices) const;

  /// \brief
  ///   Returns one corner of the box.
  ///
  /// \param bMaxX
  ///   If set to true the X coordinate of the returned corner will be from the max extent.
  ///   Otherwise it will be from the min extent.
  ///
  /// \param bMaxY
  ///   If set to true the Y coordinate of the returned corner will be from the max extent.
  ///   Otherwise it will be from the min extent.
  ///
  /// \param bMaxZ
  ///   If set to true the Z coordinate of the returned corner will be from the max extent.
  ///   Otherwise it will be from the min extent.
  ///
  /// \sa hkvAlignedBBox::getCorners
  HKV_FORCE_INLINE hkvVec3 getCorner (bool bMaxX, bool bMaxY, bool bMaxZ) const;

  /// \brief
  ///   Checks which of the extents (X,Y,Z) is the longest. Returns that value and its index (0,1,2).
  ///
  /// \param out_iAxis
  ///   Optional. If set the index of the longest extent (0, 1 or 2) will be stored here.
  ///
  /// \return
  ///   The length of the longest extent.
  HKV_FORCE_INLINE float getMaxExtent (hkUint32* HKV_RESTRICT out_iAxis = NULL) const;

  /// \brief
  ///   Checks which of the extents (X,Y,Z) is the shortest. Returns that value and its index (0,1,2).
  ///
  /// \param out_iAxis
  ///   Optional. If set the index of the shortest extent (0, 1 or 2) will be stored here.
  ///
  /// \return
  ///   The length of the shortest extent.
  HKV_FORCE_INLINE float getMinExtent (hkUint32* HKV_RESTRICT out_iAxis = NULL) const;

  /// \brief
  ///   Returns a hkvBoundingSphere that perfectly fits around this box.
  HKV_FORCE_INLINE const hkvBoundingSphere getBoundingSphere () const;

  ///
  /// @}
  ///

  ///
  /// @name Bounding Volume Checks
  /// @{
  ///

  /// \brief
  ///   Returns true if the given point is inside the box.
  HKV_FORCE_INLINE bool contains (const hkvVec3& v) const;

  /// \brief
  ///   Returns true if all points from the given set are inside the box.
  ///
  /// \param pPoints
  ///   Pointer to an array holding a set of points.
  ///
  /// \param uiNumPoints
  ///   Number of points that should be used from the array.
  ///
  /// \param uiStride
  ///   The stride (in bytes) between each point. Must be at least sizeof (hkvVec3) for a pure hkvVec3 array.
  ///   can be larger, if there is other data in between the points.
  VBASE_IMPEXP    bool contains (const hkvVec3* HKV_RESTRICT pPoints, hkUint32 uiNumPoints, hkUint32 uiStride = sizeof (hkvVec3)) const;

  /// \brief
  ///   Returns true if the given box is inside this box.
  HKV_FORCE_INLINE bool contains (const hkvAlignedBBox& rhs) const;

  /// \brief
  ///   Returns true if the given sphere is inside this box.
  HKV_FORCE_INLINE bool contains (const hkvBoundingSphere& rhs) const;

  /// \brief
  ///   Returns true if the given object is inside this box or partially overlaps it.
  HKV_FORCE_INLINE bool overlaps (const hkvAlignedBBox& rhs) const;

  /// \brief
  ///   Returns true if the given object is inside this box or partially overlaps it.
  HKV_FORCE_INLINE bool overlaps (const hkvBoundingSphere& rhs) const;

  ///
  /// @}
  ///

  ///
  /// @name Distance Checks
  /// @{
  ///

  /// \brief
  ///   Returns the squared distance of the point v to this box.
  ///
  /// For points that are inside the box this function returns zero.
  ///
  /// \param v
  ///   The point for which the distance is computed.
  HKV_FORCE_INLINE float getDistanceToSquared (const hkvVec3& v) const;

  /// \brief
  ///   Returns the squared distance of the given box to this box.
  ///
  /// If the two boxes overlap, zero is returned.
  ///
  /// \param rhs
  ///   The box for which the distance is computed.
  HKV_FORCE_INLINE float getDistanceToSquared (const hkvAlignedBBox& rhs) const;
  
  /// \brief
  ///   Returns the distance of the point v to this box.
  ///
  /// For points that are inside the box this function returns zero.
  ///
  /// \param v
  ///   The point for which the distance is computed.
  HKV_FORCE_INLINE float getDistanceTo (const hkvVec3& v) const;

  /// \brief
  ///   Returns the distance of the given box to this box.
  ///
  /// If the two boxes overlap, zero is returned.
  ///
  /// \param rhs
  ///   The box for which the distance is computed.
  HKV_FORCE_INLINE float getDistanceTo (const hkvAlignedBBox& rhs) const;


  /// \brief
  ///   Returns the distance of the given sphere to this box.
  ///
  /// If the two objects overlap, zero is returned.
  ///
  /// \param rhs
  ///   The sphere for which the distance is computed.
  HKV_FORCE_INLINE float getDistanceTo (const hkvBoundingSphere& rhs) const;

  ///
  /// @}
  ///

  ///
  /// @name Intersection Tests
  /// @{
  ///

  /// \brief
  ///   Checks whether the given ray intersects the surface of the box and computes the intersection time and point.
  ///
  /// If the ray start position is within the box, and intersection with the surface will always be detected.
  /// If however, you need to handle such cases as if the box was solid and the intersection point should already be
  /// at the ray start position, you should check first whether the box contains the ray start position already.
  ///
  /// \param vRayStartPos
  ///   The start position of the ray. If the start position is inside the box, an intersection will always
  ///   be detected, however not at time 0, but wherever the ray hits the surface of the box.
  ///
  /// \param vRayDir
  ///   The direction of the ray. May be normalized but is not required.
  ///
  /// \param out_fIntersectionTime
  ///   Optional. If set, the 'intersection-time' will be stored here.
  ///   The intersection time will always be >= 0.
  ///   If the ray misses the box, this value will not be modified.
  ///   vRayStartPos + vRayDir * *out_fIntersectionTime == *out_IntersectionPoint
  ///   That means the intersection time scales anti-proportionally to the length of vRayDir.
  ///
  /// \param out_IntersectionPoint
  ///   Optional. If set, the point of intersection will be stored here.
  ///   If the ray misses the box, this value will not be modified.
  ///   vRayStartPos + vRayDir * *out_fIntersectionTime == *out_IntersectionPoint
  ///
  /// \return
  ///   True if the ray hits the box, false otherwise.
  ///
  /// \sa hkvAlignedBBox::getLineSegmentIntersection
  VBASE_IMPEXP    bool getRayIntersection (const hkvVec3& vRayStartPos, const hkvVec3& vRayDir, float* HKV_RESTRICT out_fIntersectionTime = NULL, hkvVec3* HKV_RESTRICT out_IntersectionPoint = NULL) const;

  /// \brief
  ///   Computes the intersection of a line segment and the surface of the box.
  ///
  /// If the line segment's start and end point are both within the box, no intersection will be reported.
  /// If you need to handle these cases as intersections, test first, whether the box contains the start 
  /// position already.
  ///
  /// \param vLineStartPos
  ///   The start position of the line.
  ///   If the start position is inside the box, an intersection will only be detected, if the end point
  ///   is outside the box.
  ///
  /// \param vLineEndPos
  ///   The end position of the line.
  ///
  /// \param out_fHitFraction
  ///   Optional. If set, the fraction along the line, where the intersection point is located,
  ///   will be stored here. This value will always be between 0 and 1.\n
  ///   E.g: vLineStartPos + (vLineEndPos - vLineStartPos) * *out_fHitFraction == *out_IntersectionPoint\n
  ///   Or in other words:\n
  ///   hkvMath::interpolate (vLineStartPos, vLineEndPos, *out_fHitFraction) == *out_IntersectionPoint\n
  ///   If the line does not intersect with the box, this variable will not be modified.
  ///
  /// \param out_IntersectionPoint
  ///   Optional. If set, the intersection point will be stored here.
  ///   If the line does not intersect with the box, this variable will not be modified.\n
  ///   vLineStartPos + (vLineEndPos - vLineStartPos) * *out_fHitFraction == *out_IntersectionPoint\n
  ///
  /// \return
  ///   true if the line segment intersects with the box, false otherwise.
  ///
  /// \sa hkvAlignedBBox::getRayIntersection
  HKV_FORCE_INLINE bool getLineSegmentIntersection (const hkvVec3& vLineStartPos, const hkvVec3& vLineEndPos, float* HKV_RESTRICT out_fHitFraction = NULL, hkvVec3* HKV_RESTRICT out_IntersectionPoint = NULL) const;

  ///
  /// @}
  ///

  ///
  /// @name Utility Functions
  /// @{
  ///

  /// \brief
  ///   Returns v clamped to the volume of the box.
  ///
  /// If v is already inside the box, it will be returned unchanged.
  /// If v is outside the box, its components will be clamped to the min and max extents of the box.
  ///
  /// \param v
  ///   The point which shall be clamped to the box's volume.
  ///
  /// \sa hkvAlignedBBox::clampPositions
  HKV_FORCE_INLINE hkvVec3 getClampedPosition (const hkvVec3& v) const;

  /// \brief
  ///   Clamps a set of points to the volume of the box.
  ///
  /// If a point is already inside the box, it will not be modified.
  /// If a point is outside the box, its components will be clamped to the min and max extents of the box.
  ///
  /// \param inout_pPoints
  ///   Pointer to an array holding a set of points.
  ///
  /// \param uiNumPoints
  ///   Number of points that should be used from the array.
  ///
  /// \param uiStride
  ///   The stride (in bytes) between each point. Must be at least sizeof (hkvVec3) for a pure hkvVec3 array.
  ///   can be larger, if there is other data in between the points.
  ///
  /// \sa hkvAlignedBBox::getClampedPosition
  VBASE_IMPEXP void clampPositions (hkvVec3* HKV_RESTRICT inout_pPoints, hkUint32 uiNumPoints, hkUint32 uiStride = sizeof (hkvVec3)) const;

  ///
  /// @}
  ///

  ///
  /// @name Serialization
  /// @{
  ///

  /// \brief Serializes the BBox the way VBBoxf was serialized (6 floats). Prefer this for NEW code.
  HKV_FORCE_INLINE void SerializeAs_VBBoxf (VArchive& ar) { SerializeX (ar, *this); }

  /// \brief Serializes the BBox the way VisBoundingBox_cl was serialized (8 floats).
  VBASE_IMPEXP void SerializeAs_VisBoundingBox (VArchive& ar);

  ///
  /// @}
  ///

public:

  /// \brief
  ///   The minimum corner of the box.
  hkvVec3 m_vMin;

  /// \brief
  ///   The maximum corner of the box.
  hkvVec3 m_vMax;

public:

  ///
  /// @name Deprecated Functions
  /// @{
  ///

    /// \brief DEPRECATED: Use hkvPlane::getMinimumDistanceTo instead.
  HKVMATH_DEPRECATED_STAGE4 VBASE_IMPEXP float DetermineMinDistanceFromPlane(const hkvPlane& planeDist) const;

  /// \brief DEPRECATED: Use hkvPlane::getMinMaxDistanceTo instead.
  HKVMATH_DEPRECATED_STAGE4 VBASE_IMPEXP float DetermineMaxDistanceFromPlane(const hkvPlane& planeDist) const;

  /// \brief DEPRECATED: Use hkvAlignedBBox::getRayIntersection instead.
  HKVMATH_DEPRECATED_STAGE4 VBASE_IMPEXP bool IntersectsLine (const hkvVec3& startPos, const hkvVec3& endPos) const;

  /// \brief DEPRECATED: Use hkvAlignedBBox::getLineSegmentIntersection instead.
  HKVMATH_DEPRECATED_STAGE4 VBASE_IMPEXP bool IntersectsRay (const hkvVec3& startPos, const hkvVec3& endPos, hkvVec3& intersectionPos) const;
  
  /// \brief DEPRECATED: Use the code below instead (see hkvAlignedBBox.h)
  HKVMATH_DEPRECATED_STAGE4 HKV_FORCE_INLINE float GetDistanceSqrXY (const hkvVec3& pos) const
  {
    HKVBBOX_INITIALIZATION_CHECK(this);

    hkvVec2 clamped (hkvMath::Min (pos.x,m_vMax.x), hkvMath::Min (pos.y,m_vMax.y));
    clamped.x = hkvMath::Max (clamped.x, m_vMin.x) - pos.x;
    clamped.y = hkvMath::Max (clamped.y, m_vMin.y) - pos.y;
    return clamped.x*clamped.x + clamped.y*clamped.y;

  }

  /// \brief DEPRECATED: Use the code below instead (see hkvAlignedBBox.h)
  HKVMATH_DEPRECATED_STAGE4 HKV_FORCE_INLINE float GetDistanceXY (const hkvVec3& pos) const
  {
    HKVBBOX_INITIALIZATION_CHECK(this);

    hkvVec2 clamped (hkvMath::Min (pos.x,m_vMax.x), hkvMath::Min (pos.y,m_vMax.y));
    clamped.x = hkvMath::Max (clamped.x, m_vMin.x) - pos.x;
    clamped.y = hkvMath::Max (clamped.y, m_vMin.y) - pos.y;
    const float fSqr = clamped.x*clamped.x + clamped.y*clamped.y;

    return hkvMath::sqrt (fSqr);
  }

  /// \brief DEPRECATED: Use expandToInclude instead.
  HKVMATH_DEPRECATED_STAGE4 VBASE_IMPEXP void Inflate (const hkvVec3& vOrigin, const hkvVec3& vDir, const hkvVec3& vRight, const hkvVec3& vUp, float fFOVX, float fFOVY, float fNearClip, float fFarClip);
  
  ///
  /// @}
  ///
  
};

// Currently deactivated to force people to use SerializeAs_VBBoxf or SerializeAs_VisBoundingBox instead.
// Will be activated in some later release.
V_DECLARE_SERIALX_NONINTRUSIVE (hkvAlignedBBox, VBASE_IMPEXP);

/// \brief
///   Checks whether the two boxes are identical.
HKV_FORCE_INLINE bool operator== (const hkvAlignedBBox& lhs, const hkvAlignedBBox& rhs);

/// \brief
///   Checks whether the two boxes are not identical.
HKV_FORCE_INLINE bool operator!= (const hkvAlignedBBox& lhs, const hkvAlignedBBox& rhs);



#include <Vision/Runtime/Base/Math/BoundingVolume/hkvAlignedBBox.inl>

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
