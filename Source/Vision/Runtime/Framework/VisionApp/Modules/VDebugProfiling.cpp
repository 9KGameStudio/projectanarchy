/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/Framework/VisionApp/Modules/VDebugProfiling.hpp>
#include <Vision/Runtime/Framework/VisionApp/VAppImpl.hpp>

V_IMPLEMENT_DYNCREATE(VDebugProfiling, VAppModule, Vision::GetEngineModule());

#if defined(_VISION_WIN32)
  #define PREV_PROFILING_PAGE VAPP_INPUT_CONTROL_LAST_ELEMENT - 5
  #define NEXT_PROFILING_PAGE VAPP_INPUT_CONTROL_LAST_ELEMENT - 6
  #define RESET_MAX_VALUES VAPP_INPUT_CONTROL_LAST_ELEMENT - 7
#endif

VDebugProfiling::VDebugProfiling()
  : VAppModule()
  , m_iCurrentProfilingPage(-1)
{
}

void VDebugProfiling::Init()
{
#if defined(_VISION_WIN32)
  GetParent()->GetInputMap()->MapTrigger(PREV_PROFILING_PAGE, VInputManager::GetKeyboard(), CT_KB_F9, VInputOptions::Once(ONCE_ON_RELEASE));
  GetParent()->GetInputMap()->MapTrigger(NEXT_PROFILING_PAGE, VInputManager::GetKeyboard(), CT_KB_F10, VInputOptions::Once(ONCE_ON_RELEASE));
  GetParent()->GetInputMap()->MapTrigger(RESET_MAX_VALUES, VInputManager::GetKeyboard(), CT_KB_F11, VInputOptions::Once(ONCE_ON_RELEASE));

  Vision::Callbacks.OnUpdateSceneBegin += this;
#endif

  Vision::Callbacks.OnAfterSceneLoaded += this;
}

void VDebugProfiling::DeInit()
{
  DeRegisterCallbacks();
  Vision::Callbacks.OnAfterSceneLoaded -= this;

#if defined(_VISION_WIN32)
  Vision::Callbacks.OnUpdateSceneBegin -= this;
#endif
}

void VDebugProfiling::OnHandleCallback(IVisCallbackDataObject_cl* pData)
{
  if (pData->m_pSender == &Vision::Callbacks.OnUpdateSceneBegin)
  {
#if defined(_VISION_WIN32)
    if (GetParent()->GetInputMap()->GetTrigger(PREV_PROFILING_PAGE))
    {
      int iIndex = m_iCurrentProfilingPage - 1;
      if (iIndex < -1)
        iIndex = Vision::Profiling.GetNumOfGroups() - 1;

      OnActionTriggered(iIndex);
    }
    else if (GetParent()->GetInputMap()->GetTrigger(NEXT_PROFILING_PAGE))
    {
      int iIndex = m_iCurrentProfilingPage + 1;
      if (iIndex >= (int)Vision::Profiling.GetNumOfGroups())
        iIndex = -1;

      OnActionTriggered(iIndex);
    }
    else if (GetParent()->GetInputMap()->GetTrigger(RESET_MAX_VALUES))
    {
      OnActionTriggered(s_iResetMaxValuesAction);
    }
#endif
  }
  else if (pData->m_pSender == &Vision::Callbacks.OnAfterSceneLoaded)
  {
    RebuildMenu();
  }
  else
  {    
    OnActionTriggered(GetCallbackIndex(pData));
  }
}

void VDebugProfiling::RebuildMenu()
{
  const char* szGroupName = "Debug Profiling";

  DeRegisterCallbacks();

  VAppMenu* pMainMenu = GetParent()->GetAppModule<VAppMenu>();
  if (pMainMenu == NULL)
    return;

  pMainMenu->RemoveGroup(szGroupName);

  VProfilingNode* pRoot = Vision::Profiling.GetProfilingRootNode();
  if (pRoot)
  {
    VAppMenuItems items;
    const unsigned int iCount = Vision::Profiling.GetNumOfGroups();
    for (unsigned int i=0; i<iCount; ++i)
    {
      VProfilingNode* pNode = pRoot->Children().GetAt(i);
      if (pNode)
        items.Add(VAppMenuItem(pNode->GetName(), i, i, true));
    }

    items.Add(VAppMenuItem("Reset Max Values", s_iResetMaxValuesAction, iCount, false));

    m_callbacks = pMainMenu->RegisterGroup(szGroupName, items, NULL, VAPP_DEFAULT_SORTING_1, false);
    RegisterCallbacks();
  }
}

void VDebugProfiling::OnActionTriggered(int iIndex)
{
  VAppMenu* pMainMenu = GetParent()->GetAppModule<VAppMenu>();
  const VProfilingNodeCollection& pages = Vision::Profiling.GetProfilingRootNode()->Children();

  // deselect old menu item
  if (m_iCurrentProfilingPage != -1)
  {
    if (pMainMenu!=NULL)
      pMainMenu->SetItemCheckState(pages.GetAt(m_iCurrentProfilingPage)->GetName(), false);
  }

  if (iIndex == s_iResetMaxValuesAction)
  {
    Vision::Profiling.ResetProfilingMaxValues();
  }
  else
  {
    if (iIndex == -1 || Vision::Profiling.IsPageVisible(iIndex))
    {
      Vision::Profiling.Hide();
      m_iCurrentProfilingPage = -1;
    }
    else
    {
      Vision::Profiling.SetPage(iIndex);
      m_iCurrentProfilingPage = iIndex;
    }
  }

  // select new menu item
  if (m_iCurrentProfilingPage != -1)
  {
    if (pMainMenu!=NULL)
      pMainMenu->SetItemCheckState(pages.GetAt(m_iCurrentProfilingPage)->GetName(), true);
  }
}

/*
 * Havok SDK - Base file, BUILD(#20140804)
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
