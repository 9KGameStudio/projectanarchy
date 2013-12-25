/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VLUA_APIDOC

class hkvVec4
{
public:
  %extend 
  {
    hkvVec4()
    {
      return new hkvVec4(0.0f);
    }

    hkvVec4(const hkvVec4& rhs)
    {
      return new hkvVec4(rhs);
    } 
  }

  explicit hkvVec4(float xyzw);
  hkvVec4(float _x, float _y, float _z, float _w);

  void setZero(); 
  void set(float xyzw);
  void set(float _x, float _y, float _z, float _w);

  const hkvVec2 getAsVec2() const;
  const hkvVec3 getAsVec3() const;

  bool isZero(float fEpsilon = HKVMATH_DEFAULT_EPSILON) const;
  bool isIdentical(const hkvVec4& rhs) const;
  bool isEqual(const hkvVec4& rhs, float fEpsilon) const;
  bool isValid() const;
  bool isNormalized(float fEpsilon = HKVMATH_DEFAULT_EPSILON) const;

  float dot(const hkvVec4& rhs) const;
  float getLength() const;
  float getLengthSquared() const;
  float getLengthInverse() const;
  float getLengthAndNormalize();
  void normalize();
  const hkvVec4 getNormalized () const;
  
  %extend 
  {
    bool normalizeIfNotZero(float fEpsilon = HKVMATH_SMALL_EPSILON)
    {
      return self->normalizeIfNotZero(fEpsilon)==HKV_SUCCESS;
    }
  }
  
  float normalizedEnsureUnitLength(float fEpsilon = HKVMATH_EPSILON, const hkvVec4& vFallback = hkvVec4(1.0f, 0.0f, 0.0f, 0.0f));
  float getDistanceTo(const hkvVec4& rhs) const;
  float getDistanceToSquared(const hkvVec4& rhs) const;

  %extend 
  {
    bool setLength(float f, float fEpsilon = HKVMATH_SMALL_EPSILON)
    {
      return self->setLength(f, fEpsilon)==HKV_SUCCESS;
    }
  }

  void setMin(const hkvVec4& rhs);
  void setMax(const hkvVec4& rhs);
  void clampTo(const hkvVec4& MinVal, const hkvVec4& MaxVal);
  void setMin(const hkvVec4& lhs, const hkvVec4& rhs);
  void setMax(const hkvVec4& lhs, const hkvVec4& rhs);
  void setClamped(const hkvVec4& val, const hkvVec4& MinVal, const hkvVec4& MaxVal);
  void setAbs(const hkvVec4& val);

  void setInterpolate(const hkvVec4& lhs, const hkvVec4& rhs, float t);

  %extend 
  {
    hkvVec4 clone() const
    {
      return *self; //will call the copy constructor
    }
  }

  void negate();

  const hkvVec4 operator- (void) const;

  const hkvVec4 compMul(const hkvVec4& rhs) const;
  const hkvVec4 compDiv(const hkvVec4& rhs) const;

  %extend 
  {
    const hkvVec4 compMul(float x, float y, float z, float w)
    {
      return self->compMul(hkvVec4(x, y, z, w));
    }
    
    const hkvVec4 compDiv(float x, float y, float z, float w)
    {
      return self->compDiv(hkvVec4(x, y, z, w));
    }

    bool __eq (const hkvVec4* pRhs) const
    {
      if(pRhs == NULL) 
        return false;
      return pRhs->isEqual(*self, HKVMATH_DEFAULT_EPSILON);
    }

    hkvVec4 __add (const hkvVec4* pRhs)
    {
      if(pRhs == NULL) 
        return *self;
      return (*self + *pRhs);
    }

    hkvVec4 __sub (const hkvVec4* pRhs)
    {
      if(pRhs == NULL)  
        return *self;
      return (*self - *pRhs);
    }

    hkvVec4 __mul (float f)
    {
      return (*self * f);
    }

    hkvVec4 __div (float f)
    {
      return (*self / f);
    }
    
    float __getitem__(int iElem)
    {
      if((iElem >= 0) && (iElem < 4))
        return self->data[iElem];  
      return 0;
    }
    
    void __setitem__(int iElem, float fValue)
    {
      if((iElem >= 0) && (iElem < 4))
        self->data[iElem] = fValue;
    }
  }

  float x;
  float y;
  float z;
  float w;
};

//add lua tostring and concat operators
VSWIG_CREATE_CONCAT(hkvVec4, 128, "[%1.2f,%1.2f,%1.2f,%1.2f]", self->x, self->y, self->z, self->w)
VSWIG_CREATE_TOSTRING(hkvVec4, "hkvVec4: [%1.2f,%1.2f,%1.2f,%1.2f]", self->x, self->y, self->z, self->w)

#else

/// \brief 
///   Wrapper class for the 4-component vector hkvVec4.
///
/// \note 
///   Since all wrappers classes are handled as pointers in Lua you cannot copy its content by assignment. 
///   Please take a look at the code samples.
///
/// \par Example
///   \code
///     -- create a new vector:
///     local vec1 = Vision.hkvVec4(1, 3, 7, 5)
///
///     -- this will not copy the vectors content (just another pointer to it):
///     local vec2 = vec1
///
///     -- this will affect access to vec1 and vec2 since the pointers share the same object:
///     vec2.x = 10
///     -- this assertion will not fail
///     Vision.Assert(vec1 == vec2, "When this assertions fails vec1 and vec2 are pointing to different vectors.")
///
///     -- in order to copy the content to another vector, you have to clone:
///     local vec4 = vec1:clone()
///
///     -- or create a new vector using the copy constructor
///     local vec4 = Vision.hkvVec4(vec1)
///
///     -- if you have problems comparing vectors you can use 'isEqual' and specify an alternative epsilon.
///     -- The comparison operator uses Vision.MATH_DEFAULT_EPSILON (0.00001), but you can use any arbitrary value.
///     local result = pos1:isEqual(pos2, Vision.MATH_HUGE_EPSILON) -- tolerance will be 0.001
///   \endcode
/// \see clone
class hkvVec4
{
public:

  /// @name Initialization
  /// @{
  
  /// \brief 
  ///   Default Constructor. Initializes all data to zero.
  hkvVec4();

  /// \brief 
  ///   Copy Constructor.
  ///
  /// \param other 
  ///   The vector to copy.
  hkvVec4(hkvVec4 other);
  
  /// \brief 
  ///   Constructor with single value initialization.
  ///
  /// \param xyzw
  ///   The value for the x, y, z and w axis.
  hkvVec4(number xyzw);
  
  /// \brief
  ///   Constructor with x,y,z and w value initialization.
  ///
  /// \param x 
  ///   Initial value for x.
  ///
  /// \param y 
  ///   Initial value for y.
  ///
  /// \param z 
  ///   Initial value for z.
  ///
  /// \param w
  ///   Initial value for w.
  hkvVec4(number x, number y, number z, number w);

  /// \brief 
  ///   Sets all components to zero.
  void setZero();
  
  /// \brief 
  ///    Sets all components to the same specified value.
  ///
  /// \param xyzw 
  ///   The value for the components.
  void set(number xyzw);
  
  /// \brief 
  ///   Sets all components to a specified value.
  ///
  /// \param x 
  ///   Value for x.
  ///
  /// \param y
  ///   Value for y.
  ///
  /// \param z 
  ///   Value for z.
  ///
  /// \param w 
  ///   Value for w.
  void set(number x, number y, number z, number w);

  /// @}
  /// @name Conversions
  /// @{
  
  /// \brief
  ///   Returns a hkvVec2 with the same values for x and y.
  hkvVec2 getAsVec2();

  /// \brief
  ///   Returns a hkvVec3 with the same values for x, y and z.
  hkvVec4 getAsVec3();

  /// @}
  /// @name Checks
  /// @{

  /// \brief 
  ///   Checks whether this vector is zero (0,0,0,0 - within some epsilon range).
  ///
  /// \param epsilon (\b optional) 
  ///   Specify an epsilon for this range (default is Vision.MATH_DEFAULT_EPSILON).
  ///
  /// \return 
  ///   True if zero within the specified epsilon, otherwise false.
  boolean isZero(number epsilon = Vision.MATH_DEFAULT_EPSILON);

  /// \brief 
  ///   Checks whether the two vectores are identical.
  ///
  /// \param other 
  ///   The vector to check.
  ///
  /// \return 
  ///   True if identical (without epsilon), otherwise false.
  boolean isIdentical(hkvVec4 other);
  
  /// \brief 
  ///   Checks whether the two vectors are equal (within some epsilon range).
  ///
  /// \param other 
  ///   The vector to check.
  ///
  /// \param epsilon 
  ///   Specify an epsilon for this range (e.g Vision.MATH_LARGE_EPSILON).
  ///
  /// \return 
  ///   True if equal within the given epsilon, otherwise false.
  boolean isEqual(hkvVec4 other, number epsilon);

  /// \brief  
  ///   Checks whether all components are valid floats.
  ///
  /// \return 
  ///   True if any component is +/- infinity or NaN, false otherwise.
  boolean isValid();
  
  /// \brief 
  ///   Checks whether this vector has a length of 1, within some epsilon range.
  ///
  /// \param epsilon (\b optional) 
  ///   The maximum deviation that is allowed (default is Vision.MATH_DEFAULT_EPSILON).
  boolean isNormalized(number epsilon = Vision.MATH_DEFAULT_EPSILON);

  /// @}
  /// @name Utility Methods
  /// @{
  
  /// \brief
  ///   Returns the dot product of two vectors.
  ///
  /// \param other 
  ///   The other vector for the dot product operation.
  ///
  /// \return 
  ///   The dot product: self.x*other.x + self.y*other.y + self.z*other.z + self.w*other.w
  number dot(hkvVec4 other);

  /// \brief
  ///   Returns the length of this vector.
  ///
  /// \note 
  ///   Prefer to use getLengthSquared, if you only want to compare relative lengths,
  ///   since the computation of the squared length does not require a sqrt.
  ///
  /// Prefer to use getLengthInverse if you want to divide other values by the length,
  /// since that function might be implemented more efficiently.
  ///
  /// \return 
  ///   The length of the vector.
  ///
  /// \see hkvVec4::getLengthSquared
  ///
  /// \see hkvVec4::getLengthInverse
  ///
  /// \see hkvVec4::getDistanceTo
  ///
  /// \see hkvVec4::getDistanceToSquared
  number getLength();
  
  /// \brief  
  ///   Returns the squared length of this vector.
  ///
  /// \note 
  ///   It is more efficient to use the squared length, instead of the real length,
  ///   if one only wants to compare vectors for their relative length.
  /// 
  /// \return 
  ///   The squared length of the vector.
  ///
  /// \see hkvVec4::dot
  ///
  /// \see hkvVec4::getLength
  ///
  /// \see hkvVec4::getDistanceToSquared
  number getLengthSquared();
  
  /// \brief 
  /// Returns (1.0f / getLength()), but computes this more efficiently.
  ///
  /// \note 
  ///   If the length is supposed to be used for normalization purposes, prefer this function.
  ///
  /// \return 
  ///   The inverse length of the vector.
  ///
  /// \see hkvVec4::getLength
  number getLengthInverse();

  /// \brief
  ///   Returns the current length and then normalizes this vector. More efficient than calling 'getLength' and then 'normalize'.
  ///
  /// This function will only compute the length once, whereas 'getLength' followed by 'normalize' will do this twice.
  ///
  /// \sa hkvVec4::getLength
  ///
  /// \sa hkvVec4::getLengthInverse
  ///
  /// \sa hkvVec4::normalize
  number getLengthAndNormalize();

  /// \brief 
  ///   Normalizes this vector - makes sure its length is 1.
  ///
  /// \note 
  ///   If the vector was zero or degenerate before, it will have invalid data afterwards.
  ///   Normalizing zero-length vectors will give undefined results and may be very different on each platform.
  ///
  /// \see hkvVec4::getLength
  ///
  /// \see hkvVec4::getLengthInverse
  ///
  /// \see hkvVec4::getNormalized
  void normalize();
  
  /// \brief 
  ///   Returns a normalized copy of this vector. The behavior is identical to normalize.
  ///
  /// \return 
  ///   The normalized vector.
  ///
  /// \see hkvVec4::normalize
  hkvVec4 getNormalized();

  /// \brief  
  ///   Normalizes the vector, if its length is non-zero. Otherwise the vector will stay unmodified.
  ///
  /// \return 
  ///   If normalization was not possible, it returns false otherwise true.
  ///
  /// \note 
  ///   In case of failure the vector will not be modified. That means the vector will either be all zero
  ///   (and thus zero-length) or some component is invalid (+/-Infinity or NaN).
  ///
  /// \see hkvVec4::normalize
  ///
  /// \see hkvVec4::normalizedEnsureUnitLength
  boolean normalizeIfNotZero();

  /// \brief
  ///   Normalizes this vector under all circumstances and returns its previous length.
  ///
  /// If the vector can not be normalized, the vector is set to fallback.
  /// In this case the function will return 0, as the vectors previous length was 0 (or invalid).
  /// If the vector can be normalized, the previous length is returned.
  ///
  /// \param epsilon 
  ///   If all values are in range [-epsilon; +epsilon] the vector is considered to be 'zero'.
  ///
  /// \param fallback
  ///   The fall back is used if the vector cannot be normalized.
  ///
  /// \sa hkvVec4::normalize
  ///
  /// \sa hkvVec4::normalizeIfNotZero
  number normalizedEnsureUnitLength(number epsilon = MATH_EPSILON, hkvVec4 fallback = hkvVec4(1.0, 0.0, 0.0, 0.0));

  /// \brief 
  ///   Returns the distance between two vectos.
  ///
  /// \note 
  ///   Prefer to use getDistanceToSquared if you only need to compare relative distances,
  ///   as this is more efficient to compute.
  ///
  /// \param vector 
  ///   The point to which to compute the distance.
  ///
  /// \see hkvVec4::getDistanceToSquared
  ///
  /// \see hkvVec4::getLength
  number getDistanceTo(hkvVec4 vector);
  
  /// \brief 
  ///   Returns the squared distance between this vector and the other.
  ///
  /// \note 
  ///   This is more efficient to use than getLength, if only relative comparisons are needed.
  ///   E.g. to check whether the distance between v1 and v2 is larger than between v1 and v3.
  ///
  /// \param vector 
  ///   The point to which to compute the distance.
  ///
  /// \see hkvVec4::getDistanceTo
  ///
  /// \see hkvVec4::getLengthSquared
  number getDistanceToSquared(hkvVec4 vector);

  /// \brief 
  ///   Normalizes this vector and then multiplies it by the supplied value.
  ///
  /// \note
  ///   In case of a failure the vector stays unchanged and the function acts identical to normalizeIfNotZero.
  ///
  /// \param length 
  ///   The new length to set.
  ///
  /// \return 
  ///   Returns false if the vector could not be normalized, otherwise true.
  ///
  /// \see hkvVec4::normalizeIfNotZero  
  boolean setLength(number length);

  /// \brief 
  ///   Each component of this vector is set to the specified minimum (if below the minimum).
  ///
  /// \param min 
  ///   The min values for this vector.
  ///
  /// \see setMax  
  ///
  /// \see clampTo
  void setMin(hkvVec4 min);
  
  /// \brief 
  ///   Each component of this vector is set to the specified maximum (if above the maximum).
  ///
  /// \param max 
  ///   The max values for this vector.
  ///
  /// \see setMin
  ///
  /// \see clampTo
  void setMax(hkvVec4 max);
  
  /// \brief
  ///   Each component of this vector clamped to the min/max range defined by minVal and maxVal.
  ///
  /// \param minVal 
  ///   The min values for this vector.
  ///
  /// \param maxVal 
  ///   The max values for this vector.
  ///
  /// \see setMin
  ///
  /// \see setMax
  void clampTo(hkvVec4 minVal, hkvVec4 maxVal);
  
  /// \brief
  ///   Each component of this vector is set to the minimum of lhs and the corresponding component of rhs.
   void setMin(hkvVec4 lhs, hkvVec4 rhs);

  /// \brief
  ///   Each component of this vector is set to the maximum of lhs and the corresponding component of rhs.
  void setMax(hkvVec4 lhs, hkvVec4 rhs);

  /// \brief
  ///   Each component of this vector is set to val clamped to the min/max range defined by minVal and maxVal.
  void setClamped(hkvVec4 val, hkvVec4 minVal, hkvVec4 maxVal);
  
  /// \brief 
  ///   All components of this vector are set to the absolute value of the corresponding component of val.
  void setAbs(hkvVec4 value);

  /// \brief 
  ///   Returns the cross product of this vector and other, using a right-handed coordinate system.
  ///
  /// \note 
  ///   The cross product is not commutative, that means 
  ///   v1.cross(v2) will yield a different result than
  ///   v2.cross(v1) (it will be the negated vector).
  ///
  /// \param other 
  ///   The second vector to compute the cross product with.
  ///
  /// \return 
  ///   The cross product.
  hkvVec4 cross(hkvVec4 other);
  
  /// \brief 
  ///   Sets this vector to the linear interpolation between two vectors.
  ///
  /// \param start 
  ///   The first vector for the interpolation.
  /// 
  /// \param stop 
  ///   The second vector for the interpolation.
  ///
  /// \param value 
  ///   The interpolation factor between 0 and 1.
  void setInterpolate(hkvVec4 start, hkvVec4 stop, number value);

  /// \brief 
  ///   Makes this vector orthogonal to the specified normal if possible. 
  ///
  /// \param normal 
  ///   The normal used for the operation (will be normalized internally)!
  ///
  /// \note 
  ///   This vector will not be normalized afterwards.
  ///
  /// If this vector and specified normal are parallel, the resulting vector will be the zero vector.
  ///
  /// \see hkvVec4::getOrthogonalVector
  void makeOrthogonalTo(hkvVec4 normal);
  
  /// \brief 
  ///   Returns some arbitrary vector that is orthogonal to this vector.
  ///
  /// \note 
  ///   There is no guarantee into which direction the resulting vector will point, other
  ///   than that it will be orthogonal to this vector.
  ///
  /// \return 
  ///   A vector which is orthogonal to this.
  ///
  /// \see hkvVec4::makeOrthogonalTo
  hkvVec4 getOrthogonalVector();

  /// \brief
  ///   Reflects this vector at the plane that goes through the origin with the given normal.
  ///
  /// \param normal 
  ///   The normal of a plane through the origin, which will be used to reflect this vector.
  ///
  /// \see hkvVec4::getReflected
  void reflect(hkvVec4 normal);
  
  /// \brief 
  ///   Returns a vector that is reflected at the plane that goes through the origin with the given normal.
  ///
  /// \param normal 
  ///   The normal of a plane through the origin, which will be used to reflect this vector.
  ///
  /// \return 
  ///   The reflected vector.
  ///
  /// \see hkvVec4::reflect
  hkvVec4 getReflected (hkvVec4 normal);

  /// \brief
  ///   Returns the angle (in degrees) between the two vectors. The vectors need not to be normalized.
  number getAngleBetween(hkvVec3 rhs);

  /// \brief 
  ///   Clones the current vector.
  ///
  /// \return 
  ///   An identical hkvVec4.
  ///
  /// \note 
  ///   Since all wrappers classes are handled as pointers in Lua you cannot copy its content by assignment.
  ///   Please take a look at the code samples.
  ///
  /// \par Example
  ///   \code
  ///     -- create a new vector:
  ///     local vec1 = Vision.hkvVec4(1, 2, 3, 4)
  ///
  ///     -- this will not copy the content of vec1 (just another pointer to it):
  ///     local vec2 = vec1
  ///
  ///     -- this will affect access to vec1 and vec2 since the pointers share the same object:
  ///     vec2.x = 5
  ///     -- this assertion will not fail
  ///     Vision.Assert(vec1 == vec2, "When this assertions fails 'vec1' and 'vec2' are pointing to different vectors.")
  ///
  ///     -- in order to copy the content to another vector, you have to clone:
  ///     local vec3 = vec1:clone()
  ///
  ///     -- this will affect only vec3:
  ///     vec3.y = 6
  ///     -- this assertion will not fail
  ///     Vision.Assert(vec1 ~= vec3, "When this assertions fails 'vec1' and 'vec3' are pointing to same vector.")
  ///
  ///     -- instead of clone you can also create a new vector using the copy constructor:
  ///     local vec4 = Vision.hkvVec4(vec1)
  ///   \endcode
  hkvVec4 clone();

  /// @}
  /// @name Operators and Math Methods
  /// @{

  /// \brief 
  ///   Negates this vector - flips the sign of each component.
  void negate();

  /// \brief 
  ///   Returns a vector that is the component-wise multiplication of this vector and the specified vector.
  ///
  /// \param other 
  ///   The other vector used for the component-wise multiplication.
  ///
  /// \return 
  ///   The result vector.
  hkvVec4 compMul(hkvVec4 other);
  
  /// \brief 
  ///   Returns a copy of a component-wise multiplied vector.
  ///
  /// \param x
  ///   The x multiplication factor.
  ///
  /// \param y 
  ///   The y multiplication factor.
  ///
  /// \param z
  ///   The z multiplication factor.
  ///
  /// \param w
  ///   The w multiplication factor.
  ///
  /// \return 
  ///   The result vector.
  hkvVec4 compMul(number x, number y, number z, number w);
    
  /// \brief 
  ///   Returns a vector that is the component-wise division of this vector and the specified vector.
  ///
  /// \param other
  ///   The other vector used for the component-wise multiplication.
  ///
  /// \return 
  ///   The result vector.
  hkvVec4 compDiv(hkvVec4 other);

  /// \brief 
  ///   Returns a copy of a component-wise devided vector.
  ///
  /// \param x 
  ///   The x division factor.
  /// \param y 
  ///   The y division factor.
  ///
  /// \param z 
  ///   The z division factor.
  ///
  /// \param w
  ///   The w division factor.
  ///
  /// \return 
  ///   The result vector.
  hkvVec4 compDiv(number x, number y, number z, numnber w);
  
  /// \brief 
  ///   Unary - Operator. Returns a negated copy of this vector (local vec = -anotherVec).
  ///
  /// \return 
  ///   The negated copy.
  hkvVec4 __unm();

  /// \brief 
  ///   Operator == for hkvVec4 comparison.
  ///
  /// \note 
  ///   Use isEqual for a custom epsilon.
  ///
  /// \param other 
  ///   The other vector to compare with.
  ///
  /// \return 
  ///   True if the vectors are equal within Vision.MATH_DEFAULT_EPSILON, otherwise false.
  ///
  /// \see isEqual
  boolean __eq(hkvVec4 other);

  /// \brief 
  ///   Operator + for hkvVec4, which adds the two hkvVec4 component-wise.
  ///
  /// \param other 
  ///   The hkvVec4 to add.
  ///
  /// \return 
  ///  The result hkvVec4.
  hkvVec4 __add(hkvVec4 other);

  /// \brief 
  ///   Operator - for hkvVec4, which subtracts the two hkvVec4 component-wise.
  ///
  /// \param other 
  ///   The hkvVec4 to subtract.
  ///
  /// \return 
  ///   The result hkvVec4.
  hkvVec4 __sub (hkvVec4 other);

  /// \brief
  ///   Operator * to multiply each element by a scalar value.
  ///
  /// \param value 
  ///   The scalar value multiplier.
  ///
  /// \return 
  ///   The result vector.
  hkvVec4 __mul(number value);

  /// \brief 
  ///   Operator / to devide each element by a scalar value.
  ///
  /// \param value 
  ///   The scalar value used for the division.
  ///
  /// \return
  ///   The result vector.
  hkvVec4 __div (number value);
  
  /// \brief 
  ///   Returns a specific component from the vector (local vec = vec[0]).
  ///
  /// \param component 
  ///   The index, of the component (0 = x, 1 = y, 2 = z, 3 = w).
  ///
  /// \return 
  ///   The value of the requested component.
  number __getitem(number component);
  
  /// \brief 
  ///   Sets a specific component of the vector.
  ///
  /// \param component 
  ///   The index, of the component (0 = x, 1 = y, 2 = z, 3 = w).
  ///
  /// \param value 
  ///   The value to be set.
  void __setitem(number component, number value);
  
  /// @}
  /// @name Public Members
  /// @{
  
  /// \brief 
  ///  The x component of the vector.
  number x;
  
  /// \brief 
  ///   The y component of the vector.
  number y;
  
  /// \brief 
  ///   The z component of the vector.
  number z; 

  /// \brief 
  ///   The w component of the vector.
  number w; 
    
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
