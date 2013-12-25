/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// ***********************************************************************************************
// Render-To-Texture
// Copyright (C) Havok.com Inc. All rights reserved.
// ***********************************************************************************************
// This demo shows the "RenderToTexture" feature of the Vision Engine
// It loads a map with two "security camera" entities in it. Security cameras are a typical example for using 
// rendering to a texture.
// The security camera entities each reference a destination texture which is their render target. The destination 
// textures are mapped onto monitors so that the monitors display the security camera's view. The engine
// has a powerful optimization, i.e. that the "render to texture" operation is only performed if the destination
// texture is actually visible!
// Please have a look at the security camera entity code to see how the rendering is performed in detail.
//
// Other examples for "render to texture":
//  - rearview mirror in a racing game
//  - location dependent environment map for a envmap shader (e.g. on a car)
//  - terminal dialog with a person at ground control
// ***********************************************************************************************

#include <Vision/Samples/Engine/RenderToTexture/RenderToTexturePCH.h>
#include <Vision/Runtime/Framework/VisionApp/VAppImpl.hpp>
#include <Vision/Runtime/Framework/VisionApp/Modules/VHelp.hpp>

class RenderToTextureApp : public VAppImpl
{
public:
  RenderToTextureApp() {}
  virtual ~RenderToTextureApp() {}

  virtual void Init() HKV_OVERRIDE
  {
    VisAppLoadSettings settings("Crossing.vscene");
    settings.m_customSearchPaths.Append(":havok_sdk/Data/Vision/Samples/Engine/Common");
    settings.m_customSearchPaths.Append(":havok_sdk/Data/Vision/Samples/Engine/Maps/SciFi");
    LoadScene(settings);
  }

  virtual void AfterSceneLoaded(bool bLoadingSuccessful) HKV_OVERRIDE;

private:
  SecurityCamEntity_cl* m_pSecCam1;
  SecurityCamEntity_cl* m_pSecCam2;
};

VAPP_IMPLEMENT_SAMPLE(RenderToTextureApp);

void RenderToTextureApp::AfterSceneLoaded(bool bLoadingSuccessful)
{
  VArray<const char*> help;
  help.Append("You can relocate the security cameras!");
  help.Append("Move very close to a security camera to pick it");
  help.Append("Drop it where ever you want");
  help.Append("");

#ifdef SUPPORTS_KEYBOARD
  help.Append("KEYBOARD - ENTER : Pick/Drop security camera");
  help.Append("");
#endif

#if defined (_VISION_XENON) || (defined(_VISION_WINRT) && !defined(_VISION_METRO) && !defined(_VISION_APOLLO))
  help.Append("PAD1 - A   : Pick/Drop security camera");
#elif defined (_VISION_PS3)
  help.Append("PAD1 - CROSS : Pick/Drop security camera");
#elif defined (_VISION_PSP2)
  help.Append("PAD - CROSS : Pick/Drop security camera");
#elif defined (_VISION_MOBILE)
  help.Append("Upper right screen corner : Pick/Drop security camera");
#elif defined (_VISION_WIIU)
  help.Append("DRC - B : Pick/Drop security camera");
#endif

  RegisterAppModule(new VHelp(help));

  // Create two security cameras.
  m_pSecCam1 = (SecurityCamEntity_cl*)Vision::Game.CreateEntity("SecurityCamEntity_cl", hkvVec3(-520,300,20), "Models/SecurityCam/securitycam.model");
  m_pSecCam1->SetYawSpeed(0.05f * hkvMath::pi () * 2.0f);
  m_pSecCam1->SetMonitorSurfaceName("MonitorScreen03_Mat");

  m_pSecCam2 = (SecurityCamEntity_cl*)Vision::Game.CreateEntity("SecurityCamEntity_cl", hkvVec3(-720,0,390), "Models/SecurityCam/securitycam.model", "m_bUseThermalImage=TRUE");
  m_pSecCam2->SetYawSpeed(0.07f * hkvMath::pi () * 2.0f);
  m_pSecCam2->SetMonitorSurfaceName("MonitorScreen01_Mat");
  
  // Don't display menus and messages on the security cam screens
  unsigned int renderTargetVisiblityMask = ~(GetContext()->GetVisibleBitmask() | Vision::Message.GetVisibleBitmask());
  m_pSecCam1->SetRenderTargetRenderFilterMask(renderTargetVisiblityMask);
  m_pSecCam2->SetRenderTargetRenderFilterMask(renderTargetVisiblityMask);


  VisBaseEntity_cl *pCam = Vision::Game.CreateEntity("VFreeCamera", hkvVec3::ZeroVector());

  VisBaseEntity_cl *pSpawnPos = Vision::Game.SearchEntity("spawnpos");
  if (pSpawnPos)
  {
    pCam->SetPosition(pSpawnPos->GetPosition());
    Vision::Camera.ReComputeVisibility();
  }
  else
  {
    pCam->SetPosition(150, 0, 200);
    pCam->SetOrientation(180, -15, 0);
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
