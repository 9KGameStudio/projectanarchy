/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */



VEvent::VEvent(VEvent::ResetMode_e eResetMode)
{
#ifdef _VISION_WINRT
  DWORD dwFlags = STANDARD_RIGHTS_ALL | EVENT_MODIFY_STATE;
  if(eResetMode == MANUAL_RESET)
  {
      dwFlags |= CREATE_EVENT_MANUAL_RESET;
  }
  m_hEvent = CreateEventEx(NULL, NULL, 0, dwFlags);
#else
  m_hEvent = CreateEvent(NULL, eResetMode == MANUAL_RESET ? TRUE : FALSE, FALSE, NULL);
#endif
}

VEvent::~VEvent()
{
  CloseHandle(m_hEvent);
}

void VEvent::Wait()
{
#ifdef _VISION_WINRT
  DWORD dwResult = WaitForSingleObjectEx(m_hEvent, INFINITE, FALSE);
#else 
  DWORD dwResult = WaitForSingleObject(m_hEvent, INFINITE);
#endif

  VASSERT_MSG(dwResult == WAIT_OBJECT_0, "Wait failed");
}

VEvent::WaitResult_e VEvent::Wait(VTimeSpan timeOut)
{
#ifdef _VISION_WINRT
  DWORD dwResult = WaitForSingleObjectEx(m_hEvent, static_cast<DWORD>(timeOut.TotalMilliSeconds()), FALSE);
#else 
  DWORD dwResult = WaitForSingleObject(m_hEvent, static_cast<DWORD>(timeOut.TotalMilliSeconds()));
#endif

  if(dwResult == WAIT_TIMEOUT)
  {
    return TIMEOUT;
  }
  else 
  {
    VASSERT_MSG(dwResult == WAIT_OBJECT_0, "Wait failed");
    return SIGNALED;
  }
}

void VEvent::Signal()
{
  BOOL bRes = SetEvent(m_hEvent);
  VASSERT(bRes == TRUE);
}

void VEvent::Reset()
{
  ResetEvent(m_hEvent);
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
