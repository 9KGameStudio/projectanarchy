/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VBASE_HKVMATH_HKVQUATERNION_H
#define VBASE_HKVMATH_HKVQUATERNION_H

#include <Vision/Runtime/Base/Math/hkvMath.h>
#include <Vision/Runtime/Base/Math/Vector/hkvVec3.h>
#include <Vision/Runtime/Base/Math/Matrix/hkvMat3.h>
#include <Vision/Runtime/Base/Math/Matrix/hkvMat4.h>

#ifdef HKVQUAT_CHECK_FOR_NAN
  #define HKVQUAT_INITIALIZATION_CHECK(obj) (obj)->isInitializedCheck()
#else
  #define HKVQUAT_INITIALIZATION_CHECK(obj) 
#endif

/// \brief A quaternion class that can be used to represent rotations.
class hkvQuat
{
public:

  /// \brief
  ///   ATTENTION: The object is NOT initialized by the constructor. You MUST initialize it yourself before using it.
  ///
  /// \note In Dev and Debug builds the object will be initialized with NaN values. Member functions that read the values will check that they are not NaN.
  /// If an NaN value is encountered, those functions will trigger an assert. Thus when you run into such an assert, you have not initialized your object
  /// after construction. Make sure you always initialize objects properly before using them.
  HKV_FORCE_INLINE hkvQuat ()
  {
    #ifdef HKVQUAT_INITIALIZE_TO_NAN

      x = hkvMath::generateNaN();
      y = hkvMath::generateNaN();
      z = hkvMath::generateNaN();
      w = hkvMath::generateNaN();

    #elif defined(HKVQUAT_INITIALIZE_TO_IDENTITY)

      setIdentity ();

    #endif
  }

  #ifdef HKVQUAT_CHECK_FOR_NAN
    HKV_FORCE_INLINE void isInitializedCheck() const
    {
      VASSERT_MSG(!hkvMath::isNaN (x) && !hkvMath::isNaN (y) && !hkvMath::isNaN (z) && !hkvMath::isNaN (w), 
        "This object has invalid (NaN) members: (%.2f | %.2f | %.2f | %.2f).\nThis happens when you use this object without properly initializing it first, as the default constructor will set all members to NaN in debug builds.", x, y, z, w);
    }
  #endif

  /// \brief
  ///   Does not initialize this object. Prefer this constructor over the default constructor.
  ///
  /// \param init
  ///   Set this to hkvNoInitialization to actually select this constructor.
  HKV_FORCE_INLINE explicit hkvQuat (hkvInit_None init /* = hkvNoInitialization */) { }

  /// \brief Static function that returns an identity quaternion (ie. no rotation).
  HKV_FORCE_INLINE static const hkvQuat IdentityQuaternion ();

  /// \brief Normalizes the quaternion to unit length. Only a normalized quaternion represents a valid rotation.
  HKV_FORCE_INLINE void normalize ();

  /// \brief Checks whether the quaternion is normalized withing some epsilon range.
  HKV_FORCE_INLINE bool isNormalized (float fEpsilon = HKVMATH_GIGANTIC_EPSILON) const;

  /// \brief Inverts the rotation that the quaternion represents.
  HKV_FORCE_INLINE void invert ();

  /// \brief Sets the quaternion to identity.
  HKV_FORCE_INLINE void setIdentity ();

  /// \brief Checks whether the quaternion is the identity quaternion.
  HKV_FORCE_INLINE bool isIdentity (float fEpsilon = HKVMATH_LARGE_EPSILON) const;

  /// \brief Returns the length of the quaternion. Will always be 1 for quaternions that represent valid rotations.
  HKV_FORCE_INLINE float getLength () const;

  /// \brief Returns the squared length of the quaternion.
  HKV_FORCE_INLINE float getLengthSquared () const;

  /// \brief Creates the quaternion from a 3x3 matrix.
  HKV_FORCE_INLINE void setFromMat3 (const hkvMat3& mRotation);

  /// \brief Sets the quaternion from a rotation axis and and angle.
  ///
  /// \param vAxis
  ///   The rotation axis. Must be normalized.
  ///
  /// \param fAngleDeg
  ///   The amount of rotation around the axis, given in degree.
  ///   The rotation will be counter-clockwise around the rotation axis.
  ///
  // /// \sa getAxisAndAngle
  HKV_FORCE_INLINE void setFromAxisAndAngle (const hkvVec3& vAxis, float fAngleDeg);

  /// \brief Creates a quaternion from euler angles.
  HKV_FORCE_INLINE void setFromEulerAngles (float fRollDeg, float fPitchDeg, float fYawDeg);

  /// \brief Returns euler angles which represent the same rotation as this quaternion.
  HKV_FORCE_INLINE void getAsEulerAngles (float& out_fRollDeg, float& out_fPitchDeg, float& out_fYawDeg) const;

  /// \brief Returns a 3x3 matrix that represents the same rotation as this quaternion.
  HKV_FORCE_INLINE const hkvMat3 getAsMat3 () const;

  /// \brief Returns a 4x4 matrix that represents the same rotation as this quaternion.
  HKV_FORCE_INLINE const hkvMat4 getAsMat4 () const;

  /// \brief Returns an inverted quaternion, ie. one that rotates into the opposite direction.
  HKV_FORCE_INLINE const hkvQuat operator- () const;

  /// \brief Returns the rotation axis and angle of this quaternion.
  ///
  /// \param out_vRotationAxis
  ///   The rotation axis is stored here. Will be normalized.
  ///
  /// \param out_fAngles
  ///   The rotation angle will be stored here. 
  ///   The rotation is supposed to be counter-clockwise around the rotation axis.
  ///
  /// \sa setFromAxisAndAngle
  HKV_FORCE_INLINE void getAxisAndAngle (hkvVec3& out_vRotationAxis, float& out_fAngles) const;

  /// \brief
  ///   Will check whether two quaternions are identical.
  ///
  /// Since a quaternion is equal to itself with all components negated, that will also
  /// be taken into account. Thus (x,y,z,w) will be considered equal to (-x,-y,-z,-w).
  /// Generally this function creates two matrices from the two quaternions and compares those for equality.
  /// This function should only be used in unit-tests.
  ///
  /// \param rhs      The right-hand side quaternion to check.
  HKV_FORCE_INLINE bool isIdentical (const hkvQuat& rhs) const;

  /// \brief
  ///   Will check whether two quaternions are equal within some epsilon range.
  ///
  /// Since a quaternion is equal to itself with all components negated, that will also
  /// be taken into account. Thus (x,y,z,w) will be considered equal to (-x,-y,-z,-w).
  /// Generally this function creates two matrices from the two quaternions and compares those for equality.
  /// This function should only be used in unit-tests.
  ///
  /// \param rhs      The right-hand side quaternion to check.
  /// \param fEpsilon The allowed epsilon range.
  HKV_FORCE_INLINE bool isEqual (const hkvQuat& rhs, float fEpsilon) const;

  /// \brief Computes the 4-component dot-product between the two quaternions.
  HKV_FORCE_INLINE float dot (const hkvQuat& rhs) const;

  /// \brief Sets this quaternion to the shortest rotation quaternion that would rotate the direction vector 'vFrom' to 'vTo'.
  ///
  /// This function can be used to take two directions and create a quaternion that defines how to
  /// rotate the direction 'vFrom' such that it becomes 'vTo'.
  ///
  /// \param vFrom
  ///   Original direction vector. Must be normalized.
  ///
  /// \param vTo
  ///   Target direction vector. Must be normalized.
  VBASE_IMPEXP void setShortestRotation (const hkvVec3& vFrom, const hkvVec3& vTo);

  /// \brief Concatenates two rotations. This operation is not commutative.
  HKV_FORCE_INLINE const hkvQuat multiply (const hkvQuat& rhs) const;

  /// \brief Concatenates two rotations in the reverse order than 'multiply' does. This operation is not commutative.
  HKV_FORCE_INLINE const hkvQuat multiplyReverse (const hkvQuat& rhs) const;

  /// \brief Transforms (rotates) the given vector by this quaternion.
  HKV_FORCE_INLINE const hkvVec3 transform (const hkvVec3& rhs) const;

  /// \brief Transforms (rotates) the given vector by the inverse of this quaternion. Ie. rotates it into the opposite direction.
  HKV_FORCE_INLINE const hkvVec3 transformReverse (const hkvVec3& rhs) const;

  /// \brief Flips the sign of all four components.
  HKV_FORCE_INLINE void flipSigns ();

  /// \brief Sets the 4 components of this quaternion. Negates the w component. Useful when porting old, VisQuaternion_cl based, code to hkvQuat.
  HKV_FORCE_INLINE void setValuesDirect_Old (float inx, float iny, float inz, float negw) { x = inx; y = iny; z = inz; w = -negw; }

  /// \brief Sets the 4 components of this quaternion.
  HKV_FORCE_INLINE void setValuesDirect (float inx, float iny, float inz, float inw) { x = inx; y = iny; z = inz; w = inw; }

  /// \brief Returns the x component.
  HKV_FORCE_INLINE float getX () const;
  
  /// \brief Returns the y component.
  HKV_FORCE_INLINE float getY () const;

  /// \brief Returns the z component.
  HKV_FORCE_INLINE float getZ () const;

  /// \brief Returns the w component.
  HKV_FORCE_INLINE float getW () const;

  /// \brief Checks whether the 4 components are all valid (ie. not NaN or infinity). Does NOT check whether the quaternion is normalized.
  HKV_FORCE_INLINE bool isValid () const;

  /// \brief Returns a float pointer to the internal data.
  HKV_FORCE_INLINE const float* getDataPointer () const;

  /// \brief Returns a float pointer to the internal data.
  HKV_FORCE_INLINE float* getDataPointer ();

  /// \brief Sets this quaternion to the spherical interpolation of 'qFrom' and 'qTo' by the factor 't'.
  VBASE_IMPEXP void setSlerp (const hkvQuat& qFrom, const hkvQuat& qTo, float t);

  float x, y, z, w;
};

// Allows to serialize the hkvQuat to a VArchiv using >> and <<
V_DECLARE_SERIALX_NONINTRUSIVE (hkvQuat, VBASE_IMPEXP);

/// \brief Rotates the vector rhs by the quaternion lhs.
HKV_FORCE_INLINE const hkvVec3 operator* (const hkvQuat& lhs, const hkvVec3& rhs);

/// \brief Concatenates the rotations of lhs and rhs.
HKV_FORCE_INLINE const hkvQuat operator* (const hkvQuat& lhs, const hkvQuat& rhs);

/// \brief Compares the two quaternions for equality.
HKV_FORCE_INLINE bool operator== (const hkvQuat& lhs, const hkvQuat& rhs);

/// \brief Compares the two quaternions for inequality.
HKV_FORCE_INLINE bool operator!= (const hkvQuat& lhs, const hkvQuat& rhs);

#include <Vision/Runtime/Base/Math/Quaternion/hkvQuat.inl>

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
