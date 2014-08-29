/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// Win32 Condition variable Implementation based on "Strategies for Implementing POSIX Condition Variables on Win32" (http://www.cs.wustl.edu/~schmidt/win32-cv-1.html)

VConditionVariable::VConditionVariable()
{
  m_iNumWaiters = 0;
  m_bWasBroadcast = 0;
  m_waiterQueueSemaphore = CreateSemaphore(NULL, 0, 0x7fffffff, NULL);
  InitializeCriticalSection(&m_waitersCountLock);
  m_waitersDone = CreateEvent(NULL, FALSE, FALSE, NULL);
  m_mutex = CreateMutex(NULL, FALSE, NULL);

#if defined(HK_DEBUG)
  m_bIsLocked = false;
#endif
}

VConditionVariable::~VConditionVariable()
{
  CloseHandle(m_waiterQueueSemaphore);
  CloseHandle(m_waitersDone);
  CloseHandle(m_mutex);
  DeleteCriticalSection(&m_waitersCountLock);
}

void VConditionVariable::Lock()
{
  WaitForSingleObject(m_mutex, INFINITE);

#if defined(HK_DEBUG)
  m_bIsLocked = true;
#endif
}

void VConditionVariable::Unlock()
{
#if defined(HK_DEBUG)
  m_bIsLocked = false;
#endif

  ReleaseMutex(m_mutex);
}

void VConditionVariable::Signal()
{
  EnterCriticalSection (&m_waitersCountLock);
  bool bHaveWaiters = m_iNumWaiters > 0;
  LeaveCriticalSection (&m_waitersCountLock);

  // If there aren't any waiters, then this is a no-op.  
  if (bHaveWaiters)
    ReleaseSemaphore(m_waiterQueueSemaphore, 1, 0);
}

void VConditionVariable::Broadcast()
{
  // This is needed to ensure that <waiters_count_> and <was_broadcast_> are
  // consistent relative to each other.
  EnterCriticalSection (&m_waitersCountLock);
  bool bHaveWaiters = 0;

  if (m_iNumWaiters > 0) {
    // We are broadcasting, even if there is just one waiter...
    // Record that we are broadcasting, which helps optimize
    // <pthread_cond_wait> for the non-broadcast case.
    m_bWasBroadcast = 1;
    bHaveWaiters = 1;
  }

  if (bHaveWaiters) {
    // Wake up all the waiters atomically.
    ReleaseSemaphore (m_waiterQueueSemaphore, m_iNumWaiters, 0);

    LeaveCriticalSection (&m_waitersCountLock);

    // Wait for all the awakened threads to acquire the counting
    // semaphore. 
    WaitForSingleObject (m_waitersDone, INFINITE);
    // This assignment is okay, even without the <waiters_count_lock_> held 
    // because no other waiter threads can wake up to access it.
    m_bWasBroadcast = 0;
  }
  else
    LeaveCriticalSection (&m_waitersCountLock);
}

VConditionVariable::WaitResult_e VConditionVariable::Wait(VTimeSpan timeOut)
{
#if defined(HK_DEBUG)
  VASSERT_MSG(m_bIsLocked, "Condition variable needs to be locked using VScopedLock first");
#endif

  // Avoid race conditions.
  EnterCriticalSection (&m_waitersCountLock);
  m_iNumWaiters++;
  LeaveCriticalSection (&m_waitersCountLock);

  DWORD waitTime = timeOut == VTimeSpan::Infinite() ? INFINITE : static_cast<DWORD>(timeOut.TotalMilliSeconds());

  // This call atomically releases the mutex and waits on the
  // semaphore until Signal or Broadcast
  // are called by another thread.
  DWORD result = SignalObjectAndWait(m_mutex, m_waiterQueueSemaphore, waitTime, FALSE);

  bool bTimeout = result == WAIT_TIMEOUT;

  // Reacquire lock to avoid race conditions.
  EnterCriticalSection (&m_waitersCountLock);

  // We're no longer waiting...
  m_iNumWaiters--;

  // Check to see if we're the last waiter after <pthread_cond_broadcast>.
  int last_waiter = m_bWasBroadcast && m_iNumWaiters == 0;

  LeaveCriticalSection (&m_waitersCountLock);

  // If we're the last waiter thread during this particular broadcast
  // then let all the other threads proceed.
  if (last_waiter)
    // This call atomically signals the <m_waitersDone> event and waits until
      // it can acquire the <lock>.  This is required to ensure fairness. 
        SignalObjectAndWait (m_waitersDone, m_mutex, INFINITE, FALSE);
  else
    // Always regain the external mutex since that's the guarantee we
    // give to our callers. 
    WaitForSingleObject(m_mutex, INFINITE);

  return bTimeout ? TIMEOUT : SIGNALED;
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
