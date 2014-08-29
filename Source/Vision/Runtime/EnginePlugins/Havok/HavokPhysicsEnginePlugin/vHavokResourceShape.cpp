/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokResourceShape.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokFileStreamAccess.hpp>
#include <Vision/Runtime/Engine/System/VisApiSerialization.hpp>

#include <Common/Serialize/Util/hkRootLevelContainer.h>
#include <Physics2012/Dynamics/World/hkpWorld.h>

V_IMPLEMENT_SERIAL(vHavokResourceShape,VisTypedEngineObject_cl,0,&g_vHavokModule);
V_IMPLEMENT_DYNCREATE(vHavokResourceLoadingTask, VThreadedTask, &g_vHavokModule);


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class vHavokResourceShape::VResourceLoadingTask
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void vHavokResourceLoadingTask::Run(VManagedThread *pThread)
{
  bool bAsynchronously = pThread!=NULL;
  {
    vHavokStreamReader reader(m_pStream); // m_pStream is closed at the end of this scope
    hkBufferedStreamReader seekable(&reader);
    hkSerializeUtil::ErrorDetails errRes;

    m_pShape->m_spHavokResource = hkRefNew<hkObjectResource>(hkSerializeUtil::loadOnHeap(&seekable, &errRes));
  }
  m_pShape->m_bIsLoaded = m_pShape->m_spHavokResource!=NULL;

  if (m_bInstantiate && m_pShape->m_spHavokResource!=NULL)
  {
    vHavokPhysicsModule::GetInstance()->OnResourceShapeLoaded(m_pShape);
  }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class vHavokResourceShape
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


vHavokResourceShape::vHavokResourceShape()
{
  m_bLoadAsynchronously = true;
  m_bActiveAtStartup = true;
  m_bIsLoaded = m_bInstantiated = m_bQueued = false;

  vHavokPhysicsModule::GetInstance()->m_ResourceShapes.Add(this);
}

vHavokResourceShape::~vHavokResourceShape()
{
  Unload();
}

void vHavokResourceShape::DisposeObject()
{
  VisTypedEngineObject_cl::DisposeObject();
  Unload();

  vHavokPhysicsModule::GetInstance()->m_ResourceShapes.SafeRemove(this);
}

void vHavokResourceShape::MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB)
{
  VisTypedEngineObject_cl::MessageFunction(iID, iParamA, iParamB);
  if (iID==VIS_MSG_ACTIVATE)
  {
    EnsureTasksFinished();
    m_bActiveAtStartup = iParamA!=0;

    // remove from queue if it is in there
    if (m_bQueued)
    {
      if (!m_bActiveAtStartup)
        vHavokPhysicsModule::GetInstance()->OnResourceShapeDestroyed(this);
    } 
    else if (m_spHavokResource!=NULL && m_bActiveAtStartup!=m_bInstantiated)
    {
      m_bInstantiated = m_bActiveAtStartup;

      // add or remove instances
      vHavokResourceCallbackData cbdata(m_spHavokResource, m_bInstantiated ? vHavokResourceCallbackData::HRA_CREATE_INSTANCES : vHavokResourceCallbackData::HRA_DELETE_INSTANCES);
      vHavokPhysicsModule::GetInstance()->TriggerHavokResourceCallback(cbdata);
    }
  }
}

bool vHavokResourceShape::LoadFromFile(const char *szFilename, bool bInstantiate)
{
  Unload();
  m_sFilename = szFilename;
  IVFileInStream* pIn = Vision::File.Open(szFilename);
  if (pIn==NULL)
    return false;

  m_LoadingTask.m_pStream = pIn;
  m_LoadingTask.m_pShape = this;
  m_LoadingTask.m_bInstantiate = bInstantiate;

  if (m_bLoadAsynchronously && !Vision::Editor.IsInEditor())
  {
    // load asynchronously
    Vision::GetThreadManager()->ScheduleTask(&m_LoadingTask);
    return pIn!=NULL; // can't check for m_bIsLoaded
  } else
  {
    m_LoadingTask.Run(NULL);
  }


  return m_bIsLoaded;
}

void vHavokResourceShape::Unload()
{
  EnsureTasksFinished();

  if (m_bQueued) // it is still hanging in the queue of the manager?
  {
    vHavokPhysicsModule::GetInstance()->OnResourceShapeDestroyed(this);
  }
  if (m_bInstantiated)
  {
    vHavokResourceCallbackData cbdata(m_spHavokResource, vHavokResourceCallbackData::HRA_DELETE_INSTANCES);
    vHavokPhysicsModule::GetInstance()->TriggerHavokResourceCallback(cbdata);
    m_bInstantiated  = false;
  }

  m_bIsLoaded = false;
  m_spHavokResource = NULL;
  m_BoundingBox.setInvalid();
}

void vHavokResourceShape::Serialize(VArchive& ar )
{
  VisTypedEngineObject_cl::Serialize(ar);
  unsigned char iLocalVersion = 1;
  if (ar.IsLoading())
  {
    ar >> iLocalVersion; VASSERT_MSG(iLocalVersion<=1, "Invalid version number. Please re-export.");
    ar >> m_bLoadAsynchronously;
    if (iLocalVersion>=1)
      ar >> m_bActiveAtStartup;
    ar >> m_sFilename;
  } else
  {
    EnsureTasksFinished();
    ar << iLocalVersion;
    ar << m_bLoadAsynchronously << m_bActiveAtStartup;
    ar << m_sFilename;
  }
}

void vHavokResourceShape::OnSerialized(VArchive& ar)
{
  VisTypedEngineObject_cl::OnSerialized(ar);
  if (ar.IsLoading())
    LoadFromFile(m_sFilename, m_bActiveAtStartup);
}

 #ifdef SUPPORTS_SNAPSHOT_CREATION
void vHavokResourceShape::GetDependencies(VResourceSnapshot &snapshot)
{
  EnsureTasksFinished();
  VisTypedEngineObject_cl::GetDependencies(snapshot);
  if (m_sFilename.IsEmpty())
    return;
  IVFileInStream *pIn = Vision::File.Open(m_sFilename);
  if (pIn==NULL)
    return;
  snapshot.AddFileDependency(NULL, m_sFilename, pIn->GetSize());
  pIn->Close();
}

#endif

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
