/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file 

#ifndef HKVLOGWRITER_RESOURCEVIEWER_H_INCLUDED
#define HKVLOGWRITER_RESOURCEVIEWER_H_INCLUDED

#include <Vision/Runtime/Base/Logging/hkvLog.h>

#ifdef WIN32

namespace hkvLogWriter
{
  /// \brief A log writer for hkvGlobalLog that outputs to the resource viewer panel.
  class ResourceViewer
  {
  public:
    /// \brief Helper struct to cache messages while no connection to the resource viewer panel is available.
    struct CachedMsg
    {
      hkvLogMsgType::Enum m_MsgType;
      VString m_sText;
      VString m_sTag;
      int m_iIndentation;
    };

    /// \brief Call this once at startup to initialize log writers of this type.
    VBASE_IMPEXP static void Initialize();

    /// \brief Allows to modify the window handle that points to the resource viewer log panel.
    VBASE_IMPEXP static void ChangeLogWindow(HWND hLogWnd);

    /// \brief Register this function at hkvGlobalLog. pPassThrough is ignored.
    VBASE_IMPEXP static void EventHandler(hkvLogMsgType::Enum MsgType, const char* szText, int iIndentation, const char* szTag, void* pPassThrough);

    /// \brief Will make sure all cached messages are really sent to the log window.
    VBASE_IMPEXP static void PurgeCachedMessages();

  private:
    

    static bool s_bPurgingMessages;
    static VArray<CachedMsg> s_CachedMessages;
    static VMutex s_Mutex;
  };
}

#endif

#endif

/*
 * Havok SDK - Base file, BUILD(#20131218)
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
