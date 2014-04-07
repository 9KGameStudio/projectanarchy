/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// ***********************************************************************************************
// ModelMerging
// Copyright (C) Havok.com Inc. All rights reserved.
// ***********************************************************************************************
// 
// This demo shows how you can use the model merging feature in Vision to customize a character
// by attaching various parts of an armor to a model and merging them to one single object.
//
// ***********************************************************************************************

#include <Vision/Samples/Engine/ModelMerging/ModelMergingPCH.h>
#include <Vision/Runtime/Framework/VisionApp/VAppImpl.hpp>
#include <Vision/Runtime/Framework/VisionApp/Modules/VHelp.hpp>
#include <Vision/Runtime/Framework/VisionApp/Modules/VLogoOverlay.hpp>

#include <Vision/Runtime/EnginePlugins/EnginePluginsImport.hpp>

#include <Vision/Samples/Engine/ModelMerging/KeyControlledTransitionBarbarian.hpp>
#include <Vision/Samples/Engine/ModelMerging/GUI/MenuSystem.hpp>
#include <Vision/Samples/Engine/ModelMerging/MergedModelFactory.hpp>

enum MERGE_MODEL_CONTROL 
{
  MERGE_MODEL_SHOW_MENU = CHARACTER_CONTROL_LAST_ELEMENT + 1
};

class ModelMergingApp : public VAppImpl
{
public:
  ModelMergingApp() 
    : m_spMainDlg(NULL)
    , m_pMergeModelFactory(NULL)
    , m_bShowGUI(false) 
  {}
  virtual ~ModelMergingApp() 
  {}

  virtual void Init() HKV_OVERRIDE
  {
#if defined(_VISION_MOBILE) || defined( HK_ANARCHY )
    const char* szSceneFile = "ground_mobile.vscene";
#else
    const char* szSceneFile = "ground.vscene";
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

  virtual void PreloadPlugins() HKV_OVERRIDE
  {
    VISION_PLUGIN_ENSURE_LOADED(vHavok);
  }

  virtual void AfterSceneLoaded(bool bLoadingSuccessful) HKV_OVERRIDE
  {
    // Create the model merged model factory
    m_pMergeModelFactory = new MergedModelFactory_cl();
    m_pMergeModelFactory->MergeModel();

    // Load some GUI resources
    VGUIManager::GlobalManager().LoadResourceFile("ModelMerging/GUI/MenuSystem.xml");

    // Start the main menu
    m_spMainDlg = vstatic_cast<MergeModelsMainMenu*>(GetContext()->ShowDialog("ModelMerging/GUI/MainMenu.xml"));
    VASSERT(m_spMainDlg != NULL);
    m_spMainDlg->SetMergeModelFactory(m_pMergeModelFactory);
    m_bShowGUI = true;

#if !defined( _VISION_MOBILE )
    // Deactivate GUI
    GetContext()->CloseDialog(m_spMainDlg);
    m_bShowGUI = false;
#else
    m_pMergeModelFactory->PreviewModel();
#endif

    InitInput();
  }

  virtual void SetupAppConfig(VisAppConfig_cl& config) HKV_OVERRIDE
  {
#if defined(_VISION_PSP2)
    // Use double resolution on PSVita, since otherwise the menu doesn't fit.
    config.m_videoConfig.m_iXRes *= 2;
    config.m_videoConfig.m_iYRes *= 2;
#endif
  }

  virtual bool Run() HKV_OVERRIDE
  {
    // Toggle GUI and camera mode
    if (GetInputMap()->GetTrigger(MERGE_MODEL_SHOW_MENU))
    {
      if (GetContext()->GetOpenDialogs().Contains(m_spMainDlg))
      {
        GetContext()->CloseDialog(m_spMainDlg);
      }
      else
      {
        GetContext()->ShowDialog(m_spMainDlg);
      }
    }

    return true;
  }

  virtual void DeInit() HKV_OVERRIDE
  {
    m_spMainDlg->SetMergeModelFactory(NULL);

    // Deinit GUI
    m_spMainDlg = NULL;

    // Delete Factory
    V_SAFE_DELETE(m_pMergeModelFactory);
  }

private:
  void InitHelp();
  void InitInput();

  VSmartPtr<MergeModelsMainMenu> m_spMainDlg;
  MergedModelFactory_cl* m_pMergeModelFactory;
  bool m_bShowGUI;
};

VAPP_IMPLEMENT_SAMPLE(ModelMergingApp);

void ModelMergingApp::InitHelp()
{
  VArray<const char*> helpText;

#if defined(SUPPORTS_KEYBOARD)
  helpText.Append("KEYBOARD - ARROW UP: Move Forward");
  helpText.Append("KEYBOARD - ARROW UP + SHIFT: Run Forward");
  helpText.Append("KEYBOARD - ARROW LEFT: Rotate Left");
  helpText.Append("KEYBOARD - ARROW RIGHT: Rotate Right");
  helpText.Append("KEYBOARD - SPACE : Toggle the menu");
  helpText.Append("");

#endif

#if defined (_VISION_XENON) || (defined(_VISION_WINRT) && !defined(_VISION_METRO) && !defined(_VISION_APOLLO))
  helpText.Append("PAD1 - Left Thumbstick : Move character");
  helpText.Append("PAD1 - Left Shoulder  : Run");
  helpText.Append("PAD1 - Y               : Toggle the menu");

#elif defined (_VISION_PS3)
  helpText.Append("PAD1 - Left Thumbstick : Move character");
  helpText.Append("PAD1 - Left Shoulder  : Run");
  helpText.Append("PAD1 - TRIANGLE        : Toggle the menu");

#elif defined (_VISION_PSP2)
  helpText.Append("Left Thumbstick : Move character");
  helpText.Append("Left Shoulder   : Run");
  helpText.Append("TRIANGLE        : Toggle the menu");

#elif defined (_VISION_WIIU)
  helpText.Append("DRC - Left Thumbstick : Move character");
  helpText.Append("DRC - Left Shoulder   : Run");
  helpText.Append("DRC - X               : Toggle the menu");

#endif

  RegisterAppModule(new VHelp(helpText));
}

void ModelMergingApp::InitInput()
{
#if defined(SUPPORTS_KEYBOARD)
  GetInputMap()->MapTrigger(MERGE_MODEL_SHOW_MENU, V_KEYBOARD, CT_KB_SPACE, VInputOptions::Once());
#endif

#if defined (_VISION_XENON) || (defined(_VISION_WINRT) && !defined(_VISION_METRO) && !defined(_VISION_APOLLO))
  GetInputMap()->MapTrigger(MERGE_MODEL_SHOW_MENU, V_XENON_PAD(0), CT_PAD_Y, VInputOptions::Once());
#elif defined (_VISION_PS3)
  GetInputMap()->MapTrigger(MERGE_MODEL_SHOW_MENU, V_PS3_PAD(0), CT_PAD_TRIANGLE, VInputOptions::Once());
#elif defined (_VISION_PSP2)
  GetInputMap()->MapTrigger(MERGE_MODEL_SHOW_MENU, V_PSP2_PAD(0), CT_PAD_TRIANGLE, VInputOptions::Once());
#elif defined (_VISION_WIIU)
  GetInputMap()->MapTrigger(MERGE_MODEL_SHOW_MENU, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_X, VInputOptions::Once());
#endif
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
