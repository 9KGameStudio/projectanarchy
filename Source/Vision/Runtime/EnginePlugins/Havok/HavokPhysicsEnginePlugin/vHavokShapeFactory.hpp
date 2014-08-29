/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vHavokShapeFactory.hpp

#ifndef VHAVOKSHAPEFACTORY_HPP_INCLUDED
#define VHAVOKSHAPEFACTORY_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsIncludes.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokRigidBody.hpp>

#define HKVIS_MESH_SHAPE_TOLERANCE 0.1f
#define HKVIS_CONVEX_SHAPE_TOLERANCE 0.01f

/// 
/// \brief
///   Factory class with static functions to create Havok shapes.
/// 
class vHavokShapeFactory
{
public:
  ///
  /// @name Havok Shape Creation
  /// @{
  ///

  /// 
  /// \brief
  ///   Enumeration of Havok Physics shape creation flags
  ///
  enum VShapeCreationFlags_e
  {
    VShapeCreationFlags_CACHE_SHAPE         = V_BIT(0), ///< Allow runtime/ disc shape caching 
    VShapeCreationFlags_CONVEX              = V_BIT(1), ///< Force creation of convex shull hape. If not specified VColmesh info is used to determine shape type.
    VShapeCreationFlags_MESH                = V_BIT(2), ///< Force creation of compressed bounding volume mesh shape. If not specified VColmesh info is used to determine shape type.
    VShapeCreationFlags_USE_VCOLMESH        = V_BIT(3), ///< Try using collision mesh (.vcolmesh)
    VShapeCreationFlags_SHRINK              = V_BIT(4), ///< Shrink by the convex radius
  };

  /// 
  /// \brief
  ///   Factory function for creating a Havok Physics shape.
  /// 
  /// This function takes the vertex data of the given mesh including the corresponding collision mesh
  /// and creates a convex geometry which is used for the Havok Physics shape representation. This shape gets cached 
  /// on disk relative to the mesh's file name, also taking the scaling into account.
  ///
  /// \param pMesh
  ///   Pointer to the mesh.
  ///
  /// \param vScale
  ///   Scaling vector for the Havok Physics convex shape.
  ///
  /// \param uiCreationFlags
  ///   Bit mask for multiple VShapeCreationFlags_e flags.
  ///
  /// \param eWeldingType
  ///   The welding type. Note that this parameter will only be used if a compressed bounding volume mesh shape is created.
  ///
  /// \returns
  ///   Pointer to Havok Physics shape.
  /// 
  VHAVOK_IMPEXP static hkRefNew<hkpShape> CreateShapeFromMesh(VBaseMesh* pMesh, const hkvVec3& vScale, 
    unsigned int uiCreationFlags, VisWeldingType_e eWeldingType = VIS_WELDING_TYPE_NONE);

  /// 
  /// \brief
  ///   Factory function for creating a Havok Physics shape from static mesh instances.
  /// 
  /// \param instances
  ///   Collection of static mesh instances for which the shape gets created.
  ///
  /// \param uiCreationFlags
  ///   Bit mask for multiple VShapeCreationFlags_e flags.
  ///
  /// \returns
  ///   Pointer to Havok Physics shape.
  /// 
  inline static hkRefNew<hkpShape> CreateShapeFromStaticMeshInstances(const VisStaticMeshInstCollection &instances, unsigned int uiCreationFlags)
  {
    return CreateShapeFromStaticMeshInstances(instances.GetPtrs(), static_cast<unsigned int>(instances.GetLength()), uiCreationFlags);
  }

  /// 
  /// \brief
  ///   Factory function for creating a Havok Physics shape from static mesh instances.
  ///
  /// \param ppInstances
  ///   Array of static mesh instances for which the shape gets created.
  ///
  /// \param uiNumInstances
  ///   The number of static mesh instances.
  ///
  /// \param uiCreationFlags
  ///   Bit mask for multiple VShapeCreationFlags_e flags.
  ///
  /// \returns
  ///   Pointer to Havok Physics shape.
  /// 
  VHAVOK_IMPEXP static hkRefNew<hkpShape> CreateShapeFromStaticMeshInstances(VisStaticMeshInstance_cl const* const* ppInstances, unsigned int uiNumInstances, unsigned int uiCreationFlags);

  ///
  /// \brief
  ///   Factory function for creating a Havok Physics shape from a single static mesh instance.
  ///
  /// \param pInstance
  ///   Static mesh instance for which the Havok Physics shape gets created.
  ///
  /// \param uiCreationFlags
  ///   Bit mask for multiple VShapeCreationFlags_e flags.
  ///
  /// \returns
  ///   Pointer to Havok Physics shape.
  ///
  inline static hkRefNew<hkpShape> CreateShapeFromStaticMeshInstance(const VisStaticMeshInstance_cl* pInstance, unsigned int uiCreationFlags)
  {
    return CreateShapeFromStaticMeshInstances(&pInstance, 1, uiCreationFlags);
  }

#if defined(SUPPORTS_TERRAIN)

  /// 
  /// \brief
  ///   Factory function to create a Havok Physics shape based on a height field Shape (hkvSampledHeightfield). 
  ///   It can optionally be wrapped by a triangle sampler so that collisions are exact.
  /// 
  /// \param sector
  ///   Terrain sector which will be represented by the Havok Physics height field shape.
  ///
  /// \returns
  ///   Pointer to Havok Physics shape.
  /// 
  VHAVOK_IMPEXP static hkpShape* CreateShapeFromTerrain(const VTerrainSector &sector);

#endif

  ///
  /// @}
  ///

  ///
  /// @name Helpers
  /// @{
  ///

  /// 
  /// \brief
  ///   Gets the pivot offset of the bounding box relative to the mesh's pivot.
  ///   
  /// \param pMesh
  ///   Pointer to the mesh.
  ///
  /// \param vScaleEntity
  ///   Scaling of the mesh instance.
  ///
  /// \returns
  ///   Pivot offset vector (in Vision units).
  /// 
  static const hkvVec3 GetPivotOffset(const VDynamicMesh *pMesh, const hkvVec3& vScaleEntity);

  /// 
  /// \brief
  ///   Gets the pivot offset of the bounding box relative to the mesh's pivot.
  ///   
  /// \param pMesh
  ///   Pointer to the mesh.
  ///
  /// \param fUniformScaleEntity
  ///   Uniform scaling of the mesh instance.
  ///
  /// \returns
  ///   Pivot offset vector (in Vision units).
  /// 
  static const hkvVec3 GetPivotOffset(const VDynamicMesh *pMesh, float fUniformScaleEntity);

  /// 
  /// \brief
  ///   Builds a hkGeometry instance from the specified sub mesh of a collision mesh.
  /// 
  /// \param pColMesh
  ///   The collision mesh containing to the sub mesh, from which to build the geometry.
  /// 
  /// \param iSubmeshIndex
  ///   Index of the sub mesh within the collision mesh  
  ///   
  /// \param transform
  ///   Transformation matrix applied to the hkGeometry object.
  ///
  /// \param bConvex
  ///   Specifies whether the hkGeometry object will be used for convex or mesh shapes.
  /// 
  /// \param [out] geom 
  ///   Reference to a hkGeometry object which receives the results.
  ///
  static void BuildGeometryFromCollisionMesh(const IVCollisionMesh *pColMesh, int iSubmeshIndex, const hkvMat4 &transform, bool bConvex, hkGeometry& geom);

  ///
  /// @}
  ///

  ///
  /// @name Resource Dependencies
  /// @{
  ///

#if defined(SUPPORTS_SNAPSHOT_CREATION)

  /// \brief
  ///   Gets HKT file dependencies of a static mesh instance for precaching in a resource snapshot.
  static void GetHktDependencies(VResourceSnapshot &snapshot, VisStaticMeshInstance_cl *pMeshInstance);

  /// \brief
  ///   Gets HKT file dependencies of an entity for precaching in a resource snapshot.
  static void GetHktDependencies(VResourceSnapshot &snapshot, VisBaseEntity_cl *pEntity);

  /// \brief
  ///   Gets HKT file dependencies of a terrain sector for precaching in a resource snapshot.
  static void GetHktDependencies(VResourceSnapshot &snapshot, VTerrainSector *pSector);

#endif

  ///
  /// @}
  ///

private:
  static hkpShape* CreateConvexHullShapeFromMesh(VBaseMesh* pMesh, const hkvMat4& mTransform, unsigned int uiCreationFlags);
  static hkpShape* CreateCompressedBvMeshShapeFromMesh(VBaseMesh* pMesh, const hkvMat4& mTransform, unsigned int uiCreationFlags, VisWeldingType_e eWeldingType);
  static hkpShape* CreateMeshShapeFromStaticMeshInstances(VisStaticMeshInstance_cl const* const* ppInstances, unsigned int uiNumInstances, unsigned int uiCreationFlags);

  static inline IVCollisionMesh* GetCollisionMesh(VBaseMesh* pMesh, unsigned int uiCreationFlags)
  {
    return (uiCreationFlags & VShapeCreationFlags_USE_VCOLMESH) != 0 ? 
      pMesh->GetCollisionMesh(true, true) : pMesh->GetTraceMesh(true, true);
  }

  static inline bool IsShapeConvex(VBaseMesh* pMesh, unsigned int uiCreationFlags)
  {
    if ((uiCreationFlags & VShapeCreationFlags_CONVEX) != 0)
      return true;
    else if ((uiCreationFlags & VShapeCreationFlags_MESH) != 0)
      return false;
    else
      return (GetCollisionMesh(pMesh, uiCreationFlags)->GetType() == VIS_COLMESH_GEOTYPE_CONVEXHULL);
  }

  static const hkvMat4 ComputeReferenceTransform(const hkvMat4& mTransform); 
  static const hkvVec3 ExtractScaling(const hkvMat4& mTransform);
};

#endif // VHAVOKSHAPEFACTORY_HPP_INCLUDED

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
