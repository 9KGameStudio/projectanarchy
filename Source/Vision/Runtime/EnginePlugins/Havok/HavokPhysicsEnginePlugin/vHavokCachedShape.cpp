/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokCachedShape.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokFileStreamAccess.hpp>     
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>

#include <Common/Base/Config/hkConfigVersion.h>
#include <Common/Base/System/Io/Writer/Buffered/hkBufferedStreamWriter.h>
#include <Common/Base/System/Io/Reader/Buffered/hkBufferedStreamReader.h>
#include <Common/Base/Reflection/hkClassMemberAccessor.h>
#include <Common/Serialize/Util/hkSerializeUtil.h>
#include <Common/Base/Reflection/Registry/hkVtableClassRegistry.h>

// May also work in 2011.3, but if you save 2011.3 bvtreeshape they will not version up to 2012.1 anyway
// You can set this back to 2011.3 if you really want to use that version 
#if HAVOK_SDK_VERSION_NUMBER >= 20120100 
#define SUPPORTS_HKT
#endif

#if defined(HK_DEBUG_SLOW) || defined(HK_DEBUG)
  bool vHavokCachedShape::s_bCheckHktUpToDate = true;
#else
  bool vHavokCachedShape::s_bCheckHktUpToDate = false;
#endif


// -------------------------------------------------------------------------- //
// Saving 
// -------------------------------------------------------------------------- //

bool vHavokCachedShape::SaveConvexShape(const VBaseMesh *pMesh, const hkvVec3& vScale, bool bShrinkToFit, const hkvConvexVerticesShape *pShape)
{
#ifndef SUPPORTS_HKT
  return false; 
#else
  VASSERT(pMesh!=NULL && pShape!=NULL);

  VFileAccessManager::NativePathResult meshNativeResult;
  if (VFileAccessManager::GetInstance()->MakePathNative(pMesh->GetFilename(), meshNativeResult, VFileSystemAccessMode::READ, VFileSystemElementType::FILE) != HKV_SUCCESS)
  {
    VASSERT_MSG(FALSE, "vHavokCachedShape::SaveConvexShape: Failed to determine the native mesh path; the file may not exist!");
    return false;
  }

  VStaticString<FS_MAX_PATH> szCachedShapeDirname(meshNativeResult.m_sNativePath);
  szCachedShapeDirname += "_data";
  if (!VFileHelper::ExistsDir(szCachedShapeDirname))
    VFileHelper::MkDir(szCachedShapeDirname);

  // Build the shape filename
  VStaticString<FS_MAX_PATH> szCachedShapeName(meshNativeResult.m_sNativePath);
  GetConvexShapePath(szCachedShapeName, vScale, bShrinkToFit);

  return SaveShape(szCachedShapeName, hkvConvexVerticesShapeClass, pShape);
#endif
}

bool vHavokCachedShape::SaveMeshShape(const VBaseMesh *pMesh, const hkvVec3& vScale, VisStaticMeshInstance_cl::VisCollisionBehavior_e eCollisionBehavior, 
                                      VisWeldingType_e eWeldingType, const hkvBvCompressedMeshShape *pShape)
{
#ifndef SUPPORTS_HKT
  return false; 
#else
  VASSERT(pMesh!=NULL && pShape!=NULL);

  VFileAccessManager::NativePathResult meshNativeResult;
  if (VFileAccessManager::GetInstance()->MakePathNative(pMesh->GetFilename(), meshNativeResult, VFileSystemAccessMode::READ, VFileSystemElementType::FILE) != HKV_SUCCESS)
  {
    VASSERT_MSG(FALSE, "vHavokCachedShape::SaveConvexShape: Failed to determine the native mesh path; the file may not exist!");
    return false;
  }

  VStaticString<FS_MAX_PATH> szCachedShapeDirname(meshNativeResult.m_sNativePath);
  szCachedShapeDirname += "_data";
  if (!VFileHelper::ExistsDir(szCachedShapeDirname))
    VFileHelper::MkDir(szCachedShapeDirname);

  // Build the shape filename
  VStaticString<FS_MAX_PATH> szCachedShapeName(meshNativeResult.m_sNativePath);
  GetMeshShapePath(szCachedShapeName, vScale, eCollisionBehavior, eWeldingType);

  return SaveShape(szCachedShapeName, hkvBvCompressedMeshShapeClass, pShape);
#endif
}

#ifdef SUPPORTS_TERRAIN
bool vHavokCachedShape::SaveTerrainSectorShape(const VTerrainSector *pSector, const hkpShape *pShape)
{
#ifndef SUPPORTS_HKT
  return false; 
#else
  VASSERT(pSector!=NULL && pShape!=NULL);

  // First get filename by specifying extension (hmap) to be able to retrieve the absolute path. 
  // Afterwards remove extension.
  char szFilename[FS_MAX_PATH];
  pSector->m_Config.GetSectorFilename(szFilename, pSector->m_iIndexX, pSector->m_iIndexY, "hmap", true);
  VFileAccessManager::NativePathResult sectorNativeResult;
  if (VFileAccessManager::GetInstance()->MakePathNative(szFilename, sectorNativeResult, VFileSystemAccessMode::READ, VFileSystemElementType::FILE) != HKV_SUCCESS)
  {
    VASSERT_MSG(FALSE, "vHavokShapeFactory::GetHktDependencies: Failed to determine the native path to the sector hmap; the file may not exist!");
    return false;
  }
  VFileHelper::GetFilenameNoExt(szFilename, sectorNativeResult.m_sNativePath);

  // Build the shape filename
  VStaticString<FS_MAX_PATH> szCachedShapeName(szFilename); 
  GetTerrainSectorShapePath(szCachedShapeName, pSector->GetPhysicsType(), pSector->HasHoles());

  const hkClass* pClassType = hkVtableClassRegistry::getInstance().getClassFromVirtualInstance(pShape);

  return SaveShape(szCachedShapeName, *pClassType, pShape);
#endif
}
#endif


// -------------------------------------------------------------------------- //
// Loading
// -------------------------------------------------------------------------- //

hkvConvexVerticesShape* vHavokCachedShape::LoadConvexShape(VBaseMesh *pMesh, const hkvVec3& vScale, bool bShrinkToFit)
{
#ifndef SUPPORTS_HKT
  return HK_NULL;
#else
  VASSERT(pMesh != NULL);

  // Build the cached shape filename
  VStaticString<FS_MAX_PATH> szCachedShapeName(pMesh->GetFilename());
  GetConvexShapePath(szCachedShapeName, vScale, bShrinkToFit);

  hkvConvexVerticesShape *pShape = (hkvConvexVerticesShape*)LoadShape(szCachedShapeName, hkvConvexVerticesShapeClass);
  if ((Vision::Editor.IsInEditor() || IsHktUpToDateCheckingEnabled()) && pShape!=HK_NULL)
  {
    if (!IsHktUpToDate(pMesh, pShape, hkvConvexVerticesShapeClass))
      return HK_NULL;
  }

  return pShape;
#endif
}

hkvBvCompressedMeshShape* vHavokCachedShape::LoadMeshShape(VBaseMesh *pMesh, const hkvVec3& vScale, VisStaticMeshInstance_cl::VisCollisionBehavior_e eCollisionBehavior, 
                                           VisWeldingType_e eWeldingType)
{
#ifndef SUPPORTS_HKT
  return HK_NULL;
#else
  VASSERT(pMesh != NULL);

  // Build the cached shape filename
  VStaticString<FS_MAX_PATH> szCachedShapeName(pMesh->GetFilename());
  GetMeshShapePath(szCachedShapeName, vScale, eCollisionBehavior, eWeldingType);

  hkvBvCompressedMeshShape *pShape = (hkvBvCompressedMeshShape*)LoadShape(szCachedShapeName, hkvBvCompressedMeshShapeClass);
  if ((Vision::Editor.IsInEditor() || IsHktUpToDateCheckingEnabled()) && pShape!=HK_NULL)
  {
    if (!IsHktUpToDate(pMesh, pShape, hkvBvCompressedMeshShapeClass))
      return HK_NULL;
  }

  return pShape;
#endif
}

#ifdef SUPPORTS_TERRAIN
hkpShape* vHavokCachedShape::LoadTerrainSectorShape(const VTerrainSector *pSector)
{
#ifndef SUPPORTS_HKT
  return HK_NULL;
#else
  VASSERT(pSector != NULL);

  const VTerrainSector::VPhysicsType_e ePhysicsType = pSector->GetPhysicsType();
  const bool bHasHoles = pSector->HasHoles();

  // First get filename by specifying extension (hmap) to be able to retrieve the absolute path. 
  // Afterwards remove extension.
  char szFilename[FS_MAX_PATH];
  pSector->m_Config.GetSectorFilename(szFilename, pSector->m_iIndexX, pSector->m_iIndexY, "hmap", true);
  VFileAccessManager::NativePathResult sectorNativeResult;
  if (VFileAccessManager::GetInstance()->MakePathNative(szFilename, sectorNativeResult, VFileSystemAccessMode::READ, VFileSystemElementType::FILE) != HKV_SUCCESS)
  {
    VASSERT_MSG(FALSE, "vHavokShapeFactory::GetHktDependencies: Failed to determine the native path to the sector hmap; the file may not exist!");
    return NULL;
  }
  VFileHelper::GetFilenameNoExt(szFilename, sectorNativeResult.m_sNativePath);

  // Build the cached shape filename
  VStaticString<FS_MAX_PATH> szCachedShapeName(szFilename);
  GetTerrainSectorShapePath(szCachedShapeName, ePhysicsType, bHasHoles);

  if (ePhysicsType == VTerrainSector::VPHYSICSTYPE_APPROXIMATE)
    return LoadShape(szCachedShapeName, hkvSampledHeightFieldShapeClass);
  else if (bHasHoles)
    return LoadShape(szCachedShapeName, hkvTriSampledHeightFieldBvTreeShapeClass);
  else
    return LoadShape(szCachedShapeName, hkpTriSampledHeightFieldBvTreeShapeClass);
#endif
}
#endif


// -------------------------------------------------------------------------- //
// Helper functions
// -------------------------------------------------------------------------- //

void vHavokCachedShape::GetConvexShapePath(VStaticString<FS_MAX_PATH>& szInOut, const hkvVec3& vScale, bool bShrinkToFit)
{
  // Use Havok instead of Vision scale to account for global HavokToVision scale.
  const hkvVec3 vHavokScale = vScale * vHavokConversionUtils::GetVision2HavokScale();

  // The max. filename length on Xbox360 is 40 chars. The worst case should be here 37 chars.
  char tempString[64];
  if (bShrinkToFit)
    sprintf(tempString, "C_%.3g_%.3g_%.3g_t", vHavokScale.x, vHavokScale.y, vHavokScale.z);
  else
    sprintf(tempString, "C_%.3g_%.3g_%.3g", vHavokScale.x, vHavokScale.y, vHavokScale.z);

  szInOut += "_data";
  szInOut += VFILE_STR_SEPARATOR;
  szInOut += tempString;
  szInOut += ".hkt";
}

void vHavokCachedShape::GetMeshShapePath(VStaticString<FS_MAX_PATH>& szInOut, const hkvVec3& vScale, VisStaticMeshInstance_cl::VisCollisionBehavior_e eCollisionBehavior, 
                                         VisWeldingType_e eWeldingType)
{
  // Use Havok instead of Vision scale to account for global HavokToVision scale.
  const hkvVec3 vHavokScale = vScale * vHavokConversionUtils::GetVision2HavokScale();

  // The max. filename length on Xbox360 is 40 chars. The worst case should be here 39 chars.
  char tempString[64];
  sprintf(tempString, "M_%.3g_%.3g_%.3g_%i_%i", vHavokScale.x, vHavokScale.y, vHavokScale.z, (int)eCollisionBehavior, (int)eWeldingType);

  szInOut += "_data";
  szInOut += VFILE_STR_SEPARATOR;
  szInOut += tempString;
  szInOut += ".hkt";
}

#ifdef SUPPORTS_TERRAIN
void vHavokCachedShape::GetTerrainSectorShapePath(VStaticString<FS_MAX_PATH>& szInOut, VTerrainSector::VPhysicsType_e ePhysicsType, bool bHasHoles)
{
  char tempString[64];
  sprintf(tempString, "_%i_%i", (int)ePhysicsType, bHasHoles ? 1 : 0);
  szInOut += tempString;
  szInOut += ".hkt";
}
#endif

bool vHavokCachedShape::SaveShape(const char *szCachedShapeName, const hkClass& expectedClass, const hkpShape *pShape)
{
  VASSERT(szCachedShapeName!=NULL && pShape!=NULL);

  // make sure it's editable
  VRCSHelper::RCSEditFile(szCachedShapeName);

  // save shape to file
  {
    vHavokStreamWriter writer(szCachedShapeName);
    hkBufferedStreamWriter seekable(&writer);
    hkSerializeUtil::ErrorDetails err;
    hkResult res = hkSerializeUtil::save((const void*)pShape, expectedClass, &seekable); 

    if (res == HK_FAILURE)
    {
      hkvLog::Warning("vHavokCachedShape::Save of %s failed", szCachedShapeName);
      return false;
    }

    // make sure the file write is closed, before trying to add it to P4
  }

  // make sure it's added
  VRCSHelper::RCSAddFile(szCachedShapeName, true /* Binary file */);

  return true;
}

hkpShape* vHavokCachedShape::LoadShape(const char *szCachedShapeName, const hkClass &expectedClass)
{
  VASSERT(szCachedShapeName != NULL);
  if(!Vision::File.Exists(szCachedShapeName))
    return HK_NULL;

  // load cached shape from file
  hkpShape *pShape = HK_NULL; 
  vHavokStreamReader reader(szCachedShapeName);
 
  hkBufferedStreamReader seekable(&reader);
  hkSerializeUtil::ErrorDetails err;

#if 0 // Enforce only exact version number here if you want to be sure to get all latest compression fixes in shapes
  hkSerializeUtil::FormatDetails formatDetails;
  hkSerializeUtil::detectFormat( &seekable, formatDetails);
  if ( formatDetails.m_version.cString() && (hkString::strCmp( formatDetails.m_version.cString(), HAVOK_SDK_VERSION_NUM_STRING ) != 0 ))
  {
    hkStringBuf fullWarning( "vHavokCachedShape::Load of ", szCachedShapeName, " failed due to non exact Havok SDK version:");
    fullWarning.appendPrintf("(%s in file, %s runtime)",formatDetails.m_version.cString(), HAVOK_SDK_VERSION_NUMBER );
    hkvLog::Warning( fullWarning.cString() );
    return HK_NULL;                    
  }
#endif

  // Note we are on purpose failing on versioning as any issues in bvtreecompressed shape in the compression etc are not patched (also patch may be slower than just create again). So best to fail and cause an implicit resave.
  hkObjectResource* fileContents = hkSerializeUtil::loadOnHeap(&seekable, &err, hkSerializeUtil::LOAD_FAIL_IF_VERSIONING); 
  if (fileContents)
  {
    pShape = (hkpShape*)fileContents->stealContentsPointer(expectedClass.getName(), hkBuiltinTypeRegistry::getInstance().getLoadedObjectRegistry() );
    fileContents->removeReference();
  }
  else
    hkvLog::Warning("vHavokCachedShape::Load of %s failed due to [%s]", szCachedShapeName, err.defaultMessage.cString());
  
  return pShape;
}

bool vHavokCachedShape::IsHktUpToDate(VBaseMesh *pMesh, hkpShape *pShape, const hkClass &expectedClass)
{
  VASSERT(pMesh!=NULL && pShape!=NULL);

  const vHavokPhysicsModule *pModule = vHavokPhysicsModule::GetInstance();
  VASSERT(pModule != NULL);
  const bool bForceHktShapeCaching = pModule->IsHktShapeCachingEnforced();

  // Get the collision mesh 
  const IVCollisionMesh *pColMesh = pMesh->GetCollisionMesh(true, true);
  VASSERT(pColMesh != NULL);

  // Get m_iFileTime member of custom Vision shape class via reflection
  hkClassMemberAccessor accessor(pShape, expectedClass, "iFileTime");
  VASSERT(accessor.isOk());
  hkInt64 iFileTime = accessor.asInt64();

  // Check whether cached file is still up to date
  if (iFileTime != pColMesh->GetFileTime())
  {
    if (!Vision::Editor.IsInEditor() && !bForceHktShapeCaching)
      hkvLog::Warning("vHavokCachedShape::Load for %s failed since HKT file is outdated. Please re-generate HKT file (see documentation for details).", pMesh->GetFilename());
    return false;
  }
  
  return true;
}

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
