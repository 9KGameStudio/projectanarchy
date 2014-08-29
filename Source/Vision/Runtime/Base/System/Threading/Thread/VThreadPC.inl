/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

VThread::VThread(VPlatformThreadFunc pStartFunction, void *pArgument, int iStackSize, VThreadPriority ePriority, const char* szThreadName) :
  m_pStartFunc(pStartFunction),
  m_bStarted(false),
  m_iProcessor(-1),
  m_ePriority(ePriority),
  m_iStackSize(iStackSize < 0 ? 0 : iStackSize)
{
  const DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack(push,8)
  typedef struct tagTHREADNAME_INFO
  {
    DWORD dwType; // Must be 0x1000.
    LPCSTR szName; // Pointer to name (in user addr space).
    DWORD dwThreadID; // Thread ID (-1=caller thread).
    DWORD dwFlags; // Reserved for future use, must be zero.
  } THREADNAME_INFO;
#pragma pack(pop)

  THREADNAME_INFO info;
  info.dwType = 0x1000;
  info.szName = szThreadName;
  info.dwFlags = 0;

  m_hHandle = CreateThread(NULL, m_iStackSize, m_pStartFunc, pArgument, CREATE_SUSPENDED, &m_threadId);
  VASSERT(m_hHandle);
  SetPriority(ePriority);

  info.dwThreadID = m_threadId;

  // See http://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx - How to: Set a Thread Name in Native Code
  __try
  {
    RaiseException( MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info );
  }
  __except (GetExceptionCode() == MS_VC_EXCEPTION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
  {
    // Do nothing
    (void*)0;
  }
}

VThread::~VThread()
{
  Join();
  CloseHandle(m_hHandle);
}

void VThread::Start()
{
  m_bStarted = true;
  ResumeThread(m_hHandle);
}

VPlatformThreadHandle VThread::GetHandle() const
{
  return m_threadId;
}

VPlatformThreadHandle VThread::GetCurrentThreadHandle()
{
  return GetCurrentThreadId();
}

void VThread::SetProcessor(int iProcessor)
{
  if (iProcessor == -1)
    iProcessor = MAXIMUM_PROCESSORS;
  SuspendThread(m_hHandle);

  #ifdef _VISION_WINRT
    PROCESSOR_NUMBER n;
    n.Group = 0;
    n.Number = iProcessor;
	n.Reserved = 0;
    BOOL bRes = SetThreadIdealProcessorEx(m_hHandle, &n, NULL);
    //XX Currently fails on July XDK :   VASSERT(bRes);
  #else
    int iRes = SetThreadIdealProcessor(m_hHandle, iProcessor);
    VASSERT(iRes>=0);
  #endif
  ResumeThread(m_hHandle);

  m_iProcessor = iProcessor;
}


/// \brief
///   Sets the thread priority
bool VThread::SetPriority(VThreadPriority ePriority)
{
  BOOL bRes = SetThreadPriority(m_hHandle, (int)ePriority);
  if (bRes)
  {
    m_ePriority = ePriority;
  }
  return bRes == TRUE;
}


void VThread::Join()
{
  if(m_bStarted)
  {
    WaitForSingleObject(m_hHandle, INFINITE);
    m_bStarted = false;
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
