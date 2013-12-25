/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VLUA_APIDOC
  
class VScriptUtil_wrapper
{
public:
  VScriptUtil_wrapper();
  ~VScriptUtil_wrapper();

  inline int GetRandInt(int iMaxRand = RAND_MAX);
  inline int GetRandIntSym(int iMaxRand = RAND_MAX_2);

  inline float GetRandFloat(float fMaxRand = 1.0f);
  inline float GetRandFloatSym(float fMaxRand = 1.0f);
  
  inline hkvVec3 GetRandVector(float fMaxX, float fMaxY, float fMaxZ);
  inline hkvVec3 GetRandVectorSym(float fMaxX, float fMaxY, float fMaxZ);
  
  inline float AngleBetweenVectors(const hkvVec3* pVec1, const hkvVec3* pVec2);

  inline hkvMat3 CreateLookAtMatrix( const hkvVec3* pPos, const hkvVec3* pTarget, const hkvVec3* pUp = NULL );

  inline float GetFovX(float fFovY, float fAspectRatio);
  inline float GetFovY(float fFovX, float fAspectRatio);

  inline hkvMat3 EulerToMat3_Rad(float fRoll, float fPitch, float fYaw);
  inline hkvMat3 EulerToMat3_Deg(float fRoll, float fPitch, float fYaw);
  inline hkvMat4 EulerToMat4_Rad(float fRoll, float fPitch, float fYaw);
  inline hkvMat4 EulerToMat4_Deg(float fRoll, float fPitch, float fYaw);

  inline hkvQuat EulerToQuat_Rad(float fRoll, float fPitch, float fYaw);
  inline hkvQuat EulerToQuat_Deg(float fRoll, float fPitch, float fYaw);

  inline hkvVec3 Mat3ToEuler_Rad(const hkvMat3* pMatrix);
  inline hkvVec3 Mat3ToEuler_Deg(const hkvMat3* pMatrix);
  inline hkvVec3 Mat4ToEuler_Rad(const hkvMat4* pMatrix);
  inline hkvVec3 Mat4ToEuler_Deg(const hkvMat4* pMatrix);

  inline hkvVec3 QuatToEuler_Rad(const hkvQuat* pQuat);
  inline hkvVec3 QuatToEuler_Deg(const hkvQuat* pQuat);

  inline float RadToDeg(float radValue);
  inline float DegToRad(float degValue);
  inline bool IsAngleEqualDeg(float fAngle1, float fAngle2, float fEpsilon = HKVMATH_DEFAULT_EPSILON);

  inline float SqrtInverse(float f);
  inline float Root(float f, float fNthRoot);
  inline bool IsPowerOf(unsigned int x, unsigned int base);
  inline bool IsPowerOf2(unsigned int x);
  inline float Log2(float f);
  inline float LogB(float f, float base);

  inline float Sign(float f);
  inline unsigned int PowerOf2_ceil(unsigned int x);
  inline unsigned int PowerOf2_floor(unsigned int x);
  inline float Round(float f);
  inline float Round(float f, unsigned int iDecimalPlaces);
  inline float RoundTo(float f, float fRoundTo);
  inline float Clamp(float f, float fMin, float fMax);
  inline float Saturate(float f);
  inline bool IsInRange(float f, float fMinVal, float fMaxVal);

  inline bool IsZero(float f, float fEpsilon = HKVMATH_LARGE_EPSILON);
  inline bool IsEqual(float f1, float f2, float fEpsilon = HKVMATH_LARGE_EPSILON);
  inline bool IsFiniteNumber(float f);
  inline bool IsNaN(float f);

  inline float Interpolate(float f1, float f2, float fInterpolation);
};

#else

/// \brief 
///   Provides utility functions and is available in Lua as the global instance \b Util.
///
/// \par Example
///   \code
///     function OnThink(self)
///       local origin = Vision.hkvVec3()
///       local matrix = Util:CreateLookAtMatrix(origin, self:GetPosition)
///       ...
///     end
///   \endcode
class VScriptUtil_wrapper 
{
public:

  /// @name Number Utilities
  /// @{
  
  /// \brief Generate a random integer number in the range of [0,maxRand)
  /// \param maxRand (\b optional) The upper limit for your random numbers (excluding the specified number). The limit will be RAND_MAX (=0x7fff) when not specified. 
  /// \return A random integer number in the range of [0,maxRand).
  number GetRandInt(number maxRand = Vision.RAND_MAX);

  /// \brief Generate a random integer number in the symmetrical range of (-maxRand,maxRand)
  /// \param maxRand (\b optional) The upper and lower (negative) limit for your random numbers (excluding the specified number). The limit will be RAND_MAX/2 (=0x3fff) when not specified. 
  /// \return A random integer number in the range of (-maxRand,maxRand).
  number GetRandIntSym(number maxRand = Vision.RAND_MAX_2);

  /// \brief Generate a random float number in the range of [0,maxRand]
  /// \param maxRand (\b optional) The upper limit for your random numbers (including the specified number). The limit will be 1.0 when not specified. 
  /// \return A random float number in the range of [0,maxRand].
  number GetRandFloat(number maxRand = 1);

  /// \brief Generate a random float number in the symmetrical range of [-maxRand,maxRand]
  /// \param maxRand (\b optional) The upper and lower (negative) limit for your random numbers (including the specified number). The limit will be 1.0 when not specified. 
  /// \return A random float number in the range of [-maxRand,maxRand].
  number GetRandFloatSym(number maxRand = 1);
  
  /// \brief Generate a random vector, where each component has a random number in the range of [0, max].
  /// \param maxX The upper limit for your random numbers along the x axis. 
  /// \param maxY The upper limit for your random numbers along the y axis. 
  /// \param maxZ The upper limit for your random numbers along the z axis. 
  /// \return A vector with random values in the range of [0, max].
  hkvVec3 GetRandVector(number maxX, number maxY, number maxZ);

  /// \brief Generate a random vector, where each component has a random number in the symmetrical range of [-max, max].
  /// \param maxX The upper and lower limit for your random numbers along the x axis. 
  /// \param maxY The upper and lower limit for your random numbers along the y axis. 
  /// \param maxZ The upper and lower limit for your random numbers along the z axis. 
  /// \return A vector with random values in the range of [-max, max].
  hkvVec3 GetRandVectorSym(number maxX, number maxY, number maxZ);

  /// @}
  /// @name Vector & Matrix Utilities
  /// @{
  
  /// \brief 
  ///   Calculate the angle between two vectors.
  ///
  /// \param v1 
  ///   The first vector.
  ///
  /// \param v2 
  ///   The second vector.
  ///
  /// \return 
  ///   The angle in degree. When any of the input is nil, zero will be returned.
  number AngleBetweenVectors(hkvVec3 v1, hkvVec3 v2);
  
  /// \brief Create a 3x3 look-at matrix.
  /// \param pos The position of the observer.
  /// \param target The position of the target.
  /// \param up (\b optional) The up vector.
  /// \return A 3x3 matrix which could be used to transform 3D objects.
  hkvMat3 CreateLookAtMatrix(hkvVec3 pos, hkvVec3 target, hkvVec3 up = nil);
  
  /// @}
  /// @name Camera Utilities 
  /// @{
    
  /// \brief 
  ///   Computes the horizontal view angle from the given vertical view angle and aspect ratio.
  ///
  /// \param fovY
  ///   Vertical field of view angle in degree.
  /// 
  /// \param aspectRatio
  ///   Aspect ratio.
  ///
  /// \return
  ///   Horizontal field of view angle in degree.
  number GetFovX(number fovY, number aspectRatio);

  /// \brief 
  ///   Computes the vertical view angle from the given horizontal view angle and aspect ratio.
  ///
  /// \param fovX
  ///   Horizontal field of view angle in degree.
  /// 
  /// \param aspectRatio
  ///   Aspect ratio.
  ///
  /// \return
  ///   Vertical field of view angle in degree.
  number GetFovY(number fovX, number aspectRatio);

  /// @}
  /// @name Euler Angles Utilities
  /// @{

  /// \brief 
  ///   Creates a hkvMat3 rotation matrix from Euler Angles which are given in radians.
  ///
  /// \param roll
  ///   Roll angle in radians.
  ///
  /// \param pitch
  ///   Pitch angle in radians.
  ///
  /// \param yaw
  ///   Yaw angle in radians.
  ///
  /// \note
  ///   Uses default representation mode of euler angles in Vision Engine, so that
  ///   roll==X axis, pitch==Y axis and yaw==Z axis.
  hkvMat3 EulerToMat3_Rad(number roll, number pitch, number yaw);

  /// \brief
  ///   Creates a hkvMat3 rotation matrix from Euler Angles which are given in degrees.
  ///
  /// \param roll
  ///   Roll angle in degrees.
  ///
  /// \param pitch
  ///   Pitch angle in degrees.
  ///
  /// \param yaw
  ///   Yaw angle in degrees.
  ///
  /// \note
  ///   Uses default representation mode of euler angles in Vision Engine, so that
  ///   roll==X axis, pitch==Y axis and yaw==Z axis.
  hkvMat3 EulerToMat3_Deg(number roll, number pitch, number yaw);

  /// \brief 
  ///   Creates a hkvMat4 rotation matrix from Euler Angles which are given in radians.
  ///
  /// \param roll
  ///   Roll angle in radians.
  ///
  /// \param pitch
  ///   Pitch angle in radians.
  ///
  /// \param yaw
  ///   Yaw angle in radians.
  ///
  /// \note
  ///   Uses default representation mode of euler angles in Vision Engine, so that
  ///   roll==X axis, pitch==Y axis and yaw==Z axis.
  hkvMat4 EulerToMat4_Rad(number roll, number pitch, number yaw);

  /// \brief 
  ///   Creates a hkvMat4 rotation matrix from Euler Angles which are given in degrees.
  ///
  /// \param roll
  ///   Roll angle in degrees.
  ///
  /// \param pitch
  ///   Pitch angle in degrees.
  ///
  /// \param yaw
  ///   Yaw angle in degrees.
  ///
  /// \note
  ///   Uses default representation mode of euler angles in Vision Engine, so that
  ///   roll==X axis, pitch==Y axis and yaw==Z axis.
  hkvMat4 EulerToMat4_Deg(number roll, number pitch, number yaw);

  /// \brief 
  ///   Creates a hkvQuat from Euler Angles which are given in radians.
  ///
  /// \param roll
  ///   Roll angle in radians.
  ///
  /// \param pitch
  ///   Pitch angle in radians.
  ///
  /// \param yaw
  ///   Yaw angle in radians.
  ///
  /// \note
  ///   Uses default representation mode of euler angles in Vision Engine, so that
  ///   roll==X axis, pitch==Y axis and yaw==Z axis.
  hkvQuat EulerToQuat_Rad(number roll, number pitch, number yaw);

  /// \brief 
  ///   Creates a hkvQuat from Euler Angles which are given in degrees.
  ///
  /// \param roll
  ///   Roll angle in degrees.
  ///
  /// \param pitch
  ///   Pitch angle in degrees.
  ///
  /// \param yaw
  ///   Yaw angle in degrees.
  ///
  /// \note
  ///   Uses default representation mode of euler angles in Vision Engine, so that
  ///   roll==X axis, pitch==Y axis and yaw==Z axis.
  hkvQuat EulerToQuat_Deg(number roll, number pitch, number yaw);

  /// \brief 
  ///   Extracts Euler Angles from a hkvMat3 and returns them in radians.
  ///
  /// \note
  ///   Uses default representation mode of euler angles in Vision Engine, so that
  ///   roll==X axis, pitch==Y axis and yaw==Z axis.
  ///
  /// \param m
  ///   hkvMat3 matrix to extract euler angles from.
  ///
  /// \return
  ///   hkvVec3 with x=roll, y=pitch and z=yaw in radians. 
  ///   When input is nil, all components of the returned vector will be set to zero.
  hkvVec3 Mat3ToEuler_Rad(hkvMat3 m);

  /// \brief 
  ///   Extracts Euler Angles from a hkvMat3 and returns them in degrees.
  ///
  /// \note
  ///   Uses default representation mode of euler angles in Vision Engine, so that
  ///   roll==X axis, pitch==Y axis and yaw==Z axis.
  ///
  /// \param m
  ///   hkvMat3 matrix to extract euler angles from.
  ///
  /// \return
  ///   hkvVec3 with x=roll, y=pitch and z=yaw in degrees.
  ///   When input is nil, all components of the returned vector will be set to zero.
  hkvVec3 Mat3ToEuler_Deg(hkvMat3 m);

  /// \brief 
  ///   Extracts Euler Angles from a hkvMat4 and returns them in radians.
  ///
  /// \note
  ///   Uses default representation mode of euler angles in Vision Engine, so that
  ///   roll==X axis, pitch==Y axis and yaw==Z axis.
  ///
  /// \param m
  ///   hkvMat4 matrix to extract euler angles from.
  ///
  /// \return
  ///   hkvVec3 with x=roll, y=pitch and z=yaw in radians.
  ///   When input is nil, all components of the returned vector will be set to zero.
  hkvVec3 Mat4ToEuler_Rad(hkvMat4 m);

  /// \brief 
  ///   Extracts Euler Angles from a hkvMat4 and returns them in degrees.
  ///
  /// \note
  ///   Uses default representation mode of euler angles in Vision Engine, so that
  ///   roll==X axis, pitch==Y axis and yaw==Z axis.
  ///
  /// \param m
  ///   hkvMat4 matrix to extract euler angles from.
  ///
  /// \return
  ///   hkvVec3 with x=roll, y=pitch and z=yaw in degrees.
  ///   When input is nil, all components of the returned vector will be set to zero.
  hkvVec3 Mat4ToEuler_Deg(hkvMat4 m);

  /// \brief 
  ///   Extracts Euler Angles from a hkvQuat and returns them in radians.
  ///
  /// \note
  ///   Uses default representation mode of euler angles in Vision Engine, so that
  ///   roll==X axis, pitch==Y axis and yaw==Z axis.
  ///
  /// \param q
  ///   hkvQuat quaternion to extract euler angles from.
  ///
  /// \return
  ///   hkvVec3 with x=roll, y=pitch and z=yaw in radians.
  ///   When input is nil, all components of the returned vector will be set to zero.
  hkvVec3 QuatToEuler_Rad(hkvQuat q);

  /// \brief 
  ///   Extracts Euler Angles from a hkvQuat and returns them in degrees.
  ///
  /// \note
  ///   Uses default representation mode of euler angles in Vision Engine, so that
  ///   roll==X axis, pitch==Y axis and yaw==Z axis.
  ///
  /// \param q
  ///   hkvQuat quaternion to extract euler angles from.
  ///
  /// \return
  ///   hkvVec3 with x=roll, y=pitch and z=yaw in degrees.
  ///   When input is nil, all components of the returned vector will be set to zero.
  hkvVec3 QuatToEuler_Deg(hkvQuat q);

  /// @}
  /// @name Trigonometric Utilities
  /// @{

  /// \brief 
  ///   Convert radians to degrees.
  ///
  /// \param radValue 
  ///   A value in radians.
  ///
  /// \return
  ///   A value in degrees.
  number RadToDeg(number radValue);

  /// \brief 
  ///   Convert degrees to radians.
  ///
  /// \param degValue 
  ///   A value in degrees.
  ///
  /// \return 
  ///   A value in radians.
  number DegToRad(number degValue);

  /// \brief
  ///   Checks whether two angles, given in degrees, are equal. Wraps around 360, if necessary.
  boolean IsAngleEqualDeg(number angle1, number angle2, number epsilon = HKVMATH_DEFAULT_EPSILON);

  /// @}
  /// @name Exponential Utilities
  /// @{

  /// \brief
  ///   Returns the inverse square root of f, ie. (1.0/sqrt(f)).
  ///
  /// \param f
  ///   The value from which to compute the inverse square root.
  ///
  /// This function may use approximations to speed up the computation, compare to actually
  /// computing (1.0/sqrt(f)).
  /// It should be preferred to multiply values with 'sqrtInverse(f)' instead of
  /// dividing by 'sqrt(f)'
  number SqrtInverse(number f);

  /// \brief
  ///   Returns the n-th root of f.
  ///
  /// \param f
  ///   The value from which to compute the nth root.
  ///
  /// \param nthRoot
  ///   Which root to compute.
  ///
  /// E.g. Root(16.0, 2.0) will compute the square root of 16.0, and will thus return 4.0.
  /// E.g. Root(81.0, 4.0) will compute the 4th root of 81.0, and will thus return 3.0.
  number Root(number f, number nthRoot);

  /// \brief
  ///   Checks whether x is base raised to any power.
  ///
  /// \param x
  ///   The value to check whether it is a power of 'base'. 
  ///   Will be internally always casted to an unsigned integer.
  ///
  /// \param base
  ///   The base value. Will be always casted to an integer.
  ///
  /// E.g. IsPowerOf (81, 3) would return true.
  /// Always returns true for x == 1, as any number raised to 0 yields 1.
  boolean IsPowerOf(number x, number base);

  /// \brief
  ///   Checks whether x is 2 raised to any power. 
  ///
  /// \param x
  ///   The value to check whether it is a power of 2. 
  ///   Will be internally always casted to an unsigned integer.
  ///
  /// E.g. will return true for x == 1, 2, 4, 8, 16, ...
  boolean IsPowerOf2(number x);

  /// \brief
  ///   Returns the logarithm of f using 2 as its base.
  number Log2(number f);

  /// \brief
  ///   Returns the logarithm of f using 'base' as its base.
  number logB(number f, number base);

  /// @}
  /// @name Clamping and Rounding Utilities
  /// @{

  /// \brief
  ///   Returns the sign (0, -1 or +1) of f.
  ///
  /// \param f
  ///   The value from which to return the sign.
  ///
  /// \note 
  ///   For f == 0 it will return 0, not '+1'.
  number Sign(number f);

  /// \brief
  ///   Returns the smallest power of two that is larger or equal to x.
  ///
  /// E.g. powerOf2_ceil(31) == 32
  /// E.g. powerOf2_ceil(32) == 32
  /// E.g. powerOf2_ceil(33) == 64
  ///
  /// The result is undefined if the result can't be represented as an unsigned 32bit integer.
  ///
  /// \param x
  ///   The value which will be rounded up to the next power-of-two.
  ///   Will be internally always casted to an unsigned integer.
  number PowerOf2_ceil(number x);

  /// \brief
  ///   Returns the largest power of two that is smaller or equal to x.
  ///
  /// E.g. powerOf2_ceil(31) == 16
  /// E.g. powerOf2_ceil(32) == 32
  /// E.g. powerOf2_ceil(33) == 32
  ///
  /// The result of powerOf2_floor(0) is undefined.
  ///
  /// \param x
  ///   The value which will be rounded down to the next power-of-two.
  ///   Will be internally always casted to an unsigned integer.
  number PowerOf2_floor(number x);

  /// \brief 
  ///   Round a given value.
  ///
  /// \param f 
  ///   The value to round.
  ///
  /// \return 
  ///   The rounded value.
  number Round(number f);

  /// \brief 
  ///   Round a given value to specified decimal places.
  ///
  /// \param f 
  ///   The value to round.
  ///
  /// \param decimalPlaces 
  ///   The decimal place to round. 
  ///  
  /// \note
  ///   Use Round(number f) for integer rounding.
  ///
  /// \return 
  ///   The rounded value.
  number Round(number f, number decimalPlaces);

  /// \brief
  ///   Rounds f to the closest multiple of roundTo.
  ///
  /// E.g. roundTo( 15.0, 4.0) == 16.0
  /// E.g. roundTo(-15.0, 4.0) ==-16.0
  ///
  /// \param f
  ///   The value which will be rounded to the closest multiple of roundTo.
  ///
  /// \param roundTo
  ///   To a multiple of this value will f be rounded.
  number RoundTo(number f, number roundTo);

  /// \brief
  ///   Returns f clamped to the range [minVal;maxVal].
  ///
  /// In other words 'Max(minVal, min(maxVal , f))'
  ///
  /// E.g. clamp(3, 2, 5) == 3 // no clamping
  /// E.g. clamp(1, 2, 5) == 2 // clamped to minVal
  /// E.g. clamp(7, 2, 5) == 5 // clamped to maxVal
  ///
  /// \param f
  ///   The value to be clamped.
  ///
  /// \param minVal
  ///   The lower bound to clamp val to.
  ///
  /// \param maxVal
  ///   The upper bound to clamp val to.
  number Clamp(number f, number minVal, number maxVal);

  /// \brief
  ///   Returns f clamped to the range [0;1].
  ///
  /// E.g. saturate (0.3) == 0.3 // no clamping
  /// E.g. saturate (-0.1) == 0.0 // clamped to 0
  /// E.g. saturate (1.2) == 1.0 // clamped to 1
  ///
  /// \param f
  ///   The value to be clamped.
  number Saturate(number f);

  /// \brief
  ///   Checks whether f is between minVal and maxVal (inclusive).
  boolean IsInRange(number f, number minVal, number maxVal);

  /// @}
  /// @name Checking Utilities
  /// @{

  /// \brief
  ///   Checks whether f is equal to zero within a given epsilon range.
  boolean IsZero(number f, number epsilon = HKVMATH_LARGE_EPSILON);

  /// \brief
  ///   Checks whether f1 is equal to f2 within a given epsilon range.
  boolean IsEqual(number f1, number f2, number epsilon = HKVMATH_LARGE_EPSILON);

  /// \brief
  ///   Returns true if f is between -Infinity and +Infinity and is not NaN.
  boolean IsFiniteNumber(number f);

  /// \brief
  ///   Returns true if f is NaN (Not-a-Number). In all other cases (including +/- Infinity) it will return false.
  boolean IsNaN(float f);

  /// @}
  /// @name Misc Utilities
  /// @{

  /// \brief
  ///   Returns a value that was linearly interpolated between f1 and f2.
  ///
  /// \param f1
  ///   This value will be returned if interpolation is 0.0.
  ///
  /// \param f2
  ///   This value will be returned if interpolation is 1.0.
  ///
  /// \param interpolation
  ///   Interpolation factor, must be between 0 and 1.
  number Interpolate(number f1, number f2, number interpolation);

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
