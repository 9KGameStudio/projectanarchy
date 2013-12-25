/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// ***********************************************************************************************
// Unicode demo
// Copyright (C) Havok.com Inc. All rights reserved.
// ***********************************************************************************************
// This sample shows how to print unicode text in UTF8 encoding. The font data
// has been created using the font generator tool of the SDK. For
// A simple font implementation can also be found in the SimpleFont demo of the SDK.
// ***********************************************************************************************

#include <Vision/Samples/Engine/Unicode/UnicodePCH.h>
#include <Vision/Runtime/Framework/VisionApp/VAppImpl.hpp>
#include <Vision/Runtime/Framework/VisionApp/Modules/VHelp.hpp>

#include <Vision/Samples/Engine/Unicode/WorldLanguages.hpp>

class UnicodeApp : public VAppImpl
{
public:
  UnicodeApp() : m_pLangEnt(NULL) {}
  virtual ~UnicodeApp() {}

  virtual void SetupAppConfig(VisAppConfig_cl& config)
  {
#if defined(_VISION_PSP2)
    config.m_videoConfig.m_iXRes = 960;
    config.m_videoConfig.m_iYRes = 544;
#endif
  }

  virtual void AfterEngineInit() HKV_OVERRIDE;

  virtual void Init() HKV_OVERRIDE
  {
    LoadScene(VisAppLoadSettings("worldmap.vscene", ":havok_sdk/Data/Vision/Samples/Engine/Unicode"));
  }

  virtual void AfterSceneLoaded(bool bLoadingSuccessful) HKV_OVERRIDE
  {
    m_pLangEnt = static_cast<WorldLanguages_cl*>(Vision::Game.CreateEntity("WorldLanguages_cl", hkvVec3 (0,0,0)));
    m_pLangEnt->SetEntityKey("languages");
  }

  virtual void DeInit() HKV_OVERRIDE
  {
    Vision::Game.RemoveEntity(m_pLangEnt);
    m_pLangEnt = NULL;
  }

private:
  WorldLanguages_cl* m_pLangEnt;
};

VAPP_IMPLEMENT_SAMPLE(UnicodeApp);

void UnicodeApp::AfterEngineInit()
{
  VAppImpl::AfterEngineInit();

  VArray<const char*> help;
  help.Append("Switch places to see text in different languages.");
  help.Append("Note: Of course, most of the texts don't mean anything");
  help.Append("really, they are just a string of characters.");
  help.Append("");

#if defined(SUPPORTS_KEYBOARD)
  help.Append("KEYBOARD - SPACE : Switches to the next place");
  help.Append("KEYBOARD - BACKSPACE : Switches to the previous place");
  help.Append("");
#elif defined(SUPPORTS_MULTITOUCH)
  help.Append("SINGLE TOUCH : Switches to the next place");
  help.Append("");
#endif

#if defined (_VISION_XENON) || (defined(_VISION_WINRT) && !defined(_VISION_METRO) && !defined(_VISION_APOLLO))
  help.Append("PAD1 - A : Switches to the next place");
  help.Append("PAD1 - B : Switches to the previous place");
#elif defined (_VISION_PS3)
  help.Append("PAD1 - CROSS : Switches to the next place");
  help.Append("PAD1 - CIRCLE : Switches to the previous place");
#elif defined (_VISION_WIIU)
  help.Append("DRC - B : Switches to the next place");
  help.Append("DRC - A : Switches to the previous place");
#elif defined(_VISION_MOBILE)
  help.Append("RIGHT SCREEN BORDER : Switches to the next place");
  help.Append("LEFT SCREEN BORDER : Switches to the previous place");  
#endif

  RegisterAppModule(new VHelp(help));
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
