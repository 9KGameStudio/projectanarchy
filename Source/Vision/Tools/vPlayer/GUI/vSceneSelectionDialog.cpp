/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Tools/vPlayer/vPlayerPCH.h>
#include <Vision/Tools/vPlayer/GUI/vCommonControls.hpp>
#include <Vision/Tools/vPlayer/GUI/vSceneSelectionDialog.hpp>

#include <Vision/Runtime/Framework/VisionApp/VAppModule.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////

class TabButton : public VStyledButton
{
public:
  TabButton(const char* szTabTitle) : VStyledButton(szTabTitle, false, VUISharedData::GetFont(), VUISharedData::GetScale(), VUISharedData::GetBorderWidth(), VUISharedData::GetFontScale())
  {
    m_TextCfg.FinishSetup();

    m_bIsSelected = false;
  }

  virtual void OnPaint(VGraphicsInfo& Graphics, const VItemRenderInfo& parentState) HKV_OVERRIDE
  {
    const VRectanglef rect = GetBoundingBox();
    VColorRef color = m_bIsSelected ? VAppMenuColors::GetColor(VAppMenuColors::COLOR_LIST_CONTROL_BG) : VAppMenuColors::GetColor(VAppMenuColors::COLOR_DIALOG_BG);
    const VSimpleRenderState_t iProperties = VGUIManager::DefaultGUIRenderState();

    Graphics.Renderer.DrawSolidQuad(rect.m_vMin, rect.m_vMax, color, iProperties);

    if (!m_bIsSelected)
    {
      const float fBorderWidth = VUISharedData::GetBorderWidth();
      color = VAppMenuColors::GetColor(VAppMenuColors::COLOR_LIST_CONTROL_BG);
      Graphics.Renderer.DrawSolidQuad(rect.m_vMin, hkvVec2(rect.m_vMax.x, rect.m_vMin.y + fBorderWidth), color, iProperties);
      Graphics.Renderer.DrawSolidQuad(hkvVec2(rect.m_vMin.x, rect.m_vMin.y), hkvVec2(rect.m_vMin.x + fBorderWidth, rect.m_vMax.y), color, iProperties);
      Graphics.Renderer.DrawSolidQuad(hkvVec2(rect.m_vMax.x, rect.m_vMin.y), hkvVec2(rect.m_vMax.x + fBorderWidth, rect.m_vMax.y), color, iProperties);
    }

    VPushButton::OnPaint(Graphics, parentState);
  }

  bool m_bIsSelected;
};

///////////////////////////////////////////////////////////////////////////////////////////////////

class ListItem;

class ListButton : public VStyledButton
{
  V_DECLARE_DYNAMIC(ListButton);
public:
  ListButton(const char* szText, int iID, ListItem* pParentItem) : VStyledButton(szText, true, VUISharedData::GetFont(), VUISharedData::GetScale(), VUISharedData::GetBorderWidth(), VUISharedData::GetFontScale())
  {
    m_iID = iID;
    m_pParentItem = pParentItem;

    SetSize(90.0f * VUISharedData::GetScale(), VUISharedData::GetIconSize());
  }

  virtual void OnPaint(VGraphicsInfo& Graphics, const VItemRenderInfo& parentState) HKV_OVERRIDE;

  ListItem* m_pParentItem;
};

V_IMPLEMENT_DYNAMIC(ListButton, VPushButton, Vision::GetEngineModule());

///////////////////////////////////////////////////////////////////////////////////////////////////

class ListItem : public VListControlItem
{
public:
  ListItem(const char* szText, VTextureObject* pThumbnail, hkUint32 uiFlags, VDialog* pParentDialog)
  {
    m_iCustomHeight = (int)(VUISharedData::GetIconSize() + 2 * VUISharedData::GetPadding());

    SetText(szText);
    m_Text.SetFont(VUISharedData::GetFont());
    m_Text.m_States[VWindowBase::NORMAL].SetColor(VAppMenuColors::GetColor(VAppMenuColors::COLOR_LIST_ITEM_TEXT_NORMAL));
    m_Text.m_States[VWindowBase::MOUSEOVER].SetColor(VAppMenuColors::GetColor(VAppMenuColors::COLOR_LIST_ITEM_TEXT_OVER));
    m_Text.m_States[VWindowBase::SELECTED].SetColor(VAppMenuColors::GetColor(VAppMenuColors::COLOR_LIST_ITEM_TEXT_SELECTED));
    m_Text.SetVerticalAlignment(VisFont_cl::ALIGN_CENTER);
    m_Text.SetTextOfs(hkvVec2(VUISharedData::GetPadding(), 0.0f));
    m_Text.SetScaling(VUISharedData::GetFontScale());
    m_Text.FinishSetup();

    m_Icon.m_States[VWindowBase::NORMAL].SetTexture(pThumbnail);

    m_pClearCacheButton = NULL;
    m_pRemoveFromListButton = NULL;

    if ((uiFlags & ISceneListDataProvider::SpecificFlags::CACHE_ENABLED) != 0)
    {
      m_pClearCacheButton = new ListButton("Clear Cache", VSceneListEvent::CLEAR_CACHE, this);
      m_pClearCacheButton->SetOrder(1);      
      pParentDialog->AddControl(m_pClearCacheButton);
    }

    if ((uiFlags & IDataProvider::Flags::READ_ONLY) == 0)
    {
      m_pRemoveFromListButton = new ListButton("Remove", VSceneListEvent::REMOVE_FROM_LIST, this);
      m_pRemoveFromListButton->SetOrder(1);
      pParentDialog->AddControl(m_pRemoveFromListButton);
    }
  }

  ~ListItem()
  {
    VDialog* pParentDialog = vstatic_cast<VDialog*>(GetParent()->GetParent());
    pParentDialog->Items().SafeRemove(m_pClearCacheButton);
    pParentDialog->Items().SafeRemove(m_pRemoveFromListButton);
  }

  void SetButtonsVisibility(bool bState)
  {
    if (m_pClearCacheButton != NULL)
      m_pClearCacheButton->SetVisible(bState);

    if (m_pRemoveFromListButton != NULL)
      m_pRemoveFromListButton->SetVisible(bState);
  }

  virtual void OnPaint(VGraphicsInfo& Graphics, const VItemRenderInfo& parentState) HKV_OVERRIDE
  {
    if (IsClipped())
      return;

    VItemRenderInfo thisState(parentState, this, 1.0f);

    const int iItemCount = static_cast<VListControl*>(GetParent())->Items().Count();

    VRectanglef rect = thisState.m_pWindow->GetBoundingBox();
    const float fBorderWidth = VUISharedData::GetBorderWidth();
    rect.m_vMin += hkvVec2(fBorderWidth, (GetIndex() == 0) ? fBorderWidth : fBorderWidth * 0.5f);
    rect.m_vMax -= hkvVec2(fBorderWidth, (GetIndex() == iItemCount - 1) ? fBorderWidth : fBorderWidth * 0.5f);

    const bool bHighlight = (IsMouseOver() || IsSelected());
    VColorRef color = bHighlight ? VAppMenuColors::GetColor(VAppMenuColors::COLOR_LIST_ITEM_BG) : VAppMenuColors::GetColor(VAppMenuColors::COLOR_LIST_GROUP_BG);
    const VSimpleRenderState_t iProperties = VGUIManager::DefaultGUIRenderState();

    Graphics.Renderer.DrawSolidQuad(rect.m_vMin, rect.m_vMax, color, iProperties);

    // Item itself (text, etc.)
    VListControlItem::OnPaint(Graphics, parentState);
  }

  virtual void OnMeasureItem(const VRectanglef& area, hkvVec2& vCurrentPos) HKV_OVERRIDE
  {
    VListControlItem::OnMeasureItem(area, vCurrentPos);

    if (IsClipped())
    {
      SetButtonsVisibility(false);
    }
    else
    {
      hkvVec2 btnPos = m_vPosition + GetParent()->GetPosition();
      btnPos.x += GetSize().x;
      btnPos.y += (GetSize().y - VUISharedData::GetIconSize()) * 0.5f;

      if (m_pRemoveFromListButton != NULL)
      {
        btnPos.x -= m_pRemoveFromListButton->GetSize().x + VUISharedData::GetPadding();

        m_pRemoveFromListButton->SetPosition(btnPos.x, btnPos.y);
        m_pRemoveFromListButton->SetVisible(true);

        btnPos.x -= VUISharedData::GetPadding();
      }

      if (m_pClearCacheButton != NULL)
      {
        btnPos.x -= m_pClearCacheButton->GetSize().x + VUISharedData::GetPadding();

        m_pClearCacheButton->SetPosition(btnPos.x, btnPos.y);
        m_pClearCacheButton->SetVisible(true);
      }
    }
  }

  ListButton* m_pClearCacheButton;
  ListButton* m_pRemoveFromListButton;
};

///////////////////////////////////////////////////////////////////////////////////////////////////

void ListButton::OnPaint(VGraphicsInfo& Graphics, const VItemRenderInfo& parentState)
{
  const VRectanglef rect = GetBoundingBox();
  const VRectanglef listRect = m_pParentItem->GetParent()->GetBoundingBox();
  const bool bNeedsClipping = !listRect.IsCompletelyInside(rect);

  if (bNeedsClipping)
    Graphics.ClippingStack.Push(listRect, false);

  VStyledButton::OnPaint(Graphics, parentState);

  if (bNeedsClipping)
    Graphics.ClippingStack.Pop();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

VSceneSelectionPage::VSceneSelectionPage(const char* szTabTitle, ISceneListDataProvider* pDataProvider, int iIndex, VDialog* pParentDialog)
{
  const float fBorderWidth = 5.0f * VUISharedData::GetScale();

  VSliderControl* pScrollbar = new VSliderControl();
  pScrollbar->SetSize(VUISharedData::GetScrollBarWidth(), 1.0f);
  pScrollbar->SetSliderRange(0, 100, 0);
  pScrollbar->Frame().m_States[VWindowBase::NORMAL].SetTexture(Vision::TextureManager.Load2DTexture("Textures/vapp_scrollbar.dds"));
  pScrollbar->GetSlider()->Images().m_States[VWindowBase::NORMAL].SetTexture(Vision::TextureManager.Load2DTexture("Textures/vapp_slider.dds"));
  pScrollbar->GetSlider()->Images().m_States[VWindowBase::NORMAL].SetTextureRange(VRectanglef(-VUISharedData::GetScale()*0.5f, -VUISharedData::GetScale()*0.5f, VUISharedData::GetScale()*0.5f, VUISharedData::GetScale()*0.5f));
  pScrollbar->GetSlider()->OnBuildFinished();

  SetBackgroundColor(VAppMenuColors::GetColor(VAppMenuColors::COLOR_LIST_CONTROL_BG));
  SetIconSize(VUISharedData::GetIconSize(), hkvVec2(VUISharedData::GetPadding()));
  SetScrollBar(pScrollbar);
  pParentDialog->AddControl(this);

  m_pTabButton = new TabButton(szTabTitle);
  pParentDialog->AddControl(m_pTabButton);

  m_pDataProvider = pDataProvider;
  m_pDataProvider->m_OnDataChanged += this;

  m_iIndex = iIndex;
  
  UpdateSceneList(NULL);
}

VSceneSelectionPage::~VSceneSelectionPage()
{
  m_pDataProvider->m_OnDataChanged -= this;
}

void VSceneSelectionPage::OnItemClicked(VMenuEventDataObject* pEvent)
{
  ListButton* pButton = vdynamic_cast<ListButton*>(pEvent->m_pItem);
  if (pButton != NULL && pButton->m_pParentItem->GetParent() == this)
  {
    VSceneSelectionDialog* pDialog = static_cast<VSceneSelectionDialog*>(GetParent());
    VisCallback_cl* pCallback = pButton->GetID() == VSceneListEvent::CLEAR_CACHE ? &pDialog->m_OnClearCache : &pDialog->m_OnRemoveFromList;

    VSceneListEvent eventData(pCallback, m_iIndex, pButton->m_pParentItem->GetIndex(), static_cast<VSceneListEvent::Action>(pButton->GetID()));
    pCallback->TriggerCallbacks(&eventData);
  }
  else if (pEvent->m_pItem == this)
  {
    VGUIUserInfo_t* pUser = GetContext()->GetUser(VGUIUserInfo_t::GUIUser0);
    VListControlItem* pItem = GetItemAt(*pUser, pEvent->m_vMousePos);
    if (pItem != NULL && pItem->IsSelected())
    {
      VSceneSelectionDialog* pDialog = static_cast<VSceneSelectionDialog*>(GetParent());

      VSceneListEvent eventData(&pDialog->m_OnLoadScene, m_iIndex, pItem->GetIndex(), VSceneListEvent::LOAD_SCENE);
      pDialog->m_OnLoadScene.TriggerCallbacks(&eventData);
    }
  }
}

void VSceneSelectionPage::OnTick(float dtime)
{
  for (int i = 0; i < VGUIUserInfo_t::GUIMaxUser; ++i)
  {
    VGUIUserInfo_t* pUser = GetContext()->GetUser((VGUIUserInfo_t::VGUIUserID_e)i);
    if (pUser == NULL) 
      continue;

    ListItem* pItem = static_cast<ListItem*>(GetItemAt(*pUser, pUser->m_vMousePos));
    if (pItem != NULL)
    {
      if ((pItem->m_pClearCacheButton != NULL && pItem->m_pClearCacheButton->IsMouseOver()) || 
        (pItem->m_pRemoveFromListButton != NULL && pItem->m_pRemoveFromListButton->IsMouseOver()))
      {
        pItem = NULL;
      }
    }

    SetMouseOverItem(*pUser, pItem);
  }

  TickScrollbar(dtime);
}

void VSceneSelectionPage::OnHandleCallback(IVisCallbackDataObject_cl* pData)
{
  if (pData->m_pSender == &m_pDataProvider->m_OnDataChanged)
  {
    UpdateSceneList(static_cast<VSceneListEvent*>(pData));
  }
}

void VSceneSelectionPage::SetSelectionState(bool bSelect)
{
  SetVisible(bSelect);
  m_pTabButton->m_bIsSelected = bSelect;

  for (int i = 0; i < m_Items.Count(); ++i)
  {
    ListItem* pItem = static_cast<ListItem*>(m_Items.GetAt(i));
    pItem->SetButtonsVisibility(bSelect);
  }  
}

void VSceneSelectionPage::DoLayout(float& fButtonX)
{
  const float fSrollbarWidth = GetScrollBar()->GetSize().x;
  const float fOuterBorderWidth = VUISharedData::GetOuterBorderWidth();
  const float fTabsHeight = VUISharedData::GetTabsHeight();
  const float fStatusBarHeight = VUISharedData::GetStatusBarHeight();

  VRectanglef rect;
  VisFont_cl* pFont = m_pTabButton->Text().GetFont();
  pFont->GetTextDimension(m_pTabButton->Text().GetText(), rect);

  float fTextWidth = rect.GetSizeX() * VUISharedData::GetFontScale() + VUISharedData::GetPadding() * 20.0f;

  m_pTabButton->SetPosition(fOuterBorderWidth + fButtonX, fOuterBorderWidth);
  m_pTabButton->SetSize(fTextWidth, fTabsHeight);
  fButtonX += fTextWidth;

  const float fDialogSpacing = VUISharedData::GetDialogSpacing();
  float fResX = (float)Vision::Video.GetXRes() - fDialogSpacing * 2.0f;
  float fResY = (float)Vision::Video.GetYRes() - fDialogSpacing * 2.0f;

  SetPosition(fOuterBorderWidth, fOuterBorderWidth + fTabsHeight);
  SetSize(fResX - 2 * fOuterBorderWidth - fSrollbarWidth, fResY - 2 * fOuterBorderWidth - fTabsHeight - fStatusBarHeight);
}

bool VSceneSelectionPage::WasTabClicked(VMenuEventDataObject* pEvent)
{
  return pEvent->m_pItem == m_pTabButton;
}

void VSceneSelectionPage::UpdateSceneList(VSceneListEvent* pData)
{
  const int iEntryIndex = pData != NULL ? pData->GetEntryIndex() : -1;
  if (iEntryIndex >= 0)
  {
    const VSceneListEntry& entry = m_pDataProvider->GetData()[iEntryIndex];
    
    VListControlItem* pItem = m_Items.GetAt(iEntryIndex);
    pItem->SetText(entry.sDisplayName);

    for (unsigned int i=0; i<STATE_COUNT; ++i)
    {
      pItem->m_Icon.m_States[i].SetTexture(entry.spThumbnail);
    }
  }
  else
  {
    Reset();

    const VArray<VSceneListEntry>& list = m_pDataProvider->GetData();
    for (int i = 0; i < list.GetSize(); ++i)
    {
      const VSceneListEntry& entry = list[i];

      ListItem* pItem = new ListItem(entry.sDisplayName, entry.spThumbnail, m_pDataProvider->GetFlags(), static_cast<VDialog*>(GetParent()));
      pItem->SetButtonsVisibility(IsVisible());
      AddItem(pItem, -1, false);
    }
  }
}

//////////////////////////////////////////////////////////////////////////

VSceneSelectionDialog::VSceneSelectionDialog(ITextDataProvider* pStatusDataProvider)
{
  const float fResX = (float)Vision::Video.GetXRes();
  const float fResY = (float)Vision::Video.GetYRes();

  const float fDialogSpacing = VUISharedData::GetDialogSpacing();
  SetPosition(fDialogSpacing, fDialogSpacing);
  SetSize(fResX - fDialogSpacing * 2.0f, fResY - fDialogSpacing * 2.0f);
  m_iBackColor = VAppMenuColors::GetColor(VAppMenuColors::COLOR_DIALOG_BG);

  const float fTabsHeight = VUISharedData::GetTabsHeight();
  m_pCloseButton = new VStyledButton("X", false, VUISharedData::GetFont(), VUISharedData::GetScale(), VUISharedData::GetBorderWidth(), VUISharedData::GetFontScale());
  m_pCloseButton->SetSize(VUISharedData::GetScale()*90.0f, fTabsHeight - VUISharedData::GetScale()*2.0f);
  m_pCloseButton->SetOrder(2);

  AddControl(m_pCloseButton);

  m_pStatusText = new VTextLabel();
  m_pStatusText->Text().SetFont(VUISharedData::GetFont());
  m_pStatusText->Text().SetScaling(VUISharedData::GetFontScale());
  m_pStatusText->Text().FinishSetup();

  AddControl(m_pStatusText);

  m_pStatusDataProvider = pStatusDataProvider;
  m_pStatusDataProvider->m_OnDataChanged += this;

  UpdateStatusText();
}

VSceneSelectionDialog::~VSceneSelectionDialog()
{
  m_pStatusDataProvider->m_OnDataChanged -= this;

  m_Pages.Reset();
}

void VSceneSelectionDialog::OnItemClicked(VMenuEventDataObject* pEvent)
{
  VDialog::OnItemClicked(pEvent);

  for (int i = 0; i < m_Pages.GetSize(); ++i)
  {
    if (m_Pages[i]->WasTabClicked(pEvent))
    {
      SelectPageByIndex(i);
      return;
    }
  }

  if (pEvent->m_pItem == m_pCloseButton)
  {
    m_OnClose.TriggerCallbacks();
    return;
  }

  for (int i = 0; i < m_Pages.GetSize(); ++i)
  {
    m_Pages[i]->OnItemClicked(pEvent);
  }
}

void VSceneSelectionDialog::OnHandleCallback(IVisCallbackDataObject_cl* pData)
{
  if (pData->m_pSender == &m_pStatusDataProvider->m_OnDataChanged)
  {
    UpdateStatusText();
  }
}

void VSceneSelectionDialog::AddPage(const char* szPageTitle, ISceneListDataProvider* pDataProvider)
{
  m_Pages.Add(new VSceneSelectionPage(szPageTitle, pDataProvider, m_Pages.GetSize(), this));
  
  DoLayout();
}

void VSceneSelectionDialog::SelectPageByIndex(int iIndex)
{
  for (int i = 0; i < m_Pages.GetSize(); ++i)
  {
    m_Pages[i]->SetSelectionState(i == iIndex);
  }

  SetFocusItem(m_Pages[iIndex]);
}


void VSceneSelectionDialog::DoLayout()
{
  float fButtonX = 0;
  for (int i = 0; i < m_Pages.GetSize(); ++i)
  {
    m_Pages[i]->DoLayout(fButtonX);
  }

  const float fDialogSpacing = VUISharedData::GetDialogSpacing();
  const float fOuterBorderWidth = VUISharedData::GetOuterBorderWidth();
  const float fSizeX = GetSize().x - m_pCloseButton->GetSize().x - fOuterBorderWidth - VUISharedData::GetScrollBarWidth() - VUISharedData::GetPadding();
  m_pCloseButton->SetPosition(fSizeX, fOuterBorderWidth);

  const float fStatusBarHeight = VUISharedData::GetStatusBarHeight();
  m_pStatusText->SetPosition(fOuterBorderWidth, GetSize().y - fStatusBarHeight);
  m_pStatusText->SetSize(GetSize().x - 2 * fOuterBorderWidth, fStatusBarHeight);
}

void VSceneSelectionDialog::UpdateStatusText()
{
  m_pStatusText->SetText(m_pStatusDataProvider->GetData());

  VColorRef color = m_pStatusDataProvider->GetColor();
  for (int i = 0; i < VWindowBase::STATE_COUNT; ++i)
  {
    m_pStatusText->Text().m_States[i].SetColor(color);
  }
}

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
