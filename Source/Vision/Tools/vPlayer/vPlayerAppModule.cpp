/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Tools/vPlayer/vPlayerPCH.h>
#include <Vision/Tools/vPlayer/vPlayerAppConfig.hpp>
#include <Vision/Tools/vPlayer/vPlayerAppModule.hpp>
#include <Vision/Tools/vPlayer/vThumbnailManager.hpp>
#include <Vision/Tools/vPlayer/GUI/vCommonControls.hpp>

#include <Vision/Runtime/Framework/VisionApp/VAppImpl.hpp>
#include <Vision/Runtime/Framework/VisionApp/Modules/VExitHandler.hpp>

#include <Vision/Runtime/Base/System/IO/FileSystems/VFileServeDaemon.hpp>

//////////////////////////////////////////////////////////////////////////

static VSceneListEvent s_EventDataAllChanged = VSceneListEvent(NULL, -1, -1, VSceneListEvent::DATA_CHANGED);

class SceneListDataProvider : public ISceneListDataProvider
{
public:
  SceneListDataProvider(const char* szFilePath, hkUint32 uiTargetPlatformFilterMask, hkUint32 flags = Flags::DEFAULT) :
    ISceneListDataProvider(flags)
  {
    m_List.LoadFromFile(szFilePath, uiTargetPlatformFilterMask);
    LoadThumbnails();
  }

  virtual VArray<VSceneListEntry>& GetData() HKV_OVERRIDE
  {
    return m_List;
  }

  virtual const VArray<VSceneListEntry>& GetData() const HKV_OVERRIDE
  {
    return m_List;
  }

  void Reload()
  {
    m_List.Reload();
    LoadThumbnails();
    m_OnDataChanged.TriggerCallbacks(&s_EventDataAllChanged);
  }

  void LoadThumbnails()
  {
    for (int i = 0; i < m_List.GetSize(); ++i)
    {
      VThumbnailManager::LoadThumbnail(this, i);    
    }
  }

  VSceneList m_List;
};

//////////////////////////////////////////////////////////////////////////

class StatusDataProvider : public ITextDataProvider
{
public:
  StatusDataProvider(hkUint32 flags = Flags::DEFAULT) : ITextDataProvider(flags) {}

  virtual const VString& GetData() const HKV_OVERRIDE
  {
    return m_sData;
  }

  virtual VColorRef GetColor() const HKV_OVERRIDE
  {
    return m_Color;
  }

  void Update()
  {
#if defined(USE_FILESERVE)
    if (VFileServeDaemon::IsInitialized())
    {
      VFileServeDaemon* pDaemon = VFileServeDaemon::GetInstance();

      if (pDaemon->IsConnected())
      {
        m_sData.Format("FileServe connected to %s (%s:%d)", pDaemon->GetRemoteMachineName(), pDaemon->GetRemoteHost(), pDaemon->GetReconnectionPort());
        m_Color = VColorRef(0, 220, 0);
      }
      else
      {
        m_sData.Format("FileServe not connected. Using Cache instead.");
        m_Color = VColorRef(220, 0, 0);
      }    
    }
    else
    {
      m_sData.Format("FileServe not initialized.");
      m_Color = VColorRef(128, 128, 128);
    }
#endif

    m_OnDataChanged.TriggerCallbacks();
  }

private:
  VString m_sData;
  VColorRef m_Color;
};

//////////////////////////////////////////////////////////////////////////

V_IMPLEMENT_DYNAMIC(VPlayerAppModule, VAppModule, Vision::GetEngineModule());

VPlayerAppModule::VPlayerAppModule(const VPlayerAppConfig* pConfig)
{
  m_pConfig = pConfig;
  m_pToggleSelectionDialog = NULL;
  m_pStatusDataProvider = NULL;
  m_bRequestPending = false;  

  m_pCommandConnection = NULL;

  VTarget::OnConnection += this;
  VTarget::OnDisconnection += this;
}

VPlayerAppModule::~VPlayerAppModule()
{
  VTarget::OnDisconnection -= this;
  VTarget::OnConnection -= this;
}

void VPlayerAppModule::Init()
{
  // Try to establish a SCVR connection by soliciting from the host of the FileServe connection.
  SolicitCommandConnection();


  VUISharedData::Init();
  VThumbnailManager::Init();

  // setup data providers
  {
    const hkUint32 uiTargetPlatformFilterMask = m_pConfig->uiTargetPlatformFilterMask;

    hkUint32 uiFlags = IDataProvider::Flags::DEFAULT;
    if (VFileServeDaemon::IsInitialized())
      uiFlags |= ISceneListDataProvider::SpecificFlags::CACHE_ENABLED;

    m_SceneListDataProviders[DP_RECENT]   = new SceneListDataProvider(":app_data/RecentScenes.lua", uiTargetPlatformFilterMask, uiFlags);

    uiFlags |= IDataProvider::Flags::READ_ONLY;
    m_SceneListDataProviders[DP_EXPORTED] = new SceneListDataProvider(":havok_sdk/Data/Vision/Tools/vPlayer/Exported.lua", uiTargetPlatformFilterMask, uiFlags);
    m_SceneListDataProviders[DP_SAMPLES]  = new SceneListDataProvider(":havok_sdk/Data/Vision/Tools/vPlayer/Samples.lua", uiTargetPlatformFilterMask, uiFlags);
    m_SceneListDataProviders[DP_SAMPLES]->m_List.Sort();

    m_pStatusDataProvider = new StatusDataProvider();
    m_pStatusDataProvider->Update();
  }

  // setup dialogs
  {
    m_spSelectionDialog = new VSceneSelectionDialog(m_pStatusDataProvider);

    VAppMenuContextPtr spContext = GetParent()->GetContext();
    spContext->ShowDialog(m_spSelectionDialog);

    VAppMenu* pMainMenu = GetParent()->GetAppModule<VAppMenu>();
    m_pToggleSelectionDialog = pMainMenu->RegisterItem(VAppMenuItem("Load Scene", 0, 0, false));
    *m_pToggleSelectionDialog += this;

    m_spSelectionDialog->AddPage("Recently Used", m_SceneListDataProviders[DP_RECENT]);
    m_spSelectionDialog->AddPage("Exported", m_SceneListDataProviders[DP_EXPORTED]);
    m_spSelectionDialog->AddPage("Samples", m_SceneListDataProviders[DP_SAMPLES]);

    // find a non empty page
    int iPageIndex = DP_RECENT;
    while (m_SceneListDataProviders[iPageIndex]->m_List.GetSize() == 0 && iPageIndex < DP_SAMPLES)
    {
      ++iPageIndex;
    }
    m_spSelectionDialog->SelectPageByIndex(iPageIndex);

    m_spSelectionDialog->m_OnClose += this;
    m_spSelectionDialog->m_OnLoadScene += this;
    m_spSelectionDialog->m_OnClearCache += this;
    m_spSelectionDialog->m_OnRemoveFromList += this;
    Vision::Callbacks.OnFrameUpdatePreRender += this;

    m_spInfoDialog = new VInfoDialog();
    spContext->ShowDialog(m_spInfoDialog);
    HideInfoDialog();

    m_spInfoDialog->m_OnOK += this;
    m_spInfoDialog->m_OnCancel += this;
  }

  SetVariantKeys();
  if (!m_pConfig->sFileToLoad.IsEmpty())
  {
    LoadFromFile(m_pConfig->sFileToLoad);
    HideSelectionDialog();
  }
  else
  {
    ShowSelectionDialog();
  }
}

void VPlayerAppModule::DeInit()
{
  Vision::Callbacks.OnFrameUpdatePreRender -= this;
  m_spSelectionDialog->m_OnRemoveFromList -= this;
  m_spSelectionDialog->m_OnClearCache -= this;  
  m_spSelectionDialog->m_OnLoadScene -= this;
  m_spSelectionDialog->m_OnClose -= this;

  *m_pToggleSelectionDialog -= this;

  HideSelectionDialog();
  m_spSelectionDialog = NULL;

  m_spInfoDialog->m_OnOK -= this;
  m_spInfoDialog->m_OnCancel -= this;

  HideInfoDialog();
  m_spInfoDialog = NULL;

  for (VPOSITION it = m_loaders.GetStartPosition(); it != NULL;)
  {
    VString key;
    ILoader* pLoader = NULL;
    m_loaders.GetNextPair(it, key, pLoader);

    delete pLoader;
  }

  for (int i = 0; i < DP_COUNT; ++i)
  {
    V_SAFE_DELETE(m_SceneListDataProviders[i]);
  }

  V_SAFE_DELETE(m_pStatusDataProvider);

  VThumbnailManager::DeInit();
  VUISharedData::DeInit();
}

void VPlayerAppModule::OnHandleCallback(IVisCallbackDataObject_cl* pData)
{
  if (pData->m_pSender == &Vision::Callbacks.OnFrameUpdatePreRender)
  {
    if (m_spInfoDialog->IsVisible() && GetParent()->GetInputMap()->GetTrigger(VAPP_EXIT))
    {
      HideInfoDialog();
    }
    else if (!m_spSelectionDialog->IsVisible() && GetParent()->GetInputMap()->GetTrigger(VAPP_EXIT))
    {
      ShowSelectionDialog();
    }
  }
  else if (pData->m_pSender == m_pToggleSelectionDialog)
  {    
    ShowSelectionDialog();
  }
  else if (pData->m_pSender == &m_spSelectionDialog->m_OnClose)
  {
    HideSelectionDialog();
  }
  else if (pData->m_pSender == &m_spSelectionDialog->m_OnLoadScene)
  {
    VSceneListEvent* pEventData = static_cast<VSceneListEvent*>(pData);
    RequestLoad(m_SceneListDataProviders[pEventData->GetListIndex()]->m_List[pEventData->GetEntryIndex()]);
  }
  else if (pData->m_pSender == &m_spSelectionDialog->m_OnClearCache)
  {
    VSceneListEvent* pEventData = static_cast<VSceneListEvent*>(pData);
    ClearCache(m_SceneListDataProviders[pEventData->GetListIndex()]->m_List[pEventData->GetEntryIndex()]);
  }
  else if (pData->m_pSender == &m_spSelectionDialog->m_OnRemoveFromList)
  {
    VSceneListEvent* pEventData = static_cast<VSceneListEvent*>(pData);
    RemoveFromList(pEventData->GetListIndex(), pEventData->GetEntryIndex());
  }
  else if (pData->m_pSender == &m_spInfoDialog->m_OnOK)
  {
    HideInfoDialog();
    ShowSelectionDialog();
  }
  else if (pData->m_pSender == &m_spInfoDialog->m_OnCancel)
  {
    HideInfoDialog();
  }
  else if (pData->m_pSender == &VTarget::OnConnection)
  {
    VTargetConnectionCallbackItem* pItem = static_cast<VTargetConnectionCallbackItem*>(pData);

    VMutexLocker lock(m_commandConnectionMutex);
    if (pItem->pConnection->IsFor("SCVR"))
    {
      m_pCommandConnection = pItem->pConnection;
    }
    else if(pItem->pConnection->IsFor("FSRV"))
    {
      // If FileServe reconnects, we should try to restore the SCVR command connection as well.
      SolicitCommandConnection();
    }
  }
  else if (pData->m_pSender == &VTarget::OnDisconnection)
  {
    VTargetConnectionCallbackItem* pItem = static_cast<VTargetConnectionCallbackItem*>(pData);
    if (pItem->pConnection == m_pCommandConnection)
    {
      VMutexLocker lock(m_commandConnectionMutex);
      m_pCommandConnection = NULL;
    }
  }
}

int VPlayerAppModule::GetCallbackSortingKey(VCallback *pCallback)
{
  if (pCallback == &Vision::Callbacks.OnFrameUpdatePreRender)
  {
    // Ensures that the vPlayer is the first one reacting to OnFrameUpdatePreRender
    // (important for input stuff)
    return 1;
  }

  return VAppModule::GetCallbackSortingKey(pCallback);
}

void VPlayerAppModule::ShowSelectionDialog()
{
  EnterModalState();

  if (VExitHandler* pExit = GetParent()->GetAppModule<VExitHandler>())
  {
    pExit->SetEnabled(true);
  }

  if (!m_spSelectionDialog->IsVisible())
  {
    m_spSelectionDialog->SetVisible(true);

    IVGUIContext* pContext = GetParent()->GetContext();
    pContext->ShowDialog(m_spSelectionDialog);
  }
}

void VPlayerAppModule::HideSelectionDialog()
{
  if (m_spSelectionDialog->IsVisible())
  {
    IVGUIContext* pContext = m_spSelectionDialog->GetContext();
    pContext->CloseDialog(m_spSelectionDialog);

    m_spSelectionDialog->SetVisible(false);
  }

  if (VExitHandler* pExit = GetParent()->GetAppModule<VExitHandler>())
  {
    pExit->SetEnabled(false);
  }

  LeaveModalState();
}

void VPlayerAppModule::ShowInfoDialog(const char* szText, VInfoDialog::Buttons buttons)
{
  EnterModalState();

  if (VExitHandler* pExit = GetParent()->GetAppModule<VExitHandler>())
  {
    pExit->SetEnabled(false);
  }

  if (!m_spInfoDialog->IsVisible())
  {
    m_spInfoDialog->SetVisible(true);
    m_spInfoDialog->SetText(szText);
    m_spInfoDialog->SetButtons(buttons);

    IVGUIContext* pContext = GetParent()->GetContext();
    pContext->ShowDialog(m_spInfoDialog);
  }
}

void VPlayerAppModule::HideInfoDialog()
{
  if (m_spInfoDialog->IsVisible())
  {
    IVGUIContext* pContext = m_spInfoDialog->GetContext();
    pContext->CloseDialog(m_spInfoDialog);

    m_spInfoDialog->SetVisible(false);
  }

  if (VExitHandler* pExit = GetParent()->GetAppModule<VExitHandler>())
  {
    pExit->SetEnabled(true);
  }

  LeaveModalState();
}

void VPlayerAppModule::RegisterLoader(ILoader* pLoader)
{
  VString sExtension = pLoader->GetSupportedExtension();
  sExtension.ToLower();

  m_loaders.SetAt(sExtension.AsChar(), pLoader);
}

void VPlayerAppModule::ProcessPendingLoadingRequest()
{
  VThumbnailManager::ProcessPendingThumbnails();

  if (m_bRequestPending)
  {
    m_bRequestPending = false;

    HideSelectionDialog();

    VString sExtension = VFileHelper::GetExtension(m_pendingLoadingRequest.sScenePath.AsChar());
    sExtension.ToLower();

    VStaticString<1024> sError;
    ILoader* pLoader = NULL;
    if (!m_loaders.Lookup(sExtension, pLoader))
    {
      sError.Format("No loader found for extension '%s'", sExtension.AsChar());
      hkvLog::Error(sError.AsChar());
      ShowInfoDialog(sError.AsChar(), VInfoDialog::OK);
      return;
    }

    VASSERT(pLoader != NULL);

    if (!pLoader->Load(m_pendingLoadingRequest))
    {
      VString errorString = pLoader->GetLastError();
      if(errorString.IsEmpty())
      {
        sError.Format("Could not load '%s'", m_pendingLoadingRequest.sScenePath.AsChar());
      }
      else
      {
        sError.Format("Could not load '%s': %s", m_pendingLoadingRequest.sScenePath.AsChar(), errorString.AsChar());
      }
      hkvLog::Error(sError.AsChar());
      ShowInfoDialog(sError.AsChar(), VInfoDialog::OK);
      return;
    }

    if (pLoader->NeedsManualCallbackAfterLoading())
    {
      { // Trigger callback OnAfterSceneLoaded
        VisSceneLoadedDataObject_cl data(&Vision::Callbacks.OnAfterSceneLoaded, m_pendingLoadingRequest.sScenePath.AsChar());
        Vision::Callbacks.OnAfterSceneLoaded.TriggerCallbacks(&data);
      }

      GetParent()->AfterSceneLoaded(true);
    } 
  }
}

void VPlayerAppModule::ProcessMessages()
{
  if (m_pStatusDataProvider != NULL)
  {
    m_pStatusDataProvider->Update();
  }

  VMutexLocker lock(m_commandConnectionMutex);
  if (m_pCommandConnection == NULL)
    return;

  bool bWantsRefresh = false;

  VMessage* pLastOpenMessage = NULL;

  while (VMessage* pMessage = m_pCommandConnection->GetNextASyncReceivedMessage())
  {
    switch(pMessage->GetMessageType())
    {
    case 'REFR':
      bWantsRefresh = true;
      V_SAFE_DELETE(pMessage);
      break;

    case 'OPEN':
      // Only read the last received OPEN message - ignore all others.
      V_SAFE_DELETE(pLastOpenMessage);
      pLastOpenMessage = pMessage;
      break;
    }
  }

  if (pLastOpenMessage)
  {
    char* szScenePath;
    char* szSearchPaths;
    if (pLastOpenMessage->ReadString(&szScenePath) && pLastOpenMessage->ReadString(&szSearchPaths))
    {
      VSceneListEntry entry;
      entry.uiTargetPlatforms = V_BIT(TARGETDEVICE_THIS);

      char sceneName[FS_MAX_PATH];
      vstrncpy(sceneName, VPathHelper::GetFilename(szScenePath), FS_MAX_PATH);
      VPathHelper::GetFilenameNoExt(sceneName, sceneName);	// Remove .vscene
      VPathHelper::GetFilenameNoExt(sceneName, sceneName);	// Remove .<profile>
      entry.sDisplayName.Format("%s (Exported Scene)", sceneName);
      entry.sScenePath = szScenePath;

      VNameValueListParser<';', '='> parser(szSearchPaths, false);
      while (parser.next())
      {
        entry.sSearchPaths.Append(parser.name());
      }

      RequestLoad(entry);     
    }

    V_SAFE_DELETE(pLastOpenMessage);
  }


  if(bWantsRefresh)
  {
    m_SceneListDataProviders[DP_EXPORTED]->Reload();
  }
}

void VPlayerAppModule::FinalizeLoading(bool bLoadingSuccessful)
{
  if (!m_pendingLoadingRequest.sDisplayName.IsEmpty()) // valid request?
  {
    if (bLoadingSuccessful)
    {   
      AddToRecentList(m_pendingLoadingRequest);
    }
    else
    {
      VStaticString<1024> sError;
      sError.Format("Could not load '%s': %s", m_pendingLoadingRequest.sScenePath.AsChar(), 
        GetParent()->GetSceneLoader().GetLastError());
      ShowInfoDialog(sError.AsChar(), VInfoDialog::OK);
    }

    // reset loading request
    m_pendingLoadingRequest = VSceneListEntry();
  }
}

//////////////////////////////////////////////////////////////////////////

void VPlayerAppModule::EnterModalState()
{
  if (VAppMenu* pMenu = GetParent()->GetAppModule<VAppMenu>())
  {
    pMenu->Reset();
    pMenu->SetEnabled(false);
  }

  VInputMap::LockInputMaps(true);
  GetParent()->GetInputMap()->SetEnabled(true);
}

void VPlayerAppModule::LeaveModalState()
{
  if (VInputMap::AreInputMapsLocked())
  {
    VInputMap::LockInputMaps(false);
  }

  if (VAppMenu* pMenu = GetParent()->GetAppModule<VAppMenu>())
  {
    pMenu->SetEnabled(true);
  }
}

void VPlayerAppModule::ClearCache(const VSceneListEntry& entry)
{
  if (VFileServeDaemon::IsInitialized())
  {
    for (int i = 0; i < entry.sSearchPaths.GetSize(); ++i)
    {
      VFileServeDaemon::GetInstance()->ClearCacheDirectory(entry.sSearchPaths[i]);
    }
  }
}

void VPlayerAppModule::RemoveFromList(int iListIndex, int iEntryIndex)
{
  VSceneList& list = m_SceneListDataProviders[iListIndex]->m_List;

  // backup the entry so it can be deleted from the recent list later
  VSceneListEntry entry = list[iEntryIndex]; 
  list.RemoveAt(iEntryIndex);

  list.SaveToFile();
  m_SceneListDataProviders[iListIndex]->m_OnDataChanged.TriggerCallbacks(&s_EventDataAllChanged);

  if (iListIndex != DP_RECENT)
    RemoveFromRecentList(entry);
}

void VPlayerAppModule::AddToRecentList(const VSceneListEntry& entry)
{
  VSceneList& list = m_SceneListDataProviders[DP_RECENT]->m_List;

  int iIndex = list.Find(entry);
  if (iIndex < 0)
  {
    iIndex = list.GetSize();
    list.Add(entry);
    VThumbnailManager::LoadThumbnail(m_SceneListDataProviders[DP_RECENT], iIndex);
  }
  list[iIndex].LastUsed = VDateTime::Now();

  list.Sort();
  list.SaveToFile();
  m_SceneListDataProviders[DP_RECENT]->m_OnDataChanged.TriggerCallbacks(&s_EventDataAllChanged);
}

void VPlayerAppModule::RemoveFromRecentList(const VSceneListEntry& entry)
{
  VSceneList& list = m_SceneListDataProviders[DP_RECENT]->m_List;

  int iIndex = list.Find(entry);
  if (iIndex >= 0)
  {
    list.RemoveAt(iIndex);

    list.SaveToFile();
    m_SceneListDataProviders[DP_RECENT]->m_OnDataChanged.TriggerCallbacks(&s_EventDataAllChanged);
  } 
}

void VPlayerAppModule::RequestLoad(const VSceneListEntry& entry)
{
  m_pendingLoadingRequest = entry;
  m_bRequestPending = true;
}

void VPlayerAppModule::LoadFromFile(const char* szAbsoluteScenePath)
{
  char szSceneDir[FS_MAX_PATH];

  VFileHelper::GetFileDir(szAbsoluteScenePath, szSceneDir);
  VString sRelativeScenePath = VFileHelper::GetFilename(szAbsoluteScenePath);

  char szProjectDir[FS_MAX_PATH];
  if (VFileHelper::FindProjectDir(szSceneDir, szProjectDir))
  {
    // make the scene name relative to project path
    sRelativeScenePath = szAbsoluteScenePath + strlen(szProjectDir) + 1;
  }
  else
  {
    strcpy(szProjectDir, szSceneDir);
  }

  VStaticString<FS_MAX_PATH> sWorkspaceDir, sProjectSearchPath;
  if(VBaseAppHelpers::FindVForgeWorkspace(szProjectDir, "workspace", sWorkspaceDir, sProjectSearchPath) == HKV_FAILURE)
  {
    hkvLog::Info("hkvAssetProject: No workspace found, using project directory.");
    sWorkspaceDir = szProjectDir;
    sProjectSearchPath = ":workspace";
  }

  strcpy(szSceneDir, sRelativeScenePath.AsChar());
  VFileHelper::BackToFrontSlash(szSceneDir);
  VFileHelper::BackToFrontSlash(sProjectSearchPath);
  VFileHelper::BackToFrontSlash(sWorkspaceDir);  

  VSceneListEntry entry;
  entry.uiTargetPlatforms = TARGETPLATFORM_MASK_THIS;
  entry.sDisplayName.Format("%s (in %s)", szSceneDir, sProjectSearchPath.AsChar());
  entry.sScenePath = szSceneDir;
  entry.sSearchPaths.Append(sProjectSearchPath.AsChar());
  entry.sRoot = sWorkspaceDir; 

  RequestLoad(entry);
}

void VPlayerAppModule::SetVariantKeys()
{
  VNameValueListParser<';', '='> parser(m_pConfig->sVariantKeys.AsChar(), false);
  while (parser.next())
  {
    const char* szVarKey = parser.name();

    AssetVariantKeys::Add(szVarKey);
  }
}

void VPlayerAppModule::SolicitCommandConnection()
{
  if (VFileServeDaemon::IsInitialized())
  {
    VFileServeDaemon* pDaemon = VFileServeDaemon::GetInstance();
    VTargetThread::GetTargetThread()->GetTarget()->SolicitConnection(pDaemon->GetRemoteHost(), pDaemon->GetReconnectionPort(), "SCVR");
  }
}

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
