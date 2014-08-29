/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vHavokStaticMesh.hpp

#ifndef VHAVOKSYNC_HPP_INCLUDED
#define VHAVOKSYNC_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsIncludes.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>

#include <Common/Base/System/hkBaseSystem.h>
#include <Common/Base/System/Io/FileSystem/hkFileSystem.h>
#include <Common/Base/DebugUtil/MultiThreadCheck/hkMultiThreadCheck.h>
#include <Common/Base/Monitor/hkMonitorStream.h>
#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>

#include <Common/Base/Config/hkConfigVersion.h>

#ifndef _VISION_DOC

#if (defined(_WIN32) || defined(_VISION_XENON)) && ( !defined(VBASE_LIB) && !defined(LINK_WITH_VISION) )
#undef HAVOK_NEED_SYNC_MACROS
#define HAVOK_NEED_SYNC_MACROS
#endif

#if defined(HAVOK_NEED_SYNC_MACROS)

void VHAVOK_IMPEXP vHavokPhysicsModuleGetSyncInfo(hkMemoryInitUtil::SyncInfo& s);

#if !defined(__vHavokPhysicsModuleIncludes)

// A listener for Havok module callbacks
class vHavokSyncStaticsCallbackHandler : public IVisCallbackHandler_cl
{
  class VSyncLocalThreadStaticsInModuleTask : public VThreadedTask
  {
  public:
    virtual void Run(VManagedThread* pThread) HKV_OVERRIDE
    {
      SyncThreadLocalStatics();
    }
  };

public:
  vHavokSyncStaticsCallbackHandler() {}

  void Init();
  void DeInit();
  
  virtual void OnHandleCallback(IVisCallbackDataObject_cl* pData) HKV_OVERRIDE;

private:
  static void SyncStatics();
  static void UnsyncStatics();

  static void SyncThreadLocalStatics();
  static void UnsyncThreadLocalStatics();
};

//-----------------------------------------------------------------------------------
// Implementation

void vHavokSyncStaticsCallbackHandler::Init()
{
  vHavokPhysicsModule::OnSyncStatics += this;
  vHavokPhysicsModule::OnUnsyncStatics += this;
  vHavokPhysicsModule::OnSyncThreadLocalStatics += this;
  vHavokPhysicsModule::OnUnsyncThreadLocalStatics += this;

  // Trigger synchronization only for this module if the Havok Physics Plugin is already initialized.
  vHavokPhysicsModule* pModule = vHavokPhysicsModule::GetInstance();
  if (pModule != NULL)
  {
    pModule->WaitForSimulationToComplete();

    SyncStatics();

    VSyncLocalThreadStaticsInModuleTask task;
    vHavokPhysicsModule::PerformTaskOnAllThreads(&task);

    // Force synchronization of hkWorkerThreads.
    pModule->SyncThreadLocalStaticsInHkThreadPool();
  }
}

void vHavokSyncStaticsCallbackHandler::DeInit()
{
  vHavokPhysicsModule::OnSyncStatics -= this;
  vHavokPhysicsModule::OnUnsyncStatics -= this;
  vHavokPhysicsModule::OnSyncThreadLocalStatics -= this;
  vHavokPhysicsModule::OnUnsyncThreadLocalStatics -= this;
}

void vHavokSyncStaticsCallbackHandler::OnHandleCallback(IVisCallbackDataObject_cl* pData)
{
  if (pData->m_pSender == &vHavokPhysicsModule::OnSyncStatics)
  {
    SyncStatics();
  }
  else if (pData->m_pSender == &vHavokPhysicsModule::OnUnsyncStatics)
  {
    UnsyncStatics();
  }
  else if (pData->m_pSender == &vHavokPhysicsModule::OnSyncThreadLocalStatics)
  {
    SyncThreadLocalStatics();
  }
  else if (pData->m_pSender == &vHavokPhysicsModule::OnUnsyncThreadLocalStatics)
  {
    UnsyncThreadLocalStatics();
  }
}

void vHavokSyncStaticsCallbackHandler::SyncStatics()
{
  hkMemoryInitUtil::SyncInfo s;
  vHavokPhysicsModuleGetSyncInfo(s);

  extern hkBool hkBaseSystemIsInitialized;
  extern HK_THREAD_LOCAL(hkMonitorStream*) hkMonitorStream__m_instance;

  if (!hkBaseSystemIsInitialized && s.m_memoryRouter)
  {
    hkStackTracer::replaceImplementation(s.m_stackTracerImpl);
    hkMemoryRouter::replaceInstance(s.m_memoryRouter);
    hkMemorySystem::replaceInstance(s.m_memorySystem);
    if (s.m_singletonList) 
    { 
      hkSingletonInitNode::populate( hkSingletonInitList, s.m_singletonList ); 
    }
    hkBaseSystem::initSingletons();
    HK_THREAD_LOCAL_SET(hkMonitorStream__m_instance, s.m_monitors);
    HK_ON_DEBUG_MULTI_THREADING(hkMultiThreadCheck::m_criticalSection =  s.m_mtCheckSection;)
    HK_ON_DEBUG_MULTI_THREADING(hkMultiThreadCheck::s_stackTree =  s.m_mtCheckStackTree;)
    hkBaseSystemIsInitialized = true;
  } 
}

void vHavokSyncStaticsCallbackHandler::UnsyncStatics()
{
  hkMemoryInitUtil::SyncInfo s;
  vHavokPhysicsModuleGetSyncInfo(s);

  extern hkBool hkBaseSystemIsInitialized;
  extern HK_THREAD_LOCAL(hkMonitorStream*) hkMonitorStream__m_instance;

  if (hkBaseSystemIsInitialized)
  {
    hkBaseSystemIsInitialized = false;
    HK_ON_DEBUG_MULTI_THREADING(hkMultiThreadCheck::s_stackTree =  HK_NULL;)
    HK_ON_DEBUG_MULTI_THREADING(hkMultiThreadCheck::m_criticalSection =  HK_NULL;)
    HK_THREAD_LOCAL_SET(hkMonitorStream__m_instance, HK_NULL);

    hkBaseSystem::quitSingletons();
    if (s.m_singletonList) 
    {
      hkSingletonInitNode::depopulate(hkSingletonInitList, s.m_singletonList); 
    }

    hkMemorySystem::replaceInstance(HK_NULL);
    hkMemoryRouter::replaceInstance(HK_NULL);
    hkStackTracer::replaceImplementation(HK_NULL);
  }
}

void vHavokSyncStaticsCallbackHandler::SyncThreadLocalStatics()
{
  hkMemoryInitUtil::SyncInfo s;
  vHavokPhysicsModuleGetSyncInfo(s);

  extern HK_THREAD_LOCAL(hkMonitorStream*) hkMonitorStream__m_instance;
  if (s.m_memoryRouter)
  {
    hkMemoryRouter::replaceInstance(s.m_memoryRouter);
    HK_THREAD_LOCAL_SET(hkMonitorStream__m_instance, s.m_monitors);
  }
}

void vHavokSyncStaticsCallbackHandler::UnsyncThreadLocalStatics()
{
  extern HK_THREAD_LOCAL(hkMonitorStream*) hkMonitorStream__m_instance;
  hkMemoryRouter::replaceInstance(HK_NULL);
  HK_THREAD_LOCAL_SET(hkMonitorStream__m_instance, HK_NULL);
}

//-----------------------------------------------------------------------------------
// Synchronization macros

static vHavokSyncStaticsCallbackHandler g_HavokModuleCallbackHandler;

#define VISION_HAVOK_SYNC_STATICS() g_HavokModuleCallbackHandler.Init()
#define VISION_HAVOK_UNSYNC_STATICS() g_HavokModuleCallbackHandler.DeInit()

#endif

#else // No synchronization needed

#define VISION_HAVOK_SYNC_STATICS() {}
#define VISION_HAVOK_UNSYNC_STATICS() {}

#endif // macros

#endif // DOCS

#endif //VHAVOKSYNC_HPP_INCLUDED

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
