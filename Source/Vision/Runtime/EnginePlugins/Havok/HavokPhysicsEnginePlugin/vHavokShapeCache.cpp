/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokShapeCache.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokShapeFactory.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokVisionShapes.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokFileStreamAccess.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>

#include <Common/Base/Config/hkConfigVersion.h>
#include <Common/Base/System/Io/Writer/Buffered/hkBufferedStreamWriter.h>
#include <Common/Base/System/Io/Reader/Buffered/hkBufferedStreamReader.h>
#include <Common/Base/Reflection/hkClassMemberAccessor.h>
#include <Common/Serialize/Util/hkSerializeUtil.h>
#include <Common/Base/Reflection/Registry/hkVtableClassRegistry.h>

#if defined(HK_DEBUG_SLOW) || defined(HK_DEBUG)
  bool vHavokShapeCache::s_bCheckHktUpToDate = true;
#else
  bool vHavokShapeCache::s_bCheckHktUpToDate = false;
#endif

bool vHavokShapeCache::s_bUpToDateWarningEnabled = true;

vHavokShapeCache::vShapeMap* vHavokShapeCache::m_pShapeCacheTable = NULL;

//-----------------------------------------------------------------------------------
// Cache Table

void vHavokShapeCache::Init()
{
  // hkStoragStringMap already allocates memory at construction time. 
  // Since the hkBase memory system is not yet initialized at static construction time, the creation must be done later.
  m_pShapeCacheTable = new vShapeMap();
}

void vHavokShapeCache::DeInit()
{
  V_SAFE_DELETE(m_pShapeCacheTable);
}

hkpShape* vHavokShapeCache::FindShapeOnDisk(const char* szShapeId, __int64 iFileTime)
{
  // Try to find in cache table.
  hkpShape* pShape = FindShape(szShapeId);
  if (pShape != HK_NULL)
  {
    pShape->addReference();
    return pShape;
  }

  // Try to find a cached version on disk.
  if (vHavokPhysicsModule_GetDefaultWorldRuntimeSettings().m_bEnableDiskShapeCaching == TRUE)
  {
    pShape = vHavokShapeCache::LoadShapeById(szShapeId);

    if (pShape != NULL)
    {
      // Check if HKT is up-to-date.
      if (s_bCheckHktUpToDate && !IsShapeUpToDate(pShape, iFileTime))
      {
        if (!Vision::Editor.IsInEditor() && s_bUpToDateWarningEnabled)
        {
          VStaticString<FS_MAX_PATH> shapePath;
          GetShapePath(szShapeId, shapePath, false);
          hkvLog::Warning("Loading '%s' failed since HKT file is outdated. Please re-generate HKT file (see documentation for details).", shapePath.AsChar());
        }

        return HK_NULL;
      }

      AddShape(szShapeId, pShape);
      SetUpToDateFlag(pShape);
      return pShape;
    }
  }

  // not found
  return HK_NULL;
}

void vHavokShapeCache::AddShape(const char *szShapeId, hkpShape *pShape)
{
  VASSERT(m_pShapeCacheTable != NULL && szShapeId != NULL);
  VASSERT_MSG(!m_pShapeCacheTable->isValid(m_pShapeCacheTable->findKey(szShapeId)),
    "Shape with ID '%s' is already cached", szShapeId);

  // Get pointer to string that is stored internally.
  const char* szStoredShapeId = m_pShapeCacheTable->insert(szShapeId, pShape);

  // The table keeps one reference for itself.
  pShape->addReference();

  // Save ID string pointer as user data. This way we can search for it in the table when removing the shape.
  SetShapeId(pShape, szStoredShapeId);
}

void vHavokShapeCache::RemoveShape(const hkpShape* pShape)
{
  RemoveShape(GetShapeId(pShape));
}

void vHavokShapeCache::RemoveShape(const char *szShapeId)
{
  VASSERT(m_pShapeCacheTable != NULL);
  if (szShapeId == NULL)
    return;

  vShapeMap::Iterator iter = m_pShapeCacheTable->findKey(szShapeId);
  if (!m_pShapeCacheTable->isValid(iter))
    return;

  hkpShape* pShape = m_pShapeCacheTable->getValue(iter);
  if (pShape == NULL)
    return;

  if (pShape->getReferenceCount() == 1)
  {
    m_pShapeCacheTable->remove(iter);

    pShape->setUserData(0);
    pShape->removeReference();
  }
}

bool vHavokShapeCache::Clear()
{
  VASSERT(m_pShapeCacheTable != NULL);

  // Iterate over all shapes in cache and check whether all shapes have a reference count 
  // of 1 before releasing the reference.
  bool bRefCountSafe = true;
  
  // Remove reference from all shapes
  vShapeMap::Iterator iter = m_pShapeCacheTable->getIterator();
  while (m_pShapeCacheTable->isValid(iter))
  {
    hkpShape* pShape = m_pShapeCacheTable->getValue(iter);
    bRefCountSafe &= (pShape->getReferenceCount() == 1);
    pShape->removeReference();
    iter = m_pShapeCacheTable->getNext(iter);
  }

  m_pShapeCacheTable->clear();
 
  return bRefCountSafe;
}

hkpShape* vHavokShapeCache::FindShape(const char* szShapeId)
{
  VASSERT(m_pShapeCacheTable != NULL && szShapeId != NULL);

  vShapeMap::Iterator iter = m_pShapeCacheTable->findKey(szShapeId);
  if (m_pShapeCacheTable->isValid(iter))
  {
    return m_pShapeCacheTable->getValue(iter);
  }

  return HK_NULL;
}

void vHavokShapeCache::SaveAllShapesToDisk(bool bOnlyModified)
{
  vShapeMap::Iterator iter = m_pShapeCacheTable->getIterator();
  while (m_pShapeCacheTable->isValid(iter))
  {
    const hkpShape* pShape = m_pShapeCacheTable->getValue(iter);

    VASSERT(pShape->getUserData() != HK_NULL);
    if (!bOnlyModified || !GetUpToDateFlag(pShape))
      vHavokShapeCache::SaveShape(pShape);

    iter = m_pShapeCacheTable->getNext(iter);
  }
}

//-----------------------------------------------------------------------------------
// Shape IDs

void vHavokShapeCache::ComputeShapeIdForConvexShape(char* szIDStringOut, const char* szMeshName, const hkvVec3& vScale, unsigned int uiCreationFlags)
{
  // Use Havok instead of Vision scale to account for global HavokToVision scale. 
  const hkvVec3 vHavokScale = vScale * vHavokConversionUtils::GetVision2HavokScale();

  // The max. filename length on Xbox360 is 40 chars. The worst case should be 37 chars here.
  if ((uiCreationFlags & vHavokShapeFactory::VShapeCreationFlags_SHRINK) != 0)
    sprintf(szIDStringOut, "|%s|C_%.3g_%.3g_%.3g_t", szMeshName, vHavokScale.x, vHavokScale.y, vHavokScale.z);
  else
    sprintf(szIDStringOut, "|%s|C_%.3g_%.3g_%.3g", szMeshName, vHavokScale.x, vHavokScale.y, vHavokScale.z);
}

void vHavokShapeCache::ComputeShapeIdForMeshShape(char* szIDStringOut, const char* szMeshName, const hkvVec3& vScale, 
  VisStaticMeshInstance_cl::VisCollisionBehavior_e eCollisionBehavior, VisWeldingType_e eWeldingType) 
{
  // Use Havok instead of Vision scale to account for global HavokToVision scale.
  const hkvVec3 vHavokScale = vScale * vHavokConversionUtils::GetVision2HavokScale();

  // The max. filename length on Xbox360 is 40 chars. The worst case should be 39 chars here.
  sprintf(szIDStringOut, "|%s|M_%.3g_%.3g_%.3g_%i_%i", szMeshName, vHavokScale.x, vHavokScale.y, vHavokScale.z, 
    static_cast<int>(eCollisionBehavior), static_cast<int>(eWeldingType));
}

//-----------------------------------------------------------------------------------
// Saving 

bool vHavokShapeCache::SaveShape(const hkpShape* pShape)
{
  VASSERT(pShape != HK_NULL);

  VStaticString<FS_MAX_PATH> szNativePath;
  if (!GetShapePath(GetShapeId(pShape), szNativePath, true))
    return false;

  return SaveShape(szNativePath, pShape);
}

bool vHavokShapeCache::SaveShape(const char* szFilePath, const hkpShape* pShape)
{
  VASSERT(szFilePath != NULL && pShape != NULL);

  // Make sure it's editable.
  VRCSHelper::RCSEditFile(szFilePath);

  // Save shape to file.
  {
    vHavokStreamWriter writer(szFilePath);
    hkBufferedStreamWriter seekable(&writer);
    hkSerializeUtil::ErrorDetails err;
    hkResult res = hkSerializeUtil::save(pShape, &seekable); 

    if (res == HK_FAILURE)
    {
      hkvLog::Warning("vHavokShapeCache: Save of %s failed", szFilePath);
      return false;
    }

    // Make sure the file write is closed, before trying to add it to P4.
  }

  // Make sure it's added.
  VRCSHelper::RCSAddFile(szFilePath, true /* Binary file */);

  // Disk version is now up-to-date.
  SetUpToDateFlag(const_cast<hkpShape*>(pShape));

  return true;
}

hkpShape* vHavokShapeCache::LoadShapeById(const char *szCachedShapeId)
{
  VASSERT(szCachedShapeId != NULL);

  VStaticString<FS_MAX_PATH> szFilePath(szCachedShapeId);
  if (!GetShapePath(szCachedShapeId, szFilePath, false))
    return HK_NULL;

  return LoadShapeByFileName(szFilePath);
}

hkpShape* vHavokShapeCache::LoadShapeByFileName(const char* szFilePath)
{
  // Load cached shape from file.
  vHavokStreamReader reader(szFilePath);
  if (!reader.isOk())
  {
#if defined(HK_DEBUG)
    if (!Vision::Editor.IsInEditor() && s_bUpToDateWarningEnabled)
    {
      hkvLog::Warning("Cached HKT file '%s' is missing. Please generate HKT file (see documentation for details).", szFilePath);
    }
#endif
    return HK_NULL;
  }

  hkBufferedStreamReader seekable(&reader);
  hkSerializeUtil::ErrorDetails err;

#if 0 // Enforce only exact version number here if you want to be sure to get all latest compression fixes in shapes
  hkSerializeUtil::FormatDetails formatDetails;
  hkSerializeUtil::detectFormat( &seekable, formatDetails);
  if (formatDetails.m_version.cString() && (hkString::strCmp( formatDetails.m_version.cString(), HAVOK_SDK_VERSION_NUM_STRING ) != 0))
  {
    hkStringBuf fullWarning( "vHavokShapeCache::Load of ", szCachedShapeName, " failed due to non exact Havok SDK version:");
    fullWarning.appendPrintf("(%s in file, %s runtime)",formatDetails.m_version.cString(), HAVOK_SDK_VERSION_NUMBER );
    hkvLog::Warning( fullWarning.cString() );
    return HK_NULL;
  }
#endif

  // Note we are on purpose failing on versioning as any issues in bvtreecompressed shape in the compression etc are not patched (also patch may be slower than just create again). So best to fail and cause an implicit resave.
  hkObjectResource* fileContents = hkSerializeUtil::loadOnHeap(&seekable, &err, hkSerializeUtil::LOAD_FAIL_IF_VERSIONING); 
  hkpShape *pShape = HK_NULL; 
  if (fileContents)
  {
    pShape = static_cast<hkpShape*>(fileContents->stealContentsPointer(hkpShapeClass.getName(), hkBuiltinTypeRegistry::getInstance().getLoadedObjectRegistry()));
    fileContents->removeReference();
  }
  else
  {
    hkvLog::Error("vHavokShapeCache::Load of '%s' failed due to [%s]", szFilePath, err.defaultMessage.cString());
  }

  return pShape;
}

// -------------------------------------------------------------------------- //
// Terrain
// -------------------------------------------------------------------------- //

#if defined(SUPPORTS_TERRAIN)

bool vHavokShapeCache::SaveTerrainSectorShape(const VTerrainSector *pSector, const hkpShape *pShape)
{
  VASSERT(pSector != NULL && pShape != NULL);

  // Build the shape filename
  VStaticString<FS_MAX_PATH> szCachedShapeName; 
  if (!GetTerrainSectorShapePath(szCachedShapeName, pSector))
    return false;

  return SaveShape(szCachedShapeName, pShape);
}

hkpShape* vHavokShapeCache::LoadTerrainSectorShape(const VTerrainSector* pSector)
{
  VASSERT(pSector != NULL);

  // Build the cached shape filename
  VStaticString<FS_MAX_PATH> szCachedFilePath;
  if (!GetTerrainSectorShapePath(szCachedFilePath, pSector))
    return NULL;

  hkpShape* pShape = LoadShapeByFileName(szCachedFilePath);
  if (pShape == NULL)
  {
    // Retry with old file name format, which did not include the Havok2Vision scale.
    if (!GetTerrainSectorShapePath(szCachedFilePath, pSector, false))
      return NULL;

    pShape = LoadShapeByFileName(szCachedFilePath);
  }

  return pShape;
}

#endif

// -------------------------------------------------------------------------- //
// Helper functions
// -------------------------------------------------------------------------- //

bool vHavokShapeCache::GetShapePath(const char* szShapeId, VStaticString<FS_MAX_PATH>& szOut, bool bNative)
{
  if (VStringUtil::IsEmpty(szShapeId) || szShapeId[0] != '|')
  {
    // Shape isn't cached to disk.
    return false;
  }

  // Extract the mesh's / model's file name.
  szShapeId += 1; // Skip first character.
  const int iSeparatorIndex = VStringUtil::IndexOf(szShapeId, '|');
  if (iSeparatorIndex < 0)
  {
    hkvLog::Error("vHavokShapeCache::SaveShape: Couldn't parse shape ID for file name.");
    return false;
  }

  if (bNative)
  {
    // For native paths, we make sure that the path matches the actual location of the mesh's file.
    VStaticString<FS_MAX_PATH> szMeshPath;
    szMeshPath.Set(szShapeId, iSeparatorIndex); // Only look for the mesh in file system.

    VFileAccessManager::NativePathResult meshNativeResult;
    if (VFileAccessManager::GetInstance()->MakePathNative(szMeshPath, meshNativeResult, VFileSystemAccessMode::READ, VFileSystemElementType::FILE) != HKV_SUCCESS)
    {
      hkvLog::Error("vHavokShapeCache::SaveConvexShape: Failed to determine the native mesh path.");
      return false;
    }

    szOut = meshNativeResult.m_sNativePath;
  }
  else
  {
    szOut.Set(szShapeId, iSeparatorIndex);
  }
  
  szOut += "_data/";
  szOut += &szShapeId[iSeparatorIndex + 1];
  szOut += ".hkt";
  return true;
}

#if defined(SUPPORTS_TERRAIN)

bool vHavokShapeCache::GetTerrainSectorShapePath(VStaticString<FS_MAX_PATH>& szOut, const VTerrainSector* pSector, bool bIncludeHavok2VisionScale)
{
  VASSERT(pSector != NULL);

  // First get filename by specifying extension (hmap) to be able to retrieve the absolute path. 
  // Afterwards remove extension.
  pSector->m_Config.GetSectorFilename(szOut, pSector->m_iIndexX, pSector->m_iIndexY, "hmap", true);
  VFileAccessManager::NativePathResult sectorNativeResult;
  if (VFileAccessManager::GetInstance()->MakePathNative(szOut, sectorNativeResult, VFileSystemAccessMode::READ, VFileSystemElementType::FILE) != HKV_SUCCESS)
  {
    VASSERT_MSG(FALSE, "vHavokShapeCache::GetTerrainSectorShapePath: Failed to determine the native path to the sector hmap; the file may not exist!");
    return false;
  }

  // Remove the extension.
  const int iExtensionPos = VPathHelper::GetExtensionPos(sectorNativeResult.m_sNativePath);
  if (iExtensionPos >= 0)
    sectorNativeResult.m_sNativePath[iExtensionPos] = 0;

  // Tight-fit only needed for triangle shapes.
  if (bIncludeHavok2VisionScale)
  {
    if (pSector->GetPhysicsType() != VTerrainSector::VPHYSICSTYPE_APPROXIMATE && pSector->GetPhysicsTightFit())
    {
      szOut.Format("%s_%i_%i_%3g_t.hkt", sectorNativeResult.m_sNativePath.AsChar(), static_cast<int>(pSector->GetPhysicsType()), 
        pSector->HasHoles() ? 1 : 0, vHavokConversionUtils::GetHavok2VisionScale());
    }
    else
    {
      szOut.Format("%s_%i_%i_%3g.hkt", sectorNativeResult.m_sNativePath.AsChar(), static_cast<int>(pSector->GetPhysicsType()), 
        pSector->HasHoles() ? 1 : 0, vHavokConversionUtils::GetHavok2VisionScale());
    }
  }
  else
  {
    // Old file name format without Havok2VisionScale.
    if (pSector->GetPhysicsType() != VTerrainSector::VPHYSICSTYPE_APPROXIMATE && pSector->GetPhysicsTightFit())
    {
      szOut.Format("%s_%i_%i_t.hkt", sectorNativeResult.m_sNativePath.AsChar(), static_cast<int>(pSector->GetPhysicsType()), 
        pSector->HasHoles() ? 1 : 0);
    }
    else
    {
      szOut.Format("%s_%i_%i.hkt", sectorNativeResult.m_sNativePath.AsChar(), static_cast<int>(pSector->GetPhysicsType()), 
        pSector->HasHoles() ? 1 : 0);
    }
  }

  return true;
}

#endif

bool vHavokShapeCache::IsShapeUpToDate(const hkpShape* pShape, __int64 iFileTime)
{
  if (iFileTime != 0)
  {
    const __int64 iHktFileTime = GetHktFileTime(pShape);
    return (iFileTime == iHktFileTime);
  }

  return true;
}

__int64 vHavokShapeCache::GetHktFileTime(const hkpShape* pShape)
{
  // Get m_iFileTime member of custom Vision shape class via reflection
  const hkClass* pClassType = pShape->getClassType();
  if (pClassType == &hkvConvexVerticesShapeClass)
    return static_cast<const hkvConvexVerticesShape*>(pShape)->GetFileTime();
  else if (pClassType == &hkvBvCompressedMeshShapeClass)
    return static_cast<const hkvBvCompressedMeshShape*>(pShape)->GetFileTime();
  
  VASSERT_MSG(false, "vHavokShapeCache::GetHktFileTime: Cached shape does not support file time.");
  return 0;
}

//-----------------------------------------------------------------------------------
// User Data Helpers
// The lowest bit of the user data indicates the additional up-to-date flag.

inline void vHavokShapeCache::SetShapeId(hkpShape* pShape, const char* szShapeId)
{
  VASSERT_MSG((reinterpret_cast<hkUlong>(szShapeId) & 0x3) == 0, "Shape ID string is not 4-byte aligned.");
  pShape->setUserData(reinterpret_cast<hkUlong>(szShapeId));
}

inline const char* vHavokShapeCache::GetShapeId(const hkpShape* pShape)
{
  // The lower two bits are not used.
  return reinterpret_cast<const char*>(pShape->getUserData() & ~hkUlong(0x3));
}

// The up-to-date flag indicates if the shape is in sync with the cached version on the disk.
inline void vHavokShapeCache::SetUpToDateFlag(hkpShape* pShape)
{
  pShape->setUserData(pShape->getUserData() | 0x1);
}

inline bool vHavokShapeCache::GetUpToDateFlag(const hkpShape* pShape)
{
  return (pShape->getUserData() & 0x1) != 0;
}

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
