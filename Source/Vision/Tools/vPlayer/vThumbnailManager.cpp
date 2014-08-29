/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Tools/vPlayer/vPlayerPCH.h>
#include <Vision/Tools/vPlayer/vThumbnailManager.hpp>

struct PendingThumbnail
{
  ISceneListDataProvider* pDataProvider;
  VSceneListEntry entry;

  VString sFileName;

  VLoadingTaskPtr spTask;
};

static VArray<PendingThumbnail> s_PendingTasks;

//////////////////////////////////////////////////////////////////////////

static VTextureObjectPtr s_spDefaultThumbnail;
static VTextureObjectPtr s_spLoadingThumbnail;

void VThumbnailManager::Init()
{
  s_spDefaultThumbnail = Vision::TextureManager.Load2DTexture(":havok_sdk/Data/Vision/Tools/vPlayer/default_thumbnail.tga", 
    VTM_FLAG_DEFAULT_NON_MIPMAPPED|VTM_FLAG_NO_DOWNSCALE);

  s_spLoadingThumbnail = Vision::TextureManager.Load2DTexture(":havok_sdk/Data/Vision/Tools/vPlayer/ThumbnailLoading.tga", 
    VTM_FLAG_DEFAULT_NON_MIPMAPPED|VTM_FLAG_NO_DOWNSCALE);
}

void VThumbnailManager::DeInit()
{
  if (s_PendingTasks.GetSize() > 0)
  {
    VLoadingTask* pTask = s_PendingTasks[0].spTask;
    Vision::GetThreadManager()->WaitForTask(pTask, true);
  }

  s_PendingTasks.Reset();

  s_spDefaultThumbnail = NULL;
  s_spLoadingThumbnail = NULL;
}


void VThumbnailManager::LoadThumbnail(ISceneListDataProvider* pDataProvider, int iEntryIndex)
{
  VSceneListEntry& entry = pDataProvider->GetAt(iEntryIndex);
  entry.spThumbnail = s_spLoadingThumbnail;

  // search for thumbnail, only test the first search path
  char szFullPath[FS_MAX_PATH];
  char szThumbnailFileName[FS_MAX_PATH];

  VFileHelper::CombineDirAndFile(szFullPath, entry.sSearchPaths[0], entry.sScenePath);
  VFileHelper::ReplaceExtension(szThumbnailFileName, szFullPath, "tga");

  if (VTextureObject* pThumbnail = static_cast<VTextureObject*>(VTextureManager::GlobalManager().GetResourceByName(szThumbnailFileName)))
  {
    entry.spThumbnail = pThumbnail;
  }
  else
  {
    PendingThumbnail pendingThumbnail;
    pendingThumbnail.pDataProvider = pDataProvider;
    pendingThumbnail.entry = entry;
    pendingThumbnail.sFileName = szThumbnailFileName;
    pendingThumbnail.spTask = Vision::File.GetMemoryStreamManager().PrecacheFile(szThumbnailFileName);

    s_PendingTasks.Add(pendingThumbnail);
  }
  
}

void VThumbnailManager::ProcessPendingThumbnails()
{
  if (s_PendingTasks.GetSize() == 0)
    return;

  PendingThumbnail& pendingThumbnail = s_PendingTasks[0];
  VLoadingTask* pTask = pendingThumbnail.spTask;

  if (pTask->GetState() == TASKSTATE_UNASSIGNED)
  {
    pTask->ScheduleLoadingTask();
  }
  else if (pTask->GetState() == TASKSTATE_FINISHED)
  {
    // entry might have been moved or deleted in the meantime
    int iEntryIndex = pendingThumbnail.pDataProvider->Find(pendingThumbnail.entry);
         
    if (iEntryIndex >= 0)
    {
      VSceneListEntry& entry = pendingThumbnail.pDataProvider->GetAt(iEntryIndex);

      if (pTask->IsValid())
      {
        VASSERT(pTask->IsLoaded());

        entry.spThumbnail = Vision::TextureManager.Load2DTexture(pendingThumbnail.sFileName, 
          VTM_FLAG_DEFAULT_NON_MIPMAPPED|VTM_FLAG_NO_DOWNSCALE);
      }
      else
      {
        entry.spThumbnail = s_spDefaultThumbnail;
      }

      VSceneListEvent data(&pendingThumbnail.pDataProvider->m_OnDataChanged, -1, iEntryIndex, VSceneListEvent::DATA_CHANGED);
      data.Trigger();
    }

    s_PendingTasks.RemoveAt(0);
  }
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
