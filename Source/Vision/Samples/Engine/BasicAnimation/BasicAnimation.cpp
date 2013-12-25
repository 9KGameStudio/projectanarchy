/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// ***********************************************************************************************
// Simple Animation Sample
// Copyright (C) Havok.com Inc. All rights reserved.
// ***********************************************************************************************
// 
// The Simple Animation Sample has been designed to give a brief introduction to the various
// parts of the animation system.
//
// The SimpleSkeletalAnimatedObject_cl class covers the skeletal animation topic and shows you how
// to start skeletal animations, blend skeletal animation, layer skeletal animations, listen to
// animation events, set the animation time manually and apply forward kinematics to a bone.
//
// The SimpleVertexAnimatedObject_cl class is about vertex animations and shows you how to start
// a vertex animation.
//
// ***********************************************************************************************

#include <Vision/Samples/Engine/BasicAnimation/BasicAnimationPCH.h>
#include <Vision/Samples/Engine/BasicAnimation/BasicAnimation.h>
#include <Vision/Samples/Engine/BasicAnimation/SimpleSkeletalAnimatedObject.h>
#include <Vision/Samples/Engine/Common/Entities/StaticCamera.hpp>

#if !defined (_VISION_MOBILE ) && !defined( HK_ANARCHY )
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ShadowMapping/VShadowMapComponentSpotDirectional.hpp>
#else
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ShadowMapping/VMobileShadowMapComponentSpotDirectional.hpp>
#endif

#include <Vision/Runtime/Framework/VisionApp/VAppImpl.hpp>
#include <Vision/Runtime/Framework/VisionApp/Modules/VHelp.hpp>
#include <Vision/Runtime/Framework/VisionApp/Modules/VLogoOverlay.hpp>

class BasicAnimationApp : public VAppImpl
{
public:
  BasicAnimationApp() 
    : m_spMainDlg(NULL)
  {}
  virtual ~BasicAnimationApp() {}

  virtual void Init() HKV_OVERRIDE
  {
#if defined(_VISION_MOBILE) || defined(_VISION_PSP2) || defined( HK_ANARCHY )
    const char* szSceneFile = "ground_mobile";
#else
    const char* szSceneFile = "ground";
#endif

    VisAppLoadSettings loadSettings(szSceneFile, ":havok_sdk/Data/Vision/Samples/Engine/Maps/SimpleGround");
    loadSettings.m_customSearchPaths.Append(":havok_sdk/Data/Vision/Samples/Engine/Common");
    LoadScene(loadSettings);
  }

  virtual void AfterEngineInit() HKV_OVERRIDE
  {
    VAppImpl::AfterEngineInit();

    InitHelp();

    // Alternative logo placement.
    VLogoOverlay* pLogo = GetAppModule<VLogoOverlay>();
    if (pLogo != NULL)
      pLogo->SetAlignment(VLogoOverlay::ALIGN_TOP);
  }

  virtual void AfterSceneLoaded(bool bLoadingSuccessful) HKV_OVERRIDE;

  virtual void SetupAppConfig(VisAppConfig_cl& config) HKV_OVERRIDE
  {
#if defined(_VISION_PSP2)
    config.m_videoConfig.m_iXRes = 960;
    config.m_videoConfig.m_iYRes = 544;
#endif
  }

  virtual bool Run() HKV_OVERRIDE
  {
    return (m_spMainDlg->GetDialogResult() == 0);
  }

  virtual void DeInit() HKV_OVERRIDE
  {
    m_spMainDlg = NULL;
  }

private:
  void InitHelp();

  VDialogPtr m_spMainDlg;
};

VAPP_IMPLEMENT_SAMPLE(BasicAnimationApp); 

void BasicAnimationApp::AfterSceneLoaded(bool bLoadingSuccessful)
{
  VAppHelper::CreateForwardRenderer();

  const hkvVec3 characterOrigin(-150.0f, 150.f, -20.f);
  const hkvVec3 bubbleOrigin(-150.0f, 150.f, 120.f);
  const hkvVec3 lightPos(200.f, 100.f, 500.f);
  const hkvVec3 lightTarget = characterOrigin + hkvVec3(-150.0f, 100.0f, -20.f);

  // setup dynamic light
  VisLightSource_cl *pLight = new VisLightSource_cl(VIS_LIGHT_SPOTLIGHT, 2000.f);
  pLight->SetPosition(lightPos);
  pLight->SetDirection(lightTarget - lightPos);

#if defined(SUPPORTS_SHADOW_MAPS)

  // create shadow map component
#if defined( _VISION_MOBILE ) || defined( HK_ANARCHY )
  VMobileShadowMapComponentSpotDirectional *pComponent = new VMobileShadowMapComponentSpotDirectional(0);
  pComponent->SetShadowMapSize(1024);
  pComponent->SetNearClip( 100.0f );
  pLight->AddComponent(pComponent);

#elif defined(_VISION_PSP2)
  // No shadows for performance reasons

#else
  VShadowMapComponentSpotDirectional *pComponent = new VShadowMapComponentSpotDirectional(0);
  pComponent->SetShadowMapSize(1024);
  pComponent->SetShadowMappingMode(SHADOW_MAPPING_MODE_PCF8);
  pLight->AddComponent(pComponent);

#endif

#endif

  // Create two entities.
  SimpleSkeletalAnimatedObject_cl *pSkeletalObj = static_cast<SimpleSkeletalAnimatedObject_cl*>(
    Vision::Game.CreateEntity("SimpleSkeletalAnimatedObject_cl", characterOrigin));
  VisBaseEntity_cl *pCharacter = Vision::Game.CreateEntity("SimpleVertexAnimatedObject_cl", bubbleOrigin);

  // Set entity keys so that the GUI can find them.
  pSkeletalObj->SetEntityKey("SkeletalAnimEntity");
  pCharacter->SetEntityKey("VertexAnimEntity");

  // Setup the camera.
  hkvVec3 cameraOrigin(0.f, 0.f, 200.f);
  StaticCamera_cl *pCamera = static_cast<StaticCamera_cl*>(Vision::Game.CreateEntity("StaticCamera_cl", cameraOrigin));
  pCamera->SetTarget( pSkeletalObj );
  hkvVec3 vCamTarget(0.f, 100.f, 50.f);
  pCamera->SetTargetOffset(vCamTarget); // look at the center of the character

  // load some GUI resources
  VGUIManager::GlobalManager().LoadResourceFile("BasicAnimation/MenuSystem.xml");

  // start the main menu
  m_spMainDlg = GetContext()->ShowDialog("BasicAnimation/MainMenu.xml");
  VASSERT(m_spMainDlg != NULL);
}

void BasicAnimationApp::InitHelp()
{
  VArray<const char*> helpText;

#if defined(WIN32) && (!defined(_VISION_WINRT) || defined(_VISION_METRO))
#if defined(SUPPORTS_KEYBOARD)
  helpText.Append("- Select the animation type by clicking on the Skeletal Animation or Vertex Animation labels");
  helpText.Append("- When playing skeletal animations, select the animation mode from the list box below");
  helpText.Append("- Depending on the selected animation type and mode, additional controls may be shown");
#endif
#elif defined (_VISION_XENON) || (defined(_VISION_WINRT) && !defined(_VISION_METRO) && !defined(_VISION_APOLLO) )
  helpText.Append("- Move the cursor with the left thumbstick of Pad1 and click with the A button");
  helpText.Append("- Select the animation type by clicking on the Skeletal Animation or Vertex Animation labels");
  helpText.Append("- When playing skeletal animations, select the animation mode from the list box below");
  helpText.Append("- Depending on the selected animation type and mode, additional controls may be shown");
#elif defined (_VISION_PS3) || defined (_VISION_PSP2)
  helpText.Append("- Move the mouse cursor with the left thumbstick of Pad1 and click with the CROSS button");
  helpText.Append("- Select the animation type by clicking on the Skeletal Animation or Vertex Animation labels");
  helpText.Append("- When playing skeletal animations, select the animation mode from the list box below");
  helpText.Append("- Depending on the selected animation type and mode, additional controls may be shown");
#endif

  if(helpText.GetSize() > 0)
    RegisterAppModule(new VHelp(helpText));
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
