/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vHavokShapeCache.hpp

#ifndef VHAVOKSHAPECACHE_HPP_INCLUDED
#define VHAVOKSHAPECACHE_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsIncludes.hpp>

#if defined(SUPPORTS_TERRAIN)
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainSector.hpp>
#endif

#include <Common/Base/Container/StringMap/hkStorageStringMap.h>

class hkpShape;

/// \brief
///   Class with static functions for saving / loading Havok hkpShapes to / from file
class vHavokShapeCache
{
public:
  ///
  /// @name Initialization / DeInitialization
  /// @{
  ///

  /// 
  /// \brief
  ///   Initializes shape factory.
  /// 
  static void Init();

  /// 
  /// \brief
  ///   DeInitializes shape factory.
  /// 
  static void DeInit();

  ///
  /// @}
  ///

  ///
  /// @name Havok Physics Shape Caching Table
  /// @{
  ///

  /// \brief
  ///   Find shape that has been cached to disk using the shape ID.
  ///
  /// If the shape has already been loaded, it has been added to the internal shape caching table.
  /// In this case the shape is returned from the shape caching table.
  ///
  /// \param szShapeId
  ///   The shape ID of the shape.
  ///
  /// \param iFileTime
  ///   Optional parameter which is used to check whether the time stamp of the collision mesh
  ///   matches the cached shape file. An outdated shape will not be loaded and NULL is return instead.
  ///   Set to 0 if the file time stamp is not of interest.
  ///   If HKT-up-to-date checking is disabled, this parameter has no effect.
  ///
  /// \sa
  ///   IsHktUpToDateCheckingEnabled
  ///
  VHAVOK_IMPEXP static hkpShape* FindShapeOnDisk(const char* szShapeId, __int64 iFileTime);

  /// 
  /// \brief
  ///   Adds a Havok Physics shape with the given string ID to the cache table.
  /// 
  /// When adding a shape to the cache the reference count is incremented by 1. This way, 
  /// a reference to the shape is always kept even after all rigid bodies that reference it 
  /// have been removed. This way shapes don't have to be created multiple times, once they
  /// are added.
  ///
  /// \param szShapeId
  ///   ID string for the shape to be added.
  /// 
  /// \param pShape
  ///   Pointer to Havok Physics shape to be cached.
  ///
  VHAVOK_IMPEXP static void AddShape(const char *szShapeId, hkpShape *pShape);

  /// \brief
  ///   Removes a single shape from the cache if there is no other reference to it.
  ///
  /// \param pShape
  ///   The shape to remove.
  ///
  VHAVOK_IMPEXP static void RemoveShape(const hkpShape* pShape);

  /// 
  /// \brief
  ///   Purges all cached shapes.
  /// 
  /// When clearing the cache, the reference of the shape factory to all cached shapes
  /// is released and the internal table is cleared.
  ///
  /// \return
  ///   Returns TRUE if no other references to the shapes exist. It is advised to ensure
  ///   that this is always the case when this function is called.
  ///
  /// \sa AddShape
  ///
  VHAVOK_IMPEXP static bool Clear();

  /// 
  /// \brief
  ///   Finds a Havok Physics shape in the cache table.
  /// 
  /// \param szShapeId
  ///   Input: ID string of the collision shape to be found.
  /// 
  /// \return
  ///   hkpShape* : Pointer to Havok shape, or \c NULL if shape could not be found.
  ///
  VHAVOK_IMPEXP static hkpShape* FindShape(const char *szShapeId);

  ///
  /// \brief
  ///   Saves all shapes in the cache table, that are configured for it, to the disk.
  ///
  /// \param bOnlyModified
  ///   Specifies if only the cached shapes that were modified should be saved.
  ///
  VHAVOK_IMPEXP static void SaveAllShapesToDisk(bool bOnlyModified);

  ///
  /// @}
  ///

  ///
  /// @name Saving
  /// @{
  ///

  ///
  /// \brief
  ///   Saves a cached shape to a HKT file using the shape ID.
  ///
  /// The shape's ID must have been created by one of the helper functions in this class.
  ///
  /// \param pShape
  ///   Pointer to the shape to save.
  ///
  VHAVOK_IMPEXP static bool SaveShape(const hkpShape* pShape);

  ///
  /// \brief
  ///   Saves a cached shape to a HKT file using a file path.
  ///
  /// \param szFilePath
  ///   The path where the cached shape file is written to.
  ///
  /// \param pShape
  ///   Pointer to the shape to save.
  ///
  VHAVOK_IMPEXP static bool SaveShape(const char* szFilePath, const hkpShape* pShape);

#if defined(SUPPORTS_TERRAIN)
  ///
  /// \brief
  ///   Saves a terrain sector related shape to a HKT file.
  ///
  /// \param pSector
  ///   Pointer to the terrain sector for which the shape was created.
  /// 
  /// \param pShape
  ///   Pointer to the shape to save.
  ///
  VHAVOK_IMPEXP static bool SaveTerrainSectorShape(const VTerrainSector *pSector, const hkpShape *pShape);
#endif

  ///
  /// @}
  ///

  ///
  /// @name Loading
  /// @{
  ///

  ///
  /// \brief
  ///   Loads a shape from a HKT file. 
  ///
  /// \param szCachedShapeID
  ///   The string ID of the shape.
  //
  /// \returns 
  ///   Pointer to loaded shape if loading was successful, NULL otherwise.
  ///
  VHAVOK_IMPEXP static hkpShape* LoadShapeById(const char *szCachedShapeID);

  // TODO:
  VHAVOK_IMPEXP static hkpShape* LoadShapeByFileName(const char* szFilePath);

#if defined(SUPPORTS_TERRAIN)
  /// \brief
  ///   Loads a terrain sector related shape from HKT file. 
  ///
  /// \param pSector
  ///   Pointer to the terrain sector for which shape was created.
  /// 
  /// \returns 
  ///   Pointer to loaded shape if successful, NULL otherwise.
  VHAVOK_IMPEXP static hkpShape* LoadTerrainSectorShape(const VTerrainSector *pSector);
#endif

  ///
  /// @}
  ///

  ///
  /// @name Configuration
  /// @{
  ///

  /// \brief
  ///   Returns whether HKT files are checked on loading for being up to date.
  static inline bool IsHktUpToDateCheckingEnabled() 
  {
    return s_bCheckHktUpToDate;
  }

  /// 
  /// \brief
  ///   Sets whether HKT files are checked on loading for being up to date.
  /// 
  /// \param bEnable
  ///   Toggles up to date checking of HKT files.
  /// 
  /// \note
  ///   Per default this feature is only enabled for debug and development builds.
  static inline void EnableHktUpToDateChecking(bool bEnable)
  {
    s_bCheckHktUpToDate = bEnable;
  }

  static inline bool IsUpToDateWarningEnabled()
  {
    return s_bUpToDateWarningEnabled;
  }

  static inline void EnableUpToDateWarning(bool bEnable)
  {
    s_bUpToDateWarningEnabled = bEnable;
  }

  ///
  /// @}
  ///

  ///
  /// @name Path helpers 
  /// @{
  ///

  VHAVOK_IMPEXP static bool GetShapePath(const char* szShapeId, VStaticString<FS_MAX_PATH>& szOut, bool bNative);

  inline static bool GetShapePath(const hkpShape* pShape, VStaticString<FS_MAX_PATH>& szOut, bool bNative)
  {
    return GetShapePath(reinterpret_cast<const char*>(pShape->getUserData()), szOut, bNative);
  }

#if defined(SUPPORTS_TERRAIN)
  /// \brief
  ///   Returns path to a terrain sector shape file.
  VHAVOK_IMPEXP static bool GetTerrainSectorShapePath(VStaticString<FS_MAX_PATH>& szOut, const VTerrainSector* pSector, bool bIncludeHavok2VisionScale = true);
#endif

  // Shape IDs
  VHAVOK_IMPEXP static void ComputeShapeIdForConvexShape(char* szIDStringOut, const char* szMeshName, const hkvVec3& vScale, 
    unsigned int uiCreationFlags);

  VHAVOK_IMPEXP static void ComputeShapeIdForMeshShape(char* szIDStringOut, const char* szMeshName, const hkvVec3& vScale, 
    VisStaticMeshInstance_cl::VisCollisionBehavior_e eCollisionBehavior, VisWeldingType_e eWeldingType);

  ///
  /// @}
  ///

private:
  static void RemoveShape(const char *szShapeId);

  static bool IsShapeUpToDate(const hkpShape* pShape, __int64 iFileTime);
  static __int64 GetHktFileTime(const hkpShape* pShape);

  // Helper functions for interpreting the user data.
  static inline void SetShapeId(hkpShape* pShape, const char* szShapeId);
  static inline const char* GetShapeId(const hkpShape* pShape);

  // The up-to-date flag indicates if the shape is in sync with the cached version on the disk.
  static inline void SetUpToDateFlag(hkpShape* pShape);
  static inline bool GetUpToDateFlag(const hkpShape* pShape);

  VHAVOK_IMPEXP static bool s_bCheckHktUpToDate; // When true, HKT files are checked on loading whether they are still up to date.
  VHAVOK_IMPEXP static bool s_bUpToDateWarningEnabled;

  typedef hkStorageStringMap<hkpShape*> vShapeMap;
  static vShapeMap* m_pShapeCacheTable;  ///< Cached shapes stored in hash map.
};

#endif // VHAVOKSHAPECACHE_HPP_INCLUDED

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
