/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// ***********************************************************************************************
// ScaleformSample : Sample binding for Scaleform middleware
// Copyright (C) Havok.com Inc. All rights reserved.
// ***********************************************************************************************
// This sample shows a binding for the Scaleform middleware to playback flash movies
// or UIs  (.swf files). Key features of the binding are:
//    - Loading .swf files through the Vision filemanager (e.g. loading from ZIP)
//    - Hooking into existing renderloops so no modifications to existing renderloops have to be done
//    - Mouse control support
//    - Support for FSCommand and ExternalInterfaceCall listening
//    - movie rendering can be bound to any render contexts, e.g. monitor textures
//    - movie::Advance function runs multithreaded using the engine's task system
//
// On PC the command line argument '-rtt' shows the render-to-texture functionality. This functionality
// can be activated on other platforms by changing the default value of the 'g_bShowOnTexture' variable
// to true.
// ***********************************************************************************************

#include <Vision/Samples/Engine/Scaleform/ScaleformPCH.h>
#include <Vision/Runtime/Framework/VisionApp/VAppImpl.hpp>
#include <Vision/Runtime/Framework/VisionApp/Modules/VHelp.hpp>

#include <Vision/Runtime/EnginePlugins/EnginePluginsImport.hpp>

#include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/VScaleformMovie.hpp>
#include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/VScaleformVariable.hpp>

#include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/VFmodManager.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Input/VVirtualThumbStick.hpp>

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Input/VFreeCamera.hpp>

// On PC: command line '-rtt', On other platforms set to true: Render Scaleform video into a texture instead of as a main HUD overlay.
bool g_bShowOnTexture = false;

class ScaleformApp : public VAppImpl, public IVisCallbackHandler_cl
{
public:
  ScaleformApp();
  virtual ~ScaleformApp() {}

  void OnHandleCallback(IVisCallbackDataObject_cl *pData) HKV_OVERRIDE;

  virtual void PreloadPlugins() HKV_OVERRIDE
  {
    VISION_PLUGIN_ENSURE_LOADED(vFmodEnginePlugin);
    VISION_PLUGIN_ENSURE_LOADED(vScaleformPlugin);
  }

  virtual void Init() HKV_OVERRIDE;
  virtual void AfterSceneLoaded(bool bLoadingSuccessful) HKV_OVERRIDE;
  virtual bool Run() HKV_OVERRIDE;
  virtual void DeInit() HKV_OVERRIDE;

private:
  void ParseCommandLine();
  VScaleformMovieInstance* LoadMovie(const char* szMovieName);

  VString m_sMovieName;

  VScaleformMovieInstancePtr m_spMovie;
  VScaleformVariable m_mainMenuVar;

  bool m_bMute;
#if defined(_VISION_MOBILE)
  VisScreenMask_cl **m_ppTouchPoints;
#else
  VisScreenMaskPtr m_spMouseScreenMask;
#endif

  VisBaseEntity_cl* pEntity;
  VFreeCamera* pFreeCamera;

  static const int TOGGLE_MOUSE_MODE = 1 + VAPP_INPUT_CONTROL_LAST_ELEMENT;
  static const int TOGGLE_BORDERLESS_FULLSCREEN_MODE = 2 + VAPP_INPUT_CONTROL_LAST_ELEMENT;

  bool m_bPseudoFullScreen;

  int m_iCurrentXRes;
  int m_iCurrentYRes;
};

VAPP_IMPLEMENT_SAMPLE(ScaleformApp);

ScaleformApp::ScaleformApp()
  : m_sMovieName("")
  , m_spMovie(NULL)
  , m_mainMenuVar()
  , m_bMute(false)
#if defined(_VISION_MOBILE)
  , m_ppTouchPoints(NULL)
#else
  , m_spMouseScreenMask(NULL)
#endif
  , pEntity(NULL)
  , pFreeCamera(NULL)
  , m_bPseudoFullScreen(false)
  , m_iCurrentXRes(VVIDEO_DEFAULTWIDTH)
  , m_iCurrentYRes(VVIDEO_DEFAULTHEIGHT)
{
}

// Receive command callbacks and external interface calls.
// Alternatively a class can be derived from VScaleformMovieInstance and override the OnCommand / OnExternalInterfaceCall function
void ScaleformApp::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if (pData->m_pSender == &VOnFSCommand::OnFSCallback)
  { 
    // This call back is marked as deprecated by Scaleform.
    VOnFSCommand *pCommand = static_cast<VOnFSCommand*>(pData);

    // Output to screen and log
    VString sCommand("FS COMMAND: ", 128);
    sCommand += pCommand->m_sCommand + " ARGS:" + pCommand->m_sArgs;
    hkvLog::Info(sCommand.AsChar());
    Vision::Message.Add(1, sCommand);
  }
  else if (pData->m_pSender == &VOnExternalInterfaceCall::OnExternalInterfaceCallback)
  {  
    VOnExternalInterfaceCall *pExternalCall = static_cast<VOnExternalInterfaceCall*>(pData);

    // Handle volume change.
    if (pExternalCall->m_sMethodName == "SetVolume" && pExternalCall->m_uiArgCount==1)
    {
      VFmodManager::GlobalManager().SetVolumeAll(static_cast<float>(pExternalCall->m_pArgs[0].GetNumber()));
    }
    else if (pExternalCall->m_sMethodName == "Mute")
    {
      m_bMute = !m_bMute;
      Vision::Message.Add(1, m_bMute ? "Mute on" : "Mute off");
      VFmodManager::GlobalManager().SetMuteAll(m_bMute);
    }
    else
    {
      // Output to screen and log.
      VString sCommand("EXTERNAL CALL: ", 128);
      sCommand += pExternalCall->m_sMethodName + "(";

      for (int i = 0; i < pExternalCall->m_uiArgCount; i++)
      {
        if(i != 0) sCommand += ", ";
        sCommand += pExternalCall->m_pArgs[i].ToString();
      }
      sCommand += ')';

      hkvLog::Info(sCommand);
      Vision::Message.Add(1, sCommand);
    }
  }
}

void ScaleformApp::Init()
{
  //the default movie
#if defined(_VISION_MOBILE)
  m_sMovieName = "as3_havok_multitouch.swf";
#else
  m_sMovieName = "as2_havok.swf";
#endif

  ParseCommandLine();

  if (g_bShowOnTexture)
  {
    m_sMovieName = "GFx_GDC_06.swf";
  }

  // Simple scene when rendering as a HUD overlay
  VisAppLoadSettings settings("ViewerMap.vscene");
  settings.m_customSearchPaths.Append(":havok_sdk/Data/Vision/Samples/Engine/Scaleform");
  settings.m_customSearchPaths.Append(":havok_sdk/Data/Vision/Samples/Engine/Maps/ViewerMap");
  LoadScene(settings);
}

void ScaleformApp::AfterSceneLoaded(bool bLoadingSuccessful)
{
  // register a listener to the command callback (optional)
  VOnFSCommand::OnFSCallback += this;
  VOnExternalInterfaceCall::OnExternalInterfaceCallback += this;

  pFreeCamera = vstatic_cast<VFreeCamera*>(Vision::Game.CreateEntity("VFreeCamera", hkvVec3::ZeroVector()));

#if defined(SUPPORTS_MULTITOUCH)
  if (pFreeCamera->GetVirtualThumbStick() != NULL)
    pFreeCamera->GetVirtualThumbStick()->Hide();
#endif

#if defined(_VISION_MOBILE)
  if (!g_bShowOnTexture)
  {
    int iMaxTouchPoints = VScaleformManager::GlobalManager().GetMaxTouchPoints();
    m_ppTouchPoints = new VisScreenMask_cl*[iMaxTouchPoints];

    for(int i = 0; i<iMaxTouchPoints; i++)
    {
      m_ppTouchPoints[i] = new VisScreenMask_cl("GUI/circle.dds");
      VSimpleRenderState_t renderState(VIS_TRANSP_ALPHA);
      renderState.SetFlags(RENDERSTATEFLAG_FRONTFACE|RENDERSTATEFLAG_FILTERING);
      m_ppTouchPoints[i]->SetRenderState(renderState);
      m_ppTouchPoints[i]->SetVisible(false);
      m_ppTouchPoints[i]->SetPos(0, 0);
      m_ppTouchPoints[i]->SetColor(VColorRef(255/((i%4)+1), (25*i)%255, 255/(i/3+1))); //randomize colors
    
      //rescale the texture a little bit, so that we get some smoother edges through the filtering
      int iWidth, iHeight;
      m_ppTouchPoints[i]->GetTextureSize(iWidth, iHeight);
      m_ppTouchPoints[i]->SetTargetSize(iWidth-2, iHeight-2);
    }
  }
#else
  // display our own mouse cursor (optional)
  m_spMouseScreenMask = new VisScreenMask_cl("mouseNorm.TGA");
  m_spMouseScreenMask->SetTransparency(VIS_TRANSP_ALPHA);
  m_spMouseScreenMask->SetVisible(true);
#endif

  pFreeCamera->SetThinkFunctionStatus(false);

  /*
  //Sample code for associating a subset of an input map with scaleform's key events:
  VInputMap *pInputMap = GetInputMap();
  const int iTriggerID[] = {FOV_INCREASE,FOV_DECREASE};
  const int iGFxKey[] = {VScaleformManager::SF_KEY_UP, VScaleformManager::SF_KEY_DOWN};
  const int iTriggerCount = sizeof(iTriggerID)/sizeof(iTriggerID[0]); // 2 elements
  // FOV_INCREASE/DECREASE (KEYPAD +/-) will be mapped to VScaleformManager::SF_KEY_UP/DOWN (GFxKey::Up and GFxKey::Down)
  VScaleformManager::GlobalManager().SetKeyMapping(pInputMap, iTriggerCount, iTriggerID, iGFxKey);
  */

  //optional code for rendering the GUI into a texture that is used as a monitor texture:
  if (g_bShowOnTexture)
  {
    // apply to simple rotating entity
    pEntity = Vision::Game.CreateEntity("VisBaseEntity_cl",hkvVec3(30.f,0,-10.f), "Models/MissingModel.model");
    pEntity->SetScaling(1.6f);
    VDynamicMesh *pMesh = pEntity->GetMesh();

    VisSurface_cl* pSurface = pMesh->GetSurfaceByName("blinn1");
    VASSERT(pSurface);
    
    pSurface->m_spDiffuseTexture = Vision::TextureManager.Load2DTexture(m_sMovieName);
  }
  else
  {
    // create a movie instance to playback
    m_spMovie = LoadMovie(m_sMovieName.AsChar());
    m_mainMenuVar = m_spMovie->GetVariable("_root.mainMenu");
  }
  
  VArray<const char*> help;
#if defined(SUPPORTS_KEYBOARD)
  GetInputMap()->MapTrigger(TOGGLE_MOUSE_MODE,                 V_KEYBOARD, CT_KB_ENTER, VInputOptions::Once());
  GetInputMap()->MapTrigger(TOGGLE_BORDERLESS_FULLSCREEN_MODE, V_KEYBOARD, CT_KB_PGDN,  VInputOptions::Once());
  GetInputMap()->MapTrigger(TOGGLE_BORDERLESS_FULLSCREEN_MODE, V_KEYBOARD, CT_KB_PGUP,  VInputOptions::Once());
  help.Append("Enter - Toggle Mouse Cursor / Mouse Look");
# if defined(WIN32)
  help.Append("Page Up/Down - FullScreen/Window mode");
  help.Append("");
  help.Append("Try to start the sample with command line parameter -rtt");
# endif
#endif

#if defined(_VISION_PS3) || defined(_VISION_XENON)
  GetInputMap()->MapTrigger(TOGGLE_MOUSE_MODE, VInputManager::GetPad(0), CT_PAD_LEFT_SHOULDER, VInputOptions::Once());
  help.Append("PAD1 - Left Trigger : Show/Hide Mouse Cursor");
#endif

  if(help.GetSize() > 0)
    RegisterAppModule(new VHelp(help));
}

bool ScaleformApp::Run()
{
#if defined(_VISION_PS3) || defined(_VISION_XENON)
  Vision::Message.Print(1, 10, 20, "Press \"PAD1 - Left Shoulder\" to toggle camera / mouse cursor control");
#endif

  if (pEntity)
  {
    pEntity->IncOrientation(Vision::GetTimer()->GetTimeDifference()*10.f,0,0);
  }

#if !defined(_VISION_MOBILE)
  if (GetInputMap()->GetTrigger(TOGGLE_MOUSE_MODE))
  {
    m_spMouseScreenMask->SetVisible(! m_spMouseScreenMask->IsVisible() );
    bool bMouseInput = m_spMouseScreenMask->IsVisible()==TRUE;
    VScaleformManager::GlobalManager().SetHandleCursorInput(bMouseInput);
    pFreeCamera->SetThinkFunctionStatus(!bMouseInput);
  }
#endif

#if defined(WIN32)
  if (GetInputMap()->GetTrigger(TOGGLE_BORDERLESS_FULLSCREEN_MODE))
  {
    if(m_bPseudoFullScreen)
    {
      //Gets the default resolution
      m_iCurrentXRes = VVIDEO_DEFAULTWIDTH;
      m_iCurrentYRes = VVIDEO_DEFAULTHEIGHT;
      m_bPseudoFullScreen = false;
    }
    else
    {
      //Gets the full screen resolution, note that we'll change resolution only.(It's not the real fullscreen mode)
      DEVMODEA deviceMode;
      deviceMode = Vision::Video.GetAdapterMode(VAppBase::Get()->m_appConfig.m_videoConfig.m_iAdapter);
      m_iCurrentXRes = deviceMode.dmPelsWidth;
      m_iCurrentYRes = deviceMode.dmPelsHeight;
      m_bPseudoFullScreen = true;
    }

    if(!g_bShowOnTexture)
    {
      int iOrigX=0, iOrigY=0;
      m_spMovie->GetAuthoredSize(iOrigX,iOrigY);
      VASSERT(iOrigX!=0&&iOrigY!=0);
      float fRatio = (float)iOrigX/(float)iOrigY;

      if(m_iCurrentXRes/fRatio>m_iCurrentYRes)
      {
        if(m_sMovieName=="havok.swf")
        { 
          //do not "over scale" for the havok movie, otherwise
          //controls are outside the reachable area
          m_spMovie->SetPosition((int)((m_iCurrentXRes-m_iCurrentYRes*fRatio)*0.5f),0);
          m_spMovie->SetSize((int)(m_iCurrentYRes*fRatio), m_iCurrentYRes);
        }
        else
        {
          //in this case the window is wider than the move
          //(avoid border by scaling the movie up)
          m_spMovie->SetPosition(0,(int)(((float)m_iCurrentXRes/fRatio-m_iCurrentYRes)*-0.5f));
          m_spMovie->SetSize(m_iCurrentXRes, (int)(m_iCurrentXRes/fRatio));
        }
      }
      else
      {
        m_spMovie->SetPosition(0,0);
        m_spMovie->SetSize(m_iCurrentXRes, m_iCurrentYRes);
      }
    }

    VisionWindowManager::GetActiveWindow()->GetConfig().SetBorderlessWindow(m_bPseudoFullScreen);       
    Vision::Video.ChangeScreenResolution(0,0, m_iCurrentXRes, m_iCurrentYRes, false, 0);
  }
#endif

#ifdef _VISION_MOBILE
  if (!g_bShowOnTexture)
  {
    for(int i=0;i<VScaleformManager::GlobalManager().GetMaxTouchPoints();i++)
    {
      float fX, fY;
      int iWidth, iHeight;
      int iTextX, iTextY;

      VScaleformManager::GlobalManager().GetCursorPos(fX,fY,i);
      m_ppTouchPoints[i]->GetTextureSize(iWidth, iHeight);

      if(VInputManager::GetTouchScreen().IsActiveTouch(i))
      {
        m_ppTouchPoints[i]->SetVisible(true);
        m_ppTouchPoints[i]->SetPos(fX-iWidth/2, fY-iHeight/2); //update the position when active
        iTextX = fX-iWidth*0.65f;
        iTextY = fY-iHeight*0.65f; //display the number outside for active touch points
      }
      else
      {
        iTextX = fX-iWidth/12;
        iTextY = fY-iHeight/8; //display the number inside for in-active touch points
      }

      Vision::Message.SetTextColor(V_RGBA_BLACK);
      Vision::Message.Print(1, iTextX+1, iTextY+1, "%d", i); 
      Vision::Message.SetTextColor(V_RGBA_WHITE);
      Vision::Message.Print(1, iTextX, iTextY, "%d", i);
    }
  }
#else
  if(!pFreeCamera->GetThinkFunctionStatus())
  {
    float x,y;
    VScaleformManager::GlobalManager().GetCursorPos(x,y);
    m_spMouseScreenMask->SetPos(x,y);

    if (!m_mainMenuVar.IsUndefined())
    {
      m_mainMenuVar.Display_SetXYRotation((m_iCurrentYRes*0.5f-y)*-0.07f, (m_iCurrentXRes*0.5f-x)*0.07f);
    }
  }
#endif

  return true;
}

void ScaleformApp::DeInit()
{
  // de-register
  VOnFSCommand::OnFSCallback -= this;
  VOnExternalInterfaceCall::OnExternalInterfaceCallback -= this;

#ifdef _VISION_MOBILE
  V_SAFE_DELETE_ARRAY(m_ppTouchPoints);
#else
  m_spMouseScreenMask = NULL; 
#endif

  m_spMovie = NULL; 
}

VScaleformMovieInstance* ScaleformApp::LoadMovie(const char* szMovieName)
{
  VScaleformMovieInstance* pMovie = VScaleformManager::GlobalManager().LoadMovie(szMovieName);
  if (pMovie==NULL)
  {
    hkvLog::FatalError("Could not load movie: %s", szMovieName);
  }
  return pMovie;
}

void ScaleformApp::ParseCommandLine()
{
#if defined(WIN32)
  VMemoryTempBuffer<256> tmpString = GetCommandLine();
  if (!VStringUtil::IsEmpty(tmpString.AsChar()))
  {
    // works not for spaces in the path
    VStringTokenizerInPlace tokenizer(tmpString.AsChar(), ' ', true);
    for (unsigned int i = 0; i < tokenizer.GetTokenCount(); i++)
    {
      VString sArgument = tokenizer.Next();
      if(!sArgument.IsEmpty())
      {
        if (sArgument == "-rtt")
        {
          g_bShowOnTexture = true;
        }
        else if (sArgument.GetLen()>4 && (sArgument.EndsWith(".gfx")||sArgument.EndsWith(".swf")) )
        {
          m_sMovieName = sArgument;
        }
      }
    }
  }
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
