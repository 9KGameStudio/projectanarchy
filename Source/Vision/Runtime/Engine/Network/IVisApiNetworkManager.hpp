/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file IVisApiNetworkManager.hpp

#ifndef VISAPINETWORKMANAGER_HPP_INCLUDED
#define VISAPINETWORKMANAGER_HPP_INCLUDED

#include <Vision/Runtime/Engine/Network/VisApiDataHistory.hpp>
#include <Vision/Runtime/Base/System/IO/Stream/VMemoryStream.hpp>

typedef VMemBlockWrapperStream VNetworkStreamReader;
#define VNetworkStreamWriter VMemoryTempBufferOutStream

// forward:
struct VNetworkViewContext;
class IVNetworkManager;
class IVNetworkViewComponent;

typedef __int64 VSystemGUID; ///< Unique system identifier used in network related functions.

/// \brief
///   Packet IDs.
///
/// Everything before VPackageID_FirstUserID is reserved by Vision
enum VPacketID_e
{
  VPackageID_Callback = 196,
  VPackageID_Messages = 197,
  VPackageID_DataBlockMessages = 198,
  VPackageID_Sync = 199,
  VPackageID_FirstUserID = 200, // rather use a large hardcoded value here to not break compatibility
};

#if defined(__SNC__)
#pragma diag_push
#pragma diag_suppress=178
#endif

static const int iNUMBER_OF_ORDERING_CHANNELS = 32; ///< The number of ordering channels usable for sending messages on

#if defined(__SNC__)
#pragma diag_pop
#endif

/// \brief
///   Defines the contents of a VIS_MSG_DATABLOCK
///
/// \sa VRakNetManager::SendDataBlockMessage
struct VDataBlockMessageInfo
{
public:
  VSystemGUID m_iSenderID;              ///< Unique system ID of the sender of the message
  void* m_pDataPtr;                     ///< Pointer to the start of the data block
  unsigned int m_iDataLength;           ///< Length of the data block
  unsigned int m_iDataBlockMessageID;   ///< ID to differentiate DataBlockMessages from each other
};


/// \brief
///   This determines the reliability of the following message.
enum VMessageReliability_e
{
  VMR_Unreliable=0,           ///< Same as regular UDP, except that it will also discard duplicate datagrams.
  VMR_Unreliable_Sequenced=1, ///< Regular UDP with a sequence counter. Out of order messages will be discarded.
  VMR_Reliable=2,             ///< The message is sent reliably, but not necessarily in any order.
  VMR_Reliable_Ordered=3,	    ///< This message is reliable and will arrive in the order you sent it. Messages will be delayed while waiting for out of order messages.
  VMR_Reliable_Sequenced=4,	  ///< This message is reliable and will arrive in the sequence you sent it. Out or order messages will be dropped.
};


/// \brief
///   This determines the priority of the following message.
///
/// Messages at VPP_HighPriority and lower are buffered to be sent in groups at 10 millisecond intervals to reduce UDP overhead and better measure congestion control.
enum VMessagePriority_e
{
  VMP_ImmediatePriority = 0, ///<  Messages with this setting are sent immediately, and are generally not buffered or aggregated into a single datagram.
  VMP_HighPriority = 1,      ///<  For every 2 VPP_ImmediatePriority messages, 1 VPP_HighPriority will be sent.
  VMP_MediumPriority = 2,    ///<  For every 2 VPP_HighPriority messages, 1 VPP_MediumPriority will be sent.
  VMP_LowPriority = 3,       ///<  For every 2 VPP_MediumPriority messages, 1 VPP_LowPriority will be sent.
};


/// \brief
///   The settings used for the next sent message.
///
/// - Reliability: see VMessageReliability_e for the different levels of reliability.
/// - Priority: see VMessagePriority_e for the different levels of priority.
/// - Ordering channel: all messages sent ordered or sequenced on a channel will retain their order/sequence on the receiver's side.
///   This value is ignored if ordered or sequenced is not stated. There are 32 ordering channels in total (0 to 31).
///   The default ordering channel is 0, the general idea is that a higher ordering channel takes precedence,
///   for example if one IVNetworkSynchronizationGroup wants to send on channel 2 and another on 3, that the update will be sent on channel 3.
/// .
///
/// \sa VMessageReliability_e, VMessagePriority_e, IVNetworkSynchronizationGroup::QuerySynchronize
struct VMessageSettings
{
public:
  /// \brief
  ///   Constructor
  ///
  /// Initializes with the weakest settings available: unreliable, low priority, ordering channel zero.
  inline VMessageSettings ()
    : m_iReliabilityFlags(0), m_iPriority(VMP_LowPriority), m_iOrderingChannel(0)
  {
  }

  /// \brief
  ///   Constructor
  inline VMessageSettings (VMessageReliability_e messageReliability, VMessagePriority_e messagePriority, BYTE iOrderingChannel)
    : m_iPriority(messagePriority), m_iOrderingChannel(iOrderingChannel)
  {
    SetReliability (messageReliability);
  }

  /// \brief
  ///   Sets the reliability of the message.
  ///
  /// \param messageReliability
  ///   Reliability of the corresponding message, \sa VMessageReliability_e
  ///
  /// \sa VMessageReliability_e
  VISION_APIFUNC void SetReliability (VMessageReliability_e messageReliability);

  /// \brief
  ///   Sets the priority of the message.
  ///
  /// \param messagePriority
  ///   Priority of the corresponding message, \sa VMessagePriority_e
  ///
  /// \sa VMessagePriority_e
  inline void SetPriority (VMessagePriority_e messagePriority)
  {
    VASSERT_MSG (messagePriority >= 0 && messagePriority <= VMP_LowPriority, "VMessageSettings::SetPriority: parameter messagePriority out of range!");
    m_iPriority = (BYTE)messagePriority;
  }

  /// \brief
  ///   Sets the ordering channel of the message.
  inline void SetOrderingChannel (BYTE iOrderingChannel)
  {
    VASSERT_MSG (iOrderingChannel < iNUMBER_OF_ORDERING_CHANNELS, "VMessageSettings::SetOrderingChannel: parameter iOrderingChannel out of range!");
    m_iOrderingChannel = iOrderingChannel;
  }

  /// \brief
  ///   Returns the VMessageReliability_e of the message.
  VISION_APIFUNC VMessageReliability_e GetReliability();

  /// \brief
  ///   Returns the VMessagePriority_e of the message.
  inline VMessagePriority_e GetPriority()
  {
    return (VMessagePriority_e)m_iPriority;
  }

  /// \brief
  ///   Returns the ordering channel of the message.
  inline BYTE GetOrderingChannel()
  {
    return m_iOrderingChannel;
  }

  /// \brief
  ///   Generalizes this instance by adding the given settings.
  VISION_APIFUNC void Generalize (const VMessageSettings& messageSettings);

private:
  /// \brief
  ///   These flags determine the reliability of the next message.
  ///
  /// These are used instead of VMessageReliability_e, because the flags can easily be accumulated over all groups by a bitwise OR.
  enum VMessageReliabilityFlags
  {
    VMRF_RELIABLE = V_BIT(0),
    VMRF_SEQUENCED = V_BIT(1),
    VMRF_ORDERED = V_BIT(2),
  };

  BYTE m_iReliabilityFlags; ///< VMessageReliabilityFlags
  BYTE m_iPriority;         ///< VMessagePriority_e
  BYTE m_iOrderingChannel;
};


/// \brief
///   Each custom IVNetworkSynchronizationGroup group may allocate data per instance if needed, by deriving a class from this interface.
///
/// Each groups CreatePerInstanceData function will be called by VNetworkSynchronizationGroupInstanceInfo_t::Set and the created instance will be
/// destroyed by VNetworkSynchronizationGroupInstanceInfo_t destructor.
class IVNetworkSynchronizationGroupInstanceData
{
public:
  /// \brief
  ///   Constructor
  inline IVNetworkSynchronizationGroupInstanceData()
  {
  }

  /// \brief
  ///   Destructor
  VISION_APIFUNC virtual ~IVNetworkSynchronizationGroupInstanceData()
  {
  }
};


/// \brief
///   A synchronization group is responsible for network serialization of a group of data. For instance "Transform" (VNetworkTransformationGroupI). An object must provide a list of groups
///
/// \sa VisTypedEngineObject_cl::GetSynchronizationGroupList
class IVNetworkSynchronizationGroup
{
public:

  /// \brief
  ///   Pre-defined bits for the GetUsageTypeFlags function
  ///
  /// The purpose of this mask is to define exclusion criteria for similar groups that cannot be attached to an object simultaneously because
  /// their behavior is mutually exclusive. For instance, a "Transformation" group and "Interpolated Transformation" group would collide as
  /// both update the position.
  enum VGroupTypeFlags_e
  {
    VGroupTypeBit_None = 0,
    VGroupTypeBit_Transformation = V_BIT(0),  ///< The group controls the transformation
    VGroupTypeBit_Visibility = V_BIT(1),  ///< The group controls the objects visibility via SetVisibleBitmask
    VGroupTypeBit_Light = V_BIT(2),  ///< The group controls the parameters of a light
    VGroupTypeBit_ParticleEffect = V_BIT(3),  ///< The group controls the paused and halted states of a particle effect
    VGroupTypeBit_VolumetricConeCommon = V_BIT(4),  ///< The group controls the angle, color and density of a volumetric cone
    VGroupTypeBit_VolumetricConeUncommon = V_BIT(5),  ///< The group controls the length, start size and start distance of a volumetric cone
    VGroupTypeBit_TransitionStateMachine = V_BIT(6),  ///< The group controls the current state in a transition state machine
    VGroupTypeBit_FirstCustom = V_BIT(16),    ///< Indicates the first user group
  };

  /// \brief
  ///   Constructor
  inline IVNetworkSynchronizationGroup()
  {
  }

  /// \brief
  ///   Destructor
  VISION_APIFUNC virtual ~IVNetworkSynchronizationGroup()
  {
  }

  /// \brief
  ///   Groups must provide a unique name to identify themselves
  VISION_APIFUNC virtual const char *GetName() const = 0;

  /// \brief
  ///   Returns a bitmask here (bits defined in VGroupTypeFlags_e). Groups with overlapping bitmasks cannot be used by an object at the same time. vForge filters them automatically.
  VISION_APIFUNC virtual VGroupTypeFlags_e GetUsageTypeFlags() const {return VGroupTypeBit_None;}

  /// \brief
  ///   Returns the status whether this group should be enabled by default when an object references it.
  VISION_APIFUNC virtual bool GetDefaultEnabledState() const {return true;}

  /// \brief
  ///   Creates per instance custom data that is saved in VNetworkSynchronizationGroupInstanceInfo_t.
  ///
  /// \return
  ///   Pointer to the custom instance data
  ///
  /// This function is automatically called via VNetworkSynchronizationGroupInstanceInfo_t::Set. If per instance data is needed by a group,
  /// this function has to be overloaded.
  ///
  /// \sa IVNetworkSynchronizationGroupInstanceData
  VISION_APIFUNC virtual IVNetworkSynchronizationGroupInstanceData* CreatePerInstanceData() const {return NULL;}

  /// \brief
  ///   Queries if a synchronization should take place.
  ///
  /// The default implementation returns true with out_paketSettings set to VPR_Reliable_Ordered, VPP_HighPriority on OrderingChannel 0.
  ///
  /// This is accumulated over all groups of a network component. The result is the most general VMessageSettings. If all groups return false, no packet will be sent.
  ///
  /// \param context
  ///   The context of the network connection
  ///
  /// \param instanceInfo
  ///   The instance that should be synchronized
  ///
  /// \param[out] out_paketSettings
  ///   These flags determine the reliability of the next Synchronize call.
  ///
  /// \return
  ///   Returns true if the group wants to be sent on the next Synchronize call.
  ///
  /// \sa Synchronize
  VISION_APIFUNC virtual bool QuerySynchronize(const VNetworkViewContext& context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, VMessageSettings& out_paketSettings);

  /// \brief
  ///   Key function of this interface: Read/Write synchronization of the group data for the passed instance
  ///
  /// Implementations of this interface function must synchronize relevant data for the passed object instance. For example, a 'Transform' group
  /// writes or reads the position and orientation to or from archive. The read/write mechanisms are the same as for object serialization.
  ///
  /// \param context
  ///   The context of the network connection
  ///
  /// \param instanceInfo
  ///   The instance that should be synchronized
  ///
  /// \param ar
  ///   The archive that provides the data stream for synchronization
  ///
  /// \sa VArchive
  VISION_APIFUNC virtual void Synchronize(const VNetworkViewContext& context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, VArchive &ar)
  {
  }

  /// \brief
  ///   Executed each frame. Mostly used to interpolate received data for the current frame.
  ///
  /// \param context
  ///   The context of the network connection
  ///
  /// \param instanceInfo
  ///   The instance that should be synchronized
  ///
  /// \param fTimeDelta
  ///   Time in seconds passed since the last frame
  ///
  /// \sa VArchive
  VISION_APIFUNC virtual void TickFunction(const VNetworkViewContext &context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, float fTimeDelta);

  /// \brief
  ///   Static helper to generate a hash value
  static inline void Hash_AddFloat(__int64 &iHash, float fValue, int iIndex)
  {
    __int64 x = *((int *)&fValue);
    iHash ^= (x*iIndex*127);
  }  

  /// \brief
  ///   Static helper to generate a hash value
  static inline void Hash_AddFloats(__int64 &iHash, const float *pValues, int iValueCount, int iIndex)
  {
    for (int i=0;i<iValueCount;i++)
      Hash_AddFloat(iHash, pValues[i], iIndex+i);
  }

};


/// \brief
///   A pair of group and instance. An array of this struct is passed to VisTypedEngineObject_cl::GetSynchronizationGroupList so it can be filled out by that function
///
/// \sa VisTypedEngineObject_cl::GetSynchronizationGroupList
struct VNetworkSynchronizationGroupInstanceInfo_t
{
  /// \brief
  ///   Constructor
  VNetworkSynchronizationGroupInstanceInfo_t()
    : m_pInstance(NULL), m_pComponent (NULL), m_pGroup(NULL), m_pCustomData(NULL), m_iLastUpdateDataHash(0)
  {
  }

  /// \brief
  ///   Destructor
  ~VNetworkSynchronizationGroupInstanceInfo_t()
  {
    V_SAFE_DELETE(m_pCustomData);
  }

  /// \brief
  ///   Associate one instance with a group
  ///
  /// \param pInstance
  ///   Pointer to the instance that should be synchronized. Inside the GetSynchronizationGroupList function the 'this' pointer is passed here 
  ///
  /// \param pGroup
  ///   Pointer to the group instance. This group can be one global singleton shared across all instances
  inline void Set(VisTypedEngineObject_cl *pInstance, IVNetworkSynchronizationGroup *pGroup)
  {
    VASSERT(pInstance!=NULL && pGroup!=NULL);
    VASSERT(m_pCustomData==NULL);
    m_pInstance = pInstance;
    m_pGroup = pGroup;
    m_pComponent = NULL;
    m_pCustomData = pGroup->CreatePerInstanceData();
  }


  VisTypedEngineObject_cl *m_pInstance;     ///< Instance to be synchronized
  IVNetworkViewComponent *m_pComponent;     ///< Component to be synchronized, will be set by IVNetworkViewComponent::QuerySynchronizationGroups()
  IVNetworkSynchronizationGroup *m_pGroup;  ///< Pointer to the group instance
  IVNetworkSynchronizationGroupInstanceData *m_pCustomData;  ///< Allocated by the 'Set' function, released by the destructor.
  __int64 m_iLastUpdateDataHash;            ///< arbitrary use to store a hash to skip update (i.e. checked in IVNetworkSynchronizationGroup::QuerySynchronize)
};



/// \brief
///   Interface that can be used to control the synchronization group assignment to single instances. See function IVNetworkManager::SetCustomGroupProvider
class IVSynchronizationGroupProvider
{
public:
  IVSynchronizationGroupProvider()
  {
  }

  virtual ~IVSynchronizationGroupProvider()
  {
  }

  /// \brief
  ///   Key function of this interface: Filters the incoming group list that has been collected by the core engine.
  ///
  /// \param pForObject
  ///   The instance to provide with synchronization groups
  ///
  /// \param context
  ///   Analogue to VisTypedEngineObject_cl::GetSynchronizationGroupList
  ///
  /// \param pDestList
  ///   Destination list to fill with group information. Analogue to VisTypedEngineObject_cl::GetSynchronizationGroupList
  ///
  /// \param pSourceList
  ///   This list has been evaluated through pForObject->GetSynchronizationGroupList prior to this function call
  ///
  /// \param iSourceListLength
  ///   The number of entries in pSourceList, i.e. the return value of pForObject->GetSynchronizationGroupList
  ///
  /// \returns
  ///   The number of elements in pDestList
  virtual int GetSynchronizationGroupList(VisTypedEngineObject_cl *pForObject, const VNetworkViewContext &context, VNetworkSynchronizationGroupInstanceInfo_t *pDestList, const VNetworkSynchronizationGroupInstanceInfo_t *pSourceList, int iSourceListLength) = 0;
};


/// \brief
///   Interface for the network view component. Network view components of specific network implementations can be added in vForge.
/// 
/// A network view component will synchronize all groups that are returned by its owners VisTypedEngineObject_cl::GetSynchronizationGroupList function.
///
/// \sa VisTypedEngineObject_cl::GetSynchronizationGroupList, IVNetworkSynchronizationGroup
class IVNetworkViewComponent : public IVObjectComponent
{
public:
  /// \brief
  ///   Constructor that takes the owner network manager
  inline IVNetworkViewComponent(IVNetworkManager *pOwnerManager=NULL)
    : m_pManager(pOwnerManager), m_iGroupCount(0), m_iNetworkOwnerID(0), m_iObservedObjCount(0), m_ObservedObj(0)
  {
    m_iGroupsEnabledMask = 0;
    m_iGroupsTickFunctionMask = 0;
  }
  /// \brief
  ///   Destructor
  VISION_APIFUNC virtual ~IVNetworkViewComponent()
  {
  }

  VISION_APIFUNC virtual void DisposeObject() HKV_OVERRIDE;

#ifdef _VISION_WIN32
  ///
  /// @name Internal Editor functions
  /// @{
  ///

  /// \brief
  ///   Internal function used by the editor
  void ParseGroupBitmaskString(const char *szString);
  /// \brief
  ///   Internal function used by the editor
  void BuildGroupBitmaskString();

  /// \brief
  ///   Internal function used by the editor
  VISION_APIFUNC virtual void MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB) HKV_OVERRIDE;
  /// \brief
  ///   Internal function used by the editor
  VISION_APIFUNC virtual void OnVariableValueChanged(VisVariable_cl *pVar, const char * value) HKV_OVERRIDE;
  /// \brief
  ///   Internal function used by the editor
  VISION_APIFUNC virtual void GetVariableAttributes(VisVariable_cl* pVariable, VVariableAttributeInfo& destInfo) HKV_OVERRIDE;
  /// \brief
  ///   Internal function used by the editor
  VISION_APIFUNC virtual bool GetVariableValue(const char * name, char * value) const HKV_OVERRIDE;

  ///
  /// @}
  ///
#endif
  ///
  /// @name Overridden component functions
  /// @{
  ///

  /// \brief
  ///   Overridden component function
  VISION_APIFUNC virtual void SetOwner(VisTypedEngineObject_cl *pOwner) HKV_OVERRIDE;

  /// \brief
  ///   Overridden component function: Serialize component data to file
  VISION_APIFUNC virtual void Serialize( VArchive &ar ) HKV_OVERRIDE;

  /// \brief
  ///   RTTI setup
  V_DECLARE_SERIAL_DLLEXP(IVNetworkViewComponent, VISION_APIDATA)

  /// \brief
  ///   RTTI setup
  V_DECLARE_VARTABLE(IVNetworkViewComponent, VISION_APIDATA)

  ///
  /// @}
  ///

  ///
  /// @name Groups and Synchronization
  /// @{
  ///

  /// \brief
  ///   Internal function to query serialization groups from the owner object
  VISION_APIFUNC void QuerySynchronizationGroups();

  /// \brief
  ///   Perform synchronization on with all active groups on the owner object
  VISION_APIFUNC void Synchronize(VArchive &ar);

  /// \brief
  ///   Helper function to fully replicate an object from the archive. This component is then attached to it.
  VISION_APIFUNC VisTypedEngineObject_cl *ReplicateObject(VArchive &ar);

  /// \brief
  ///   Helper function to find a synchronization group by its (unique) name. Name comparison is case insensitive. Returns the index of the match or -1
  inline int GetGroupIndexByName(const char *szName) const
  {
    for (int i=0;i<m_iGroupCount;i++)
      if (!_stricmp(m_AllGroupList[i].m_pGroup->GetName(),szName))
        return i;
    return -1;
  }

  /// \brief
  ///   Return the number of synchronization groups that are attached to the owner
  inline int GetGroupCount() const
  {
   return m_iGroupCount;
  }

  /// \brief
  ///   Return the list of groups that are attached to the owner. The instance pointer in the pair is either the owner of this component or a sibling component
  const VNetworkSynchronizationGroupInstanceInfo_t *GetGroupList() const
  {
    return m_AllGroupList;
  }

  /// \brief
  ///   Sets the VSystemGUID identifying the owner of the entity in the network.
  ///
  /// While in a server/client environment all network entities are created by the server, it is sometimes necessary to give control of an entity to a client
  /// (e.g. the player controller). This will still not allow replication from client to server, it is merely used for the client to determine if he should discard
  /// incoming game states in IVNetworkSynchronizationGroup::Synchronize, as he is generating the data himself and wouldn't need updates for those.
  /// See also the RakNetClientServer sample code for client side prediction of input.
  ///
  /// \param iNetworkOwnerID
  ///   Unique system ID of the owner
  void SetNetworkOwnerID(VSystemGUID iNetworkOwnerID)
  {
    m_iNetworkOwnerID = iNetworkOwnerID;
  }

  /// \brief
  ///   Returns the VSystemGUID identifying the owner of the entity in the network.
  ///
  /// \sa SetNetworkOwnerID
  VSystemGUID GetNetworkOwnerID() const
  {
    return m_iNetworkOwnerID;
  }

  /// \brief
  ///   Returns the NetworkID that you can use to refer to this object over the network.
  VISION_APIFUNC virtual VSystemGUID GetNetworkObjectID() const = 0;

  /// \brief
  ///   Sets/removes the flag for a specific group that defines whether the virtual TickFunction is called
  VISION_APIFUNC void SetTickFunctionState(VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, bool bStatus);

  /// \brief
  ///   Method to set/remove the flag for all groups in this component
  VISION_APIFUNC void SetTickFunctionMask(unsigned int iMask);

  /// \brief
  ///   Called every frame by the manager
  VISION_APIFUNC void TickFunction(const VNetworkViewContext &context, float fTimeDelta);
 
  /// \brief
  ///   Adds a child object that should be under observation along with this component
  ///
  /// This is the only way to synchronize hierarchies of objects properly (as opposed to adding a network component both to parent and child object).
  /// From the passed object, the relevant synchronization groups will be added to the list of groups of this component so they are simply synchronized alongside with this
  /// object. An exception are transformation related groups as it is assumed that transformation is propagated through the parent/child relationship.
  /// This function must be called before the component is added to the object through AddComponent
  ///
  /// \param pObject
  ///   The object to synchronize as part of this component. Usually this is a child or grand-child of the component's owner, however this is not a requirement.
  ///
  VISION_APIFUNC void AddObservedObject(VisTypedEngineObject_cl *pObject);

  /// \brief
  ///   Removes an object that has been added through AddObservedObject. Unlike AddObservedObject this can also be called when the component is attached already. A child object that has a shorter lifetime should call this function
  VISION_APIFUNC void RemoveObservedObject(VisTypedEngineObject_cl *pObject);

  ///
  /// @}
  ///

  IVNetworkManager *m_pManager;

  unsigned int m_iGroupsEnabledMask; ///< bitmask that stores which groups are enabled for synchronization
  unsigned int m_iGroupsTickFunctionMask; ///< bitmask that stores which groups use a tick function
#ifdef _VISION_WIN32
  VString m_sGroupBitmaskString; ///< only required inside vForge to temporarily store the string value of the control
#endif
  // current list of groups:
  int m_iGroupCount;
  VNetworkSynchronizationGroupInstanceInfo_t m_AllGroupList[16];
  VSystemGUID m_iNetworkOwnerID;

  /// \brief
  /// internal data structure
  struct VObservedObjectEntry_t
  {
    VObservedObjectEntry_t() {m_pObject=NULL;m_iFirstGroup=0;m_iGroupCount=0;}
    VisTypedEngineObject_cl *m_pObject;
    short m_iFirstGroup, m_iGroupCount; // the range in the parent component's list
  };

  int m_iObservedObjCount;
  DynObjArray_cl<VObservedObjectEntry_t> m_ObservedObj;

};

/// \brief
///   Enum that specifies the type of the connection
enum VNetType_e
{
  VNT_None,     ///< No connection present
  VNT_Server,   ///< The machine is acting as a server
  VNT_Client,   ///< The machine is a client, connected to a server
  VNT_Peer      ///< The machine is a peer in a peer to peer network
};

/// \brief
///   Context structure that gets passed to network related functions to retrieve specific information.
struct VNetworkViewContext
{
  /// \brief
  ///   Enum that specifies the type of synchronization
  enum VNetworkViewType_e
  {
    VNVT_Unknown,        ///< Unknown view type
    VNVT_LiveUpdate,     ///< Future use: live update of properties
    VNVT_Network,        ///< Network synchronization (default)
    VNVT_FirstUserType   ///< Users can define custom types
  };

  /// \brief
  ///   Constructor
  VNetworkViewContext(VNetworkViewType_e eViewType = VNVT_Unknown, unsigned short iUpdateIntervalMS = 30, unsigned short iInterpolationDelayMS = 100, unsigned short iExtrapolationMS = 200)
    : m_eViewType(eViewType), m_eNetType(VNT_None), m_iUpdateIntervalMS (iUpdateIntervalMS), m_bSupportsInterpolation(true), m_iInterpolationDelayMS(iInterpolationDelayMS), m_iExtrapolationMS(iExtrapolationMS),
      m_iUserID(0), m_iCurrentServerTimeMS (0)
  {
  }

  VNetworkViewType_e m_eViewType;         ///< The type of view, \sa VNetworkViewType_e
  VNetType_e m_eNetType;                  ///< The type of the connection, \sa VNetType_e
  unsigned short m_iUpdateIntervalMS;     ///< How often changes are transmitted to the other connections
  bool m_bSupportsInterpolation;          ///< If true (default) groups may use inter-/extrapolation
  unsigned short m_iInterpolationDelayMS; ///< The time in ms rendering trails behind the current server time
  unsigned short m_iExtrapolationMS;      ///< The amount of time to be extrapolated if no further data has arrived yet
  VSystemGUID m_iUserID;                  ///< The unique ID of this user in the network
  __int64 m_iCurrentServerTimeMS;         ///< The current time in MS
  
};



/// \brief
///   Collection of network components
class VNetworkComponentCollection : public VObjectComponentCollection
{
public:
  inline VNetworkComponentCollection()
  {
    m_iIterator = -1;
  }

  inline void TickFunction(const VNetworkViewContext &context, float fTimeDelta)
  {
    for (m_iIterator=0;m_iIterator<Count();m_iIterator++)
      ((IVNetworkViewComponent *)GetAt(m_iIterator))->TickFunction(context,fTimeDelta);
    m_iIterator = -1;
  }

  inline void Remove(IVNetworkViewComponent *pComponent)
  {
    int iIndex = IndexOf(pComponent);
    VASSERT(iIndex>=0);
    RemoveAt(iIndex);
  }

  inline void RemoveAt(int iIndex)
  {
    // while we are inside a loop we have to take care the loop does not get out of sync:
    if (iIndex<=m_iIterator)
      m_iIterator--;
    VObjectComponentCollection::RemoveAt(iIndex);
  }

  int m_iIterator;
};



/// \brief
///   Interface for a custom packet listener.
///
/// Derive from this class and implement OnReceive. Attach the packet listener via IVNetworkManager::SetPacketListener.
/// Every message the network manager receives will pass through the OnReceive function, including internal messages,
/// Vision's messages and any user defined messages.
class IVPacketListener
{
public:
  /// \brief
  ///   Destructor
  virtual ~IVPacketListener() {}

  /// \brief
  ///   The packet listener's receive function.
  ///
  /// \param iID
  ///   ID of the packet, see VPacketID_e.
  ///
  /// \param iTimeMS
  ///   If the packet was timestamped, the local time will be saved in here.
  ///
  /// \param pDataBlock
  ///   A pointer to the start of the packet's data.
  ///
  /// \param iDataLength
  ///   The length of the packet's data.
  ///
  /// \param iSenderID
  ///   The GUID of the sender.
  ///
  /// \return
  ///   Returns whether the packet was processed by this function, processed messages will not be further handled by the network plugin.
  VISION_APIFUNC virtual bool OnReceive(UBYTE iID, __int64 iTimeMS, const unsigned char *pDataBlock, int iDataLength, VSystemGUID iSenderID) = 0;
};



/// \brief
///   base class of the network manager
class IVNetworkManager
{
public:
  /// \brief
  ///   Constructor
  inline IVNetworkManager(VNetworkViewContext context)
    : m_context(context), m_CallbackQueue(true)
  {
    m_pCustomGroupProvider = NULL;
  }

  /// \brief
  ///   Destructor
  VISION_APIFUNC virtual ~IVNetworkManager()
  {
  }

  /// \brief
  ///   Attaches a custom packet listener to the packet receive loop. Must be implemented by any derived network manager.
  ///
  /// \param pListener
  ///   Pointer to the custom packet listener, derived from IVPacketListener
  ///
  /// \sa IVPacketListener
  VISION_APIFUNC virtual void SetPacketListener(IVPacketListener* pListener) = 0;

  /// \brief
  ///   Returns the currently attached custom packet listener. Must be implemented by any derived network manager.
  VISION_APIFUNC virtual IVPacketListener* GetPacketListener() const = 0;

  /// \brief
  ///   Sends a message to pDest's MessageFunction on the system iReceiverUserID. 
  ///
  /// \param pDest
  ///   Target entity to send the message to.
  ///
  /// \param iID
  ///   Message ID, see also: include\Engine\VisApiDefs.hpp
  ///
  /// \param iParamA
  ///   First parameter to VisBaseEntity_cl::MessageFunction
  ///
  /// \param iParamB
  ///   Second parameter to VisBaseEntity_cl::MessageFunction
  ///
  /// \param iReceiverUserID
  ///   Unique system identifier of the Receiver system. Set to 0 to broadcast to all connections.
  ///
  /// \param bSendLocal
  ///   On true, send to the local copy of pDest. on false, sends the message over the network to iReceiverUserID.
  ///
  /// \param pSettings
  ///   Pointer to the settings used to send the message.
  VISION_APIFUNC virtual void SendMessage(VisTypedEngineObject_cl* pDest, int iID, INT_PTR iParamA = 0, INT_PTR iParamB = 0, VSystemGUID iReceiverUserID = 0, bool bSendLocal=false, VMessageSettings* pSettings = NULL) = 0;

  /// \brief
  ///   Sends a message to pDest's MessageFunction on the system iReceiverUserID. 
  ///
  /// \param pDest
  ///   Target entity to send the message to.
  ///
  /// \param iDataBlockMessageID
  ///   Message ID of the DataBlockMessage to differentiate different DataBlockMessages from each other
  ///
  /// \param pDataBlock
  ///   Pointer to the start of the data block in memory
  ///
  /// \param iLength
  ///   Length of pDataBlock
  ///
  /// \param iReceiverUserID
  ///   Unique system identifier of the Receiver system. Set to 0 to broadcast to all connections.
  ///
  /// \param bSendLocal
  ///   On true, send to the local copy of pDest. on false, sends the message over the network to iReceiverUserID.
  ///
  /// \param pSettings
  ///   Pointer to the settings used to send the message.
  VISION_APIFUNC virtual void SendDataBlockMessage(VisTypedEngineObject_cl* pDest, unsigned int iDataBlockMessageID, const void* pDataBlock, unsigned int iLength, VSystemGUID iReceiverUserID = 0, bool bSendLocal=false, VMessageSettings* pSettings = NULL) = 0;

  /// \brief
  ///   Sends a custom message to the system iReceiverUserID which can be read by a package listener derived from IVPacketListener.
  ///
  /// \param iUserMessageID
  ///   Message ID of the custom message to differentiate different custom messages from each other.
  ///   Must be greater or equal to VPackageID_FirstUserID defined in VPacketID_e.
  ///
  /// \param pDataBlock
  ///   Pointer to the start of the data block in memory.
  ///
  /// \param iLength
  ///   Length of pDataBlock.
  ///
  /// \param iReceiverUserID
  ///   Unique system identifier of the Receiver system. Set to 0 to broadcast to all connections.
  ///
  /// \param pSettings
  ///   Pointer to the settings used to send the message.
  VISION_APIFUNC virtual void SendUserMessage(UBYTE iUserMessageID, const void* pDataBlock, unsigned int iLength, VSystemGUID iReceiverUserID = 0, VMessageSettings* pSettings = NULL) = 0;

  /// \brief
  ///   Pure virtual function that must be implemented to create a new network component.
  ///
  /// This is a key function of this interface. Synchronized objects are provided with network components which are implementation specific.
  ///
  /// \return
  ///   A new component instance
  VISION_APIFUNC virtual IVNetworkViewComponent* CreateNetworkViewComponent() = 0;

  /// \brief
  ///   Resolves a IVNetworkViewComponent by its unique NetworkID on this system.
  ///
  /// Each replica is uniquely identified in a network by its VSystemGUID. On each connected system the
  /// local corresponding IVNetworkViewComponent can be resolved via this function.
  ///
  /// \param iNetworkID
  ///   Unique identifier of a IVNetworkViewComponent in the network
  ///
  /// \return
  ///   Pointer to the IVNetworkViewComponent corresponding to iNetworkID. Returns NULL if none is present.
  VISION_APIFUNC virtual IVNetworkViewComponent* GetComponentByNetworkID (VSystemGUID iNetworkID) = 0;

  /// \brief
  ///   Resolves a VisTypedEngineObject_cl by the unique NetworkID of its attached IVNetworkViewComponent.
  ///
  /// Same as GetComponentByNetworkID, but returns the result of GetOwner on the component.
  ///
  /// \param iNetworkID
  ///   Unique identifier of a IVNetworkViewComponent in the network
  ///
  /// \return
  ///   Pointer to the owner of the IVNetworkViewComponent corresponding to iNetworkID. Returns NULL if none is present or if the component has no owner.
  VISION_APIFUNC VisTypedEngineObject_cl* GetObjectByNetworkID (VSystemGUID iNetworkID);


  VISION_APIFUNC void TriggerCallback(IVisCallbackDataObject_cl &data, bool bSendLocal=true)
  {
    if (bSendLocal)
      data.Trigger();
    m_CallbackQueue.Trigger(data);
  }

  inline VCallbackSenderQueue &Callbacks() {return m_CallbackQueue;}

  /// \brief
  ///   Returns the global network context.
  ///
  /// \return
  ///   Reference to the global VNetworkViewContext instance.
  inline const VNetworkViewContext &GetContext() const {return m_context;} 

  /// \brief
  ///   Sets the type of the machine in the network.
  ///
  /// - VNT_None: default if no connection is present
  /// - VNT_Server: this machine should act as a server. It will send replication data to its connected clients but will not accept any replication data
  ///   send to it. The server is the only system in the network that can create replicas.
  /// - VNT_Client: this machine will act as a client. It will receive replication data but cannot send any. Also the client cannot create any replicas.
  /// - VNT_Peer: this machine will act as a peer in a p2p network. It can create replicas and will act as a server for these. For all replicas created by
  ///   other peers in the network it will act like a client.
  /// .
  /// Notice that this value does not change any configurations in the connected network. It is merely used to decide which code path to take.
  /// Also, changing the net type from client to server will not give a user any advantage, as the real server will reject replication data sent by this system.
  /// This value is part of the network context and can be retrieved via IVNetworkManager::GetContext.
  ///
  /// \param eNetType
  ///   The role this machine will play in the network.
  ///
  /// \sa IVNetworkManager::GetContext
  VISION_APIFUNC virtual void SetNetType(VNetType_e eNetType)
  {
    VASSERT (eNetType >= VNT_None || eNetType <= VNT_Peer);
    m_context.m_eNetType = eNetType;
  }

  /// \brief
  ///   Returns the NetType of the system. Same as IVNetworkManager::GetContext().m_eNetType
  inline VNetType_e GetNetType() const
  {
    return m_context.m_eNetType;
  }

  /// \brief
  ///   Sets the unique VSystemGUID of the connected server.
  ///
  /// To skip going through the list of connected systems each time a packet to the server should be sent,
  /// the unique VSystemGUID of the server is saved here and can be retrieved via GetServerID.
  ///
  /// \param iSystemID
  ///   Unique identifier of the server system in the network connected to this machine
  ///
  /// \sa GetServerID
  VISION_APIFUNC virtual void SetServerID(VSystemGUID iSystemID)
  {
    m_iServerID = iSystemID;
  }

  /// \brief
  ///   Returns the unique VSystemGUID of the connected server.
  ///
  /// To skip going through the list of connected systems each time a packet to the server should be sent,
  /// the unique VSystemGUID of the server is retrieved via this function and can be set by calling SetServerID.
  ///
  /// \return
  ///   Unique identifier of the server system in the network connected to this machine
  ///
  /// \sa SetServerID
  inline VSystemGUID GetServerID() const
  {
    return m_iServerID;
  }

  /// \brief
  ///   Sets the time interval in milliseconds between updates to other connected systems.
  ///
  /// This value is part of the network context and can be retrieved via IVNetworkManager::GetContext.
  ///
  /// \param iUpdateIntervalMS
  ///   Milliseconds between updates
  ///
  /// \sa IVNetworkManager::GetContext
  VISION_APIFUNC virtual void SetUpdateIntervalMS (unsigned short iUpdateIntervalMS)
  {
    m_context.m_iUpdateIntervalMS = iUpdateIntervalMS;
  }

  /// \brief
  ///   Sets the time delay in milliseconds that rendering on the client side lags behind the current server time.
  ///
  /// By adding a certain lag to the rendering, one can interpolate between the already received updates from the server.
  /// If the delay is too small, there is not enough data present for the current frame and the current state on the client side has to be extrapolated.
  /// This may lead to artifacts once the next update arrives. If the delay is too large, it will be noticeable by the user which leads to a
  /// suboptimal experience. A good value is twice the update interval as this will allow continuous interpolation of the current state even if one packet
  /// gets dropped.
  /// This value is part of the network context and can be retrieved via IVNetworkManager::GetContext.
  ///
  /// \param iInterpolationDelayMS
  ///   Rendering delay in milliseconds
  ///
  /// \sa IVNetworkManager::GetContext
  VISION_APIFUNC virtual void SetInterpolationDelayMS (unsigned short iInterpolationDelayMS)
  {
    m_context.m_iInterpolationDelayMS = iInterpolationDelayMS;
  }

  /// \brief
  ///   Sets how far the current state should be extrapolated, if no new data from the server arrives.
  ///
  /// This value is part of the network context and can be retrieved via IVNetworkManager::GetContext.
  ///
  /// \param iExtrapolationMS
  ///   Extrapolation length in milliseconds
  ///
  /// \sa IVNetworkManager::GetContext
  VISION_APIFUNC virtual void SetExtrapolationMS (unsigned short iExtrapolationMS)
  {
    m_context.m_iExtrapolationMS = iExtrapolationMS;
  }


  /// \brief
  ///   Installs an optional synchronization group provider which allows for hooking into the process which attaches synchronization groups to single network instances
  ///
  /// Installing a custom provider is optional and can be used if the standard group assignment done by the engine's core
  /// (e.g. attaching VNetworkTransformationGroupI to transformable game objects) is not appropriate. With a custom group provider the
  /// default groups can be filtered and also new ones can be added.
  /// If a custom provider is set with this function, the manager only holds a pointer to the provider instance and does not take ownership
  /// (i.e. the IVSynchronizationGroupProvider interface is not reference counted).
  ///
  /// \param pProvider
  ///   The custom provider to install. There can only be one provider installed on this manager (or NULL for none)
  inline void SetCustomGroupProvider(IVSynchronizationGroupProvider* pProvider)
  {
    m_pCustomGroupProvider=pProvider;
  }

  /// \brief
  ///   Returns the instance of the group provider that has been installed on this manager via SetCustomGroupProvider. By default, this is NULL
  inline IVSynchronizationGroupProvider* GetCustomGroupProvider() {return m_pCustomGroupProvider;}

protected:
  friend class IVNetworkViewComponent;
  VNetworkViewContext::VNetworkViewType_e m_ViewType;
  VNetworkViewContext m_context;
  VCallbackSenderQueue m_CallbackQueue;
  VNetworkComponentCollection m_ComponentsWithTickFunction;
  IVSynchronizationGroupProvider *m_pCustomGroupProvider;
  VSystemGUID m_iServerID; ///< Unique system ID of the server provided for convenience
};


/// \brief
///   Implements IVNetworkSynchronizationGroup as a synchronization group that synchronizes PlayerEntityClientServer's position and rotation.
///
/// The received states are cached in VHistoryData and each frame an intermediate state is computed via interpolation and extrapolation,
/// depending on the current point in time.
///
/// The server is the only one allowed to send replication data and only clients that are not the owner of the entity do interpolation.
/// The client that is the owner of the entity does his own computations via client side prediction that is reevaluated by the server.
///
/// There is one global instance: g_InstanceInterpolatePO
/// /sa PlayerEntityClientServer
class VNetworkTransformationGroupI : public IVNetworkSynchronizationGroup
{
public:
  /// \brief
  ///   Data history for position and rotation
  class VHistoryData : public IVNetworkSynchronizationGroupInstanceData
  {
  public:
    VHistoryData() : m_iChangedDataFlags(0), m_iDataFlagsToSend(0)
    {
    }

    VisDataHistory_cl<hkvVec3, 3> m_positionHistory;
    VisDataHistory_cl<hkvVec3, 3, VLerpEulerAngles> m_rotationHistory;
    BYTE m_iChangedDataFlags;
    BYTE m_iDataFlagsToSend;
  };

  /// \brief
  ///   Flags defining which data is being sent
  enum VChangedDataFlags
  {
    VCD_POSITION = V_BIT(0),
    VCD_ROTATION = V_BIT(1),
    VCD_DataFlags = V_BIT(0) | V_BIT(1),
  };

  VISION_APIFUNC virtual const char *GetName() const HKV_OVERRIDE {return "Transformation";}
  VISION_APIFUNC virtual VGroupTypeFlags_e GetUsageTypeFlags() const HKV_OVERRIDE {return VGroupTypeBit_Transformation;}
  VISION_APIFUNC virtual IVNetworkSynchronizationGroupInstanceData* CreatePerInstanceData() const HKV_OVERRIDE {return (new VHistoryData);}
  VISION_APIFUNC virtual bool QuerySynchronize(const VNetworkViewContext& context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, VMessageSettings& out_paketSettings) HKV_OVERRIDE;
  VISION_APIFUNC virtual void Synchronize(const VNetworkViewContext& context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, VArchive &ar) HKV_OVERRIDE;
  VISION_APIFUNC virtual void TickFunction(const VNetworkViewContext &context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, float fTimeDelta) HKV_OVERRIDE;

  VISION_APIDATA static VNetworkTransformationGroupI g_InstancePO;
};

/// \brief
///   Similar to VNetworkTransformationGroupI, but does not support interpolation. Instead, values are read/written directly
///
/// Since the interpolation mechanism is very different from the direct sync, this is a dedicated class for it 
class VNetworkTransformationGroup : public IVNetworkSynchronizationGroup
{
public:
  VISION_APIFUNC virtual const char *GetName() const HKV_OVERRIDE {return "Transformation";}
  VISION_APIFUNC virtual VGroupTypeFlags_e GetUsageTypeFlags() const HKV_OVERRIDE {return VGroupTypeBit_Transformation;}
  VISION_APIFUNC virtual bool QuerySynchronize(const VNetworkViewContext& context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, VMessageSettings& out_paketSettings) HKV_OVERRIDE;
  VISION_APIFUNC virtual void Synchronize(const VNetworkViewContext& context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, VArchive &ar) HKV_OVERRIDE;

  VISION_APIDATA static VNetworkTransformationGroup g_InstancePO;
};


/// \brief
///   Implements IVNetworkSynchronizationGroup as a synchronization group that synchronizes the visibility of an object. This version supports interpolation
class VNetworkEntityVisibilityGroupI : public IVNetworkSynchronizationGroup
{
public:
  /// \brief
  ///   Data history
  class VHistoryDataVisibility : public IVNetworkSynchronizationGroupInstanceData
  {
  public:
    VisDataHistory_cl<unsigned int, 3, VStepFunction<unsigned int> > m_VisibilityHistory;
  };

  VISION_APIFUNC virtual const char *GetName() const HKV_OVERRIDE {return "Visible Status";}
  VISION_APIFUNC virtual VGroupTypeFlags_e GetUsageTypeFlags() const HKV_OVERRIDE {return VGroupTypeBit_Visibility;}
  VISION_APIFUNC virtual IVNetworkSynchronizationGroupInstanceData* CreatePerInstanceData() const HKV_OVERRIDE {return (new VHistoryDataVisibility);}
  VISION_APIFUNC virtual bool QuerySynchronize(const VNetworkViewContext& context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, VMessageSettings& out_paketSettings) HKV_OVERRIDE;
  VISION_APIFUNC virtual void Synchronize(const VNetworkViewContext& context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, VArchive &ar) HKV_OVERRIDE;
  VISION_APIFUNC virtual void TickFunction(const VNetworkViewContext &context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, float fTimeDelta) HKV_OVERRIDE;

  VISION_APIDATA static VNetworkEntityVisibilityGroupI g_Instance;
};


/// \brief
///   Non-interpolating version of VNetworkEntityVisibilityGroup
class VNetworkEntityVisibilityGroup : public IVNetworkSynchronizationGroup
{
public:
  VISION_APIFUNC virtual const char *GetName() const HKV_OVERRIDE {return "Visible Status";}
  VISION_APIFUNC virtual VGroupTypeFlags_e GetUsageTypeFlags() const HKV_OVERRIDE {return VGroupTypeBit_Visibility;}
  VISION_APIFUNC virtual bool QuerySynchronize(const VNetworkViewContext& context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, VMessageSettings& out_paketSettings) HKV_OVERRIDE;
  VISION_APIFUNC virtual void Synchronize(const VNetworkViewContext& context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, VArchive &ar) HKV_OVERRIDE;
  
  VISION_APIDATA static VNetworkEntityVisibilityGroup g_Instance;
};

#endif // VISAPINETWORKMANAGER_HPP_INCLUDED

/*
 * Havok SDK - Base file, BUILD(#20140624)
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
