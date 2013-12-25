/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef MainMenu_h__
#define MainMenu_h__

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VMenuIncludes.hpp>

#include <Vision/Tools/vPlayer/vSceneList.hpp>

class TabButton;
class VStyledButton;

class VSceneSelectionPage : public VListControl, public IVisCallbackHandler_cl
{
public:
  VSceneSelectionPage(const char* szTabTitle, ISceneListDataProvider* pDataProvider, int iIndex, VDialog* pParentDialog);
  ~VSceneSelectionPage();

  void OnItemClicked(VMenuEventDataObject* pEvent);
  virtual void OnTick(float dtime) HKV_OVERRIDE;

  virtual void OnHandleCallback(IVisCallbackDataObject_cl* pData) HKV_OVERRIDE;

  void DoLayout(float& fButtonX);
  void SetSelectionState(bool bSelect);

  bool WasTabClicked(VMenuEventDataObject* pEvent);

  void UpdateSceneList(VSceneListEvent* pData);

private:
  TabButton* m_pTabButton;

  ISceneListDataProvider* m_pDataProvider;

  int m_iIndex;
};

class VSceneSelectionDialog : public VDialog, public IVisCallbackHandler_cl
{
public:
  VSceneSelectionDialog(ITextDataProvider* pStatusDataProvider);
  virtual ~VSceneSelectionDialog();

  virtual void OnItemClicked(VMenuEventDataObject* pEvent) HKV_OVERRIDE;

  virtual void OnHandleCallback(IVisCallbackDataObject_cl* pData) HKV_OVERRIDE;
 
  void AddPage(const char* szPageTitle, ISceneListDataProvider* pDataProvider);
  
  void SelectPageByIndex(int iIndex);

  VisCallback_cl m_OnClose;
  VisCallback_cl m_OnLoadScene;
  VisCallback_cl m_OnClearCache;
  VisCallback_cl m_OnRemoveFromList;

private:

  void DoLayout();

  void UpdateStatusText();

  VArray<VSceneSelectionPage*> m_Pages;

  VStyledButton* m_pCloseButton;

  VTextLabel* m_pStatusText;
  ITextDataProvider* m_pStatusDataProvider;
};

#endif // MainMenu_h__

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
