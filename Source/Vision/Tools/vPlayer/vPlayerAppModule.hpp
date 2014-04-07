/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef vPlayerAppModule_h__
#define vPlayerAppModule_h__

#include <Vision/Runtime/Framework/VisionApp/VAppModule.hpp>

#include <Vision/Tools/vPlayer/vSceneList.hpp>
#include <Vision/Tools/vPlayer/GUI/vInfoDialog.hpp>
#include <Vision/Tools/vPlayer/GUI/vSceneSelectionDialog.hpp>

struct VPlayerAppConfig;
class VAppImpl;
class SceneListDataProvider;
class StatusDataProvider;

class ILoader
{
public:
  ILoader(const char* szSupportedExtension) : m_sSupportedExtension(szSupportedExtension) {}
  virtual ~ILoader() {}

  virtual bool Load(const VSceneListEntry& entry) = 0;

  virtual bool NeedsManualCallbackAfterLoading() const = 0;

  virtual VString GetLastError() const = 0;

  const char* GetSupportedExtension() const { return m_sSupportedExtension.AsChar(); }

private:
  VString m_sSupportedExtension;
};

class VPlayerAppModule : public VAppModule
{
  V_DECLARE_DYNAMIC(VPlayerAppModule);

public:
  VPlayerAppModule(const VPlayerAppConfig* pConfig);
  virtual ~VPlayerAppModule();

  virtual void Init() HKV_OVERRIDE;
  virtual void DeInit() HKV_OVERRIDE;

  virtual void OnHandleCallback(IVisCallbackDataObject_cl* pData) HKV_OVERRIDE;
  virtual int GetCallbackSortingKey(VCallback *pCallback) HKV_OVERRIDE;

  void ShowSelectionDialog();
  void HideSelectionDialog();

  void ShowInfoDialog(const char* szText, VInfoDialog::Buttons buttons);
  void HideInfoDialog();

  void RegisterLoader(ILoader* pLoader);

  void ProcessPendingLoadingRequest();
  void ProcessMessages();

  void FinalizeLoading(bool bLoadingSuccessful);

private:
  void SolicitCommandConnection();

  void EnterModalState();
  void LeaveModalState();

  void ClearCache(const VSceneListEntry& entry);
  void RemoveFromList(int iListIndex, int iEntryIndex);

  void AddToRecentList(const VSceneListEntry& entry);
  void RemoveFromRecentList(const VSceneListEntry& entry);

  void RequestLoad(const VSceneListEntry& entry);
  void LoadFromFile(const char* szAbsoluteScenePath);
  void SetVariantKeys();

  const VPlayerAppConfig* m_pConfig;

  VSmartPtr<VSceneSelectionDialog> m_spSelectionDialog;
  VSmartPtr<VInfoDialog> m_spInfoDialog;

  VisCallback_cl* m_pToggleSelectionDialog;
  
  enum
  {
    DP_RECENT,
    DP_EXPORTED,
    DP_SAMPLES,
    
    DP_COUNT
  };

  SceneListDataProvider* m_SceneListDataProviders[DP_COUNT];
  StatusDataProvider* m_pStatusDataProvider;

  VSceneListEntry m_pendingLoadingRequest;
  bool m_bRequestPending;

  VMap<VString, ILoader*> m_loaders;

  VMutex m_commandConnectionMutex;
  VConnection* m_pCommandConnection;
};

#endif // vPlayerAppModule_h__

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
