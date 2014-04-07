/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \brief
///   Simple performance timer to measure timings.
class VisPerformanceTimer_cl
{
public:
  /// \brief
  ///   Constructor of the performance timer class.
  VisPerformanceTimer_cl() { Reset(); }
  ~VisPerformanceTimer_cl() {}

  /// \brief
  ///   Resets the performance timer to the current time.
  inline void Reset()
  {
    m_uiStartTime = VGLGetTimer();
    m_uiLastTime = m_uiStartTime;
  }

  /// \brief
  ///   Returns the elapsed time since creation of the timer instance or the last call to reset.
  inline float GetTime() const
  {
    return float(VGLGetTimer() - m_uiStartTime) / float(VGLGetTimerResolution());
  }

  /// \brief
  ///   Returns the elapsed time since the last call to GetDiff().
  inline float GetDiff()
  {
    float fDiff = float(VGLGetTimer() - m_uiLastTime) / float(VGLGetTimerResolution());
    m_uiLastTime = VGLGetTimer();
    return fDiff;
  }

private:
  uint64 m_uiStartTime;
  uint64 m_uiLastTime;
};

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
