/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file Base\RemoteComm\VMessage.hpp

#ifndef __VMESSAGE_INCLUDED__
#define __VMESSAGE_INCLUDED__

class VSocket;

  /// \brief
  ///   This class describes a message in the remote communication system.
  class VMessage
  {
    public:
      /// \fn VBASE_IMPEXP VMessage()
      ///
      /// \brief Default constructor. 
      ///
      VBASE_IMPEXP VMessage();

      /// \fn VBASE_IMPEXP VMessage(int Type)
      ///
      /// \brief  Constructor taking a message type in the form 'XXXX' (4 bytes)
      ///
      /// \param  Type  The type of the message. 
      ///
      VBASE_IMPEXP VMessage(int Type);

      /// \fn VBASE_IMPEXP VMessage(int Type, int PreallocSize)
      ///
      /// \brief  Constructor taking the type of the message and the data buffer size.
      ///
      /// \param  Type  The type of the message. 
      /// \param  PreallocSize  Number of bytes to preallocate in the data buffer.
      ///
      VBASE_IMPEXP VMessage(int Type, int PreallocSize);

      /// \brief Copy constructor.
      VBASE_IMPEXP VMessage(const VMessage& other);

      /// \fn VBASE_IMPEXP ~VMessage()
      ///
      /// \brief  The destructor. 
      ///
      VBASE_IMPEXP ~VMessage();

      /// \brief Assignment operator.
      VBASE_IMPEXP VMessage& operator=(const VMessage& other);

      /// \fn VBASE_IMPEXP unsigned int GetContentSize() const
      ///
      /// \brief  Gets the content (payload) size of the message. 
      ///
      /// \return The content size. 
      ///
      VBASE_IMPEXP unsigned int GetContentSize() const;

      /// \brief  Gets the message type. 
      ///
      /// \return The message type. 
      ///
      VBASE_IMPEXP int GetMessageType() const;

      /// \brief  Writes an integer value into the data buffer.
      ///
      /// \param  iValue  The int to write. 
      ///
      VBASE_IMPEXP void WriteInt(int iValue);

      ///
      /// \brief  Writes a 64bit integer value into the data buffer.
      ///
      /// \param  iValue  The int to write. 
      ///
      VBASE_IMPEXP void WriteInt64(__int64 iValue);

      /// \brief  Writes a string into the message. 
      ///
      /// \param [in] pString If non-null, the string to write into the data buffer. 
      ///
      VBASE_IMPEXP void WriteString(const char* pString);

      /// \brief  Writes a binary blob into the message. 
      ///
      /// \param [in] pBytes  If non-null, the bytes to write. 
      /// \param  uiCount  Number of bytes to write. 
      ///
      VBASE_IMPEXP void WriteBytes(const char* pBytes, unsigned int uiCount);

      /// \fn VBASE_IMPEXP bool ReadInt(int& iValue)
      ///
      /// \brief  Reads an integer value from the data buffer of the message. 
      ///
      /// \param [out] iValue  the value. 
      ///
      /// \return true if it succeeds, false if it fails. 
      ///
      VBASE_IMPEXP bool ReadInt(int& iValue);

      /// \brief  Reads a 64bit integer value from the data buffer of the message. 
      ///
      /// \param [out] iValue  the value. 
      ///
      /// \return true if it succeeds, false if it fails. 
      ///
      VBASE_IMPEXP bool ReadInt64(__int64& iValue);

      /// \fn VBASE_IMPEXP bool ReadString(char** ppString)
      ///
      /// \brief  Reads a string from the data buffer of the message. The string lifetime is bound to the lifetime of the VMessage object.
      ///
      /// \param [out] ppString  If non-null, the string. 
      ///
      /// \return true if it succeeds, false if it fails. 
      ///
      VBASE_IMPEXP bool ReadString(char** ppString);

      /// \brief Gets a raw memory block from the messages buffer and advances the read offset.
      ///
      /// The lifetime of the pointer is bound to the lifetime of the VMessage object.
      ///
      /// \param uiLen the length of the raw memory block to get
      VBASE_IMPEXP const void* GetRawBlock(unsigned int uiLen);

      /// \fn VBASE_IMPEXP void ResetReadPointer()
      ///
      /// \brief  Resets the read pointer of the data buffer. This function can be used to rewind the read pointer to read the message again from the beginning.
      ///
      VBASE_IMPEXP void ResetReadPointer();

      /// \fn VBASE_IMPEXP bool WasSent()
      ///
      /// \brief  Returns if the message was already sent or not. 
      ///
      /// \return true if it was, false if not. 
      ///
      VBASE_IMPEXP bool WasSent() { return m_bSent; }

      /// \fn VBASE_IMPEXP bool ShouldAutoDelete()
      ///
      /// \brief  Queries if this message should be deleted automatically after being sent. (Default = true)
      ///
      /// \return true if it should, false if not.
      ///
      VBASE_IMPEXP bool ShouldAutoDelete() { return m_bAutoDelete; }

      /// \fn VBASE_IMPEXP void SetAutoDelete(bool bAutoDelete)
      ///
      /// \brief  Sets the automatic delete flag of the message
      ///
      /// \param  bAutoDelete true to automatic delete of the message after being sent asynchronously. 
      ///
      VBASE_IMPEXP void SetAutoDelete(bool bAutoDelete) {m_bAutoDelete = bAutoDelete; }

      /// \brief Receives a message from the given socket.
      VBASE_IMPEXP hkvResult ReceiveFrom(VSocket& socket);

      /// \brief Sends a message on the given socket.
      VBASE_IMPEXP hkvResult SendTo(VSocket& socket);

      /// \brief Broadcasts a message on the given socket.
      VBASE_IMPEXP hkvResult Broadcast(VSocket& socket, unsigned short usPort);

    private:  
      friend class VConnection;

      /// \brief  Sets the content (payload) size of the message. 
      VBASE_IMPEXP void SetContentSize(unsigned int uiContentSize);

      /// \brief  Sets the message type. 
      VBASE_IMPEXP void SetMessageType(int iMessageType);
   
      //! Stores the header and content of the message.
      DynArray_cl<char> m_headerAndContent;
      
      //! Stores the current read offset into the data buffer. 
      int m_iReadOffset;

      //! Stores if this message was sent already
      bool m_bSent;

      //! Stores if this message should be deleted after being sent (For asynchronous messages). Default = true
      bool m_bAutoDelete;
  };

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
