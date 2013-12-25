/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VLUA_APIDOC

class hkvVec2
{
public:
  %extend 
  {
    hkvVec2()
    {
      return new hkvVec2(0.0f);
    }

    hkvVec2(const hkvVec2& rhs)
    {
      return new hkvVec2(rhs);
    } 
  }

  explicit hkvVec2(float xy);
  hkvVec2(float _x, float _y);

  void setZero();
  void set(float xy);
  void set(float _x, float _y);

  const hkvVec3 getAsVec3(float _z) const;
  const hkvVec4 getAsVec4(float _z, float _w) const;

  bool isZero(float fEpsilon = HKVMATH_DEFAULT_EPSILON) const;
  bool isIdentical(const hkvVec2& rhs) const;
  bool isEqual(const hkvVec2& rhs, float fEpsilon) const;
  bool isValid() const;
  bool isNormalized(float fEpsilon = HKVMATH_DEFAULT_EPSILON) const;

  float dot(const hkvVec2& rhs) const;
  float getLength() const;
  float getLengthSquared() const;
  float getLengthInverse() const;
  float getLengthAndNormalize();
  void normalize();
  const hkvVec2 getNormalized() const;
  
  %extend 
  {
    bool normalizeIfNotZero()
    {
      return self->normalizeIfNotZero()==HKV_SUCCESS;
    }
  }
  
  float normalizedEnsureUnitLength(float fEpsilon = HKVMATH_EPSILON, const hkvVec2& vFallback = hkvVec2(1.0f, 0.0f));
  float getDistanceTo(const hkvVec2& rhs) const;
  float getDistanceToSquared(const hkvVec2& rhs) const;

  %extend 
  {
    bool setLength(float f, float fEpsilon = HKVMATH_SMALL_EPSILON)
    {
      return self->setLength(f, fEpsilon)==HKV_SUCCESS;
    }
  }

  void setMin(const hkvVec2& rhs);
  void setMax(const hkvVec2& rhs);
  void clampTo(const hkvVec2& MinVal, const hkvVec2& MaxVal);

  void setMin(const hkvVec2& lhs, const hkvVec2& rhs);
  void setMax(const hkvVec2& lhs, const hkvVec2& rhs);
  void setClamped(const hkvVec2& val, const hkvVec2& MinVal, const hkvVec2& MaxVal);

  void setAbs(const hkvVec2& val);

  void setInterpolate(const hkvVec2& lhs, const hkvVec2& rhs, float t);

  %extend 
  {
    void makeOrthogonalTo(const hkvVec2& vNormal)
    {
      self->makeOrthogonalTo(vNormal.getNormalized());
    }
  }

  const hkvVec2 getOrthogonalVector() const;

  void reflect(const hkvVec2& vNormal);
  const hkvVec2 getReflected(const hkvVec2& vNormal) const;

  %extend 
  {
    hkvVec2 clone() const
    {
      return *self; //will call the copy constructor
    }
  }

  void negate();

  const hkvVec2 operator- (void) const;

  const hkvVec2 compMul(const hkvVec2& rhs) const;
  const hkvVec2 compDiv(const hkvVec2& rhs) const;

  %extend 
  {
    const hkvVec2 compMul(float x, float y)
    {
      return self->compMul(hkvVec2(x, y));
    }
    
    const hkvVec2 compDiv(float x, float y)
    {
      return self->compDiv(hkvVec2(x, y));
    }

    bool __eq(const hkvVec2* pRhs) const
    {
      if (pRhs == NULL) 
        return false;
      return pRhs->isEqual(*self, HKVMATH_DEFAULT_EPSILON);
    }

    hkvVec2 __add(const hkvVec2* pRhs)
    {
      if (pRhs == NULL)  
        return *self;
      return (*self + *pRhs);
    }

    hkvVec2 __sub(const hkvVec2* pRhs)
    {
      if (pRhs == NULL) 
        return *self;
      return (*self - *pRhs);
    }

    hkvVec2 __mul(float f)
    {
      return (*self * f);
    }

    hkvVec2 __div(float f)
    {
      return (*self / f);
    }

    float __getitem__(int iElem)
    {
      if ((iElem >= 0) && (iElem < 2))
        return self->data[iElem];
      return 0;
    }
    
    void __setitem__(int iElem, float fValue)
    {
      if ((iElem >= 0) && (iElem < 2)) 
        self->data[iElem] = fValue;
    }
  }

  float x;
  float y;
};

//add lua tostring and concat operators
VSWIG_CREATE_CONCAT(hkvVec2, 128, "[%1.2f,%1.2f]", self->x, self->y)
VSWIG_CREATE_TOSTRING(hkvVec2, "hkvVec2: [%1.2f,%1.2f]", self->x, self->y)

#else

/// \brief 
///   Wrapper class for the 2-component vector hkvVec2.
///
/// \note
///   Since all wrappers classes are handled as pointers in Lua you
///   cannot copy its content by assignment. Please take a look at the code samples.
///
/// \par Example
///   \code
///     -- create a new vector:
///     local vec1 = Vision.hkvVec2(1, 3)
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
///     local vec3 = vec1:clone()
///
///     -- or create a new vector using the copy constructor
///     local vec4 = Vision.hkvVec2(vec1)
///
///     -- if you have problems comparing vectors you can use 'isEqual' and specify an alternative epsilon.
///     -- The comparison operator uses Vision.MATH_DEFAULT_EPSILON (0.00001), but you can use any arbitrary value.
///     local result = pos1:isEqual(pos2, Vision.MATH_DEFAULT_EPSILON) -- tolerance will be 0.001
///   \endcode
///
/// \see clone
class hkvVec2
{
public:

  /// @name Initialization
  /// @{
  
  /// \brief 
  ///   Default Constructor. Initializes all data to zero.
  hkvVec2();

  /// \brief 
  ///   Copy Constructor.
  ///
  /// \param other 
  ///   The vector to copy.
  hkvVec2(hkvVec2 other);
  
  /// \brief 
  ///   Constructor with single value initialization.
  ///
  /// \param xy
  ///   The value for the x, y.
  hkvVec2(number xy);
  
  /// \brief 
  ///    Constructor with x and y value initialization.
  ///
  /// \param x 
  ///   Initial value for x.
  ///
  /// \param y
  ///   Initial value for y.
  hkvVec2 (number x, number y);

  /// \brief 
  ///   Sets all components to zero.
  void setZero();
  
  /// \brief 
  ///   Sets all components to the same specified value.
  ///
  /// \param xy
  ///   The value for the components.
  void set(number xy);
  
  /// \brief 
  ///   Sets all components to a specified value.
  ///
  /// \param x 
  ///   Value for x.
  /// 
  /// \param y 
  ///   Value for y.
  void set(number x, number y);

  /// @}
  /// @name Conversions
  /// @{

  /// \brief
  ///   Returns a hkvVec3 with the same values for x and y, and z set to _z.
  ///
  /// \param _z
  ///   The value that will be stored in the z-component of the hkvVec3.
  hkvVec3 getAsVec3(number _z);

  /// \brief
  ///   Returns a hkvVec4 with the same values for x and y, z set to _z and w set to _w.
  ///
  /// \param _z
  ///   The value that will be stored in the z-component of the hkvVec4.
  ///
  /// \param _w
  ///   The value that will be stored in the w-component of the hkvVec4.
  hkvVec4 getAsVec4(number _z, number _w);

  /// @}
  /// @name Checks
  /// @{

  /// \brief 
  ///   Checks whether this vector is zero (0,0 - within some epsilon range).
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
  /// \return 
  ///   True if identical (without epsilon), otherwise false.
  boolean isIdentical(hkvVec2 other);
  
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
  boolean isEqual(hkvVec2 other, number epsilon);

  /// \brief 
  ///   Checks whether all components are valid floats.
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
  ///   The dot product: self.x*other.x + self.y*other.y 
  number dot(hkvVec2 other);

  /// \brief 
  ///   Returns the length of this vector.
  ///
  /// \note 
  ///   Prefer to use getLengthSquared, if you only want to compare relative lengths,
  ///   since the computation of the squared length does not require a sqrt.
  ///   Prefer to use getLengthInverse if you want to divide other values by the length,
  ///   since that function might be implemented more efficiently.
  ///
  /// \return 
  ///   The length of the vector.
  /// 
  /// \see hkvVec2::getLengthSquared
  ///
  /// \see hkvVec2::getLengthInverse
  ///
  /// \see hkvVec2::getDistanceTo
  ///
  /// \see hkvVec2::getDistanceToSquared
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
  /// \see hkvVec2::dot
  ///
  /// \see hkvVec2::getLength
  ///
  /// \see hkvVec2::getDistanceToSquared
  number getLengthSquared();

  /// \brief 
  ///   Returns (1.0 / getLength ()), but computes this more efficiently.
  ///
  /// \note 
  ///   If the length is supposed to be used for normalization purposes, prefer this function.
  ///
  /// \return 
  ///   The inverse length of the vector.
  ///
  /// \see hkvVec2::getLength
  number getLengthInverse();

  /// \brief
  ///   Returns the current length and then normalizes this vector. More efficient than calling 'getLength' and then 'normalize'.
  ///
  /// This function will only compute the length once, whereas 'getLength' followed by 'normalize' will do this twice.
  ///
  /// \sa hkvVec2::getLength
  ///
  /// \sa hkvVec2::getLengthInverse
  ///
  /// \sa hkvVec2::normalize
  number getLengthAndNormalize();

  /// \brief
  ///   Normalizes this vector - makes sure its length is 1.
  ///
  /// \note 
  ///   If the vector was zero or degenerate before, it will have invalid data afterwards.
  ///
  ///   Normalizing zero-length vectors will give undefined results and may be very different on each platform.
  ///
  /// \see hkvVec2::getLength
  ///
  /// \see hkvVec2::getLengthInverse
  ///
  /// \see hkvVec2::getNormalized
  void normalize();

  /// \brief 
  ///   Returns a normalized copy of this vector. The behavior is identical to normalize.
  ///
  /// \return 
  ///   The normalized vector.
  ///
  /// \see hkvVec2::normalize
  hkvVec2 getNormalized();

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
  /// \see hkvVec2::normalize
  ///
  /// \see hkvVec2::normalizedEnsureUnitLength
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
  /// \sa hkvVec2::normalize
  ///
  /// \sa hkvVec2::normalizeIfNotZero
  number normalizedEnsureUnitLength(number epsilon = MATH_EPSILON, hkvVec2 fallback = hkvVec2(1.0, 0.0));

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
  /// \see hkvVec2::getDistanceToSquared
  ///
  /// \see hkvVec2::getLength
  number getDistanceTo(hkvVec2 vector);

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
  /// \see hkvVec2::getDistanceTo
  ///
  /// \see hkvVec2::getLengthSquared
  number getDistanceToSquared(hkvVec2 vector);

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
  /// \see hkvVec2::normalizeIfNotZero
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
  void setMin(hkvVec2 min);
  
  /// \brief 
  ///   Each component of this vector is set to the specified maximum (if above the maximum).
  ///
  /// \param max 
  ///   The max values for this vector.
  ///
  /// \see setMin
  ///
  /// \see clampTo
  void setMax(hkvVec2 max);
  
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
  void clampTo(hkvVec2 minVal, hkvVec2 maxVal);
  
  /// \brief
  ///   Each component of this vector is set to the minimum of lhs and the corresponding component of rhs.
  void setMin(hkvVec2 lhs, hkvVec2 rhs);

  /// \brief
  ///   Each component of this vector is set to the maximum of lhs and the corresponding component of rhs.
  void setMax(hkvVec2 lhs, hkvVec2 rhs);

  /// \brief
  ///   Each component of this vector is set to val clamped to the min/max range defined by minVal and maxVal.
  void setClamped(hkvVec2 val, hkvVec2 minVal, hkvVec2 maxVal);
  
  /// \brief 
  ///   All components of this vector are set to the absolute value of the corresponding component of val.
  void setAbs(hkvVec2 value);
  
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
  ///    The interpolation factor between 0 and 1.
  void setInterpolate (hkvVec2 start, hkvVec2 stop, number value);

  /// \brief 
  ///   Makes this vector orthogonal to the specified normal if possible. 
  ///
  /// \param normal 
  ///   The normal used for the operation (will be normalized internally).
  ///
  /// \note 
  ///   This vector will not be normalized afterwards.
  ///
  /// If this vector and specified normal are parallel, the resulting vector will be the zero vector.
  ///
  /// \see hkvVec2::getOrthogonalVector
  void makeOrthogonalTo(hkvVec2 normal);

  /// \brief
  ///   Returns some arbitrary vector that is orthogonal to this vector.
  ///
  /// There is no guarantee into which direction the resulting vector will point, other
  /// than that it will be orthogonal to this vector.
  ///
  /// \sa hkvVec2::makeOrthogonalTo
  hkvVec2 getOrthogonalVector();

  /// \brief
  ///   Reflects this vector at the plane that goes through the origin with the given normal.
  ///
  /// \param normal 
  ///   The normal of a plane through the origin, which will be used to reflect this vector.
  /// 
  /// \see hkvVec2::getReflected
  void reflect(hkvVec2 normal);

  /// \brief 
  ///   Returns a vector that is reflected at the plane that goes through the origin with the given normal.
  ///
  /// \param normal 
  ///   The normal of a plane through the origin, which will be used to reflect this vector.
  ///
  /// \return 
  ///   The reflected vector.
  ///
  /// \see hkvVec2::reflect
  hkvVec2 getReflected(hkvVec2 normal);

  /// \brief
  ///   Clones the current vector.
  ///
  /// \return 
  ///    An identical hkvVec2.
  ///
  /// \note 
  ///   Since all wrappers classes are handled as pointers in Lua you cannot copy it's content by assignment.
  ///   Please take a look at the code samples.
  ///
  /// \par Example
  ///   \code
  ///     -- create a new vector:
  ///     local vec1 = Vision.hkvVec2(1, 2)
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
  ///     local vec4 = Vision.hkvVec2(vec1)
  ///   \endcode
  hkvVec2 clone();

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
  /// \return The result vector.
  hkvVec2 compMul(hkvVec2 other);
  
  /// \brief 
  ///   Returns a copy of a component-wise multiplyed vector.
  ///
  /// \param x 
  ///   The x multiplication factor.
  ///
  /// \param y 
  ///   The y multiplication factor.
  ///
  /// \return 
  ///   The result vector.
  hkvVec2 compMul(number x, number y);
    
  /// \brief 
  ///   Returns a vector that is the component-wise division of this vector and the specified vector.
  ///
  /// \param other 
  ///   The other vector used for the component-wise multiplication.
  ///
  /// \return 
  ///   The result vector.
  hkvVec2 compDiv(hkvVec2& other);

  /// \brief 
  ///   Returns a copy of a component-wise divided vector.
  ///
  /// \param x
  ///   The x division factor.
  ///
  /// \param y 
  ///   The y division factor.
  ///
  /// \return 
  ///   The result vector.
  hkvVec2 compDiv(number x, number y);
  
  /// \brief 
  ///   Unary - Operator. 
  ///   Returns a negated copy of this vector (local vec = -anotherVec).
  ///
  /// \return 
  ///   The negated copy.
  hkvVec2 __unm();

  /// \brief 
  ///   Operator == for hkvVec2 comparison.
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
  boolean __eq(hkvVec2 other);

  /// \brief 
  ///   Operator + for hkvVec2, which adds the two hkvVec2 component-wise.
  ///
  /// \param other 
  ///   The hkvVec2 to add.
  ///
  /// \return 
  ///   The result hkvVec2.
  hkvVec2 __add(hkvVec2 other);

  /// \brief 
  ///   Operator - for hkvVec2, which subtracts the two hkvVec2 component-wise.
  ///
  /// \param other 
  ///   The hkvVec2 to subtract.
  ///
  /// \return 
  ///   The result hkvVec2.
  hkvVec2 __sub(hkvVec2 other);

  /// \brief
  ///   Operator * to multiply each element by a scalar value.
  ///
  /// \param value
  ///   The scalar value multiplier.
  ///
  /// \return 
  ///   The result vector.
  hkvVec2 __mul(number value);

  /// \brief 
  ///   Operator / to devide each element by a scalar value.
  ///
  /// \param value 
  ///   The scalar value used for the division.
  ///
  /// \return 
  ///   The result vector.
  hkvVec2 __div(number value);

  /// \brief
  ///   Returns a specific component from the vector (local vec = vec[0]).
  ///
  /// \param component 
  ///   The index, of the component (0 = x, 1 = y).
  ///
  /// \return 
  ///   The value of the requested component.
  number __getitem(number component);
  
  /// \brief 
  ///   Sets a specific component of the vector.
  ///
  /// \param component 
  ///   The index, of the component (0 = x, 1 = y).
  ///
  /// \param value 
  ///   The value to be set.
  void __setitem(number component, number value);
  
  /// @}
  /// @name Public Members
  /// @{
  
  /// \brief 
  ///   The x component of the vector.
  number x;
  
  /// \brief 
  ///   The y component of the vector.
  number y;   
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
