/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VLUA_APIDOC

class hkvQuat
{
public:
  %extend 
  {
    hkvQuat()
    {
      hkvQuat *pNew = new hkvQuat(hkvNoInitialization);
      pNew->setIdentity();
      return pNew;
    }  

    hkvQuat(const hkvQuat& rhs)
    {
      return new hkvQuat(rhs);
    } 

    hkvQuat(float _x, float _y, float _z, float _w)
    {
      hkvQuat *pQuat = new hkvQuat(hkvNoInitialization);
      pQuat->x = _x;
      pQuat->y = _y;
      pQuat->z = _z;
      pQuat->w = _w;
      return pQuat;
    }
  } 

  void setIdentity();

  void setFromMat3(const hkvMat3& mRotation);

  %extend 
  {
    void setFromAxisAndAngle(const hkvVec3& vAxis, float fAngleDeg)
    { 
      self->setFromAxisAndAngle(vAxis.getNormalized(), fAngleDeg);
    }
  }
  void setFromEulerAngles(float fRollDeg, float fPitchDeg, float fYawDeg); 

  %extend 
  {
    void setShortestRotation(const hkvVec3& vFrom, const hkvVec3& vTo)
    {
      self->setShortestRotation(vFrom.getNormalized(), vTo.getNormalized());
    }
  }
  
  const hkvMat3 getAsMat3()const;
  const hkvMat4 getAsMat4()const;  

  %extend 
  {
    hkvVec4 getAxisAndAngle() const
    {
      hkvVec3 vRotationAxis;
      float fAngle;
      self->getAxisAndAngle(vRotationAxis, fAngle);
      return hkvVec4(vRotationAxis.x, vRotationAxis.y, vRotationAxis.z, fAngle);
    }

    hkvVec3 getAsEulerAngles() const
    {
      float fRoll, fPitch, fYaw;
      self->getAsEulerAngles(fRoll, fPitch, fYaw);
      return hkvVec3(fRoll, fPitch, fYaw);
    }
  }

  bool isNormalized(float fEpsilon = HKVMATH_GIGANTIC_EPSILON) const;
  bool isValid() const;

  void normalize();

  void invert();

  float getLength() const;
  float getLengthSquared() const;

  const hkvVec3 transform(const hkvVec3& rhs) const;
  const hkvVec3 transformReverse (const hkvVec3& rhs) const;

  void flipSigns();

  void setSlerp(const hkvQuat& qFrom, const hkvQuat& qTo, float t);

  %extend 
  {
    hkvQuat clone() const
    {
      return *self; //will call the copy constructor
    }
  }

  float dot(const hkvQuat& rhs) const;

  const hkvQuat multiply(const hkvQuat& rhs) const;
  const hkvQuat multiplyReverse(const hkvQuat& rhs) const;

  const hkvQuat operator-() const;

  %extend 
  {
    bool __eq (const hkvQuat* pRhs) const
    {
      if(pRhs == NULL) 
        return false;
      return (*self == (*pRhs));
    }

    /*
    hkvQuat __mul(const hkvQuat* pRhs)
    {
      %#ifdef HKVMATH_ENABLE_NEW_OPERATORS
        if (pRhs == NULL) 
          return *self;
        return (*self * (*pRhs));
      %#else
        hkvLog::Info("Warning: Calling 'hkvQuat operator*(const hkvQuat& lhs, const hkvQuat& rhs)' without enabling 'HKVMATH_ENABLE_NEW_OPERATORS'!");
        return *self;
      %#endif
    }
    */

    hkvVec3 __mul(const hkvVec3* pRhs)
    {
      if (pRhs == NULL) 
        return hkvVec3(0);
      return (*self * (*pRhs));
    }
  }

  float x;
  float y;
  float z;
  float w;
};

//add lua tostring and concat operators
VSWIG_CREATE_CONCAT(hkvQuat, 128, "[%1.2f,%1.2f,%1.2f,%1.2f]", self->x, self->y, self->z, self->w)
VSWIG_CREATE_TOSTRING(hkvQuat, "hkvQuat: [%1.2f,%1.2f,%1.2f,%1.2f]", self->x, self->y, self->z, self->w)

#else

/// \brief 
///   Wrapper class for quaternions that can be used to represent rotations.
///
/// \note 
///   Since all wrappers classes are handled as pointers in Lua you cannot copy its content by assignment. 
///   Please take a look at the code samples.
///
/// \par Example
///   \code
///     -- create a new quaternion:
///     local quat1 = Vision.hkvQuat()
///     quat1:setFromAxisAndAngle(Vision.hkvVec3(0, 0, 1), 44.8)
///
///     -- this will not copy the quaternions content (just another pointer to it):
///     local quat2 = quat1
///
///     -- this will affect access to quat1 and quat2 since the pointers share the same object:
///     quat2.x = 10
///     -- this assertion will not fail
///     Vision.Assert(quat1 == quat2, "When this assertions fails quat1 and quat2 are pointing to different quaternions.")
///
///     -- in order to copy the content to another quaternion, you have to clone:
///     local quat4 = quat1:clone()
///
///     -- or create a new quaternion using the copy constructor
///     local quat4 = Vision.hkvQuat(quat1)
///   \endcode
/// \see clone
class hkvQuat
{
public:

  /// @name Initialization
  /// @{
  
  /// \brief 
  ///   Default Constructor. Will set quaternion to identity.
  hkvQuat();

  /// \brief 
  ///   Copy Constructor.
  ///
  /// \param other 
  ///   The quaternion to copy.
  hkvQuat(hkvQuat other);

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
  hkvQuat(number x, number y, number z, number w);
  
  /// \brief 
  ///   Sets the quaternion to identity.
  HKV_FORCE_INLINE void setIdentity();

  /// @}
  /// @name Creating Quaternions 
  /// @{

  /// \brief 
  ///   Creates the quaternion from a 3x3 matrix.
  void setFromMat3(hkvMat3 rotation);

  /// \brief 
  ///   Sets the quaternion from a rotation axis and and an angle.
  ///
  /// \param axis
  ///   The rotation axis. Gets internally normalized (\b always).
  ///
  /// \param angleDeg
  ///   The amount of rotation around the axis, given in degrees.
  ///   The rotation will be counter-clockwise around the rotation axis.
  ///
  /// \sa getAxisAndAngle
  void setFromAxisAndAngle(hkvVec3 axis, number angleDeg);
  
  /// \brief 
  ///   Creates a quaternion from euler angles.
  void setFromEulerAngles(number rollDeg, number pitchDeg, number yawDeg);

  /// \brief 
  ///   Sets this quaternion to the shortest rotation quaternion that would rotate the direction vector 'from' to 'to'.
  ///
  /// This function can be used to take two directions and create a quaternion that defines how to
  /// rotate the direction 'from' such that it becomes 'to'.
  ///
  /// \param from
  ///   Original direction vector. Gets internally normalized (\b always).
  ///
  /// \param to
  ///   Target direction vector. Gets internally normalized (\b always).
  void setShortestRotation(hkvVec3 from, hkvVec3 to);

  /// @}
  /// @name Conversions
  /// @{

  /// \brief 
  ///   Returns a 3x3 matrix that represents the same rotation as this quaternion.
  hkvMat3 getAsMat3();

  /// \brief 
  ///   Returns a 4x4 matrix that represents the same rotation as this quaternion.
  hkvMat4 getAsMat4();

  /// \brief 
  ///   Returns the rotation axis and angle of this quaternion.
  ///
  /// \return
  ///   A hkvVec4 vector, with the rotation axis (will be normalized) stored in xyz and the 
  ///   rotation angle (supposed to be counter-clockwise around the rotation axis) stored in w.
  ///
  /// \sa setFromAxisAndAngle
  hkvVec4 getAxisAndAngle();

  /// \brief 
  ///   Returns euler angles which represent the same rotation as this quaternion.
  ///
  /// \return
  ///   A hkvVec3 vector with x=roll, y=pitch and z=yaw stored in degrees.
  hkvVec3 getAsEulerAngles();

  /// @}
  /// @name Checks
  /// @{

  /// \brief 
  ///   Checks whether the quaternion is normalized within some epsilon range.
  boolean isNormalized(float epsilon = MATH_GIGANTIC_EPSILON);

  /// \brief 
  ///   Checks whether the 4 components are all valid (ie. not NaN or infinity). Does NOT check whether the quaternion is normalized.
  boolean isValid();

  /// @}
  /// @name Utility Methods
  /// @{

  /// \brief 
  ///   Normalizes the quaternion to unit length. Only a normalized quaternion represents a valid rotation.
  void normalize();

  /// \brief 
  ///   Inverts the rotation that the quaternion represents.   
  void invert();

  /// \brief 
  ///   Returns the length of the quaternion. Will always be 1 for quaternions that represent valid rotations.
  number getLength();

  /// \brief 
  ///   Returns the squared length of the quaternion.
  number getLengthSquared();

  /// \brief 
  ///   Transforms (rotates) the given vector by this quaternion.
  hkvVec3 transform(hkvVec3 vec);

  /// \brief 
  ///   Transforms (rotates) the given vector by the inverse of this quaternion. Ie. rotates it into the opposite direction.
  hkvVec3 transformReverse(hkvVec3 vec);

  /// \brief 
  ///   Flips the sign of all four components.
  void flipSigns();

  /// \brief 
  ///   Sets this quaternion to the spherical interpolation of 'from' and 'to' by the factor 't'.
  void setSlerp(hkvQuat from, hkvQuat& to, number t);

  /// \brief 
  ///   Clones the current quaternion.
  ///
  /// \return 
  ///   An identical hkvQuat.
  ///
  /// \note 
  ///   Since all wrappers classes are handled as pointers in Lua you cannot copy its content by assignment.
  ///   Please take a look at the code samples.
  ///
  /// \par Example
  ///   \code
  ///     -- create a new quaternion:
  ///     local quat1 = Vision.hkvQuat()
  ///     quat1:setFromAxisAndAngle(Vision.hkvVec3(0, 0, 1), 20)
  ///
  ///     -- this will not copy the content of quat1 (just another pointer to it):
  ///     local quat2 = quat1
  ///
  ///     -- this will affect access to quat1 and quat2 since the pointers share the same object:
  ///     quat2.x = 5
  ///     -- this assertion will not fail
  ///     Vision.Assert(quat1 == quat2, "When this assertions fails 'quat1' and 'quat2' are pointing to different quaternions.")
  ///
  ///     -- in order to copy the content to another quaternion, you have to clone:
  ///     local quat3 = quat1:clone()
  ///
  ///     -- this will affect only quat3:
  ///     quat3.y = 6
  ///     -- this assertion will not fail
  ///     Vision.Assert(quat1 ~= quat3, "When this assertions fails 'quat1' and 'quat3' are pointing to same quaternion.")
  ///
  ///     -- instead of clone you can also create a new quaternion using the copy constructor:
  ///     local quat4 = Vision.hkvQuat(quat1)
  ///   \endcode
  hkvVec4 clone();

  /// @}
  /// @name Operators and Math Methods
  /// @{
   
  /// \brief 
  ///   Computes the 4-component dot-product between the two quaternions.
  number dot(hkvQuat other);

  /// \brief 
  ///   Concatenates two rotations. This operation is not commutative.
  hkvQuat multiply(hkvQuat other);

  /// \brief 
  ///   Concatenates two rotations in the reverse order than 'multiply' does. This operation is not commutative.
  hkvQuat multiplyReverse(hkvQuat other);

  /// \brief 
  ///   Unary - Operator. Returns an inverted quaternion, ie. one that rotates into the opposite direction.
  hkvQuat __unm();

  /// \brief 
  ///   Operator == for hkvQuat comparison.
  ///
  /// \note
  ///   Since a quaternion is equal to itself with all components negated, that will also
  ///   be taken into account. Thus (x,y,z,w) will be considered equal to (-x,-y,-z,-w).
  ///   Generally this function creates two matrices from the two quaternions and compares those for equality.
  ///
  /// \param other 
  ///   The other quaternion to compare with.
  ///
  /// \return 
  ///   True if the quaternions are identical, otherwise false.
  boolean __eq(hkvQuat other);

  /// \brief 
  ///   Operator * to rotate vector by the quaternion.
  ///
  /// \param vec 
  ///   The hkvVec3 vector to rotate.
  ///
  /// \return 
  ///   The result of the multiplication.
  /// 
  /// \see hkvQuat::transform
  hkvVec3 __mul(hkvVec3 vec);

  /// @}
  /// @name Public Members
  /// @{
  
  /// \brief 
  ///  The x component of the quaternion.
  number x;
  
  /// \brief 
  ///   The y component of the quaternion.
  number y;
  
  /// \brief 
  ///   The z component of the quaternion.
  number z; 

  /// \brief 
  ///   The w component of the quaternion.
  number w; 
    
  /// @}

};

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
