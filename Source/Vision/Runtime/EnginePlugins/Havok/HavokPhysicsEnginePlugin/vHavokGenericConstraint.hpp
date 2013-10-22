/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef V_HAVOK_GENERIC_CONSTRAINT_HPP_INCLUDED
#define V_HAVOK_GENERIC_CONSTRAINT_HPP_INCLUDED

#include <Physics2012/Dynamics/Constraint/ConstraintKit/hkpConstraintConstructionKit.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConstraint.hpp>

/// \brief
///   Descriptor class for a vHavokPhysics Generic constraint.
class vHavokGenericConstraintDesc : public vHavokConstraintDesc
{
public:
  /// 
  /// \brief
  ///   Enumeration of motion types.
  ///
  enum VGenericMotion_e
  {
    VGENERIC_MOTION_LOCKED,         ///< The DOF (degree of freedom) is locked, it does not allow any relative motion.
    VGENERIC_MOTION_LIMITED,        ///< The DOF is limited, it only allows motion within a specific range.
    VGENERIC_MOTION_FREE,           ///< The DOF is free and has full range of motion.
    VGENERIC_MOTION_UNCONSTRAINED   ///< The motion for the corresponding axis is not constrained.
  };

  /// \brief
  ///   Constructor
  VHAVOK_IMPEXP vHavokGenericConstraintDesc();

  /// \brief
  ///   Sets the default value for all members.
  VHAVOK_IMPEXP virtual void Reset();

  ///
  /// @name Serialization
  /// @{
  ///

  V_DECLARE_SERIAL_DLLEXP(vHavokGenericConstraintDesc, VHAVOK_IMPEXP)
  VHAVOK_IMPEXP virtual void Serialize(VArchive &ar) HKV_OVERRIDE;

  ///
  /// @}
  ///

public:
  hkvVec3 m_vGenericPivot;             ///< generic constraint's pivot point in world space
  hkvVec3 m_vAngularAxis;              ///< generic constraint's angular axis in world space
  VGenericMotion_e m_eXMotion;         ///< motion along the X-axis
  VGenericMotion_e m_eYMotion;         ///< motion along the Y-axis
  VGenericMotion_e m_eZMotion;         ///< motion along the Z-axis
  VGenericMotion_e m_eAngularMotion;   ///< motion around the angular axis
  float m_fLinearMin;                  ///< minimum linear limit 
  float m_fLinearMax;                  ///< maximum linear limit 
  float m_fAngularMin;                 ///< minimum angular limit (in degrees)
  float m_fAngularMax;                 ///< maximum angular limit (in degrees) 
  float m_fVelocityTarget;             ///< goal velocity of motor
  float m_fTau;                        ///< motor stiffness (Range: 0/ 1)
  float m_fMinForce;                   ///< absolute min force applied to achieve the velocity target 
  float m_fMaxForce;                   ///< absolute max force applied to achieve the velocity target
  float m_fLinearFriction;             ///< friction for linear movement
  float m_fAngularFriction;            ///< friction for angular movement

private:
  static const unsigned int s_iSerialVersion;
};

/// \brief
///   Implementation of the vHavok Generic constraint.
class vHavokGenericConstraint : public vHavokConstraint
{
public:
  // Six degrees of freedom along the x-, y- and z-axis.
  enum VGenericAxis_e
  {
    VGENERIC_AXIS_X=0,
    VGENERIC_AXIS_Y=1,
    VGENERIC_AXIS_Z=2, 
    VGENERIC_AXIS_ANGULAR=0
  };

  ///
  /// @name Serialization and Type Management
  /// @{
  ///

  V_DECLARE_SERIAL_DLLEXP(vHavokGenericConstraint, VHAVOK_IMPEXP)
  VHAVOK_IMPEXP virtual void Serialize(VArchive &ar) HKV_OVERRIDE;

  ///
  /// @}
  ///

protected:
  ///
  /// @name Base Class Overrides
  /// @{
  ///

  VHAVOK_IMPEXP virtual hkpConstraintData* CreateConstraintData() HKV_OVERRIDE;
  VHAVOK_IMPEXP virtual vHavokConstraintDesc *CreateConstraintDesc() HKV_OVERRIDE;
  VHAVOK_IMPEXP virtual void InitConstraintDataFromDesc(hkpConstraintData& data, vHavokConstraintDesc const& desc) HKV_OVERRIDE;
public:
  VHAVOK_IMPEXP virtual void SaveToDesc(vHavokConstraintDesc& desc);
  
  ///
  /// @}
  ///

private:
  void SetLinearLimits(hkpConstraintConstructionKit &kit, const vHavokGenericConstraintDesc* pGenericDesc, 
    VGenericAxis_e eAxis, vHavokGenericConstraintDesc::VGenericMotion_e eMotion);

  void SetAngularLimits(hkpConstraintConstructionKit &kit, const vHavokGenericConstraintDesc* pGenericDesc, 
    vHavokGenericConstraintDesc::VGenericMotion_e eMotion);

  static const unsigned int s_iSerialVersion;

  vHavokGenericConstraintDesc m_savedDesc;  ///< Saved generic constraint desc
};

#endif //V_HAVOK_GENERIC_CONSTRAINT_HPP_INCLUDED

/*
 * Havok SDK - Base file, BUILD(#20131019)
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
