/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Tools/vPlayer/vPlayerPCH.h>
#include <Vision/Tools/vPlayer/vLoader.hpp>

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scene/VPrefab.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Components/VOrbitCamera.hpp>

#include <Vision/Runtime/Framework/VisionApp/VAppImpl.hpp>

#include <Vision/Runtime/Base/System/IO/FileSystems/VFileServeDaemon.hpp>

struct Helper
{
  static void FillAppLoadSettings(const VSceneListEntry& entry, VisAppLoadSettings& out_settings)
  {
    out_settings.m_sSceneName = entry.sScenePath;

    for (int i = 0; i < entry.sSearchPaths.GetSize(); ++i)
    {
      out_settings.m_customSearchPaths.Add(entry.sSearchPaths[i]);
    }

    out_settings.m_sWorkspaceRoot = entry.sRoot;
  }

  static void SetupScene(const VSceneListEntry& entry)
  {
    VisAppLoadSettings settings;
    Helper::FillAppLoadSettings(entry, settings);
    VAppBase::Get()->GetAppImpl()->SetupScene(settings);

    // Clear scene / reinit world only after the manifest file has been processed.
    VSceneLoader::ClearScene();

    { // Trigger callback OnBeforeSceneLoaded
      VisSceneLoadedDataObject_cl data(&Vision::Callbacks.OnBeforeSceneLoaded, entry.sScenePath);
      Vision::Callbacks.OnBeforeSceneLoaded.TriggerCallbacks(&data);
    }
  }

  static void LookAtBox(const hkvAlignedBBox &bbox)
  {
    if (!bbox.isValid())
      return;

    VisBaseEntity_cl* pCenterEntity = Vision::Game.CreateEntity("VisBaseEntity_cl", bbox.getCenter());

    const float fBBRadius = bbox.getBoundingSphere().m_fRadius;

    float fFovX, fFovY;
    Vision::Contexts.GetMainRenderContext()->GetFinalFOV(fFovX, fFovY);
    
    const float fDist = fBBRadius / hkvMath::tanDeg(hkvMath::Min(fFovX, fFovY) * 0.5f);

    VOrbitCamera* pOrbitCamera = new VOrbitCamera();
    pOrbitCamera->MinimalDistance = fDist * 0.01f;
    pOrbitCamera->MaximalDistance = fDist * 100.0f;
    pOrbitCamera->CameraDistance = fDist;

    pCenterEntity->AddComponent(pOrbitCamera);
  }
};

//////////////////////////////////////////////////////////////////////////

bool SceneLoader::Load(const VSceneListEntry& entry)
{
  m_sceneListEntry = entry;

  VisAppLoadSettings settings;
  Helper::FillAppLoadSettings(entry, settings);
  return VAppBase::Get()->GetAppImpl()->LoadScene(settings);
}

VString SceneLoader::GetLastError() const
{
  VSceneLoader& loader = VAppBase::Get()->GetAppImpl()->GetSceneLoader();

  // If the scene wasn't found, attempt to figure out why and display something useful to the user. Do this
  // here (rather than at a lower level) because the vPlayer knows that all search paths _should_ be there - usually
  // search path don't have to exist, so it's difficult to generally find out WHY a file wasn't found.
  if(loader.IsNotFound())
  {
    if(m_sceneListEntry.sSearchPaths.GetLength() == 0)
    {
      return "The list of search paths is empty";
    }

    // Check all search paths - this is the most likely source when the scene wasn't found
    for(int i = 0; i < m_sceneListEntry.sSearchPaths.GetLength(); i++)
    {
      const VString& sSearchPath = m_sceneListEntry.sSearchPaths[i];

      VStaticString<VFileAccessManager::MAX_ROOT_NAME_LENGTH> sSearchPathRoot;
      VFileAccessManager::SplitOffRoot(sSearchPath, sSearchPathRoot);

      if(VFileAccessManager::GetInstance()->GetRoot(sSearchPathRoot) == NULL)
      {
        VString error;
        error.Format("The root named '%s' is not mounted.", sSearchPathRoot.AsChar());
        return error;
      }

      VStaticString<VFileAccessManager::MAX_ROOT_NAME_LENGTH> sSearchPathTopLevelDir;
      sSearchPathTopLevelDir.Format(":%s/", sSearchPathRoot.AsChar());

      if(!VFileAccessManager::GetInstance()->DirectoryExists(sSearchPathTopLevelDir))
      {
        VString error;
        if(VFileServeDaemon::IsInitialized())
        {
          if(VFileServeDaemon::GetInstance()->IsConnected())
          {
            error.Format("The root named '%s' is not accessible. Is this root mapped in vFileServe?", sSearchPathRoot.AsChar());
          }
          else
          {
            error.Format("The root named '%s' is not accessible. The scene does not seem to be in the cache.", sSearchPathRoot.AsChar());
          }
        }
        else
        {
          error.Format("The root named '%s' is not accessible.", sSearchPathRoot.AsChar());
        }
        return error;
      }

      if(!VFileAccessManager::GetInstance()->DirectoryExists(sSearchPath))
      {
        VString error;
        if(VFileServeDaemon::IsInitialized())
        {
          if(VFileServeDaemon::GetInstance()->IsConnected())
          {
            error.Format("The search path '%s' could not be found. Is vFileServe configured correctly?", sSearchPath.AsChar());
          }
          else
          {
            error.Format("The search path '%s' could not be found. The scene does not seem to be in the cache.", sSearchPath.AsChar());
          }
        }
        else
        {
          error.Format("The search path '%s' could not be found.", sSearchPath.AsChar());
        }
        return error;
      }
    }

    // All search paths are okay, but the scene was not in any of them. It should usually be found in the first (= project dir), so display that one
    VString error;
    error.Format("The scene file was not found in '%s' or any other search directory.", m_sceneListEntry.sSearchPaths[0].AsChar());
    return error;
  }
  
  return loader.GetLastError();
}

//////////////////////////////////////////////////////////////////////////

bool MeshLoader::Load(const VSceneListEntry& entry)
{
  Helper::SetupScene(entry);

  // load vmesh
  VisStaticMesh_cl* pRes = VisStaticMesh_cl::GetResourceManager().LoadStaticMeshFile(entry.sScenePath);
  if (pRes == NULL) 
    return false;
  
  // force loading of .colmesh file if it exists
  if (pRes->GetCollisionMesh(true)) 
    hkvLog::Info("Loaded collision mesh");
  else
    hkvLog::Info("No collision mesh loaded");

  // get a instance
  hkvMat4 mTransform;
  VisStaticMeshInstance_cl *pInst = pRes->CreateInstance(mTransform);

  // create the visibility zone
  const hkvAlignedBBox& bbox = pInst->GetBoundingBox();
  int iZones = Vision::GetSceneManager()->GetNumVisibilityZones();
  VisVisibilityZone_cl* pVisZone;
  if (iZones > 0)
  {
    pVisZone = Vision::GetSceneManager()->GetVisibilityZone(0);
  } 
  else
  {
    pVisZone = new VisVisibilityZone_cl(bbox);
    Vision::GetSceneManager()->AddVisibilityZone(pVisZone);
  }

  // add submeshes to vis zone
  for (int i = 0; i < pInst->GetSubmeshInstanceCount(); ++i)
  {
    pVisZone->AddStaticGeometryInstance(pInst->GetSubmeshInstance(i));
  }

  Helper::LookAtBox(bbox);
  return true;
}

VString MeshLoader::GetLastError() const
{
  return "";
}

//////////////////////////////////////////////////////////////////////////

bool PrefabLoader::Load(const VSceneListEntry& entry)
{
  Helper::SetupScene(entry);

  float fLarge = 100000000.f;
  hkvAlignedBBox large(hkvVec3(-fLarge), hkvVec3(fLarge));
  VisVisibilityZone_cl* pVisZone = new VisVisibilityZone_cl(large);
  Vision::GetSceneManager()->AddVisibilityZone(pVisZone);

  // load the prefab file and instantiate once
  VPrefab* pPrefab = VPrefabManager::GlobalManager().LoadPrefab(entry.sScenePath);
  if (pPrefab == NULL)
    return false;

  VPrefabInstanceInfo info;
  if (!pPrefab->Instantiate(info))
    hkvLog::Info("Failed to instantiate prefab");

  hkvAlignedBBox scenebox;
  Vision::GetSceneManager()->GetDynamicSceneExtents(scenebox);
  Helper::LookAtBox(scenebox);

  return true;
}

VString PrefabLoader::GetLastError() const
{
  return "";
}

//////////////////////////////////////////////////////////////////////////

bool ZoneLoader::Load(const VSceneListEntry& entry)
{
  Helper::SetupScene(entry);

  float fLarge = 100000000.f;
  hkvAlignedBBox large(hkvVec3(-fLarge), hkvVec3(fLarge));
  VisVisibilityZone_cl* pVisZone = new VisVisibilityZone_cl(large);
  Vision::GetSceneManager()->AddVisibilityZone(pVisZone);

  VisZoneResource_cl* pZone = VisZoneResourceManager_cl::GlobalManager().CreateZone(entry.sScenePath, large);
  if (pZone == NULL)
    return false;

  pZone->EnsureLoaded();
  pZone->m_bHandleZone = false; // don't uncache

  hkvAlignedBBox scenebox;
  Vision::GetSceneManager()->GetDynamicSceneExtents(scenebox);
  Helper::LookAtBox(scenebox);

  return true;
}

VString ZoneLoader::GetLastError() const
{
  return "";
}

//////////////////////////////////////////////////////////////////////////

bool LitLoader::Load(const VSceneListEntry& entry)
{
  Helper::SetupScene(entry);

  VLightmapSceneInfo meshes;
  VLightmapSceneInfo lit;
  if (!lit.LoadOutputFile(entry.sScenePath, &Vision::TextureManager.GetManager()))
    return false;

  char szMeshFile[FS_MAX_PATH];
  VFileHelper::AddExtension(szMeshFile, entry.sScenePath, "temp");
  if (!meshes.LoadMeshFile(szMeshFile))
    return false;

  VisionAppHelpers::CreateLightmapMeshes(meshes,&lit);

  hkvAlignedBBox scenebox;
  Vision::GetSceneManager()->GetDynamicSceneExtents(scenebox);
  Helper::LookAtBox(scenebox);

  return true;
}

VString LitLoader::GetLastError() const
{
  return "";
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
