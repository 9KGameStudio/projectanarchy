/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef vCommonControls_h__
#define vCommonControls_h__

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VMenuIncludes.hpp>

class VUISharedData
{
public:
  static void Init();
  static void DeInit();

  static VisFont_cl* GetFont() { return s_spFont; }
  static float GetFontScale() { return s_fFontScale; }

  static float GetScale() { return s_fScale; }
  static float GetDialogSpacing() { return s_fDialogSpacing; }
  static float GetIconSize() { return s_fIconSize; }
  static float GetScrollBarWidth() { return s_fScrollBarWidth; }
  static float GetTabsHeight() { return s_fTabsHeight; }
  static float GetStatusBarHeight() { return s_fStatusBarHeight; }
  static float GetPadding() { return s_fPadding; }
  static float GetOuterBorderWidth() { return s_fOuterBorderWidth; }
  static float GetBorderWidth() { return s_fBorderWidth; }

private:
  static VisFontPtr s_spFont;
  static float s_fFontScale;
  
  static float s_fScale;
  static float s_fDialogSpacing;
  static float s_fIconSize;
  static float s_fScrollBarWidth;
  static float s_fTabsHeight;
  static float s_fStatusBarHeight;
  static float s_fPadding;
  static float s_fOuterBorderWidth;
  static float s_fBorderWidth;
};

#endif // vCommonControls_h__

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
