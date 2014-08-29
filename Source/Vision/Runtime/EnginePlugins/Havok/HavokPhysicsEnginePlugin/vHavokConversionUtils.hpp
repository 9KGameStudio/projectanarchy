/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vHavokConversionUtils.hpp

#ifndef VHAVOKCONVERSIONUTILS_HPP_INCLUDED
#define VHAVOKCONVERSIONUTILS_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsIncludes.hpp>

/*! Work around, see [SIM-41] */
/*  all load/store must be byte aligned to be able to attach double precision Physics to single precision Vision */

/// \brief
///   Macro: Scale a float from Vision to Havok space.
#define VIS2HK_FLOAT_SCALED(v) (v * vHavokConversionUtils::GetVision2HavokScale())

/// \brief
///   Macro: Scale a float from Havok to Vision space.
#define HK2VIS_FLOAT_SCALED(v) (v * vHavokConversionUtils::GetHavok2VisionScale())

///
/// \brief
///   Utility class with static functions for converting data types between Havok and Vision.
///
class vHavokConversionUtils
{
public:

  ///
  /// \brief
  ///   Converts a Havok Physics quaternion to a Vision matrix
  ///
  VHAVOK_IMPEXP static HK_FORCE_INLINE void HkQuatToVisMatrix(hkQuaternionParameter hkQuat, hkvMat3 &visMatrixOut)
  {
	  hkRotation rot;
	  rot.set(hkQuat); // out-of line!
	  HkRotationToVisMatrix(rot,visMatrixOut);
  }

  ///
  /// \brief
  ///   Converts a Havok Physics quaternion to a Vision quaternion
  ///
  VHAVOK_IMPEXP static HK_FORCE_INLINE void HkQuatToVisQuat(hkQuaternionParameter hkQuat, hkvQuat &visQuaternionOut)
  {
	  hkQuat.m_vec.store<4, HK_IO_NATIVE_ALIGNED>(visQuaternionOut.getDataPointer()); 
  }

  ///
  /// \brief
  ///   Converts a Vision quaternion to a Havok Physics quaternion
  ///
  VHAVOK_IMPEXP static HK_FORCE_INLINE void VisQuatToHkQuat(const hkvQuat &visQuaternion, hkQuaternion &hkQuatOut)
  {
	  hkQuatOut.m_vec.load<4, HK_IO_NATIVE_ALIGNED>(visQuaternion.getDataPointer());
	  HK_ASSERT(0xdee882, hkQuatOut.isOk());
  }

  ///
  /// \brief
  ///   Get a Havok Physics object space/scalar vector from a Vision vector; taking Vision scaling into account.
  ///
  VHAVOK_IMPEXP static HK_FORCE_INLINE void VisVecToPhysVecLocal(const hkvVec3& visV, hkVector4& physV)
  {
	  physV.load<3, HK_IO_NATIVE_ALIGNED>(&visV.data[0]);
	  physV.zeroComponent<3>();
	  physV.mul(m_cachedVis2PhysScale);
	  HK_ASSERT(0xdee883, physV.isOk<4>());
  }

  ///
  /// \brief
  ///   Get a Havok Physics object space/scalar vector from a Vision vector; taking Vision scaling into account.
  ///
  VHAVOK_IMPEXP static HK_FORCE_INLINE void VisVecToPhysVecLocal(const hkvVec3& visV, hkVector4d& physV)
  {
	  physV.load<3, HK_IO_NATIVE_ALIGNED>(&visV.data[0]);
	  physV.zeroComponent<3>();
	  physV.mul(m_cachedVis2PhysScaleD);
	  HK_ASSERT(0xdee883, physV.isOk<4>());
  }

  ///
  /// \brief
  ///   Get a Havok Physics object space/scalar vector from a Vision vector; taking Vision scaling into account.
  ///
  VHAVOK_IMPEXP static HK_FORCE_INLINE void VisVecToPhysVecLocal(const hkvVec3d& visV, hkVector4& physV)
  {
	  physV.load<3, HK_IO_NATIVE_ALIGNED>(&visV.data[0]);
	  physV.zeroComponent<3>();
	  physV.mul(m_cachedVis2PhysScale);
	  HK_ASSERT(0xdee883, physV.isOk<4>());
  }

  ///
  /// \brief
  ///   Get a Physics world space vector from a Vision render space vector; taking Vision scaling into account.
  ///
  VHAVOK_IMPEXP static HK_FORCE_INLINE void VisVecToPhysVecWorld(const hkvVec3& visV, hkVector4& physV)
  {
	  physV.load<3, HK_IO_NATIVE_ALIGNED>(&visV.data[0]);
	  physV.zeroComponent<3>();
	  physV.mul(m_cachedVis2PhysScale);
      physV.add(m_cachedWorldPivot);
	  HK_ASSERT(0xdee883, physV.isOk<4>());
  }

  ///
  /// \brief
  ///   Get a Physics world space vector from a Vision render space vector; taking Vision scaling into account.
  ///
  VHAVOK_IMPEXP static HK_FORCE_INLINE void VisVecToPhysVecWorld(const hkvVec3& visV, hkVector4d& physV)
  {
	  physV.load<3, HK_IO_NATIVE_ALIGNED>(&visV.data[0]);
	  physV.zeroComponent<3>();
	  physV.mul(m_cachedVis2PhysScaleD);
	  physV.add(m_cachedWorldPivotD);
	  HK_ASSERT(0xdee883, physV.isOk<4>());
  }

  ///
  /// \brief
  ///   Get a Havok Physics object space/scalar vector from a Vision vector; taking Vision scaling into account.
  ///
  VHAVOK_IMPEXP static HK_FORCE_INLINE void VisVecToPhysVecLocal(const hkvVec4& visV, hkVector4& physV)
  {
	  physV.load<4, HK_IO_NATIVE_ALIGNED>(&visV.data[0]);
	  physV.mul(m_cachedVis2PhysScale);
	  HK_ASSERT(0xdee884, physV.isOk<4>());
  }

  ///
  /// \brief
  ///   Get a Havok Physics world space vector from a Vision render space vector; taking Vision scaling into account.
  ///
  VHAVOK_IMPEXP static HK_FORCE_INLINE void VisVecToPhysVecWorld(const hkvVec4& visV, hkVector4& physV)
  {
	  physV.load<4, HK_IO_NATIVE_ALIGNED>(&visV.data[0]);
	  physV.mul(m_cachedVis2PhysScale);
	  physV.add(m_cachedWorldPivot);
	  HK_ASSERT(0xdee884, physV.isOk<4>());
  }

  ///
  /// \brief
  ///   Get a Vision object space/scalar vector from a Havok Physics vector; taking Vision scaling into account.
  ///
  VHAVOK_IMPEXP static HK_FORCE_INLINE void PhysVecToVisVecLocal(hkVector4Parameter physV, hkvVec3& visV)
  {
	  hkVector4 p; p.setMul(physV, m_cachedPhys2VisScale);
	  p.store<3, HK_IO_NATIVE_ALIGNED>(&visV.data[0]);
  }

  ///
  /// \brief
  ///   Get a Vision object space/scalar vector from a Havok Physics vector; taking Vision scaling into account.
  ///
  VHAVOK_IMPEXP static HK_FORCE_INLINE void PhysVecToVisVecLocal(hkVector4Parameter physV, hkvVec3d& visV)
  {
	  hkVector4 p; p.setMul(physV, m_cachedPhys2VisScale);
	  p.store<3, HK_IO_NATIVE_ALIGNED>(&visV.data[0]);
  }

  ///
  /// \brief
  ///   Get a Vision render space vector from a Havok Physics world space vector; taking Vision scaling into account.
  ///
  VHAVOK_IMPEXP static HK_FORCE_INLINE void PhysVecToVisVecWorld(hkVector4Parameter physV, hkvVec3& visV)
  {
	  hkVector4 p; p.setSub(physV, m_cachedWorldPivot); p.mul(m_cachedPhys2VisScale);
	  p.store<3, HK_IO_NATIVE_ALIGNED>(&visV.data[0]);
  }

  ///
  /// \brief
  ///   Get a Vision object space/scalar vector from a Havok Physics vector; taking Vision scaling into account.
  ///
  VHAVOK_IMPEXP static HK_FORCE_INLINE void PhysVecToVisVecLocal(hkVector4Parameter physV, hkvVec4& visV)
  {
	  hkVector4 p; p.setMul(physV, m_cachedPhys2VisScale);
	  p.store<4, HK_IO_NATIVE_ALIGNED>(&visV.data[0]);
  }

  ///
  /// \brief
  ///   Get a Vision render space vector from a Havok Physics world vector; taking Vision scaling into account.
  ///
  VHAVOK_IMPEXP static HK_FORCE_INLINE void PhysVecToVisVecWorld(hkVector4Parameter physV, hkvVec4& visV)
  {
	  hkVector4 p; p.setSub(physV, m_cachedWorldPivot); p.mul(m_cachedPhys2VisScale);
	  p.store<4, HK_IO_NATIVE_ALIGNED>(&visV.data[0]);
  }

  ///
  /// \brief
  ///   Get a Havok Physics vector from a Vision vector; ignoring Vision scaling.
  ///
  VHAVOK_IMPEXP static HK_FORCE_INLINE void VisVecToPhysVec_noscale(const hkvVec3& visV, hkVector4& physV)
  {
	  physV.load<3, HK_IO_NATIVE_ALIGNED>(&visV.data[0]);
	  physV.zeroComponent<3>();
	  HK_ASSERT(0xdee885, physV.isOk<4>());
  }

  ///
  /// \brief
  ///   Get a Havok Physics vector from a Vision vector; ignoring Vision scaling.
  ///
  VHAVOK_IMPEXP static HK_FORCE_INLINE void VisVecToPhysVec_noscale(const hkvVec4& visV, hkVector4& physV)
  {
	  physV.load<4, HK_IO_NATIVE_ALIGNED>(&visV.data[0]);
	  HK_ASSERT(0xdee886, physV.isOk<4>());
  }

  ///
  /// \brief
  ///   Get a Vision vector from a Havok Physics vector; ignoring Vision scaling.
  ///
  VHAVOK_IMPEXP static HK_FORCE_INLINE void PhysVecToVisVec_noscale(hkVector4Parameter physV, hkvVec3& visV)
  {
	  physV.store<3, HK_IO_NATIVE_ALIGNED>(&visV.data[0]);
  }

  ///
  /// \brief
  ///   Get a Vision vector from a Havok Physics vector; ignoring Vision scaling.
  ///
  VHAVOK_IMPEXP static HK_FORCE_INLINE void PhysVecToVisVec_noscale(hkVector4Parameter physV, hkvVec4& visV)
  {
	  physV.store<4, HK_IO_NATIVE_ALIGNED>(&visV.data[0]);
  }

  ///
  /// \brief
  ///   Converts a Havok Physics transformation to a Vision rotation matrix and translation vector 
  //    taking the Vision scale into account.
  ///
  /// \param hkTf
  ///   Incoming Havok transformation
  ///
  /// \param visPositionOut
  ///   Converted position as Vision vector instance (including Vision scale) [out]
  ///
  /// \param visRotMatrixOut
  ///   Converted rotation as Vision matrix instance [out]
  ///
  VHAVOK_IMPEXP static void PhysTransformToVisMatVec(const hkTransform &hkTf, hkvMat3 &visRotMatrixOut, 
    hkvVec3& visPositionOut);

  ///
  /// \brief
  ///   Converts a Havok Physics transformation to a Vision rotation matrix and render space vector 
  ///   taking the Vision scale into account.
  ///
  /// \param hkTf
  ///   Incoming transformation from Havok Physics.
  ///
  /// \param visPositionOut
  ///   Converted position as Vision vector instance in render space (including Vision scale). [out]
  ///
  /// \param visRotMatrixOut
  ///   Converted rotation as Vision matrix instance. [out]
  ///
  VHAVOK_IMPEXP static void PhysTransformToVisMatVecWorld(const hkTransform &hkTf, hkvMat3 &visRotMatrixOut, 
    hkvVec3& visPositionOut);

  ///
  /// \brief
  ///   Converts a Vision rotation matrix and translation vector to a Havok Physics transform 
  ///   taking the Vision scale into account.
  ///
  /// \param visPosition
  ///   Incoming Vision position 
  ///
  /// \param visRotMatrix
  ///   Incoming Vision rotation
  ///
  /// \param hkTfOut
  ///   Converted position and rotation to Havok Physics transform (including Physics scale). [out]
  ///
  VHAVOK_IMPEXP static void VisMatVecToPhysTransform(const hkvMat3 &visRotMatrix, const hkvVec3& visPosition, 
    hkTransform &hkTfOut);

  ///
  /// \brief
  ///   Converts a Vision rotation matrix and translation vector to a Havok Physics transform 
  ///   taking the Vision scale into account.
  ///
  /// \param visPosition
  ///   Incoming Vision position 
  ///
  /// \param visRotMatrix
  ///   Incoming Vision rotation
  ///
  /// \param hkTfOut
  ///   Converted position and rotation to Havok Physics transform (including Physics scale and world pivot). [out]
  ///
  VHAVOK_IMPEXP static void VisMatVecToPhysTransformWorld(const hkvMat3 &visRotMatrix, const hkvVec3& visPosition, 
    hkTransform &hkTfOut);

  ///
  /// \brief
  ///   General method to convert a Havok Physics 4x4 matrix to a Vision 4x4 matrix.
  ///
  /// \param hkMatrix
  ///   Incoming Havok Physics transform.
  ///
  /// \param visMatrixOut
  ///   Resulting Vision matrix instance (scaling according to parameters) [out]
  ///
  /// \param bScalePosition
  ///   Specifies whether the outgoing position value is scaled from Havok Physics to Vision space.
  ///
  /// \param bScaleRotation
  ///   Specifies whether the outgoing rotation value is scaled from Havok Physics to Vision space.
  ///
  /// \param bIsWorldTransform
  ///		Specifies whether to convert from Havok Physics world space to Vision render space.
  ///
  VHAVOK_IMPEXP static void HkMatrixToVisMatrix(const hkMatrix4 &hkMatrix, hkvMat4 &visMatrixOut, 
    bool bScalePosition = true, bool bScaleRotation = false, bool bIsWorldTransform = true);

  ///
  /// \brief
  ///   Converts a Vision rotation matrix to a Havok Physics rotation matrix.
  ///
  VHAVOK_IMPEXP static HK_FORCE_INLINE void VisMatrixToHkRotation(const hkvMat3 &visRotMatrix, hkRotation &hkRotOut)
  {
	  // Extract matrix columns from Vision
	  const float* visRot = (const float*)visRotMatrix.getPointer();
	  hkRotOut.getColumn<0>().load<3,HK_IO_NATIVE_ALIGNED>(visRot  );
	  hkRotOut.getColumn<0>().zeroComponent<3>();
	  hkRotOut.getColumn<1>().load<3,HK_IO_NATIVE_ALIGNED>(visRot+3);
	  hkRotOut.getColumn<1>().zeroComponent<3>();
	  hkRotOut.getColumn<2>().load<3,HK_IO_NATIVE_ALIGNED>(visRot+6);
	  hkRotOut.getColumn<2>().zeroComponent<3>();
	  HK_ASSERT(0xdee887, hkRotOut.isOk() /*&& hkRotOut.isOrthonormal()*/);
  }

  ///
  /// \brief
  ///   General method to convert a Vision 4x4 matrix to a Havok Physics 4x4 matrix.
  ///
  /// \param visMatrix
  ///   Incoming Vision transformation
  ///
  /// \param hkMatOut
  ///   Resulting Havok matrix instance (scaling according to parameters) [out]
  ///
  /// \param bScalePosition
  ///   Specifies whether the outgoing position value is scaled from Vision to Havok space
  ///
  /// \param bScaleRotation
  ///   Specifies whether the outgoing rotation value is scaled from Vision to Havok space
  ///
  /// \param bIsWorldTransform
  ///		Specifies whether to to convert from Vision render space to Havok world space
  ///
  VHAVOK_IMPEXP static void VisMatrixToHkMatrix(const hkvMat4 &visMatrix, hkMatrix4& hkMatOut, 
    bool bScalePosition = true, bool bScaleRotation = false, bool bIsWorldTransform = true);

  ///
  /// \brief
  ///   Converts a Vision rotation matrix to a Havok Physics quaternion.
  ///
  /// \param visRotMatrix
  ///   Incoming Vision transformation
  ///
  /// \param hkQuatOut
  ///   Resulting Havok Physics quaternion instance [out]
  ///
  VHAVOK_IMPEXP static HK_FORCE_INLINE void VisMatrixToHkQuat(const hkvMat3 &visRotMatrix, hkQuaternion &hkQuatOut)
  {
	  hkRotation hkRotMtx;
	  VisMatrixToHkRotation(visRotMatrix, hkRotMtx);
	  hkQuatOut.set(hkRotMtx); // out-of line!
	  hkQuatOut.normalize(); // strip scale
	  HK_ASSERT(0xdee888, hkQuatOut.isOk());
  }

  ///
  /// \brief
  ///   Converts a Havok Physics rotation matrix to a Vision rotation matrix.
  ///
  VHAVOK_IMPEXP static HK_FORCE_INLINE void HkRotationToVisMatrix(const hkRotation &hkRot, hkvMat3 &visRotMatrixOut)
  {
	  // Extract matrix columns from Havok, both col-major
	  float* visRot = (float*)visRotMatrixOut.getPointer();
	  hkRot.getColumn<0>().store<3,HK_IO_NATIVE_ALIGNED>(visRot  );
	  hkRot.getColumn<1>().store<3,HK_IO_NATIVE_ALIGNED>(visRot+3);
	  hkRot.getColumn<2>().store<3,HK_IO_NATIVE_ALIGNED>(visRot+6);
  }

  ///
  /// \brief
  ///   Converts a Havok Physics rotation matrix to a Vision quaternion matrix.
  ///
  VHAVOK_IMPEXP static HK_FORCE_INLINE void HkRotationToVisQuat(const hkRotation &hkRot, hkvQuat &visQuaternionOut)
  {
	  hkQuaternion q;
	  q.set(hkRot); // out-of line!
	  HkQuatToVisQuat(q, visQuaternionOut);
  }

  ///
  /// \brief
  ///   Returns the scale factor for converting from Havok Physics to Vision coordinates.
  ///
  VHAVOK_IMPEXP static HK_FORCE_INLINE hkReal GetHavok2VisionScale() 
  { 
    return m_cachedHavok2VisionScale; 
  }

  ///
  /// \brief
  ///   Returns the scale factor for converting from Vision Physics to Havok coordinates.
  ///
  VHAVOK_IMPEXP static HK_FORCE_INLINE hkReal GetVision2HavokScale() 
  { 
    return m_cachedVision2HavokScale; 
  }

  ///
  /// \brief
  ///   Gets the scale factor for converting from Havok Physics to Vision coordinates. (SIMD version)
  ///
  VHAVOK_IMPEXP static HK_FORCE_INLINE hkSimdReal GetHavok2VisionScaleSIMD() 
  { 
    return m_cachedPhys2VisScale; 
  }

  ///
  /// \brief
  ///   Gets the scale factor for converting from Vision to Havok Physics coordinates. (SIMD version)
  ///
  VHAVOK_IMPEXP static HK_FORCE_INLINE hkSimdReal GetVision2HavokScaleSIMD() 
  { 
    return m_cachedVis2PhysScale; 
  }

  ///
  /// \brief
  ///   Sets the scale factor for conversion from Havok Physics to Vision coordinates. Called by physics module.
  ///
  VHAVOK_IMPEXP static void SetHavok2VisionScale(hkReal fScale);

  ///
  /// \brief
  ///   Update the current world pivot.
  ///
  VHAVOK_IMPEXP static void SetVisionWorldPivot(const hkvVec3d& worldPivot);

  ///
  /// \brief
  ///   Update the current world pivot.
  ///
  VHAVOK_IMPEXP static const hkVector4& GetVisionWorldPivot();

  /// \brief
  ///   Converts Vision welding type to Havok welding type.
  VHAVOK_IMPEXP static hkpWeldingUtility::WeldingType VisToHkWeldingType(enum VisWeldingType_e eWeldingType);

protected:
  VHAVOK_IMPEXP static hkReal m_cachedHavok2VisionScale;  ///< Cached value of the Havok2Vision scale factor (original value is stored in physics module)
  VHAVOK_IMPEXP static hkReal m_cachedVision2HavokScale;  ///< Cached value of the Vision2Havok scale factor (original value is stored in physics module)
  VHAVOK_IMPEXP static hkSimdReal m_cachedVis2PhysScale;  ///< Cached value of the Havok2Vision scale factor for SIMD (original value is stored in physics module)
  VHAVOK_IMPEXP static hkSimdReal m_cachedPhys2VisScale;  ///< Cached value of the Vision2Havok scale factor for SIMD (original value is stored in physics module)
  VHAVOK_IMPEXP static hkSimdDouble64 m_cachedVis2PhysScaleD;  ///< Cached value of the Havok2Vision scale factor for SIMD (original value is stored in physics module)
  VHAVOK_IMPEXP static hkSimdDouble64 m_cachedPhys2VisScaleD;  ///< Cached value of the Vision2Havok scale factor for SIMD (original value is stored in physics module)
  VHAVOK_IMPEXP static hkVector4 m_cachedWorldPivot;			///< Cached value of the world pivot. If using the single precision Havok Physics build, this can suffer from precision loss.
  VHAVOK_IMPEXP static hkVector4d m_cachedWorldPivotD;			///< Cached value of the world pivot in double precision.
};

#endif // VHAVOKCONVERSIONUTILS_HPP_INCLUDED

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
