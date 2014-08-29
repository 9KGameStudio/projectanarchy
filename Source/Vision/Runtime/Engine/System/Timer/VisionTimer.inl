/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisionTimer.inl

//Inlines
inline void IVTimer::Disable()
{
  m_bDisabled = true;
}

inline void IVTimer::Enable()
{
  m_bDisabled = false;
  m_bFirstStart = true;
}

inline float IVTimer::GetTime() const
{
  return m_fTime;
}

inline float IVTimer::GetTimeDifference() const
{
  return m_fTimeDifference;
}

inline void IVTimer::SetTimeDifference(float fDiff)
{
  m_fTimeDifference = fDiff;
}

inline bool IVTimer::IsDisabled() const
{
  return m_bDisabled;
}

inline int IVTimer::GetForcedFrameRate() const
{
  if (m_iForcedCountNumber == 0)
    return 0;
  else
    return int(GetClockFreq() / m_iForcedCountNumber);
}

inline void IVTimer::SkipOneFrame()
{
  m_bFirstStart = TRUE;
}

inline void IVTimer::SetMaxTimeDifference(float fMaxDifference)
{
  m_fMaxTimeDifference = fMaxDifference;
}

inline float IVTimer::GetMaxTimeDifference() const
{
  return m_fMaxTimeDifference;
}

inline void IVTimer::SetSlowMotion(bool bEnable)
{
  m_bSlowMotionEnabled = bEnable;
}

inline bool IVTimer::GetSlowMotion() const
{
  return m_bSlowMotionEnabled;
}

inline void IVTimer::SetSlowMotionTimeScale(float fTimeScale)
{
  m_fSlowMotionScale = fTimeScale;
}

inline float IVTimer::GetSlowMotionTimeScale() const
{
  return m_fSlowMotionScale;
}

inline uint64 IVTimer::GetClockFreq() const
{
  return VGLGetTimerResolution();
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
