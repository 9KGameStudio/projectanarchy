/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>         // precompiled header
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VGUIIncludes.hpp>


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class VTreeViewCollapseButton
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

// internal class
class VTreeViewCollapseButton : public VDlgControlBase
{
public:
  VTreeViewCollapseButton(VTreeViewItem *pItem);
  virtual ~VTreeViewCollapseButton() {}

  virtual void OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState) HKV_OVERRIDE;
  virtual void OnClick(VMenuEventDataObject *pEvent) HKV_OVERRIDE;
  virtual VCursor *GetMouseOverCursor(VGUIUserInfo_t &user) HKV_OVERRIDE;

  VTreeViewItem *m_pOwner;
};

VTreeViewCollapseButton::VTreeViewCollapseButton(VTreeViewItem *pItem)
  : m_pOwner(pItem)
{
  SetSize(16.f,16.f);
  this->SetParent(pItem->m_pOwner);
}

void VTreeViewCollapseButton::OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState)
{
  VDlgControlBase::OnPaint(Graphics, parentState);
  VImageStates &states(m_pOwner->IsCollapsed() ? m_pOwner->m_pOwner->m_CollapsedButtonStyle : m_pOwner->m_pOwner->m_UnCollapsedButtonStyle);
  VItemRenderInfo thisInfo(parentState,this,1.f);

  states.OnPaint(Graphics, thisInfo);
}


VCursor *VTreeViewCollapseButton::GetMouseOverCursor(VGUIUserInfo_t &user) 
{
  VImageStates &states(m_pOwner->IsCollapsed() ? m_pOwner->m_pOwner->m_CollapsedButtonStyle : m_pOwner->m_pOwner->m_UnCollapsedButtonStyle);
  return states.m_States[GetCurrentState()].GetCursor();
}

void VTreeViewCollapseButton::OnClick(VMenuEventDataObject *pEvent)
{
  VDlgControlBase::OnClick(pEvent);
  m_pOwner->SetCollapsed(!m_pOwner->IsCollapsed()); // toggle
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class VTreeViewItem
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

VTreeViewItem::VTreeViewItem(VTreeViewControl *pOwner, VDlgControlBase *pControl)
{
  m_Items.m_pParentItem = this;
  m_bCollapsed = m_bClipped = false;
  m_pOwner = pOwner;
  m_spControl = pControl;
  m_pParentItem = NULL;

  if (pControl!=NULL)
  {
    pControl->m_pOwner = pOwner;
    pControl->m_pContext = pOwner->GetContext();
  }
}

VTreeViewItem::~VTreeViewItem()
{
  V_SAFE_DISPOSEOBJECT(m_spCollapseButton);
  V_SAFE_DISPOSEOBJECT(m_spControl);
}


void VTreeViewItem::OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState)
{
  if (m_bClipped)
    return;
  if (m_spControl!=NULL)
    m_spControl->OnPaint(Graphics, parentState);
  if (m_spCollapseButton!=NULL)
    m_spCollapseButton->OnPaint(Graphics, parentState);

  if (!IsCollapsed())
    m_Items.OnPaint(Graphics, parentState);
}


void VTreeViewItem::MeasureItem(const VRectanglef &area, hkvVec2 &vCurrentPos)
{
  m_bClipped = vCurrentPos.y > this->m_pOwner->GetSize().y;
  if (m_bClipped)
    return;
  float fHeight = 0.f;
  if (m_spControl!=NULL)
  {
    fHeight = m_spControl->m_vSize.y;
    m_spControl->m_vPosition = hkvVec2(vCurrentPos.x+m_pOwner->m_fIndentCollapseButton, vCurrentPos.y);
  }

  // create the collapse button aand set visible state
  if (m_Items.Count()>0)
  {
    if (m_spCollapseButton==NULL)
      m_spCollapseButton = new VTreeViewCollapseButton(this);
  }

  if (m_spCollapseButton!=NULL)
  {
    m_spCollapseButton->m_vPosition.set(vCurrentPos.x, vCurrentPos.y + floorf((fHeight-m_spCollapseButton->m_vSize.y)*0.5f)); // center vertically
    m_spCollapseButton->SetVisible(m_Items.Count()>0);
  }

  vCurrentPos.y += fHeight;
  hkvVec2 vIndentPos(vCurrentPos.x + m_pOwner->m_fIndentChildren, vCurrentPos.y);

  if (!IsCollapsed())
    m_Items.MeasureItems(area, vIndentPos);

  vCurrentPos.y = vIndentPos.y;
}

VWindowBase* VTreeViewItem::TestMouseOver(VGUIUserInfo_t &user, const hkvVec2 &vAbsMouse)
{
  if (m_bClipped)
    return NULL;
  if (m_spControl!=NULL)
  {
    VWindowBase* pHit = m_spControl->TestMouseOver(user,vAbsMouse);
    if (pHit!=NULL)
      return pHit;
  }
  if (m_spCollapseButton!=NULL && m_spCollapseButton->IsVisible())
  {
    if (m_spCollapseButton->TestMouseOver(user,vAbsMouse))
      return m_spCollapseButton;
 //   VRectanglef r(m_spCollapseButton->GetBoundingBox());
 //   Vision::Game.DrawSingleLine2D(r.m_vMin.x,r.m_vMin.y,r.m_vMax.x,r.m_vMax.y);
  }
  if (IsCollapsed())
    return NULL;
  return m_Items.TestMouseOver(user, vAbsMouse); // recurse
}


void VTreeViewItem::SetCollapsed(bool bStatus, bool bRecursive)
{
  if (bStatus==IsCollapsed())
    return;
  m_bCollapsed = bStatus;
  m_pOwner->FlagForMeasure();

  if (bRecursive)
    m_Items.SetCollapsed(bStatus, bRecursive);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class VTreeViewItemCollection
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void VTreeViewItemCollection::OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState)
{
  for (int i=0;i<Count() && !GetAt(i)->m_bClipped;i++)
    GetAt(i)->OnPaint(Graphics, parentState);
}


void VTreeViewItemCollection::MeasureItems(const VRectanglef &area, hkvVec2 &vCurrentPos)
{
  for (int i=0;i<Count();i++)
    GetAt(i)->MeasureItem(area, vCurrentPos);
}


VWindowBase* VTreeViewItemCollection::TestMouseOver(VGUIUserInfo_t &user, const hkvVec2 &vAbsMouse)
{
  for (int i=0;i<Count() && !GetAt(i)->m_bClipped; i++)
  {
    VWindowBase* pHit = GetAt(i)->TestMouseOver(user, vAbsMouse);
    if (pHit!=NULL)
      return pHit;
  }
  return NULL;
}


void VTreeViewItemCollection::Add(VTreeViewItem *pItem)
{
  VRefCountedCollection<VTreeViewItem>::Add(pItem);
  pItem->m_pOwner->FlagForMeasure();
  pItem->m_pParentItem = m_pParentItem;
}
void VTreeViewItemCollection::Remove(VTreeViewItem *pItem)
{
  VRefCountedCollection<VTreeViewItem>::Remove(pItem);
  pItem->m_pOwner->FlagForMeasure();
  pItem->m_pParentItem = NULL;
}
void VTreeViewItemCollection::RemoveAt(int iIndex)
{
  GetAt(iIndex)->m_pOwner->FlagForMeasure();
  GetAt(iIndex)->m_pParentItem = NULL;
  VRefCountedCollection<VTreeViewItem>::RemoveAt(iIndex);
}
void VTreeViewItemCollection::Clear()
{
  for (int i=0;i<Count();i++)
  {
    GetAt(i)->m_pOwner->FlagForMeasure();
    GetAt(i)->m_pParentItem = NULL;
  }
  VRefCountedCollection<VTreeViewItem>::Clear();
}

void VTreeViewItemCollection::SetCollapsed(bool bStatus, bool bRecursive)
{
  for (int i=0;i<Count();i++)
    GetAt(i)->SetCollapsed(bStatus, bRecursive);
}


void VTreeViewItemCollection::SerializeX( VArchive &ar, VTreeViewControl *pOwner)
{
  char iLocalVersion = 0;
  int iCount;
  if (ar.IsLoading())
  {
    ar >> iLocalVersion; VASSERT_MSG(iLocalVersion==0,"Invalid version");
    ar >> iCount;
    EnsureCapacity(iCount);
    for (int i=0;i<iCount;i++)
    {
      VDlgControlBase *pControl = (VDlgControlBase *)ar.ReadObject(VDlgControlBase::GetClassTypeId());
      VTreeViewItem *pNewItem = new VTreeViewItem(pOwner, pControl);
      Add(pNewItem);
      pNewItem->m_Items.SerializeX(ar, pOwner);
    }
  } 
  else
  {
    ar << iLocalVersion;
    iCount = Count();
    ar << iCount;
    for (int i=0;i<iCount;i++)
    {
      ar.WriteObject(GetAt(i)->m_spControl);
      GetAt(i)->m_Items.SerializeX(ar, pOwner);
    }
  }
}


bool VTreeViewItemCollection::Build(TiXmlElement *pNode, const char *szPath, bool bWrite, VTreeViewControl *pOwner)
{
  if (!pNode)
    return false;

  VASSERT_MSG(bWrite==false, "Writing not supported yet");

  // iterate through all "listitem" elements
  for (TiXmlElement *pItemNode=pNode->FirstChildElement("item"); pItemNode; pItemNode=pItemNode->NextSiblingElement("item") )
  {
    const char *szClassName = XMLHelper::Exchange_String(pItemNode,"class",NULL,bWrite);
    VASSERT(szClassName != NULL && szClassName[0]); // must be defined!

    VType *pType = Vision::GetTypeManager()->GetType(szClassName);
    VASSERT(pType && "Control class not available");
    if (!pType)
      continue;
    
    VDlgControlBase *pControl = (VDlgControlBase *)pType->CreateInstance();
    // sanity check
    if (!pControl->IsOfType(Vision::GetTypeManager()->GetType("VDlgControlBase")))
      hkvLog::FatalError("class '%s' is not derived from base class VDlgControlBase",szClassName);
    pControl->SetParent(pOwner);
    pControl->Build(pItemNode, szPath, bWrite);
    pControl->OnBuildFinished();
    VTreeViewItem *pNewItem = new VTreeViewItem(pOwner, pControl);
    
    // parse child collection
    pNewItem->m_Items.Build(XMLHelper::SubNode(pItemNode,"treeitems",bWrite), szPath, bWrite, pOwner);

    Add(pNewItem);
  }

  return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class VTreeViewSlider
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

class VTreeViewSlider : public VSliderControl
{
public:
  VTreeViewSlider(VTreeViewControl *pOwner)
  {
    SetParent(pOwner);
    SetVertical(true);
    SetSize(16.f,32.f);

    m_Frame.SetStretchMode(VImageState::STRETCHED);
    for (int i=0;i<VWindowBase::STATE_COUNT;i++)
    {
      m_Frame.m_States[i].SetTexture(Vision::TextureManager.GetPlainWhiteTexture());
      m_Frame.m_States[i].SetColor(VColorRef(80,80,80, 128));
    }

    m_bIgnoreChange = false;

    AdjustSize();
  }

  VTreeViewSlider()
  {
  }

  virtual ~VTreeViewSlider()
  {
  }

  virtual void OnBuildFinished() HKV_OVERRIDE
  {
    VSliderControl::OnBuildFinished();

    if (m_spSlider==NULL)
    {
      VSlider *pSlider = GetSlider(); // create one
      pSlider->SetSize(16.f,16.f);
      pSlider->m_vUnscaledSize.set(16.f,16.f);
      pSlider->m_Image.SetStretchMode(VImageState::STRETCHED);
      for (int i=0;i<VWindowBase::STATE_COUNT;i++)
      {
        pSlider->m_Image.m_States[i].SetTexture(Vision::TextureManager.GetPlainWhiteTexture());
        pSlider->m_Image.m_States[i].SetColor(VColorRef(220,220,220, 220));
      }
      pSlider->OnBuildFinished();
    }
  }


  virtual void OnValueChanged(VItemValueChangedEvent *pEvent) HKV_OVERRIDE
  {
    if (m_bIgnoreChange)
      return;
    VSliderControl::OnValueChanged(pEvent);
    float fPos = floorf(pEvent->AsFloat());
    m_bIgnoreChange = true;
    VTreeViewControl *pOwner = (VTreeViewControl *)GetParent();
    pOwner->SetScrollPosition(fPos);
    m_bIgnoreChange = false;
  }


  void AdjustSize()
  {
    const VRectanglef &r(m_pOwner->GetClientRect());
    SetPosition(r.GetSizeX()-m_vSize.x, 0.f);
    SetSize(m_vSize.x, r.GetSizeY());
  }

// serialization
  V_DECLARE_SERIAL_DLLEXP( VTreeViewSlider, GUI_IMPEXP_DATA );
  GUI_IMPEXP virtual void Serialize( VArchive &ar ) HKV_OVERRIDE
  {
    VSliderControl::Serialize(ar);
  }

  bool m_bIgnoreChange;
};

V_IMPLEMENT_SERIAL( VTreeViewSlider, VSliderControl, 0, &g_VisionEngineModule );


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class VTreeViewControl
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

VTreeViewControl::VTreeViewControl()
{
  m_bNeedsMeasure = true;
  m_fIndentCollapseButton = 20.f;
  m_fIndentChildren = 24.f;
  m_fCurrentScrollPos = 0.f;
  m_iBackgroundCol.SetRGBA(0,0,0, 96);
}

VTreeViewControl::~VTreeViewControl()
{
  m_Items.Clear();
  V_SAFE_DISPOSEOBJECT(m_spVScrollbar);
}

void VTreeViewControl::DisposeObject()
{
  m_Items.Clear();
  V_SAFE_DISPOSEOBJECT(m_spVScrollbar);
  VDlgControlBase::DisposeObject();
}

void VTreeViewControl::SetScrollPosition(float fPos)
{
  if (m_fCurrentScrollPos==fPos) return;
  m_fCurrentScrollPos = fPos;
  FlagForMeasure();
}


VTextLabel *NewTextLabel(const char *szString)
{
  VTextLabel *pLabel = new VTextLabel();
  pLabel->SetText(szString);
  VisFont_cl *pFont = VGUIManager::GlobalManager().GetDefaultFont();
  pLabel->Text().SetFont(pFont);
  VRectanglef textSize;
  pFont->GetTextDimension(szString, textSize);
  pLabel->SetSize(textSize.GetSizeX(),textSize.GetSizeY());
  pLabel->Text().FinishSetup();
  return pLabel;
}

void VTreeViewControl::TestBuild()
{
/*
  const char *szPath = "GUI\\Dialogs";
  m_CollapsedButtonStyle.m_States[VWindowBase::NORMAL].SetTexture(VGUIManager::GlobalManager().LoadTexture("tree_collapsed.png",szPath));
  m_CollapsedButtonStyle.m_States[VWindowBase::MOUSEOVER].SetTexture(VGUIManager::GlobalManager().LoadTexture("tree_collapsed_sel.png",szPath));
  m_CollapsedButtonStyle.m_States[VWindowBase::SELECTED].SetTexture(VGUIManager::GlobalManager().LoadTexture("tree_collapsed_sel.png",szPath));
  m_UnCollapsedButtonStyle.m_States[VWindowBase::NORMAL].SetTexture(VGUIManager::GlobalManager().LoadTexture("tree_uncollapsed.png",szPath));
  m_UnCollapsedButtonStyle.m_States[VWindowBase::MOUSEOVER].SetTexture(VGUIManager::GlobalManager().LoadTexture("tree_uncollapsed_sel.png",szPath));
  m_UnCollapsedButtonStyle.m_States[VWindowBase::SELECTED].SetTexture(VGUIManager::GlobalManager().LoadTexture("tree_uncollapsed_sel.png",szPath));

  char name[128];
  for (int i=0;i<10;i++)
  {
    sprintf(name,"Root %i",i);
    VTreeViewItem *pItem = new VTreeViewItem(this, NewTextLabel(name));
    for (int j=0;j<i;j++)
    {
      sprintf(name,"R%i: Child %i",i,j);
      VTreeViewItem *pChild = new VTreeViewItem(this, NewTextLabel(name));
      pItem->m_Items.Add(pChild);
      for (int k=0;k<3;k++)
      {
        sprintf(name,"R%i: Child %i, %i",i,j,k);
        VTreeViewItem *pChild2 = new VTreeViewItem(this, NewTextLabel(name));
        pChild->m_Items.Add(pChild2);
      }
    }
    m_Items.Add(pItem);
  }
  */
}

bool VTreeViewControl::Build(TiXmlElement *pNode, const char *szPath, bool bWrite)
{
  if (!VDlgControlBase::Build(pNode, szPath, bWrite))
    return false;

  // background color
  XMLHelper::Exchange_Color(XMLHelper::SubNode(pNode,"background",bWrite),"color",m_iBackgroundCol,bWrite);

  // default states for collapsed button:
  m_CollapsedButtonStyle.Build(this, XMLHelper::SubNode(pNode,"button_collapsed",bWrite), szPath, bWrite);
  m_UnCollapsedButtonStyle.Build(this, XMLHelper::SubNode(pNode,"button_uncollapsed",bWrite), szPath, bWrite);

  // load items
  m_Items.Build(XMLHelper::SubNode(pNode,"treeitems",bWrite), szPath, bWrite, this);

  // scroll bars
  TiXmlElement *pScrollV = XMLHelper::SubNode(pNode,"vscrollbar",bWrite);
  if (pScrollV)
  {
    m_spVScrollbar = new VTreeViewSlider(this);
    m_spVScrollbar->SetParent(this);
    m_spVScrollbar->SetVertical(true);
    m_spVScrollbar->Build(pScrollV,szPath,bWrite);
    m_spVScrollbar->OnBuildFinished();
  }

//  TestBuild();


  FlagForMeasure();
  return true;
}


V_IMPLEMENT_SERIAL( VTreeViewControl, VDlgControlBase, 0, &g_VisionEngineModule );
void VTreeViewControl::Serialize( VArchive &ar )
{
  char iLocalVersion = 1;
  VDlgControlBase::Serialize(ar);
  if (ar.IsLoading())
  {
    ar >> iLocalVersion; VASSERT_MSG(iLocalVersion<=1,"Invalid version");

    // base properties
    ar >> m_CollapsedButtonStyle >> m_UnCollapsedButtonStyle >> m_iBackgroundCol;
    ar >> m_fCurrentScrollPos >> m_fIndentChildren >> m_fIndentCollapseButton;

    // scroll bar
    m_spVScrollbar = (VSliderControl *)ar.ReadObject(VTreeViewSlider::GetClassTypeId());

    // items
    m_Items.SerializeX(ar, this);

    FlagForMeasure();
  } else
  {
    ar << iLocalVersion;

    // base properties
    ar << m_CollapsedButtonStyle << m_UnCollapsedButtonStyle << m_iBackgroundCol;
    ar << m_fCurrentScrollPos << m_fIndentChildren << m_fIndentCollapseButton;

    // scroll bar
    ar.WriteObject(m_spVScrollbar);

    // items
    m_Items.SerializeX(ar, this);
  }
}


void VTreeViewControl::OnSizeChanged() 
{
  VDlgControlBase::OnSizeChanged();
  FlagForMeasure();
}


void VTreeViewControl::MeasureItems()
{
  hkvVec2 vItemStart = hkvVec2(0,-m_fCurrentScrollPos);
  hkvVec2 vItemEnd = vItemStart;
  
  m_Items.MeasureItems(this->GetClientRect(), vItemEnd);
  hkvVec2 vItemSize = vItemEnd - vItemStart;

  if (vItemSize.y>m_vSize.y)
  {
    if (m_spVScrollbar==NULL)
    {
      m_spVScrollbar = new VTreeViewSlider(this);
      m_spVScrollbar->OnBuildFinished();
    }
    m_spVScrollbar->SetVisible(true);
    float fRange = vItemSize.y - GetSize().y; 
    VASSERT(fRange>0.f)
    m_spVScrollbar->SetSliderRange(0.f, fRange, 0);
    m_spVScrollbar->SetSliderRelSize(GetSize().y/fRange);
  } else
  {
    if (m_spVScrollbar!=NULL)
      m_spVScrollbar->SetVisible(false);
  }

  m_bNeedsMeasure = false;
}


void VTreeViewControl::OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState)
{
  if (m_bNeedsMeasure)
  {
    MeasureItems();
  }

  VItemRenderInfo thisInfo(parentState,this,1.f);
  VDlgControlBase::OnPaint(Graphics, thisInfo);

  // background:
  if (m_iBackgroundCol.a>0)
  {
    VRectanglef bbox = GetBoundingBox();
    VSimpleRenderState_t state = VGUIManager::DefaultGUIRenderState();
    Graphics.Renderer.DrawSolidQuad(bbox.m_vMin,bbox.m_vMax,m_iBackgroundCol,state);
  }

  VRectanglef clip = GetClientRect();
  if (m_spVScrollbar && m_spVScrollbar->IsVisible())
    clip.m_vMax.x = m_spVScrollbar->GetAbsPosition().x-2.f;

  Graphics.ClippingStack.Push(clip,true);
  m_Items.OnPaint(Graphics, thisInfo);
  Graphics.ClippingStack.Pop();

  // paint scroll bar
  if (m_spVScrollbar && m_spVScrollbar->IsVisible())
    m_spVScrollbar->OnPaint(Graphics,parentState);

}

VWindowBase* VTreeViewControl::TestMouseOver(VGUIUserInfo_t &user, const hkvVec2 &vAbsMouse)
{
  VWindowBase* pHit;

  // priority to scroll bar
  if (m_spVScrollbar!=NULL && m_spVScrollbar->IsVisible())
  {
    pHit = m_spVScrollbar->TestMouseOver(user, vAbsMouse);
    if (pHit!=NULL)
      return pHit;
  }

  // items, including collapse buttons
  pHit = m_Items.TestMouseOver(user, vAbsMouse);
  if (pHit!=NULL)
    return pHit;

  pHit = VDlgControlBase::TestMouseOver(user, vAbsMouse);
  return pHit;
}

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
