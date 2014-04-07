/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Tools/vPlayer/vPlayerPCH.h>
#include <Vision/Tools/vPlayer/GUI/vCommonControls.hpp>

#include <Vision/Runtime/Framework/VisionApp/VAppModule.hpp>

VisFontPtr VUISharedData::s_spFont;
float VUISharedData::s_fFontScale = 0.76f;

float VUISharedData::s_fScale = 1.0f;
float VUISharedData::s_fDialogSpacing = 0.0f;
float VUISharedData::s_fIconSize = 48.0f;
float VUISharedData::s_fScrollBarWidth = 16.0f;
float VUISharedData::s_fTabsHeight = 34.0f;
float VUISharedData::s_fStatusBarHeight = 28.0f;
float VUISharedData::s_fPadding = 4.0f;
float VUISharedData::s_fOuterBorderWidth = 5.0f;
float VUISharedData::s_fBorderWidth = 1.5f;

void VUISharedData::Init()
{
  s_fScale = VAppHelper::GetUIScalingFactor();
  s_fFontScale *= s_fScale;
  
  s_fDialogSpacing *= s_fScale;
  s_fIconSize *= s_fScale;
  s_fScrollBarWidth *= s_fScale;
  s_fTabsHeight *= s_fScale;
  s_fStatusBarHeight *= s_fScale;
  s_fPadding *= s_fScale;
  s_fOuterBorderWidth *= s_fScale;
  s_fBorderWidth = hkvMath::ceil(s_fBorderWidth * s_fScale);

  s_spFont = Vision::Fonts.LoadFont("Fonts/OpenSans_22.fnt");
}

void VUISharedData::DeInit()
{
  s_spFont = NULL;
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
