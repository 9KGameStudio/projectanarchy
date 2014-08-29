/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef V_HAVOK_POINTTOPLANE_CONSTRAINT_HPP_INCLUDED
#define V_HAVOK_POINTTOPLANE_CONSTRAINT_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConstraint.hpp>

/// \brief
///   Descriptor for vHavokPhysics PointToPlane constraint.
class vHavokPointToPlaneConstraintDesc : public vHavokConstraintDesc
{
public:
  VHAVOK_IMPEXP vHavokPointToPlaneConstraintDesc();

  VHAVOK_IMPEXP virtual void Reset() HKV_OVERRIDE;

  V_DECLARE_SERIAL_DLLEXP(vHavokPointToPlaneConstraintDesc, VHAVOK_IMPEXP)
  VHAVOK_IMPEXP virtual void Serialize(VArchive &ar) HKV_OVERRIDE;

public:
  hkvVec3 m_vPointToPlanePivot; ///< Specifies both the origin point for the constraining plane, and bodyA's pivot point, specified in world space
  hkvVec3 m_vPlaneNormal;       ///< Specifies the normal direction of the constraining plane, specified in world space

private:
  static const unsigned int s_iSerialVersion;
};

/// \brief
///   Implementation of the vHavokPhysics PointToPlane constraint.
class vHavokPointToPlaneConstraint : public vHavokConstraint
{
public:
  // serialization and type management
  V_DECLARE_SERIAL_DLLEXP(vHavokPointToPlaneConstraint, VHAVOK_IMPEXP)
  VHAVOK_IMPEXP VOVERRIDE void Serialize(VArchive &ar) HKV_OVERRIDE;

protected:
  VHAVOK_IMPEXP virtual hkpConstraintData* CreateConstraintData() HKV_OVERRIDE;
  VHAVOK_IMPEXP virtual vHavokConstraintDesc *CreateConstraintDesc() HKV_OVERRIDE;
  VHAVOK_IMPEXP virtual void InitConstraintDataFromDesc(hkpConstraintData& data, 

    vHavokConstraintDesc const& desc) HKV_OVERRIDE;
public:
  VHAVOK_IMPEXP virtual void SaveToDesc(vHavokConstraintDesc& desc) HKV_OVERRIDE;

private:
  vHavokPointToPlaneConstraintDesc m_savedPointToPlaneDesc;  ///< Saved PointToPlane constraint desc

  static const unsigned int s_iSerialVersion;
};

#endif //V_HAVOK_POINTTOPLANE_CONSTRAINT_HPP_INCLUDED

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
