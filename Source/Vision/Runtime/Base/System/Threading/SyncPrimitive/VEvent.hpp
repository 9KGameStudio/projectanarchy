/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VEvent.hpp

#ifndef VEVENT_HPP_INCLUDED
#define VEVENT_HPP_INCLUDED

#ifdef _VISION_PS3
#include <sys/ppu_thread.h>
#include <sys/synchronization.h>
#endif

/// \brief
///   Synchronization object class for events.
///
/// VEvent is a simple synchronization object that allows threads to wait for another thread.
class VEvent
{
public:

  /// \brief
  ///  Specifies whether an event is reset automatically when it is received by a waiting thread.
  enum ResetMode_e
  {
    AUTO_RESET,
    MANUAL_RESET
  };

  /// \brief
  ///  Specifies the result of waiting for an event with a timeout.
  enum WaitResult_e
  {
    SIGNALED,
    TIMEOUT
  };

  ///
  /// @name Constructor / Destructor
  /// @{
  ///

  /// \brief
  ///   Constructor; creates the event.
  inline VEvent(ResetMode_e eResetMode = AUTO_RESET);

  /// \brief
  ///   Destructor; destroys the event.
  inline ~VEvent();

  /// \brief
  ///   Waits for the event to be signaled
  inline void Wait();

  /// \brief
  ///   Waits for the event to be signaled
  inline WaitResult_e Wait(VTimeSpan timeOut);

  /// \brief
  ///   Signals the event
  inline void Signal();

  /// \brief
  ///  Manually resets the event
  inline void Reset();

private:

  #if defined(_VISION_WIN32)  || defined(_VISION_XENON)  
    volatile HANDLE m_hEvent;

  #elif defined(_VISION_PS3)
    sys_cond_t m_Condition;
    sys_mutex_t m_Mutex;
    volatile bool m_bEventCondition;
    ResetMode_e m_eResetMode;

  #elif defined(_VISION_POSIX)
    pthread_cond_t m_Condition;
    pthread_mutex_t m_Mutex;
    volatile bool m_bEventCondition;
    ResetMode_e m_eResetMode;

  #elif defined(_VISION_PSP2)
    SceUID m_Event;
    ResetMode_e m_eResetMode;

  #elif defined(_VISION_WIIU)
    OSEvent m_Event;

  #else
    #error "Missing Platform!"
  #endif

};

#if defined(_VISION_WIN32)
  #include <Vision/Runtime/Base/System/Threading/SyncPrimitive/VEventPC.inl>
#elif defined(_VISION_XENON)
  #include <Vision/Runtime/Base/System/Threading/SyncPrimitive/VEventXenon.inl>
#elif defined(_VISION_PS3)
  #include <Vision/Runtime/Base/System/Threading/SyncPrimitive/VEventPS3.inl>
#elif defined(_VISION_POSIX)
  #include <Vision/Runtime/Base/System/Threading/SyncPrimitive/VEventPosix.inl>
#elif defined(_VISION_PSP2)
  #include <Vision/Runtime/Base/System/Threading/SyncPrimitive/VEventPSP2.inl>
#elif defined(_VISION_WIIU)
  #include <Vision/Runtime/Base/System/Threading/SyncPrimitive/VEventWiiU.inl>
#else
  #error Undefined platform!
#endif


#endif  // VEVENT_HPP_INCLUDED

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
