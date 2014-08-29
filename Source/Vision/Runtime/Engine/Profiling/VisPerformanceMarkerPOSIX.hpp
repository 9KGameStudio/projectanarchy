/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#if !(defined(HK_DEBUG) || defined(PROFILING))
  #error This file must only be included for profiling builds
#endif

#if !defined(_VISION_POSIX)
  #error This file must not be included on this platform!
#endif

#if defined(_VISION_IOS)

inline void SetPerfMarkerString(const char* pszString)
{
  glInsertEventMarkerEXT(0, pszString);
}

inline void StartPerfMarkerBracket(const char *pszString)
{
  glPushGroupMarkerEXT(0, pszString);
}

inline void StopPerfMarkerBracket(const char *pszString)
{
  glPopGroupMarkerEXT();
}

#define INSERT_PERF_MARKER(_markerString_) SetPerfMarkerString(_markerString_);
#define INSERT_PERF_MARKER_SCOPE(_markerString_) VisPerformanceMarker_cl ___visPerfMarker___(_markerString_);
#define START_PERF_MARKER_BRACKET(_markerString_) StartPerfMarkerBracket(_markerString_);
#define STOP_PERF_MARKER_BRACKET(_markerString_) StopPerfMarkerBracket(_markerString_);

#else

#define INSERT_PERF_MARKER(_markerString_)
#define INSERT_PERF_MARKER_SCOPE(_markerString_)
#define START_PERF_MARKER_BRACKET(_markerString_)
#define STOP_PERF_MARKER_BRACKET(_markerString_)

#endif

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
