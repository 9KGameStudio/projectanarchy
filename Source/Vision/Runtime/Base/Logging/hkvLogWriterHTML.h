/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file 

#ifndef HKVLOGWRITER_HTML_H_INCLUDED
#define HKVLOGWRITER_HTML_H_INCLUDED

#include <Vision/Runtime/Base/Logging/hkvLog.h>

namespace hkvLogWriter
{
  /// \brief A log writer for hkvGlobalLog that outputs to an HTML file.
  class HTML
  {
  public:
    /// \brief Constructor.
    HTML();

    /// \brief Opens the given file for logging.
    void BeginLogging(const char* szLogFile);

    /// \brief Ends writing log messages to the log file.
    void EndLogging();

    /// \brief This message is to be registered at hkvGlobalLog, using the pointer to an instance of this class for the pPassThrough parameter.
    VBASE_IMPEXP static void HandleLogMessage(hkvLogMsgType::Enum MsgType, const char* szText, int iIndentation, const char* szTag, void* pPassThrough)
    {
      HTML* pWriter = (HTML*) pPassThrough;
      pWriter->HandleLogMessage(MsgType, szText, szTag);
    }

  private:
    void HandleLogMessage(hkvLogMsgType::Enum MsgType, const char* szText, const char* szTag);

    FILE* m_pFile;

  };
}

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
