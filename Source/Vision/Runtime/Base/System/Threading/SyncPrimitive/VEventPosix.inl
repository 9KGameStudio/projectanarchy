/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */



VEvent::VEvent()
{
  m_bEventCondition = false;
  pthread_mutexattr_t mutexAttributes;
  pthread_mutexattr_init(&mutexAttributes);
  pthread_mutexattr_settype(&mutexAttributes, PTHREAD_MUTEX_RECURSIVE);
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
    pthread_cond_wait(&m_Condition, &m_Mutex);
  m_bEventCondition = false;
  pthread_mutex_unlock(&m_Mutex);
}

void VEvent::Signal()
{
  pthread_mutex_lock(&m_Mutex);
  m_bEventCondition = true;
  pthread_cond_signal(&m_Condition);
  pthread_mutex_unlock(&m_Mutex);
}

/*
 * Havok SDK - Base file, BUILD(#20130717)
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2013
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
