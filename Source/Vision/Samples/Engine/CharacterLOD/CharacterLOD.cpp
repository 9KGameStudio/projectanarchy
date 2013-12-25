/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// ***********************************************************************************************
// Character LOD Sample
// Copyright (C) Havok.com Inc. All rights reserved.
// ***********************************************************************************************
// This sample shows how to use characters with multiple models at different quality levels (LOD),
// which automatically change according to distance of the camera.
// ***********************************************************************************************

#include <Vision/Samples/Engine/CharacterLOD/CharacterLODPCH.h>

#include <Vision/Runtime/Framework/VisionApp/VAppImpl.hpp>
#include <Vision/Runtime/Framework/VisionApp/Modules/VDebugOptions.hpp>
#include <Vision/Runtime/Framework/VisionApp/Modules/VHelp.hpp>
#include <Vision/Runtime/Framework/VisionApp/Modules/VLogoOverlay.hpp>

#include <Vision/Samples/Engine/CharacterLOD/GUI/MenuSystem.hpp>
#include <Vision/Samples/Engine/Common/Entities/StaticCamera.hpp>

#include <Vision/Runtime/Base/System/IO/Stream/VMemoryStream.hpp>

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VMenuIncludes.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Components/VEntityLODComponent.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Entities/_AnimEntity.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Components/VOrbitCamera.hpp>

enum CharacterLODControl
{
  CAMERA_ENABLE_MOVEMENT = VAPP_INPUT_CONTROL_LAST_ELEMENT + 1
};

class CharacterLODApp : public VAppImpl
{
public:
  CharacterLODApp() 
    : m_spMainDlg(NULL)
    , m_pOrbitCamera(NULL)
  {}
  virtual ~CharacterLODApp() 
  {}

  virtual void Init() HKV_OVERRIDE
  {
#if defined(_VR_GLES2)
    Vision::Renderer.SetUseSingleBufferedStaticMeshes(false);
#endif
#if defined(_VISION_PS3)
    // Ensure that we have enough mapped main memory available (to avoid warnings)
    VAppBase::Get()->m_appConfig.m_gcmConfig.uiMappedMainMemorySize = 10 * 1024 * 1024;
#endif

#if defined(_VISION_MOBILE) || defined(_VISION_PSP2) || defined( HK_ANARCHY )
    const char* szSceneFile = "ground_mobile";
#else
    const char* szSceneFile = "ground";
#endif

    VisAppLoadSettings settings(szSceneFile);
    settings.m_customSearchPaths.Append(":havok_sdk/Data/Vision/Samples/Engine/Maps/SimpleGround");
    settings.m_customSearchPaths.Append(":havok_sdk/Data/Vision/Samples/Engine/Common");

    LoadScene(settings);
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

  virtual bool Run() HKV_OVERRIDE
  {
#if defined(WIN32) && (!defined(_VISION_WINRT) || defined(_VISION_METRO))
    if (GetInputMap()->GetTrigger(CAMERA_ENABLE_MOVEMENT))
      m_pOrbitCamera->FollowFixed = FALSE;
    else
      m_pOrbitCamera->FollowFixed = TRUE;
#else
    m_pOrbitCamera->FollowFixed = FALSE;
#endif

    return (m_spMainDlg->GetDialogResult() == 0);
  }

  virtual void DeInit() HKV_OVERRIDE
  {
    m_spMainDlg = NULL;
  }

private:
  void InitInput();
  void InitHelp();

  VDialogPtr m_spMainDlg;
  VOrbitCamera* m_pOrbitCamera;
};

VAPP_IMPLEMENT_SAMPLE(CharacterLODApp);

void CharacterLODApp::AfterSceneLoaded(bool bLoadingSuccessful)
{
  // Setup dynamic light
  const hkvVec3 vLightPos(0.f, 0.f, 300.f);
  VisLightSource_cl *pLight = Vision::Game.CreateLight(vLightPos, VIS_LIGHT_POINT, 15000.f);

  pLight->SetMultiplier(1.1f);
  pLight->SetPosition(vLightPos);

  // Create the entities.
  VisBaseEntity_cl* pLODObject = NULL;
  VisBaseEntity_cl* pLookAtEntity = NULL;
  for (int i = 0; i < ENTITY_COUNT; i++)
  {
    char szKey[64];

    hkvVec3 origin(
      (static_cast<float>(i % 3) - 1.0f) * 400.f,
      (static_cast<float>(i / 3) - 1.0f) * 400.f, 
      80.f);

    pLODObject = Vision::Game.CreateEntity("VisBaseEntity_cl", origin, "Models/Soldier/soldier_high.MODEL");

    if (i == ENTITY_COUNT / 2)
      pLookAtEntity = pLODObject;

    sprintf(szKey, "Soldier_%i", i);
    pLODObject->SetEntityKey(szKey);

    // Add Animation component first.
    VSimpleAnimationComponent* pAnimationComponent = new VSimpleAnimationComponent();
    pAnimationComponent->AnimationName = "Walk";
    pLODObject->AddComponent(pAnimationComponent);

    // Add LOD component.
    VEntityLODComponent* pLODComponent = new VEntityLODComponent();
    pLODComponent->SetLODLevel(VLOD_AUTO);
    pLODComponent->SetLODLevelCount(3);
    pLODComponent->SetMediumLevelMesh("Models/Soldier/soldier_medium.MODEL");
    pLODComponent->SetMediumLevelDistance(600.0f);
    pLODComponent->SetLowLevelMesh("Models/Soldier/soldier_low.MODEL");
    pLODComponent->SetLowLevelDistance(1200.0f);
    pLODComponent->SetUltraLowLevelMesh("Models/Soldier/soldier_ultralow.MODEL");
    pLODComponent->SetUltraLowLevelDistance(2400.0f);
    pLODObject->AddComponent(pLODComponent);
  }

  // Set up the camera.
  m_pOrbitCamera = new VOrbitCamera();
  m_pOrbitCamera->FollowFixed = TRUE;
  m_pOrbitCamera->Collides = TRUE;
  m_pOrbitCamera->CameraDistance = 650.0f;
  m_pOrbitCamera->MinimalDistance = 50.0f;
  m_pOrbitCamera->MaximalDistance = 3500.0f;
  pLookAtEntity->AddComponent(m_pOrbitCamera);

  // load some GUI resources
  VGUIManager::GlobalManager().LoadResourceFile("CharacterLOD/Dialogs/MenuSystem.xml");

  // start the main menu
  m_spMainDlg = GetContext()->ShowDialog("CharacterLOD/Dialogs/MainMenu.xml");
  VASSERT(m_spMainDlg != NULL);

  // Show fps
  VDebugOptions* pDebugOptionsModule = GetAppModule<VDebugOptions>();
  if (pDebugOptionsModule != NULL)
    pDebugOptionsModule->SetFrameRateVisible(true);

  // deactivate automatic use of @2x resources on high-resolution displays
  bool bAllow2x = Vision::Video.GetAllowAutomaticUseOf2xAssets();
  Vision::Video.SetAllowAutomaticUseOf2xAssets(false);

  // Place the overlay.
  VisScreenMask_cl *pOverlay = new VisScreenMask_cl("Models/Soldier/Textures/rb_cs_logo.dds");
  int iOverlayWidth, iOverlayHeight;
  pOverlay->GetTextureSize(iOverlayWidth, iOverlayHeight);
  pOverlay->SetTransparency(VIS_TRANSP_ALPHA);
  pOverlay->SetFiltering(FALSE);

  // Place at the bottom center.
  const float fScreenWidth = static_cast<float>(Vision::Video.GetXRes());
  const float fScreenHeight = static_cast<float>(Vision::Video.GetYRes());
  pOverlay->SetPos(
    (fScreenWidth - static_cast<float>(iOverlayWidth)) * 0.5f + 0.5f, 
    fScreenHeight - static_cast<float>(iOverlayHeight) - 13.5f);

  Vision::Video.SetAllowAutomaticUseOf2xAssets(bAllow2x);

  InitInput();
}

void CharacterLODApp::InitInput()
{
#if defined(WIN32) && (!defined(_VISION_WINRT) || defined(_VISION_METRO))
  // Only enable camera movement when holding down the right mouse button.
  GetInputMap()->MapTrigger(CAMERA_ENABLE_MOVEMENT, V_PC_MOUSE, CT_MOUSE_RIGHT_BUTTON);
#endif
}

void CharacterLODApp::InitHelp()
{
  VArray<const char*> helpText;

#if defined(WIN32) && (!defined(_VISION_WINRT) || defined(_VISION_METRO))
  helpText.Append("PAGE UP                    : Zoom in");
  helpText.Append("PAGE DOWN                  : Zoom Out");

  helpText.Append("MOUSE - WHEEL              : Zoom in/out");
  helpText.Append("MOUSE - RIGHT MOUSE BUTTON : Rotate scene");
#endif

#if defined (_VISION_XENON) || (defined(_VISION_WINRT) && !defined(_VISION_METRO) && !defined(_VISION_APOLLO))
  helpText.Append(" PAD 1 - LEFT/RIGHT TRIGGER  : Zoom in/out");
  helpText.Append(" PAD 1 - DIGITAL CURSOR-KEYS : Rotate scene");
#elif defined(_VISION_PS3)
  helpText.Append(" PAD 1 - LEFT/RIGHT TRIGGER  : Zoom in/out");
  helpText.Append(" PAD 1 - DIGITAL CURSOR-KEYS : Rotate scene");
#elif defined(_VISION_WIIU)
  helpText.Append(" DRC - LEFT/RIGHT TRIGGER  : Zoom in/out");
  helpText.Append(" DRC - DIGITAL CURSOR-KEYS : Rotate scene");
#endif

  helpText.Append("");
  helpText.Append("Watch the soldiers (+ the frame rate)");
  helpText.Append("and how each model changes according");
  helpText.Append("to the distance of the camera. The");
  helpText.Append("letters above the soldiers indicate");
  helpText.Append("the selected model.");
  helpText.Append("");
  helpText.Append("(You can also switch to manual model");
  helpText.Append("selection via the side menu)");
  helpText.Append("");
  helpText.Append("Models (C) by Rocketbox Studios GmbH");
  helpText.Append("www.rocketbox-libraries.com");

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
