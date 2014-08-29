/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/ScaleformEnginePlugin.hpp>

// GFx includes
#include "GFx/GFx_Player.h"
#include "GFx/GFx_Loader.h"
#include "GFx/GFx_Log.h"

#if defined(_VISION_WIN32) && defined(_VR_DX9)
  #include "GFx_Renderer_D3D9.h"
  #include "GFx_FontProvider_Win32.h"
#elif defined(_VISION_WIN32) && defined(_VR_DX11)
  #include "GFx_Renderer_D3D1x.h"
  #include "GFx_FontProvider_Win32.h"
#elif defined (_VISION_XENON)
  #include "Render/X360/X360_HAL.h"
  #include "xtl.h"
#elif defined (_VISION_PS3)
  #include "GFx_Renderer_PS3.h"
#elif defined (_VISION_MOBILE)
  #include "GFx_Renderer_GL.h"
#endif

#ifdef SF_AMP_SERVER
  #include "GFx_AMP.h"
#endif

#include <Vision/Runtime/Engine/Renderer/RenderLoop/VisionRenderLoop.hpp>

#include <Apps/Samples/Common/FontConfigParser.h>

#include "Render/ImageFiles/PNG_ImageFile.h"
#include "Render/ImageFiles/DDS_ImageFile.h"

#include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/VFmodManager.hpp>
#include "Sound/Sound_SoundRendererFMOD.h"

#include "GFx/AS3/AS3_Global.h"

#include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/vScaleformAlloc.hpp>
#include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/vScaleformManager.hpp>
#include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/VScaleformMovie.hpp>
#include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/VScaleformVariableManager.hpp>
#include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/VScaleformInternal.hpp>
#include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/VScaleformTexture.hpp>
#include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformUtils/vScaleformUtils.hpp>

// IME libs
#if defined(WIN32) && !defined(_VISION_WINRT) && defined(USE_SF_IME)
#pragma comment(lib, "libgfx_ime.lib")
#pragma comment(lib, "imm32.lib")
#pragma comment(lib, "uuid.lib")
#endif

//this is just the startup behavior, please use method void EnableMultithreadedAdvance(bool bEnable)
#define MULTITHREADED_ADVANCE true

//we do not use namespace Scaleform since it contains ambiguous symbols like Ptr, Value, etc...
using namespace Scaleform::Render;
using namespace Scaleform::GFx;

// Set maximum number of touch points.
#if defined(_VISION_IOS)
  const int VScaleformManager::s_iMaxTouchPoints = hkvMath::Min(SF_MAX_CURSOR_NUM, V_IOS_MAX_TOUCH_POINTS);
#elif defined(_VISION_ANDROID)
  const int VScaleformManager::s_iMaxTouchPoints = hkvMath::Min(SF_MAX_CURSOR_NUM, V_ANDROID_MAX_TOUCH_POINTS);
#elif defined(_VISION_TIZEN)
  const int VScaleformManager::s_iMaxTouchPoints = hkvMath::Min(SF_MAX_CURSOR_NUM, V_TIZEN_MAX_TOUCH_POINTS);
#endif

#if defined(_VISION_ANDROID)
  // Custom implementation of GL::HAL, that separates DeInit and ReInit for the ResetContext() function.
  // This is a workaround for Scaleform 4.3.26, and should not be necessary anymore for higher versions, which
  // will include the split versions of ResetContext();
  class VisionGLHAL : public GL::HAL
  {
  public:
    VisionGLHAL(ThreadCommandQueue* commandQueue)
      : HAL(commandQueue)
    {}

    void PrepareResetContext()
    {
      notifyHandlers(HAL_PrepareForReset);
      pTextureManager->NotifyLostContext();
      Cache.Reset(true);
      SManager.Reset();
      ShaderData.ResetContext();
    }

    bool RestoreAfterResetContext()
    {
#ifdef SF_GL_RUNTIME_LINK
      Extensions::Init();
#endif

      pTextureManager->Initialize(this);

      if (!SManager.Initialize(this, VMCFlags))
        return false;

      if (!Cache.Initialize(this))
        return false;

      if (pRenderBufferManager)
        pRenderBufferManager->Reset();

      notifyHandlers(HAL_RestoreAfterReset);
      return true;
    }
  };
#elif defined(_VISION_MOBILE)
  typedef GL::HAL VisionGLHAL;
#endif

/// \brief
///   Local implementation of the loader class, to enforce proper memory deallocation
///
/// On PS3 release build the Loader class does not get destroyed with the global delete operation, which
/// routes to VBaseDealloc. We thus create a local wrapper class to ensure that VBaseDealloc is used.
/// See ticket 4417.
class VLocalLoader : public Scaleform::GFx::Loader
{
public:
  VLocalLoader() : Loader() {}
  virtual ~VLocalLoader() {}
};

#ifdef PROFILING
  //profiling
  int PROFILING_ADVANCE        = 0; 
  int PROFILING_RENDER         = 0;
  int PROFILING_FSCOMMAND      = 0;
  int PROFILING_EXT_INTERFACE  = 0;
#endif

enum SF_CURSOR_CONTROLS
{
  CURSOR_0_CLICK_LEFT,
  CURSOR_0_CLICK_RIGHT,
  CURSOR_0_CLICK_MIDDLE,
  CURSOR_0_SCROLL,
  CURSOR_0_DELTA_X,
  CURSOR_0_DELTA_Y,

  SF_CONTROLS_LENGTH = CURSOR_0_DELTA_Y + 1
};

enum SF_BUTTON_TOUCH_MASK
{
  MOUSEB_LEFT = 1,
  MOUSEB_MIDDLE = 2,
  MOUSEB_RIGHT = 4,
  MOUSEB_MASK = 7,
  
  TOUCH_STARTED = 8,
  TOUCH_DRAG = 16,
  TOUCH_MASK = 24
};

const int VScaleformManager::SF_KEY_BACKSPACE = Scaleform::Key::Backspace;
const int VScaleformManager::SF_KEY_TAB = Scaleform::Key::Tab;
const int VScaleformManager::SF_KEY_CLEAR = Scaleform::Key::Clear;
const int VScaleformManager::SF_KEY_RETURN = Scaleform::Key::Return;
const int VScaleformManager::SF_KEY_SHIFT = Scaleform::Key::Shift;
const int VScaleformManager::SF_KEY_CONTROL = Scaleform::Key::Control;
const int VScaleformManager::SF_KEY_ALT = Scaleform::Key::Alt;
const int VScaleformManager::SF_KEY_PAUSE = Scaleform::Key::Pause;
const int VScaleformManager::SF_KEY_CAPSLOCK = Scaleform::Key::CapsLock;
const int VScaleformManager::SF_KEY_ESCAPE = Scaleform::Key::Escape;
const int VScaleformManager::SF_KEY_SPACE = Scaleform::Key::Space;
const int VScaleformManager::SF_KEY_PAGEUP = Scaleform::Key::PageUp;
const int VScaleformManager::SF_KEY_PAGEDOWN = Scaleform::Key::PageDown;
const int VScaleformManager::SF_KEY_END = Scaleform::Key::End;
const int VScaleformManager::SF_KEY_HOME = Scaleform::Key::Home;
const int VScaleformManager::SF_KEY_LEFT = Scaleform::Key::Left;
const int VScaleformManager::SF_KEY_UP = Scaleform::Key::Up;
const int VScaleformManager::SF_KEY_RIGHT =  Scaleform::Key::Right;
const int VScaleformManager::SF_KEY_DOWN = Scaleform::Key::Down;
const int VScaleformManager::SF_KEY_INSERT = Scaleform::Key::Insert;
const int VScaleformManager::SF_KEY_DELETE = Scaleform::Key::Delete;
const int VScaleformManager::SF_KEY_HELP = Scaleform::Key::Help;
const int VScaleformManager::SF_KEY_NUMLOCK = Scaleform::Key::NumLock;
const int VScaleformManager::SF_KEY_SCROLLLOCK = Scaleform::Key::ScrollLock;

const int VScaleformManager::SF_KEY_SEMICOLON = Scaleform::Key::Semicolon;
const int VScaleformManager::SF_KEY_EQUAL = Scaleform::Key::Equal;
const int VScaleformManager::SF_KEY_COMMA = Scaleform::Key::Comma;
const int VScaleformManager::SF_KEY_MINUS = Scaleform::Key::Minus;
const int VScaleformManager::SF_KEY_PERIOD = Scaleform::Key::Period;
const int VScaleformManager::SF_KEY_SLASH = Scaleform::Key::Slash;
const int VScaleformManager::SF_KEY_BAR = Scaleform::Key::Bar;
const int VScaleformManager::SF_KEY_BRACKETLEFT = Scaleform::Key::BracketLeft;
const int VScaleformManager::SF_KEY_BACKSLASH = Scaleform::Key::Backslash;
const int VScaleformManager::SF_KEY_BRACKETRIGHT = Scaleform::Key::BracketRight;
const int VScaleformManager::SF_KEY_QUOTE = Scaleform::Key::Quote;
const int VScaleformManager::SF_KEY_OEM_AX = Scaleform::Key::OEM_AX;   //  'AX' key on Japanese AX kbd
const int VScaleformManager::SF_KEY_OEM_102 = Scaleform::Key::OEM_102;  //  "<>" or "\|" on RT 102-key kbd.
const int VScaleformManager::SF_KEY_ICO_HELP = Scaleform::Key::ICO_HELP; //  Help key on ICO
const int VScaleformManager::SF_KEY_ICO_00 = Scaleform::Key::ICO_00;	 //  00 key on ICO
// global instance of one particle group
VScaleformManager VScaleformManager::s_globalInstance;

#if defined (_VISION_PS3) || defined(_VISION_WII)
VScaleformMemoryManager g_MemManager;
#endif

void VScaleformManager::EnableMultithreadedAdvance(bool bEnable)
{
  VASSERT_MSG(!m_bInitialized || m_Instances.Count() == 0, 
    "EnableMultithreadedAdvance has no effect! Call it before the first movie is loaded!")

  if(!m_bInitialized)
    m_bMultithreadedAdvance = bEnable;
}

bool VScaleformManager::IsMultithreadedAdvanceEnabled()
{
  return m_bMultithreadedAdvance;
}

void VScaleformManager::WaitForAllTasks()
{
  int iCount = m_Instances.Count();
  VScaleformMovieInstance ** ppInstances = m_Instances.GetPtrs();

  for (int i = 0; i < iCount; i++)
  {
    ppInstances[i]->m_pAdvanceTask->WaitUntilFinished();
  }
}

VScaleformManager::VScaleformManager()
  : m_bInitialized(false)
  , m_Instances()
  , m_pLoader(NULL)
  , m_pRenderHal(NULL)
  , m_pRenderer(NULL)
  , m_RenderOrder(SCALEFORM_RENDERHOOK_ORDERCONST)
#if defined (_VR_DX9) || defined (_VISION_XENON) 
  , m_pd3dDevice(NULL)
#elif defined(_VR_DX11)
  , m_pd3dDevice(NULL)
  , m_pd3dContext(NULL)
#endif
  , m_pLastUsedColorRenderTarget(NULL)
  , m_pLastUsedDepthStencilTarget(NULL)
#if defined(_VISION_MOBILE)
  , m_iPrimaryTouchPoint(0)
  , m_bSimulateMouseEvents(true)
#endif
  , m_pCommandQueue(NULL)
  , m_pCursorInputMap(NULL)
#if defined(_VISION_WIN32)
  , m_pFontConfigSet(NULL)
#endif
  , m_bHandleInput(true)
  , m_bHandlesWindowsInput(false)
  , m_bUseAbsoluteCursorPosition(false)
  , m_bMultithreadedAdvance(MULTITHREADED_ADVANCE)
  , m_iLastActiveCursor(0)
  , m_iTriggerCount(0)
  , m_pTriggerID()
  , m_pGfXKeyMap()
  , m_bTriggerIDOldState()
  , m_pInputMap(NULL)
#ifdef SF_AMP_SERVER
  , m_pAmpAppController(NULL)
#endif
{
  // init all mouse cursors
  for(int i = 0; i < SF_MAX_CURSOR_NUM; i++)
  {
    m_pfCursorPosX[i] = 0;
    m_pfCursorPosY[i] = 0;
    m_iButtonMask[i] = 0;
  }
}

VScaleformManager::~VScaleformManager()
{
}

/*static*/ VScaleformManager& VScaleformManager::GlobalManager()
{
  return s_globalInstance;
}

/// Returns the collection of all scaleform movie instances
VRefCountedCollection<VScaleformMovieInstance>& VScaleformManager::Instances()
{
  return m_Instances;
}

///\brief
/// Static function to access the global instance of the Scaleform manager
Scaleform::GFx::Loader * VScaleformManager::GetLoader() const
{
  return m_pLoader;
}

void VScaleformManager::Init()
{
  if (m_bInitialized) 
    return;

  // register profiling (once)
  #ifdef PROFILING
    PROFILING_ADVANCE       = Vision::Profiling.GetFreeElementID();
    PROFILING_RENDER        = Vision::Profiling.GetFreeElementID();
    PROFILING_FSCOMMAND     = Vision::Profiling.GetFreeElementID();
    PROFILING_EXT_INTERFACE = Vision::Profiling.GetFreeElementID();

    Vision::Profiling.AddGroup("Scaleform");
    Vision::Profiling.AddElement(PROFILING_ADVANCE, "Play", TRUE);
    Vision::Profiling.AddElement(PROFILING_RENDER,  "Render", TRUE);
    Vision::Profiling.AddElement(PROFILING_FSCOMMAND,  "Fs Command",TRUE);
    Vision::Profiling.AddElement(PROFILING_EXT_INTERFACE,  "External Interface", TRUE);
  #endif

  // initialize input
  m_pCursorInputMap = new VInputMap(SF_CONTROLS_LENGTH, 2);
  
  #if defined(_VISION_WIN32)
    m_pCursorInputMap->MapTrigger(CURSOR_0_CLICK_LEFT,   V_PC_MOUSE, CT_MOUSE_LEFT_BUTTON);
    m_pCursorInputMap->MapTrigger(CURSOR_0_CLICK_MIDDLE, V_PC_MOUSE, CT_MOUSE_MIDDLE_BUTTON);
    m_pCursorInputMap->MapTrigger(CURSOR_0_CLICK_RIGHT,  V_PC_MOUSE, CT_MOUSE_RIGHT_BUTTON);

    m_pCursorInputMap->MapTrigger(CURSOR_0_DELTA_X,  V_PC_MOUSE, CT_MOUSE_NORM_DELTA_X, VInputOptions::Sensitivity(2.0f));
    m_pCursorInputMap->MapTrigger(CURSOR_0_DELTA_Y,  V_PC_MOUSE, CT_MOUSE_NORM_DELTA_Y, VInputOptions::Sensitivity(2.0f));
    
    m_pCursorInputMap->MapTriggerAxis(CURSOR_0_SCROLL,   V_PC_MOUSE, CT_MOUSE_WHEEL_DOWN, CT_MOUSE_WHEEL_UP);

  #elif defined(_VISION_XENON)
    m_pCursorInputMap->MapTrigger(CURSOR_0_CLICK_LEFT,   V_XENON_PAD(0), CT_PAD_X);
    m_pCursorInputMap->MapTrigger(CURSOR_0_CLICK_MIDDLE, V_XENON_PAD(0), CT_PAD_A);
    m_pCursorInputMap->MapTrigger(CURSOR_0_CLICK_RIGHT,  V_XENON_PAD(0), CT_PAD_B);

    m_pCursorInputMap->MapTriggerAxis(CURSOR_0_DELTA_X,  V_XENON_PAD(0), CT_PAD_LEFT_THUMB_STICK_LEFT, CT_PAD_LEFT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.25f, true));
    m_pCursorInputMap->MapTriggerAxis(CURSOR_0_DELTA_Y,  V_XENON_PAD(0), CT_PAD_LEFT_THUMB_STICK_UP, CT_PAD_LEFT_THUMB_STICK_DOWN,    VInputOptions::DeadZone(0.25f, true));

    m_pCursorInputMap->MapTriggerAxis(CURSOR_0_SCROLL,   V_XENON_PAD(0), CT_PAD_RIGHT_THUMB_STICK_DOWN, CT_PAD_RIGHT_THUMB_STICK_UP,  VInputOptions::DeadZone(0.25f, true));
  
  #elif defined(_VISION_PS3)
    m_pCursorInputMap->MapTrigger(CURSOR_0_CLICK_LEFT,   V_PS3_PAD(0), CT_PAD_CROSS);
    m_pCursorInputMap->MapTrigger(CURSOR_0_CLICK_MIDDLE, V_PS3_PAD(0), CT_PAD_TRIANGLE);
    m_pCursorInputMap->MapTrigger(CURSOR_0_CLICK_RIGHT,  V_PS3_PAD(0), CT_PAD_SQUARE);

    m_pCursorInputMap->MapTriggerAxis(CURSOR_0_DELTA_X,  V_PS3_PAD(0), CT_PAD_LEFT_THUMB_STICK_LEFT, CT_PAD_LEFT_THUMB_STICK_RIGHT,  VInputOptions::DeadZone(0.25f, true));
    m_pCursorInputMap->MapTriggerAxis(CURSOR_0_DELTA_Y,  V_PS3_PAD(0), CT_PAD_LEFT_THUMB_STICK_UP, CT_PAD_LEFT_THUMB_STICK_DOWN,     VInputOptions::DeadZone(0.25f, true));

    m_pCursorInputMap->MapTriggerAxis(CURSOR_0_SCROLL,   V_PS3_PAD(0), CT_PAD_RIGHT_THUMB_STICK_DOWN, CT_PAD_RIGHT_THUMB_STICK_UP,   VInputOptions::DeadZone(0.25f, true));

  #elif defined(_VISION_IOS)
    // Implemented in the platform specific VScaleformManager::HandleInput()

  #elif defined(_VISION_ANDROID)
    // Implemented in the platform specific VScaleformManager::HandleInput()

  #elif defined(_VISION_TIZEN)
    // Implemented in the platform specific VScaleformManager::HandleInput()

  #else
    #error "Undefined Platform!"

  #endif

  m_pCommandQueue = new VScaleformCommandQueue();
  m_pCommandQueue->Start();

  // initialize Scaleform HAL
#if defined (_VR_DX9)
  D3DPRESENT_PARAMETERS d3dpp;
  VVideoConfig *pVideoCfg = Vision::Video.GetCurrentConfig();
  VASSERT(pVideoCfg);
  Vision::Video.GetD3DPPFromVideoConfig(*pVideoCfg, d3dpp);
  m_pd3dDevice = Vision::Video.GetD3DDevice();
  VASSERT_MSG(m_pd3dDevice!=NULL, "No D3D device present!");
#elif defined (_VISION_XENON) 
  m_pd3dDevice = Vision::Video.GetD3DDevice();
  VASSERT_MSG(m_pd3dDevice!=NULL, "No D3D device present!");
#elif defined (_VR_DX11)
  m_pd3dDevice = Vision::Video.GetD3DDevice();
  m_pd3dContext = Vision::Video.GetD3DDeviceContext();
  VASSERT_MSG(m_pd3dDevice!=NULL, "No D3D device present!");
#endif

  bool bSuccess;
#if defined(_VISION_WIN32) && defined(_VR_DX9)
  m_pRenderHal = new D3D9::HAL(m_pCommandQueue);
  bSuccess = m_pRenderHal->InitHAL(D3D9::HALInitParams(m_pd3dDevice, d3dpp, D3D9::HALConfig_NoSceneCalls));
#elif defined(_VISION_WIN32) && defined(_VR_DX11)
  m_pRenderHal = new D3D1x::HAL(m_pCommandQueue);
  bSuccess = m_pRenderHal->InitHAL(D3D1x::HALInitParams(m_pd3dDevice, D3D11(m_pd3dContext)));
#elif defined (_VISION_XENON)
  m_pRenderHal = new X360::HAL(m_pCommandQueue);
  bSuccess = m_pRenderHal->InitHAL(X360::HALInitParams(m_pd3dDevice, d3dpp, 0, X360::HALConfig_NoSceneCalls, 
    0)); // Scaleform::GetCurrentThreadId())); // use specific thread id to limit texture creation to the render thread
#elif defined (_VISION_PS3)
  m_pRenderHal = new PS3::HAL(m_pCommandQueue);
  bSuccess = m_pRenderHal->InitHAL(PS3::HALInitParams(gCellGcmCurrentContext, &g_MemManager));
#elif defined(_VISION_MOBILE)
  m_pRenderHal = new VisionGLHAL(m_pCommandQueue);
  bSuccess = m_pRenderHal->InitHAL(GL::HALInitParams());

  // TODO: remove this as soon as SF provides proper solution (used to clean error stack - temp solution)
  glGetError();
#endif
  VASSERT_MSG(bSuccess, "Unable to initialize Scaleform HAL");

  m_pLastUsedColorRenderTarget = NULL;
  m_pLastUsedDepthStencilTarget = NULL;

  m_pRenderer = new Renderer2D(m_pRenderHal);

  VASSERT_MSG(m_pRenderer, "Unable to initialize Scaleform Renderer2D");

  #ifdef SF_AMP_SERVER
    AMP::Server::Init();
    AMP::Server::GetInstance().SetRenderer(m_pRenderer);

    m_pAmpAppController = new VScaleformAmpAppController(this);
    AMP::Server::GetInstance().SetAppControlCallback(m_pAmpAppController);
    AMP::Server::GetInstance().SetAppControlCaps(m_pAmpAppController->GetCaps());
  #endif

  m_bInitialized = true; //set the bool to true at this point will avoid a loop in CreateLoader

  CreateLoader();
  VASSERT_MSG(m_pLoader, "Unable to create a Loader object")

  // register callbacks
  Vision::Callbacks.OnUpdateSceneBegin += this;
  Vision::Callbacks.OnFinishScene += this;
  Vision::Callbacks.OnRenderHook += this;
  Vision::Callbacks.OnVideoChanged += this;
  Vision::Callbacks.OnBeforeVideoChanged += this;
  Vision::Callbacks.OnLeaveForeground += this;
  Vision::Callbacks.OnEnterForeground += this;
  Vision::Callbacks.OnEnterBackground += this;
  Vision::Callbacks.OnLeaveBackground += this;
  Vision::Callbacks.OnEngineDeInit += this;

  // Start / Stop the movie when switching the PlayTheGame modes in vForge.
  if (Vision::Editor.IsInEditor())
  {
    Vision::Callbacks.OnAfterSceneLoaded += this;
    Vision::Callbacks.OnBeforeSceneUnloaded += this;
  }

  // Also register to Fmod deinit callback to be able to release all resources
  // prior to Fmod shutdown.
  VFmodManager::GlobalManager().OnBeforeDeinitializeFmod += this;
}

void VScaleformManager::DeInit()
{
  if (!m_bInitialized) 
    return;  

  // deregister callbacks
  Vision::Callbacks.OnUpdateSceneBegin -= this;
  Vision::Callbacks.OnFinishScene -= this;
  Vision::Callbacks.OnRenderHook -= this;
  Vision::Callbacks.OnVideoChanged -= this;
  Vision::Callbacks.OnBeforeVideoChanged -= this;
  Vision::Callbacks.OnLeaveForeground -= this;
  Vision::Callbacks.OnEnterForeground -= this;
  Vision::Callbacks.OnEnterBackground -= this;
  Vision::Callbacks.OnLeaveBackground -= this;
  Vision::Callbacks.OnEngineDeInit -= this;
  Vision::Callbacks.OnVideoInitialized -= this;

  if (Vision::Editor.IsInEditor())
  {
    Vision::Callbacks.OnAfterSceneLoaded -= this;
    Vision::Callbacks.OnBeforeSceneUnloaded -= this;
  }

  VFmodManager::GlobalManager().OnBeforeDeinitializeFmod -= this;

  // Free all elements.
  // Make sure all remaining movie instances in scaleform textures are released.
  const int iCount = m_Instances.Count();
  for (int i = 0; i < iCount; i++)
  {
    VScaleformMovieInstance* pMovieInstance = m_Instances.GetAt(i);
    
    // Only scaleform textures should have a reference to the remaining movies now.
    if (pMovieInstance->GetRefCount() > 1)
    {
      VASSERT_MSG(pMovieInstance->m_bIsTexture, "Scaleform movie instance was not released.");
      pMovieInstance->Invalidate();
    }
  }

  m_Instances.Clear();

#if defined(SF_AMP_SERVER)
  AMP::Server::GetInstance().RemoveLoader(m_pLoader);
#endif
  V_SAFE_DELETE(m_pLoader);

#if defined(SF_AMP_SERVER)
  AMP::Server::Uninit();
  V_SAFE_DELETE(m_pAmpAppController);
#endif

  V_SAFE_RELEASE(m_pRenderer);
  V_SAFE_RELEASE(m_pRenderHal);

#if defined(_VISION_WIN32)
  SetHandleWindowsInput(false);
  V_SAFE_DELETE(m_pFontConfigSet);
#endif
  V_SAFE_DELETE(m_pCursorInputMap);
  V_SAFE_DELETE(m_pCommandQueue);

  m_pLastUsedColorRenderTarget = NULL;
  m_pLastUsedDepthStencilTarget = NULL;

  m_bInitialized = false;
}

Scaleform::Render::TextureManager* VScaleformManager::GetTextureManager() const
{
  return (m_pRenderHal == NULL) ? NULL : m_pRenderHal->GetTextureManager();
}

bool VScaleformManager::UnloadMovie(VScaleformMovieInstance *pMovie)
{
  if (m_Instances.Contains(pMovie))
  {
    m_Instances.Remove(pMovie);
    return true; 
  }

  return false;
}

void VScaleformManager::UnloadAllMovies()
{
  // clear the list of instances, but not the ones that are created for textures
  for (int i = m_Instances.Count()-1; i >= 0; i--)
  {
    if (!m_Instances.GetAt(i)->m_bIsTexture)
      m_Instances.RemoveAt(i);
  }
}

VScaleformMovieInstance* VScaleformManager::LoadMovie(const char *szFileName, const char *szCandidateMovie, const char *szImeXml,
  int iPosX, int iPosY, int iWidth, int iHeight)
{
  //Lazy loading: This is required because at EngineInitilization the thread manager is not present!
  if (!m_bInitialized) 
    Init();

  if (!Vision::File.Exists(szFileName))
  {
    hkvLog::Warning("Scaleform: File '%s' does not exist, movie file not loaded.", szFileName);
    return NULL;
  }

  VScaleformMovieInstance *pInstance = new VScaleformMovieInstance(szFileName, m_pLoader, szCandidateMovie, szImeXml, iPosX, iPosY, iWidth, iHeight);

  // Return NULL if loading fails.
  if (!pInstance->IsValid())
  {
    V_SAFE_DELETE(pInstance);
    return NULL;
  }

  m_Instances.Add(pInstance);

  // In the editor, we don't want the movie to start if the view is not animated.
  if (Vision::Editor.IsInEditor() && !Vision::Editor.IsAnimatingOrPlaying())
  {
    // Advance the movie, so that the first frame is visible.
    pInstance->m_pAdvanceTask->ScheduleMinStep();
    pInstance->SetPaused(true);
  }

  return pInstance;
}

VScaleformMovieInstance * VScaleformManager::LoadMovie(
  const char *szFileName, int iPosX, int iPosY, int iWidth, int iHeight)
{
  return LoadMovie(szFileName, NULL, NULL, iPosX, iPosY, iWidth, iHeight);
}

VScaleformMovieInstance * VScaleformManager::GetMovie(const char *szFileName) const
{
  int iCount = m_Instances.Count();

  for(int i = 0; i < iCount; i++)
  {
    if (VStringHelper::SafeCompare(m_Instances.GetAt(i)->GetFileName(), szFileName, true) == 0)
      return m_Instances.GetAt(i);
  }

  return NULL;
}

Scaleform::GFx::Loader* VScaleformManager::CreateLoader()
{
  if (m_pLoader != NULL)
  {
    return m_pLoader;
  }

  if (!m_bInitialized)
  {
    Init(); //will care about loader creation
    VASSERT_MSG(m_pLoader, "No loader after initialization present!");
    return m_pLoader;
  }

  // Have to init Scaleform before creating a Scaleform::GFx::Loader
  calculateScaleformBase();

  m_pLoader = new VLocalLoader();

  // Set log
  m_pLoader->SetLog(Scaleform::Ptr<Log>(*new VScaleformLog()));

  // Set file-opener
  Scaleform::Ptr<FileOpener> opener = *new VGFxFileOpener;
  m_pLoader->SetFileOpener(opener);

  // install ImageFileHandlerRegistry on the loader
  Scaleform::Ptr<Scaleform::GFx::ImageFileHandlerRegistry> pImgReg = *new Scaleform::GFx::ImageFileHandlerRegistry();
#ifdef SF_ENABLE_LIBJPEG
  pImgReg->AddHandler(&JPEG::FileReader::Instance);
#endif
#ifdef SF_ENABLE_LIBPNG
  pImgReg->AddHandler(&PNG::FileReader::Instance);
#endif
  pImgReg->AddHandler(&TGA::FileReader::Instance); 
  pImgReg->AddHandler(&DDS::FileReader::Instance);
  m_pLoader->SetImageFileHandlerRegistry(pImgReg);

  // Set FSCallback on a movie (deprecated!)
  VASSERT_MSG(m_pLoader->GetFSCommandHandler()==NULL, "Warning: Existing FSCommandHandler will be overwritten!");
  Scaleform::Ptr<FSCommandHandler> spFsCommandHandler = *new VScaleformFSCommandHandler;
  m_pLoader->SetFSCommandHandler(spFsCommandHandler);

  // Set the external interface callback on the movie
  VASSERT_MSG(m_pLoader->GetExternalInterface()==NULL, "Warning: Existing ExternalInterface will be overwritten!");
  Scaleform::Ptr<ExternalInterface> spExternalInterface = *new VScaleformExternalInterfaceHandler;
  m_pLoader->SetExternalInterface(spExternalInterface);

#if defined (_VISION_WIN32)
  // win32 specific: add default font loader
  Scaleform::Ptr<FontProviderWin32> fontProvider = *new FontProviderWin32(::GetDC(0));
  m_pLoader->SetFontProvider(fontProvider);
#endif

  Scaleform::Ptr<ASSupport> pAS2Support = *new AS2Support();
  m_pLoader->SetAS2Support(pAS2Support); 
  Scaleform::Ptr<ASSupport> pASSupport = *new AS3Support();
  m_pLoader->SetAS3Support(pASSupport);

  Scaleform::Render::TextureManager *pTextureManager = m_pRenderHal->GetTextureManager();
  VASSERT_MSG(pTextureManager!=NULL, "Scaleform texture manager not present!");

  pTextureManager->ProcessQueues(); //just to be sure that there are no pending tasks

  // Assign the image creator to the loader (with our texture manager).
  Scaleform::Ptr<ImageCreator> spImageCreator = *new ImageCreator(pTextureManager);
  m_pLoader->SetImageCreator(spImageCreator);

#if defined(GFX_USE_VIDEO) && defined(GFX_VIDEO_SDK_INSTALLED)
  Scaleform::Ptr<Video::Video> pVideo = *new Video::VideoPC(Video::VideoVMSupportAll());
  m_pLoader->SetVideo(pVideo);

  pVideo->SetTextureManager(pTextureManager);
#endif

#if defined(GFX_ENABLE_SOUND)
  FMOD::System* pSoundManager =  VFmodManager::GlobalManager().GetFmodSystem();
  VASSERT_MSG(pSoundManager!=NULL, "No FMOD sound manager found! Maybe you miss VISION_PLUGIN_ENSURE_LOADED(vFmodEnginePlugin); in your code, or compile vScaleform plugin without GFX_ENABLE_SOUND");

  // Create scaleform sound renderer.
  Scaleform::Ptr<Scaleform::Sound::SoundRendererFMOD> pSoundRenderer = *Scaleform::Sound::SoundRendererFMOD::CreateSoundRenderer();
  bool result = pSoundRenderer->Initialize(pSoundManager, false /* call update */, false /* use own thread for fmod update */);
  VASSERT_MSG(result, "Unable to initialize SoundRendererFMOD instance in VScaleformManager");

  //set as current audio system
  Scaleform::Ptr<Audio> pAudio = *new Audio(pSoundRenderer);
  m_pLoader->SetAudio(pAudio);
#else
  hkvLog::Warning("VScaleformManager: Sound is disabled - maybe the linkage of the vScaleformPlugin is incorrect.");
#endif

#ifdef SF_AMP_SERVER
  AMP::Server::GetInstance().AddLoader(m_pLoader);
#endif

  return m_pLoader;
}

#if defined(_VISION_WIN32)

SCALEFORM_IMPEXP bool VScaleformManager::ApplyFontToTheLoader(const char *szFontFilename, const char *szConfigName)
{
  VFileAccessManager::NativePathResult nativePathResult;
  if (VFileAccessManager::GetInstance()->MakePathNative(szFontFilename, nativePathResult, VFileSystemAccessMode::READ, VFileSystemElementType::FILE) == HKV_FAILURE)
  {
    return false;
  }

  CreateLoader();

  if(m_pFontConfigSet == NULL)
    m_pFontConfigSet = new FontConfigSet;

  ConfigParser parser(nativePathResult.m_sNativePath); 
  VASSERT(parser.IsValid()); 
  m_pFontConfigSet->Parse(&parser); 

  if(m_pFontConfigSet->Configs.IsEmpty())
  {
    return false;
  }

  if(szConfigName == NULL)
    szConfigName = m_pFontConfigSet->Configs[0]->ConfigName;

  return ApplyFontConfig(szConfigName);
}

bool VScaleformManager::ApplyFontConfig(const char *szConfigName)
{
  VASSERT_MSG(m_pFontConfigSet != NULL,"No Fontconfigset present, Please call ApplyFontToTheLoader method first.");
  VASSERT_MSG(szConfigName != NULL,"Please use correct config name");
  if( m_pFontConfigSet != NULL && szConfigName != NULL)
  { 
    int idx = GetFontConfigIndexByName(szConfigName); 
    if (idx != -1) 
    { 
      FontConfig* pconfig = (*m_pFontConfigSet)[idx];
      pconfig->Apply(m_pLoader); 
      return true;
    } 
  }
  return false;
}

int VScaleformManager::GetFontConfigIndexByName(const char *szConfigName)
{
  VASSERT_MSG(m_pFontConfigSet,"No Fontconfigset present, Please call ApplyFontToTheLoader method first.");
  VASSERT_MSG(szConfigName != NULL,"Please use correct config name");
  if( m_pFontConfigSet != NULL && szConfigName != NULL)
  {
    for (unsigned i = 0; i < m_pFontConfigSet->GetSize(); i++)
    {
      if (strcmp(szConfigName, (*m_pFontConfigSet)[i]->ConfigName) == 0)
        return i;
    }   
  }
  return -1;
}

#endif

void VScaleformManager::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  // De-Initialization
  if ((pData->m_pSender == &Vision::Callbacks.OnEngineDeInit || 
    pData->m_pSender == &VFmodManager::GlobalManager().OnBeforeDeinitializeFmod))
  {
    DeInit();
    return;
  }
  
  const int iCount = m_Instances.Count();
  VScaleformMovieInstance** ppInstances = m_Instances.GetPtrs();

  // Perform rendering
  if (pData->m_pSender == &Vision::Callbacks.OnRenderHook)
  {
    if (((VisRenderHookDataObject_cl *)pData)->m_iEntryConst != m_RenderOrder)
    {
      return;
    }

    RenderMovies(ppInstances, iCount);
    return;
  }

  // Tick function: Advance the movie and handle input.
  if (pData->m_pSender == &Vision::Callbacks.OnUpdateSceneBegin)
  {
    if (Vision::Editor.IsAnimatingOrPlaying()) 
    {
      // Trigger call back. May be used to access VScaleformValue objects just before advancing the movies.
      OnBeforeAdvanceMovies.TriggerCallbacks();

      VISION_PROFILE_FUNCTION(PROFILING_ADVANCE);

      if (m_bHandleInput) 
      {
        HandleInput();
      }

      for (int i = 0; i < iCount; i++)
      {
        ppInstances[i]->m_pAdvanceTask->Schedule(Vision::GetTimer()->GetTimeDifference());
      }

      #if defined(SF_AMP_SERVER)
        AMP::Server::GetInstance().AdvanceFrame();
      #endif
    }

    return;
  }

  if (pData->m_pSender == &Vision::Callbacks.OnFinishScene)
  {
    for (int i = 0; i < iCount; i++)
    {
      // Wait for task to finish in order to fire the callbacks.
      // If we do several update steps per frame. RenderMovies() has not been called yet.
      ppInstances[i]->m_pAdvanceTask->WaitUntilFinished();

      ppInstances[i]->HandleScaleformCallbacks();

      #if defined(USE_SF_IME)
        ppInstances[i]->HandleImeStatusChanges();
      #endif
    }
    return;
  }

  // Prepare for video mode change
  if (pData->m_pSender == &Vision::Callbacks.OnBeforeVideoChanged)
  {
#if defined(_VISION_WIN32) && (defined(_VR_DX9) || defined(_VR_DX11))
    WaitForAllTasks();

    if(m_pRenderHal != NULL)
      m_pRenderHal->PrepareForReset();

#endif
    return;
  }

  // Update the viewport
  if (pData->m_pSender == &Vision::Callbacks.OnVideoChanged)
  {
    WaitForAllTasks();

#if defined(_VISION_WIN32) && (defined(_VR_DX9) || defined(_VR_DX11))

    if(m_pRenderHal != NULL)
      m_pRenderHal->RestoreAfterReset();

#endif

    for (int i = 0; i < iCount; i++)
    {
      ppInstances[i]->m_pLastRenderContext = NULL;
    }

    m_pLastUsedColorRenderTarget = NULL;
    m_pLastUsedDepthStencilTarget = NULL;

    return;
  }

  // Leave foreground (= sleep mode on Android)
  if (pData->m_pSender == &Vision::Callbacks.OnLeaveForeground)
  {

#if defined(_VISION_ANDROID)

    WaitForAllTasks();

    Scaleform::GFx::AppLifecycleEvent lifecycleEvent(Scaleform::GFx::AppLifecycleEvent::OnPause);
    HandleEvent(&lifecycleEvent);

#endif
    return;
  }

  if (pData->m_pSender == &Vision::Callbacks.OnEnterForeground)
  {

#if defined(_VISION_ANDROID)

    WaitForAllTasks();


    Scaleform::GFx::AppLifecycleEvent lifecycleEvent(Scaleform::GFx::AppLifecycleEvent::OnResume);
    HandleEvent(&lifecycleEvent);

#endif

    return;
  }

  // Enter Background / Device lost
  if (pData->m_pSender == &Vision::Callbacks.OnEnterBackground)
  {

#if defined(_VISION_WIN32) && defined(_VR_DX9)

    WaitForAllTasks();

    if(m_pRenderHal != NULL)
      m_pRenderHal->PrepareForReset();

#elif defined(_VISION_ANDROID)

    // OpenGLES context will soon be lost. Notify Scaleform.
    if (m_pRenderHal != NULL)
    {
      static_cast<VisionGLHAL*>(m_pRenderHal)->PrepareResetContext();

      VASSERT_MSG(glGetError() == GL_NO_ERROR, "OpenGLES error detected after deinitializing Scaleform HAL.");
    }

#endif

    return;
  }

  // Leave Background / Device recreated
  if (pData->m_pSender == &Vision::Callbacks.OnLeaveBackground)
  {
    WaitForAllTasks();

#if defined(_VISION_WIN32) && defined(_VR_DX9) 

    if(m_pRenderHal != NULL)
      m_pRenderHal->RestoreAfterReset();

#elif defined(_VISION_ANDROID)

    // Reinitialize HAL with new OpenGLES context.
    if (m_pRenderHal != NULL)
    {
      const bool bResult = static_cast<VisionGLHAL*>(m_pRenderHal)->RestoreAfterResetContext();

      VASSERT_MSG(bResult, "ScaleformEnginePlugin: RestoreAfterResetContext() failed.");
      VASSERT_MSG(glGetError() == GL_NO_ERROR, "OpenGLES error detected after reinitializing Scaleform HAL.");

      Vision::Renderer.ResetAllStates();
    }

#endif

    for (int i = 0; i < iCount; i++)
    {
      ppInstances[i]->m_pLastRenderContext = NULL;
    }

    m_pLastUsedColorRenderTarget = NULL;
    m_pLastUsedDepthStencilTarget = NULL;
  
    return;
  }

  // Start movies in vForge when starting to animate.
  if (pData->m_pSender == &Vision::Callbacks.OnAfterSceneLoaded)
  {
    for (int i = 0; i < iCount; i++)
    {
      VScaleformMovieInstance* pInstance = ppInstances[i];
      if (pInstance->m_bIsTexture)
      {
        pInstance->SetPaused(false);
        pInstance->Restart();
      }
    }
    return;
  }

  // Pause and restart movies in vForge when not animating anymore.
  if (pData->m_pSender == &Vision::Callbacks.OnBeforeSceneUnloaded)
  {
    for (int i = 0; i < iCount; i++)
    {
      VScaleformMovieInstance* pInstance = ppInstances[i];
      if (pInstance->m_bIsTexture)
      {
        pInstance->Restart();

        // Advance the movie, so that the first frame is visible.
        pInstance->m_pAdvanceTask->ScheduleMinStep();
        pInstance->SetPaused(true);
      } 
    }
    return;
  }

#if defined(_VISION_WIN32)
  #if defined(USE_SF_IME)
    if (pData->m_pSender == &VInputCallbacks::OnPreTranslateMessage)
    {
      VWindowsMessageDataObject* pObj = (VWindowsMessageDataObject*)pData;

      for (int i = 0;i < iCount;i++)
      {
        if (ppInstances[i]->IsFocused())
        {
          ppInstances[i]->HandlePreTranslatedIMEInputMessage(pObj); 
          return; //we are done when the focus movie handled the message
        }
      }
      return;
    }
  #endif

  if (pData->m_pSender == &VInputCallbacks::OnPostTranslateMessage)
  {
    VWindowsMessageDataObject* pObj = (VWindowsMessageDataObject*)pData;

    if (pObj->m_Msg.message == WM_IME_SETCONTEXT)
    {
      DefWindowProc(pObj->m_Msg.hwnd, pObj->m_Msg.message, pObj->m_Msg.wParam, 0);
      pObj->m_bProcessed = true;
      return;
    }
      
    if(m_bHandlesWindowsInput)
    {
      for (int i = 0; i < iCount; i++)
      {
        if (ppInstances[i]->IsFocused())
        { 
          ppInstances[i]->HandleWindowsInputMessage(pObj);

          if (pObj->m_bProcessed)
            return; // We are done when the focused movie handled the message.
        }
      }
    }
    
    //no window has the focus...
    if (pObj->m_Msg.message == WM_SETFOCUS)
    {
      float x, y;
      GetCursorPos(x, y, m_iLastActiveCursor);
      ValidateFocus(x, y);
    }
    return;
  }

#endif
}

// Input Handling
//=================
#if defined(_VISION_WIN32)

void VScaleformManager::SetHandleWindowsInput(bool bEnable)
{
  if(bEnable == m_bHandlesWindowsInput)
    return;

  if(bEnable)
  {
    VInputCallbacks::OnPostTranslateMessage += this;
    VInputCallbacks::OnPreTranslateMessage += this;
  }
  else
  {
    VInputCallbacks::OnPostTranslateMessage -= this;
    VInputCallbacks::OnPreTranslateMessage -= this;
  }

  m_bHandlesWindowsInput = bEnable;
}
#endif

void VScaleformManager::SetRenderOrder(VRenderHook_e eRenderHook)
{ 
  m_RenderOrder = eRenderHook;
}

void VScaleformManager::UpdateRenderTargetToScaleform()
{
  VisRenderContext_cl *pCurrentContext = Vision::Contexts.GetCurrentContext();  

  VTextureObject* pColorRenderTarget = pCurrentContext->GetRenderTarget();
  VTextureObject* pDepthStencilTarget = pCurrentContext->GetDepthStencilTarget();

  // If render target hasn't changed, we don't need to do anything
  if(pColorRenderTarget == m_pLastUsedColorRenderTarget && pDepthStencilTarget == m_pLastUsedDepthStencilTarget)
  {
    return;
  }

  // If render target has changed from previous time Scaleform has rendered, we also need to
  // tell Scaleform about the changed render target.
#if defined (_VR_DX9)

  D3DSurface* pDepthStencilSurface = NULL;
  D3DSurface* pRenderTargetSurface = NULL;
  pCurrentContext->GetDepthStencilSurface(pDepthStencilSurface);
  pCurrentContext->GetRenderSurface(0, pRenderTargetSurface);

  RenderTarget* pScaleformRenderTarget = m_pRenderHal->CreateRenderTarget(pRenderTargetSurface, pDepthStencilSurface);
  m_pRenderHal->SetRenderTarget(pScaleformRenderTarget);
  pScaleformRenderTarget->Release();

#elif defined(_VR_DX11)

  D3DDepthStencilView* pDepthStencilView = NULL;
  D3DRenderTargetView* pRenderTargetView = NULL;
  pCurrentContext->GetDepthStencilView(pDepthStencilView);
  pCurrentContext->GetRenderTargetView(0, pRenderTargetView);

  RenderTarget* pScaleformRenderTarget = m_pRenderHal->CreateRenderTarget(pRenderTargetView, pDepthStencilView);
  m_pRenderHal->SetRenderTarget(pScaleformRenderTarget);
  pScaleformRenderTarget->Release();

#elif defined(_VR_GLES2)

  unsigned int iFrameBufferHandle = 0;
  pCurrentContext->GetFrameBufferObject(iFrameBufferHandle);

  RenderTarget* pScaleformRenderTarget = m_pRenderHal->CreateRenderTarget(iFrameBufferHandle);
  m_pRenderHal->SetRenderTarget(pScaleformRenderTarget);
  pScaleformRenderTarget->Release();

#endif

  m_pLastUsedColorRenderTarget = pColorRenderTarget;
  m_pLastUsedDepthStencilTarget = pDepthStencilTarget;
}

void VScaleformManager::RenderMovies(VScaleformMovieInstance** ppInstances, unsigned int iCount)
{
  // We need to check if we have switched render targets and if so, notify Scaleform about it
  UpdateRenderTargetToScaleform();

  VisRenderContext_cl *pCurrentContext = Vision::Contexts.GetCurrentContext();  
  // Scaleform relies on these depth stencil state values
  VStateGroupDepthStencil defaultDepthStencilState;
  defaultDepthStencilState.m_bStencilTestEnabled = true;
  defaultDepthStencilState.ComputeHash();
  VisRenderStates_cl::SetDepthStencilState(defaultDepthStencilState);
  defaultDepthStencilState.m_bStencilTestEnabled = false;
  defaultDepthStencilState.ComputeHash();
  VisRenderStates_cl::SetDepthStencilState(defaultDepthStencilState);

  for (unsigned int i = 0; i < iCount; i++)
  {
    ppInstances[i]->m_pAdvanceTask->WaitUntilFinished();
  }

  m_pRenderer->BeginFrame();
  for (unsigned int i = 0; i < iCount; i++)
  {
    INSERT_PERF_MARKER_SCOPE("Scaleform Rendering");
    VISION_PROFILE_FUNCTION(PROFILING_RENDER);

    VScaleformMovieInstance* pCurrentInstance = ppInstances[i];

    const bool bRenderMovie = (pCurrentInstance->m_iVisibleMask & pCurrentContext->GetRenderFilterMask()) != 0;

    if (bRenderMovie)
    {
      if (pCurrentInstance->m_pLastRenderContext != pCurrentContext)
      {
        pCurrentInstance->m_pLastRenderContext = pCurrentContext;
        pCurrentInstance->UpdateViewport();
      }

#if defined (_VISION_PS3)
      // Note: Scaleform expects the transfer location to be LOCAL!
      cellGcmSetTransferLocation(gCellGcmCurrentContext, CELL_GCM_LOCATION_LOCAL);
#endif

      if (pCurrentInstance->m_phMovieDisplay->NextCapture(m_pRenderer->GetContextNotify()))
      { 
        m_pRenderer->Display(*pCurrentInstance->m_phMovieDisplay);
      }
    }
  }
  m_pRenderer->EndFrame();

  // Make renderer states are set back to the old values.
  Vision::Renderer.ResetAllStates();
}

void VScaleformManager::SetHandleCursorInput(bool bStatus)
{
  m_bHandleInput=bStatus;
}

void VScaleformManager::SetCursorPos(float fCursorX, float fCursorY, float fWheelDelta /* = 0 */,
  bool bLeftBtn /* = false */, bool bMiddleBtn /* = false */, bool bRightBtn /* = false */, int iCursorNum /* = 0 */)
{
  VASSERT_MSG(iCursorNum<SF_MAX_CURSOR_NUM, "Too many cursors present! Increment constant SF_MAX_CURSOR_NUM to use more cursors than currently defined.");
  
  //clamping
  if(fCursorX<0) fCursorX=0;
  if(fCursorY<0) fCursorY=0;

  if(fCursorX>Vision::Video.GetXRes()) fCursorX=(float)Vision::Video.GetXRes();
  if(fCursorY>Vision::Video.GetYRes()) fCursorY=(float)Vision::Video.GetYRes();

  if(bLeftBtn || bMiddleBtn || bRightBtn)
  {
    ValidateFocus(fCursorX, fCursorY);
  }

  // mouse move
  if (fCursorX!=m_pfCursorPosX[iCursorNum] || fCursorY!=m_pfCursorPosY[iCursorNum] )
  {
    //report delta value
    MouseEvent moveEvent(Event::MouseMove, 0, fCursorX, fCursorY, 0, iCursorNum);
    HandleEvent(&moveEvent);
  }

  //compare with current button mask (because we can handle multiple cursors)
  int iMask = m_iButtonMask[iCursorNum];

  //left (mouse) button
  if ((iMask & MOUSEB_LEFT)==0 && bLeftBtn)
  {
    MouseEvent leftButtonDownEvent(Event::MouseDown, 0, fCursorX, fCursorY, 0, iCursorNum);
    HandleEvent(&leftButtonDownEvent);
  }
  else if ((iMask & MOUSEB_LEFT)!=0 && !bLeftBtn)
  {
    MouseEvent leftButtonUpEvent(Event::MouseUp, 0, fCursorX, fCursorY, 0, iCursorNum);
    HandleEvent(&leftButtonUpEvent);
  }

  //middle (mouse) button
  if ((iMask& MOUSEB_MIDDLE)==0 && bMiddleBtn)
  {
    MouseEvent middleButtonDownEvent(Event::MouseDown, 2 /* SF middle */, fCursorX, fCursorY, 0, iCursorNum);
    HandleEvent(&middleButtonDownEvent);
  }
  else if ((iMask& MOUSEB_MIDDLE)!=0 && !bMiddleBtn)
  {
    MouseEvent middleButtonUpEvent(Event::MouseUp, 2 /* SF middle */, fCursorX, fCursorY, 0, iCursorNum);
    HandleEvent(&middleButtonUpEvent);
  }

  //right (mouse) button
  if ((iMask & MOUSEB_RIGHT)==0 && bRightBtn)
  {
    MouseEvent rightButtonDownEvent(Event::MouseDown, 1 /* SF right */, fCursorX, fCursorY, 0, iCursorNum);
    HandleEvent(&rightButtonDownEvent);
  }
  else if ((iMask & MOUSEB_RIGHT)!=0 && !bRightBtn)
  {
    MouseEvent rightButtonUpEvent(Event::MouseUp, 1 /* SF right */, fCursorX, fCursorY, 0, iCursorNum);
    HandleEvent(&rightButtonUpEvent);
  }

  if(fWheelDelta!=0)
  {
    MouseEvent mouseWheelEvent(Event::MouseWheel, 0, fCursorX, fCursorY, fWheelDelta, iCursorNum);
    HandleEvent(&mouseWheelEvent);
  }

  //remember mouse position and buttons as mask
  m_pfCursorPosX[iCursorNum] = fCursorX;
  m_pfCursorPosY[iCursorNum] = fCursorY;
  m_iButtonMask[iCursorNum] &= ~(int)MOUSEB_MASK;
  m_iButtonMask[iCursorNum] |= (bLeftBtn ? MOUSEB_LEFT : 0) | (bMiddleBtn ? MOUSEB_MIDDLE : 0) | (bRightBtn ? MOUSEB_RIGHT : 0);
  m_iLastActiveCursor = iCursorNum;
}

//broadcast to all movies
void VScaleformManager::ValidateFocus(float x, float y)
{
  int iCount = m_Instances.Count();

  VScaleformMovieInstance ** ppInstances = m_Instances.GetPtrs();

  bool bFocused = false;
  //start with the most recent movie (top)
  for (int i = iCount-1; i >= 0; i--)
  {
    //shortcut evaluation
    VASSERT_MSG(ppInstances[i]->m_pMovieInst!=NULL, "GFx Movie not present");
    if(ppInstances[i]->m_bHandleInput)
    {
      if(bFocused)
        ppInstances[i]->m_pMovieInst->HandleEvent(Event::KillFocus); //kill other focused object if new focus has been set
      else
        bFocused = ppInstances[i]->ValidateFocus(x, y); //this method is aware of the movies position
    }
  }
}

//broadcast to all movies
void VScaleformManager::HandleEvent(Scaleform::GFx::Event * pEvent)
{
  int iCount = m_Instances.Count();
  VScaleformMovieInstance ** ppInstances = m_Instances.GetPtrs();
  for (int i=0;i<iCount;i++)
  {
    VASSERT_MSG(ppInstances[i]->m_pMovieInst!=NULL, "GFx Movie not present");
    if(ppInstances[i]->m_bHandleInput)
    {
      ppInstances[i]->m_pMovieInst->HandleEvent(*pEvent);
    }
  }
}

void VScaleformManager::HandleEvent(Scaleform::GFx::MouseEvent * pEvent)
{
  int iCount = m_Instances.Count();
  VScaleformMovieInstance ** ppInstances = m_Instances.GetPtrs();

  int iPosX, iPosY;

  for (int i=0;i<iCount;i++)
  {
    VASSERT_MSG(ppInstances[i]->m_pMovieInst!=NULL, "GFx Movie not present");
    if(ppInstances[i]->m_bHandleInput)
    {
      ppInstances[i]->GetPosition(iPosX, iPosY);

      Scaleform::GFx::MouseEvent event(*pEvent);
      event.x -= iPosX;
      event.y -= iPosY;
      ppInstances[i]->m_pMovieInst->HandleEvent(event);
    }
  }
}

#ifdef _VISION_MOBILE
void VScaleformManager::HandleEvent(Scaleform::GFx::TouchEvent * pEvent)
{
  int iCount = m_Instances.Count();
  VScaleformMovieInstance ** ppInstances = m_Instances.GetPtrs();

  int iPosX, iPosY;
  
  for (int i=0;i<iCount;i++)
  {
    VASSERT_MSG(ppInstances[i]->m_pMovieInst!=NULL, "GFx Movie not present");
    if(ppInstances[i]->m_bHandleInput)
    {
      ppInstances[i]->GetPosition(iPosX, iPosY);
      
      Scaleform::GFx::TouchEvent event(*pEvent);
      event.x -= iPosX;
      event.y -= iPosY;
      ppInstances[i]->m_pMovieInst->HandleEvent(event);
    }
  }
}
#endif

void VScaleformManager::GetCursorPos(float &fCursorX, float &fCursorY, int iCursorNum /* = 0 */) const
{
  if(iCursorNum<0 || iCursorNum>=SF_MAX_CURSOR_NUM) iCursorNum = m_iLastActiveCursor;
  
  fCursorX=m_pfCursorPosX[iCursorNum];
  fCursorY=m_pfCursorPosY[iCursorNum];
}


void VScaleformManager::SetKeyMapping(VInputMap *pInputMap, int iTriggerCount, const int *pTriggerID, 
                                      const int *pGfXKeyMap)
{
  m_pInputMap = pInputMap;
  m_iTriggerCount = iTriggerCount;
  if (m_iTriggerCount<1)
    return;

  // enlarge dyn arrays
  m_pTriggerID.EnsureSize(m_iTriggerCount);
  m_pGfXKeyMap.EnsureSize(m_iTriggerCount);
  m_bTriggerIDOldState.EnsureSize(m_iTriggerCount);
  
  // copy the triggers and gfx keys
  memcpy(m_pTriggerID.GetDataPtr(), pTriggerID, m_iTriggerCount*sizeof(int));
  memcpy(m_pGfXKeyMap.GetDataPtr(), pGfXKeyMap, m_iTriggerCount*sizeof(int));

  // create boolean
  memset(m_bTriggerIDOldState.GetDataPtr(), 0, m_iTriggerCount*sizeof(bool));
}

#if defined(_VISION_WIN32)
  void VScaleformManager::SetAbsoluteCursorPositioning(bool bAbsolute)
  {
    m_bUseAbsoluteCursorPosition = bAbsolute;
  }
#endif


void VScaleformManager::HandleInputMap()
{
  // process input mapping, if defined via SetInputMapping
  if (m_pInputMap)
  {
    bool *pOldState = m_bTriggerIDOldState.GetDataPtr();
    for (int i=0;i<m_iTriggerCount;i++)
    {
      bool bNewState = m_pInputMap->GetTrigger(m_pTriggerID.GetDataPtr()[i])>0.f;
      if (bNewState && !pOldState[i])
      {
        KeyEvent keyDownEvent(Event::KeyDown, (Scaleform::KeyCode) m_pGfXKeyMap.GetDataPtr()[i]);
        HandleEvent(&keyDownEvent);
      } 
      else if (!bNewState && pOldState[i])
      {
        KeyEvent keyUpEvent(Event::KeyUp, (Scaleform::KeyCode) m_pGfXKeyMap.GetDataPtr()[i]);
        HandleEvent(&keyUpEvent);
      }
      pOldState[i] = bNewState;
    }
  }

}

#if defined(_VISION_MOBILE)

int VScaleformManager::GetPrimaryTouchPoint()
{
  return m_iPrimaryTouchPoint;
}

void VScaleformManager::SetPrimaryTouchPoint(int iTouchPoint)
{
  m_iPrimaryTouchPoint = hkvMath::Min(iTouchPoint, s_iMaxTouchPoints-1); //we do accept negative values to disable the primary touch point
}

void VScaleformManager::HandleInput()
{
  for(int i = 0; i < s_iMaxTouchPoints; i++)
  {
    const bool bIsPrimary = i == m_iPrimaryTouchPoint;
    
    if(VInputManager::GetTouchScreen().IsActiveTouch(i))
    {
      const float fX = VInputManager::GetTouchScreen().GetTouchPointValue(i, CT_TOUCH_ABS_X, false);
      const float fY = VInputManager::GetTouchScreen().GetTouchPointValue(i, CT_TOUCH_ABS_Y, false);
      
      if((m_iButtonMask[i]&TOUCH_STARTED) == 0)
      {
        m_iButtonMask[i] |= TOUCH_STARTED;
        
        TouchEvent dragStartEvent(Event::TouchBegin, i, fX, fY, 0 /* width */ , 0 /* height */, bIsPrimary);
        HandleEvent(&dragStartEvent);
      }
      else
      { 
        //the 'begin event' has been sent
        if(m_pfCursorPosX[i] != fX || m_pfCursorPosY[i] != fY)
        {
          TouchEvent dragEvent(Event::TouchMove, i, fX, fY, 0 /* width */ , 0 /* height */, bIsPrimary);
          HandleEvent(&dragEvent);
          
          m_iButtonMask[i] |= TOUCH_DRAG;
        }
      }
      
      //remember mouse position and buttons as mask
      if(m_bSimulateMouseEvents)
      {
        SetCursorPos(fX, fY, 0, true, false, false, i);
      }
      else
      {
        m_pfCursorPosX[i] = fX;
        m_pfCursorPosY[i] = fY;
        m_iLastActiveCursor = i;
      }   
    }
    else if(m_iButtonMask[i]!=0)
    {
      //this code is useful to explicitly generate TAP events, SF however will generate them after a BEGIN-END event sequence
      //if((m_iButtonMask[i]&TOUCH_DRAG) == 0)
      //{
      //  TouchEvent tapEvent(Event::TouchTap, i, m_pfCursorPosX[i], m_pfCursorPosY[i], 0 /* width */ , 0 /* height */, bIsPrimary);
      //  HandleEvent(&tapEvent);
      //}
      
      TouchEvent dragEndEvent(Event::TouchEnd, i, m_pfCursorPosX[i], m_pfCursorPosY[i], 0 /* width */ , 0 /* height */, bIsPrimary);
      HandleEvent(&dragEndEvent);
      
      m_iButtonMask[i] &= ~(int)TOUCH_MASK;
      
      if(m_bSimulateMouseEvents)
        IncCursorPos(0, 0, 0, false, false, false, i);
      else
        m_iLastActiveCursor = i;
    }
  }
  
  HandleInputMap();
}

#elif defined(_VISION_WIN32)

void VScaleformManager::HandleInput()
{
  // Get input control values.
  const float fScroll = m_pCursorInputMap->GetTrigger(CURSOR_0_SCROLL) * SF_WHEEL_SENSITIVITY;
  const bool bLeftBtn = m_pCursorInputMap->GetTrigger(CURSOR_0_CLICK_LEFT) > 0.5f;
  const bool bRightBtn = m_pCursorInputMap->GetTrigger(CURSOR_0_CLICK_RIGHT) > 0.5f;
  const bool bMiddleBtn = m_pCursorInputMap->GetTrigger(CURSOR_0_CLICK_MIDDLE) > 0.5f;

  if (m_bUseAbsoluteCursorPosition || Vision::Editor.IsInEditor())
  {
    const float fX = static_cast<float>(V_MOUSE.GetRawControlValue(CT_MOUSE_ABS_X));
    const float fY = static_cast<float>(V_MOUSE.GetRawControlValue(CT_MOUSE_ABS_Y));
    SetCursorPos(fX, fY, fScroll, bLeftBtn, bMiddleBtn, bRightBtn);
  }
  else
  {
    const float fX = m_pCursorInputMap->GetTrigger(CURSOR_0_DELTA_X) * SF_CURSOR_SENSITIVITY;
    const float fY = m_pCursorInputMap->GetTrigger(CURSOR_0_DELTA_Y) * SF_CURSOR_SENSITIVITY;
    IncCursorPos(fX, fY, fScroll, bLeftBtn, bMiddleBtn, bRightBtn);
  }
  
  HandleInputMap();
}

#else

void VScaleformManager::HandleInput()
{
  float fX= m_pCursorInputMap->GetTrigger(CURSOR_0_DELTA_X) * SF_CURSOR_SENSITIVITY;
  float fY = m_pCursorInputMap->GetTrigger(CURSOR_0_DELTA_Y) * SF_CURSOR_SENSITIVITY;

  float fScroll = m_pCursorInputMap->GetTrigger(CURSOR_0_SCROLL) * SF_WHEEL_SENSITIVITY;
  bool bLeftBtn = m_pCursorInputMap->GetTrigger(CURSOR_0_CLICK_LEFT) > 0.5f;
  bool bRightBtn = m_pCursorInputMap->GetTrigger(CURSOR_0_CLICK_RIGHT) > 0.5f;
  bool bMiddleBtn = m_pCursorInputMap->GetTrigger(CURSOR_0_CLICK_MIDDLE) > 0.5f;

  IncCursorPos(fX, fY, fScroll, bLeftBtn, bMiddleBtn, bRightBtn);
  
  HandleInputMap();
}

#endif


//texture conversion
//==================

//conversion from vision texture to scaleform texture (required for render to texture)
#if defined(_VISION_WIN32)
  #if defined(_VR_DX9)
    Scaleform::Render::TextureImage * VScaleformManager::ConvertTexture(VTextureObject *pTexture)
    {
      VASSERT_MSG(pTexture, "You have to provide a valid texture!");
      ImageSize imageSize(pTexture->GetTextureWidth(), pTexture->GetTextureHeight());

      //access the texture manager
      D3D9::TextureManager * pManager = (D3D9::TextureManager*)m_pRenderHal->GetTextureManager();
      VASSERT_MSG(pManager!=NULL, "No texture manager present!");

      //check the type of th texture before casting
      IDirect3DBaseTexture9 *pBaseTexture = pTexture->GetD3DInterface();
      VASSERT_MSG(pBaseTexture->GetType()==D3DRTYPE_TEXTURE, "Base texture is of wrong type, cast will fail...");

      IDirect3DTexture9 *pTex = (IDirect3DTexture9 *)pBaseTexture;

      //create a scaleform representation
      Scaleform::Ptr<D3D9::Texture> spSfTexture = *(D3D9::Texture *)pManager->CreateTexture(pTex, imageSize);
      VASSERT_MSG(spSfTexture.GetPtr()!=NULL, "failed to create SF texture from resource!");
      return SF_NEW Scaleform::Render::TextureImage(Image_R8G8B8, spSfTexture->GetSize(), 0, spSfTexture);
    }
  #elif defined(_VR_DX11)
    Scaleform::Render::TextureImage * VScaleformManager::ConvertTexture(VTextureObject *pTexture)
    {
      VASSERT_MSG(pTexture, "You have to provide a valid texture!");
      ImageSize imageSize(pTexture->GetTextureWidth(), pTexture->GetTextureHeight());

      //access the texture manager
      D3D1x::TextureManager * pManager = (D3D1x::TextureManager*)m_pRenderHal->GetTextureManager();
      VASSERT_MSG(pManager!=NULL, "No texture manager present!");

      //check the type of th texture before casting
      D3DResource *pResource = pTexture->GetD3DResource(); //is a ID3D11Resource *
      VASSERT_MSG(pResource!=NULL, "Could not find a D3DResource representation of the texture!");

      #if defined(HK_DEBUG_SLOW)
        D3D11_RESOURCE_DIMENSION dim;
        pResource->GetType(&dim);

        VASSERT_MSG(dim == D3D11_RESOURCE_DIMENSION_TEXTURE2D, "Base texture is of wrong type, cast will fail...");
      #endif

      D3D11_TEXTURE2D_DESC desc;
      ID3D11Texture2D * pTextureDX11 = (ID3D11Texture2D *)pResource;

      pTextureDX11->GetDesc(&desc);

      //create a scaleform representation
      Scaleform::Ptr<D3D1x::Texture> spSfTexture = *(D3D1x::Texture *)pManager->CreateTexture(pTextureDX11, imageSize);
      VASSERT_MSG(spSfTexture.GetPtr()!=NULL, "failed to create SF texture from resource!");
      return SF_NEW TextureImage(Image_R8G8B8, spSfTexture->GetSize(), 0, spSfTexture);
    }
  #endif
#elif defined(_VISION_XENON)
  Scaleform::Render::TextureImage * VScaleformManager::ConvertTexture(VTextureObject *pTexture)
  {
    VASSERT_MSG(pTexture, "You have to provide a valid texture!");
    ImageSize imageSize(pTexture->GetTextureWidth(), pTexture->GetTextureHeight());

    //access the texture manager
    X360::TextureManager * pManager = (X360::TextureManager*)m_pRenderHal->GetTextureManager();
    VASSERT_MSG(pManager!=NULL, "No texture manager present!");

    //check the type of th texture before casting
    IDirect3DBaseTexture9 *pBaseTexture = pTexture->GetD3DInterface();
    VASSERT_MSG(pBaseTexture->GetType()==D3DRTYPE_TEXTURE, "Base texture is of wrong type, cast will fail...");

    //create a scaleform representation
    Scaleform::Ptr<X360::Texture> pSfTexture = *(X360::Texture *)pManager->CreateTexture((IDirect3DTexture9 *)pBaseTexture, imageSize);
    return SF_NEW TextureImage(Image_R8G8B8, pSfTexture->GetSize(), 0, pSfTexture);
  }
#elif defined(_VISION_PS3)
  Scaleform::Render::TextureImage * VScaleformManager::ConvertTexture(VTextureObject *pTexture)
  {
    VASSERT_MSG(pTexture, "You have to provide a valid texture!");
    ImageSize imageSize(pTexture->GetTextureWidth(), pTexture->GetTextureHeight());

    //access the texture manager
    PS3::TextureManager * pManager = (PS3::TextureManager*)m_pRenderHal->GetTextureManager();
    VASSERT_MSG(pManager!=NULL, "No texture manager present!");

    //check the type of th texture before casting
    CellGcmTexture* pGcmTexture = pTexture->GetGCMTexture();
    VASSERT_MSG(pGcmTexture!=NULL, "Could not grab GCM texture!");

    //create a scaleform representation
    Scaleform::Ptr<PS3::Texture> pSfTexture = *(PS3::Texture *)pManager->CreateTexture(*pGcmTexture, imageSize);
    return SF_NEW TextureImage(Image_R8G8B8, pSfTexture->GetSize(), 0, pSfTexture);
  }

#elif defined(_VR_GLES2)
  Scaleform::Render::TextureImage * VScaleformManager::ConvertTexture(VTextureObject *pTexture)
  {
    VASSERT_MSG(pTexture, "You have to provide a valid texture!");
    ImageSize imageSize(pTexture->GetTextureWidth(), pTexture->GetTextureHeight());

    //access the texture manager
    GL::TextureManager* pManager = static_cast<GL::TextureManager*>(m_pRenderHal->GetTextureManager());
    VASSERT_MSG(pManager!=NULL, "No texture manager present!");

    GLuint uiHandle = pTexture->m_GLHandle;
    VASSERT_MSG(uiHandle != 0, "Texture does not have a GL handle!");

    //create a scaleform representation
    bool bDeleteTexture = false;
    Scaleform::Ptr<GL::Texture> pSfTexture = *static_cast<GL::Texture *>(pManager->CreateTexture(uiHandle, bDeleteTexture, imageSize));
    return SF_NEW TextureImage(Image_R8G8B8, pSfTexture->GetSize(), 0, pSfTexture);
  }
#endif

VScaleformMovieExclusiveRenderLoop::VScaleformMovieExclusiveRenderLoop(VScaleformMovieInstance* pMovieInstance) :
  m_pMovieInstance(pMovieInstance)
{

}

void VScaleformMovieExclusiveRenderLoop::OnDoRenderLoop(void *pUserData)
{
  VisRenderContext_cl *pContext = Vision::Contexts.GetCurrentContext();
  int x1,y1,x2,y2;
  pContext->GetViewport(x1,y1,x2,y2);
  x2+=x1;y2+=y1; 

  // clear the screen by rendering a 2D quad
  IVRender2DInterface *pRI = Vision::RenderLoopHelper.BeginOverlayRendering();
  VSimpleRenderState_t state(VIS_TRANSP_NONE);
  pRI->DrawSolidQuad(hkvVec2((float)x1,(float)y1),hkvVec2((float)x2,(float)y2),VColorRef(0,0,0,255),state);
  Vision::RenderLoopHelper.EndOverlayRendering();

  VScaleformManager::GlobalManager().RenderMovies(&m_pMovieInstance, 1);
}

/*
 * Havok SDK - Base file, BUILD(#20140618)
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
