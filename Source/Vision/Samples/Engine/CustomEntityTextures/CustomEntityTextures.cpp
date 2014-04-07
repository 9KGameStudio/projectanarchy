/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// ***********************************************************************************************
// Custom Entity Textures
// Copyright (C) Havok.com Inc. All rights reserved.
// ***********************************************************************************************
// Shows how to replace textures of single entity instances without modifying the shader assignment
// ***********************************************************************************************

#include <Vision/Samples/Engine/CustomEntityTextures/CustomEntityTexturesPCH.h>
#include <Vision/Runtime/Framework/VisionApp/VAppImpl.hpp>
#include <Vision/Runtime/Framework/VisionApp/Modules/VHelp.hpp>

class CustomEntityTexturesApp : public VAppImpl
{
public:
  CustomEntityTexturesApp() {}
  virtual ~CustomEntityTexturesApp() {}

  virtual void Init() HKV_OVERRIDE
  {
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
  }

  virtual void AfterSceneLoaded(bool bLoadingSuccessful) HKV_OVERRIDE;

private:
  void InitHelp();

  void ReplaceEntityBaseTexture(VisBaseEntity_cl *pEntity, const char *szSurfacename, const char *szNewTexture);
};

VAPP_IMPLEMENT_SAMPLE(CustomEntityTexturesApp);

void CustomEntityTexturesApp::AfterSceneLoaded(bool bLoadingSuccessful)
{
  // create several entities of the same model and animate them
  VisBaseEntity_cl *pEntity[3];

  for (int i = 0; i < 3; i++)
  {
    hkvVec3 vPos(300.0f, (i-1) * 120.0f, 60.0f);
    pEntity[i] = Vision::Game.CreateEntity("VisBaseEntity_cl",vPos,"Models/Soldier/soldier_high.MODEL");
    pEntity[i]->SetOrientation(270.f,0.f,0.f);
    pEntity[i]->SetCastShadows(true);

    if (i==0)
    {
      VisAnimConfig_cl::StartSkeletalAnimation(pEntity[i], "Walk", VSKELANIMCTRL_DEFAULTS|VANIMCTRL_LOOP, 1.f);

      VASSERT_MSG(pEntity[i]->GetAnimConfig() != NULL, "Entity animation was not loaded - missing animation file?");

      // set the HINT_FREQUENT_REUSE flag for the config as we reuse it for 5 entities. This improves performance drastically
      pEntity[i]->GetAnimConfig()->SetFlags(pEntity[i]->GetAnimConfig()->GetFlags() | HINT_FREQUENT_REUSE);
    }
    else // share the animation data
    {
      pEntity[i]->SetAnimConfig(pEntity[0]->GetAnimConfig());
    }
  }

  // texture variation #1
  ReplaceEntityBaseTexture(pEntity[0], "s01_w_us_jg", "Models/Soldier/Textures/s01_w_ru_ur.dds");
  ReplaceEntityBaseTexture(pEntity[0], "h6_us_jg",    "Models/Soldier/Textures/h06_ru_ur.dds"); // helmet

  // texture variation #2
  ReplaceEntityBaseTexture(pEntity[1], "s01_w_us_jg", "Models/Soldier/Textures/s01_w_uk_ds.dds");
  ReplaceEntityBaseTexture(pEntity[1], "h6_us_jg",    "Models/Soldier/Textures/h06_uk_de.dds"); // helmet

  // Create a mouse controlled camera (without gravity)
  VisBaseEntity_cl *pCamera = Vision::Game.CreateEntity("VFreeCamera", hkvVec3::ZeroVector());
  pCamera->IncPosition(0.0f, 0.0f, 100.0f);

  // We need a forward renderer node to cast shadows (will do nothing on platforms that don't support renderer nodes)
  VAppHelper::CreateForwardRenderer();

  hkvVec3 lightPos( 0.f, 0.f, 300.f );
  VisLightSource_cl *pLight = Vision::Game.CreateLight( lightPos, VIS_LIGHT_POINT, 1250.f );
  pLight->SetPosition( lightPos );

#if !defined(_VISION_PSP2)
  VAppHelper::SetShadowsForLight(pLight, true);
#endif

  //place the overlay
  VisScreenMask_cl *pOverlay = new VisScreenMask_cl("Models/Soldier/Textures/rb_cs_logo.dds");

  pOverlay->SetTransparency (VIS_TRANSP_ALPHA);
  pOverlay->SetFiltering(FALSE);
  pOverlay->SetPos(static_cast<float>(Vision::Video.GetXRes()) - 185.5f, 20.5f);   
}

void CustomEntityTexturesApp::InitHelp()
{
  VArray<const char*> helpText;
  helpText.Append("");
  helpText.Append("Models (c) by Rocketbox Studios GmbH");
  helpText.Append("www.rocketbox-libraries.com");

  RegisterAppModule(new VHelp(helpText));
}

// helper function to replace a single base texture of an entity
void CustomEntityTexturesApp::ReplaceEntityBaseTexture(VisBaseEntity_cl *pEntity, const char *szSurfacename, const char *szNewTexture)
{
  VisSurfaceTextureSet_cl *pSet = pEntity->GetCustomTextureSet();
  if (!pSet)
  {
    pSet = pEntity->CreateCustomTextureSet();
    pEntity->SetCustomTextureSet(pSet);
  }

  // replace the diffuse base texture in the array
  int iIndex = pEntity->GetMesh()->GetSurfaceIndexByName(szSurfacename);
  VASSERT(iIndex>=0);
  pSet->GetTextures(iIndex)->m_spDiffuseTexture = Vision::TextureManager.Load2DTexture(szNewTexture);
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
