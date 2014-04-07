/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VLUA_APIDOC

class hkvAlignedBBox
{
public:
  %extend
	{
    // For backward compatibility the input parameter is kept.
		hkvAlignedBBox(bool bUnused = true)
		{
			hkvAlignedBBox *pBox = new hkvAlignedBBox(hkvNoInitialization);
			pBox->setInvalid();
			return pBox;
		} 

    hkvAlignedBBox(const hkvAlignedBBox& rhs)
    {
      return new hkvAlignedBBox(rhs);
    } 

    hkvAlignedBBox(float x1, float y1, float z1, float x2, float y2, float z2)
    {
      return new hkvAlignedBBox(hkvVec3(x1, y1, z1), hkvVec3(x2, y2, z2));
    }
	}

  hkvAlignedBBox(const hkvVec3& vMin, const hkvVec3& vMax);

  void setZero();
  void setCenterAndSize(const hkvVec3& vCenter, const hkvVec3& vHalfSizeXYZ);
  void set(const hkvVec3& vMin, const hkvVec3& vMax);

  %extend
	{
	  void set(float x1, float y1, float z1, float x2, float y2, float z2)
    {
      self->set(hkvVec3(x1, y1, z1), hkvVec3(x2, y2, z2));
    }
	}
  
  void setInvalid();

  void expandToInclude(const hkvVec3& v);
  void expandToInclude(const hkvAlignedBBox& rhs);
	void expandToInclude(const hkvBoundingSphere& rhs); 
	void expandToCube();
	void addBoundary(const hkvVec3& vChangeXYZ);

  bool isIdentical(const hkvAlignedBBox& rhs) const;
  bool isEqual(const hkvAlignedBBox& rhs, float fEpsilon) const;
  bool isZero(float fEpsilon = HKVMATH_DEFAULT_EPSILON) const;
  bool isValid() const;

	void transformFromCenter(const hkvMat4& mTransform);
  void transformFromOrigin(const hkvMat4& mTransform);
  void translate(const hkvVec3& vTranslation);
  void scaleFromCenter(const hkvVec3& v);
  void scaleFromOrigin(const hkvVec3& vScaleXYZ);

  const hkvVec3 getCenter() const;
  float getSizeX() const;
  float getSizeY() const;
  float getSizeZ() const;

	hkvVec3 getCorner(bool bMaxX, bool bMaxY, bool bMaxZ) const;

	%extend
	{
	  float getMaxExtent() const
		{
		  return self->getMaxExtent();
		}

	  float getMinExtent() const
		{
		  return self->getMinExtent();
		}
	}

	const hkvBoundingSphere getBoundingSphere() const;

  bool contains(const hkvVec3& v) const;
  bool contains(const hkvAlignedBBox& rhs) const;
	bool contains(const hkvBoundingSphere& rhs) const;
 
  bool overlaps(const hkvAlignedBBox& rhs) const;
	bool overlaps(const hkvBoundingSphere& rhs) const;

  float getDistanceToSquared(const hkvVec3& v) const;
  float getDistanceToSquared(const hkvAlignedBBox& rhs) const;
  
  float getDistanceTo(const hkvVec3& v) const;
  float getDistanceTo(const hkvAlignedBBox& rhs) const;
  float getDistanceTo(const hkvBoundingSphere& rhs) const;

  hkvVec3 getClampedPosition(const hkvVec3& v) const;
  
  %extend 
	{
    hkvAlignedBBox clone() const
    {
      return *self; //will call the copy constructor
    }
    
    bool __eq (const hkvAlignedBBox * pRhs) const
    {
      if(pRhs == NULL) 
			  return false;
      return pRhs->isEqual(*self, HKVMATH_DEFAULT_EPSILON);
    }
  }
  
  hkvVec3 m_vMin;
  hkvVec3 m_vMax;
};

//Implement getCorners native because it returns an array (table) of hkvVec3s
%native(hkvAlignedBBox_getCorners) int hkvAlignedBBox_getCorners(lua_State *L);
%{
  SWIGINTERN int hkvAlignedBBox_getCorners(lua_State *L)
  {
    IS_MEMBER_OF(hkvAlignedBBox) //this will move this function to the method table of the specified class
    
    SWIG_CONVERT_POINTER(L, 1, hkvAlignedBBox, pBox)

    lua_newtable(L); //create an empty table (or an array if you would like to see it this way)
    
    hkvVec3 vecs[8];
    
    pBox->getCorners(vecs);
    
    SWIG_Lua_NewPointerObj(L,new hkvVec3(vecs[0]),SWIGTYPE_p_hkvVec3, VLUA_MANAGE_MEM_BY_LUA);
    lua_rawseti(L, -2, 1);
    SWIG_Lua_NewPointerObj(L,new hkvVec3(vecs[1]),SWIGTYPE_p_hkvVec3, VLUA_MANAGE_MEM_BY_LUA);
    lua_rawseti(L, -2, 2);
    SWIG_Lua_NewPointerObj(L,new hkvVec3(vecs[2]),SWIGTYPE_p_hkvVec3, VLUA_MANAGE_MEM_BY_LUA);
    lua_rawseti(L, -2, 3);
    SWIG_Lua_NewPointerObj(L,new hkvVec3(vecs[3]),SWIGTYPE_p_hkvVec3, VLUA_MANAGE_MEM_BY_LUA);
    lua_rawseti(L, -2, 4);
    
    SWIG_Lua_NewPointerObj(L,new hkvVec3(vecs[4]),SWIGTYPE_p_hkvVec3, VLUA_MANAGE_MEM_BY_LUA);
    lua_rawseti(L, -2, 5);
    SWIG_Lua_NewPointerObj(L,new hkvVec3(vecs[5]),SWIGTYPE_p_hkvVec3, VLUA_MANAGE_MEM_BY_LUA);
    lua_rawseti(L, -2, 6);
    SWIG_Lua_NewPointerObj(L,new hkvVec3(vecs[6]),SWIGTYPE_p_hkvVec3, VLUA_MANAGE_MEM_BY_LUA);
    lua_rawseti(L, -2, 7);
    SWIG_Lua_NewPointerObj(L,new hkvVec3(vecs[7]),SWIGTYPE_p_hkvVec3, VLUA_MANAGE_MEM_BY_LUA);
    lua_rawseti(L, -2, 8);
    
    return 1; //the table 
  }
%}

%native(hkvAlignedBBox_getRayIntersection) int hkvAlignedBBox_getRayIntersection(lua_State *L);
%{
   SWIGINTERN int hkvAlignedBBox_getRayIntersection(lua_State *L)
   {
     IS_MEMBER_OF(hkvAlignedBBox) //this will move this function to the method table of the specified class
    
		 SWIG_CONVERT_POINTER(L, 1, hkvAlignedBBox, pSelf)

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

%native(hkvAlignedBBox_getLineSegmentIntersection) int hkvAlignedBBox_getLineSegmentIntersection(lua_State *L);
%{
   SWIGINTERN int hkvAlignedBBox_getLineSegmentIntersection(lua_State *L)
   {
     IS_MEMBER_OF(hkvAlignedBBox) //this will move this function to the method table of the specified class
    
		 SWIG_CONVERT_POINTER(L, 1, hkvAlignedBBox, pSelf)

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
VSWIG_CREATE_CONCAT(hkvAlignedBBox, 64, "[%1.2f,%1.2f,%1.2f][%1.2f,%1.2f,%1.2f]", self->m_vMin.x,self->m_vMin.y,self->m_vMin.z, self->m_vMax.x,self->m_vMax.y,self->m_vMax.z)
VSWIG_CREATE_TOSTRING(hkvAlignedBBox, "hkvAlignedBBox: Min[%1.2f,%1.2f,%1.2f] - Max[%1.2f,%1.2f,%1.2f]", self->m_vMin.x,self->m_vMin.y,self->m_vMin.z, self->m_vMax.x,self->m_vMax.y,self->m_vMax.z)

#else

/// \brief 
///   Wrapper for the hkvAlignedBBox axis aligned bounding. 
///
/// \note 
///   Since all wrappers classes are handled as pointers in Lua you
///   cannot copy its content by assignment. Please take a look at the code samples.
///
/// \par Example
///   \code
///     -- create a new bounding box:
///     local bbox1 = Vision.hkvAlignedBBox(-1,-1,-1, 1,1,1)
///
///     -- this will not copy the content (just another pointer to it):
///     local bbox2 = bbox1
///
///     -- this will affect access to 'bbox2' and 'bbox1' since the pointers share the same object:
///     bbox2:set(-2,-2,-2, 2,2,2)
///
///     -- this assertion will not fail
///     Vision.Assert(bbox1 == bbox2, "When this assertions fails 'bbox1' and 'bbox2' are pointing to different bounding boxes.")
///
///     -- in order to copy the content, you have to clone:
///     local bbox3 = bbox1:clone()
///
///     -- or create a new bounding box using the copy constructor
///     local bbox4 = Vision.hkvAlignedBBox(bbox1)
///
///     -- to make life easier, every Vision scripting method creates a copy for you - e.g:
///     local b1 = entity:GetBoundingBox()
///     local b2 = entity:GetBoundingBox()
///     b1.m_vMin.x = 12
///     b2.m_vMin.x = -10
///
///     -- this assertion will not fail
///     Vision.Assert(b1 ~= b2, "When this assertion fails 'b1' and 'b2' are pointing to the same bounding box.")
///
///   \endcode
/// \see Clone
class hkvAlignedBBox 
{
public:

  /// @name Constructors
  /// @{
  
  /// \brief 
	///   Default Constructor. Will initialize bounding box as invalid (max < min).
  hkvAlignedBBox();

  /// \brief 
  ///   Copy Constructor.
  ///
  /// \param other 
  ///   The bounding box to copy.
  hkvAlignedBBox(hkvAlignedBBox other);
  
  /// \brief 
	///   Constructor initializing the bounding box with the specified values.
	///
  /// \param minX 
	///   X position of the minimum point of the bounding box.
	///
  /// \param minY 
	///   Y position of the minimum point of the bounding box.
	///
  /// \param minZ 
	///   Z position of the minimum point of the bounding box.
	///
  /// \param maxX 
	///   X position of the maximum point of the bounding box.
	///
  /// \param maxY 
	///   Y position of the maximum point of the bounding box.
	///
  /// \param maxZ 
	///   Z position of the maximum point of the bounding box.
  hkvAlignedBBox(number minX, number minY, number minZ, number maxX, number maxY, number maxZ);

	/// \brief 
	///   Constructor which takes two vectors as parameters to define the extents of the box.
	///
  /// \param min 
	///   Minimum point of the bounding box.
	///
  /// \param max 
	///   Maximum point of the bounding box.
  hkvAlignedBBox(hkvVec3 min, hkvVec3 max);

  /// @}
  /// @name Manipulation
  /// @{
  
  /// \brief 
	///   Sets the box to all zero.
	///
  /// \note 
	///   This is a valid bounding box with a volume of zero.
  void setZero();

	/// \brief
	///   Sets the box's min and max values from the given center and half-size extents.
	///
  /// \param center 
	///   The new center position.
	///
  /// \param halfSize 
	///   The half-size extent of the new box.
  void setCenterAndSize(hkvVec3 center, hkvVec3 halfSize);

	/// \brief 
	///   Sets min/max extents that make up the bounding box.
	///
  /// \note 
	///   In debug mode this will check that vMin <= vMax, to ensure a valid bbox!
	///
  /// \param min 
	///   Minimum point of the bounding box.
	///
  /// \param max 
	///   Maximum point of the bounding box.
  void set(hkvVec3 min, hkvVec3 max);
  
  /// \brief 
	///   Sets min/max extents that make up the bounding box.
	///
  /// \note
	///   In debug mode this will check that vMin <= vMax, to ensure a valid bbox!
	///
  /// \param minX 
	///   X position of the minimum point of the bounding box.
	///
  /// \param minY
	///   Y position of the minimum point of the bounding box.
	///
  /// \param minZ 
	///   Z position of the minimum point of the bounding box.
	///
  /// \param maxX 
	///   X position of the maximum point of the bounding box.
	///
  /// \param maxY 
	///   Y position of the maximum point of the bounding box.
	///
  /// \param maxZ 
	///   Z position of the maximum point of the bounding box.
  void set(number minX, number minY, number minZ, number maxX, number maxY, number maxZ);

  /// \brief 
	///   Sets m_vMin to the maximum positive float value, and m_vMax to the maximum negative float value (min > max),
  ///   making it an invalid box.
	///
  /// \note 
	///   The expandToInclude functions can then be used to enlarge the box until it is large enough.
	///
  /// \see hkvAlignedBBox::expandToInclude
  void setInvalid();

  /// \brief 
  ///   Enlarges the box (if necessary) such that the specified vector is contained in it.
  /// \param vector 
  ///   The vector to include.
  void expandToInclude(hkvVec3 vector);
  
  /// \brief 
	///   Enlarges the bounding box (if necessary) such that the specified box is contained in it.
	///
  /// \param box 
	///   The bounding box to include.
  void expandToInclude(hkvAlignedBBox box);

	/// \brief
	///   Enlarges the bounding box (if necessary) such that the specified bounding sphere is contained in it.
	///
	/// \param sphere
	///   The bounding sphere to include.
  void expandToInclude(hkvBoundingSphere sphere);

	/// \brief
	///   Makes all sides of the box as long as its longest side, thus making it a cube.
  void expandToCube();

  /// \brief 
	///   Changes the size in each direction, e.g. along the positive AND negative axis.
  void addBoundary(hkvVec3 changeXYZ);

	/// @}
  /// @name Checks
  /// @{

	/// \brief
	///   Checks whether the two boxes are identical.
	///
  /// \param other 
	///   The other box to check.
	///
  /// \return 
	///   True if identical (no epsilon involved), otherwise false.
  boolean isIdentical(hkvAlignedBBox other);

	/// \brief 
	///   Checks whether the two boxes are equal within some epsilon range.
	///
  /// \param other 
	///   The other box to check.
	///
  /// \param epsilon
	///    Specify an epsilon for this range (e.g Vision.MATH_LARGE_EPSILON).
	///
  /// \return 
	///   True if equal (within the epsilon), otherwise false.
  boolean isEqual(hkvAlignedBBox other, number epsilon);

	/// \brief 
	///   Checks whether this bounding box is all zero (within some epsilon range).
	///
  /// \param epsilon (\b optional) 
	///   Specify an epsilon for this range (default is Vision.MATH_DEFAULT_EPSILON).
	///
  /// \return 
	///   True if zero within the specified epsilon, otherwise false.
  boolean isZero(number epsilon = Vision.MATH_DEFAULT_EPSILON);

  /// \brief 
	///   Checks if the bounding box is valid (m_vMin <= m_vMax).
	///
  /// \return 
	///   True if m_vMin <= m_vMax, otherwise false.
  boolean isValid();

  /// @}
  /// @name Modification
  /// @{

	/// \brief 
  ///   Transforms the 8 corners in object-space and then recomputes the AABB.
  void transformFromCenter(hkvMat4 transform);

  /// \brief 
  ///   Transforms the 8 corners in world-space and then recomputes the AABB.
  void transformFromOrigin(hkvMat4 transform);

	/// \brief 
	///   Translates the box in world-space.
	///
  /// \param translation 
	///   The translation vector.
  void translate(hkvVec3 translation);

  /// \brief 
	///   Scales the box's size, but does not change its position.
	///
  /// \param scale 
	///   The scaling values for the x y and z axes.
  void scaleFromCenter(hkvVec3 scale);

  /// \brief 
	///   Scales the box's min and max extents, thus changing its size and position.
	///
  /// \param scale 
	///   The scaling values for the x y and z axes.
  void scaleFromOrigin(hkvVec3 scale);

	/// @}
  /// @name Retrieving Data
  /// @{

	/// \brief 
	///   Returns the center point of the box.
  hkvVec3 getCenter();
  
  /// \brief
	///   Returns the box's extent along the X axis.
  number getSizeX();
  
  /// \brief 
	///   Returns the box's extent along the Y axis.
  number getSizeY();
  
  /// \brief 
	///   Returns the box's extent along the Z axis.
  number getSizeZ();

	/// \brief
  ///   Returns one corner of the box.
  ///
  /// \param maxX
  ///   If set to true the X coordinate of the returned corner will be from the max corner.
  ///   Otherwise it will be from the min corner.
  ///
  /// \param maxY
  ///   If set to true the Y coordinate of the returned corner will be from the max corner.
  ///   Otherwise it will be from the min corner.
  ///
  /// \param maxZ
  ///   If set to true the Z coordinate of the returned corner will be from the max corner.
  ///   Otherwise it will be from the min corner.
  ///
  /// \sa hkvAlignedBBox::getCorners
  hkvVec3 getCorner(boolean maxX, boolean maxY, boolean maxZ);
 
	/// \brief
  ///   Writes the 8 corners of the box to a table.
  ///
	/// \return
	///   Table that contains 8 corners of the box.
  ///
	/// \par Example
  ///   \code
  ///     function OnThink(self)
	///       local bbox = entity:GetBoundingBox()
	///       local corners = bbox:getCorners()
	///       Debug:PrintLine("1.Corner = x: "..corners[1].x", y: "..corners[1].y", z: "..corners[1].z)
  ///     end
  ///   \endcode
  ///
  /// \sa hkvAlignedBBox::getCorner
  table getCorners();

	/// \brief
  ///   Checks which of the extents (X,Y,Z) is the longest. 
  ///
  /// \return
  ///   The length of the longest extent.
  number getMaxExtent();

	/// \brief
  ///   Checks which of the extents (X,Y,Z) is the shortest. 
  ///
  /// \return
  ///   The length of the shortest extent.
  number getMinExtent();

	/// \brief
  ///   Returns a hkvBoundingSphere that perfectly fits around this box.
  hkvBoundingSphere getBoundingSphere();

  /// @}
  /// @name Bounding Volume Checks
  /// @{
  
  /// \brief 
	///   Returns true if the specified point is inside this bounding box.
	///
  /// \param point 
	///   The point to check.
	///
  /// \return
	///   True if the box contains the specified point, otherwise false.
  boolean contains(hkvVec3 point);
  
  /// \brief 
	///   Returns true if the specified bounding box is inside this bounding box.
	///
  /// \param box 
	///   The bounding box to check.
	///
  /// \return 
	///   True if the specified bounding box is inside this bounding box, otherwise false.
  boolean contains(hkvAlignedBBox box);

	/// \brief
  ///   Returns true if the specified bounding sphere is inside this bounding box.
	///
	/// \param sphere
	///   The bounding sphere to check.
	///
	/// \return
	///   True if the specified bounding sphere is inside this bounding box, otherwise false.
  boolean contains(hkvBoundingSphere sphere);

  /// \brief 
	///   Returns true if the specified bounding box is inside this bounding box or partially overlaps it.
	///
  /// \param box 
	///   The bounding box to check.
	///
  /// \return 
	///   Returns true if the specified bounding box is inside this bounding box or partially overlaps it.
  boolean overlaps(hkvAlignedBBox box);

	/// \brief 
	///   Returns true if the specified bounding sphere is inside this bounding box or partially overlaps it.
	///
  /// \param sphere 
	///   The bounding sphere to check.
	///
  /// \return 
	///   Returns true if the specified bounding sphere is inside this bounding box or partially overlaps it.
  boolean overlaps(hkvBoundingSphere sphere);

	/// @}
  /// @name Intersection Tests
  /// @{ 

	/// \brief
  ///   Checks whether the given ray intersects the surface of the box and computes the intersection time and point.
  ///
  /// If the ray start position is within the box, an intersection with the surface will always be detected.
  /// If however, you need to handle such cases as if the box was solid and the intersection point should already be
  /// at the ray start position, you should check first whether the box contains the ray start position already.
  ///
  /// \param rayStartPos
  ///   The start position of the ray. If the start position is inside the box, an intersection will always
  ///   be detected, however not at time 0, but wherever the ray hits the surface of the box.
  ///
  /// \param rayDir
  ///   The direction of the ray. May be normalized but is not required.
	///
	/// \return
	///   The first return parameter is a boolean that indicates whether the ray hits the box.
  ///   In case of a hit the next two return parameters are:
	///   - IntersectionTime [number]: Time of intersection. The intersection time will always be >= 0.
	///   - IntersectionPoint [hkvVec3]: Point (position) of intersection.
  ///   rayStartPos + rayDir * IntersectionTime == IntersectionPoint
	///
  /// \par Example
  ///   \code
  ///     function OnThink(self)
	///       local bbox = entity:GetBoundingBox()
	///       local isHit, intersectionTime, intersectionPoint = bbox.getRayIntersection(Vision.hkvVec3(50, 20, 250), Vision.hkvVec3(1, 0, 0))
  ///       if isHit == true then 
  ///         Debug:PrintLine("Intersection time: " ..intersectionTime)
  ///         Debug:PrintLine("Intersection point: " ..intersectionPoint)
  ///       end
  ///     end
  ///   \endcode
  ///
  /// \sa hkvAlignedBBox::getLineSegmentIntersection
	multiple getRayIntersection(hkvVec3 rayStartPos, hkvVec3 rayDir);

  /// \brief
  ///   Computes the intersection of a line segment and the surface of the box.
  ///
  /// If the line segment's start and end point are both within the box, no intersection will be reported.
  /// If you need to handle these cases as intersections, test first, whether the box contains the start 
  /// position already.
  ///
  /// \param lineStartPos
  ///   The start position of the line.
  ///
  /// \param lineEndPos
  ///   The end position of the line.
  ///
	/// \return
	///   The first return parameter is a boolean that indicates whether the line segment intersects with the box.
  ///   In case of an intersection the next two return parameters are:    
	///   - HitFraction [number]: Fraction along the line, where the intersection point is located. This value will always be between 0 and 1.
	///   - IntersectionPoint [hkvVec3]: Point (position) of intersection.
  ///   lineStartPos + (lineEndPos - lineStartPos) * HitFraction == IntersectionPoint
  ///
  /// \par Example
  ///   \code
  ///     function OnThink(self)
	///       local bbox = entity:GetBoundingBox()
	///       local isHit, hitFraction, intersectionPoint = bbox.getLineSegmentIntersection(Vision.hkvVec3(50, 20, 250), Vision.hkvVec3(200, 20, 44))
  ///       if isHit == true then 
  ///         Debug:PrintLine("Hit fraction: " ..hitFraction)
  ///         Debug:PrintLine("Intersection point: " ..intersectionPoint)
  ///       end
  ///     end
  ///   \endcode
  ///  
  /// \sa hkvAlignedBBox::getRayIntersection
	multiple getLineSegmentIntersection(hkvVec3 lineStartPos, hkvVec3 lineEndPos);

  /// @}
  /// @name Utility Functions
  /// @{

  /// \brief  
	///   Returns the squared distance of a given point to this box.
	///
  /// \note 
	///   For points that are inside the box this function returns zero.
	///
  /// \param vector 
	///   The point for which the distance is computed.
	///
  /// \return 
	///   The (min) squared distance to the specified point.
	///
  /// \see getDistanceTo
  number getDistanceToSquared(hkvVec3 vector);
  
  /// \brief 
	///   Returns the squared distance of the given box to this box.
	///
  /// \note 
	///   If the two boxes overlap, zero is returned.
	///
  /// \param box 
	///   The box for which the distance is computed.
	///
  /// \return 
  ///   The (min) squared distance between the two boxes.
	///
  /// \see getDistanceTo
  number getDistanceToSquared(hkvAlignedBBox box);

  /// \brief 
	///   Returns the distance of a given point to this box.
	///
  /// \note 
	///   For points that are inside the box this function returns zero.
	///
  /// \param vector
	///   The point for which the distance is computed.
	///
  /// \return 
	///   The (min) distance to the specified point.
	///
  /// \see getDistanceToSquared
  number getDistanceTo(hkvVec3 vector);

  /// \brief  
	///   Returns the distance of the given box to this box.
	///
  /// \note 
	///   If the two boxes overlap, zero is returned.
	///
  /// \param box 
	///   The box for which the distance is computed.
	///
  /// \return 
	///   The (min) distance between the two boxes.
	///
  /// \see getDistanceToSquared
  number getDistanceTo(hkvAlignedBBox box);

	/// \brief
  ///   Returns the distance of the given sphere to this box.
  ///
  /// \note
	///   If the two objects overlap, zero is returned.
  ///
  /// \param sphere
  ///   The sphere for which the distance is computed.
	///
	/// \return
	///   The (min) distance to the specified sphere.
  number getDistanceTo(hkvBoundingSphere sphere);

	/// \brief
  ///   Returns vector clamped to the volume of the box.
  ///
	/// \note
  ///   If vector is already inside the box, it will be returned unchanged.
  ///   If vector is outside the box, its components will be clamped to the min and max extents of the box.
  ///
  /// \param vector
  ///   The point which shall be clamped to the box's volume.
  /// 
	/// \return
	///   The specified point clamped to the box's volume.
  hkvVec3 getClampedPosition(hkvVec3 vector);
  
  /// \brief 
	///   Clone the current bounding box.
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
  ///     -- create a new bounding box:
  ///     local bbox1 = Vision.hkvAlignedBBox(-1,-1,-1, 1,1,1)
  ///
  ///     -- this will not copy the content (just another pointer to it):
  ///     local bbox2 = bbox1
  ///
  ///     -- this will affect access to 'bbox2' and 'bbox1' since the pointers share the same object:
  ///     bbox2:set(-2,-2,-2, 2,2,2)
  ///
  ///     -- this assertion will not fail
  ///     Vision.Assert(bbox1 == bbox2, "When this assertions fails 'bbox1' and 'bbox2' are pointing to different bounding boxes.")
  ///
  ///     -- in order to copy the content, you have to clone:
  ///     local bbox3 = bbox1:clone()
  ///     bbox3:set(-3,-3,-3, 3,3,3)
  ///
  ///     -- this assertion will not fail
  ///     Vision.Assert(bbox1 ~= bbox3, "When this assertions fails 'bbox1' and 'bbox3' are pointing to the same bounding box.")
  ///
  ///   \endcode
  hkvAlignedBBox clone();
  
  /// @}
  /// @name Operators
  /// @{
  
  /// \brief 
	///   Comparison operator. Checks whether two boxes are equal.
	///
  /// \param other 
	///   The other bounding box.
	///
  /// \return 
	///   True if the bounding boxes are equal within Vision.MATH_DEFAULT_EPSILON, otherwise false.
  boolean __eq (hkvAlignedBBox other);

  /// @}
  /// @name Public Members
  /// @{
  
  /// \brief 
	///   Minimum corner of the bounding box.
  hkvVec3 m_vMin;
  
  /// \brief 
	///   Maximum corner of the bounding box.
  hkvVec3 m_vMax;
  
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
