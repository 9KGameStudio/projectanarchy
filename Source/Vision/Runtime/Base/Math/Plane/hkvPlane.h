/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VBASE_HKVMATH_HKVPLANE_H
#define VBASE_HKVMATH_HKVPLANE_H

#include <Vision/Runtime/Base/Math/hkvMath.h>


#ifdef HKVPLANE_CHECK_FOR_NAN
  #define HKVPLANE_INITIALIZATION_CHECK(obj) (obj)->isInitializedCheck()
#else
  #define HKVPLANE_INITIALIZATION_CHECK(obj) 
#endif

/// \brief Serializes the plane to/from a VArchive.
VBASE_IMPEXP void SerializeX (VArchive& ar, hkvPlane& obj);

/// \brief This struct is used to describe on which side of a plane a vertex or object is located.
struct hkvPlanePosition
{
  /// \brief The struct hkvPlanePosition acts as a namespace for this enum.
  enum Enum
  {
    Back,      ///< A point / object is completely on the negative side of the plane
    Coplanar,  ///< A point / object is completely coplanar to the plane (possibly within a given epsilon)
    Front,     ///< A point / object is completely on the positive side of the plane
    Spanning,  ///< An object spans the positive and negative side of the plane
  };
};

/// \brief DEPRECATED
enum {CLASS_FRONT=0,CLASS_BACK,CLASS_COPLANAR,CLASS_SPANNING};

/// \brief This struct describes the orientation of a triangle or polygon, e.g. which side is its front and which its back.
struct hkvTriangleOrientation
{
  /// \brief The struct hkvTriangleOrientation acts as a namespace for this enum.
  enum Enum
  {
    ClockWise,        ///! When looking at a triangle such that the vertices appear clockwise, that side is considered to be the 'front'
    CounterClockWise, ///! When looking at a triangle such that the vertices appear clockwise, that side is considered to be the 'back'
  };
};

/// \brief This class represents a mathematical plane.
///
/// The plane is stored as a normal and a (negated) distance to the origin.
/// That means for each point on the plane the following equation holds true:\n
/// Point.x * PlaneNormal.x + Point.y * PlaneNormal.y + Point.z + PlaneNormal.z + NegPlaneDistance == 0\n
/// 
/// In other words:\n
/// PlaneNormal.dot (Point) + PlaneDistance == 0\n
class hkvPlane
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
  HKV_FORCE_INLINE hkvPlane ()
  {
    #ifdef HKVPLANE_INITIALIZE_TO_NAN

      m_vNormal.set(hkvMath::generateNaN());
      m_fNegDist = hkvMath::generateNaN();

    #elif defined(HKVPLANE_INITIALIZE_TO_IDENTITY)

      m_vNormal.setZero();
      m_fNegDist = 0.0f;

    #endif
  }

  #ifdef HKVPLANE_CHECK_FOR_NAN
    HKV_FORCE_INLINE void isInitializedCheck() const
    {
      VASSERT_MSG(!hkvMath::isNaN (m_vNormal.x) && !hkvMath::isNaN (m_vNormal.y) && !hkvMath::isNaN (m_vNormal.z) && !hkvMath::isNaN (m_fNegDist), 
        "This object has invalid (NaN) members: (%.2f | %.2f | %.2f | %.2f).\nThis happens when you use this object without properly initializing it first, as the default constructor will set all members to NaN in debug builds.", m_vNormal.x, m_vNormal.y, m_vNormal.z, m_fNegDist);
    }
  #endif

  /// \brief
  ///   Does not initialize this object. Prefer this constructor over the default constructor.
  ///
  /// \param init
  ///   Set this to hkvNoInitialization to actually select this constructor.
  HKV_FORCE_INLINE explicit hkvPlane (hkvInit_None init /* = hkvNoInitialization */) { }

  // this constructor is evil, because we do not see where the distance is passed through but would need to be negated
  // we can enable this constructor once all the legacy code has been removed, but at the moment it mustn't be available!
  //HKV_FORCE_INLINE hkvPlane (const hkvVec3& vNormal, float fDistance);

  ///
  /// @}
  ///

  ///
  /// @name Setting Data
  /// @{
  ///

  /// \brief
  ///   Sets the plane with the given normal, computes the distance through the point on the plane.
  ///
  /// \note
  ///   Asserts if vNormal is not normalized.
  ///
  /// \param vPointOnPlane
  ///   Some arbitrary point on the plane's surface.
  ///
  /// \param vNormal
  ///   The normal of the plane.
  HKV_FORCE_INLINE void setFromPointAndNormal (const hkvVec3& vPointOnPlane, const hkvVec3& vNormal);

  /// \brief
  ///   Sets the plane from the points given through an array.
  ///
  /// \param pVertices
  ///   Pointer to an array of points. Exactly 3 points are used from that array, although it may contain more than that.
  ///
  /// \param ori
  ///   Describes whether the points should be considered to be in clock-wise or counter-clock-wise order.
  ///
  /// \param uiStride
  ///   The stride (in bytes) between the points in the pVertices array. Must be at least sizeof (hkvVec3) for a densely packed array.
  HKV_FORCE_INLINE hkvResult setFromPoints (const hkvVec3* pVertices, hkvTriangleOrientation::Enum ori, hkUint32 uiStride = sizeof (hkvVec3));

  /// \brief
  ///   Computes the plane's normal and distance from three points lying on the plane.
  ///
  /// Might result in a degenerate result if the points are located on a line or even a point.
  ///
  /// \param v0
  ///   Some point on the plane. Must be different from v1 and v2.
  ///
  /// \param v1
  ///   Some point on the plane. Must be different from v0 and v2.
  ///
  /// \param v2
  ///   Some point on the plane. Must be different from v0 and v1.
  ///
  /// \param ori
  ///   Describes whether the points should be considered to be in clock-wise or counter-clock-wise order.
  ///
  /// \sa hkvPlane::setFromDirections
  HKV_FORCE_INLINE hkvResult setFromPoints (const hkvVec3& v0, const hkvVec3& v1, const hkvVec3& v2, hkvTriangleOrientation::Enum ori);

  /// \brief
  ///   Creates this plane from two direction vectors that lie in the plane and a point on its surface.
  ///
  /// The normal of the plane is the cross product between vTangent1 and vTangent2. That means the plane can be flipped around
  /// by swapping the two tangent vectors. Another way to ensure the orientation of the plane, is to call hkvPlane::flipIfNecessary
  /// with a point that is known to be on the front of the plane.
  ///
  /// \param vTangent1
  ///   A direction vector that is orthogonal to the plane's normal. Must be different from vTangent2.
  ///
  /// \param vTangent2
  ///   A direction vector that is orthogonal to the plane's normal. Must be different from vTangent1.
  ///
  /// \param vPointOnPlane
  ///   Some arbitrary point on the plane's surface.
  ///
  /// \sa hkvPlane::setFromPoints
  /// \sa hkvPlane::flipIfNecessary
  HKV_FORCE_INLINE hkvResult setFromDirections (const hkvVec3& vTangent1, const hkvVec3& vTangent2, const hkvVec3& vPointOnPlane);

  /// \brief
  ///   Sets the plane to all zero, thus making it 'invalid', because it has no proper normal anymore.
  ///
  /// \sa hkvPlane::isValid
  HKV_FORCE_INLINE void setInvalid ();


  /// \brief 
  ///   Sets the normal and the distance of the plane.
  ///
  /// The plane internally stores the distance to the origin in negated form, so this function will negate the distance value before passing it on.
  /// This function may get deprecated at some point in the future.
  ///
  /// \param x
  ///   The x value of the normal.
  ///
  /// \param y
  ///   The y value of the normal.
  ///
  /// \param z
  ///   The z value of the normal.
  ///
  /// \param fDistance
  ///   The distance of the plane to the origin.
  ///
  /// \sa hkvPlane::setNormalAndNegativeDistance
  HKV_FORCE_INLINE void setNormalAndDistance (float x, float y, float z, float fDistance) { m_vNormal.set (x, y, z); m_fNegDist = -fDistance; }


  /// \brief 
  ///   Sets the normal and the negative distance of the plane.
  ///
  /// The plane internally stores the distance to the origin in negated form, so this function will just pass the values through unmodified.
  /// This function may get deprecated at some point in the future.
  ///
  /// \param x
  ///   The x value of the normal.
  ///
  /// \param y
  ///   The y value of the normal.
  ///
  /// \param z
  ///   The z value of the normal.
  ///
  /// \param fNegDist
  ///   The negated distance of the plane to the origin.
  ///
  /// \sa hkvPlane::setNormalAndDistance
  HKV_FORCE_INLINE void setNormalAndNegativeDistance (float x, float y, float z, float fNegDist) { m_vNormal.set (x, y, z); m_fNegDist = fNegDist; }

  ///
  /// @}
  ///

  ///
  /// @name Checks
  /// @{
  ///

  /// \brief
  ///   Returns whether two planes are identical.
  HKV_FORCE_INLINE bool isIdentical (const hkvPlane& rhs) const;

  /// \brief
  ///   Returns whether two planes are equal within some epsilon range.
  HKV_FORCE_INLINE bool isEqual (const hkvPlane& rhs, float fEpsilon) const;

  /// \brief
  ///   Returns whether the plane has a valid (non-zero, normalized) normal.
  ///
  /// \sa hkvPlane::setInvalid
  HKV_FORCE_INLINE bool isValid () const;

  ///
  /// @}
  ///

  ///
  /// @name Modification
  /// @{
  ///

  /// \brief
  ///   Flips the planes orientation around.
  ///
  /// That means the normal and the distance value get negated.
  ///
  /// \sa hkvPlane::flipIfNecessary
  HKV_FORCE_INLINE void flip ();

  /// \brief
  ///   If necessary the plane is flipped around to ensure that the given point is on its correct (positive or negative) side.
  ///
  /// \param vPoint
  ///   The point that should be either on the positive or negative side of the plane.
  ///
  /// \param bPointShouldBeOnPositiveSide
  ///   If true, vPoint is supposed to be on the positive side of the plane.
  ///   If false, vPoint is supposed to be on the negative side of the plane.
  ///   That means, if it is currently on the other side, the plane will be flipped around.
  ///
  /// \return
  ///   true if the plane was flipped to ensure that the point is on the correct side.
  ///   false, if it was not necessary to flip the plane around.
  ///
  /// \sa hkvPlane::flip
  HKV_FORCE_INLINE bool flipIfNecessary (const hkvVec3& vPoint, bool bPointShouldBeOnPositiveSide = true);

  /// \brief
  ///   Transforms the plane by the given matrix.
  HKV_FORCE_INLINE void transform (const hkvMat3& m);

  /// \brief
  ///   Transforms the plane by the given matrix.
  HKV_FORCE_INLINE void transform (const hkvMat4& m);

  /// \brief
  ///   Returns a transformed copy of this plane.
  HKV_FORCE_INLINE const hkvPlane getTransformed (const hkvMat3& m) const;

  /// \brief
  ///   Returns a transformed copy of this plane.
  HKV_FORCE_INLINE const hkvPlane getTransformed (const hkvMat4& m) const;

  ///
  /// @}
  ///

  ///
  /// @name Distance Computations
  /// @{
  ///


  /// \brief
  ///   Returns the distance of the point to the plane.
  ///
  /// A positive distance means the point is in front of the plane.
  /// A negative distance means the point is behind the plane.
  /// A distance of zero means the point is exactly on the plane.
  ///
  /// \param vPoint
  ///   The point for which to compute the distance.
  ///
  /// \sa hkvPlane::getPointPosition
  HKV_FORCE_INLINE float getDistanceTo (const hkvVec3& vPoint) const;

  /// \brief
  ///   Returns the minimum distance of any of the points to the plane. The result may be negative if any point is behind the plane.
  ///
  /// \param pPoints
  ///   Pointer to an array of points.
  ///
  /// \param uiNumPoints
  ///   The number of points to use from the array.
  ///
  /// \param uiStride
  ///   The stride (in bytes) between the points in the array. Must be at least sizeof (hkvVec3)
  ///   for densely packed arrays.
  ///
  /// \return
  ///   The minimum distance that any of the given points has to the plane.
  ///
  /// \sa hkvPlane::getMinMaxDistanceTo
  HKV_FORCE_INLINE float getMinimumDistanceTo (const hkvVec3* pPoints, hkUint32 uiNumPoints, hkUint32 uiStride = sizeof (hkvVec3)) const;

  /// \brief
  ///   Returns the minimum and maximum distance any of the given points has to the plane.
  ///
  /// \param out_fMin
  ///   The minimum distance of any of the points to the plane.
  ///
  /// \param out_fMax
  ///   The maximum distance of any of the points to the plane.
  ///
  /// \param pPoints
  ///   Pointer to an array of points.
  ///
  /// \param uiNumPoints
  ///   The number of points to use from the array.
  ///
  /// \param uiStride
  ///   The stride (in bytes) between the points in the array. Must be at least sizeof (hkvVec3)
  ///   for densely packed arrays.
  HKV_FORCE_INLINE void getMinMaxDistanceTo (float &out_fMin, float &out_fMax, const hkvVec3* pPoints, hkUint32 uiNumPoints, hkUint32 uiStride = sizeof (hkvVec3)) const;

  ///
  /// @}
  ///

  ///
  /// @name Position Checks
  /// @{
  ///


  /// \brief
  ///   Returns in which half-space the point lies. Can also be 'on' the plane.
  HKV_FORCE_INLINE hkvPlanePosition::Enum getPointPosition (const hkvVec3& vPoint) const;

  /// \brief
  ///   Returns in which half-space the point lies. Can also be 'on' the plane.
  ///
  /// \param vPoint
  ///   The point for which the check is done.
  ///
  /// \param fPlaneHalfThickness
  ///   The epsilon distance value for which a point is considered to be 'on' the plane.
  ///   That means if the absolute distance of the point to the plane is less than fPlaneHalfThickness,
  ///   hkvPlanePosition::Coplanar is returned.
  ///
  /// \return
  ///   hkvPlanePosition::Back, hkvPlanePosition::Coplanar or hkvPlanePosition::Front
  ///
  /// \sa hkvPlane::getObjectPosition
  HKV_FORCE_INLINE hkvPlanePosition::Enum getPointPosition (const hkvVec3& vPoint, float fPlaneHalfThickness) const;

  /// \brief
  ///   Checks on which side(s) of the plane a set of points is located.
  ///
  /// \param pPoints
  ///   Pointer to an array of points.
  ///
  /// \param uiNumPoints
  ///   The number of points to use from the array.
  ///
  /// \param uiStride
  ///   The stride (in bytes) between the points in the array. Must be at least sizeof (hkvVec3)
  ///   for densely packed arrays.
  ///
  /// \return
  ///   If some points are in front, others are on the back, hkvPlanePosition::Spanning is returned.\n
  ///   If some points are in front and the rest is 'on' the plane, hkvPlanePosition::Front is returned.\n
  ///   If some points are on the back and the rest is 'on' the plane, hkvPlanePosition::Back is returned.\n
  ///   If all points are 'on' the plane hkvPlanePosition::Coplanar is returned.
  ///
  /// \sa hkvPlane::getPointPosition
  HKV_FORCE_INLINE hkvPlanePosition::Enum getObjectPosition (const hkvVec3* pPoints, hkUint32 uiNumPoints, hkUint32 uiStride = sizeof (hkvVec3)) const;

  /// \brief
  ///   Checks on which side(s) of the plane a set of points is located.
  ///
  /// \param pPoints
  ///   Pointer to an array of points.
  ///
  /// \param uiNumPoints
  ///   The number of points to use from the array.
  ///
  /// \param fPlaneHalfThickness
  ///   All points whose absolute distance to the plane is less than this value are considered
  ///   to be 'on' the plane.
  ///   That means an object can be classified to be 'in front' of a plane, even if some points
  ///   are actually behind the plane (and vice versa), if their distance to the plane falls into
  ///   this epsilon range.
  ///
  /// \param uiStride
  ///   The stride (in bytes) between the points in the array. Must be at least sizeof (hkvVec3)
  ///   for densely packed arrays.
  ///
  /// \return
  ///   If some points are in front, others are on the back, hkvPlanePosition::Spanning is returned.\n
  ///   If some points are in front and the rest is 'on' the plane, hkvPlanePosition::Front is returned.\n
  ///   If some points are on the back and the rest is 'on' the plane, hkvPlanePosition::Back is returned.\n
  ///   If all points are 'on' the plane hkvPlanePosition::Coplanar is returned.
  ///
  /// \sa hkvPlane::getPointPosition
  HKV_FORCE_INLINE hkvPlanePosition::Enum getObjectPosition (const hkvVec3* pPoints, hkUint32 uiNumPoints, float fPlaneHalfThickness, hkUint32 uiStride = sizeof (hkvVec3)) const;

  /// \brief
  ///   Checks on which side(s) of the plane the box is located.
  ///
  /// \param aabb
  ///   The box for which to check the position.
  ///
  /// \return
  ///   hkvPlanePosition::Front, hkvPlanePosition::Back or hkvPlanePosition::Spanning
  ///
  /// \sa hkvPlanePosition
  HKV_FORCE_INLINE hkvPlanePosition::Enum getObjectPosition (const hkvAlignedBBox& aabb) const;

  /// \brief
  ///   Checks on which side(s) of te plane the sphere is located.
  ///
  /// \param sphere
  ///   The sphere for which to check the position.
  ///
  /// \return
  ///   hkvPlanePosition::Front, hkvPlanePosition::Back or hkvPlanePosition::Spanning
  ///
  /// \sa hkvPlanePosition
  HKV_FORCE_INLINE hkvPlanePosition::Enum getObjectPosition (const hkvBoundingSphere& sphere) const;


  ///
  /// @}
  ///

  ///
  /// @name Intersection Tests
  /// @{
  ///


  /// \brief
  ///   Checks whether the given ray intersects the plane and computes the intersection time and point.
  ///
  /// The ray will only miss the plane, if it points away from it. It does not matter whether the ray
  /// starts on the positive or negative side of the plane (ie. there is no such thing as 'back-face culling').
  /// If you need to ignore plane intersections with planes where the ray starts on the negative side,
  /// just do that check manually via getPointPosition.
  ///
  /// \param vRayStartPos
  ///   The start position of the ray.
  ///
  /// \param vRayDir
  ///   The direction of the ray. May be normalized but is not required.
  ///
  /// \param out_fIntersectionTime
  ///   Optional. If set, the 'intersection-time' will be stored here.
  ///   The intersection time will always be >= 0.
  ///   If the ray misses the plane, this value will not be modified.
  ///   vRayStartPos + vRayDir * *out_fIntersectionTime == *out_IntersectionPoint
  ///   That means the intersection time scales anti-proportionally to the length of vRayDir.
  ///
  /// \param out_IntersectionPoint
  ///   Optional. If set, the point of intersection will be stored here.
  ///   If the ray misses the plane, this value will not be modified.
  ///   vRayStartPos + vRayDir * *out_fIntersectionTime == *out_IntersectionPoint
  ///
  /// \return
  ///   True if the ray hits the plane, false otherwise.
  ///
  /// \sa hkvPlane::getPointPosition
  /// \sa hkvPlane::getRayIntersectionBiDirectional
  /// \sa hkvPlane::getLineSegmentIntersection
  HKV_FORCE_INLINE bool getRayIntersection (const hkvVec3& vRayStartPos, const hkvVec3& vRayDir, float* out_fIntersectionTime = NULL, hkvVec3* out_IntersectionPoint = NULL) const;

  /// \brief
  ///   Does the same as hkvPlane::getRayIntersection but also returns negative intersection points ('behind' the start position).
  ///
  /// The ray will only miss the plane, if it is exactly parallel to the plane. It does not matter whether the ray
  /// starts on the positive or negative side of the plane (ie. there is no such thing as 'back-face culling').
  /// If you need to ignore plane intersections with planes where the ray starts on the negative side,
  /// just do that check manually via getPointPosition.
  ///
  /// \param vRayStartPos
  ///   The start position of the ray.
  ///
  /// \param vRayDir
  ///   The direction of the ray. May be normalized but is not required.
  ///
  /// \param out_fIntersectionTime
  ///   Optional. If set, the 'intersection-time' will be stored here.
  ///   Contrary to hkvPlane::getRayIntersection this value can be positive or negative.
  ///   Ie. If the ray points away from the plane, the intersection point will still be computed,
  ///   though it is located along the negative direction.
  ///   If the ray misses the plane, this value will not be modified.
  ///   vRayStartPos + vRayDir * *out_fIntersectionTime == *out_IntersectionPoint
  ///   That means the intersection time scales anti-proportionally to the length of vRayDir.
  ///
  /// \param out_IntersectionPoint
  ///   Optional. If set, the point of intersection will be stored here.
  ///   If the ray misses the plane, this value will not be modified.
  ///   vRayStartPos + vRayDir * *out_fIntersectionTime == *out_IntersectionPoint
  ///
  /// \return
  ///   True if the ray hits the plane, false otherwise.
  ///
  /// \sa hkvPlane::getPointPosition
  /// \sa hkvPlane::getRayIntersection
  /// \sa hkvPlane::getLineSegmentIntersection
  HKV_FORCE_INLINE bool getRayIntersectionBiDirectional (const hkvVec3& vRayStartPos, const hkvVec3& vRayDir, float* out_fIntersectionTime = NULL, hkvVec3* out_IntersectionPoint = NULL) const;


  /// \brief
  ///   Computes the intersection of a line segment and the plane.
  ///
  /// The line will only intersect the plane, if vLineStartPos is on another side of the plane
  /// than vLineEndPos. It does not matter whether the start point is on the positive or negative side
  /// of the plane, ie. there is no such thing as 'back-face culling'.
  ///
  /// \param vLineStartPos
  ///   The start position of the line.
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
  ///   If the line does not intersect with the plane, this variable will not be modified.
  ///
  /// \param out_IntersectionPoint
  ///   Optional. If set, the intersection point will be stored here.
  ///   If the line does not intersect with the plane, this variable will not be modified.\n
  ///   vLineStartPos + (vLineEndPos - vLineStartPos) * *out_fHitFraction == *out_IntersectionPoint\n
  ///
  /// \return
  ///   true if the line segment intersects with the plane, false otherwise.
  ///
  /// \sa hkvPlane::getRayIntersection
  /// \sa hkvPlane::getRayIntersectionBiDirectional
  HKV_FORCE_INLINE bool getLineSegmentIntersection (const hkvVec3& vLineStartPos, const hkvVec3& vLineEndPos, float* out_fHitFraction = NULL, hkvVec3* out_IntersectionPoint = NULL) const;

  /// \brief
  ///   Returns the point at which all three planes intersect.
  ///
  /// \param p0
  ///   Some arbitrary plane. Should be different from p1 and p2.
  ///
  /// \param p1
  ///   Some arbitrary plane. Should be different from p0 and p2.
  ///
  /// \param p2
  ///   Some arbitrary plane. Should be different from p0 and p1.
  ///
  /// \param out_Result
  ///   The variable in which the result is written, if there is one single point of intersection.
  ///
  /// \return
  ///   true, if there is a single point in which all three planes intersect.
  ///   false, if the planes are arranged in a way that there is no intersection or an infinite number of intersection points.
  HKV_FORCE_INLINE static hkvResult get3PlaneIntersectionPoint (const hkvPlane& p0, const hkvPlane& p1, const hkvPlane& p2, hkvVec3& out_Result);

  ///
  /// @}
  ///

  ///
  /// @name Projections
  /// @{
  ///


  /// \brief
  ///   The given point is projected along the plane's normal onto the plane.
  HKV_FORCE_INLINE void projectOntoPlane (hkvVec3& inout_vPoint) const;

  /// \brief
  ///   The given points are projected along the plane's normal onto the plane.
  ///
  /// \param inout_vPoints
  ///   Pointer to an array of points. The points will be read and modified in place.
  ///
  /// \param uiNumPoints
  ///   The number of points to use from the array.
  ///
  /// \param uiStride
  ///   The stride (in bytes) between the points in the array. Must be at least sizeof (hkvVec3)
  ///   for densely packed arrays.
  HKV_FORCE_INLINE void projectOntoPlane (hkvVec3* inout_vPoints, hkUint32 uiNumPoints, hkUint32 uiStride = sizeof (hkvVec3)) const;

  /// \brief
  ///   The direction vector is adjusted to be coplanar to the plane.
  ///
  /// That means afterwards the adjusted vector is orthogonal to the plane normal.
  /// The vector is only made coplanar, it is not normalized.
  ///
  /// \param inout_vDirection
  ///   The vector that should be made coplanar to the plane. Will be modified in place.
  HKV_FORCE_INLINE void makeDirectionCoplanar (hkvVec3& inout_vDirection) const;

  /// \brief
  ///   The direction vectors are adjusted to be coplanar to the plane.
  ///
  /// That means afterwards the adjusted vectors are orthogonal to the plane normal.
  /// The vectors are only made coplanar, they are not normalized.
  ///
  /// \param inout_vDirections
  ///   Pointer to an array of points that should be made coplanar to the plane. Will be modified in place.
  ///
  /// \param uiNumPoints
  ///   The number of points to use from the array.
  ///
  /// \param uiStride
  ///   The stride (in bytes) between the points in the array. Must be at least sizeof (hkvVec3)
  ///   for densely packed arrays.
  HKV_FORCE_INLINE void makeDirectionCoplanar (hkvVec3* inout_vDirections, hkUint32 uiNumPoints, hkUint32 uiStride = sizeof (hkvVec3)) const;

  ///
  /// @}
  ///


  ///
  /// @name Utility Functions
  /// @{
  ///

  /// \brief 
  ///   Returns three different points that are located on the plane.
  HKV_FORCE_INLINE void getPoints (hkvVec3& out_v0, hkvVec3& out_v1, hkvVec3& out_v2) const;


  /// \brief
  ///   Returns the index of the least significant component of the planes normal.
  ///
  /// The least significant component of the plane is the normal vector's component with the largest absolute value.
  ///
  /// \return
  ///   [0;2] : Index for the x,y,z component.
  HKV_FORCE_INLINE hkUint32 GetLeastSignificantComponent () const;


  /// \brief
  ///   Will return the point mirrored at the plane.
  ///
  /// The mirrored point is the point on the other side of the plane, but at the same (absolute) distance to the plane.
  ///
  /// \param v
  ///   The point that should be mirrored.
  ///
  /// \return
  ///   (v - 2 * getDistanceTo (v) * m_vNormal)
  ///
  /// \sa hkvPlane::getDistanceTo
  HKV_FORCE_INLINE const hkvVec3 getMirrored (const hkvVec3& v) const;

  ///
  /// @}
  ///


  ///
  /// @name Serialization
  /// @{
  ///

  /// \brief Serializes the plane like a VisPlane_cl. Do not use this directly, use 'SerializeX' or operator>> / operator<< instead.
  HKV_FORCE_INLINE void SerializeAs_VisPlane (VArchive& ar) { SerializeX (ar, *this); }

  /// \brief Serializes the plane like a VisPlane_cl. Do not use this directly, use 'SerializeX' or operator>> / operator<< instead.
  VBASE_IMPEXP void SerializeAs_VPlanef (VArchive& ar);

  ///
  /// @}
  ///


public:

  /// \brief
  ///   The normal of the plane.
  hkvVec3 m_vNormal;

  /// \brief
  ///   The negative distance of the plane to the origin.
  ///
  /// Therefore for every point on the plane holds:\n
  /// vPtOnPlane.dot (m_vNormal) + m_fNegDist == 0
  float m_fNegDist;

};

// Currently deactivated to force people to use SerializeAs_VBBoxf or SerializeAs_VisBoundingBox instead.
// Will be activated in some later release.
V_DECLARE_SERIALX_NONINTRUSIVE (hkvPlane, VBASE_IMPEXP);

/// \brief
///   Compares to planes for exact equality (no epsilon).
bool operator== (const hkvPlane& p0, const hkvPlane& p1);

/// \brief
///   Compares to planes for exact inequality (no epsilon).
bool operator!= (const hkvPlane& p0, const hkvPlane& p1);

#include <Vision/Runtime/Base/Math/Plane/hkvPlane.inl>

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
