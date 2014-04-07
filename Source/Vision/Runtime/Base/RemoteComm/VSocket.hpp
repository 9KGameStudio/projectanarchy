/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VSocket.hpp

#ifndef VSOCKET_HPP_INCLUDED
#define VSOCKET_HPP_INCLUDED

#include <Vision/Runtime/Base/Math/hkvMathConfig.h>

#if !defined(_VISION_ANDROID) && !defined(_VISION_TIZEN)
struct fd_set;
#endif

#if defined(WIN32)
#include <winsock2.h>
#endif

/// \brief A class managing a network socket handling.
///
/// This class has RAII-semantics, ie. it owns the created socket handle and takes over the responsibility
/// for closing and releasing the associated systems resources.
class VSocket
{
public:

  /// \brief The platform specific socket handle type.
#if defined(WIN32)
  typedef SOCKET socket_t;
#else
  typedef int socket_t;
#endif

  /// \brief The type of socket to create.
  enum SocketType
  {
    TCP,    ///< A TCP socket.
    UDP     ///< A UDP socket.
  };

  /// \brief Type tag to be used in the constructor to transfer the handle ownership.
  struct TransferOwnership {};

  /// \brief Constructs an empty socket that needs to be initialized with CreateHandle.
  VBASE_IMPEXP VSocket();

  /// \brief Constructs a socket from the platform handle.
  VBASE_IMPEXP VSocket(socket_t handle);

  /// \brief Constructs a socket by moving the handle from another socket.
  ///
  /// \example
  ///   \code
  ///     VSocket a;
  ///     a.CreateHandle(VSocket::TCP);
  ///     //...
  ///     VSocket b(a, VSocket::TransferOwnership());
  ///     // a.IsValid() is now false
  ///   \endcode
  ///
  /// \param socket Socket on which this instance should be constructed from.
  /// \param typeTag Helper used by VConnection constructor.
  VBASE_IMPEXP VSocket(VSocket& socket, TransferOwnership typeTag);

  /// \brief Destructor. Closes the socket.
  VBASE_IMPEXP ~VSocket();

  /// \brief Closes the socket.
  VBASE_IMPEXP void Close();

  /// \brief Returns true if the socket contains a valid handle.
  VBASE_IMPEXP bool IsValid() const;

  /// \brief Returns the platform-specific native socket handle.
  VBASE_IMPEXP socket_t GetHandle() const;

  /// \brief Creates a handle for the given type.
  VBASE_IMPEXP hkvResult CreateHandle(SocketType type);

  /// \brief Accepts an incoming TCP connection from the given listening socket.
  VBASE_IMPEXP hkvResult Accept(VSocket& listeningSocket, VString* pOutHost = NULL, unsigned short* pOutPort = NULL);

  /// \brief Establishes a TCP connection to the given IP.
  VBASE_IMPEXP hkvResult Connect(const char* szIpAddress, unsigned short usPort);

  /// \brief Allows sending of broadcasts (default: false). Only available for UDP sockets.
  VBASE_IMPEXP hkvResult SetBroadcastEnabled(bool bEnabled);

  /// \brief Allows immediately sending packets without queueing them according to Nagle's algorithm (default: false). Only available for TCP sockets.
  VBASE_IMPEXP hkvResult SetNoDelayEnabled(bool bEnabled);

  /// \brief Allows using the same address for multiple sockets (default: false).
  VBASE_IMPEXP hkvResult SetReuseAddressEnabled(bool bEnabled);

  /// \brief Binds the socket to the given port and listens for incoming TCP connections.
  VBASE_IMPEXP hkvResult Listen(unsigned short usPort);

  /// \brief Broadcasts a data packet. Only available for UDP sockets.
  VBASE_IMPEXP hkvResult Broadcast(unsigned short usPort, const void* pData, unsigned int uiDataSize);

  /// \brief Sends data.
  VBASE_IMPEXP hkvResult Send(const void* pData, unsigned int uiDataSize);

  /// \brief Receives data until the buffer is filled.
  VBASE_IMPEXP hkvResult ReceiveFill(void* pBuffer, unsigned int uiBufferSize);

  /// \brief Receives data and returns the amount that was received.
  VBASE_IMPEXP hkvResult Receive(void* pBuffer, unsigned int uiMaxBufferSize, unsigned int& uiReceivedOut);

  /// \brief Monitors the socket FD sets until a time out occurs or one of the sockets becomes ready for I/O operations.
  ///   Returns the number of sockets that changed state.
  VBASE_IMPEXP static int Select(socket_t maxHandle, fd_set* pReadSet, fd_set* pWriteSet, fd_set* pExceptSet, VTimeSpan timeOut);

  /// \brief Enables or disables logging of socket errors for this thread. Default is true.
  VBASE_IMPEXP static void SetAllowErrorLoggingForThread(bool bAllowErrorLogging);

private:
  // Non-copyable
  VSocket(const VSocket&);
  VSocket& operator=(const VSocket&);

  socket_t m_handle;

  static VISION_THREADLOCAL_DECL(bool, s_bAllowErrorLogging);
};


#endif

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
