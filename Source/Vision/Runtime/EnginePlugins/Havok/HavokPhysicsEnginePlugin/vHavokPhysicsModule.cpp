/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokShapeFactory.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokShapeCache.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConstraint.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConstraintChain.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConstraintListener.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokCpuThreadPool.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokStaticMesh.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokRagdoll.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokCharacterController.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokCharacterControllerHelpers.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokBlockerVolumeComponent.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokProfiler.hpp>

#ifdef SUPPORTS_TERRAIN
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokTerrain.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainSector.hpp>
#endif

#include <Common/Base/Thread/JobQueue/hkJobQueue.h>
#ifdef _VISION_PS3
#include <Common/Base/Thread/Pool/hkSpuThreadPool.h>
#endif
#include <Common/Base/System/Error/hkDefaultError.h>
#include <Common/Base/System/Hardware/hkHardwareInfo.h>
#include <Common/Base/Algorithm/Sort/hkSort.h>

// Support for MT raycast query (if Enqueue is called, otherwise it will just do it as normal)
#include <Physics2012/Collide/Query/CastUtil/hkpWorldRayCastOutput.h>
#include <Physics2012/Collide/Query/Multithreaded/RayCastQuery/hkpRayCastQueryJobs.h>
#include <Physics2012/Collide/Query/Multithreaded/RayCastQuery/hkpRayCastQueryJobQueueUtils.h>
#include <Physics2012/Collide/Query/Multithreaded/CollisionQuery/hkpCollisionQueryJobQueueUtils.h>

#include <Physics2012/Collide/Query/Collector/RayCollector/hkpClosestRayHitCollector.h>
#include <Physics2012/Collide/Query/Collector/RayCollector/hkpAllRayHitCollector.h>
#include <Physics2012/Collide/Query/Collector/PointCollector/hkpAllCdPointCollector.h>
#include <Physics2012/Collide/Query/Collector/PointCollector/hkpRootCdPoint.h>
#include <Physics2012/Collide/Query/Collector/PointCollector/hkpFixedBufferCdPointCollector.h>

#include <Physics2012/Collide/Shape/Query/hkpShapeRayCastInput.h>
#include <Physics2012/Collide/Query/CastUtil/hkpWorldRayCastInput.h>
#include <Physics2012/Collide/Query/CastUtil/hkpWorldRayCaster.h>

#include <Physics2012/Dynamics/Constraint/Chain/hkpConstraintChainInstance.h>
#include <Physics2012/Dynamics/Collide/Filter/Constraint/hkpConstraintCollisionFilter.h>
#include <Physics2012/Internal/Collide/BvCompressedMesh/hkpBvCompressedMeshShape.h>

#include <Physics2012/Utilities/Dynamics/ImpulseAccumulator/hkpImpulseAccumulator.h>

#include <Common/Base/Math/Vector/hkVector4Util.h>
#include <Physics2012/Collide/BroadPhase/hkpBroadPhase.h>
#include <Physics2012/Collide/BroadPhase/3AxisSweep/hkp3AxisSweep.h>
#include <Physics2012/Internal/BroadPhase/TreeBroadPhase/hkpTreeBroadPhase.h>
#include <Physics2012/Dynamics/World/BroadPhaseBorder/hkpBroadPhaseBorder.h>
#include <Physics2012/Dynamics/World/hkpPhysicsSystem.h>

#include <Common/Base/Config/hkConfigVersion.h>
#include <Common/Serialize/Version/hkVersionPatchManager.h>
#include <Common/Serialize/Util/hkBuiltinTypeRegistry.h>
#include <Common/Base/Reflection/Registry/hkDefaultClassNameRegistry.h>
#include <Common/Base/Reflection/Registry/hkVtableClassRegistry.h>
#include <Common/Serialize/Util/hkRootLevelContainer.h>

#if (HAVOK_SDK_VERSION_MAJOR >= 2010)
   #include <Common/Base/Memory/Allocator/Malloc/hkMallocAllocator.h>
#else
    // Serialization and patches
    #include <Common/Serialize/Util/hkSerializeDeprecated.h>
    extern void HK_CALL registerPatches(hkVersionPatchManager&);
#endif

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/VScriptIncludes.hpp>


#if defined(HK_PLATFORM_HAS_SPU)
#	include <Common/Base/Spu/Util/hkSpuUtil.h>
# include <Common/Base/Spu/Spurs/hkSpurs.h>
# include <Vision/Runtime/Base/System/Threading/SpursHandler/VSpursHandler.hpp>
#endif

#if defined HK_PLATFORM_PS3_PPU
#	include <Common/Base/Memory/PlatformUtils/Spu/hkSpuMemoryInternal.h>
#	define SPURS_THREAD_LAYER_PRIORITY 250
#	define SPURS_HANDLER_THREAD_PRIORITY 1
// Max. no. of SPUs available in hardware
#	define MAX_SPU_THREADS 5

// Max. no. of SPUs Havok will use. Keep this number > 0 and < MAX_SPU_THREADS
// Vision needs at least 1 SPU free, so setting this number to the maximum of availables SPUs will
// cause a hang in the system.
# define HK_MAX_SPU 2
#endif

// Uncomment the following two lines to always force single-threaded physics
//#undef HK_CONFIG_THREAD
//#define HK_CONFIG_THREAD HK_CONFIG_SINGLE_THREADED

class vHavokCreateMemoryRouterTask : public VThreadedTask
{
public:
  virtual void Run(VManagedThread* pThread) HKV_OVERRIDE
  {
    VASSERT_MSG(VThreadManager::GetManager()->GetExecutingThread() != NULL, "This callback should only be triggered on worker threads");

#if defined(HK_COMPILER_HAS_INTRINSICS_IA32) && HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED
    // Flush all denormal/subnormal numbers (2^-1074 to 2^-1022) to zero.
    // Typically operations on denormals are very slow, up to 100 times slower than normal numbers.
    _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
#endif

    // circumvent problem with uninitialized pointer to hkMemoryRouter in hkBaseSystem
    hkMemoryRouter::replaceInstance(HK_NULL);

    hkMemoryRouter* pMemoryRouter = ::new hkMemoryRouter();
    hkMemorySystem::getInstance().threadInit(*pMemoryRouter, "hkCpuThreadPool");
    hkBaseSystem::initThread(pMemoryRouter);

    vHavokPhysicsModule::OnSyncThreadLocalStatics.TriggerCallbacks();
  }
};

class vHavokDeleteMemoryRouterTask : public VThreadedTask
{
public:
  virtual void Run(VManagedThread* pThread) HKV_OVERRIDE
  {
    VASSERT_MSG(VThreadManager::GetManager()->GetExecutingThread() != NULL, "This callback should only be triggered on worker threads");

    vHavokPhysicsModule::OnUnsyncThreadLocalStatics.TriggerCallbacks();

    hkMemoryRouter* pMemoryRouter = hkMemoryRouter::getInstancePtr();
    VASSERT(pMemoryRouter != NULL);
    if (pMemoryRouter != NULL)
    {
      hkBaseSystem::quitThread();
      hkMemorySystem::getInstance().threadQuit(*pMemoryRouter);
      hkMemoryRouter::replaceInstance(HK_NULL);
      ::delete pMemoryRouter;
    }
  }
};

#if defined(_VISION_WIN32) || defined(_VISION_XENON) || defined(_VISION_PS3)
  //bool g_bUseVisionTask = false;
  bool g_bUseVisionTask = true;
#else
  bool g_bUseVisionTask = false;
#endif

unsigned int g_iHavokMaxThreads = hkvMath::Min<int> (vHavokCpuThreadPool::MAX_NUM_THREADS, (MAX_NUM_CPU_THREADS - 1));

#define RAYCAST_THREAD_THRESHOLD 10
#define RAYCAST_THREAD_RESULTS_PER_CMD 10 // please note: MAXIMUM_RESULTS_CAPACITY = 32

#define BROADPHASE_SIZE_TOLERANCE hkReal(10.0)

// -------------------------------------------------------------------------- //
// Physics background task                                                    //
// -------------------------------------------------------------------------- //
V_IMPLEMENT_DYNCREATE( VHavokTask, VThreadedTask, &g_vHavokModule );

#if defined(HAVOK_SDK_VERSION_MAJOR) && (HAVOK_SDK_VERSION_MAJOR >= 2012)

#include <Common/Compat/hkHavokVersions.h>

#ifdef _VISION_MOBILE
  #define HK_SERIALIZE_MIN_COMPATIBLE_VERSION_INTERNAL_VALUE HK_HAVOK_VERSION_660b1
#else
  #define HK_SERIALIZE_MIN_COMPATIBLE_VERSION_INTERNAL_VALUE HK_HAVOK_VERSION_300
#endif

void HK_CALL registerCommonCollidePhysicsPatches(hkVersionPatchManager& man)
{
   #undef HK_EXCLUDE_COMMON_PATCHES
   #undef HK_FEATURE_PRODUCT_AI
   #undef HK_FEATURE_PRODUCT_ANIMATION
   #undef HK_FEATURE_PRODUCT_CLOTH
   #undef HK_FEATURE_PRODUCT_DESTRUCTION_2012
   #undef HK_FEATURE_PRODUCT_DESTRUCTION
   #undef HK_FEATURE_PRODUCT_BEHAVIOR
   #undef HK_FEATURE_PRODUCT_PHYSICS
   #undef HK_FEATURE_PRODUCT_SIMULATION
   #define HK_FEATURE_PRODUCT_PHYSICS_2012
   #include <Common/Compat/Patches/hkRegisterPatches.cxx>
}
#else
extern void HK_CALL registerCommonPatches();
extern void HK_CALL registerCollidePatches();
extern void HK_CALL registerPhysicsPatches();
#endif

// Global world settings
const char* vHavokPhysicsModule_GetVersionString()
{
  return HAVOK_SDK_VERSION_NUM_STRING;
}

// disable collisions with HK_LAYER_COLLIDABLE_TERRAIN_HOLE and HK_LAYER_COLLIDABLE_DISABLED
#define DEFAULT_MASK (0xfffffffe &~ (1 << vHavokPhysicsModule::HK_LAYER_COLLIDABLE_TERRAIN_HOLE) &~ (1 << vHavokPhysicsModule::HK_LAYER_COLLIDABLE_DISABLED))

#define DEFAULT_HAVOK_TO_VISION_SCALE (100.0f*Vision::World.GetGlobalUnitScaling())

#define VHAVOK_BASE_TIME_STEP (1.0f/60.0f)

struct vHavokPhysicsModuleDefaultWorldParams
{
  vHavokPhysicsModuleDefaultWorldParams()
  {
    Reset();
  }

  void Reset()
  {
    m_setupParams.m_fHavokToVisionScale =  DEFAULT_HAVOK_TO_VISION_SCALE;
    m_setupParams.m_iStaticGeomMode = vHavokPhysicsModule::SGM_SINGLE_INSTANCES;
    m_setupParams.m_iMergedStaticWeldingType = VIS_WELDING_TYPE_NONE;

    m_runtimeParams.m_bBroadphaseAutoCompute = TRUE;
    m_runtimeParams.m_fBroadphaseManualSize =  2000.f*DEFAULT_HAVOK_TO_VISION_SCALE;
    m_runtimeParams.m_vGravity = hkvVec3(0, 0, -9.81f*DEFAULT_HAVOK_TO_VISION_SCALE);
    m_runtimeParams.m_bEnableDiskShapeCaching = TRUE;
    m_runtimeParams.m_bEnableConstraintCollisionFilter = FALSE;
    m_runtimeParams.m_bEnableLegacyCompoundShapes = FALSE;
    m_runtimeParams.m_iCollisionGroupMasks[0] = 0xfffffffe;
    for (int i=1;i<32;i++)
      m_runtimeParams.m_iCollisionGroupMasks[i] = DEFAULT_MASK;
    m_runtimeParams.m_iCollisionGroupMasks[vHavokPhysicsModule::HK_LAYER_COLLIDABLE_TERRAIN_HOLE] = 0;
    m_runtimeParams.m_iCollisionGroupMasks[vHavokPhysicsModule::HK_LAYER_COLLIDABLE_DISABLED] = 0;

    m_runtimeParams.m_solverIterations = 4;
    m_runtimeParams.m_solverHardness = 1;

  }

  vHavokPhysicsModuleDefaultSetupWorldParams m_setupParams;
  vHavokPhysicsModuleDefaultRuntimeWorldParams m_runtimeParams;
};

static vHavokPhysicsModuleDefaultWorldParams g_vGlobalWorldParams;

vHavokPhysicsModuleDefaultSetupWorldParams& vHavokPhysicsModule_GetWorldSetupSettings()
{
  return g_vGlobalWorldParams.m_setupParams;
}

vHavokPhysicsModuleDefaultRuntimeWorldParams& vHavokPhysicsModule_GetDefaultWorldRuntimeSettings()
{
  return g_vGlobalWorldParams.m_runtimeParams;
}

void vHavokPhysicsModule_ResetWorldSettings()
{
  g_vGlobalWorldParams.Reset();
}

class vHavokPhysicsRaycaster
{
public:

   vHavokPhysicsRaycaster(vHavokPhysicsModule *pModule, bool bHandleMultipleHits) : m_pModule(pModule)
  {
    m_bHandleMultipleHits = bHandleMultipleHits;
    m_iResultCapacity = m_bHandleMultipleHits ? RAYCAST_THREAD_RESULTS_PER_CMD : 1;
   }

   void AddRay(VisPhysicsRaycastBase_cl *pRaycastData)
   {
      m_rays.pushBack(pRaycastData);
      hkpWorldRayCastCommand& cmd = m_commands.expandOne();

      hkpWorldRayCastInput& ray = cmd.m_rayInput;
    vHavokConversionUtils::VisVecToPhysVecWorld(pRaycastData->vRayStart, ray.m_from);
    vHavokConversionUtils::VisVecToPhysVecWorld(pRaycastData->vRayEnd, ray.m_to);
    ray.m_enableShapeCollectionFilter = true;
      ray.m_filterInfo = pRaycastData->iCollisionBitmask;
      cmd.m_resultsCapacity = m_iResultCapacity;
      cmd.m_numResultsOut = 0;
      cmd.m_useCollector = true;
   }

   void Execute()
   {

      int numRaycasts = m_commands.getSize();
      if (numRaycasts == 0)
         return;

      const hkpWorld* world = m_pModule->GetPhysicsWorld(); // make the world const so we can just mark for Read Only access
      hkJobQueue* jobQueue = m_pModule->GetJobQueue();
      hkThreadPool* threadPool = m_pModule->GetThreadPool();

      // Do we have a thread pool to do work for us
      if ( threadPool && (numRaycasts > RAYCAST_THREAD_THRESHOLD) )
      {
         // Fix up the output ptrs
         m_rayOutputs.setSize(m_commands.getSize() * m_iResultCapacity);
         for (int i=0; i < m_commands.getSize(); ++i)
         {
            hkpWorldRayCastCommand& cmd = m_commands[i];
            cmd.m_results = &m_rayOutputs[i*m_iResultCapacity];
            cmd.m_results->reset();
         }

         hkArray<hkpCollisionQueryJobHeader*> jobHeaders;
         // Work out how many jobs we should make
         int numJobs = threadPool->getNumThreads() + 1; // +1 == this one
         int numFullSize = numRaycasts / RAYCAST_THREAD_THRESHOLD;
         numJobs = hkvMath::Min(numFullSize, numJobs);

         int jobSize = numRaycasts / numJobs;
         int firstJobSize = numRaycasts % numJobs;// first job will take remainder too
         int jobsSoFar = 0;

         // Add the jobs
         for (int j=0; j < numJobs; ++j)
         {
            int thisJobSize = jobSize;
            if (j==0)
            {
               thisJobSize += firstJobSize;
            }
            hkpCollisionQueryJobHeader* jobHeader = hkAllocateChunk<hkpCollisionQueryJobHeader>(1, HK_MEMORY_CLASS_DISPLAY);
            jobHeaders.pushBack(jobHeader);
            world->markForRead();
            hkpWorldRayCastJob worldRayCastJob( world->getCollisionInput(), jobHeader, m_commands.begin() + jobsSoFar, thisJobSize, world->m_broadPhase, HK_NULL);
            worldRayCastJob.setRunsOnSpuOrPpu();
            world->unmarkForRead();
            jobQueue->addJob( worldRayCastJob, hkJobQueue::JOB_LOW_PRIORITY );
            jobsSoFar += thisJobSize;
         }

         // Do the work
         threadPool->processJobQueue( jobQueue ); // do some ourselves
         threadPool->waitForCompletion(); // done

         for (int i=0;i<numRaycasts;i++)
         {
            hkpWorldRayCastCommand* command = &m_commands[i];
        for(int j=0;j<m_iResultCapacity;j++)
        {
          hkpWorldRayCastOutput &output = m_rayOutputs[i*m_iResultCapacity+j];
          if(output.hasHit())
            m_pModule->ForwardRaycastData(m_rays[i], &output);
        }
            m_rays[i]->onFinished();
         }

         for(int j = 0; j < jobHeaders.getSize(); ++j)
         {
            hkDeallocateChunk<hkpCollisionQueryJobHeader>(jobHeaders[j], 1, HK_MEMORY_CLASS_DISPLAY);
         }
      }
      else // not threaded
      {
      if (!m_bHandleMultipleHits)
      {
        hkArray<hkpWorldRayCastInput>::Temp inputs; inputs.setSize(numRaycasts);
        hkArray<hkpClosestRayHitCollector>::Temp collectors; collectors.setSize(numRaycasts);
        for (int i=0; i < numRaycasts; ++i)
        {
          inputs[i] = m_commands[i].m_rayInput;
        }

        world->markForRead();
        hkpWorldRayCaster rayCaster;
        rayCaster.castRayGroup( *world->getBroadPhase(), inputs.begin(), numRaycasts, world->getCollisionFilter(), collectors.begin(), sizeof(hkpClosestRayHitCollector) );
        world->unmarkForRead();

        for (int i =0; i < numRaycasts; i++)
        {
          hkpClosestRayHitCollector& collector = collectors[i];
          const hkpWorldRayCastOutput& output = collector.getHit();
          if(output.hasHit())
          {
            m_pModule->ForwardRaycastData(m_rays[i], &output);
          }
          m_rays[i]->onFinished();
        }
      }
      else
      {
        hkArray<hkpWorldRayCastInput>::Temp inputs; inputs.setSize(numRaycasts);
        hkArray<hkpAllRayHitCollector>::Temp collectors; collectors.setSize(numRaycasts);
        for (int i=0;i<numRaycasts;i++)
        {
          inputs[i] = m_commands[i].m_rayInput;
        }

        world->markForRead();
        hkpWorldRayCaster rayCaster;
        rayCaster.castRayGroup( *world->getBroadPhase(), inputs.begin(), numRaycasts, world->getCollisionFilter(), collectors.begin(), sizeof(hkpAllRayHitCollector) );
        world->unmarkForRead();

        for (int i=0;i<numRaycasts;i++)
        {
          hkpAllRayHitCollector& collector = collectors[i];
          const hkArray<hkpWorldRayCastOutput>& hits = collector.getHits();
          for (int j=0;j<hits.getSize();j++)
          {
            const hkpWorldRayCastOutput& hit = hits[j];
            if (hit.hasHit())
            {
              m_pModule->ForwardRaycastData(m_rays[i],&hit);
            }
          }
          m_rays[i]->onFinished();
        }
      }
      }


      Reset();
   }

   void Reset()
   {
      m_rayOutputs.setSize(0);
      m_commands.setSize(0);
      m_rays.setSize(0);
   }

protected:

   vHavokPhysicsModule* m_pModule;
  bool m_bHandleMultipleHits;
  int m_iResultCapacity;

   hkArray<hkpWorldRayCastOutput> m_rayOutputs;
   hkArray<hkpWorldRayCastCommand> m_commands;
   hkArray<VisPhysicsRaycastBase_cl*> m_rays;


};

VHavokTask::VHavokTask(vHavokPhysicsModule *pModule)
{
  m_pModule = pModule;
}

void VHavokTask::Run(VManagedThread *pThread)
{
  m_pModule->PerformSimulation(true, (pThread != NULL));
  // The last simulation run may not yet be finished; wait for it here.
  m_pModule->WaitForForegroundSimulationToComplete();
}


// --------------------------------------------------------------------------
// Physics callback data object
// --------------------------------------------------------------------------
vHavokPhysicsModuleCallbackData::vHavokPhysicsModuleCallbackData(
  VisCallback_cl *pSender, vHavokPhysicsModule *pHavokModule)
: IVisCallbackDataObject_cl(pSender)
{
  m_pHavokModule = pHavokModule;
}

vHavokPhysicsModule *vHavokPhysicsModuleCallbackData::GetHavokModule() const
{
  return m_pHavokModule;
}


// -------------------------------------------------------------------------- //
// Static Members
// -------------------------------------------------------------------------- //

VisCallback_cl vHavokPhysicsModule::OnBeforeInitializePhysics;
VisCallback_cl vHavokPhysicsModule::OnAfterInitializePhysics;
VisCallback_cl vHavokPhysicsModule::OnBeforeDeInitializePhysics;
VisCallback_cl vHavokPhysicsModule::OnAfterDeInitializePhysics;
VisCallback_cl vHavokPhysicsModule::OnBeforeWorldCreated;
VisCallback_cl vHavokPhysicsModule::OnAfterWorldCreated;
VisCallback_cl vHavokPhysicsModule::OnFetchPhysicsResults;
VisCallback_cl vHavokPhysicsModule::OnHandleResourceFile;

VisCallback_cl vHavokPhysicsModule::OnSyncStatics;
VisCallback_cl vHavokPhysicsModule::OnUnsyncStatics;
VisCallback_cl vHavokPhysicsModule::OnSyncThreadLocalStatics;
VisCallback_cl vHavokPhysicsModule::OnUnsyncThreadLocalStatics;


// -------------------------------------------------------------------------- //
// Custom Error Handling                                                             //
// -------------------------------------------------------------------------- //

void HK_CALL dummyErrorReport(const char* msg, void* errorReportObject)
{
}

class vHavokError : public hkDefaultError
{
public:

  HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE);

   vHavokError( vHavokPhysicsModule* m ) : hkDefaultError(dummyErrorReport, HK_NULL), m_pPhysicsModule(m)
  {
    setEnabled(0x70aae483, false); // disables unnecessary alignment check
    setEnabled(0xf03421de, false); // disables unnecessary warning in "hkpPointToPathConstraintData::setInBodySpace()"
  }

  virtual ~vHavokError() { }
   virtual int message(hkError::Message msg, int id, const char* description, const char* file, int line)
   {
      if( id == -1 && m_sectionIds.getSize() )
      {
         id = m_sectionIds.back();
      }

      if (!isEnabled(id))
      {
         return 0;
      }

      const char* what = "";
    VStrList *pList = &m_pPhysicsModule->m_cachedWarnings;

      hkBool stackTrace = false;
      switch( msg )
      {
         case MESSAGE_REPORT:
            what = "Report";
        pList = &m_pPhysicsModule->m_cachedMessages;
            break;
         case MESSAGE_WARNING:
            what = "Warning";
            break;
         case MESSAGE_ASSERT:
            what = "Assert";
            stackTrace = true;
            break;
         case MESSAGE_ERROR:
            what = "Error";
            stackTrace = true;
            break;
      }

      VVERIFY_OR_RET_VAL(m_pPhysicsModule, true);

      hkStringBuf sMsg;
      sMsg.printf("%s(%d) [Havok %s id:0x%x] : %s", file, line, what, id, description);

      // This errorReport method can be called from worker threads. We thus delay
      // the logging of the messages to the main thread (FetchResults);
      m_pPhysicsModule->m_errorReportMutex.Lock();
      pList->AddString(sMsg.cString());
      m_pPhysicsModule->m_errorReportMutex.Unlock();



      bool assert = ( msg == MESSAGE_ASSERT || msg == MESSAGE_ERROR); // breakpoint or not
      if (assert)
      {
         if ( VAssert( file, line, sMsg.cString() ) )
         {
            VDBGBREAK; // uses __debugbreak so kinder to vforge etc
         }
      }
      return false; // == never use default breakpoint
   }

   vHavokPhysicsModule* m_pPhysicsModule;
};




// -------------------------------------------------------------------------- //
// Memory Management                                                          //
// -------------------------------------------------------------------------- //

#if (HAVOK_SDK_VERSION_MAJOR >= 2010)
class vHavokAllocator : public hkMemoryAllocator
{
public:
  vHavokAllocator()
  {}

  virtual void* blockAlloc(int numBytes)
  {
    hkUint32 cur = hkCriticalSection::atomicExchangeAdd(&m_currentUsed, numBytes);
     if(cur + numBytes > m_peakUse)
     {
        m_peakUse = m_currentUsed;
     }

    return vMemAlignedAlloc(numBytes, HK_REAL_ALIGNMENT);
  }

  virtual void blockFree(void* p, int numBytes)
  {
    hkCriticalSection::atomicExchangeAdd(&m_currentUsed,  -numBytes);

    vMemAlignedFree(p);
  }

  virtual void getMemoryStatistics( MemoryStatistics& u ) const
  {
     u.m_allocated = m_currentUsed;
     u.m_peakInUse = m_peakUse;
  }

  virtual void resetPeakMemoryStatistics()
  {
     m_peakUse = m_currentUsed;
  }

  virtual int getAllocatedSize( const void* obj, int numBytes ) const
  {
     return numBytes;
  }

protected:

  hkUint32 m_currentUsed;
   hkUint32 m_peakUse;

} gAllocator;
#endif


// -------------------------------------------------------------------------- //
// Initialisation & Deinitialisation                                          //
// -------------------------------------------------------------------------- //

vHavokPhysicsModule::vHavokPhysicsModule()
  : m_steppedExternally(false)
  , m_vdbSteppedExternally(false)
  , m_iMinSolverBufferIncreaseStep(2 * 1024 * 1024)
  , m_bPaused(false)
  , m_bSimulationStarted(false)
  , m_bNewResultsAvailable(false)
  , m_bCurrentRunIsAsync(false)
  , m_spVisualDebugger(NULL)
  , m_spDisplayHandler(NULL)
  , m_bVisualDebugger(false)
  , m_bDebugDisplay(false)
  , m_bIsSceneLoading(false)
  , m_bResultsExpected(false)
  , m_pSweepSemaphore(NULL)
  , m_iVisualDebuggerPort(HK_VISUAL_DEBUGGER_DEFAULT_PORT)
{
  vHavokConversionUtils::SetHavok2VisionScale(DEFAULT_HAVOK_TO_VISION_SCALE);
  m_staticGeomMode = vHavokPhysicsModule::SGM_SINGLE_INSTANCES;
  m_mergedStaticWeldingType = VIS_WELDING_TYPE_NONE;

  VSceneLoader::OnCustomChunkSerialization += this; // this callback is used to save out globals (deprecated, only for 8.2 releases)
  VSceneLoader::OnCustomChunkBeforeShapesSerialization += this; // this callback is used to save out globals

  m_pJobQueue = HK_NULL;
  m_pTaskQueue = HK_NULL;
  m_pThreadPool = HK_NULL;
  m_pSpuUtil = HK_NULL;
  m_pRayCasterSingle = HK_NULL;
  m_pRayCasterMultiple = HK_NULL;

  m_pProfiler = HK_NULL;
  m_pPhysicsWorld = HK_NULL;

  m_bAsyncPhysics = false;
  m_pTask = new VHavokTask(this);

  m_bUpdateFilter = false;
  m_iCollisionNotificationQueueSize = 0;
  m_iTriggerNotificationQueueSize = 0;

  m_bDebugRenderRigidBodies = false;
  m_bDebugRenderRagdolls = false;
  m_bDebugRenderCharacterControllers = false;
  m_bDebugRenderTriggerVolumes = false;
  m_bDebugRenderBlockerVolumes = false;
  m_bDebugRenderStaticMeshes = false;

  // Set simulation time step
  m_fAccumulatedTime = 0.0f;
  m_fTimeStep = VHAVOK_BASE_TIME_STEP;

  m_iMaxTicksPerFrame = 2;            // use up to two physics steps per frame
  m_bFixedTicksPerFrame = false;      // don't use a fixed number of physics steps per frame
  m_bAllowVariableStepRate = true;    // allow variable step rate...
  m_fMinTimeStep = 1.0f / 120.0f;     // ...between 120Hz...
  m_fMaxTimeStep = 1.0f / 30.0f;      // ...and 30Hz.

  /*
  Example for fixed step rate at VHAVOK_BASE_TIME_STEP with max. 4 steps per frame:
  m_iMaxTicksPerFrame = 4;
  m_bFixedTicksPerFrame = false;
  m_fMinTimeStep = 0.0f;
  m_fMaxTimeStep = 0.0f;
  m_bAllowVariableStepRate = false;
  */
}

vHavokPhysicsModule::~vHavokPhysicsModule(void)
{
  VSceneLoader::OnCustomChunkSerialization-= this;
  VSceneLoader::OnCustomChunkBeforeShapesSerialization -= this;

  if (g_bUseVisionTask && m_pTask)
    Vision::GetThreadManager()->WaitForTask(m_pTask, true);
  V_SAFE_DELETE(m_pTask);
}

BOOL vHavokPhysicsModule::OnInitPhysics()
{
  // Keep in sync
  vHavokConversionUtils::SetHavok2VisionScale(g_vGlobalWorldParams.m_setupParams.m_fHavokToVisionScale);
  m_staticGeomMode = (StaticGeomMode)g_vGlobalWorldParams.m_setupParams.m_iStaticGeomMode;
  m_mergedStaticWeldingType = (VisWeldingType_e)g_vGlobalWorldParams.m_setupParams.m_iMergedStaticWeldingType;

  // circumvent problem with uninitialized pointer to hkMemoryRouter in hkBaseSystem
  hkMemoryRouter::replaceInstance( HK_NULL );

  // Initialize the base system including our memory system
  {
#if (HAVOK_SDK_VERSION_MAJOR >= 2010)
    hkMemorySystem::FrameInfo finfo(m_iMinSolverBufferIncreaseStep);

    #if 0
      // A special Debug allocator that checks overruns etc.
      // This will route all allocations to the Vision allocator, also the temp allocations during simulation, which will result in many small allocations.
      // Only activate it, if you really need to track memory issues, like leaks or overruns.
      hkMemoryRouter* pMemoryRouter = hkMemoryInitUtil::initChecking(&gAllocator, finfo);
      //hkMemoryInitUtil::initMemoryTracker();
    #else
      // Normal
      hkMemoryRouter* pMemoryRouter = hkMemoryInitUtil::initDefault(&gAllocator, finfo);
    #endif

#else // if (HAVOK_SDK_VERSION_MAJOR < 2010)

    hkMemoryRouter* pMemoryRouter = hkMemoryInitUtil::initFreeList();

#endif

    if (pMemoryRouter == HK_NULL)
      return FALSE;
    
#if defined(HK_COMPILER_HAS_INTRINSICS_IA32) && HK_CONFIG_SIMD == HK_CONFIG_SIMD_ENABLED
    // Flush all denormal/subnormal numbers (2^-1074 to 2^-1022) to zero.
    // Typically operations on denormals are very slow, up to 100 times slower than normal numbers.
    _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
#endif

    if ( hkBaseSystem::init( pMemoryRouter, dummyErrorReport, this ) != HK_SUCCESS)
      return FALSE;

   // The call to hkBaseSystem::init() above calls hkProductFeatures::initialize() which will
   // register patches for all products which have keycodes. Clear these patches and only
   // register patches for Common/Physics products.
   if ( hkVersionPatchManager::getInstance().getNumPatches() > 0 )
   {
      hkVersionPatchManager::getInstance().clearPatches();
   }
#if defined(HAVOK_SDK_VERSION_MAJOR) && (HAVOK_SDK_VERSION_MAJOR >= 2012)
    extern void HK_CALL registerCommonCollidePhysicsPatches(hkVersionPatchManager& man);
    registerCommonCollidePhysicsPatches( hkVersionPatchManager::getInstance() );
#else
   registerCommonPatches();
   registerCollidePatches();
   registerPhysicsPatches();
#endif
	hkVersionPatchManager::getInstance().recomputePatchDependencies();

     // Replace the error hander
     hkError::replaceInstance( new vHavokError(this) );
  }

  // Initialize serialization and patches
#if (HAVOK_SDK_VERSION_MAJOR < 2010)
  {
    hkSerializeDeprecated::initDeprecated();                // Will do the 'pre 7.0' part
    registerPatches(hkVersionPatchManager::getInstance());  // Will do the 'patches' from 7.0 onward
  }
#endif

  // Register custom Vision shapes
  hkBuiltinTypeRegistry::getInstance().addType(&hkvConvexVerticesShapeTypeInfo, &hkvConvexVerticesShapeClass);
  hkBuiltinTypeRegistry::getInstance().addType(&hkvMeshMaterialTypeInfo, &hkvMeshMaterialClass);
  hkBuiltinTypeRegistry::getInstance().addType(&hkvBvCompressedMeshShapeTypeInfo, &hkvBvCompressedMeshShapeClass);
  hkBuiltinTypeRegistry::getInstance().addType(&hkvSampledHeightFieldShapeTypeInfo, &hkvSampledHeightFieldShapeClass);
  hkBuiltinTypeRegistry::getInstance().addType(&hkvSampledOffsetHeightFieldShapeTypeInfo, &hkvSampledOffsetHeightFieldShapeClass);
  hkBuiltinTypeRegistry::getInstance().addType(&hkvTriSampledHeightFieldCollectionTypeInfo, &hkvTriSampledHeightFieldCollectionClass);
  hkBuiltinTypeRegistry::getInstance().addType(&hkvTriSampledHeightFieldBvTreeShapeTypeInfo, &hkvTriSampledHeightFieldBvTreeShapeClass);

  hkCheckDeterminismUtil::createInstance();

  // Initialize shape cache.
  vHavokShapeCache::Init();

  // Synchronize statics and create thread-local memory routers.
  OnSyncStatics.TriggerCallbacks();
  vHavokCreateMemoryRouterTask task;
  PerformTaskOnAllThreads(&task);

  // If ThreadManager hasn't been initialized yet, we need to create the memory routers in these callbacks:
  VManagedThread::OnThreadLocalVariableInit += this;
  VManagedThread::OnThreadLocalVariableDeInit += this;

  {
    vHavokPhysicsModuleCallbackData callbackData(&OnBeforeInitializePhysics, this);
    callbackData.Trigger();
  }

  CreateJobQueue();

  m_pPhysicsWorld = HK_NULL;
  CreateWorld(); // don't delay so that vForge etc always has one (even if will be recreated soon enough anyway..)

  // Setup misc stats
  m_bIsSceneLoading = false;

  // Register some relevant Vision callbacks
  Vision::Callbacks.OnBeforeSceneLoaded += this;
  Vision::Callbacks.OnAfterSceneLoaded += this;
  Vision::Callbacks.OnWorldDeInit += this;
  IVisSceneManager_cl::OnReposition += this;
  if(Vision::Editor.IsInEditor())
    Vision::Callbacks.OnAfterSceneExported += this;

  m_pRayCasterSingle = new vHavokPhysicsRaycaster(this, false);
  m_pRayCasterMultiple = new vHavokPhysicsRaycaster(this, true);

  // notify listeners that physics was correctly initialized
  {
    vHavokPhysicsModuleCallbackData callbackData(&OnAfterInitializePhysics, this);
    callbackData.Trigger();
  }

  return TRUE;
}

void vHavokPhysicsModule::CreateJobQueue()
{
  // register profiling ids
  PROFILING_HAVOK_PHYSICS_SIMULATION    = Vision::Profiling.GetFreeElementID();
  PROFILING_HAVOK_PHYSICS_FETCH_RESULTS = Vision::Profiling.GetFreeElementID();

  VProfilingNode* pGroup = Vision::Profiling.AddGroup("Havok integrations");
  Vision::Profiling.AddElement(PROFILING_HAVOK_PHYSICS_SIMULATION, "Physics Simulation", TRUE, pGroup);
  Vision::Profiling.AddElement(PROFILING_HAVOK_PHYSICS_FETCH_RESULTS, "Physics Fetch results", TRUE, pGroup);

#if (HK_CONFIG_THREAD == HK_CONFIG_MULTI_THREADED) && !defined(HAVOK_FORCE_SINGLETHREADED) // Will be on PC, PS3 etc.

#if defined(HK_PLATFORM_HAS_SPU)
  VASSERT_MSG( HK_MAX_SPU > 0, "You must specify at least 1 SPU" );
  VASSERT_MSG( HK_MAX_SPU < MAX_SPU_THREADS, "You need to leave at least 1 SPU free for Vision engine." );
  HK_CELL_SPURS* spurs = Vision::GetSpursHandler()->GetSpurs();
  m_pSpuUtil = new hkSpuUtil( spurs );
  m_pSpuUtil->attachHelperThreadToSpurs();

  hkSpuThreadPoolCinfo spuInfo;
  spuInfo.m_maxNumSpus = HK_MAX_SPU; // TODO: Might want to set this according to scene props etc.
  spuInfo.m_spuUtil = m_pSpuUtil;
  m_pThreadPool = new hkSpuThreadPool( spuInfo );
  m_pThreadPool->setNumThreads(spuInfo.m_maxNumSpus);

#else
  vHavokCpuThreadPoolCinfo info;

  //Vision counts Hyper threads as processors
  //unsigned int iNumProcessors = Vision::GetThreadManager()->GetNumberOfProcessors();
  unsigned int iNumProcessors = hkHardwareInfo::calcNumHardwareThreads();
  info.m_numThreads = hkvMath::Min(g_iHavokMaxThreads - 1, iNumProcessors - 1); // leave one for this thread / the main calling thread
  if (info.m_numThreads > 0)
  {
    info.m_OnWorkerThreadCreatedPtr = &OnSyncThreadLocalStatics;
    info.m_OnWorkerThreadFinishedPtr = &OnUnsyncThreadLocalStatics;
    info.m_timerBufferPerThreadAllocation = HKVIS_MONITOR_STREAM_BYTES_PER_THREAD;
    m_pThreadPool = new vHavokCpuThreadPool(info);
  }
#endif

  if (m_pThreadPool)
  {
    // Create multi-threading classes
    hkJobQueueCinfo jobQueueInfo;
    jobQueueInfo.m_maxNumJobTypes = HK_JOB_TYPE_MAX; // Allow user jobs
    jobQueueInfo.m_jobQueueHwSetup.m_numCpuThreads = g_iHavokMaxThreads;
    m_pJobQueue = new hkJobQueue(jobQueueInfo);

    m_pTaskQueue = new hkTaskQueue();

#if defined(PROFILING)
    m_pProfiler = new vHavokProfiler(pGroup);
    m_pJobQueue->setExternalProfiler(m_pProfiler);
#endif

    // Get the Physics world jobs into the job queue dispatch tables etc
    // Will be similar for the other Havok products:
    hkpWorld::registerWithJobQueue(m_pJobQueue);

    hkpFixedBufferCdPointCollector::registerDefaultAddCdPointFunction();

    hkpRayCastQueryJobQueueUtils::registerWithJobQueue(m_pJobQueue);
    hkpCollisionQueryJobQueueUtils::registerWithJobQueue(m_pJobQueue);
  }
#endif
}

void vHavokPhysicsModule::PerformTaskOnAllThreads(VThreadedTask* pTask)
{
  VThreadManager* pThreadManager = VThreadManager::GetManager();
  if (!pThreadManager->IsInitialized() || pThreadManager->GetThreadCount() == 0)
    return;
  pThreadManager->WaitForAllThreads();

  // Store back up of task masks.
  VMemoryTempBuffer<16 * sizeof(unsigned int)> tmpBuffer(pThreadManager->GetThreadCount() * sizeof(unsigned int));
  unsigned int* pThreadMasksBackup = tmpBuffer.GetAs<unsigned int>();

  for (int i = 0; i < pThreadManager->GetThreadCount(); i++)
  {
    pThreadMasksBackup[i] = pThreadManager->GetThreadTaskMask(i);
  }

  // Perform the task.
  for (int i = 0; i < pThreadManager->GetThreadCount(); i++)
  {
    // Disable all threads.
    for (int j = 0; j < pThreadManager->GetThreadCount(); j++)
    {
      pThreadManager->SetThreadTaskMask(j, 0);
    }
    // Enable task just for the current thread.
    pThreadManager->SetThreadTaskMask(i, 1);
    pTask->SetThreadMask(1);

    pThreadManager->ScheduleTask(pTask);
    pThreadManager->WaitForAllThreads();
  }

  // Restore original thread masks.
  for (int i = 0; i < pThreadManager->GetThreadCount(); i++)
  {
    pThreadManager->SetThreadTaskMask(i, pThreadMasksBackup[i]);
  }
}

void vHavokPhysicsModule::SyncThreadLocalStaticsInHkThreadPool()
{
#if !defined(HK_PLATFORM_HAS_SPU)
  if (m_pThreadPool == NULL)
    return;

  static_cast<vHavokCpuThreadPool*>(m_pThreadPool)->setWorkerThreadCreatedFlag();
#endif
}

bool vHavokPhysicsModule::AutoComputeBroadphaseSize(hkAabb &worldBounds)
{
  bool haveBounds = false;
  hkAabb bounds;
  bounds.setEmpty();

  if(m_pPhysicsWorld)
    m_pPhysicsWorld->markForRead();

  hkReal tolerance = 0.0f;
  for (int i=0; i < m_simulatedRigidBodies.Count(); ++i)
  {
    const hkpRigidBody* r = m_simulatedRigidBodies.GetAt(i)->GetHkRigidBody();
    hkAabb bodyBounds;
    r->getCollidable()->getShape()->getAabb( r->getTransform(), tolerance, bodyBounds );
    bounds.includeAabb( bodyBounds );
    // haveBounds = true; just dynamic rbs not enough to set bounds (but are in it)
  }

  for (int i = 0; i < m_simulatedRagdolls.Count(); ++i)
  {
    hkvAlignedBBox bbox;
    m_simulatedRagdolls.GetAt(i)->GetOwnerEntity()->GetCurrentVisBoundingBox(bbox);
    hkAabb bodyBounds;
    vHavokConversionUtils::VisVecToPhysVecWorld( bbox.m_vMin, bodyBounds.m_min );
    vHavokConversionUtils::VisVecToPhysVecWorld( bbox.m_vMax, bodyBounds.m_max );
    bounds.includeAabb(bodyBounds);
  }

  for (int i=0; i < m_simulatedStaticMeshes.Count(); ++i)
  {
    const hkpRigidBody* r = m_simulatedStaticMeshes.GetAt(i)->GetHkRigidBody();
    hkAabb bodyBounds;
    r->getCollidable()->getShape()->getAabb( r->getTransform(), tolerance, bodyBounds );
    bounds.includeAabb( bodyBounds );
    haveBounds = true;
  }

#ifdef SUPPORTS_TERRAIN
  for (int i=0; i < m_simulatedTerrainSectors.Count(); ++i)
  {
    const hkpRigidBody* r = m_simulatedTerrainSectors.GetAt(i)->GetHkRigidBody();
    hkAabb bodyBounds;
    r->getCollidable()->getShape()->getAabb( r->getTransform(), tolerance, bodyBounds );
    bounds.includeAabb( bodyBounds );
    haveBounds = true;
  }
#endif

  for (int i=0; i < m_simulatedTriggerVolumes.Count(); ++i)
  {
    const hkpRigidBody* r = m_simulatedTriggerVolumes.GetAt(i)->GetHkTriggerBody();
    hkAabb bodyBounds;
    r->getCollidable()->getShape()->getAabb( r->getTransform(), tolerance, bodyBounds );
    bounds.includeAabb( bodyBounds );
    // haveBounds = true; just keyframed trigger volumes not enough to set bounds (but are in it)
  }

  if (haveBounds)
  {
    hkVector4 he;
    bounds.getHalfExtents(he);
    const hkSimdReal smallestExt = he.horizontalMin<3>();
   haveBounds = smallestExt >= hkSimdReal::fromFloat(BROADPHASE_SIZE_TOLERANCE); // World needs a proper ext in all dirs
  }

  if(m_pPhysicsWorld)
    m_pPhysicsWorld->unmarkForRead();

  if(haveBounds)
  {
    worldBounds = bounds;
    return true;
  }

  return false;
}

void vHavokPhysicsModule::hkpWorldRemovePhysicsSystem(hkpWorld* pWorld, hkpPhysicsSystem* pSystem)
{
  // Hotfix / workaround:
  // This function is basically a copy of hkpWorld::removePhysicsSystem with the difference that
  // actions are removed before constraints, which avoids an assert that would happen
  // if a constraint chain is contained in the physics system

  HK_ACCESS_CHECK_OBJECT( pWorld, HK_ACCESS_RW );

  // actions & contraints
  for (int a=0; a < pSystem->getActions().getSize(); ++a)
  {
    if(pSystem->getActions()[a])
    {
      pWorld->removeAction(pSystem->getActions()[a]);
    }
  }
  for (int c=0; c < pSystem->getConstraints().getSize(); ++c)
  {
    // Only remove constraints that have an owner.
    if(pSystem->getConstraints()[c])
    {
      pWorld->removeConstraint(pSystem->getConstraints()[c]);
    }
  }

  // Rigid bodies
  {
    // Check for any HK_NULL entries in rigid body array.  If we have a HK_NULL entry
    // we cannot remove the rigid bodies as a batch and must remove them individually.
    if(pSystem->getRigidBodies().indexOf(HK_NULL) == -1)
    {
      pWorld->removeEntityBatch( (hkpEntity*const*)( pSystem->getRigidBodies().begin() ), pSystem->getRigidBodies().getSize() );
    }
    else
    {
      for(hkInt32 i = 0; i < pSystem->getRigidBodies().getSize(); ++i)
      {
        if(pSystem->getRigidBodies()[i] != HK_NULL)
        {
          pWorld->removeEntity(pSystem->getRigidBodies()[i]);
        }
      }
    }
  }

  // Phantoms
  {
    // Check for any HK_NULL entries in phantom array.  If we have a HK_NULL entry
    // we cannot remove the phantoms as a batch and must remove them individually.
    if(pSystem->getPhantoms().indexOf(HK_NULL) == -1)
    {
      pWorld->removePhantomBatch(pSystem->getPhantoms().begin(), pSystem->getPhantoms().getSize());
    }
    else
    {
      HK_WARN(0x31a2b8a3, "hkPhysicsSystem contains a HK_NULL phantom.  Cannot add phantoms as batch.  Adding individually.");

      for(hkInt32 i = 0; i < pSystem->getPhantoms().getSize(); ++i)
      {
        if(pSystem->getPhantoms()[i] != HK_NULL)
        {
          pWorld->removePhantom(pSystem->getPhantoms()[i]);
        }
      }
    }
  }
}

void vHavokPhysicsModule::SetBroadphaseSize(const hkAabb &worldBounds)
{
  // Apply new broadphase size to physics world.
  m_pPhysicsWorld->lock();

  hkpPhysicsSystem* entireSystem = m_pPhysicsWorld->getWorldAsOneSystem();

  // Temporarily remove the connection between rigid bodies and their corresponding Havok
  // Destruction breakable bodies to prevent hkdAddRemoveBodyForwarder::entityRemovedCallback()
  // from being called which would result in a lot of additional and unnecessary work.
  const hkArray<hkpRigidBody*>& rigidBodies = entireSystem->getRigidBodies();
  hkArray<hkpBreakableBody*> breakableBodies( rigidBodies.getSize() );
  for (int i = 0; i < breakableBodies.getSize(); i++)
  {
    breakableBodies[i] = rigidBodies[i]->m_breakableBody;
    rigidBodies[i]->m_breakableBody = HK_NULL;
  }

  // Make sure we prevent entityRemovedCallback() being called for other sources, e.g. vHavokClothPlugin
  for (int i = 0; i < m_entityListenersToIgnore.GetSize(); i++)
  {
    m_pPhysicsWorld->removeEntityListener( m_entityListenersToIgnore[i] );
  }

  // Remove all rigid bodies/phantoms/constraints/actions from the physics world
  // Use this function to mimic hkpWorld::removePhysicsSystem (with a change that first removes actions, then constraints)
  hkpWorldRemovePhysicsSystem(m_pPhysicsWorld, entireSystem);
  // m_pPhysicsWorld->removePhysicsSystem(entireSystem); -> should be this line as soon as [HVK-6398] is resolved

  // Add back the entity listeners we temporarily removed above
  for (int i = 0; i < m_entityListenersToIgnore.GetSize(); i++)
  {
    m_pPhysicsWorld->addEntityListener( m_entityListenersToIgnore[i] );
  }

  // Restore the breakable body points
  for (int i = 0; i < breakableBodies.getSize(); i++)
  {
    rigidBodies[i]->m_breakableBody = breakableBodies[i];
  }

  hkp3AxisSweep* childBroadPhase = HK_NULL;
  if (m_pPhysicsWorld->m_broadPhaseType == hkpWorldCinfo::BROADPHASE_TYPE_SAP)
  {
    hkp3AxisSweep* threeAxisBroadPhase = (hkp3AxisSweep*)m_pPhysicsWorld->getBroadPhase();
    threeAxisBroadPhase->m_aabb = worldBounds;
  }
  else if (m_pPhysicsWorld->m_broadPhaseType == hkpWorldCinfo::BROADPHASE_TYPE_HYBRID)
  {
    hkpTreeBroadPhase* treeBroadPhase = (hkpTreeBroadPhase*)m_pPhysicsWorld->getBroadPhase();
    childBroadPhase = (hkp3AxisSweep*)treeBroadPhase->getChildBroadPhase();
    childBroadPhase->m_aabb = worldBounds;
  }

  m_pPhysicsWorld->m_broadPhaseExtents[0] = worldBounds.m_min;
  m_pPhysicsWorld->m_broadPhaseExtents[1] = worldBounds.m_max;
  hkpBroadPhaseBorder *pBorder = new hkpBroadPhaseBorder(m_pPhysicsWorld, hkpWorldCinfo::BROADPHASE_BORDER_DO_NOTHING);
  m_pPhysicsWorld->setBroadPhaseBorder(pBorder);
  pBorder->removeReference();

  //
  // Calculate some basic values needed for 32bit AABBs
  //
  {
    hkpProcessCollisionInput* pci = m_pPhysicsWorld->getCollisionInputRw();

    hkVector4 span;		span.setSub( worldBounds.m_max, worldBounds.m_min );
    hkVector4 spanInv;	spanInv.setReciprocal(span); spanInv.setComponent<3>(hkSimdReal_1);

    pci->m_aabb32Info.m_bitScale.setMul( hkSimdReal::fromFloat(hkAabbUtil::AABB_UINT32_MAX_FVALUE), spanInv );
    pci->m_aabb32Info.m_bitOffsetLow.setNeg<4>( worldBounds.m_min );
    hkVector4 rounding; rounding.setMul( hkSimdReal::fromFloat(1.0f/hkAabbUtil::AABB_UINT32_MAX_FVALUE), span);
    pci->m_aabb32Info.m_bitOffsetHigh.setAdd(pci->m_aabb32Info.m_bitOffsetLow, rounding);

    pci->m_aabb32Info.m_bitScale.zeroComponent<3>();
    pci->m_aabb32Info.m_bitOffsetLow.zeroComponent<3>();
    pci->m_aabb32Info.m_bitOffsetHigh.zeroComponent<3>();

    m_pPhysicsWorld->getBroadPhase()->set32BitOffsetAndScale(pci->m_aabb32Info.m_bitOffsetLow, pci->m_aabb32Info.m_bitOffsetHigh, pci->m_aabb32Info.m_bitScale);

    if (childBroadPhase)
    {
      childBroadPhase->set32BitOffsetAndScale(pci->m_aabb32Info.m_bitOffsetLow, pci->m_aabb32Info.m_bitOffsetHigh, pci->m_aabb32Info.m_bitScale);
    }
  }

  // Re-add rigid bodies/phantoms/constraints/actions back into the world
  m_pPhysicsWorld->addPhysicsSystem(entireSystem);
  entireSystem->removeReference();

  m_pPhysicsWorld->unlock();
}

bool vHavokPhysicsModule::EnsureValidSolverBufferSize()
{
  VASSERT(m_pPhysicsWorld != NULL);
  m_pPhysicsWorld->markForWrite();

  // get required solver buffer size
  hkWorldMemoryAvailableWatchDog::MemUsageInfo memInfo;
  m_pPhysicsWorld->calcRequiredSolverBufferSize(memInfo);

  m_pPhysicsWorld->unmarkForWrite();

  // retrieve solver allocator and check if it meets the requirements
  hkSolverAllocator& solver =
    static_cast<hkSolverAllocator&>(hkMemoryRouter::getInstance().solver());
  if (memInfo.m_sumRuntimeBlockSize <= solver.getBufferSize())
    return true;

#ifdef HK_DEBUG_SLOW
  // check no memory is in use anymore
  hkMemoryAllocator::MemoryStatistics memStats;
  solver.getMemoryStatistics(memStats);
  VASSERT(memStats.m_inUse == 0);
#endif

  // allocate new buffer
  int iNewBufferSize = hkvMath::Max(solver.getBufferSize() + m_iMinSolverBufferIncreaseStep,
    memInfo.m_sumRuntimeBlockSize);

  void *pNewBuffer = gAllocator.blockAlloc(iNewBufferSize);
  if (pNewBuffer == NULL)
    return false;

  gAllocator.blockFree(solver.m_bufferStart, solver.getBufferSize());
  solver.setBuffer(pNewBuffer, iNewBufferSize);

  hkvLog::Info("Warning: Solver buffer size increased to %i Bytes.", iNewBufferSize);
  return true;
}

static hkpWorldCinfo::SolverType _getSolverType(int iters, int hardness)
{
  if (iters < 4)
  {
    return (hardness < 1) ? hkpWorldCinfo::SOLVER_TYPE_2ITERS_SOFT : ( (hardness < 2) ? hkpWorldCinfo::SOLVER_TYPE_2ITERS_MEDIUM : hkpWorldCinfo::SOLVER_TYPE_2ITERS_HARD );
  }
  else if (iters < 8)
  {
    return (hardness < 1) ? hkpWorldCinfo::SOLVER_TYPE_4ITERS_SOFT : ( (hardness < 2) ? hkpWorldCinfo::SOLVER_TYPE_4ITERS_MEDIUM : hkpWorldCinfo::SOLVER_TYPE_4ITERS_HARD );
  }
  // 8 or higher iters
  return (hardness < 1) ? hkpWorldCinfo::SOLVER_TYPE_8ITERS_SOFT : ( (hardness < 2) ? hkpWorldCinfo::SOLVER_TYPE_8ITERS_MEDIUM : hkpWorldCinfo::SOLVER_TYPE_8ITERS_HARD );
}

void vHavokPhysicsModule::CreateWorld()
{
  // Create the physics world
  hkpWorldCinfo worldInfo;

  worldInfo.setupSolverInfo( _getSolverType( g_vGlobalWorldParams.m_runtimeParams.m_solverIterations, g_vGlobalWorldParams.m_runtimeParams.m_solverHardness) );

  worldInfo.m_simulationType = m_pJobQueue ? hkpWorldCinfo::SIMULATION_TYPE_MULTITHREADED : hkpWorldCinfo::SIMULATION_TYPE_CONTINUOUS;
  vHavokConversionUtils::VisVecToPhysVecLocal(g_vGlobalWorldParams.m_runtimeParams.m_vGravity, worldInfo.m_gravity);    // default value for our scene scaling
  worldInfo.m_broadPhaseBorderBehaviour = hkpWorldCinfo::BROADPHASE_BORDER_DO_NOTHING; // just fix the entity if the object falls off too far
  worldInfo.m_broadPhaseType = hkpWorldCinfo::BROADPHASE_TYPE_TREE;

  worldInfo.m_useCompoundSpuElf = !vHavokPhysicsModule_GetDefaultWorldRuntimeSettings().m_bEnableLegacyCompoundShapes; // use the new bvcompressed and staticinstancedcompressed shapes

#ifdef _VISION_MOBILE
  worldInfo.m_minDesiredIslandSize = 8; // defaults to 64, which on mobile can be a little wasteful of cpus
#endif

  // You must specify the size of the broad phase - objects should not be simulated outside this region
  {
    if (g_vGlobalWorldParams.m_runtimeParams.m_bBroadphaseAutoCompute)
    {
    hkAabb worldBounds;
    worldBounds.setEmpty();

      if(!AutoComputeBroadphaseSize(worldBounds))
      {
        /*! Scene is not loaded yet, just set to something huge and shrink after scene is loaded callback */
        worldBounds.setFromCenterRadius(hkVector4(0,0,0,0), 12000000.f );
      }

      worldInfo.m_broadPhaseWorldAabb = worldBounds;
    }
    else
    {
      hkVector4 centerPoint;
      centerPoint.setZero();

      hkSimdReal halfLength = vHavokConversionUtils::GetVision2HavokScaleSIMD() * hkSimdReal::fromFloat(g_vGlobalWorldParams.m_runtimeParams.m_fBroadphaseManualSize) * hkSimdReal_Half;
      hkAabb bounds;
      bounds.setFromCenterRadius( centerPoint, halfLength );
      worldInfo.m_broadPhaseWorldAabb = bounds;
    }
  }

  // Give the application a chance to modify the Havok setup
  vHavokBeforeWorldCreateDataObject_cl worldConfig(&vHavokPhysicsModule::OnBeforeWorldCreated, this, worldInfo);
  vHavokPhysicsModule::OnBeforeWorldCreated.TriggerCallbacks(&worldConfig);

  m_pPhysicsWorld = new hkpWorld(worldInfo);

  // Since we are using different allocators in debug-/ release-builds, it can happen that the solver buffer run out of
  // memory only in release builds. In this case Havok will hit a HK_BREAKPOINT and the user will not know what the
  // actual reason was. Therefore disable assert and instead check m_pPhysicsWorld->initMtStep() result each step;
  // warn the user if solver buffer is too small.
  m_pPhysicsWorld->m_assertOnRunningOutOfSolverMemory = false;

  m_pPhysicsWorld->markForWrite();

  // Add the event listeners to the physics world
  m_spConstraintListener = new vHavokConstraintListener(m_pPhysicsWorld);
  m_spContactListener = new vHavokContactListener(m_pPhysicsWorld);

   // Add default collision filter object. We need a constraint collision filter for Destruction scenes using the flexible joint controller.
   {
      hkpGroupFilter* pGroupFilter = new hkpGroupFilter();
      hkpConstraintCollisionFilter* pConstraintFilter = HK_NULL;

      // Enable constraint filter if required
      const BOOL enableConstraintFilter = vHavokPhysicsModule_GetDefaultWorldRuntimeSettings().m_bEnableConstraintCollisionFilter;
      if ( enableConstraintFilter )
      {
         pConstraintFilter = new hkpConstraintCollisionFilter(pGroupFilter);
         pConstraintFilter->updateFromWorld(m_pPhysicsWorld);
      }

      // first disable collisions between all groups (group 0 should not be disabled, see docs)
      pGroupFilter->disableCollisionsUsingBitfield(0xfffffffe, 0xfffffffe);

      // then enable collisions, which are set in vHavokPhysicsModuleDefaultWorldParams
      for (int i=1;i<32;i++)
      {
         unsigned int iGroup = 1 << i;
         pGroupFilter->enableCollisionsUsingBitfield(iGroup, g_vGlobalWorldParams.m_runtimeParams.m_iCollisionGroupMasks[i]);
      }

      // Set filters in the world
      hkpCollisionFilter* pMainFilter = pConstraintFilter ? (hkpCollisionFilter*)pConstraintFilter : (hkpCollisionFilter*)pGroupFilter;
      m_pPhysicsWorld->setCollisionFilter(pMainFilter);
      pGroupFilter->removeReference();
      if ( pConstraintFilter) { pConstraintFilter->removeReference();	}
   }

  // Register all collision agents, even though only box - box will be used in this particular example.
  // It's important to register collision agents before adding any entities to the world.
  hkpAgentRegisterUtil::registerAllAgents( m_pPhysicsWorld->getCollisionDispatcher() );

  m_pPhysicsWorld->unmarkForWrite();

  CreateCachedBodies();

#if defined(_VISION_WIN32)
  // Always enable VDB, when working inside vForge
  m_bVisualDebugger |= Vision::Editor.IsInEditor();
#endif
  
  SetEnabledVisualDebugger(m_bVisualDebugger);
  SetEnabledDebug(m_bDebugDisplay);

  vHavokPhysicsModuleCallbackData newWorld(&vHavokPhysicsModule::OnAfterWorldCreated, this);
  vHavokPhysicsModule::OnAfterWorldCreated.TriggerCallbacks(&newWorld);
}

void vHavokPhysicsModule::CreateCachedBodies()
{
  VASSERT(m_pPhysicsWorld!= NULL);
  m_pPhysicsWorld->markForWrite();

 {
   for (int i=0; i < m_simulatedRigidBodies.Count(); ++i)
   {
     m_pPhysicsWorld->addEntity(m_simulatedRigidBodies.GetAt(i)->GetHkRigidBody());
   }
 }
 {
   for (int i=0; i < m_simulatedStaticMeshes.Count(); ++i)
   {
    m_pPhysicsWorld->addEntity(m_simulatedStaticMeshes.GetAt(i)->GetHkRigidBody());
   }
 }
#ifdef SUPPORTS_TERRAIN
 {
   for (int i=0; i < m_simulatedTerrainSectors.Count(); ++i)
   {
    m_pPhysicsWorld->addEntity(m_simulatedTerrainSectors.GetAt(i)->GetHkRigidBody());
   }
 }
#endif

 {
   for (int i=0; i < m_simulatedConstraints.GetLength(); ++i)
   {
     hkpConstraintInstance *pConstraintInstance = m_simulatedConstraints.Get(i)->GetHkConstraintInstance();
     m_pPhysicsWorld->addConstraint(pConstraintInstance);
   }
 }

 {
   for (int i=0; i < m_simulatedControllers.Count(); ++i)
   {
     m_pPhysicsWorld->addPhantom( m_simulatedControllers.GetAt(i)->GetCharacterProxy()->getShapePhantom() );
   }
 }

#ifdef SUPPORTS_TERRAIN
 {
   for (int i=0; i < m_simulatedDecorations.Count(); ++i)
   {
     m_pPhysicsWorld->addEntity( m_simulatedDecorations.GetAt(i)->GetHkRigidBody() );
   }
 }
#endif

 {
   for (int i=0; i < m_simulatedTriggerVolumes.Count(); ++i)
   {
     vHavokTriggerVolume *pTriggerVolume = m_simulatedTriggerVolumes.GetAt(i);
     hkpRigidBody *pRigidBody = pTriggerVolume->GetHkTriggerBody();
     m_pPhysicsWorld->addEntity(pRigidBody);
   }
 }

 m_pPhysicsWorld->unmarkForWrite();
}

void vHavokPhysicsModule::StartSimulation(float dt)
{
  if (!m_bSimulationStarted)
  {
    m_bSimulationStarted = true;
    m_bResultsExpected = false;

    if (!m_pPhysicsWorld)
      CreateWorld();

    {
      if (m_pRayCasterSingle)
        m_pRayCasterSingle->Execute();
      if (m_pRayCasterMultiple)
        m_pRayCasterMultiple->Execute();

      // Init threads for this simulation step and check if solver buffer memory is big enough.
      hkpStepResult stepResult = DoStep(dt);

      if (stepResult == HK_STEP_RESULT_MEMORY_FAILURE_BEFORE_INTEGRATION)
      {
        EnsureValidSolverBufferSize();

        // try again
        stepResult = DoStep(dt);
      }
      VASSERT_MSG(stepResult == HK_STEP_RESULT_SUCCESS, "Physics initialization step failed.");
    }
  }
}

hkpStepResult vHavokPhysicsModule::DoStep(float dt)
{
  if (IsMultithreaded())
  {
    m_pPhysicsWorld->checkUnmarked();

    const hkpStepResult result = m_pPhysicsWorld->initMtStep(m_pJobQueue, dt);

    if (result == HK_STEP_RESULT_SUCCESS)
    {
      // Get the thread pool working on the jobs (this thread will continue without 
      // doing Physics work until Wait is called later on).
      m_pThreadPool->processJobQueue(m_pJobQueue);
    }

    return result;
  }
  else
  {
    return m_pPhysicsWorld->stepDeltaTime(dt);
  }
}

void vHavokPhysicsModule::WaitForForegroundSimulationToComplete()
{
  if (m_bSimulationStarted)
  {
    m_bSimulationStarted = false;
    m_bResultsExpected = true;
#if (HK_CONFIG_THREAD == HK_CONFIG_MULTI_THREADED) && !defined(HAVOK_FORCE_SINGLETHREADED)  // Will be on PC, PS3 etc.
    if ((m_pPhysicsWorld->m_simulationType == hkpWorldCinfo::SIMULATION_TYPE_MULTITHREADED) && (m_pThreadPool != HK_NULL))
    {
      m_pPhysicsWorld->checkUnmarked();
      m_pJobQueue->processAllJobs(); // do some jobs ourself
      m_pThreadPool->waitForCompletion(); // wait for all remaining threads
      m_pPhysicsWorld->finishMtStep( m_pJobQueue, m_pThreadPool );
    }

#endif
    // else nothing to do as all done on start
  }
}

void vHavokPhysicsModule::WaitForSimulationToComplete()
{
  if (m_bCurrentRunIsAsync)
    Vision::GetThreadManager()->WaitForTask(m_pTask, true);
  else
    WaitForForegroundSimulationToComplete();
}

BOOL vHavokPhysicsModule::OnDeInitPhysics()
{
  WaitForSimulationToComplete();

  {
    vHavokPhysicsModuleCallbackData callbackData(&OnBeforeDeInitializePhysics, this);
    callbackData.Trigger();
  }

  if (m_pPhysicsWorld != NULL)
  {
    m_pPhysicsWorld->markForWrite();
  }

  // If we have registered steppers, notify them about the world getting deleted
  const int numSteppers = m_steppers.GetSize();
  for (int k = numSteppers - 1; k >= 0; k--)
  {
    IHavokStepper* stepper = m_steppers[k];
    stepper->OnDeInitPhysicsModule();
  }

  vHavokConstraint::ElementManagerDeleteAll();
  vHavokConstraintChain::ElementManagerDeleteAll();
  m_ResourceShapes.DisposeAll();
  VASSERT(m_ResourceShapesToInstantiate.Count()==0);

  // Unregister our Vision callbacks
  Vision::Callbacks.OnBeforeSceneLoaded -= this;
  Vision::Callbacks.OnAfterSceneLoaded -= this;
  Vision::Callbacks.OnWorldDeInit -= this;
  IVisSceneManager_cl::OnReposition -= this;
  if(Vision::Editor.IsInEditor())
    Vision::Callbacks.OnAfterSceneExported -= this;

  // Deinitialize binding wrappers
  {
    // Dispose all simulated static meshes, so that they get removed from the
    // module and releases their Havok data.
    const int iStaticMeshCount = m_simulatedStaticMeshes.Count();
    for (int i = iStaticMeshCount - 1; i >= 0; i--)
      m_simulatedStaticMeshes.GetAt(i)->DisposeObject();

#ifdef SUPPORTS_TERRAIN
    const int iTerrainCount = m_simulatedTerrainSectors.Count();
    for (int i = iTerrainCount - 1; i >= 0; i--)
      m_simulatedTerrainSectors.GetAt(i)->DisposeObject();
#endif

    // Dispose all simulated rigid bodies, so that they get removed from the
    // module and releases their Havok data.
    // Usually there won't be any rigid bodies left, unless the physics module
    // is deinitialized although an owner entity is still alive.
    const int iRigidBodyMeshCount = m_simulatedRigidBodies.Count();
    for (int i = iRigidBodyMeshCount - 1; i >= 0; i--)
      m_simulatedRigidBodies.GetAt(i)->DisposeObject();

    const int iRagdollCount = m_simulatedRagdolls.Count();
    for (int i = iRagdollCount - 1; i >= 0; i--)
      m_simulatedRagdolls.GetAt(i)->DisposeObject();

    const int iControllerCount = m_simulatedControllers.Count();
    for (int i = iControllerCount - 1; i >= 0; i--)
      m_simulatedControllers.GetAt(i)->DisposeObject();

    const int iConstCount = m_simulatedConstraints.GetLength();
    for (int i = iConstCount - 1; i >= 0; i--)
      m_simulatedConstraints.Get(i)->DisposeObject();

#ifdef SUPPORTS_TERRAIN
    const int iDecCount = m_simulatedDecorations.Count();
    for (int i = iDecCount - 1; i >= 0; i--)
      m_simulatedDecorations.GetAt(i)->DisposeObject();
#endif

    const int iTriggerVolumeCount = m_simulatedTriggerVolumes.Count();
    for (int i = iTriggerVolumeCount - 1; i >= 0; i--)
      m_simulatedTriggerVolumes.GetAt(i)->DisposeObject();

    const int iBlockerVolumeCount = m_simulatedBlockerVolumes.Count();
    for (int i = iBlockerVolumeCount - 1; i >= 0; i--)
      m_simulatedBlockerVolumes.GetAt(i)->DisposeObject();

    m_ResourceShapes.DisposeAll();
    VASSERT(m_ResourceShapesToInstantiate.Count()==0);
  }


  // Clean up visual debugger before local shape Cache as the VDB shape hash can keep a reference
  m_spVisualDebugger = NULL;

  // Clear Havok shape cache
  if (!vHavokShapeCache::Clear())
  {
     HK_WARN_ALWAYS(0x3123f4,"Could not safely clear the cache. You probably have held onto rigid biody handles past quit. This is very unsafe.");
  }

  // Deinitialize shape factory
  vHavokShapeCache::DeInit();

  // Cleanup the Havok internals

  // Clean up havok display handler
  m_spDisplayHandler = NULL;

  V_SAFE_DELETE(m_pRayCasterSingle);
  V_SAFE_DELETE(m_pRayCasterMultiple);

  // Clean up physics
  if (m_pPhysicsWorld)
  {
    //m_pPhysicsWorld->unmarkForWrite(); // leads to an assertion
    m_pPhysicsWorld->removeReference();
    m_pPhysicsWorld = NULL;
  }

  m_entityListenersToIgnore.RemoveAll();

  // Clean up havok listeners
  if (m_spConstraintListener != NULL)
    m_spConstraintListener = NULL;
  if (m_spContactListener != NULL)
    m_spContactListener = NULL;

#if (HK_CONFIG_THREAD == HK_CONFIG_MULTI_THREADED) && !defined(HAVOK_FORCE_SINGLETHREADED)

    if (m_pThreadPool)
    {
      m_pThreadPool->removeReference();
      m_pThreadPool = HK_NULL;
    }

    delete m_pJobQueue;
    m_pJobQueue = HK_NULL;

    delete m_pTaskQueue;
    m_pTaskQueue = HK_NULL;

    delete m_pProfiler;
    m_pProfiler = NULL;

    if (m_pSpuUtil)
    {
      // free SPURS resources if necessary (if Vision not doing that for us)
  #if defined(HK_PLATFORM_HAS_SPU)
      m_pSpuUtil->removeReference();
      m_pSpuUtil = HK_NULL;
  #endif
    }

#endif // MT enabled

  V_SAFE_DELETE(m_pSweepSemaphore);

  // Mostly used for plugin DLLs to release all their Havok heap allocated data prior to hkbase quitting
  {
    vHavokPhysicsModuleCallbackData callbackData(&OnAfterDeInitializePhysics, this);
    callbackData.Trigger();
  }

  // allow all plugins to unsync statics - this is not done in the callbacks above due to cross dependence between plugins
  // The cross dependence is introduced by multiply re-registering classes in each DLL.
  {
    // Unsync statics and remove thread local memory routers.
    OnUnsyncStatics.TriggerCallbacks();
    vHavokDeleteMemoryRouterTask task;
    PerformTaskOnAllThreads(&task);

    VManagedThread::OnThreadLocalVariableInit -= this;
    VManagedThread::OnThreadLocalVariableDeInit -= this;
  }

  hkCheckDeterminismUtil::destroyInstance();

  // Quit the Havok subsystems
  hkBaseSystem::quit();

  hkMemoryInitUtil::quit();

  return TRUE;
}


vHavokPhysicsModule *vHavokPhysicsModule::GetInstance()
{
  IVisPhysicsModule_cl *pModule = Vision::GetApplication()? Vision::GetApplication()->GetPhysicsModule() : NULL;

  if (!pModule)
    return NULL;

  // Check whether the Havok module is installed
  if (pModule->GetType() != HAVOK)
    return NULL;

  return (vHavokPhysicsModule *) pModule;
}


// -------------------------------------------------------------------------- //
// Module Configuration                                                       //
// -------------------------------------------------------------------------- //

void vHavokPhysicsModule::SetVisionWorldScale(hkReal fUnitsPerMeter)
{
  vHavokConversionUtils::SetHavok2VisionScale(fUnitsPerMeter);
}

hkReal vHavokPhysicsModule::GetVisionWorldScale() const
{
  return vHavokConversionUtils::GetHavok2VisionScale();
}

void vHavokPhysicsModule::SetStaticGeometryMode(StaticGeomMode mode)
{
  m_staticGeomMode = mode;
}

bool vHavokPhysicsModule::GetUseAsynchronousPhysics()
{
  return m_bAsyncPhysics;
}

void vHavokPhysicsModule::SetUseAsynchronousPhysics(bool bAsyncPhysics)
{
  m_bAsyncPhysics = bAsyncPhysics;
}

void vHavokPhysicsModule::SetThreadCount(unsigned int iNumThreads)
{
  if (m_pThreadPool)
  {
    iNumThreads = hkvMath::Min(iNumThreads, g_iHavokMaxThreads); // Limit by the number of allowable HAVOK threads
    m_pThreadPool->setNumThreads(iNumThreads);
  }
}

void vHavokPhysicsModule::EnableDebugRendering(bool bRigidBodies, bool bRagdolls, bool bCharacterControllers,
  bool bTriggerVolumes, bool bBlockerVolumes, bool bStaticMeshes)
{
  m_bDebugRenderRigidBodies = bRigidBodies;
  m_bDebugRenderRagdolls = bRagdolls;
  m_bDebugRenderCharacterControllers = bCharacterControllers;
  m_bDebugRenderTriggerVolumes = bTriggerVolumes;
  m_bDebugRenderBlockerVolumes = bBlockerVolumes;
  m_bDebugRenderStaticMeshes = bStaticMeshes;

  // will update debug rendering, depending on the global flags

  for (int i = 0; i < m_simulatedStaticMeshes.Count(); ++i)
    m_simulatedStaticMeshes.GetAt(i)->SetDebugRendering (m_bDebugRenderStaticMeshes);

  for (int i = 0; i < m_simulatedRigidBodies.Count(); ++i)
    m_simulatedRigidBodies.GetAt(i)->SetDebugRendering (m_simulatedRigidBodies.GetAt(i)->Debug_Render);

  for (int i = 0; i < m_simulatedRagdolls.Count(); ++i)
    m_simulatedRagdolls.GetAt(i)->SetDebugRendering(m_simulatedRagdolls.GetAt(i)->GetDebugRenderEnabled());

  for (int i = 0; i < m_simulatedControllers.Count(); ++i)
    m_simulatedControllers.GetAt(i)->SetDebugRendering (m_simulatedControllers.GetAt(i)->GetDebugRenderEnabled());

  for (int i = 0; i < m_simulatedTriggerVolumes.Count(); ++i)
    m_simulatedTriggerVolumes.GetAt(i)->SetDebugRendering (m_simulatedTriggerVolumes.GetAt(i)->GetDebugRenderEnabled());

  for (int i = 0; i < m_simulatedBlockerVolumes.Count(); ++i)
    m_simulatedBlockerVolumes.GetAt(i)->SetDebugRendering(m_simulatedBlockerVolumes.GetAt(i)->GetDebugRenderEnabled());
}

void vHavokPhysicsModule::SaveAllCachedShapes(bool bOnlyModified)
{
  if (!vHavokPhysicsModule_GetDefaultWorldRuntimeSettings().m_bEnableDiskShapeCaching)
    return;

  WaitForSimulationToComplete();

#if defined(HK_DEBUG)
  if (bOnlyModified && !vHavokShapeCache::IsHktUpToDateCheckingEnabled())
  {
    hkvLog::Warning("Calling vHavokPhysicsModule::SaveAllCachedShapes with bOnlyModified = true, but up-to-date checking is disabled.");
  }
#endif

  vHavokShapeCache::SaveAllShapesToDisk(bOnlyModified);
}

// -------------------------------------------------------------------------- //
// Simulation Update                                                          //
// -------------------------------------------------------------------------- //

void vHavokPhysicsModule::OnRunPhysics(float fDuration)
{
  if (m_bUpdateFilter)
  {
    m_pPhysicsWorld->lock();
    m_pPhysicsWorld->updateCollisionFilterOnWorld(HK_UPDATE_FILTER_ON_WORLD_FULL_CHECK, HK_UPDATE_COLLECTION_FILTER_IGNORE_SHAPE_COLLECTIONS);
    m_pPhysicsWorld->unlock();
    m_bUpdateFilter = false;
  }

  // Skip step when simulation is paused or is being stepped externally
  if (m_bPaused || m_steppedExternally)
    return;

  // Update pending duration since last time step
  m_fAccumulatedTime += fDuration;

  // Configure this frame for variable time stepping.
  if (m_bAllowVariableStepRate)
  {
    if (m_fMaxTimeStep > 0.0f && m_fAccumulatedTime > m_fMaxTimeStep)
    {
      m_iNumTicksThisFrame = static_cast<int>(m_fAccumulatedTime / m_fMaxTimeStep);
      m_fTimeStep = m_fMaxTimeStep;
    }
    else if (m_fAccumulatedTime < m_fMinTimeStep)
    {
      m_iNumTicksThisFrame = 0;
      m_fTimeStep = m_fMinTimeStep;
    }
    else
    {
      m_iNumTicksThisFrame = 1;
      m_fTimeStep = m_fAccumulatedTime;
    }
  }
  // Configure this frame for fixed time stepping.
  else
  {
    if (m_bFixedTicksPerFrame)
    {
    m_iNumTicksThisFrame = m_iMaxTicksPerFrame;
    }
    else
    {
      m_iNumTicksThisFrame = static_cast<int>(m_fAccumulatedTime / m_fTimeStep);
    }
  }

  // Clamp number of ticks.
  if (m_iNumTicksThisFrame > m_iMaxTicksPerFrame)
  {
    // Remove time frame that will not be processed due to the clamping.
    // This is important for correctly handling motion delta in this case.
    m_fAccumulatedTime = hkvMath::Max(0.0f, m_fAccumulatedTime - static_cast<float>(m_iNumTicksThisFrame - m_iMaxTicksPerFrame) * m_fTimeStep);
  
    m_iNumTicksThisFrame = m_iMaxTicksPerFrame;
  }

  // Update character controller/ rigid bodies outside physics tick in order
  // to process motion delta.
  // Also note that since the entities are modified here which is not thread safe,
  // this code can't be executed from within the task that calls PerformSimulation.

  {
    // Need to step the controllers at actual frame dt as high frame rates otherwise
    // miss delta motion etc
    int iCount = m_simulatedControllers.Count();
    for (int i = 0; i < iCount; i++)
      m_simulatedControllers.GetAt(i)->Step(m_fTimeStep, m_iNumTicksThisFrame, m_fAccumulatedTime);

    const float fTotalSimulationTime = static_cast<float>(m_iNumTicksThisFrame) * m_fTimeStep;
    if (fTotalSimulationTime > 0.0f)
    {
      int iCount = m_simulatedRigidBodies.Count();
      for (int i = 0; i < iCount; i++)
        m_simulatedRigidBodies.GetAt(i)->Step(fTotalSimulationTime);
    }
  }

  if (g_bUseVisionTask && m_bAsyncPhysics && m_pTask)
  {
    m_bCurrentRunIsAsync = true;
    Vision::GetThreadManager()->ScheduleTask(m_pTask, 2);
  }
  else
  {
    m_bCurrentRunIsAsync = false;
    PerformSimulation(false, false);
  }

  m_bNewResultsAvailable = true;
}

void vHavokPhysicsModule::ClearVisualDebuggerTimerData()
{
  hkMonitorStream &monitorStream = hkMonitorStream::getInstance();
  if ( monitorStream.getCapacity() > (char*)0 )
  {
    monitorStream.reset();
  }

  if ( m_pThreadPool != HK_NULL )
  {
    m_pThreadPool->clearTimerData();
  }
}

void vHavokPhysicsModule::PerformSimulation(bool bTask, bool bWorkerThread)
{
  VISION_PROFILE_FUNCTION(PROFILING_HAVOK_PHYSICS_SIMULATION);

  if (m_ResourceShapesToInstantiate.Count()>0)
    ProcessLoadedResourceShapes();

  hkCheckDeterminismUtil::workerThreadStartFrame(!bWorkerThread);

  bool expectExternalTimerClear = !bWorkerThread && m_vdbSteppedExternally;
  if (!expectExternalTimerClear)
  {
    ClearVisualDebuggerTimerData();
  }

  int iNumTicksThisFrame = m_iNumTicksThisFrame;
  while (iNumTicksThisFrame > 0)
  {
    // Step all the 'steppers'
    // TODO: Thread this too.
    {
      int iCount = m_steppers.GetLength();
      for (int s =0; s < iCount; ++s)
        m_steppers[s]->Step(m_fTimeStep);
    }

    // Step physics
    StartSimulation(m_fTimeStep);

    if ((iNumTicksThisFrame > 1) || bTask)
    {
      // Not great that the main thread has to wait just because we are stepping more than once
      // Could place all of this extra looping in a thread too
      WaitForForegroundSimulationToComplete();
    }
    // other wise let the func return and we will wait later on

    // Decrease loop count
    iNumTicksThisFrame--;
  }

  // Update the accumulated time since the last simulation step.
  m_fAccumulatedTime = hkvMath::Max(0.0f, m_fAccumulatedTime - static_cast<float>(m_iNumTicksThisFrame) * m_fTimeStep);

  hkCheckDeterminismUtil::workerThreadFinishFrame();
}

void vHavokPhysicsModule::FetchPhysicsResults()
{
  VISION_PROFILE_FUNCTION(PROFILING_HAVOK_PHYSICS_FETCH_RESULTS);

  // Do nothing if no simulation was performed in this step
  // OnRunPhysics sets m_bNewResultsAvailable = true (unless not stepped or early exit for arbitrary tiny dt)
  if (!m_bNewResultsAvailable)
    return;
  m_bNewResultsAvailable = false;

  // Wait for the simulation to complete, so that results are available
  // StartSimulation (called if physics is stepped) sets m_bSimulationStarted = true
  // if m_bSimulationStarted == true, WaitForSimulationToComplete sets m_bSimulationStarted = false, m_bResultsExpected = true
  WaitForSimulationToComplete();


  // Dump the cached messages to the log
  // Don't wait for a physics step to update the log (catch startup messages, character step messages etc)
  DumpCachedMessagesToLog();

  if(m_bResultsExpected)
  {
    // Update the position and rotation in Vision
    // Will only have new data if m_bResultsExpected == true
    // (Keyframed Vision objects are updated to new animation positions even if m_bResultsExpected == false)
    UpdateHavok2Vision();
  }

  // Update the entities handled by character controllers
  // Can have new data if m_bNewResultsAvailable == true, even if m_bResultsExpected == false
  // We have to do this *after* synching rigid body changes to Vision in order to keep
  // parent data for those rigid bodies consistent. (Otherwise all sorts of errors creep into
  // local position and orientation offsets.)
  {
    int iCount = m_simulatedControllers.Count();
    for (int i = 0; i < iCount; i++)
      m_simulatedControllers.GetAt(i)->UpdateOwner();
  }

  if(m_bResultsExpected)
  {
      // Fire callbacks
      OnFetchPhysicsResults.TriggerCallbacks();

    SendEnqueuedNotifications();

    vHavokConstraint::RemoveBrokenConstraints();
    vHavokConstraintChain::RemoveBrokenConstraintChains();

    // Step visual debugger
    // Clear flag until next physics step
    m_bResultsExpected = false;
  }

  m_iNumTicksThisFrame = 0;

  if (!m_vdbSteppedExternally)
  {
    StepVisualDebugger();
  }
}

void vHavokPhysicsModule::StepVisualDebugger()
{
  // Step visual debugger
  #ifdef HK_DEBUG_SLOW
    hkMemorySystem::MemoryStatistics stats;
    hkMemorySystem::getInstance().getMemoryStatistics(stats);

    for (int i = 0 ; i < stats.m_entries.getSize(); ++i)
    {
        if ( hkString::strCmp(stats.m_entries[i].m_allocatorName, "Heap") == 0 )
        {
            hkMemoryAllocator::MemoryStatistics& heapStats = stats.m_entries[i].m_allocatorStats;
            HK_MONITOR_ADD_VALUE( "Mem in use", heapStats.m_inUse/1024.f, HK_MONITOR_TYPE_SINGLE );
            HK_MONITOR_ADD_VALUE( "Mem allocated", heapStats.m_allocated/1024.f, HK_MONITOR_TYPE_SINGLE );
            break;
        }
    }
  #endif

  if (m_bVisualDebugger)
    m_spVisualDebugger->Step();
}

void vHavokPhysicsModule::UpdateHavok2Vision()
{
  // Update rigid bodies
  {
    int iCount = m_simulatedRigidBodies.Count();
    for (int i = 0; i < iCount; i++)
    {
      m_simulatedRigidBodies.GetAt(i)->UpdateOwner();
    }
  }

  // Update rag dolls
  {
    int iCount = m_simulatedRagdolls.Count();
    for (int i = 0; i < iCount; i++)
    {
      m_simulatedRagdolls.GetAt(i)->UpdateOwner();
    }
  }
}

void vHavokPhysicsModule::SetPhysicsTickCount(int iTickCount,
  int iMaxTicksPerFrame, bool bFixedTicksPerFrame, float fMinPhysicsTimeStep, float fMaxPhysicsTimeStep)
{
  VASSERT(iTickCount >= 0);
  VASSERT(fMaxPhysicsTimeStep >= 0.0f);

  // fixed time stepping
  if (iTickCount != 0)
  {
    m_fTimeStep = (1.f / (float)iTickCount);
    m_iMaxTicksPerFrame = iMaxTicksPerFrame;
#ifdef _VISION_MOBILE
    if (m_iMaxTicksPerFrame > 1)
    {
      HK_WARN_ALWAYS(0x3123f3, "Setting Vision to step physics more that once on a mobile platform. Not usually a good use of resources.");
    }
#endif
    m_bFixedTicksPerFrame = bFixedTicksPerFrame;
    m_bAllowVariableStepRate = false;

    // Don't set min and max timestep to zero, since we want to keep these values in case
    // the normal SetPhysicsTickCount function from the IVisPhysicsModule_cl interface
    // gets called for enabling variable time stepping.
  }
  // variable time stepping
  else
  {
    m_iMaxTicksPerFrame = iMaxTicksPerFrame;
    m_fTimeStep = 0.f;
    m_bFixedTicksPerFrame = false;
    m_bAllowVariableStepRate = true;
    m_fMinTimeStep = fMinPhysicsTimeStep;
    m_fMaxTimeStep = fMaxPhysicsTimeStep;
  }
}

void vHavokPhysicsModule::SetPhysicsTickCount(int iTickCount, int iMaxTicksPerFrame)
{
  SetPhysicsTickCount(iTickCount, iMaxTicksPerFrame, false, m_fMinTimeStep, m_fMaxTimeStep);
}

#ifdef SUPPORTS_SNAPSHOT_CREATION

void vHavokPhysicsModule::GetDependenciesForObject(VResourceSnapshot &snapshot, VTypedObject *pInstance)
{
  VASSERT(pInstance != NULL);
  if (pInstance->IsOfType(V_RUNTIME_CLASS(VisStaticMeshInstance_cl)))
  {
    VisStaticMeshInstance_cl *pMeshInstance = (VisStaticMeshInstance_cl*) pInstance;

    // Please note that in case of Vision::Physics.GetStaticMeshTraceMeshBehavior()==VIS_USE_COLLISIONMESHES_FOR_TRACELINES,
    // Vision trace-lines will only use .vcolmesh for trace lines. However in that case UV tracking is no longer supported,
    // thus regular ray casts via the Havok physics plugin should be used anyway. On the other hand when UV tracking is required,
    // render meshes have to be used. Furthermore for the runtime side HKT shape files (without dependency to .vcolmesh) are required,
    // thus .vcolmesh are not added to the resource snapshot dependencies.
    // Ignore static meshes that have been marked as non collidable
    if (pMeshInstance->GetCollisionBehavior() != VisStaticMeshInstance_cl::VIS_COLLISION_BEHAVIOR_FORCENOCOLLISION &&
        pMeshInstance->GetPhysicsObject() != NULL)
    {
      vHavokShapeFactory::GetHktDependencies(snapshot, pMeshInstance);
    }
  }
  else if (pInstance->IsOfType(V_RUNTIME_CLASS(VisBaseEntity_cl)))
  {
    VisBaseEntity_cl *pEntity = (VisBaseEntity_cl*) pInstance;
    vHavokShapeFactory::GetHktDependencies(snapshot, pEntity);
  }
}
void vHavokPhysicsModule::GetDependenciesForResource(VResourceSnapshot &snapshot, VManagedResource *pResource)
{
  VASSERT(pResource != NULL);
  if (strcmp(pResource->GetParentManager()->GetManagerName(), "TerrainSectors") == 0)
  {
    VTerrainSector *pSector = (VTerrainSector*) pResource;

    // Ignore terrains that have been marked as non collidable
    if (pSector->GetTraceBitmask()!=0)  //For now we still use the trace flags for backwards compatibility
      vHavokShapeFactory::GetHktDependencies(snapshot, pSector);
  }
}

#endif

// -------------------------------------------------------------------------- //
// Raycast Callbacks                                                          //
// -------------------------------------------------------------------------- //
void vHavokPhysicsModule::ForwardRaycastData(VisPhysicsRaycastBase_cl *pRaycastData, const hkpWorldRayCastOutput* pHavokResult )
{
   hkvVec3 fullRay = pRaycastData->vRayEnd - pRaycastData->vRayStart;

   // Fill the Vision hit structure
   VisPhysicsHit_t hit;
   hit.fHitFraction = float(pHavokResult->m_hitFraction);
   hit.vImpactPoint = pRaycastData->vRayStart + (fullRay * float(pHavokResult->m_hitFraction));
   vHavokConversionUtils::PhysVecToVisVec_noscale( pHavokResult->m_normal, hit.vImpactNormal );

   const hkpWorldObject* hitObj = hkpGetWorldObject( pHavokResult->m_rootCollidable );
   vHavokUserDataType_e eType;
   hkUlong userData = hitObj->getUserData();
   void* pObject = vHavokUserDataPointerPair_t::ExtractTypeAndPointer((void *)userData, eType);
   hit.pHitObject = HK_NULL;
   switch (eType)
   {
   case V_USERDATA_OBJECT:
     {
       IVObjectComponent* havokComponent = static_cast<IVObjectComponent*>(pObject); // rigid body or character controller
       VASSERT(havokComponent->IsOfType(IVObjectComponent::GetClassTypeId()));
       VisTypedEngineObject_cl *pOwner = havokComponent->GetOwner();
       if (pOwner != NULL)
       {
         VASSERT(pOwner->IsOfType(V_RUNTIME_CLASS(VisObject3D_cl)));
         hit.pHitObject = (VisObject3D_cl *) pOwner;

         if(havokComponent->IsOfType(vHavokRigidBody::GetClassTypeId()))
         {
           hkpRigidBody* pRigidBody = static_cast<vHavokRigidBody*>(havokComponent)->GetHkRigidBody();
           // leave default value for static friction
           hit.hitMaterial.fDynamicFriction = float(pRigidBody->getFriction());
           hit.hitMaterial.fRestitution = float(pRigidBody->getRestitution());
         }
         else if (havokComponent->IsOfType(vHavokCharacterController::GetClassTypeId()))
         {
           const vHavokCharacterPushableProxy* pCharacterController = static_cast<const vHavokCharacterController*>(havokComponent)->GetCharacterProxy();
           hit.hitMaterial.fStaticFriction = float(pCharacterController->m_staticFriction);
           hit.hitMaterial.fDynamicFriction = float(pCharacterController->m_dynamicFriction);
           // leave default value for restitution
         }
         else if (havokComponent->IsOfType(vHavokRagdoll::GetClassTypeId()))
         {
         #ifdef HK_DEBUG_SLOW
           const hkClass* loadedClassType = hkVtableClassRegistry::getInstance().getClassFromVirtualInstance(hitObj);
           VASSERT(loadedClassType && hkString::strCmp(loadedClassType->getName(), "hkpRigidBody") == 0);
         #endif

           const hkpRigidBody* pRigidBody = static_cast<const hkpRigidBody*>(hitObj);
           // leave default value for static friction
           hit.hitMaterial.fDynamicFriction = float(pRigidBody->getFriction());
           hit.hitMaterial.fRestitution = float(pRigidBody->getRestitution());
         }
       }
       hit.eHitType = VIS_TRACETYPE_ENTITYPOLY;
     }
     break;

   case V_USERDATA_STATIC:
     {
       hit.eHitType = VIS_TRACETYPE_STATICGEOMETRY;

       const hkpShape *pShape = pHavokResult->m_rootCollidable->getShape();
       VASSERT(pShape);

       if (pShape->getClassType() == &hkvBvCompressedMeshShapeClass)
       {
         const hkvBvCompressedMeshShape *pMeshShape = (hkvBvCompressedMeshShape*)(pShape);
         const hkvMeshMaterialCache& matCache = pMeshShape->GetMaterialCache();
         if (!matCache.isEmpty())
         {
           hkpShapeKey key = pHavokResult->m_shapeKeys[0];
           for (int i=0;pHavokResult->m_shapeKeys[i]!=HK_INVALID_SHAPE_KEY;++i)
             key = pHavokResult->m_shapeKeys[i];

           hkUint32 matId = matCache.getSize() > 1 ? pMeshShape->getPrimitiveUserData( key ) : 0;
           VASSERT(matId <= (hkUint32)matCache.getSize());
           const hkvMeshMaterial& pMaterial = matCache[matId];

           hit.pHitObject = ((vHavokStaticMesh*)pObject)->GetWrappedStaticMesh(pMaterial.m_iOrignalSubMeshIndex);
           hit.hitMaterial.fDynamicFriction = pMaterial.m_fFriction;
           hit.hitMaterial.fRestitution = pMaterial.m_fRestitution;
           hit.hitMaterial.szUserData = pMaterial.m_userData.cString();

           break;
         }
       }

       hkpRigidBody *pRigidBody = ((vHavokStaticMesh*)pObject)->GetHkRigidBody();
       VASSERT(pRigidBody);
       hit.pHitObject = ((vHavokStaticMesh*)pObject)->GetWrappedStaticMesh(0); // if multiple sub parts would have had materials anyway
       hit.hitMaterial.fDynamicFriction = float(pRigidBody->getFriction());
       hit.hitMaterial.fRestitution = float(pRigidBody->getRestitution());
     }
     break;

#ifdef SUPPORTS_TERRAIN
   case V_USERDATA_TERRAIN:
     {
       // hit.pHitObject = ((vHavokTerrain*)pObject)->GetWrappedTerrainSector(); // need to find VisObject3D_cl for the terrain?

       hkpRigidBody *pRigidBody = ((vHavokTerrain*)pObject)->GetHkRigidBody();
       VASSERT(pRigidBody);
       hit.hitMaterial.fDynamicFriction = float(pRigidBody->getFriction());
       hit.hitMaterial.fRestitution = float(pRigidBody->getRestitution());

       hit.eHitType = VIS_TRACETYPE_TERRAIN;
     }
     break;
#endif
   }

   // Pass the hit to the raycast object
   pRaycastData->onHit(hit);
}

void vHavokPhysicsModule::PerformRaycast(VisPhysicsRaycastBase_cl *pRaycastData)
{
  WaitForSimulationToComplete();

  if (!m_pPhysicsWorld)
    CreateWorld();

  // Note that in Havok there are lots of ways to speed up raycasting if you can batch them
  // and if they have some sort of coherence (See OptimizedWorldRaycast example in the Havok SDK)
  // This will just do the raycast as asked, so will not be able to batch etc
  m_pPhysicsWorld->lock();

  hkpWorldRayCastInput input;
  vHavokConversionUtils::VisVecToPhysVecWorld(pRaycastData->vRayStart, input.m_from);
  vHavokConversionUtils::VisVecToPhysVecWorld(pRaycastData->vRayEnd, input.m_to);
  input.m_enableShapeCollectionFilter = true;
  input.m_filterInfo = pRaycastData->iCollisionBitmask;

  // if not all the hits (only the closest)
  if ( ! pRaycastData->allHits() )
  {
    hkpClosestRayHitCollector output;
    m_pPhysicsWorld->castRay(input, output );

    const hkpWorldRayCastOutput& hit = output.getHit();
    if( hit.hasHit() )
    {
      ForwardRaycastData( pRaycastData, &hit );
    }
  }
  else // all the hits
  {
    hkpAllRayHitCollector output;
    m_pPhysicsWorld->castRay( input, output );
    output.sortHits();

    const hkArray<hkpWorldRayCastOutput>& hits = output.getHits();
    for ( int i = 0; i < hits.getSize(); ++i )
    {
      const hkpWorldRayCastOutput& hit = hits[i];
      if ( hit.hasHit() )
      {
        ForwardRaycastData( pRaycastData, &hit );
      }
    }
  }


  m_pPhysicsWorld->unlock();
}

void vHavokPhysicsModule::EnqueueRaycast(VisPhysicsRaycastBase_cl *pRaycastData)
{
  pRaycastData->onEnqueued();

  // if not all the hits (only the closest)
  if (!pRaycastData->allHits())
  {
    VASSERT(m_pRayCasterSingle != NULL);
    m_pRayCasterSingle->AddRay(pRaycastData);
  }
  else // all the hits
  {
    VASSERT(m_pRayCasterMultiple != NULL);
    m_pRayCasterMultiple->AddRay(pRaycastData);
  }
}

IVisPhysicsObject_cl *vHavokPhysicsModule::CreatePhysicsObject( VisObject3D_cl *pObject3D, bool bStatic)
{
  VASSERT_MSG(false, "CreatePhysicsObject should not be called in vHavokPhysicsModule");
  return NULL;
}


// -------------------------------------------------------------------------- //
// Sweep Testing                                                              //
// -------------------------------------------------------------------------- //
int vHavokPhysicsModule::PerformSweep(vHavokSweepResult *pResults, int iNumResults, vHavokRigidBody *pRigidBody, const hkvVec3& vDir,
                                      float fDistance)
{
  VVERIFY_OR_RET_VAL(pResults && pRigidBody, -1);

  WaitForSimulationToComplete();

  if (!m_pPhysicsWorld)
    CreateWorld();

  vHavokMarkWorld markWorld(m_pPhysicsWorld);

  return PerformSweepInternal(pResults, iNumResults, pRigidBody->GetHkRigidBody()->getCollidable(), pRigidBody->GetPosition(), vDir, fDistance);
}

int vHavokPhysicsModule::PerformSweep(vHavokSweepResult *pResults, int iNumResults, vHavokCharacterController *pCharacterCtrl, const hkvVec3& vDir,
  float fDistance)
{
  VVERIFY_OR_RET_VAL(pResults && pCharacterCtrl, -1);

  WaitForSimulationToComplete();

  if (!m_pPhysicsWorld)
    CreateWorld();

  vHavokMarkWorld markWorld(m_pPhysicsWorld);

  return PerformSweepInternal(pResults, iNumResults, pCharacterCtrl->GetCharacterProxy()->getShapePhantom()->getCollidable(), pCharacterCtrl->GetPosition(), vDir, fDistance);
}

int vHavokPhysicsModule::PerformSweepInternal(vHavokSweepResult *pResults, int iNumResults, const hkpCollidable *pCollidable, const hkvVec3& vPos,
  const hkvVec3& vDir, float fDistance)
{
  hkVector4 displacement;
  vHavokConversionUtils::VisVecToPhysVecLocal((vDir*fDistance),displacement);

  hkVector4 position;
  vHavokConversionUtils::VisVecToPhysVecWorld(vPos, position);

  hkpLinearCastInput input;
  input.m_to.setAdd(position, displacement);

  hkpAllCdPointCollector collector;
  m_pPhysicsWorld->linearCast(pCollidable, input, collector);

  int iNumHits = hkvMath::Min(collector.getHits().getSize(), iNumResults);
  if (iNumHits > 0)
  {
    collector.sortHits(); // start with the closest hit
    for (int i=0;i<iNumHits;i++)
    {
      hkpRootCdPoint hit = collector.getHits()[i];
      pResults[i].m_fDistance = fDistance*hit.m_contact.getDistance();
      vHavokConversionUtils::PhysVecToVisVecWorld(hit.m_contact.getPosition(), pResults[i].m_vTouchPos);
      vHavokConversionUtils::PhysVecToVisVec_noscale(hit.m_contact.getNormal(), pResults[i].m_vNormal);
      const hkpWorldObject *pHitObj = hkpGetWorldObject(hit.m_rootCollidableB);
      VASSERT(pHitObj);
      void *pUserData = (void*)pHitObj->getUserData();
      pResults[i].m_collInfo.SetInfo(pUserData); // unpack collision info
    }
  }

  return iNumHits;
}

void vHavokPhysicsModule::PerformSweepBatched(vHavokSweepCommand* pCommands, int iNumCommands)
{
  VVERIFY_OR_RET(pCommands != NULL && iNumCommands > 0);

  WaitForSimulationToComplete();

  if (!m_pPhysicsWorld)
    CreateWorld();

  vHavokMarkWorld markWorld(m_pPhysicsWorld);

  if ( m_pThreadPool != HK_NULL )
  {
    // Compute total number of output results
    int iNumTotalResults = 0;
    for(int iCommandIdx = 0; iCommandIdx < iNumCommands; iCommandIdx++)
    {
      vHavokSweepCommand& command = pCommands[iCommandIdx];

      iNumTotalResults += hkvMath::Min(command.m_iNumMaxResults, hkpWorldLinearCastCommand::MAXIMUM_RESULTS_CAPACITY);
    }

    // Must be allocated on the heap on PS3 - see the note for hkpWorldLinearCastCommand::m_results.
    hkArray<hkpRootCdPoint>::Temp resultBuffer;
    resultBuffer.setSize(iNumTotalResults);

    hkArray<hkpWorldLinearCastCommand>::Temp commandBuffer;
    commandBuffer.reserve(iNumCommands);

    int iResultBufferOffset = 0;

    // Build Physics command for each vHavokSweepCommand
    for(int iCommandIdx = 0; iCommandIdx < iNumCommands; iCommandIdx++)
    {
      vHavokSweepCommand& command = pCommands[iCommandIdx];
      hkpWorldLinearCastCommand physicsCommand;
      physicsCommand.m_numResultsOut = 0; // Fix Android compiler warning about uninitialized variable.

      hkvVec3 vPos(hkvNoInitialization);

      if(command.m_pCharacterController)
      {
        physicsCommand.m_collidable = command.m_pCharacterController->GetCharacterProxy()->getShapePhantom()->getCollidable();
        vPos = command.m_pCharacterController->GetPosition();
      }
      else if(command.m_pRigidBody)
      {
        physicsCommand.m_collidable = command.m_pRigidBody->GetHkRigidBody()->getCollidable();
        vPos = command.m_pRigidBody->GetPosition();
      }
      else
      {
        VASSERT_MSG(false, "Linear cast command specifies neither a character controller nor a rigid body for sweeping!");
      }

      hkVector4 displacement;
      vHavokConversionUtils::VisVecToPhysVecLocal((command.m_vDir * command.m_fDistance), displacement);

      hkVector4 position;
      vHavokConversionUtils::VisVecToPhysVecWorld(vPos, position);

      physicsCommand.m_input.m_to.setAdd(position, displacement);

      physicsCommand.m_resultsCapacity = hkvMath::Min(command.m_iNumMaxResults, hkpWorldLinearCastCommand::MAXIMUM_RESULTS_CAPACITY);
      physicsCommand.m_results = &resultBuffer[iResultBufferOffset];
      iResultBufferOffset += physicsCommand.m_resultsCapacity;

      commandBuffer.pushBackUnchecked(physicsCommand);
    }

    if(m_pSweepSemaphore == NULL)
    {
      m_pSweepSemaphore = new hkSemaphoreBusyWait;
    }

    m_pPhysicsWorld->linearCastMt(&commandBuffer[0], iNumCommands, m_pJobQueue, m_pThreadPool, m_pSweepSemaphore);

    // Convert results back to Vision representation
    for(int iCommandIdx = 0; iCommandIdx < iNumCommands; iCommandIdx++)
    {
      vHavokSweepCommand& command = pCommands[iCommandIdx];
      hkpWorldLinearCastCommand& physicsCommand = commandBuffer[iCommandIdx];

      hkSort(physicsCommand.m_results, physicsCommand.m_numResultsOut);

      command.m_iNumResultsOut = physicsCommand.m_numResultsOut;

      for(int iResultIdx = 0; iResultIdx < physicsCommand.m_numResultsOut; iResultIdx++)
      {
        vHavokSweepResult& result = command.m_pResults[iResultIdx];
        const hkpRootCdPoint& hit = physicsCommand.m_results[iResultIdx];

        result.m_fDistance = command.m_fDistance * hit.m_contact.getDistance();
        vHavokConversionUtils::PhysVecToVisVecWorld(hit.m_contact.getPosition(), result.m_vTouchPos);
        vHavokConversionUtils::PhysVecToVisVec_noscale(hit.m_contact.getNormal(), result.m_vNormal);

        const hkpWorldObject *pHitObj = hkpGetWorldObject(hit.m_rootCollidableB);
        VASSERT(pHitObj);
        void *pUserData = reinterpret_cast<void*>(pHitObj->getUserData());
        result.m_collInfo.SetInfo(pUserData); // unpack collision info
      }
    }
  }
  else
  {
    // fallback for running batched sweeps in a single-threaded environment
    for ( int iCommandIdx = 0; iCommandIdx < iNumCommands; ++iCommandIdx )
    {
      vHavokSweepCommand& command = pCommands[ iCommandIdx ];

      hkvVec3 v3Pos( hkvNoInitialization );
      const hkpCollidable *pCollidable;
      if ( command.m_pRigidBody != NULL )
      {
        pCollidable = command.m_pRigidBody->GetHkRigidBody()->getCollidable();
        v3Pos = command.m_pRigidBody->GetPosition();
      }
      else if ( command.m_pCharacterController != NULL )
      {
        pCollidable = command.m_pCharacterController->GetCharacterProxy()->getShapePhantom()->getCollidable();
        v3Pos = command.m_pCharacterController->GetPosition();
      }
      else
      {
        command.m_iNumResultsOut = -1;
        continue;
      }

      command.m_iNumResultsOut = PerformSweepInternal( command.m_pResults, command.m_iNumMaxResults, pCollidable, v3Pos, command.m_vDir, command.m_fDistance );
    }
  }
}


bool vHavokPhysicsModule::DropToFloor(vHavokRigidBody *pRigidBody, float fDist)
{
  VVERIFY_OR_RET_VAL(pRigidBody, false);

  vHavokSweepResult result;
  int iNumHits = PerformSweep(&result, 1, pRigidBody, hkvVec3(0.0f,0.0f,-1.0f), fDist);

  if (iNumHits > 0)
  {
    hkVector4 dVec;
    dVec.set(0,0,-result.m_fDistance); dVec.mul(vHavokConversionUtils::GetVision2HavokScaleSIMD());
    m_pPhysicsWorld->markForWrite();
    dVec.add(pRigidBody->GetHkRigidBody()->getPosition());
    pRigidBody->GetHkRigidBody()->setPosition(dVec);
    m_pPhysicsWorld->unmarkForWrite();
    hkvVec3 visVec;
    vHavokConversionUtils::PhysVecToVisVecWorld(dVec, visVec);
    pRigidBody->GetOwner3D()->SetPosition(visVec);
    return true;
  }

  return false;
}

bool vHavokPhysicsModule::DropToFloor(vHavokCharacterController *pCharacterCtrl, float fDist)
{
  VVERIFY_OR_RET_VAL(pCharacterCtrl, false);

  vHavokSweepResult result;
  int iNumHits = PerformSweep(&result, 1, pCharacterCtrl, hkvVec3(0.0f,0.0f,-1.0f), fDist);

  if (iNumHits > 0)
  {
    hkvVec3 vPos = pCharacterCtrl->GetPosition();
    vPos.z -= float(result.m_fDistance);
    pCharacterCtrl->SetPosition(vPos);
    return true;
  }

  return false;
}


// -------------------------------------------------------------------------- //
// Misc Callbacks                                                             //
// -------------------------------------------------------------------------- //

void vHavokPhysicsModule::OnNewWorldLoaded()
{
}


void vHavokPhysicsModule::OnReposition()
{
  WaitForSimulationToComplete();

  hkvVec3d worldPivot; Vision::GetSceneManager()->GetZoneRepositionInfo().GetGlobalPivotPos(worldPivot);
  vHavokConversionUtils::SetVisionWorldPivot(worldPivot);

  // Update rigid bodies
  {
    int iCount = m_simulatedRigidBodies.Count();
    for (int i = 0; i < iCount; i++)
    {
      m_simulatedRigidBodies.GetAt(i)->Reposition();
    }
  }

  // Update character controllers
  {
    int iCount = m_simulatedControllers.Count();
    for (int i = 0; i < iCount; i++)
      m_simulatedControllers.GetAt(i)->UpdateOwner();
  }


  // Update rag dolls
  {
    int iCount = m_simulatedRagdolls.Count();
    for (int i = 0; i < iCount; i++)
    {
      m_simulatedRagdolls.GetAt(i)->UpdateOwner();
    }
  }
}


void vHavokPhysicsModule::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if( pData->m_pSender == &IVisSceneManager_cl::OnReposition)
  {
    OnReposition();
  }
  else if (pData->m_pSender == &Vision::Callbacks.OnBeforeSceneLoaded)
  {
    m_bIsSceneLoading = true;
    return;
  }
  else if (pData->m_pSender==&Vision::Callbacks.OnAfterSceneLoaded)
  {
    m_bIsSceneLoading = false;

    if (!m_pPhysicsWorld)
      CreateWorld();

      m_pPhysicsWorld->m_useCompoundSpuElf = !g_vGlobalWorldParams.m_runtimeParams.m_bEnableLegacyCompoundShapes;

    hkMemorySystem::getInstance().garbageCollect();

    // Create any static mesh instances that have been collected while loading
    // the scene and still need to be initialized (as one merged rigid body)
    CreatePendingStaticMeshes();

    // Set broadphase size
    bool bAutoComputedBroadphase = false;
    if (g_vGlobalWorldParams.m_runtimeParams.m_bBroadphaseAutoCompute)
    {
      if (SetBroadphaseSizeAuto())
      {
        bAutoComputedBroadphase = true;
      }
      else
      {
        hkvLog::Info("Warning Could not automatically compute broadphase size - no static Physics meshes were found, or the total extent of the static geometry was too small (must be greater than %.1f Vision units in all dimensions). Falling back to manual size.", HK2VIS_FLOAT_SCALED(2.0f * BROADPHASE_SIZE_TOLERANCE));
      }
    }

    if (!bAutoComputedBroadphase)
    {
      SetBroadphaseSizeManual(g_vGlobalWorldParams.m_runtimeParams.m_fBroadphaseManualSize);
    }

    SetSolver( g_vGlobalWorldParams.m_runtimeParams.m_solverIterations, g_vGlobalWorldParams.m_runtimeParams.m_solverHardness );

    // Debug geometry is discarded when setting the broadphase size. The following call re-creates
    // what was enabled before.
    EnableDebugRendering(m_bDebugRenderRigidBodies, m_bDebugRenderRagdolls, m_bDebugRenderCharacterControllers,
      m_bDebugRenderTriggerVolumes, m_bDebugRenderBlockerVolumes, m_bDebugRenderStaticMeshes);

    return;
  }
  else if (pData->m_pSender == &Vision::Callbacks.OnWorldDeInit)
  {
    // Keep in sync
    vHavokConversionUtils::SetHavok2VisionScale(g_vGlobalWorldParams.m_setupParams.m_fHavokToVisionScale);
    m_staticGeomMode = (StaticGeomMode)g_vGlobalWorldParams.m_setupParams.m_iStaticGeomMode;
    m_mergedStaticWeldingType = (VisWeldingType_e)g_vGlobalWorldParams.m_setupParams.m_iMergedStaticWeldingType;

    vHavokConstraint::ElementManagerDeleteAll();
    vHavokConstraintChain::ElementManagerDeleteAll();
  
    m_ResourceShapes.DisposeAll();
    VASSERT(m_ResourceShapesToInstantiate.Count()==0);
  }
  else if(pData->m_pSender == &Vision::Callbacks.OnAfterSceneExported)
  {
    SaveAllCachedShapes(true);
  }
  else if (pData->m_pSender==&VSceneLoader::OnCustomChunkBeforeShapesSerialization)
  {
    VCustomSceneChunkDataObject &chunkData(*((VCustomSceneChunkDataObject *)pData));
    if (chunkData.m_Loader.IsSaving() || chunkData.m_iChunkID=='HVKP')
    {
      chunkData.m_iChunkID = 'HVKP'; // Havok world setup data
      chunkData.m_bSuccess = chunkData.m_bProcessed = true;
      GlobalsChunkFileExchange(chunkData.m_Loader, chunkData.m_iChunkID);
    }
  }
  else if (pData->m_pSender==&VSceneLoader::OnCustomChunkSerialization)
  {
    VCustomSceneChunkDataObject &chunkData(*((VCustomSceneChunkDataObject *)pData));
    if (chunkData.m_iChunkID=='HVKW')
    {
      hkvLog::Warning("Custom-Chunk 'HKVW' is not supported any more, please reexport scene!");
    }
  }
  else if (pData->m_pSender == &VManagedThread::OnThreadLocalVariableInit)
  {
    // Initialize memory system if threads are initialized after the physics module.
    vHavokCreateMemoryRouterTask task;
    task.Run(NULL);
  }
  else if (pData->m_pSender == &VManagedThread::OnThreadLocalVariableDeInit)
  {
    // Deinitialize memory system if threads are deinitialized before the physics module.
    vHavokDeleteMemoryRouterTask task;
    task.Run(NULL);
  }
}

// -------------------------------------------------------------------------- //
// Rigid Body Management                                                      //
// -------------------------------------------------------------------------- //

void vHavokPhysicsModule::ActivateLinkedCollidables(hkpRigidBody *pRigidBody)
{
  VASSERT(pRigidBody != NULL);
  
  // iterate through all linked collidables and re-activate them 
  hkArray<hkpLinkedCollidable::CollisionEntry>& collisionEntries = pRigidBody->getLinkedCollidable()->getCollisionEntriesNonDeterministic();
  for(int i = 0; i < collisionEntries.getSize(); i++)
  {
    const hkpLinkedCollidable::CollisionEntry& entry = collisionEntries[i];
    hkpRigidBody *pRigidBody = hkpGetRigidBody(entry.m_partner);
    if(pRigidBody != HK_NULL)
      pRigidBody->activate();
  }
}

void vHavokPhysicsModule::AddRigidBody(vHavokRigidBody *pRigidBody)
{
  VVERIFY_OR_RET(pRigidBody != NULL);

  // Add to own list of rigid bodies
  VASSERT(m_simulatedRigidBodies.Find(pRigidBody) == -1);
  m_simulatedRigidBodies.Add(pRigidBody);

  if (m_pPhysicsWorld)
  {
    // Add HkRigidBody to the Havok simulation world
    m_pPhysicsWorld->markForWrite();
    m_pPhysicsWorld->addEntity(pRigidBody->GetHkRigidBody());
    m_pPhysicsWorld->unmarkForWrite();
  }

  // will enable debug rendering, especially if the global flag is set
  pRigidBody->SetDebugRendering (pRigidBody->Debug_Render);
}

void vHavokPhysicsModule::RemoveRigidBody(vHavokRigidBody *pRigidBody)
{
  VVERIFY_OR_RET(pRigidBody != NULL);

  RemoveObjectFromQueues(pRigidBody, NULL, NULL, NULL);

  // It is possible the body was never added to the world!
  if ( m_simulatedRigidBodies.Find(pRigidBody) != -1 )
  {
    // Remove it from the Havok simulation world
    if (m_pPhysicsWorld)
    {
      m_pPhysicsWorld->markForWrite();
      ActivateLinkedCollidables(pRigidBody->GetHkRigidBody());
      m_pPhysicsWorld->removeEntity(pRigidBody->GetHkRigidBody());
      m_pPhysicsWorld->unmarkForWrite();
    }

    m_simulatedRigidBodies.Remove(pRigidBody);
  }
}

// -------------------------------------------------------------------------- //
// Rag Doll Management                                                      //
// -------------------------------------------------------------------------- //

void vHavokPhysicsModule::AddRagdoll(vHavokRagdoll* pRagdoll)
{
  VVERIFY_OR_RET(pRagdoll != NULL);

  // Add to own list of rag dolls
  VASSERT(m_simulatedRagdolls.Find(pRagdoll) == -1);
  m_simulatedRagdolls.Add(pRagdoll);

  // will enable debug rendering, especially if the global flag is set
  pRagdoll->SetDebugRendering(pRagdoll->GetDebugRenderEnabled());
}

void vHavokPhysicsModule::RemoveRagdoll(vHavokRagdoll *pRagdoll)
{
  VVERIFY_OR_RET(pRagdoll != NULL);

  // TODO: extend for rag dolls
  //RemoveObjectFromQueues(pRagdoll, NULL, NULL, NULL);

  // It is possible the rag doll was never added to the world
  if (m_simulatedRagdolls.Find(pRagdoll) != -1 )
  {
    m_simulatedRagdolls.Remove(pRagdoll);
  }
}

// -------------------------------------------------------------------------- //
// Character Controller Management                                            //
// -------------------------------------------------------------------------- //

void vHavokPhysicsModule::AddCharacterController(vHavokCharacterController *pCharacterController)
{
  VVERIFY_OR_RET(pCharacterController != NULL);

  // Add to own list of character controllers
  VASSERT(m_simulatedControllers.Find(pCharacterController) == -1);
  m_simulatedControllers.Add(pCharacterController);

  if (m_pPhysicsWorld && pCharacterController->GetCharacterProxy())
  {
    m_pPhysicsWorld->markForWrite();
    m_pPhysicsWorld->addPhantom( pCharacterController->GetCharacterProxy()->getShapePhantom() );
    m_pPhysicsWorld->unmarkForWrite();
  }

  // will enable debug rendering, especially if the global flag is set
  pCharacterController->SetDebugRendering (pCharacterController->GetDebugRenderEnabled ());
}

void vHavokPhysicsModule::RemoveCharacterController(vHavokCharacterController *pCharacterController)
{
  VVERIFY_OR_RET(pCharacterController != NULL);
  RemoveObjectFromQueues(NULL,pCharacterController,NULL,NULL);

  VASSERT(m_simulatedControllers.Find(pCharacterController) != -1);
  if (m_pPhysicsWorld && pCharacterController->GetCharacterProxy())
  {
    m_pPhysicsWorld->markForWrite();
    m_pPhysicsWorld->removePhantom( pCharacterController->GetCharacterProxy()->getShapePhantom() );
    m_pPhysicsWorld->unmarkForWrite();
  }

  // Remove from list of character controllers
  m_simulatedControllers.Remove(pCharacterController);
}


// -------------------------------------------------------------------------- //
// Static Mesh Management                                                     //
// -------------------------------------------------------------------------- //

void vHavokPhysicsModule::AddStaticMesh(vHavokStaticMesh *pStaticMesh)
{
  VVERIFY_OR_RET(pStaticMesh != NULL);

  // Add to own list of rigid bodies
  VASSERT(m_simulatedStaticMeshes.Find(pStaticMesh) == -1);
  m_simulatedStaticMeshes.Add(pStaticMesh);

  // Add HkRigidBody to the Havok simulation world
  if (m_pPhysicsWorld)
  {
    m_pPhysicsWorld->markForWrite();
    m_pPhysicsWorld->addEntity(pStaticMesh->GetHkRigidBody());
    m_pPhysicsWorld->unmarkForWrite();
  }
}

void vHavokPhysicsModule::RemoveStaticMesh(vHavokStaticMesh *pStaticMesh)
{
  VVERIFY_OR_RET(pStaticMesh != NULL);
  RemoveObjectFromQueues(NULL,NULL,pStaticMesh,NULL);

  VASSERT(m_simulatedStaticMeshes.Find(pStaticMesh) != -1);
  // Remove it from the Havok simulation world
  if (m_pPhysicsWorld)
  {
    m_pPhysicsWorld->markForWrite();
    ActivateLinkedCollidables(pStaticMesh->GetHkRigidBody());
    m_pPhysicsWorld->removeEntity(pStaticMesh->GetHkRigidBody());
    m_pPhysicsWorld->unmarkForWrite();
  }

  m_simulatedStaticMeshes.Remove(pStaticMesh);
}


// -------------------------------------------------------------------------- //
// Terrain Management                                                         //
// -------------------------------------------------------------------------- //

#ifdef SUPPORTS_TERRAIN

void vHavokPhysicsModule::AddTerrain(vHavokTerrain *pTerrain)
{
  VVERIFY_OR_RET(pTerrain != NULL);

  // Add to own list of rigid bodies
  VASSERT(m_simulatedTerrainSectors.Find(pTerrain) == -1);
  m_simulatedTerrainSectors.Add(pTerrain);

  // Add HkRigidBody to the Havok simulation world
  if (m_pPhysicsWorld)
  {
    m_pPhysicsWorld->markForWrite();
    m_pPhysicsWorld->addEntity(pTerrain->GetHkRigidBody());
    m_pPhysicsWorld->unmarkForWrite();
  }
}

void vHavokPhysicsModule::RemoveTerrain(vHavokTerrain *pTerrain)
{
  VVERIFY_OR_RET(pTerrain != NULL);
  RemoveObjectFromQueues(NULL,NULL,NULL,pTerrain);

  VASSERT(m_simulatedTerrainSectors.Find(pTerrain) != -1);
  // Remove it from the Havok simulation world
  if (m_pPhysicsWorld)
  {
    m_pPhysicsWorld->markForWrite();
    m_pPhysicsWorld->removeEntity(pTerrain->GetHkRigidBody());
    m_pPhysicsWorld->unmarkForWrite();
  }

  m_simulatedTerrainSectors.Remove(pTerrain);
}

void vHavokPhysicsModule::AddDecoration(VHavokDecorationGroup* pDec)
{
  VVERIFY_OR_RET(pDec != NULL);

  // Add to own list of rigid bodies
  VASSERT(m_simulatedDecorations.Find(pDec) == -1);
  m_simulatedDecorations.Add(pDec);

  // Add HkRigidBody to the Havok simulation world
  if (m_pPhysicsWorld)
  {
    m_pPhysicsWorld->markForWrite();
    m_pPhysicsWorld->addEntity(pDec->GetHkRigidBody());
    m_pPhysicsWorld->unmarkForWrite();
  }
}

void vHavokPhysicsModule::RemoveDecoration(VHavokDecorationGroup *pDec)
{
  VVERIFY_OR_RET(pDec != NULL);
  // RemoveObjectFromQueues(NULL,NULL,NULL,NULL,pDec);

  VASSERT(m_simulatedDecorations.Find(pDec) != -1);

  // Remove it from the Havok simulation world
  if (m_pPhysicsWorld && pDec->GetHkRigidBody())
  {
    m_pPhysicsWorld->markForWrite();
    m_pPhysicsWorld->removeEntity(pDec->GetHkRigidBody());
    m_pPhysicsWorld->unmarkForWrite();
  }
  m_simulatedDecorations.Remove(pDec);

}

#endif

// -------------------------------------------------------------------------- //
// Constraint Management                                                      //
// -------------------------------------------------------------------------- //

void vHavokPhysicsModule::AddConstraint(vHavokConstraint *pConstraint)
{
  hkpConstraintInstance *pConstraintInstance =
    pConstraint ? pConstraint->GetHkConstraintInstance() : NULL;
  VVERIFY_OR_RET(pConstraintInstance);

  VASSERT(m_simulatedConstraints.Find(pConstraint) == -1);
  m_simulatedConstraints.Append(pConstraint);

  // Add the Havok constraint instance to the Havok simulation world
  if (m_pPhysicsWorld)
  {
    m_pPhysicsWorld->markForWrite();
    m_pPhysicsWorld->addConstraint(pConstraintInstance);
    m_pPhysicsWorld->unmarkForWrite();
  }
}

void vHavokPhysicsModule::RemoveConstraint(vHavokConstraint *pConstraint)
{
  hkpConstraintInstance *pConstraintInstance =
    pConstraint ? pConstraint->GetHkConstraintInstance() : NULL;
  VVERIFY_OR_RET(pConstraintInstance);

  VASSERT(m_simulatedConstraints.Find(pConstraint) != -1);
  // Remove the Havok constraint instance from the Havok simulation world
  if (m_pPhysicsWorld && pConstraintInstance->getOwner())
  {
    m_pPhysicsWorld->markForWrite();
    hkpRigidBody *pRigidBody = pConstraintInstance->getRigidBodyA();
    if(pRigidBody != NULL)
      pRigidBody->activate();
    pRigidBody = pConstraintInstance->getRigidBodyB();
    if(pRigidBody != NULL)
      pRigidBody->activate();
    m_pPhysicsWorld->removeConstraint(pConstraintInstance);
    m_pPhysicsWorld->unmarkForWrite();
  }

  m_simulatedConstraints.Remove(pConstraint);
}

void vHavokPhysicsModule::AddConstraintChain(vHavokConstraintChain *pConstraintChain)
{
  hkpConstraintChainInstance *pInstance =
    pConstraintChain ? pConstraintChain->GetHkConstraintChainInstance() : NULL;
  VVERIFY_OR_RET(pInstance);

  // Add the Havok instance to the Havok simulation world
  if (m_pPhysicsWorld)
  {
    m_pPhysicsWorld->markForWrite();
    m_pPhysicsWorld->addConstraint(pInstance);
    m_pPhysicsWorld->unmarkForWrite();
  }
}

void vHavokPhysicsModule::RemoveConstraintChain(vHavokConstraintChain *pConstraintChain)
{
  hkpConstraintChainInstance *pInstance =
    pConstraintChain ? pConstraintChain->GetHkConstraintChainInstance() : NULL;
  VVERIFY_OR_RET(pInstance);

  // Remove the Havok instance from the Havok simulation world
  if (m_pPhysicsWorld)
  {
    m_pPhysicsWorld->markForWrite();
    hkpRigidBody *pRigidBody = pInstance->getRigidBodyA();
    if(pRigidBody != NULL)
      pRigidBody->activate();
    pRigidBody = pInstance->getRigidBodyB();
    if(pRigidBody != NULL)
      pRigidBody->activate();
    m_pPhysicsWorld->removeConstraint(pInstance);
    m_pPhysicsWorld->unmarkForWrite();
  }
}

// -------------------------------------------------------------------------- //
// Trigger Volume Management                                                  //
// -------------------------------------------------------------------------- //

void vHavokPhysicsModule::AddTriggerVolume(vHavokTriggerVolume *pTriggerVolume)
{
  VVERIFY_OR_RET(pTriggerVolume != NULL);

  // Add to own list of trigger volumes
  VASSERT(m_simulatedTriggerVolumes.Find(pTriggerVolume) == -1);
  m_simulatedTriggerVolumes.Add(pTriggerVolume);

  if (m_pPhysicsWorld)
  {
    // Add HkRigidBody of Havok trigger volume to the Havok simulation world
    m_pPhysicsWorld->markForWrite();
    hkpRigidBody *pRigidBody = pTriggerVolume->GetHkTriggerBody();
    VASSERT(pRigidBody != NULL);
    m_pPhysicsWorld->addEntity(pRigidBody);
    m_pPhysicsWorld->unmarkForWrite();
  }

  // will enable debug rendering, especially if the global flag is set
  pTriggerVolume->SetDebugRendering (pTriggerVolume->GetDebugRenderEnabled ());
}

void vHavokPhysicsModule::RemoveTriggerVolume(vHavokTriggerVolume *pTriggerVolume)
{
  VVERIFY_OR_RET(pTriggerVolume != NULL);

  RemoveVolumeFromQueues(pTriggerVolume);

  // It is possible the trigger volume was never added to the world!
  if ( m_simulatedTriggerVolumes.Find(pTriggerVolume) != -1 )
  {
    // Remove HkRigidBody of Havok trigger volume from the Havok simulation world
    if (m_pPhysicsWorld)
    {
      m_pPhysicsWorld->markForWrite();
      hkpRigidBody *pRigidBody = pTriggerVolume->GetHkTriggerBody();
      VASSERT(pRigidBody != NULL);
      m_pPhysicsWorld->removeEntity(pRigidBody);
      m_pPhysicsWorld->unmarkForWrite();
    }

    m_simulatedTriggerVolumes.Remove(pTriggerVolume);
  }
}

// -------------------------------------------------------------------------- //
// Blocker Volume Management                                                  //
// -------------------------------------------------------------------------- //

void vHavokPhysicsModule::AddBlockerVolume(vHavokBlockerVolumeComponent* pBlockerVolume)
{
  VVERIFY_OR_RET(pBlockerVolume != NULL);

  // Add to own list of rag dolls
  VASSERT(m_simulatedBlockerVolumes.Find(pBlockerVolume) == -1);
  m_simulatedBlockerVolumes.Add(pBlockerVolume);

  // will enable debug rendering, especially if the global flag is set
  pBlockerVolume->SetDebugRendering(pBlockerVolume->GetDebugRenderEnabled());
}

void vHavokPhysicsModule::RemoveBlockerVolume(vHavokBlockerVolumeComponent* pBlockerVolume)
{
  VVERIFY_OR_RET(pBlockerVolume != NULL);

  // It is possible the rag doll was never added to the world
  if (m_simulatedBlockerVolumes.Find(pBlockerVolume) != -1)
  {
    m_simulatedBlockerVolumes.Remove(pBlockerVolume);
  }
}

// -------------------------------------------------------------------------- //
// Havok Debug Rendering                                                      //
// -------------------------------------------------------------------------- //

void vHavokPhysicsModule::SetEnabledVisualDebugger(bool bEnabled)
{
#if !defined(PROFILING ) || !defined(_VISION_WINPHONE) // Conflicts at the moment
  m_bVisualDebugger = bEnabled;

  if (m_bVisualDebugger && (m_spVisualDebugger == NULL) && (m_pPhysicsWorld != NULL) )
  {
    m_spVisualDebugger = new vHavokVisualDebugger(m_pPhysicsWorld, m_iVisualDebuggerPort);
  }
  else if (!m_bVisualDebugger && m_spVisualDebugger != NULL)
  {
    m_spVisualDebugger = NULL;
  }
#else
  m_bVisualDebugger = false;
#endif
}

bool vHavokPhysicsModule::IsEnabledVisualDebugger() const
{
  return m_bVisualDebugger;
}

void vHavokPhysicsModule::SetVisualDebuggerPort(int iPort)
{
  m_iVisualDebuggerPort = iPort;
  if(m_spVisualDebugger)
  {
    m_spVisualDebugger->SetPort(m_iVisualDebuggerPort);
  }
}

void vHavokPhysicsModule::SetEnabledDebug(bool bEnabled)
{
  m_bDebugDisplay = bEnabled;

 if (m_bDebugDisplay && m_spDisplayHandler == NULL && m_pPhysicsWorld!=NULL)
    m_spDisplayHandler = new vHavokDisplayHandler(m_pPhysicsWorld);
 else if (!m_bDebugDisplay && m_spDisplayHandler != NULL)
    m_spDisplayHandler = NULL;
}

bool vHavokPhysicsModule::IsEnabledDebug() const
{
  return m_bDebugDisplay;
}

// -------------------------------------------------------------------------- //
// Collision Filtering                                                        //
// -------------------------------------------------------------------------- //

void vHavokPhysicsModule::SetGroupsCollision(int group1, int group2, bool bEnable)
{
  VASSERT(m_pPhysicsWorld);
  m_pPhysicsWorld->lock();
  const hkpCollisionFilter *pColFilter = m_pPhysicsWorld->getCollisionFilter();

   // Get the group filter
   VASSERT(pColFilter && ((pColFilter->m_type == hkpCollisionFilter::HK_FILTER_GROUP) || (pColFilter->m_type == hkpCollisionFilter::HK_FILTER_CONSTRAINT)));
   if ( pColFilter->m_type == hkpCollisionFilter::HK_FILTER_CONSTRAINT )
   {
      hkpConstraintCollisionFilter* pConstraintFilter = (hkpConstraintCollisionFilter*)pColFilter;
      pColFilter = pConstraintFilter->m_childFilter;
   }
   VASSERT(pColFilter && (pColFilter->m_type == hkpCollisionFilter::HK_FILTER_GROUP));
   hkpGroupFilter *pGroupFilter = (hkpGroupFilter*)pColFilter;

  if (bEnable)
  {
    pGroupFilter->enableCollisionsBetween(group1, group2);
    g_vGlobalWorldParams.m_runtimeParams.m_iCollisionGroupMasks[group1] |= (1 << group2);
    g_vGlobalWorldParams.m_runtimeParams.m_iCollisionGroupMasks[group2] |= (1 << group1);
  }
  else
  {
    pGroupFilter->disableCollisionsBetween(group1, group2);
    g_vGlobalWorldParams.m_runtimeParams.m_iCollisionGroupMasks[group1] &= ~(1 << group2);
    g_vGlobalWorldParams.m_runtimeParams.m_iCollisionGroupMasks[group2] &= ~(1 << group1);
  }
  m_bUpdateFilter = true;
  m_pPhysicsWorld->unlock();
}

void vHavokPhysicsModule::UpdateGroupsCollision()
{
  VASSERT(m_pPhysicsWorld);
  m_pPhysicsWorld->lock();
  const hkpCollisionFilter *pColFilter = m_pPhysicsWorld->getCollisionFilter();

   // Get group filter
   VASSERT(pColFilter && ((pColFilter->m_type == hkpCollisionFilter::HK_FILTER_GROUP) || (pColFilter->m_type == hkpCollisionFilter::HK_FILTER_CONSTRAINT)));
   if ( pColFilter->m_type == hkpCollisionFilter::HK_FILTER_CONSTRAINT )
   {
      hkpConstraintCollisionFilter* pConstraintFilter = (hkpConstraintCollisionFilter*)pColFilter;
      pColFilter = pConstraintFilter->m_childFilter;

      // Check if the constraint filter is not wanted
      if ( !vHavokPhysicsModule_GetDefaultWorldRuntimeSettings().m_bEnableConstraintCollisionFilter )
      {
         if ( m_pPhysicsWorld->m_constraintListeners.indexOf(pConstraintFilter) >= 0 )
         {
            m_pPhysicsWorld->removeConstraintListener(pConstraintFilter);
         }

         hkpCollisionFilter* nonConstFilter = const_cast<hkpCollisionFilter*>(pColFilter);
         nonConstFilter->addReference();
         m_pPhysicsWorld->setCollisionFilter(nonConstFilter);
         nonConstFilter->removeReference();
      }
   }
   else if ( vHavokPhysicsModule_GetDefaultWorldRuntimeSettings().m_bEnableConstraintCollisionFilter )
   {
      // We have no constraint collision filter and we were asked for one. Create one now!
      VASSERT(pColFilter && (pColFilter->m_type == hkpCollisionFilter::HK_FILTER_GROUP));
      hkpGroupFilter *pGroupFilter = (hkpGroupFilter*)pColFilter;
      hkpConstraintCollisionFilter* pConstraintFilter = new hkpConstraintCollisionFilter(pGroupFilter);
      pConstraintFilter->updateFromWorld(m_pPhysicsWorld);
      m_pPhysicsWorld->setCollisionFilter(pConstraintFilter);
      pConstraintFilter->removeReference();
   }
   VASSERT(pColFilter && (pColFilter->m_type == hkpCollisionFilter::HK_FILTER_GROUP));
   hkpGroupFilter *pGroupFilter = (hkpGroupFilter*)pColFilter;


  // first disable collisions between all groups (group 0 should not be disabled, see docs)
  pGroupFilter->disableCollisionsUsingBitfield(0xfffffffe, 0xfffffffe);

  // then enable collisions, which are set in vHavokPhysicsModuleDefaultRuntimeWorldParams
  for (int i=1;i<32;i++)
  {
    unsigned int iGroup = 1 << i;
    pGroupFilter->enableCollisionsUsingBitfield(iGroup, g_vGlobalWorldParams.m_runtimeParams.m_iCollisionGroupMasks[i]);
  }

  m_bUpdateFilter = true;
  m_pPhysicsWorld->unlock();
}

// -------------------------------------------------------------------------- //
// Static Mesh Callbacks                                                      //
// -------------------------------------------------------------------------- //

void vHavokPhysicsModule::OnStaticMeshInstanceCreated(VisStaticMeshInstance_cl *pMeshInstance)
{
  VVERIFY_OR_RET(pMeshInstance != NULL);

  // Ensure that merged meshes take their collision information from file
  if (m_staticGeomMode == vHavokPhysicsModule::SGM_MERGED_INSTANCES)
    pMeshInstance->SetCollisionBehavior(VisStaticMeshInstance_cl::VIS_COLLISION_BEHAVIOR_FROMFILE);

  // Ignore static meshes that have been marked as non collidable
  if (pMeshInstance->GetCollisionBehavior() == VisStaticMeshInstance_cl::VIS_COLLISION_BEHAVIOR_FORCENOCOLLISION ||
      pMeshInstance->GetPhysicsObject()!=NULL)
    return;

  // If the user configured to merge all static meshes from the scene then
  // added it to our list and continue (in case we are within a scene load operation)
  if (m_bIsSceneLoading && m_staticGeomMode == vHavokPhysicsModule::SGM_MERGED_INSTANCES)
  {
    m_pendingStaticMeshes.Append(pMeshInstance);
    return;
  }

  WaitForSimulationToComplete();

  // Create our static mesh instance representation
  VSmartPtr<vHavokStaticMesh> spStaticMesh = new vHavokStaticMesh();
  spStaticMesh->Init(*pMeshInstance);
}

void vHavokPhysicsModule::OnStaticMeshInstanceRemoved(VisStaticMeshInstance_cl *pMeshInstance)
{
  VVERIFY_OR_RET(pMeshInstance != NULL);

  // Retrieve the vHavok object from the static mesh instance
  vHavokStaticMesh *pHavokStaticMesh =
    static_cast<vHavokStaticMesh*>(pMeshInstance->GetPhysicsObject());
  if (!pHavokStaticMesh)
    return;

  // Wait until it is safe to modify physics objects...
  WaitForSimulationToComplete();

  // ...then remove the mesh instance from the physics object. CAUTION: this
  // may destroy the vHavok object!
  pHavokStaticMesh->RemoveStaticMesh(pMeshInstance);
}

void vHavokPhysicsModule::OnStaticMeshInstanceMoved(VisStaticMeshInstance_cl *pMeshInstance)
{
  VVERIFY_OR_RET(pMeshInstance != NULL);

  // Retrieve the vHavok object from the static mesh instance
  vHavokStaticMesh *pHavokStaticMesh = static_cast<vHavokStaticMesh*>(pMeshInstance->GetPhysicsObject());

  if(pHavokStaticMesh != NULL)
  {
    // Wait until it is safe to modify physics objects...
    WaitForSimulationToComplete();

    MarkForWrite();
    pHavokStaticMesh->UpdateVision2Havok();
    UnmarkForWrite();
  }
  else
  {
    // Physics static mesh can be missing due undersized source mesh. In this case try to create
    // it here with a potentially new/ sufficient scale.
    OnStaticMeshInstanceCreated(pMeshInstance);
  }
}

void vHavokPhysicsModule::OnStaticMeshInstanceCollisionBitmaskChanged(VisStaticMeshInstance_cl *pMeshInstance)
{
  VVERIFY_OR_RET(pMeshInstance != NULL);

  // for merged meshes can't reset collision information (required at creation time)
  if (m_staticGeomMode == SGM_MERGED_INSTANCES)
    return;

  unsigned int iNewMask = pMeshInstance->GetCollisionBitmask();
  VisStaticMeshInstance_cl::VisCollisionBehavior_e eCollisionBehavior = pMeshInstance->GetCollisionBehavior();

  // this means collision is turned off completely
  if (eCollisionBehavior==VisStaticMeshInstance_cl::VIS_COLLISION_BEHAVIOR_FORCENOCOLLISION)
  {
    OnStaticMeshInstanceRemoved(pMeshInstance);
    return;
  }

  // recreate mesh since collision information from file is set at creation time
  OnStaticMeshInstanceRemoved(pMeshInstance);
  OnStaticMeshInstanceCreated(pMeshInstance);
}

//
//	Returns the group collision filter

hkpGroupFilter* vHavokPhysicsModule::GetGroupCollisionFilter()
{
   if ( !m_pPhysicsWorld )
   {
      return HK_NULL;
   }

   // Get collision filter
   hkpCollisionFilter* colFilter = m_pPhysicsWorld->m_collisionFilter;
   VASSERT(colFilter && ((colFilter->m_type == hkpCollisionFilter::HK_FILTER_GROUP) || (colFilter->m_type == hkpCollisionFilter::HK_FILTER_CONSTRAINT)));
   if ( colFilter->m_type == hkpCollisionFilter::HK_FILTER_CONSTRAINT )
   {
      hkpConstraintCollisionFilter* constraintColFilter = reinterpret_cast<hkpConstraintCollisionFilter*>(colFilter);
      colFilter = const_cast<hkpCollisionFilter*>(constraintColFilter->m_childFilter);
      VASSERT(colFilter && (colFilter->m_type == hkpCollisionFilter::HK_FILTER_GROUP));
   }

   // This is a regular group filter
   return reinterpret_cast<hkpGroupFilter*>(colFilter);
}

#ifdef SUPPORTS_TERRAIN

void vHavokPhysicsModule::OnTerrainSectorCreated(VTerrainSector *pTerrainSector)
{
  VVERIFY_OR_RET(pTerrainSector != NULL);

  // Ignore terrains that have been marked as non collidable
//  if (pTerrainSector->GetCollisionBitmask()==0) //Submeshes don't have collision flags
  if (pTerrainSector->GetTraceBitmask()==0)  //For now we still use the trace flags for backwards compatibility
    return;

  // make sure the simulation has finished
  WaitForSimulationToComplete();

  // Create our terrain sector representation
  vHavokTerrain *havokTerrain = new vHavokTerrain(*this);
  havokTerrain->Init(*pTerrainSector);
  pTerrainSector->SetPhysicsUserData(havokTerrain);

}

void vHavokPhysicsModule::OnTerrainSectorRemoved(VTerrainSector *pTerrainSector)
{
  VVERIFY_OR_RET(pTerrainSector != NULL);

  WaitForSimulationToComplete();

  // Find our terrain representation and remove it
  int iCount = m_simulatedTerrainSectors.Count();
  for (int i = 0; i < iCount; i++)
  {
    vHavokTerrain *havokTerrain = m_simulatedTerrainSectors.GetAt(i);
    if (havokTerrain->GetWrappedTerrainSector() == pTerrainSector)
    {
      havokTerrain->DisposeObject();
      break;
    }
  }
}

void vHavokPhysicsModule::OnTerrainSectorSaved(VTerrainSector *pTerrainSector)
{
  // Only cache shape to HKT file when inside vForge.
  if (!Vision::Editor.IsInEditor() || !vHavokPhysicsModule_GetDefaultWorldRuntimeSettings().m_bEnableDiskShapeCaching)
    return;

  VVERIFY_OR_RET(pTerrainSector != NULL);

  WaitForSimulationToComplete();

  // Find our terrain representation and cache it's shape to a HKT file
  int iCount = m_simulatedTerrainSectors.Count();
  for (int i = 0; i < iCount; i++)
  {
    vHavokTerrain *havokTerrain = m_simulatedTerrainSectors.GetAt(i);
    if (havokTerrain->GetWrappedTerrainSector() == pTerrainSector)
    {
      havokTerrain->SaveShapeToFile();
      break;
    }
  }
}

void vHavokPhysicsModule::OnDecorationCreated(IVisDecorationGroup_cl &group)
{
  // First check whether the actor for this group was already created
  if (group.GetDecorationPhysicsUserData() != NULL)
    return;

  // Ignore empty groups
  int iCount = group.GetDecorationObjectCount();
  bool bHasCollision = false;
  for (int i=0;i<iCount;i++)
  {
    VTerrainDecorationInstance *pInst = group.GetDecorationObject(i);
    if (pInst->GetModel()!=NULL && pInst->GetModel()->GetCollisionSubObjectCount()>0)
    {
      bHasCollision = true;
      break;
    }
  }

  if (!bHasCollision)
    return;

  // Wait until it is safe to modify physics objects...
  WaitForSimulationToComplete();

  VHavokDecorationGroup *pDecoGroup = new VHavokDecorationGroup(group);
  AddDecoration(pDecoGroup);
  group.SetDecorationPhysicsUserData(pDecoGroup);
}

void vHavokPhysicsModule::OnDecorationRemoved(IVisDecorationGroup_cl &group)
{

  VHavokDecorationGroup *pDecoGroup = (VHavokDecorationGroup *)group.GetDecorationPhysicsUserData();
  if (pDecoGroup)
  {
    // Wait until it is safe to modify physics objects...
    WaitForSimulationToComplete();

    RemoveDecoration(pDecoGroup);
    group.SetDecorationPhysicsUserData(NULL);
  }
}

#endif

void vHavokPhysicsModule::CreatePendingStaticMeshes()
{
  int iCount = m_pendingStaticMeshes.GetLength();
  if (iCount == 0)
    return;

  // We must only get here when SGM_MERGED_INSTANCES mode is configured
  VASSERT(m_staticGeomMode == vHavokPhysicsModule::SGM_MERGED_INSTANCES);

  // Create our static mesh physics representation now.
  // This instance will include all Vision static mesh instances that are part of the scene file,
  // basically one big merged physics mesh (optimal for performance)
  vHavokStaticMesh* pPhysStaticMesh = new vHavokStaticMesh();
  pPhysStaticMesh->Init(m_pendingStaticMeshes);

  // Clear the array; we did the job
  m_pendingStaticMeshes.Reset();
}

const VArray<IHavokStepper*, IHavokStepper*> vHavokPhysicsModule::GetSteppers() const
{
  return m_steppers;
}

void vHavokPhysicsModule::AddStepper(IHavokStepper* s)
{
  VASSERT( m_steppers.Find(s) < 0 );
  m_steppers.Add(s);
}

void vHavokPhysicsModule::RemoveStepper(IHavokStepper* s)
{
  int idx = m_steppers.Find(s);
  if (idx >= 0)
    m_steppers.RemoveAt(idx);
}

void vHavokPhysicsModule::LockWorld()
{
  if (m_pPhysicsWorld != NULL)
  {
    m_pPhysicsWorld->lock();
  }
}

void vHavokPhysicsModule::UnlockWorld()
{
  if (m_pPhysicsWorld != NULL)
  {
    m_pPhysicsWorld->unlock();
  }
}

void vHavokPhysicsModule::MarkForWrite()
{
  if (m_pPhysicsWorld != NULL)
  {
    m_pPhysicsWorld->markForWrite();
  }
}

void vHavokPhysicsModule::MarkForRead()
{
  if (m_pPhysicsWorld != NULL)
  {
    m_pPhysicsWorld->markForRead();
  }
}

void vHavokPhysicsModule::UnmarkForWrite()
{
  if (m_pPhysicsWorld != NULL)
  {
     m_pPhysicsWorld->unmarkForWrite();
  }
}

void vHavokPhysicsModule::UnmarkForRead()
{
  if (m_pPhysicsWorld != NULL)
  {
    m_pPhysicsWorld->unmarkForRead();
  }
}

int vHavokPhysicsModule::GetNumSolverIterations() const
{
  if (m_pPhysicsWorld != NULL)
  {
    return m_pPhysicsWorld->getSolverInfo()->m_numSteps;
  }
  return 0;
}

int vHavokPhysicsModule::GetNumSolverMicrosteps() const
{
  if (m_pPhysicsWorld != NULL)
  {
    return m_pPhysicsWorld->getSolverInfo()->m_numMicroSteps;
  }
  return 0;
}

hkReal vHavokPhysicsModule::GetSolverTau() const
{
  if (m_pPhysicsWorld != NULL)
  {
    return m_pPhysicsWorld->getSolverInfo()->m_tau;
  }
  return hkReal(0);
}

void vHavokPhysicsModule::DumpCachedMessagesToLog()
{
  m_errorReportMutex.Lock();

  // Dump the cached Havok messages to the log output (e.g. resource viewer)
  for (int i = 0; i < m_cachedWarnings.GetLength(); i++)
    hkvLog::Warning(m_cachedWarnings[i]);
  for (int i = 0; i < m_cachedMessages.GetLength(); i++)
    hkvLog::Info(m_cachedMessages[i]);

  m_cachedMessages.Reset();
  m_cachedWarnings.Reset();

  m_errorReportMutex.Unlock();
}

void vHavokPhysicsModule::SetGravity(const hkvVec3& gravity)
{
  hkVector4 gravityHavokScale; vHavokConversionUtils::VisVecToPhysVecLocal(gravity,gravityHavokScale);

  // Set in defaults
  g_vGlobalWorldParams.m_runtimeParams.m_vGravity = gravity;

    // Set in world, if one
  if (m_pPhysicsWorld)
  {
     MarkForWrite();
     m_pPhysicsWorld->setGravity(gravityHavokScale);
     UnmarkForWrite();
  }
}

void vHavokPhysicsModule::GetGravity(hkvVec3& gravity) const
{
  gravity = g_vGlobalWorldParams.m_runtimeParams.m_vGravity;
}

bool vHavokPhysicsModule::SetBroadphaseSizeAuto()
{
  hkAabb worldBounds;
  worldBounds.setEmpty();
  if (AutoComputeBroadphaseSize(worldBounds))
  {
    g_vGlobalWorldParams.m_runtimeParams.m_bBroadphaseAutoCompute = true;
    SetBroadphaseSize(worldBounds);
    return true;
  }
  return false;
}

void vHavokPhysicsModule::SetBroadphaseSizeManual(float fBroadphaseManualSize)
{
  g_vGlobalWorldParams.m_runtimeParams.m_bBroadphaseAutoCompute = false;
  g_vGlobalWorldParams.m_runtimeParams.m_fBroadphaseManualSize = fBroadphaseManualSize;


  hkAabb worldBounds;
  hkReal fHalfSideLength = VIS2HK_FLOAT_SCALED(g_vGlobalWorldParams.m_runtimeParams.m_fBroadphaseManualSize)*hkReal(0.5);

  // World needs a proper ext in all dirs
  if (fHalfSideLength < BROADPHASE_SIZE_TOLERANCE)
  {
    fHalfSideLength = BROADPHASE_SIZE_TOLERANCE;
    hkvLog::Info("Warning: Manual broadphase size too small, clamped value to %.1f", HK2VIS_FLOAT_SCALED(BROADPHASE_SIZE_TOLERANCE*hkReal(2)));
  }

  worldBounds.m_min.setAll(-fHalfSideLength);
  worldBounds.m_max.setAll(fHalfSideLength);

  SetBroadphaseSize(worldBounds);
}

void vHavokPhysicsModule::SetSolver(int iters, int hardness)
{
  g_vGlobalWorldParams.m_runtimeParams.m_solverIterations = iters;
  g_vGlobalWorldParams.m_runtimeParams.m_solverHardness = hardness;

  // Set in world, if one
  if (m_pPhysicsWorld)
  {
    MarkForWrite();
    hkpSolverInfo* si = m_pPhysicsWorld->getSolverInfo();
    // Use the cinfo so always consistent
    hkpWorldCinfo::SolverType st = _getSolverType(iters, hardness);
    hkpWorldCinfo info; info.setupSolverInfo(st);

    // Note: hkpWorld ctor does all sorts of things, so try can keep this in sync, or ideally add to hkpWorld as
    si->setTauAndDamping( info.m_solverTau, info.m_solverDamp );
    si->m_numSteps    = info.m_solverIterations;
    si->m_invNumSteps = 1.0f / info.m_solverIterations;

    UnmarkForWrite();
  }
}

void vHavokPhysicsModule::AddEntityListenerToIgnore(hkpEntityListener* listener)
{
   m_entityListenersToIgnore.Add(listener);
}


// ----------------------------------------------------------------------------
// vHavokMarkWorld
// ----------------------------------------------------------------------------
vHavokMarkWorld::vHavokMarkWorld(bool bForWrite)
{
  this->bForWrite = bForWrite;
  pWorld = vHavokPhysicsModule::GetInstance()->GetPhysicsWorld();
  if (!pWorld)
    return;

  pWorld->addReference();

  if (bForWrite)
    pWorld->markForWrite();
  else
    pWorld->markForRead();
}

// ----------------------------------------------------------------------------
vHavokMarkWorld::~vHavokMarkWorld()
{
  if (!pWorld)
    return;

  if (bForWrite)
    pWorld->unmarkForWrite();
  else
    pWorld->unmarkForRead();

  pWorld->removeReference();
}

void vHavokPhysicsModule::OnObjectCollision(vHavokCollisionInfo_t& info)
{
   m_NotificationQueueMutex.Lock();
   m_CollisionNotificationQueue[m_iCollisionNotificationQueueSize++] = info;
   m_NotificationQueueMutex.Unlock();
}

void vHavokPhysicsModule::CallObjectCollisionFunction(vHavokCollisionInfo_t& info)
{
  // send message to both
  if (info.m_pSenderTarget[0])
    Vision::Game.SendMsg(info.m_pSenderTarget[0],VIS_MSG_HAVOK_ONCOLLISION,(INT_PTR)&info,sizeof(vHavokCollisionInfo_t));
  if (info.m_pSenderTarget[1])
    Vision::Game.SendMsg(info.m_pSenderTarget[1],VIS_MSG_HAVOK_ONCOLLISION,(INT_PTR)&info,sizeof(vHavokCollisionInfo_t));
}

void vHavokPhysicsModule::RemoveObjectFromQueues(vHavokRigidBody *pRB,
                                                 vHavokCharacterController *pController,
                                                 vHavokStaticMesh *pSM,
                                                 vHavokTerrain *pTerrain)
{
  if (m_iCollisionNotificationQueueSize==0)
    return;

  m_NotificationQueueMutex.Lock();
  int iCount = m_iCollisionNotificationQueueSize;
  m_iCollisionNotificationQueueSize = 0;

  vHavokCollisionInfo_t *pInfo = m_CollisionNotificationQueue.GetDataPtr();
  for (int i=0;i<iCount;i++,pInfo++)
  {
    bool bIsInQueue = false;

    if (pRB && (pInfo->m_Collider[0].m_pRigidBody == pRB || pInfo->m_Collider[1].m_pRigidBody == pRB))
      bIsInQueue = true;
    else if (pController && (pInfo->m_Collider[0].m_pCharacter == pController || pInfo->m_Collider[1].m_pCharacter == pController))
      bIsInQueue = true;
    else if (pSM && (pInfo->m_Collider[0].m_pStaticMesh == pSM || pInfo->m_Collider[1].m_pStaticMesh == pSM))
      bIsInQueue = true;
#ifdef SUPPORTS_TERRAIN
    else if (pTerrain && (pInfo->m_Collider[0].m_pTerrainSector == pTerrain || pInfo->m_Collider[1].m_pTerrainSector == pTerrain))
      bIsInQueue = true;
#endif

    // Skip the collision notification in case it references the passed object
    if (bIsInQueue)
      continue;

    m_CollisionNotificationQueue[m_iCollisionNotificationQueueSize++] = *pInfo;
  }

  //int iRemoved = iCount-m_iCollisionNotificationQueueSize;
  m_NotificationQueueMutex.Unlock();
}

void vHavokPhysicsModule::OnVolumeTrigger(vHavokTriggerInfo &info)
{
  if (this->m_bAsyncPhysics)
  {
    m_NotificationQueueMutex.Lock();
    m_TriggerNotificationQueue[m_iTriggerNotificationQueueSize++] = info;
    m_NotificationQueueMutex.Unlock();
  }
  else
  {
    CallVolumeTriggerFunction(info);
  }
}

void vHavokPhysicsModule::CallVolumeTriggerFunction(vHavokTriggerInfo &info)
{
  info.m_pTriggerSourceComponent->TriggerAllTargets();
  VisTypedEngineObject_cl *pOwner = info.m_pTriggerVolume->GetOwner();
  VASSERT(pOwner != NULL);

  // get the entity, which entered/ left the trigger volume
  VisBaseEntity_cl *pEntity = NULL;
  if (info.m_pObject)
  {
    vHavokUserDataType_e eType = V_USERDATA_UNDEFINED;
    void *pObject = vHavokUserDataPointerPair_t::ExtractTypeAndPointer((void*)info.m_pObject->getUserData(), eType);
    if (pObject && eType==V_USERDATA_OBJECT)
    {
      IVObjectComponent *pComp = static_cast<IVObjectComponent*>(pObject);
      VASSERT(pComp->IsOfType(IVObjectComponent::GetClassTypeId()));

      VisTypedEngineObject_cl *pCompOwner = pComp->GetOwner();
      if (pCompOwner && pCompOwner->IsOfType(V_RUNTIME_CLASS(VisBaseEntity_cl)))
        pEntity =  static_cast<VisBaseEntity_cl*>(pCompOwner);
    }
  }

  pOwner->TriggerScriptEvent(info.m_pTriggerSourceComponent->GetComponentName(), "*o", pEntity);
}

void vHavokPhysicsModule::RemoveVolumeFromQueues(vHavokTriggerVolume *pTriggerVolume)
{
  if (m_iTriggerNotificationQueueSize==0)
    return;

  m_NotificationQueueMutex.Lock();
  int iCount = m_iTriggerNotificationQueueSize;
  m_iTriggerNotificationQueueSize = 0;

  vHavokTriggerInfo *pInfo = m_TriggerNotificationQueue.GetDataPtr();
  for (int i=0;i<iCount;i++,pInfo++)
  {
    // Skip the trigger notification in case it references the passed object
    if (pTriggerVolume && pInfo->m_pTriggerVolume == pTriggerVolume)
      continue;

    m_TriggerNotificationQueue[m_iTriggerNotificationQueueSize++] = *pInfo;
  }

  m_NotificationQueueMutex.Unlock();
}

void vHavokPhysicsModule::SendEnqueuedNotifications()
{
  m_NotificationQueueMutex.Lock();

  {
    vHavokCollisionInfo_t *pPair = m_CollisionNotificationQueue.GetDataPtr();
    for (int i=0;i<m_iCollisionNotificationQueueSize;i++,pPair++)
      CallObjectCollisionFunction(*pPair);
    m_iCollisionNotificationQueueSize = 0;
  }

  {
    vHavokTriggerInfo *pPair = m_TriggerNotificationQueue.GetDataPtr();
    for (int i=0;i<m_iTriggerNotificationQueueSize;i++,pPair++)
      CallVolumeTriggerFunction(*pPair);
    m_iTriggerNotificationQueueSize = 0;
  }

  m_NotificationQueueMutex.Unlock();
}

// static helper function shared by rigid body and character controller
void vHavokPhysicsModule::TriggerCollisionScriptFunction(IVObjectComponent *pPhysComponent, vHavokCollisionInfo_t *pCollInfo)
{
  // Since we depend on VisionEnginePlugin we can use the optimized way to check for availability of the script function
/* Slow path
  IVScriptManager* pManager = Vision::GetScriptManager();
  if (pManager == NULL) return;

  IVScriptInstance* pScript = pManager->GetScriptInstance(GetOwner());
  if (pScript == NULL) return;

  if (pScript->HasFunction("OnCollision"))
*/
  VisTypedEngineObject_cl *pReceiver = pPhysComponent->GetOwner();
  VScriptComponent *pScriptComp = pReceiver->Components().GetComponentOfType<VScriptComponent>();

  //early out: do not continue if there is no script component
  //(there could be more than one --> see end of function)
  if (pScriptComp==NULL)
    return;

  VScriptCollisionInfo scriptInfo;

  scriptInfo.m_vPoint = pCollInfo->m_vPoint;
  scriptInfo.m_vNormal = pCollInfo->m_vNormal;
  scriptInfo.m_fRelativeVelocity = float(pCollInfo->m_fVelocity); // just for now
  scriptInfo.m_fForce = 0.f; //TODO: Add this param

  //determine which collider belongs to the current instance
  if(pPhysComponent==pCollInfo->m_Collider[0].m_pRigidBody || pPhysComponent==pCollInfo->m_Collider[0].m_pCharacter)
  {
    pCollInfo->m_Collider[0].FillScriptInfo(scriptInfo.m_ownInfo);
    pCollInfo->m_Collider[1].FillScriptInfo(scriptInfo.m_colliderInfo);
  }
  else if(pPhysComponent==pCollInfo->m_Collider[1].m_pRigidBody || pPhysComponent==pCollInfo->m_Collider[1].m_pCharacter)
  {
    pCollInfo->m_Collider[1].FillScriptInfo(scriptInfo.m_ownInfo);
    pCollInfo->m_Collider[0].FillScriptInfo(scriptInfo.m_colliderInfo);
  }
  else
  {
    VASSERT_MSG(false, "Unknown Collision!");
  }

  //execute the callback on every script component with matching callback
  for(int i=0; i<pReceiver->Components().Count();i++)
  {
    if( pReceiver->Components().GetPtrs()[i]->IsOfType(VScriptComponent::GetClassTypeId()) )
    {
      VScriptComponent *pScriptComponent = (VScriptComponent *)(pReceiver->Components().GetPtrs()[i]);

      if(pScriptComponent->GetScriptInstance()!=NULL && pScriptComponent->HasFunction(VSCRIPT_FUNC_ONCOLLISION))
        pScriptComponent->GetScriptInstance()->ExecuteCollisionFunc("OnCollision", &scriptInfo);
    }
  }
}

// Gravity, Broadphase size, Static mesh tree cache status, etc. are global settings for the plugin
void vHavokPhysicsModule::GlobalsChunkFileExchange(VChunkFile &file, CHUNKIDTYPE iID)
{
  char iLocalVersion = VHAVOKWORLDPARAMS_VERSION_CURRENT;

  if (file.IsLoading())
  {
    // reset world settings params to defaults

    g_vGlobalWorldParams.Reset();
    // in loading mode, we receive this callback already when the chunk has been identified
    file.ReadChar(iLocalVersion);

    //This version was in a demo branch and backed out during the restructuring -> 2013.1 merge
    VASSERT_MSG(iLocalVersion != VHAVOKWORLDPARAMS_VERSION_5, "Version 5 was an intermediate version and not currently supported.  Please regenerate your physics settings.");


    VASSERT(iLocalVersion>VHAVOKWORLDPARAMS_VERSION_0 && iLocalVersion<=VHAVOKWORLDPARAMS_VERSION_CURRENT);
    file.ReadFloat(g_vGlobalWorldParams.m_setupParams.m_fHavokToVisionScale);
    file.ReadInt(g_vGlobalWorldParams.m_setupParams.m_iStaticGeomMode);
    file.ReadBOOL(g_vGlobalWorldParams.m_runtimeParams.m_bBroadphaseAutoCompute);

  if( iLocalVersion == VHAVOKWORLDPARAMS_VERSION_5 )
    {
    //hopefully this never gets hit and we can just remove it
    VASSERT(0);
      double tmp[4]; file.Read(tmp, sizeof(double)*4, "dddd");
    g_vGlobalWorldParams.m_runtimeParams.m_fBroadphaseManualSize = float(tmp[0]);
    }
    else
    {
    file.ReadFloat(g_vGlobalWorldParams.m_runtimeParams.m_fBroadphaseManualSize);
    }

    file.Read(&g_vGlobalWorldParams.m_runtimeParams.m_vGravity, sizeof(float)*3, "fff");
    file.Read(g_vGlobalWorldParams.m_runtimeParams.m_iCollisionGroupMasks, sizeof(int)*32, "32i");

    if ( iLocalVersion >= VHAVOKWORLDPARAMS_VERSION_2 )
      {
      file.ReadBOOL(g_vGlobalWorldParams.m_runtimeParams.m_bEnableConstraintCollisionFilter);
      }
      if ( iLocalVersion >= VHAVOKWORLDPARAMS_VERSION_3 )
      {
        file.ReadBOOL(g_vGlobalWorldParams.m_runtimeParams.m_bEnableLegacyCompoundShapes);
    }
    if ( iLocalVersion >= VHAVOKWORLDPARAMS_VERSION_4 )
    {
      file.ReadBOOL(g_vGlobalWorldParams.m_runtimeParams.m_bEnableDiskShapeCaching);
    }

    if ( iLocalVersion == VHAVOKWORLDPARAMS_VERSION_5 )
    {
        // This was introduced in the Restructuring -> 2013.1 merge

        BOOL dummyBool; int dummyInt;
        file.ReadBOOL(dummyBool);  //m_bFlushVisionCollisionMeshes
        file.ReadBOOL(dummyBool);  //m_bForceCollisionMeshRegeneration
        file.ReadBOOL(dummyBool);  //m_bEnableStaticShapeCacheFlush
        file.ReadInt(dummyInt);    //m_iNumStaticMeshRemovesForCacheFlush
    }

    if ( iLocalVersion >= VHAVOKWORLDPARAMS_VERSION_6 )
    {
      file.ReadInt(g_vGlobalWorldParams.m_setupParams.m_iMergedStaticWeldingType);
    }

    if ( iLocalVersion >= VHAVOKWORLDPARAMS_VERSION_7 )
    {
      file.ReadInt(g_vGlobalWorldParams.m_runtimeParams.m_solverIterations);
      file.ReadInt(g_vGlobalWorldParams.m_runtimeParams.m_solverHardness);
    }

    vHavokConversionUtils::SetHavok2VisionScale( g_vGlobalWorldParams.m_setupParams.m_fHavokToVisionScale);
    m_staticGeomMode = (StaticGeomMode)g_vGlobalWorldParams.m_setupParams.m_iStaticGeomMode;
    m_mergedStaticWeldingType = (VisWeldingType_e)g_vGlobalWorldParams.m_setupParams.m_iMergedStaticWeldingType;

    // The broadphase size is set in the OnAfterSceneLoaded callback, since only there it is ensured,
    // that all static physics objects already exist, which are used for the automatic computation mode.
    SetGravity(g_vGlobalWorldParams.m_runtimeParams.m_vGravity);
    UpdateGroupsCollision();
  }
  else
  {
    file.StartChunk(iID);
    file.WriteChar(iLocalVersion);

    file.WriteFloat(g_vGlobalWorldParams.m_setupParams.m_fHavokToVisionScale);
    file.WriteInt(g_vGlobalWorldParams.m_setupParams.m_iStaticGeomMode);
    file.WriteBOOL(g_vGlobalWorldParams.m_runtimeParams.m_bBroadphaseAutoCompute);

    file.WriteFloat(g_vGlobalWorldParams.m_runtimeParams.m_fBroadphaseManualSize);
    file.Write( &g_vGlobalWorldParams.m_runtimeParams.m_vGravity, sizeof(float)*3, "fff");
    file.Write( g_vGlobalWorldParams.m_runtimeParams.m_iCollisionGroupMasks, sizeof(int)*32, "32i");
      file.WriteBOOL(g_vGlobalWorldParams.m_runtimeParams.m_bEnableConstraintCollisionFilter);
      file.WriteBOOL(g_vGlobalWorldParams.m_runtimeParams.m_bEnableLegacyCompoundShapes);
    file.WriteBOOL(g_vGlobalWorldParams.m_runtimeParams.m_bEnableDiskShapeCaching);

	  // This was introduced in the Restructuring -> 2013.1 merge
	  // Leaving it commented out in case we need to introduce dummy variables
	  // but no one should really be using the code at the moment, so just removing it
	  // There are safeguards on the read side though.

	  //file.WriteBOOL(g_vGlobalWorldParams.m_setupParams.m_bFlushVisionCollisionMeshes);
      //file.WriteBOOL(g_vGlobalWorldParams.m_setupParams.m_bForceCollisionMeshRegeneration);
      //file.WriteBOOL(g_vGlobalWorldParams.m_runtimeParams.m_bEnableStaticShapeCacheFlush);
      //file.WriteInt(g_vGlobalWorldParams.m_runtimeParams.m_iNumStaticMeshRemovesForCacheFlush);

    file.WriteInt(g_vGlobalWorldParams.m_setupParams.m_iMergedStaticWeldingType);

    file.WriteInt(g_vGlobalWorldParams.m_runtimeParams.m_solverIterations);
    file.WriteInt(g_vGlobalWorldParams.m_runtimeParams.m_solverHardness);

    file.EndChunk();
  }
}

void vHavokPhysicsModule::StartDeterminismWriteMode(const char* filename)
{
   hkCheckDeterminismUtil::getInstance().startWriteMode(filename);
}

void vHavokPhysicsModule::StartDeterminismCheckMode(const char* filename)
{
   hkCheckDeterminismUtil::getInstance().startCheckMode(filename);
}

void vHavokPhysicsModule::FinishDeterminismTest()
{
   hkCheckDeterminismUtil::getInstance().finish();
}

// Apply a single force to a point to the given rigid body.
void vHavokPhysicsModule::ApplyForce(vHavokRigidBody *pRB, const hkvVec3& Position, const hkvVec3& v3DirForce)
{
  float fTime = Vision::GetTimer()->GetTimeDifference();
  if (pRB==NULL || fTime<=0.f)
    return;

  pRB->ApplyForce(v3DirForce, Position, fTime);

}

// Apply one or more vertical forces at supplied specific points of the rigid bodies through a impulse accumulator object.
void vHavokPhysicsModule::ApplyVerticalForces(vHavokRigidBody *pRB, hkvVec4 *aForces, unsigned int iElems)
{
  float fTime = Vision::GetTimer()->GetTimeDifference();
  if (pRB==NULL || fTime<=0.f)
    return;

  hkpImpulseAccumulator myAccum(pRB->GetHkRigidBody(), fTime);
  for (unsigned int i = 0; i < iElems; i++)
  {
    hkVector4 hkVec4Pos;
    hkVector4 hkVec4Force;
    if(aForces[i].w == 0.0f) continue;
    vHavokConversionUtils::VisVecToPhysVecWorld(hkvVec4(aForces[i].x, aForces[i].y, aForces[i].z, 0.0f), hkVec4Pos);
    vHavokConversionUtils::VisVecToPhysVec_noscale(hkvVec4(0.0f, 0.0f, aForces[i].w, 0.0f), hkVec4Force);
    
    myAccum.addForce(hkVec4Pos, hkVec4Force);
  }

  MarkForWrite();
  myAccum.flushImpulses();
  UnmarkForWrite();
  
}


void Helper_AddRBToBBox(hkpRigidBody *pRB, hkvAlignedBBox &bbox)
{
  if (pRB==NULL)
    return;
  hkpRigidBodyCinfo info;
  pRB->getCinfo(info);
  if (info.m_shape==NULL)
    return;
  hkAabb aabb;
  info.m_shape->getAabb(pRB->getTransform(), 0.0f, aabb);

  hkvAlignedBBox physBox;
  vHavokConversionUtils::PhysVecToVisVecWorld(aabb.m_min, physBox.m_vMin);
  vHavokConversionUtils::PhysVecToVisVecWorld(aabb.m_max, physBox.m_vMax);

  if (physBox.isValid())
    bbox.expandToInclude(physBox);
}

void vHavokPhysicsModule::TriggerHavokResourceCallback(vHavokResourceCallbackData &data)
{
  // trigger global callback so other modules can respond to it
  data.m_pSender = &OnHandleResourceFile;
  data.m_pHavokModule = this;
  data.Trigger();
 
  // handle the physics side in this function:
  hkRootLevelContainer* root = data.m_pResource->getContents<hkRootLevelContainer>();
  if (root==NULL || data.m_Action==vHavokResourceCallbackData::HRA_NONE)
    return;
  int iProfilingID;
  if (data.m_Action==vHavokResourceCallbackData::HRA_CREATE_INSTANCES)
  {
    iProfilingID = PROFILING_HAVOK_PHYSICS_CREATESHAPES;
  } else
  {
    iProfilingID = PROFILING_HAVOK_PHYSICS_REMOVESHAPES;
  }
  VISION_PROFILE_FUNCTION(iProfilingID);

  hkpPhysicsData* physicsData = static_cast<hkpPhysicsData*>( root->findObjectByType( hkpPhysicsDataClass.getName() ) );
  if (physicsData==NULL)
    return;

  const hkArray<hkpPhysicsSystem*>& systems = physicsData->getPhysicsSystems();

  if (systems.getSize()==0)
    return;

  hkpWorld *pWorld = vHavokPhysicsModule::GetInstance()->GetPhysicsWorld();

  MarkForWrite();
  for (int i=0;i<systems.getSize();i++)
  {
    hkpPhysicsSystem *pSystem = systems[i];
    if (data.m_Action==vHavokResourceCallbackData::HRA_CREATE_INSTANCES)
    {
      // add physics system to world
      pWorld->addPhysicsSystem(pSystem);
    }
    else if (data.m_Action==vHavokResourceCallbackData::HRA_DELETE_INSTANCES)
    {
      pWorld->removePhysicsSystem(pSystem);
    }
  }
  UnmarkForWrite();
}


void vHavokPhysicsModule::OnResourceShapeLoaded(vHavokResourceShape *pShape)
{
  VMutexLocker locked(m_MutexResourceShapeHandling);

  m_ResourceShapesToInstantiate.AddUnique(pShape);
  pShape->m_bQueued = true;

  // evaluate bounding box here (vForge needs it)
  if (!pShape->m_BoundingBox.isValid())
  {
    hkRootLevelContainer* root = pShape->m_spHavokResource->getContents<hkRootLevelContainer>();
    if (root==NULL)
      return;
    hkpPhysicsData* physicsData = static_cast<hkpPhysicsData*>( root->findObjectByType( hkpPhysicsDataClass.getName() ) );
    if (physicsData==NULL)
      return;
    const hkArray<hkpPhysicsSystem*>& systems = physicsData->getPhysicsSystems();
    for (int i=0;i<systems.getSize();i++)
    {
      hkpPhysicsSystem *pSystem = systems[i];
      // add objects to bounding box (for vForge)
      const hkArray<hkpRigidBody*>& rbodies =  pSystem->getRigidBodies();
      for (int j=0;j<rbodies.getSize();j++)
        Helper_AddRBToBBox(rbodies[j], pShape->m_BoundingBox);
    }
  }

}

void vHavokPhysicsModule::OnResourceShapeDestroyed(vHavokResourceShape *pShape)
{
  m_MutexResourceShapeHandling.Lock();
  m_ResourceShapesToInstantiate.SafeRemove(pShape);
  pShape->m_bQueued = false;
  m_MutexResourceShapeHandling.Unlock();
}


void vHavokPhysicsModule::ProcessLoadedResourceShapes()
{
  m_MutexResourceShapeHandling.Lock();

  const int iCount = m_ResourceShapesToInstantiate.Count();
  for (int i=0;i<iCount;i++)
  {
    vHavokResourceShape *pShape = m_ResourceShapesToInstantiate.GetAt(i);
    vHavokResourceCallbackData cbdata(pShape->m_spHavokResource, vHavokResourceCallbackData::HRA_CREATE_INSTANCES);
    TriggerHavokResourceCallback(cbdata);
    pShape->m_BoundingBox = cbdata.m_BoundingBox;
    pShape->m_bInstantiated = true;
    pShape->m_bQueued = false;
  }

  m_ResourceShapesToInstantiate.Clear();
  m_MutexResourceShapeHandling.Unlock();
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
