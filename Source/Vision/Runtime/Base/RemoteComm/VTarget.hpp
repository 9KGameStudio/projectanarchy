/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VTarget.hpp

#ifndef __VTARGET_INCLUDED__
#define __VTARGET_INCLUDED__

  #include <Vision/Runtime/Base/System/Threading/Thread/VBackgroundThread.hpp>
  #include <Vision/Runtime/Base/RemoteComm/VSocket.hpp>

  #define VTARGET_GET_CONN(Ident) VTargetThread::GetTargetThread() ? VTargetThread::GetTargetThread()->GetTarget() ? VTargetThread::GetTargetThread()->GetTarget()->GetConnectionByIdentifier(Ident) : NULL : NULL

  class VConnection;
  class VMessage;

  #if !defined(_VISION_ANDROID) && !defined(_VISION_TIZEN)
  struct fd_set;
  #endif

  #define VTARGET_APPLICATION_PORT 4223
  #define VTARGET_EDITOR_PORT 4224

  /// \brief
  ///   A callback item used in the script event callback system
  class VTargetConnectionCallbackItem : public IVisCallbackDataObject_cl
  {
    public:

      /// \fn VTargetConnectionCallbackItem(VCallback *Sender, VConnection* Connection)
      ///
      /// \brief  Constructor. 
      ///
      /// \param [in] Sender  The callback sender (VTarget::OnConnection or VTarget::OnDisconnection)
      /// \param [in] Connection The connection instance which was established or destroyed (can be identified via it's identifier)
      ///
      VTargetConnectionCallbackItem(VCallback *Sender, VConnection* Connection)
        : IVisCallbackDataObject_cl(Sender), pConnection(Connection)
      {}

      //! The connection
      VConnection* pConnection;
  };

  /// \brief
  ///   The managing class for the remote communication handling.
  ///   The system handles multiple connections via select() based networking simultaneosly.
  ///   An usage sample can be found in the VRSDClient in the VisionEnginePlugin which uses the VTarget system and
  ///   the related classes VConnection, VMessage to implement script debugging over TCP/IP.
  ///   On the host side the functionality is encapsulated in a .NET assembly called RCHostManaged.
  class VTarget
  {
    public:

      /// \fn VTarget(unsigned short uiPort = VTARGET_APPLICATION_PORT)
      ///
      /// \brief  Default constructor. 
      ///
      /// \param uiPort The port to initialize the target thread on. (Default is 4223, Editor will start on 4224).
      ///
      VTarget(unsigned short uiPort = VTARGET_APPLICATION_PORT);

      /// \fn ~VTarget()
      ///
      /// \brief  Destructor
      ///
      ~VTarget();

      /// \fn void MainLoop()
      ///
      /// \brief  The main loop of the remote communication system which handles the connections.
      ///
      void MainLoop();

      /// \fn void Exit()
      ///
      /// \brief  Signals that the main loop should be exited.
      ///
      void Exit();

      /// \fn inline bool IsBusy()
      ///
      /// \brief  Queries whether the target system is busy (handling requests and/or waiting for incoming connections).
      ///
      /// \return true if busy, false if not. 
      ///
      inline bool IsBusy()
      {
        return m_bBusy;
      }

      /// \brief Rather than waiting for a connection, establish a VTarget connection by explicitly telling a host that we would like it to connect to us.
      VBASE_IMPEXP VConnection* SolicitConnection(const char* szIpAddress, unsigned short usPort, const char* szDesiredIdentifier);

      /// \fn VBASE_IMPEXP VConnection* GetConnectionByIdentifier(const char* pIdentifier)
      ///
      /// \brief  Gets a connection by it's four letter code identifier (e.g. "VRES"). 
      ///
      /// \param [in] pIdentifier The identifier of the searched connection.
      ///
      /// \return null if no connection was found, else the connection by identifier. 
      ///
      VBASE_IMPEXP VConnection* GetConnectionByIdentifier(const char* pIdentifier);

      /// \brief  Gets a connection by the associated socket. 
      ///
      /// \param  socket The socket. 
      ///
      /// \return null if not found, otherwise the connection for the given socket. 
      ///
      VBASE_IMPEXP VConnection* GetGonnectionBySocket(const VSocket& socket);

      /// \fn VBASE_IMPEXP void Disconnect(VConnection* pConnection)
      ///
      /// \brief  Disconnects the given pConnection. 
      ///
      /// \param [in] pConnection The VConnection* to disconnect. 
      ///
      VBASE_IMPEXP void Disconnect(VConnection* pConnection);     

      //! This callback can be used to be notified when there is a new connection.
      VBASE_IMPEXP static VCallback OnConnection;

      //! This callback can be used to be notified when a connection is disconnected.
      VBASE_IMPEXP static VCallback OnDisconnection;

      /// \fn VBASE_IMPEXP static void ForceCleanup()
      ///
      /// \brief  Cleans up the connection in case the thread is terminated externally.
      /// NOTE:  do not call directly.
      ///
      VBASE_IMPEXP void ForceCleanup();

    private:
      friend class VTargetThread;

      /// \brief Performs a handshake on an already connected socket and creates a new VConnection object on success.
      VConnection* EstablishConnection(VSocket& socket, const char* szRemoteHost);

      /// \brief  Sets up the fd sets. 
      ///
      /// \param [in,out] ReadFDs If non-null, the read f ds. 
      /// \param [in,out] WriteFDs  If non-null, the write f ds. 
      /// \param [in,out] ExceptFDs If non-null, the except f ds. 
      /// \param  ListeningSocket The listening socket. 
      ///
      void SetupFDSets(fd_set* ReadFDs, fd_set* WriteFDs, fd_set* ExceptFDs, const VSocket& ListeningSocket);

      //! Stores the list of currently active connections.
      VPListT<VConnection> m_Connections;

      //! The listening socket. 
      VSocket m_listeningSocket;

      //! Internal variable to keep track of the max socket fd in use (necessary for select). 
      VSocket::socket_t m_iMaxSocketFD;

      /// \fn bool InitNetwork()
      ///
      /// \brief  Initializes the network, this does the platform dependent stuff. 
      ///
      /// \return true if it succeeds, false if it fails. 
      ///
      static bool InitNetwork();

      /// \fn bool DeinitNetwork()
      ///
      /// \brief  Deinitializes the network. 
      ///
      /// \return true if it succeeds, false if it fails. 
      ///
      static bool DeinitNetwork();

      /// \fn bool CreateListenSocket()
      ///
      /// \brief  Creates the listen socket. 
      ///
      /// \return true if it succeeds, false if it fails. 
      ///
      bool CreateListenSocket();

      /// \brief Close the listen socket.
      void CloseListenSocket();

      //! Stores the port the target was initialized for.
      unsigned short m_uiPort;

      //! Keeps track if the remote communication system is running. 
      bool m_bRunning;

      //! Keeps track if the remote communication system is busy. 
      volatile bool m_bBusy;

      //! Prevents threads from closing / deleting connections simultaneously.
      VMutex m_ConnectionMutex;

      VEvent m_anyConnectionEvent;

      VDateTime m_lastListenAttempt;

      #if defined(_VISION_PSP2)

      static char* s_pNetworkMemory;

      static int s_iStateCallbackId;

      #endif
  };

  /// \brief
  ///   This class inherits from VBackgroundThread to provide the remote communication facilities.
  class VTargetThread : public VBackgroundThread
  {
    public:

      /// \fn VTargetThread(unsigned short uiPort = VTARGET_APPLICATION_PORT)
      ///
      /// \brief  Default constructor.
      ///
      /// \param uiPort The port to initialize the target thread on. (Default is 4223, Editor will start on 4224).
      ///
      VTargetThread(unsigned short uiPort = VTARGET_APPLICATION_PORT);

      /// \fn ~VTargetThread()
      ///
      /// \brief  Destructor. 
      ///
      ~VTargetThread();

      /// \fn virtual void Run()
      ///
      /// \brief  The main loop of the target thread (invokes MainLoop() on VTarget)
      ///
      virtual void Run();

      /// \fn VBASE_IMPEXP inline VTarget* GetTarget()
      ///
      /// \brief  Gets the associated target. 
      ///
      /// \return The associated target with the target thread.
      ///
      VBASE_IMPEXP inline VTarget* GetTarget()
      {
        return m_pTarget;
      }

      /// \fn VBASE_IMPEXP inline void Exit()
      ///
      /// \brief  Signals that the target should exit.
      ///
      VBASE_IMPEXP inline void Exit()
      {
        if(m_pTarget)
          m_pTarget->Exit();
      }

      /// \fn VBASE_IMPEXP static inline VTargetThread* GetTargetThread()
      ///
      /// \brief  Gets the target thread. 
      ///
      /// \return the target thread. 
      ///
      VBASE_IMPEXP static inline VTargetThread* GetTargetThread()
      {
        return s_pTargetThread;
      }

      /// \fn VBASE_IMPEXP static void Init(unsigned short uiPort = 4223)
      ///
      /// \brief  Initializes the target system. 
      ///
      /// \param uiPort The port to initialize the target thread on. (Default is 4223, Editor will start on 4224).
      ///
      VBASE_IMPEXP static void Init(unsigned short uiPort = 4223);

      /// \fn VBASE_IMPEXP static void DeInit()
      ///
      /// \brief  De-initializes. 
      ///
      VBASE_IMPEXP static void DeInit();

      /// \fn VBASE_IMPEXP static bool WaitForConnection(const char* pConnectionType, int TimeOut = 10)
      ///
      /// \brief  Waits for a connection of the specified type. 
      ///
      /// \param [in] pConnectionType The connection type to wait for. 
      /// \param  TimeOut The time out in seconds. 
      ///
      /// \return true if there was a connection within TimeOut (or already existed), false if no connection of the specified type was made. 
      ///
      VBASE_IMPEXP static bool WaitForConnection(const char* pConnectionType, int TimeOut = 10);

    private:

      //! The target associated with the target thread. 
      VTarget* m_pTarget;

      //! The port the system was initialized for
      unsigned short m_uiPort;

      //! The global target thread. 
      static VTargetThread* s_pTargetThread;

      VEvent m_startedEvent;
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
