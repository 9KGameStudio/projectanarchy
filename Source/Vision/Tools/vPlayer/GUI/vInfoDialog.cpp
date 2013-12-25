/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Tools/vPlayer/vPlayerPCH.h>
#include <Vision/Tools/vPlayer/GUI/vCommonControls.hpp>
#include <Vision/Tools/vPlayer/GUI/vInfoDialog.hpp>

#include <Vision/Runtime/Framework/VisionApp/VAppModule.hpp>

VInfoDialog::VInfoDialog()
{
  SetPosition(0.0f, 0.0f);
  SetSize(100.0f, 100.0f);
  m_iBackColor = VAppMenuColors::GetColor(VAppMenuColors::COLOR_DIALOG_BG);

  m_pTextLabel = new VTextLabel();
  m_pTextLabel->Text().SetFont(VUISharedData::GetFont());
  m_pTextLabel->Text().m_States[VWindowBase::NORMAL].SetColor(VAppMenuColors::GetColor(VAppMenuColors::COLOR_LIST_ITEM_TEXT_NORMAL));
  m_pTextLabel->Text().m_States[VWindowBase::MOUSEOVER].SetColor(VAppMenuColors::GetColor(VAppMenuColors::COLOR_LIST_ITEM_TEXT_NORMAL));
  m_pTextLabel->Text().m_States[VWindowBase::SELECTED].SetColor(VAppMenuColors::GetColor(VAppMenuColors::COLOR_LIST_ITEM_TEXT_NORMAL));
  m_pTextLabel->Text().SetScaling(0.72f * VUISharedData::GetScale());
  m_pTextLabel->Text().SetTextWrap(true);
  m_pTextLabel->Text().SetVerticalAlignment(VisFont_cl::ALIGN_CENTER);
  m_pTextLabel->Text().FinishSetup();
  AddControl(m_pTextLabel);

  m_pOKButton = new VStyledButton("OK", true, VUISharedData::GetFont(), VUISharedData::GetScale(), VUISharedData::GetBorderWidth(), VUISharedData::GetFontScale());
  AddControl(m_pOKButton);

  m_pCancelButton = new VStyledButton("Cancel", true, VUISharedData::GetFont(), VUISharedData::GetScale(), VUISharedData::GetBorderWidth(), VUISharedData::GetFontScale());
  AddControl(m_pCancelButton);
}

VInfoDialog::~VInfoDialog()
{

}

void VInfoDialog::OnItemClicked(VMenuEventDataObject* pEvent)
{
  if (pEvent->m_pItem == m_pOKButton)
  {
    m_OnOK.TriggerCallbacks();
  }
  else if (pEvent->m_pItem == m_pCancelButton)
  {
    m_OnCancel.TriggerCallbacks();
  }
}

void VInfoDialog::SetText(const char* szText)
{
  m_pTextLabel->SetText(szText);

  DoLayout();
}

void VInfoDialog::SetButtons(Buttons buttons)
{
  if (buttons == OK)
  {
    m_pOKButton->SetVisible(true);
    m_pCancelButton->SetVisible(false);
  }
  else if (buttons == OK_CANCEL)
  {
    m_pOKButton->SetVisible(true);
    m_pCancelButton->SetVisible(true);
  }

  DoLayout();
}

//////////////////////////////////////////////////////////////////////////

void VInfoDialog::DoLayout()
{
  const float fResX = (float)Vision::Video.GetXRes();
  const float fResY = (float)Vision::Video.GetYRes();
  const float fOuterBorderWidth = VUISharedData::GetOuterBorderWidth();
  const hkvVec2 buttonSize = hkvVec2(fResX * 0.25f - fOuterBorderWidth * 1.5f, VUISharedData::GetIconSize());

  {
    VRectanglef textDimensions;
    m_pTextLabel->Text().GetFont()->GetTextDimension(m_pTextLabel->GetText(), textDimensions);

    float fTextArea = textDimensions.GetArea() * m_pTextLabel->Text().GetScaling() * m_pTextLabel->Text().GetScaling();

    // Estimate suitable text width based on text area
    float fTextWidth = hkvMath::clamp(1.4f * hkvMath::sqrt(fTextArea), fResX * 0.5f, fResX * 0.8f);

    // Set initial dialog size estimate
    SetSize(fTextWidth, fTextWidth);

    // Paint once without graphics to compute actual text size
    VTextState &state(m_pTextLabel->Text().m_States[m_pTextLabel->GetCurrentState()]);
    state.Paint(NULL, this, V_RGBA_WHITE);
    
    float fTextHeight = state.GetHeight() * m_pTextLabel->Text().GetScaling();

    // Reinvalidate computed line wrapping etc.
    state.UpdateAlignment();

    // Set final size
    float fDialogWidth = fTextWidth + fOuterBorderWidth * 2;
    float fDialogHeight = hkvMath::Min(fTextHeight + buttonSize.y + fOuterBorderWidth * 3, fResY * 0.8f);
    SetSize(fDialogWidth, fDialogHeight);
  }

  const hkvVec2 dialogSize = GetSize();

  SetPosition((fResX - dialogSize.x) * 0.5f, (fResY - dialogSize.y) * 0.5f);

  m_pTextLabel->SetSize(dialogSize.x - fOuterBorderWidth * 2, dialogSize.y - buttonSize.y - fOuterBorderWidth * 3);
  m_pTextLabel->SetPosition(fOuterBorderWidth, fOuterBorderWidth);

  const float fOkButtonWidth = m_pCancelButton->IsVisible() ? buttonSize.x : dialogSize.x - fOuterBorderWidth * 2;
  m_pOKButton->SetSize(fOkButtonWidth, buttonSize.y);
  m_pOKButton->SetPosition(fOuterBorderWidth, dialogSize.y - buttonSize.y - fOuterBorderWidth);

  m_pCancelButton->SetSize(buttonSize.x, buttonSize.y);
  m_pCancelButton->SetPosition(fOuterBorderWidth * 2 + buttonSize.x, dialogSize.y - buttonSize.y - fOuterBorderWidth);
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
