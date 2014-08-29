/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

VConditionVariable::VConditionVariable()
{ 
  int iRes = pthread_cond_init(&m_cond, NULL);
  VASSERT(iRes == 0);

  pthread_mutexattr_t mutexAttributes;
  pthread_mutexattr_init(&mutexAttributes);
  pthread_mutexattr_settype(&mutexAttributes, PTHREAD_MUTEX_RECURSIVE);

  iRes = pthread_mutex_init(&m_mutex, &mutexAttributes);
  pthread_mutexattr_destroy(&mutexAttributes);
  VASSERT(iRes == 0);

#if defined(HK_DEBUG)
  m_bIsLocked = false;
#endif
}

VConditionVariable::~VConditionVariable()
{
  pthread_cond_destroy(&m_cond);
  pthread_mutex_destroy(&m_mutex);
}

void VConditionVariable::Lock()
{
  pthread_mutex_lock(&m_mutex);
#if defined(HK_DEBUG)
  m_bIsLocked = true;
#endif
}

void VConditionVariable::Unlock()
{
#if defined(HK_DEBUG)
  m_bIsLocked = false;
#endif
  pthread_mutex_unlock(&m_mutex);
}

void VConditionVariable::Signal()
{
  pthread_cond_signal(&m_cond);
}

void VConditionVariable::Broadcast()
{
  pthread_cond_broadcast(&m_cond);
}

VConditionVariable::WaitResult_e VConditionVariable::Wait(VTimeSpan timeOut)
{
  VASSERT_MSG(m_bIsLocked, "Condition variable needs to be locked using VScopedLock first");

  if(timeOut == VTimeSpan::Infinite())
  {
    pthread_cond_wait(&m_cond, &m_mutex);
    return SIGNALED;
  }
  else
  {
    timeval now;
    gettimeofday(&now, NULL);

    // pthread_cond_timedwait needs an absolute time value, so compute
    // it from the current time.
    struct timespec timeToWait;

    const __int64 iNanoSecondsPerSecond = 1000000000LL;
    const __int64 iMicroSecondsPerNanoSecond = 1000LL;

    __int64 endTime = now.tv_sec * iNanoSecondsPerSecond + now.tv_usec * iMicroSecondsPerNanoSecond + static_cast<__int64>(timeOut.TotalNanoSeconds());

    timeToWait.tv_sec = endTime / iNanoSecondsPerSecond;
    timeToWait.tv_nsec = endTime % iNanoSecondsPerSecond;

    int iResult = pthread_cond_timedwait(&m_cond, &m_mutex, &timeToWait);

    if(iResult == ETIMEDOUT)
    {
      return TIMEOUT;
    }
    else
    {
      return SIGNALED;
    }
  }
}

/*
 * Havok SDK - Base file, BUILD(#20140625)
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
