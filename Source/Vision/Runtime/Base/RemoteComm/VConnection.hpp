/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VConnection.hpp

#ifndef __VCONNECTION_INCLUDED__
#define __VCONNECTION_INCLUDED__

#include <Vision/Runtime/Base/RemoteComm/VSocket.hpp>

#include <Vision/Runtime/Base/Container/VRingBuffer.hpp>

#include <Vision/Runtime/Base/System/Threading/SyncPrimitive/VConditionVariable.hpp>

  class VMessage;

  /// \brief
  ///   This class describes a connection in the remote communication system
  class VConnection
  {
    public:

      /// \brief  Constructor. 
      ///
      /// \param  socket An already connected socket to use. The connection will take over ownership of the socket and leave the passed argument empty.
      /// \param [in] pIdentifier The identifier of the connection - this has to be a four letter code e.g "VRSD" or "VRES". Has to be unique per system using the VTarget functionality. 
      /// \param szRemoteHost The address of the remote host of the connection.
      ///
      VConnection(VSocket &socket, const char* pIdentifier, const char* szRemoteHost);

      /// \fn ~VConnection()
      ///
      /// \brief  Destructor.
      ///
      ~VConnection();

      /// \fn VBASE_IMPEXP bool SendAsync(VMessage* pMessage)
      ///
      /// \brief  Sends a message asynchronously. The message will be enqueued in a queue and if the connection becomes writable it will be sent. 
      ///
      /// \param [in] pMessage  The message object to send. 
      ///
      /// \return true if it succeeds, false if it fails. 
      ///
      VBASE_IMPEXP bool SendAsync(VMessage* pMessage);

      /// \brief Flushes the queue of asynchronuously sent messages.
      VBASE_IMPEXP void FlushSendAsync();

      /// \fn VBASE_IMPEXP bool Send(VMessage* pMessage)
      ///
      /// \brief  Sends the given message synchronous on the connection. 
      ///
      /// \param [in] pMessage  The message to send. 
      ///
      /// \return true if it succeeds, false if it fails. 
      ///
      VBASE_IMPEXP bool Send(VMessage* pMessage);

      /// \brief  Receives a message from the connection. This call can block if there is no message in the queue!
      VBASE_IMPEXP VMessage* Recv();

      /// \brief  Gets the internal socket of the connection. 
      ///
      /// \return The socket. 
      ///
      VBASE_IMPEXP inline VSocket& GetSocket()
      {
        return m_socket;
      }

      /// \fn VBASE_IMPEXP inline const char* GetIdentifier()
      ///
      /// \brief  Gets the identifier of the connection (e.g. "VRSD" or "VRES"). 
      ///
      /// \return null if it fails, else the identifier. 
      ///
      VBASE_IMPEXP inline const char* GetIdentifier()
      {
        return m_pIdentifier;
      }

      /// \fn VBASE_IMPEXP inline bool IsFor(const char* pIdentifier)
      ///
      /// \brief  Query if the given 'pIdentifier' is the same as this connections identifier. 
      ///
      /// \param [in] pIdentifier The identifier. 
      ///
      /// \return true if the given identifier matches the identifier of the connection, false if not. 
      ///
      VBASE_IMPEXP inline bool IsFor(const char* pIdentifier)
      {
        return (pIdentifier && !strcmp(pIdentifier, m_pIdentifier));
      }

      /// \fn VBASE_IMPEXP bool IsValid()
      ///
      /// \brief  Query if this connection is a valid (open) connection.
      ///
      /// \return true if valid, false if not. 
      ///
      VBASE_IMPEXP bool IsValid();

      /// \fn VBASE_IMPEXP VMessage* GetNextASyncReceivedMessage()
      ///
      /// \brief  Gets the next received message from the asynchronous queue. 
      ///
      /// \return null if there is no message in the queue, else the next asynchronously received message. 
      ///
      VBASE_IMPEXP VMessage* GetNextASyncReceivedMessage();

      /// \fn VBASE_IMPEXP VMessage* GetNextASyncMessageToSend()
      ///
      /// \brief  Gets the next asynchronously message to send. 
      ///
      /// \return null if there is no message to send, else the next asynchronously message to send. 
      ///
      VBASE_IMPEXP VMessage* GetNextASyncMessageToSend();

      /// \brief Returns the address of the remote host of the connection.
      VBASE_IMPEXP const char* GetRemoteHost()
      {
        return m_sRemoteHost;
      }

      /// \brief Temporarily disables asynchronous message parsing.
      ///
      /// \param bSuspended Flag if async message receiving should be supended or not.
      ///
      /// This function can only be valid if the message receive buffer is currently empty.
      /// Any messages received after changing this flag but before the TargetThread has read it may result
      /// in incorrectly parsed messages. Make sure that the message protocol used takes this into consideration.
      VBASE_IMPEXP void SuspendAsyncMessageReceive(bool bSuspended);

    private:
      friend class VTarget;

      /// \fn void SetIdentifier(const char* pIdentifier)
      ///
      /// \brief  Sets the identifier of the connection. 
      ///
      /// \param [in,out] pIdentifier The identifier. 
      ///
      void SetIdentifier(const char* pIdentifier);

      /// \fn void SetInvalid()
      ///
      /// \brief  Sets the connection to an invalid state. 
      ///
      void SetInvalid();

      /// \fn void AddASyncReceivedMessage(VMessage* pMessage)
      ///
      /// \brief  Adds a asynchronously received message. 
      ///
      /// \param [in] pMessage  The message to add to the queue. 
      ///
      void AddASyncReceivedMessage(VMessage* pMessage);

      /// \fn void Close()
      ///
      /// \brief  Closes the connection.
      ///
      void Close();

      /// \brief Reads all available messages from the ring buffer and appends them to the queue.
      void ParseMessages();

      //! Stores the socket of this connection. 
      VSocket m_socket;

      //! Stores the list of enqueued messages to send. 
      VArray<VMessage*> m_aEnqueuedMessages;
      
      //! Stores the list of received messages. 
      VArray<VMessage*> m_aReceivedMessages;

      //! The condition guarding the receive queue. 
      VConditionVariable m_receiveQueueNonEmpty;
      
      //! The mutex guarding the send queue. 
      VMutex m_SendQueueMutex;

      //! The mutex guarding the send function. 
      VMutex m_SendMutex;
      
      //! Stores the identifier of the connection. 
      char* m_pIdentifier;

      //! Stores the address of the remote host. 
      VString m_sRemoteHost;

      enum ParseState
      {
        READ_MESSAGE_TYPE_AND_SIZE,
        READ_MESSAGE_CONTENT,
      };

      ParseState m_eCurrentParseState;
      VMessage* m_pCurrentMessage;

      VRingBuffer<char, 16 * 1024> m_ringBuffer;
      VEvent m_sendQueueEmptyEvent;

#if defined(HK_DEBUG_SLOW)
      // To be able to reliably check that the async receive is only suspended while there is no unprocessed message data left,
      // we need to synchronize access to m_bReceiveSuspended. This is only needed to check for race conditions, but not required
      // when using the receive suspend correctly.
      VMutex m_receiveSuspendMutex;
#endif

      bool m_bReceiveSuspended;
  };

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
