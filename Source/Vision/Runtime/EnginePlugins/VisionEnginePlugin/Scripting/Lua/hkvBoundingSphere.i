/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VLUA_APIDOC

class hkvBoundingSphere
{
public:
  %extend
	{
		hkvBoundingSphere()
		{
			hkvBoundingSphere *pSphere = new hkvBoundingSphere();
			pSphere->setZero();
			return pSphere;
		}

    hkvBoundingSphere(const hkvBoundingSphere& rhs)
    {
      return new hkvBoundingSphere(rhs);
    } 

    hkvBoundingSphere(float fCenterX, float fCenterY, float fCenterZ, float fRadius)
    {
      return new hkvBoundingSphere(hkvVec3(fCenterX, fCenterY, fCenterZ), fRadius);
    }
	} 
	 
	hkvBoundingSphere(const hkvVec3& vCenter, float fRadius);

	void setZero(); 
	void set(const hkvVec3& vCenter, float fRadius);

	void expandToInclude(const hkvVec3& vPoint);
	void expandToInclude(const hkvBoundingSphere& rhs);
	void expandToInclude(const hkvAlignedBBox& rhs);	

	void addRadius(float fRadiusDifference);	
	
	bool isZero(float fEpsilon = HKVMATH_DEFAULT_EPSILON) const;
	bool isIdentical(const hkvBoundingSphere& rhs) const;
  bool isEqual(const hkvBoundingSphere& rhs, float fEpsilon) const;

	void transformFromOrigin(const hkvMat4& mTransform);
	void transformFromCenter(const hkvMat4& mTransform);
	void translate(const hkvVec3& vTranslation);

	void scaleFromCenter(float xyz);
	void scaleFromOrigin(float xyz);
	void scaleFromOrigin(const hkvVec3& vScaleXYZ);

	const hkvAlignedBBox getAlignedBoundingBox() const;

	bool contains(const hkvVec3& v) const;
	bool contains(const hkvAlignedBBox& rhs) const;
	bool contains(const hkvBoundingSphere& rhs) const;

	bool overlaps(const hkvAlignedBBox& rhs) const;
	bool overlaps(const hkvBoundingSphere& rhs) const;

	float getDistanceTo(const hkvVec3& v) const;
	float getDistanceTo(const hkvAlignedBBox& rhs) const;
	float getDistanceTo(const hkvBoundingSphere& rhs) const;

	hkvVec3 getClampedPosition(const hkvVec3& vPoint) const;

	%extend 
	{
    hkvBoundingSphere clone() const
    {
      return *self; //will call the copy constructor
    }
    
    bool __eq (const hkvBoundingSphere * pRhs) const
    {
      if(pRhs == NULL) 
			  return false;
      return pRhs->isEqual(*self, HKVMATH_DEFAULT_EPSILON);
    }
  }

	hkvVec3 m_vCenter;
  float m_fRadius;
};

%native(hkvBoundingSphere_getRayIntersection) int hkvBoundingSphere_getRayIntersection(lua_State *L);
%{
   SWIGINTERN int hkvBoundingSphere_getRayIntersection(lua_State *L)
   {
     IS_MEMBER_OF(hkvBoundingSphere) //this will move this function to the method table of the specified class
    
		 SWIG_CONVERT_POINTER(L, 1, hkvBoundingSphere, pSelf)

		 DECLARE_ARGS_OK

		 // Get arguments
	   GET_ARG(2, hkvVec3, vRayStartPos);
	   GET_ARG(3, hkvVec3, vRayDir);	    

		 float fIntersectionTime;
		 hkvVec3 vIntersectionPoint;
		 bool bIntersect = pSelf->getRayIntersection(vRayStartPos, vRayDir.getNormalized(), &fIntersectionTime, &vIntersectionPoint);

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

%native(hkvBoundingSphere_getLineSegmentIntersection) int hkvBoundingSphere_getLineSegmentIntersection(lua_State *L);
%{
   SWIGINTERN int hkvBoundingSphere_getLineSegmentIntersection(lua_State *L)
   {
     IS_MEMBER_OF(hkvBoundingSphere) //this will move this function to the method table of the specified class
    
		 SWIG_CONVERT_POINTER(L, 1, hkvBoundingSphere, pSelf)

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
VSWIG_CREATE_CONCAT(hkvBoundingSphere, 64, "[%1.2f,%1.2f,%1.2f][%1.2f]", self->m_vCenter.x, self->m_vCenter.y, self->m_vCenter.z, self->m_fRadius)
VSWIG_CREATE_TOSTRING(hkvBoundingSphere, "hkvBoundingSphere: Center[%1.2f,%1.2f,%1.2f], Radius[%1.2f]", self->m_vCenter.x, self->m_vCenter.y, self->m_vCenter.z, self->m_fRadius)

#else

/// \brief 
///   Wrapper for the hkvBoundingSphere bounding sphere.
///
/// \note 
///   Since all wrappers classes are handled as pointers in Lua you
///   cannot copy its content by assignment. Please take a look at the code samples.
///
/// \par Example
///   \code
///     -- create a new bounding sphere:
///     local bsphere1 = Vision.hkvBoundingSphere(Vision.hkvVec3(-40, 20, 158), 40.8)
///
///     -- this will not copy the content (just another pointer to it):
///     local bsphere2 = bsphere1
///
///     -- this will affect access to 'bsphere2' and 'bsphere1' since the pointers share the same object:
///     bsphere2:set(Vision.hkvVec3(-40, 244, 158), 12.5))
///
///     -- this assertion will not fail
///     Vision.Assert(bsphere1 == bsphere2, "When this assertions fails 'bsphere1' and 'bsphere2' are pointing to different bounding spheres.")
///
///     -- in order to copy the content, you have to clone:
///     local bsphere3 = bsphere1:clone()
///
///     -- or create a new bounding sphere using the copy constructor
///     local bsphere4 = Vision.hkvBoundingSphere(bsphere1)
///   \endcode
/// \see Clone
class hkvBoundingSphere 
{
public:

  /// @name Constructors
  /// @{
  
  /// \brief 
	///   Default Constructor. Will set all data of bounding sphere to zero.
  hkvBoundingSphere();

  /// \brief 
  ///   Copy Constructor.
  ///
  /// \param other 
  ///   The bounding sphere to copy.
  hkvBoundingSphere(hkvBoundingSphere other);

  /// \brief
  ///   Constructor initializing the bounding sphere with the specified values.
  ///
  /// \param centerX
  ///   X coordinate of the sphere center.
  ///
  /// \param centerY
  ///   Y coordinate of the sphere center.
  ///
  /// \param centerZ
  ///   Z coordinate of the sphere center.
  ///
  /// \param radius
  ///   Sphere radius.
  hkvBoundingSphere(number centerX, number centerY, number centerZ, number radius);

	/// \brief
  ///   Initializes the center and radius directly.
  hkvBoundingSphere(hkvVec3 center, number radius);

	/// @}
  /// @name Manipulation
  /// @{
  
  /// \brief 
	///   Sets all data to zero.
  void setZero();

	/// \brief
  ///   Sets the sphere center and radius.
  void set(hkvVec3 center, number radius);

	/// \brief
  ///   Increases the sphere's radius such that the given point is contained in the resulting sphere.
  ///
  /// The sphere's center is never changed. Also the radius does not change, if point is already inside in the sphere.
  ///
  /// \param point
  ///   After expanding the sphere this point will be contained in it.
  void expandToInclude(hkvVec3 point);

	/// \brief
  ///   Increases the sphere's radius such that the given sphere is contained in the resulting sphere.
  ///
  /// The sphere's center is never changed. Also the radius does not change, if the given sphere is already inside in the sphere.
  ///
  /// \param sphere
  ///   After expansion this object will be contained in the sphere.
  void expandToInclude(hkvBoundingSphere sphere);

	/// \brief
  ///   Increases the sphere's radius such that the given box is contained in the resulting sphere.
  ///
  /// The sphere's center is never changed. Also the radius does not change, if the box is already inside in the sphere.
  ///
  /// \param box
  ///   After expansion this object will be contained in the sphere.
  void expandToInclude(hkvAlignedBBox box);

  /// \brief
  ///   Adds the given value to the sphere's radius, thus increasing or decreasing its volume.
  void addRadius(number radiusDifference);

	/// @}
  /// @name Checks
  /// @{

  /// \brief
  ///   Checks whether all components are zero within some epsilon range.
  boolean isZero(number epsilon = MATH_DEFAULT_EPSILON);

	/// \brief
  ///   Checks whether this sphere and the other sphere are identical.
  boolean isIdentical(hkvBoundingSphere other);

	/// \brief
  ///   Checks whether this sphere and the other sphere are equal within some epsilon range.
  boolean isEqual(hkvBoundingSphere other, number epsilon);

	/// @}
  /// @name Modification
  /// @{

	/// \brief
  ///   Transforms the sphere using the origin (ie. (0|0|0)) as the pivot.
  ///
  /// \param transform 
  ///   The transformation to be applied on the sphere.
  ///
  /// The center of the sphere is simply multiplied by transform. 
  /// Thus all translations, rotations and scalings are applied as to any other point.
  /// The radius is multiplied by the largest scaling factor in the transformation matrix.
  /// That means for a uniform scaling matrix, the sphere scales as expected. However in case
  /// of a non-uniform scaling the radius will be multiplied by the largest scaling factor
  /// of the matrix. E.g. if the matrix scales an object by 2 along X and Y, but by 3 along Z,
  /// then the bounding sphere's radius will be scaled by 3.
  /// This makes sure that the bounding sphere will always be large enough to enclose all
  /// points of an object.
  ///
  /// \sa hkvBoundingSphere::transformFromCenter
  void transformFromOrigin(hkvMat4 transform);

	/// \brief
  ///   Transforms the sphere using the center of the sphere as the pivot.
  ///
  /// \param transform
  ///   The transformation to be applied on the sphere.
  ///
  /// The sphere's center is used as the pivot for the transformation. 
  /// That means that only the translation part of transform will have any effect
  /// on the sphere's center, as rotating a sphere around itself does not change the sphere
  /// and scaling a pivot-point also has no effect on that point.
  /// 
  /// The radius is multiplied by the largest scaling factor in the transformation matrix.
  /// That means for a uniform scaling matrix, the sphere scales as expected. However in case
  /// of a non-uniform scaling the radius will be multiplied by the largest scaling factor
  /// of the matrix. E.g. if the matrix scales an object by 2 along X and Y, but by 3 along Z,
  /// then the bounding sphere's radius will be scaled by 3.
  /// This makes sure that the bounding sphere will always be large enough to enclose all
  /// points of an object.
  ///
  /// \sa hkvBoundingSphere::transformFromOrigin
  void transformFromCenter(hkvMat4 transform);

	/// \brief
  ///   Translates the sphere's center by the given amount.
  void translate(hkvVec3 translation);

	/// \brief
  ///   Scales the sphere's radius but does not change its position.
	/// 
	/// \param xyz
  ///   Uniform scale, must be >= 0.
  void scaleFromCenter(number xyz);

	/// \brief
  ///   Scales the sphere's center and radius by the given factor, thus moving it away from the origin.
	/// 
	/// \param xyz
  ///   Uniform scale, must be >= 0.
  void scaleFromOrigin(number xyz);

	/// \brief
  ///   Scales the sphere's center component-wise by scaleXYZ. Scales its radius by the largest component of scaleXYZ.
  ///
  /// The center of the sphere is simply multiplied component-wise by scaleXYZ.
  /// The radius is multiplied by the largest component of scaleXYZ.
  /// E.g. if vScaleXYZ scales an object by 2 along X and Y, but by 3 along Z,
  /// then the bounding sphere's radius will be scaled by 3.
  /// This makes sure that the bounding sphere will always be large enough to enclose all
  /// points of an object.
  ///
  /// \param scaleXYZ
  ///   The scaling factors along each axis, each factor must be >= 0.
  ///
  /// \sa hkvBoundingSphere::transformFromCenter
  void scaleFromOrigin(hkvVec3 scaleXYZ);

	/// @}
  /// @name Retrieving Data
  /// @{

	/// \brief
  ///   Returns a hkvAlignedBBox that perfectly fits around this sphere.
  hkvAlignedBBox getAlignedBoundingBox();

	/// @}
  /// @name Bounding Volume Checks
  /// @{

	/// \brief
  ///   Checks whether the given point is inside the sphere.
  boolean contains(hkvVec3 point);

	/// \brief
  ///   Checks whether the given box is completely inside the sphere.
  ///
  /// If the box and the sphere overlap but the box is not completely inside the sphere,
  /// this function will return false.
  ///
  /// \param box
  ///   The box that is checked whether it is inside this sphere.
  boolean contains(hkvAlignedBBox box);

	/// \brief
  ///   Checks whether the given sphere is completely inside this sphere.
  ///
  /// If the two spheres overlap but the given sphere is not completely inside this sphere,
  /// this function will return false.
  ///
  /// \param sphere
  ///   The sphere that is checked whether it is inside this sphere.
  boolean contains(hkvBoundingSphere sphere);

	/// \brief
  ///   Checks whether the given box overlaps with this sphere.
  ///
  /// If the box is inside this sphere, the function will also return true.
  /// It will only return false if the two objects are completely disjunct.
  ///
  /// \param box
  ///   The box that is checked whether it is inside this sphere.
  boolean overlaps(hkvAlignedBBox box);

	/// \brief
  ///   Checks whether the given sphere overlaps with this sphere.
  ///
  /// If the sphere is inside this sphere, the function will also return true.
  /// It will only return false if the two objects are completely disjunct.
  ///
  /// \param sphere
  ///   The sphere that is checked whether it overlaps with this sphere.
  boolean overlaps(hkvBoundingSphere sphere);

	/// @}
  /// @name Intersection Tests
  /// @{ 

	/// \brief
  ///   Returns true if the ray hits the surface of the sphere and computes the intersection time and point.
  ///
  /// \param rayStartPos
  ///   The start position of the ray. If the ray starts within the sphere
  ///   the function will always return true. However even in that case the returned point of
  ///   intersection will always be on the sphere's surface, ie. when the
  ///   ray exits the sphere.
  ///
  /// \param rayDir
  ///   The direction of the ray. This parameter will be internally normalized (\b always).
  ///
	/// \return
	///   The first return parameter is a boolean that indicates whether the ray hits the sphere.
  ///   In case of a hit the next two return parameters are:
	///   - IntersectionTime [number]: Time of intersection. 
	///   - IntersectionPoint [hkvVec3]: Point (position) of intersection.
  ///   rayStartPos + rayDirNormalized * IntersectionTime == IntersectionPoint
  ///
  /// \par Example
  ///   \code
  ///     function OnThink(self)
	///       local bsphere = Vision.hkvBoundingSphere(Vision.hkvVec3(30, -40, -30.6), 25)
	///       local isHit, intersectionTime, intersectionPoint = bsphere.getRayIntersection(Vision.hkvVec3(50, 20, 250), Vision.hkvVec3(1, 0, 0))
  ///       if isHit == true then 
  ///         Debug:PrintLine("Intersection time: " ..intersectionTime)
  ///         Debug:PrintLine("Intersection point: " ..intersectionPoint)
  ///       end
  ///     end
  ///   \endcode
	///
  /// \sa hkvBoundingSphere::getLineSegmentIntersection
	multiple getRayIntersection(hkvVec3 rayStartPos, hkvVec3 rayDir);

	/// \brief
  ///   Computes the intersection of a line segment and the surface of the sphere.
  ///
  /// If the line segment's start and end point are both within the sphere, no intersection will be reported.
  /// If you need to handle these cases as intersections, test first, whether the sphere contains the start 
  /// position already.
  ///
  /// This function internally just calls hkvBoundingSphere::getRayIntersection, thus it is
  /// more efficient to call that function directly, if possible.
  ///
  /// \param lineStartPos
  ///   The start position of the line. If the line starts within the sphere
  ///   the function will only find an intersection, if the line end is outside the sphere.
  ///
  /// \param lineEndPos
  ///   The end position of the line.
  ///
	/// \return
	///   The first return parameter is a boolean that indicates whether the line segment intersects with the sphere.  
  ///   In case of an intersection the next two return parameters are:    
	///   - HitFraction [number]: Fraction along the line, where the intersection point is located.
	///   - IntersectionPoint [hkvVec3]: Point (position) of intersection.
  ///   lineStartPos + (lineEndPos - lineStartPos) * HitFraction == IntersectionPoint
  ///
  /// \par Example
  ///   \code
  ///     function OnThink(self)
	///       local bsphere = Vision.hkvBoundingSphere(Vision.hkvVec3(30, -40, -30.6), 25)
	///       local isHit, hitFraction, intersectionPoint = bsphere.getLineSegmentIntersection(Vision.hkvVec3(50, 20, 250), Vision.hkvVec3(200, 20, 250))
  ///       if isHit == true then 
  ///         Debug:PrintLine("Hit fraction: " ..hitFraction)
  ///         Debug:PrintLine("Intersection point: " ..intersectionPoint)
  ///       end
  ///     end
  ///   \endcode
  ///
  /// \sa hkvBoundingSphere::getRayIntersection
	multiple getLineSegmentIntersection(hkvVec3 lineStartPos, hkvVec3 lineEndPos);

  /// @}
  /// @name Utility Functions
  /// @{

	/// \brief
  ///   Returns the minimum distance between the sphere and the given point.
  ///
  /// Will return 0 for all points inside the sphere.
  ///
  /// \note 
	///   There is no getDistanceToSquared function, as this cannot be computed
  ///   any more efficiently.
  ///
  /// \param point
  ///   The point to which the distance should be computed.
  number getDistanceTo(hkvVec3 point);

	/// \brief
  ///   Returns the minimum distance between this sphere and the given box.
  ///
  /// Will return 0 if the two objects overlap.
  ///
  /// \note 
	///   There is no getDistanceToSquared function, as this cannot be computed
  ///   any more efficiently.
  ///
  /// \param box
  ///   The box to which the distance should be computed.
  number getDistanceTo(hkvAlignedBBox box);

	/// \brief
  ///   Returns the minimum distance between this sphere and the given sphere.
  ///
  /// Will return 0 if the two objects overlap.
  ///
  /// \note 
	///   There is no getDistanceToSquared function, as this cannot be computed
  ///   any more efficiently.
  ///
  /// \param sphere
  ///   The sphere to which the distance should be computed.
  number getDistanceTo(hkvBoundingSphere sphere);

	/// \brief
  ///   Clamps point to the volume of the sphere and returns that point.
  ///
  /// \param point
  ///   The point that should be clamped to the sphere's volume.
  ///
  /// \return
  ///   If point is inside the sphere, it will be returned without any modification.
  ///   If it is outside the sphere, the closest point on the sphere's surface is returned.
  hkvVec3 getClampedPosition(hkvVec3 point);

	/// \brief 
	///   Clone the current bounding sphere.
	///
  /// \return 
	///   The new duplicate.
	///
  /// \note 
	///   Since all wrappers classes are handled as pointers in Lua you
  ///   cannot copy its content by assignment!
	///
  /// \par Example
  ///   \code
  ///     -- create a new bounding sphere:
  ///     local bsphere1 = Vision.hkvBoundingSphere(Vision.hkvVec(80, 178, -300), 44.4)
  ///
  ///     -- this will not copy the content (just another pointer to it):
  ///     local bsphere2 = bsphere1
  ///
  ///     -- this will affect access to 'bsphere2' and 'bsphere1' since the pointers share the same object:
  ///     bsphere2:set(Vision.hkvVec(50, 78, 120), 58.6)
  ///
  ///     -- this assertion will not fail
  ///     Vision.Assert(bsphere1 == bsphere2, "When this assertions fails 'bsphere1' and 'bsphere2' are pointing to different bounding spheres.")
  ///
  ///     -- in order to copy the content, you have to clone:
  ///     local bsphere3 = bsphere1:clone()
  ///     bsphere3:set(Vision.hkvVec(20, 78, 120), 58.6)
  ///
  ///     -- this assertion will not fail
  ///     Vision.Assert(bsphere1 ~= bsphere3, "When this assertions fails 'bsphere1' and 'bsphere3' are pointing to the same bounding sphere.")
  ///
  ///   \endcode
  hkvBoundingSphere clone();

	/// @}
  /// @name Operators
  /// @{
  
  /// \brief 
	///   Comparison operator. Checks whether two spheres are equal.
	///
  /// \param other 
	///   The other bounding sphere.
	///
  /// \return 
	///   True if the bounding spheres are equal within Vision.MATH_DEFAULT_EPSILON, otherwise false.
  boolean __eq (hkvBoundingSphere other);

  /// @}
  /// @name Public Members
  /// @{

	/// \brief 
	///   The center of the sphere.
  hkvVec3 m_vCenter;

  /// \brief 
	///   The radius of the sphere.
  float m_fRadius;

  /// @}

};

#endif

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
