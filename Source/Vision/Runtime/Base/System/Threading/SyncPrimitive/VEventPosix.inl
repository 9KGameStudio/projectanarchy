/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */



VEvent::VEvent(VEvent::ResetMode_e eResetMode) : m_eResetMode(eResetMode)
{
  m_bEventCondition = false;
  pthread_mutexattr_t mutexAttributes;
  pthread_mutexattr_init(&mutexAttributes);
  int iRes = pthread_mutex_init(&m_Mutex, &mutexAttributes);
  VASSERT(iRes == 0);

  iRes = pthread_cond_init(&m_Condition, NULL);
  VASSERT(iRes == 0);
}

VEvent::~VEvent()
{
  pthread_cond_destroy(&m_Condition);
  pthread_mutex_destroy(&m_Mutex);
}

void VEvent::Wait()
{
  pthread_mutex_lock(&m_Mutex);
  while(!m_bEventCondition)
  {
    pthread_cond_wait(&m_Condition, &m_Mutex);
  }

  if(m_eResetMode == AUTO_RESET)
  {
    m_bEventCondition = false;
  }

  pthread_mutex_unlock(&m_Mutex);
}

VEvent::WaitResult_e VEvent::Wait(VTimeSpan timeOut)
{
  pthread_mutex_lock(&m_Mutex);

  if(m_bEventCondition)
  {
    pthread_mutex_unlock(&m_Mutex);
    return SIGNALED;
  }

  struct timeval now;
  gettimeofday(&now, NULL);

  // pthread_cond_timedwait needs an absolute time value, so compute
  // it from the current time.
  struct timespec timeToWait;

  const __int64 iNanoSecondsPerSecond = 1000000000LL;
  const __int64 iMicroSecondsPerNanoSecond = 1000LL;

  __int64 endTime = now.tv_sec * iNanoSecondsPerSecond + now.tv_usec * iMicroSecondsPerNanoSecond + static_cast<__int64>(timeOut.TotalNanoSeconds());

  timeToWait.tv_sec = endTime / iNanoSecondsPerSecond;
  timeToWait.tv_nsec = endTime % iNanoSecondsPerSecond;

  int iResult = pthread_cond_timedwait(&m_Condition, &m_Mutex, &timeToWait);

  if(iResult == ETIMEDOUT)
  {
    pthread_mutex_unlock(&m_Mutex);
    return TIMEOUT;
  }
  else
  {
    VASSERT_MSG(iResult == 0, "Wait failed");

    if(m_eResetMode == AUTO_RESET)
    {
      m_bEventCondition = false;
    }

    pthread_mutex_unlock(&m_Mutex);
    return SIGNALED;
  }
}

void VEvent::Signal()
{
  pthread_mutex_lock(&m_Mutex);
  m_bEventCondition = true;
  pthread_cond_signal(&m_Condition);
  pthread_mutex_unlock(&m_Mutex);
}

void VEvent::Reset()
{
  pthread_mutex_lock(&m_Mutex);
  m_bEventCondition = false;
  pthread_mutex_unlock(&m_Mutex);
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
