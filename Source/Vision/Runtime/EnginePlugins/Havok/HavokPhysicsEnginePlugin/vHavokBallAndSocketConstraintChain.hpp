/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef V_HAVOK_BALL_AND_SOCKET_CONSTRAINT_CHAIN_HPP_INCLUDED
#define V_HAVOK_BALL_AND_SOCKET_CONSTRAINT_CHAIN_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConstraintChain.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>

// ----------------------------------------------------------------------------
// vHavokBallAndSocketConstraintChainDesc
// ----------------------------------------------------------------------------

/// \brief
///   Descriptor for vHavok Ball-and-Socket constraint chains.
class vHavokBallAndSocketConstraintChainDesc : public vHavokConstraintChainDesc
{
public:
  VHAVOK_IMPEXP vHavokBallAndSocketConstraintChainDesc();

public:
  VHAVOK_IMPEXP virtual void Reset() HKV_OVERRIDE;

  V_DECLARE_SERIAL_DLLEXP( vHavokBallAndSocketConstraintChainDesc, VHAVOK_IMPEXP )
  VHAVOK_IMPEXP virtual void Serialize(VArchive &ar) HKV_OVERRIDE;

public:
  float m_fMaximumLastLinkGap; ///< The maximum gap (in fractions of the link length) allowed between the end of the last chain link and the anchor to which it is attached.

private:
  static const unsigned int s_iSerialVersion; ///< The current serialization version for objects of this class.
};


// ----------------------------------------------------------------------------
// vHavokBallAndSocketConstraintChain
// ----------------------------------------------------------------------------

/// \brief
///   Implementation of vHavok Ball-and-Socket constraint chains.
class vHavokBallAndSocketConstraintChain : public vHavokConstraintChain
{
  ///
  /// @name Serialization and Type Management
  /// @{
  ///

public:
  V_DECLARE_SERIAL_DLLEXP( vHavokBallAndSocketConstraintChain, VHAVOK_IMPEXP )
  VHAVOK_IMPEXP VOVERRIDE void Serialize( VArchive &ar );

  ///
  /// @}
  ///

protected:

  ///
  /// @name vHavokConstraintChain Overrides
  /// @{
  ///

  VHAVOK_IMPEXP virtual void AppendConstraintInfo( const hkvVec3& vPivotA, 
    const hkvVec3& vPivotB, bool bIsAnchor = false) HKV_OVERRIDE;
  VHAVOK_IMPEXP virtual hkvVec3 CalcLinkPivot(unsigned int iIndex) HKV_OVERRIDE;
  VHAVOK_IMPEXP virtual hkpConstraintChainData* CreateConstraintChainData() HKV_OVERRIDE;
  VHAVOK_IMPEXP virtual vHavokConstraintChainDesc *CreateConstraintChainDesc() HKV_OVERRIDE;
  VHAVOK_IMPEXP virtual void FillRagdollConstraintInfo(
    vHavokRagdollConstraintInfo &ragdollInfo, 
    unsigned int iConstraintIndex) HKV_OVERRIDE;
  VHAVOK_IMPEXP virtual unsigned int GetConstraintInfoCount() HKV_OVERRIDE;
  VHAVOK_IMPEXP virtual hkVector4 GetImpulseApplied() const HKV_OVERRIDE;
  VHAVOK_IMPEXP virtual float GetLastLinkAllowedGap() const HKV_OVERRIDE;
  VHAVOK_IMPEXP virtual unsigned int GetMaxNumLinks() const HKV_OVERRIDE;
  VHAVOK_IMPEXP virtual bool GetParametersFromDesc(vHavokConstraintChainDesc const& desc) HKV_OVERRIDE;
  VHAVOK_IMPEXP virtual void InitConstraintChainDataFromDesc(hkpConstraintChainData& data, 
    vHavokConstraintChainDesc const& desc) HKV_OVERRIDE;
  VHAVOK_IMPEXP virtual void RemoveConstraintInfo(unsigned int iIndex) HKV_OVERRIDE;
  VHAVOK_IMPEXP virtual bool SupportsRagdollConstraints() const HKV_OVERRIDE { return true; }
  VHAVOK_IMPEXP virtual void UpdateChainPhysicsProperties() HKV_OVERRIDE;
  VHAVOK_IMPEXP virtual void UpdateConstraintInfo(unsigned int iIndex, 
    const hkvVec3& vPivotA, const hkvVec3& vPivotB) HKV_OVERRIDE;
  VHAVOK_IMPEXP virtual void UpdateRagdollConstraintState() HKV_OVERRIDE;
public:
  VHAVOK_IMPEXP virtual void SaveToDesc(vHavokConstraintChainDesc& desc) HKV_OVERRIDE;

  ///
  /// @}
  ///

private:
  float m_fMaximumLastLinkGap; ///< The maximum gap (in fractions of the link length) allowed between the end of the last chain link and the anchor to which it is attached.

  static const unsigned int s_iSerialVersion; ///< The current serialization version for objects of this class.
};

#endif //V_HAVOK_BALL_AND_SOCKET_CONSTRAINT_CHAIN_HPP_INCLUDED

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
