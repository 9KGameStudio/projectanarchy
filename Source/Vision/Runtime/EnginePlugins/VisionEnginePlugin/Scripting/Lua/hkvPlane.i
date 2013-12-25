/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VLUA_APIDOC

class hkvPlane
{
public:

 %extend 
  {
    hkvPlane()
    {
      hkvPlane *pNew = new hkvPlane(hkvNoInitialization);
      pNew->setInvalid();
      return pNew;
    }

    hkvPlane(float fNormalX, float fNormalY, float fNormalZ, float fNegDist)
    {
      hkvPlane *pPlane = new hkvPlane(hkvNoInitialization);
      pPlane->setNormalAndNegativeDistance(fNormalX, fNormalY, fNormalZ , fNegDist);
      return pPlane;
    }

    hkvPlane(const hkvPlane& rhs)
    {
      return new hkvPlane(rhs);
    } 
  }

  %extend 
  {
    void setFromPointAndNormal(const hkvVec3& vPointOnPlane, const hkvVec3& vNormal)
    {
      self->setFromPointAndNormal(vPointOnPlane, vNormal.getNormalized());
    }

    bool setFromPoints(const hkvVec3& v0, const hkvVec3& v1, const hkvVec3& v2, bool bClockWise)
    {
      return self->setFromPoints(v0, v1, v2, bClockWise ? hkvTriangleOrientation::ClockWise : hkvTriangleOrientation::CounterClockWise)==HKV_SUCCESS;
    }

    bool setFromDirections(const hkvVec3& vTangent1, const hkvVec3& vTangent2, const hkvVec3& vPointOnPlane)
    {
      return self->setFromDirections(vTangent1, vTangent2, vPointOnPlane)==HKV_SUCCESS;
    }
  }

  void setInvalid();

  bool isIdentical(const hkvPlane& rhs) const;
  bool isEqual(const hkvPlane& rhs, float fEpsilon) const;
  bool isValid() const; 

  void flip();
  bool flipIfNecessary(const hkvVec3& vPoint, bool bPointShouldBeOnPositiveSide = true);
  
  void transform(const hkvMat3& m);
  void transform(const hkvMat4& m);

  const hkvPlane getTransformed(const hkvMat3& m) const;
  const hkvPlane getTransformed(const hkvMat4& m) const;

  float getDistanceTo(const hkvVec3& vPoint) const;

  %extend 
  {
    int getPointPosition(const hkvVec3& vPoint) const
    {
      return (int)self->getPointPosition(vPoint);
    }

    int getPointPosition(const hkvVec3& vPoint, float fPlaneHalfThickness) const
    {
      return (int)self->getPointPosition(vPoint, fPlaneHalfThickness);
    } 

    int getObjectPosition(const hkvAlignedBBox& aabb) const
    {
      return (int)self->getObjectPosition(aabb);
    }

    int getObjectPosition(const hkvBoundingSphere& sphere) const
    {
      return (int)self->getObjectPosition(sphere);
    } 

    hkvVec3 projectOntoPlane(const hkvVec3& vPoint) const
    {
      hkvVec3 vResult(vPoint);
      self->projectOntoPlane(vResult);
      return vResult;
    }

    hkvVec3 makeDirectionCoplanar(const hkvVec3& vDirection) const
    {
      hkvVec3 vResult(vDirection);
      self->makeDirectionCoplanar(vResult);
      return vResult;
    }
  }

  unsigned int GetLeastSignificantComponent() const;

  const hkvVec3 getMirrored(const hkvVec3& v) const;

  %extend 
  {
    hkvPlane clone() const
    {
      return *self; //will call the copy constructor
    }

    bool __eq(const hkvPlane* pRhs) const
    {
      if (pRhs == NULL) 
        return false;
      return (*pRhs)==(*self);
    }
  }

  hkvVec3 m_vNormal;
  float m_fNegDist;

  %extend
  {
    static const int PLANE_POSITION_BACK = hkvPlanePosition::Back;
    static const int PLANE_POSITION_COPLANAR = hkvPlanePosition::Coplanar;
    static const int PLANE_POSITION_FRONT = hkvPlanePosition::Front;
    static const int PLANE_POSITION_SPANNING = hkvPlanePosition::Spanning;
  }
};

%native(hkvPlane_getRayIntersection) int hkvPlane_getRayIntersection(lua_State *L);
%{
   SWIGINTERN int hkvPlane_getRayIntersection(lua_State *L)
   {
     IS_MEMBER_OF(hkvPlane) //this will move this function to the method table of the specified class
    
     SWIG_CONVERT_POINTER(L, 1, hkvPlane, pSelf)

     DECLARE_ARGS_OK

     // Get arguments
     GET_ARG(2, hkvVec3, vRayStartPos);
     GET_ARG(3, hkvVec3, vRayDir);      

     float fIntersectionTime;
     hkvVec3 vIntersectionPoint;
     bool bIntersect = pSelf->getRayIntersection(vRayStartPos, vRayDir, &fIntersectionTime, &vIntersectionPoint);

     // Clean stack (remove all call params including self)
     lua_settop(L, 0);   

     if (!bIntersect)
     {
       lua_pushboolean(L, false);
       return 1;
     }

     lua_pushboolean(L, true);
     lua_pushnumber(L, (lua_Number)fIntersectionTime); 
     LUA_PushObjectProxy(L, new hkvVec3(vIntersectionPoint));
                                      
     return 3;
   }
%} 

%native(hkvPlane_getRayIntersectionBiDirectional) int hkvPlane_getRayIntersectionBiDirectional(lua_State *L);
%{
   SWIGINTERN int hkvPlane_getRayIntersectionBiDirectional(lua_State *L)
   {
     IS_MEMBER_OF(hkvPlane) //this will move this function to the method table of the specified class
    
     SWIG_CONVERT_POINTER(L, 1, hkvPlane, pSelf)

     DECLARE_ARGS_OK

     // Get arguments
     GET_ARG(2, hkvVec3, vRayStartPos);
     GET_ARG(3, hkvVec3, vRayDir);      

     float fIntersectionTime;
     hkvVec3 vIntersectionPoint;
     bool bIntersect = pSelf->getRayIntersectionBiDirectional(vRayStartPos, vRayDir, &fIntersectionTime, &vIntersectionPoint);

     // Clean stack (remove all call params including self)
     lua_settop(L, 0);   

     if (!bIntersect)
     {
       lua_pushboolean(L, false);
       return 1;
     }

     lua_pushboolean(L, true);
     lua_pushnumber(L, (lua_Number)fIntersectionTime); 
     LUA_PushObjectProxy(L, new hkvVec3(vIntersectionPoint));
                                      
     return 3;
   }
%} 

%native(hkvPlane_getLineSegmentIntersection) int hkvPlane_getLineSegmentIntersection(lua_State *L);
%{
   SWIGINTERN int hkvPlane_getLineSegmentIntersection(lua_State *L)
   {
     IS_MEMBER_OF(hkvPlane) //this will move this function to the method table of the specified class
    
     SWIG_CONVERT_POINTER(L, 1, hkvPlane, pSelf)

     DECLARE_ARGS_OK

     // Get arguments
     GET_ARG(2, hkvVec3, vLineStartPos);
     GET_ARG(3, hkvVec3, vLineEndPos);      
      
     float fHitFraction;
     hkvVec3 vIntersectionPoint;
     bool bIntersect = pSelf->getLineSegmentIntersection(vLineStartPos, vLineEndPos, &fHitFraction, &vIntersectionPoint);

     // Clean stack (remove all call params including self)
     lua_settop(L, 0);   

     if (!bIntersect)
     {
       lua_pushboolean(L, false);
       return 1;
     }

     lua_pushboolean(L, true);
     lua_pushnumber(L, (lua_Number)fHitFraction); 
     LUA_PushObjectProxy(L, new hkvVec3(vIntersectionPoint));
                                      
     return 3;
   }
%}

//add lua tostring and concat operators
VSWIG_CREATE_CONCAT(hkvPlane, 128, "[%1.2f,%1.2f,%1.2f][%1.2f]", self->m_vNormal.x, self->m_vNormal.y, self->m_vNormal.z, self->m_fNegDist)
VSWIG_CREATE_TOSTRING(hkvPlane, "hkvPlane: normal[%1.2f,%1.2f,%1.2f], negatedDistance[%1.2f]", self->m_vNormal.x, self->m_vNormal.y, self->m_vNormal.z, self->m_fNegDist)

#else

/// \brief 
///   Wrapper class for a mathematical plane.
///
/// \note 
///   Since all wrappers classes are handled as pointers in Lua you cannot copy its content by assignment. 
///   Please take a look at the code samples.
///
/// \par Example
///   \code
///     -- create a new plane:
///     local plane1 = Vision.hkvPlane()
///
///     -- setup plane
///     plane:setFromPointAndNormal(Vision.hkvVe3(100, 40, -300), Vision.hkvVec3(0, 1, 0))
///
///     -- this will not copy the planes content (just another pointer to it):
///     local plane2 = plane1
///
///     -- this will affect access to plane1 and plane2 since the pointers share the same object:
///     plane.negDist = 10
///     -- this assertion will not fail
///     Vision.Assert(plane1 == plane2, "When this assertions fails plane1 and plane2 are pointing to different planes.")
///
///     -- in order to copy the content to another plane, you have to clone:
///     local plane3 = plane1:clone()
///
///     -- or create a new plane using the copy constructor
///     local plane4 = Vision.hkvPlane(plane1)
///   \endcode
/// \see clone
class hkvPlane
{
public:

  /// @name Initialization
  /// @{

  /// \brief 
  ///   Default Constructor. Initializes plane with all zero (invalid plane).
  hkvPlane();

  /// \brief 
  ///   Copy Constructor.
  ///
  /// \param other 
  ///   The plane to copy.
  hkvPlane(hkvPlane other);

  /// \brief 
	///   Constructor initializing the plane with the specified values.
	///
  /// The plane internally stores the distance to the origin in negated form, so this function will just pass the values through unmodified.
  ///
  /// \param normalX
	///   X coordinate of the plane normal.
	///
  /// \param normalY
	///   Y coordinate of the plane normal.
	///
  /// \param normalZ
	///   Z coordinate of the plane normal.
  ///
  /// \param negDist
  ///   The negated distance of the plane to the origin.
  hkvPlane(number normalX, number normalY, number normalZ, number negDist);

  /// @}
  /// @name Creating Planes
  /// @{

  /// \brief
  ///   Sets the plane with the given normal, computes the distance through the point on the plane.
  ///
  /// \param pointOnPlane
  ///   Some arbitrary point on the plane's surface.
  ///
  /// \param normal
  ///   The normal of the plane. Normal gets internally normalized (\b always).
  void setFromPointAndNormal(hkvVec3 pointOnPlane, hkvVec3 normal);

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
  /// \param clockwise
  ///   Set to true when the points should be considered to be in clock-wise order, to false for counter-clock-wise order.
  ///
  /// \return
  ///   True on success, otherwise false.
  ///
  /// \sa hkvPlane::setFromDirections
  boolean setFromPoints(hkvVec3 v0, hkvVec3 v1, hkvVec3 v2, boolean clockwise);

  /// \brief
  ///   Creates this plane from two direction vectors that lie in the plane and a point on its surface.
  ///
  /// The normal of the plane is the cross product between vTangent1 and vTangent2. That means the plane can be flipped around
  /// by swapping the two tangent vectors. Another way to ensure the orientation of the plane, is to call hkvPlane::flipIfNecessary
  /// with a point that is known to be on the front of the plane.
  ///
  /// \param tangent1
  ///   A direction vector that is orthogonal to the plane's normal. Must be different from tangent2.
  ///
  /// \param tangent2
  ///   A direction vector that is orthogonal to the plane's normal. Must be different from tangent1.
  ///
  /// \param pointOnPlane
  ///   Some arbitrary point on the plane's surface.
  ///
  /// \return
  ///   True on success, otherwise false.
  ///
  /// \sa hkvPlane::setFromPoints
  ///
  /// \sa hkvPlane::flipIfNecessary
  boolean setFromDirections(hkvVec3 tangent1, hkvVec3 tangent2, hkvVec3 pointOnPlane);

  /// \brief
  ///   Sets the plane to all zero, thus making it 'invalid', because it has no proper normal anymore.
  ///
  /// \sa hkvPlane::isValid
  void setInvalid();

  /// @}
  /// @name Checks
  /// @{ 

  /// \brief
  ///   Returns whether two planes are identical.
  boolean isIdentical(hkvPlane other);

  /// \brief
  ///   Returns whether two planes are equal within some epsilon range.
  boolean isEqual(hkvPlane other, number epsilon);

  /// \brief
  ///   Returns whether the plane has a valid (non-zero, normalized) normal.
  ///
  /// \sa hkvPlane::setInvalid
  boolean isValid();

  /// @}
  /// @name Modification
  /// @{ 

  /// \brief
  ///   Flips the planes orientation around.
  ///
  /// That means the normal and the distance value get negated.
  ///
  /// \sa hkvPlane::flipIfNecessary
  void flip();

  /// \brief
  ///   If necessary the plane is flipped around to ensure that the given point is on its correct (positive or negative) side.
  ///
  /// \param point
  ///   The point that should be either on the positive or negative side of the plane.
  ///
  /// \param pointShouldBeOnPositiveSide
  ///   If true, point is supposed to be on the positive side of the plane.
  ///   If false, point is supposed to be on the negative side of the plane.
  ///   That means, if it is currently on the other side, the plane will be flipped around.
  ///
  /// \return
  ///   True if the plane was flipped to ensure that the point is on the correct side.
  ///   False, if it was not necessary to flip the plane around.
  ///
  /// \sa hkvPlane::flip
  boolean flipIfNecessary(hkvVec3 point, boolean pointShouldBeOnPositiveSide = true);

  /// \brief
  ///   Transforms the plane by the given 3x3 matrix.
  void transform(hkvMat3 m);

  /// \brief
  ///   Transforms the plane by the given 4x4 matrix.
  void transform(hkvMat4 m);

  /// \brief
  ///   Returns a transformed copy of this plane.
  hkvPlane getTransformed(hkvMat3 m);

  /// \brief
  ///   Returns a transformed copy of this plane.
  hkvPlane getTransformed(hkvMat4 m);

  /// @}
  /// @name Distance Computation
  /// @{ 

  /// \brief
  ///   Returns the distance of the point to the plane.
  ///
  /// A positive distance means the point is in front of the plane.
  /// A negative distance means the point is behind the plane.
  /// A distance of zero means the point is exactly on the plane.
  ///
  /// \param point
  ///   The point for which to compute the distance.
  ///
  /// \sa hkvPlane::getPointPosition
  number getDistanceTo(hkvVec3 point);

  /// @}
  /// @name Position Checks
  /// @{ 

  /// \brief
  ///   Returns in which half-space the point lies. Can also be 'on' the plane.
  ///  
  /// \param point
  ///   The point for which the check is done.
  ///
  /// \return
  ///   hkvPlane_PLANE_POSITION_BACK when point is on the negative side of the plane.
  ///   hkvPlane_PLANE_POSITION_COPLANAR when point lies on plane (without epsilon).
  ///   hkvPlane_PLANE_POSITION_FRONT when point is on the positive side of the plane.
  number getPointPosition(hkvVec3 point);

  /// \brief
  ///   Returns in which half-space the point lies. Can also be 'on' the plane.
  ///
  /// \param point
  ///   The point for which the check is done.
  ///
  /// \param planeHalfThickness
  ///   The epsilon distance value for which a point is considered to be 'on' the plane.
  ///   That means if the absolute distance of the point to the plane is less than fPlaneHalfThickness,
  ///   hkvPlane_PLANE_POSITION_COPLANAR is returned.
  ///
  /// \return
  ///   hkvPlane_PLANE_POSITION_BACK when point is on the negative side of the plane.
  ///   hkvPlane_PLANE_POSITION_COPLANAR when point lies on plane. 
  ///   hkvPlane_PLANE_POSITION_FRONT when point is on the positive side of the plane.
  ///
  /// \sa hkvPlane::getObjectPosition
  number getPointPosition(hkvVec3 point, number planeHalfThickness);

  /// \brief
  ///   Checks on which side(s) of the plane the box is located.
  ///
  /// \param aabb
  ///   The box for which to check the position.
  ///
  /// \return
  ///   hkvPlane_PLANE_POSITION_BACK when box is on the negative side of the plane.
  ///   hkvPlane_PLANE_POSITION_SPANNING when box spans the positive and negative side of the plane.
  ///   hkvPlane_PLANE_POSITION_FRONT when box is on the positive side of the plane.
  number getObjectPosition(hkvAlignedBBox aabb);

  /// \brief
  ///   Checks on which side(s) of te plane the sphere is located.
  ///
  /// \param sphere
  ///   The sphere for which to check the position.
  ///
  /// \return
  ///   hkvPlane_PLANE_POSITION_BACK when sphere is on the negative side of the plane.
  ///   hkvPlane_PLANE_POSITION_SPANNING when sphere spans the positive and negative side of the plane.
  ///   hkvPlane_PLANE_POSITION_FRONT when sphere is on the positive side of the plane.
  number getObjectPosition(hkvBoundingSphere sphere);

  /// @}
  /// @name Intersection Tests
  /// @{ 

  /// \brief
  ///   Checks whether the given ray intersects the plane and computes the intersection time and point.
  ///
  /// The ray will only miss the plane, if it points away from it. It does not matter whether the ray
  /// starts on the positive or negative side of the plane (ie. there is no such thing as 'back-face culling').
  /// If you need to ignore plane intersections with planes where the ray starts on the negative side,
  /// just do that check manually via getPointPosition.
  ///
  /// \param rayStartPos
  ///   The start position of the ray.
  ///
  /// \param rayDir
  ///   The direction of the ray. May be normalized but is not required.
  ///
  /// \return
  ///   The first return parameter is a boolean that indicates whether the ray hits the plane.
  ///   In case of a hit the next two return parameters are:
  ///   - IntersectionTime [number]: Time of intersection. The intersection time will always be >= 0.
  ///   - IntersectionPoint [hkvVec3]: Point (position) of intersection.
  ///   rayStartPos + rayDir * IntersectionTime == IntersectionPoint
  ///
  /// \par Example
  ///   \code
  ///     function OnThink(self)
	///       local plane = Vision.hkvPlane(false)
  ///       plane.setFromPointAndNormal(Vision.hkvVec3(42, -52, 148), Vision.hkvVec3(1, 0, 0))
	///       local isHit, intersectionTime, intersectionPoint = plane.getRayIntersection(Vision.hkvVec3(50, 20, 250), Vision.hkvVec3(1, 0, 0))
  ///       if isHit == true then 
  ///         Debug:PrintLine("Intersection time: " ..intersectionTime)
  ///         Debug:PrintLine("Intersection point: " ..intersectionPoint)
  ///       end
  ///     end
  ///   \endcode
	///
  /// \sa hkvPlane::getPointPosition
  ///
  /// \sa hkvPlane::getRayIntersectionBiDirectional
  ///  
  /// \sa hkvPlane::getLineSegmentIntersection
  multiple getRayIntersection(hkvVec3 rayStartPos, hkvVec3 rayDir);

  /// \brief
  ///   Does the same as hkvPlane::getRayIntersection but also returns negative intersection points ('behind' the start position).
  ///
  /// The ray will only miss the plane, if it is exactly parallel to the plane. It does not matter whether the ray
  /// starts on the positive or negative side of the plane (ie. there is no such thing as 'back-face culling').
  /// If you need to ignore plane intersections with planes where the ray starts on the negative side,
  /// just do that check manually via getPointPosition.
  ///
  /// \param rayStartPos
  ///   The start position of the ray.
  ///
  /// \param rayDir
  ///   The direction of the ray. May be normalized but is not required.
  ///
  /// \return
  ///   The first return parameter is a boolean that indicates whether the ray hits the plane.
  ///   In case of a hit the next two return parameters are:
  ///   - IntersectionTime [number]: Time of intersection. Contrary to hkvPlane::getRayIntersection this value can be positive or negative.
  ///                                Ie. if the ray points away from the plane, the intersection point will still be computed, though it is  
  ///                                located along the negative direction.
  ///   - IntersectionPoint [hkvVec3]: Point (position) of intersection.
  ///   rayStartPos + rayDir * IntersectionTime == IntersectionPoint
  ///
  /// \par Example
  ///   \code
  ///     function OnThink(self)
	///       local plane = Vision.hkvPlane(false)
  ///       plane.setFromPointAndNormal(Vision.hkvVec3(42, -52, 148), Vision.hkvVec3(1, 0, 0))
	///       local isHit, intersectionTime, intersectionPoint = plane.getRayIntersectionBiDirectional(Vision.hkvVec3(50, 20, 250), Vision.hkvVec3(1, 0, 0))
  ///       if isHit == true then 
  ///         Debug:PrintLine("Intersection time: " ..intersectionTime)
  ///         Debug:PrintLine("Intersection point: " ..intersectionPoint)
  ///       end
  ///     end
  ///   \endcode
	///
  /// \sa hkvPlane::getPointPosition
  /// \sa hkvPlane::getRayIntersection
  /// \sa hkvPlane::getLineSegmentIntersection
  multiple getRayIntersectionBiDirectional(hkvVec3 rayStartPos, hkvVec3 rayDir);

  /// \brief
  ///   Computes the intersection of a line segment and the plane.
  ///
  /// The line will only intersect the plane, if lineStartPos is on another side of the plane
  /// than vLineEndPos. It does not matter whether the start point is on the positive or negative side
  /// of the plane, ie. there is no such thing as 'back-face culling'.
  ///
  /// \param lineStartPos
  ///   The start position of the line.
  ///
  /// \param lineEndPos
  ///   The end position of the line.
  ///
  /// \return
  ///   The first return parameter is a boolean that indicates whether the line segment intersects with the plane.
  ///   In case of an intersection the next two return parameters are:    
  ///   - HitFraction [number]: Fraction along the line, where the intersection point is located. This value will always be between 0 and 1.
  ///   - IntersectionPoint [hkvVec3]: Point (position) of intersection.
  ///   lineStartPos + (lineEndPos - lineStartPos) * HitFraction == IntersectionPoint
  ///
  /// \par Example
  ///   \code
  ///     function OnThink(self)
	///       local plane = Vision.hkvPlane(false)
  ///       plane.setFromPointAndNormal(Vision.hkvVec3(42, -52, 148), Vision.hkvVec3(1, 0, 0))
	///       local isHit, hitFraction, intersectionPoint = plane.getLineSegmentIntersection(Vision.hkvVec3(50, 20, 250), Vision.hkvVec3(300, 20, 205))
  ///       if isHit == true then 
  ///         Debug:PrintLine("Hit fraction: " ..hitFraction)
  ///         Debug:PrintLine("Intersection point: " ..intersectionPoint)
  ///       end
  ///     end
  ///   \endcode
  ///
  /// \sa hkvPlane::getRayIntersection
  ///
  /// \sa hkvPlane::getRayIntersectionBiDirectional
  multiple getLineSegmentIntersection(hkvVec3 lineStartPos, hkvVec3 lineEndPos);

  /// @}
  /// @name Projections
  /// @{ 

  /// \brief
  ///   The given point is projected along the plane's normal onto the plane.
  ///
  /// \param point
  ///   Point to be projected. 
  ///
  /// \return
  ///   The projected point.
  hkvVec3 projectOntoPlane(hkvVec3 point);

  /// \brief
  ///   The direction vector is adjusted to be coplanar to the plane.
  ///
  /// That means afterwards the adjusted vector is orthogonal to the plane normal.
  /// The vector is only made coplanar, it is not normalized.
  ///
  /// \param direction
  ///   The vector that should be made coplanar to the plane.
  ///
  /// \return
  ///   The modified vector.
  hkvVec3 makeDirectionCoplanar(hkvVec3 direction);

  /// @}
  /// @name Utility Functions
  /// @{ 

  /// \brief
  ///   Returns the index of the least significant component of the planes normal.
  ///
  /// The least significant component of the plane is the normal vector's component with the largest absolute value.
  ///
  /// \return
  ///   [0;2] : Index for the x,y,z component.
  number GetLeastSignificantComponent();

  /// \brief
  ///   Will return the point mirrored at the plane.
  ///
  /// The mirrored point is the point on the other side of the plane, but at the same (absolute) distance to the plane.
  ///
  /// \param v
  ///   The point that should be mirrored.
  ///
  /// \return
  ///   (v - 2 * getDistanceTo(v) * m_vNormal)
  ///
  /// \sa hkvPlane::getDistanceTo
  hkvVec3 getMirrored(hkvVec3 v);

  /// \brief 
  ///   Clones the current plane.
  ///
  /// \return 
  ///   An identical hkvPlane.
  ///
  /// \note 
  ///   Since all wrappers classes are handled as pointers in Lua you
  ///   cannot copy its content by assignment. Please take a look at the code samples.
  ///
  /// \par Example
  ///   \code
  ///     -- create a new plane:
  ///     local p1 = Vision.hkvPlane()
  ///     p1.setFromPointAndNormal(Vision.hkvVec3(50, 20, -40), Vision.hkvVec3(0, 0, -1))
  ///
  ///     -- this will not copy the content of p1 (just another pointer to it):
  ///     local p2 = p1
  ///
  ///     -- this will affect access to p1 and p2 since the pointers share the same object:
  ///     p2:negDist = 8
  ///     -- this assertion will not fail
  ///     Vision.Assert(p1 == p2, "When this assertions fails 'p1' and 'p2' are pointing to different planes.")
  ///
  ///     -- in order to copy the content to another plane, you have to clone:
  ///     local p3 = p1:clone()
  ///
  ///     -- this will affect only p3:
  ///     p3:negDist = 2.4
  ///     -- this assertion will not fail
  ///     Vision.Assert(p1 ~= p3, "When this assertions fails 'p1' and 'p3' are pointing to same plane.")
  ///
  ///     -- instead of clone you can also create a new plane using the copy constructor:
  ///     local p4 = Vision.hkvPlane(p1)
  ///   \endcode
  hkvPlane clone();

  /// @}
  /// @name Operators 
  /// @{
  
  /// \brief
  ///   Operator == for hkvPlane comparison.
  ///
  /// \param other 
  ///   The other plane to compare with.
  ///
  /// \return 
  ///   True if the planes are identical (no epsilon), otherwise false.
  boolean __eq(hkvPlane other);

  /// @}
  /// @name Public Members
  /// @{

  /// \brief 
  ///   The normal of the plane.
  hkvVec3 m_vNormal;
  
  /// \brief 
  ///   The negative distance of the plane to the origin.
  ///
  /// Therefore for every point on the plane holds:
  /// pointOnPlane.dot(normal) + negDist == 0
  number m_fNegDist;
  
  /// @}
  /// @name Static Members
  /// @{

  /// \brief
  ///   A point / object is completely on the negative side of the plane.
  static const number hkvPlane_PLANE_POSITION_BACK;

  /// \brief
  ///   A point / object is completely coplanar to the plane (possibly within a given epsilon).
  static const number hkvPlane_PLANE_POSITION_COPLANAR;

  /// \brief
  ///   A point / object is completely on the positive side of the plane.
  static const number hkvPlane_PLANE_POSITION_FRONT;

  /// \brief
  ///   An object spans the positive and negative side of the plane.
  static const number hkvPlane_PLANE_POSITION_SPANNING;

  /// @}
};

#endif

/*
 * Havok SDK - Base file, BUILD(#20131218)
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2013
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
