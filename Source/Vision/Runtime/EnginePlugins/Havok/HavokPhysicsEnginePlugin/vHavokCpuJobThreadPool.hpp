/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VHAVOKCPUTHREADPOOL_HPP_INCLUDED
#define VHAVOKCPUTHREADPOOL_HPP_INCLUDED

#include <Common/Base/Thread/Job/ThreadPool/hkJobThreadPool.h>
#include <Common/Base/Thread/Semaphore/hkSemaphore.h>
#include <Common/Base/Thread/Thread/hkThread.h>

/// The construction info for the vHavokCpuJobThreadPool
class VisCallback_cl;

/// \brief
///   Thread pool construction parameters.
struct vHavokCpuJobThreadPoolCinfo
{
  HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_UTILITIES, vHavokCpuJobThreadPoolCinfo );

  /// \brief
  ///   The number of threads (defaults to 1)
  int m_numThreads;

  /// \brief
  ///   The Havok Physics stack size for each thread (defaults to 4 Mb)
  int m_havokStackSize;

  /// \brief
  ///   The program stack size for each thread
  ///
  /// On Win32 and Xbox 360, this defaults to 0, meaning the thread inherits the executable's stack size
  /// On PlayStation(R) 3, this defaults to 256K
  /// Has no effect on other platforms (e.g. Posix)
  int m_stackSize;

  /// \brief
  ///   This is the buffer size allocated in each thread for collecting timer
  ///   information. 
  ///
  /// It defaults to 0, which means timers will be disabled.
  /// To view timers in the VDB, you need to set this buffer size to a non zero value.
  /// 2000000 (2 Mb) is a recommended size. Smaller buffers are ok, but you may
  /// lose timer info.
  int m_timerBufferPerThreadAllocation;

  /// \brief
  ///   Processor thread assignments.
  ///
  /// If this data is used to set the hardware thread IDs, on XBox360, XSetThreadProcessor is called on each thread
  /// with the hardware ID. By default this array is set to {2, 4, 1, 3, 5, 0} (i.e. the first 3 threads
  /// are set to separate cores).
  /// On Windows, it will use SetThreadIdealProcessor, and will use {1, 2..., (numProcessors-1), 0} as a default. 
  /// The two default value schemes assume that you are using core 0 as your main calling thread (that will do work too).
  hkArray<int> m_hardwareThreadIds;

  /// \brief
  ///   The thread name to be passed to hkThread::startThread
  const char* m_threadName;

  // Pointers to the physics modules callback members (so they can be modified).
  VisCallback_cl *m_OnWorkerThreadCreatedPtr;
  VisCallback_cl *m_OnWorkerThreadFinishedPtr;

  /// \brief
  ///   Default constructor.
  vHavokCpuJobThreadPoolCinfo();
};

/// \brief
///   Utility class for running Havok Physics in multiple threads.
///
/// The utility creates a pool of threads, and suspends them with a semaphore. On each call to
/// startStep the main thread resumes/releases all physics threads.
class vHavokCpuJobThreadPool : public hkJobThreadPool
{
public:
  HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_UTILITIES);

  enum { MAX_NUM_THREADS = 12 };

  /// \brief
  ///   Initializes multi-threading state and creates all threads for the pool.
  ///
  /// This will also call hkReferencedObject::setLockMode(hkReferencedObject::LOCK_MODE_AUTO)
  ///
  /// \param cinfo
  ///   construction info for initializing this instance
  ///
  vHavokCpuJobThreadPool(const vHavokCpuJobThreadPoolCinfo& cinfo);

  /// \brief
  ///   Destroys threads and deletes the state.
  ///
  /// This will also call hkReferencedObject::setLockMode(hkReferencedObject::LOCK_MODE_NONE)
  ///
  virtual ~vHavokCpuJobThreadPool();

  /// \brief
  ///   Process jobs using all threads until all jobs on the queue have completed.
  virtual void processAllJobs(hkJobQueue* queue, hkJobType firstJobType_unused = HK_JOB_TYPE_MAX) HKV_OVERRIDE;

  /// \brief
  ///   Waits until all threads have emptied the Queue.
  virtual void waitForCompletion() HKV_OVERRIDE;

  /// \brief
  ///   Returns true if processing of jobs has started (using processAllJobs)
  ///   but waitForCompletion has not been called yet.
  ///
  /// \sa processAllJobs waitForCompletion
  virtual bool isProcessing() const HKV_OVERRIDE;

  /// \brief
  ///   Returns the timer data that has been collected during processJobs.
  virtual void appendTimerData(hkArrayBase<hkTimerData>& timerDataOut, hkMemoryAllocator& alloc) HKV_OVERRIDE;

  /// \brief
  ///   Clears the timer data. This must be done every frame, otherwise the timers will overflow.
  virtual void clearTimerData() HKV_OVERRIDE;

  /// \brief
  ///   Returns the number of threads currently running
  virtual int getNumThreads() const HKV_OVERRIDE;

  /// \brief
  ///   Sets the number of threads to use.
  ///
  /// \param numThreads
  ///   The new number of threads.
  /// \return 
  ///   The number of threads actually set.
  virtual void setNumThreads(int numThreads) HKV_OVERRIDE;

  // Cause threads to garbage collect their memory immediately before signaling completion.
  virtual void gcThreadMemoryOnNextCompletion() HKV_OVERRIDE;

protected:
  void addThread();
  void removeThread();

public:
  struct SharedThreadData;

  /// \brief
  ///   Thread state data. Used only by one worker thread
  struct WorkerThreadData
  {
    HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_ENTITY, WorkerThreadData);

    /// \brief
    ///   Default constructor.
    WorkerThreadData();

    /// \brief
    ///   Pointer to the data shared between threads.
    SharedThreadData* m_sharedThreadData;

    /// \brief
    ///   System handle to the thread.
    hkThread m_thread;

    /// \brief
    ///   Thread ID (from 0 - N)
    int m_threadId;

    int m_hardwareThreadId;

    /// \brief
    ///   Flag is set to true if there is a request for the thread to be closed.
    bool m_killThread;

    bool m_clearTimers;

    /// \brief
    ///   Semaphore used to pause a physics thread after it has done its calculations.
    ///
    /// This semaphore is released by the main thread on every simulation step.
    hkSemaphore m_semaphore;

    /// \brief
    ///   Internal buffer used for collecting and copying Havok Physics' timer information from a
    ///   physics thread back to the main thread.
    char* m_monitorStreamBegin;

    /// \brief
    ///   Internal buffer used for collecting and copying Havok Physics' timer information from a
    ///   physics thread back to the main thread.
    char* m_monitorStreamEnd;
  };

  /// \brief
  ///   Data shared by all threads.
  struct SharedThreadData
  {
    HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_UTILITIES, vHavokCpuJobThreadPool::SharedThreadData);

    /// \brief
    ///   Default constructor.
    SharedThreadData();

    /// \brief
    ///   The job queue associated with the thread pool.
    hkJobQueue* m_jobQueue;

    /// \brief
    ///   Semaphore used to pause the main thread when it waits for threads
    ///   to finish their calculations.
    hkSemaphore m_workerThreadFinished;

    /// \brief
    ///   Number of threads.
    int m_numThreads;

    int m_localHavokStackSize;

    int m_timerBufferAllocation;

    hkBool m_gcThreadMemoryOnCompletion;

    // VISION SPECIFIC:
    VisCallback_cl *m_OnWorkerThreadCreatedPtr;
    VisCallback_cl *m_OnWorkerThreadFinishedPtr;

    /// Critical section to protect against parallel activation of callback 
    /// functions by multiple worker threads
    hkCriticalSection m_CallbackProtect;
  };

public:
  /// \brief
  ///   Data local to each physics thread.
  WorkerThreadData m_workerThreads[MAX_NUM_THREADS];

protected:
  /// \brief
  ///   Data shared by all physics threads.
  SharedThreadData m_sharedThreadData;

  /// \brief
  ///   Debugging flag: Set to true when worker threads are stepping.
  hkBool m_isRunning;

  /// \brief
  ///   String for thread names (depending on platform support).
  const char* m_threadName;

  /// \brief
  ///   Thread stack size.
  ///
  /// \sa vHavokCpuJobThreadPoolCinfo::m_stackSize
  int m_stackSize;
};

#endif // HK_BASE_CPU_THREAD_POOL_H

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
