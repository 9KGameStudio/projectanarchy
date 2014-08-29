/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VMutex.hpp

#ifndef VCONDITION_HPP_INCLUDED
#define VCONDITION_HPP_INCLUDED

#include <Vision/Runtime/Base/System/Threading/SyncPrimitive/VMutex.hpp>

/// \brief
///   pthreads-style condition variable consisting of a mutex and condition.
class VConditionVariable
{
  friend class VScopedLock;

public:
  /// \brief
  ///  Specifies the result of waiting for a condition with a timeout.
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
  ///   Constructor; initializes the condition variable.
  inline VConditionVariable();

  /// \brief
  ///   Destructor; deinitializes the condition variable.
  inline  ~VConditionVariable();

  ///
  /// @}
  ///

  /// \brief Waits until the condition variable is signaled.
  ///
  /// Prior to waiting, the condition variable must be locked by using a VScopedLock.
  ///
  /// \example
  ///   \code
  ///  
  ///   VScopedLock lock(m_condition);
  ///   m_condition.Wait();
  ///   
  ///   \endcode
  ///
  /// \param timeOut A timespan after which waiting is aborted, or VTimeSpan::Infinite() to never timeout.
  /// \returns VConditionVariable::SIGNALED if the condition was signaled, or VConditionVariable::TIMEDOUT if a timeout occurred.
  inline WaitResult_e Wait(VTimeSpan timeOut = VTimeSpan::Infinite());

  /// \brief Wakes up a single thread waiting on the condition.
  inline void Signal();

  /// \brief Wakes up a all threads waiting on the condition.
  inline void Broadcast();

private:
  inline void Lock();
  inline void Unlock();

  // platform specific members
  #if defined(_VISION_WIN32) 
    CRITICAL_SECTION m_waitersCountLock;
    HANDLE m_mutex;
    HANDLE m_waiterQueueSemaphore;
    HANDLE m_waitersDone;
    int m_iNumWaiters;
    bool m_bWasBroadcast;
  #elif defined(_VISION_POSIX)
    pthread_cond_t m_cond;
    pthread_mutex_t m_mutex;
   #elif defined(_VISION_WIIU)
    OSCond m_cond;
    OSMutex m_mutex;
  #else
    #error Undefined platform!
  #endif

#if defined(HK_DEBUG)
  bool m_bIsLocked;
#endif
};

#if defined(_VISION_WIN32)
  #include <Vision/Runtime/Base/System/Threading/SyncPrimitive/VConditionVariablePC.inl>
#elif defined(_VISION_POSIX)
  #include <Vision/Runtime/Base/System/Threading/SyncPrimitive/VConditionVariablePosix.inl>
#elif defined(_VISION_WIIU)
  #include <Vision/Runtime/Base/System/Threading/SyncPrimitive/VConditionVariableWiiU.inl>
#else
  #error Undefined platform!
#endif



#endif

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
