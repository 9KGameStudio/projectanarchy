/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// ***********************************************************************************************
// Accelerometer
// Copyright (C) Havok.com Inc. All rights reserved.
// ***********************************************************************************************
//
// This sample demonstrates the use of multi-touch and motion input.
//
// The multi-touch feature is used for controlling the camera. You can simultaneously move 
// the camera by using the virtual thumb-stick and adjust the orientation by scrolling over 
// the screen in view direction. Thereby the area of the virtual thumb-stick has a higher 
// priority than the area, which is assigned for the camera orientation.
//
// The motion-input (accelerometer) feature is used to move a ball through the maze
// scene. The ball will roll into the direction, to which the device is inclined.
//
// The multi-touch and motion input will only work on platforms that have support for this 
// kind of input. However, you have the possibility to use this sample on PC by using the 
// remote input feature. For that you can simply connect in the browser of your input device 
// (e.g. Android/ iOS) with the URL that is displayed in the top right corner of the screen.
// After that you can control the camera as well as the ball via your input device.
//
// It is recommended to disable auto-rotation on your input device, to avoid unintended screen
// rotations while playing the game.
// 
// ***********************************************************************************************

// TODO:
/*flags &= ~VSampleFlags::VSAMPLE_WIIU_DRCDEMO;
flags |= VSampleFlags::VSAMPLE_WIIU_DRCCOPY;*/

#include <Vision/Samples/Engine/Accelerometer/AccelerometerPCH.h>
#include <Vision/Samples/Engine/Accelerometer/GameState.hpp>
#include <Vision/Samples/Engine/Accelerometer/UpDownCamera.hpp>
#include <Vision/Samples/Engine/Accelerometer/BallEntity.hpp>

#include <Vision/Runtime/EnginePlugins/EnginePluginsImport.hpp>
#if defined(WIN32) 
#include <Vision/Runtime/EnginePlugins/RemoteInputEnginePlugin/IVRemoteInput.hpp>
#endif

#include <Vision/Runtime/Framework/VisionApp/VAppImpl.hpp>
#include <Vision/Runtime/Framework/VisionApp/Modules/VHelp.hpp>

// defines 
#define CAMERA_START_POSITION     hkvVec3(-100.f, 0.f, 800.f)
#define CAMERA_START_ORIENTATION  hkvVec3(0.0f, 90.0f, 0.0f)
#define BALL_START_POSITION       hkvVec3(392.0f, -207.0f, -237.0f)
#define OFFSET 25
#define TOUCHDEBUG VAPP_INPUT_CONTROL_LAST_ELEMENT + 10

class AccelerometerApp : public VAppImpl
{
public:
  AccelerometerApp() 
    : m_pGameStateEntity(NULL)
    , m_pCameraEntity(NULL)
    , m_pBallEntity(NULL)
    , m_vValue(hkvVec2::ZeroVector())
    , m_vDiff(hkvVec2::ZeroVector())
    , m_bRenderDebug(false)
    , m_pTouchScreen(NULL)
#if defined(SUPPORTS_MULTITOUCH)
    , m_pMotionInput(NULL)
#endif
#if defined(WIN32) 
    , m_pRemoteInput(NULL)
#endif
  {}
  virtual ~AccelerometerApp() {}

  virtual void Init() HKV_OVERRIDE
  {
    LoadScene(VisAppLoadSettings("Maze.vscene", ":havok_sdk/Data/Vision/Samples/Engine/Accelerometer"));
  }

  virtual void AfterEngineInit() HKV_OVERRIDE
  {
    VAppImpl::AfterEngineInit();

    VArray<const char*> helpText;
    helpText.Append("Rotate your device and try to get the ball onto the finish field.");

#if defined(WIN32) && !defined(_VISION_WINRT)
    helpText.Append("Connect with remote input device to the displayed Remote URL to play the game.");
    helpText.Append("Press Q to toggle touchpoint visual feedback");
#endif
    

    RegisterAppModule(new VHelp(helpText));
  }

  virtual void PreloadPlugins()
  {
#if defined(WIN32) 
    VISION_PLUGIN_ENSURE_LOADED(vRemoteInput);
#endif
    VISION_PLUGIN_ENSURE_LOADED(vHavok);
  }

  virtual void AfterSceneLoaded(bool bLoadingSuccessful) HKV_OVERRIDE;

  virtual void SetupAppConfig(VisAppConfig_cl& config) HKV_OVERRIDE
  {
#if defined(_VISION_WIIU)
    config.m_videoConfig.m_eMultiSample = VVIDEO_MULTISAMPLE_4TIMES;
#endif
  }

  virtual bool Run() HKV_OVERRIDE;

  virtual void DeInit() HKV_OVERRIDE;

private:
  GameState* m_pGameStateEntity;
  UpDownCamera* m_pCameraEntity;
  BallEntity* m_pBallEntity;

  hkvVec2 m_vValue;
  hkvVec2 m_vDiff;

  bool m_bRenderDebug;
  IVMultiTouchInput* m_pTouchScreen;

#if defined(SUPPORTS_MOTIONINPUT)
  VMotionInput* m_pMotionInput;
#endif
#if defined(WIN32) 
  IVRemoteInput* m_pRemoteInput;
#endif

  VDialogPtr m_spDialog;
};

void AccelerometerApp::AfterSceneLoaded(bool bLoadingSuccessful)
{
#if defined(WIN32) && !defined(_VISION_WINRT)
  m_pRemoteInput = (IVRemoteInput*)Vision::Plugins.GetRegisteredPlugin("vRemoteInput");
  m_pRemoteInput->StartServer("RemoteGui");
  m_pRemoteInput->AddVariable("ballXPos", 0.0f);
  m_pRemoteInput->AddVariable("ballYPos", 0.0f);
#endif

  // Create a game state entity
  FORCE_LINKDYNCLASS(GameState);
  m_pGameStateEntity = vstatic_cast<GameState*>(Vision::Game.CreateEntity("GameState", hkvVec3(), ""));

  // Create a camera entity
  FORCE_LINKDYNCLASS(UpDownCamera);
  m_pCameraEntity = vstatic_cast<UpDownCamera*>(Vision::Game.CreateEntity("UpDownCamera", CAMERA_START_POSITION));
  m_pCameraEntity->SetOrientation(CAMERA_START_ORIENTATION);

  // Create a ball entity
  FORCE_LINKDYNCLASS(BallEntity);
  m_pBallEntity = vstatic_cast<BallEntity*>(Vision::Game.CreateEntity("BallEntity", BALL_START_POSITION, "Ball.MODEL"));

#if defined(WIN32)
  GetInputMap()->MapTrigger(TOUCHDEBUG, V_PC_KEYBOARD, CT_KB_Q, VInputOptions::Once());
#elif defined(_VISION_PSP2)
  GetInputMap()->MapTrigger(TOUCHDEBUG, VInputManager::GetPad(0), CT_PAD_CROSS, VInputOptions::Once());
#endif

  // Try to setup pointer to touchscreen device
  IVInputDevice* inputDevice = &VInputManager::GetInputDevice(INPUT_DEVICE_TOUCHSCREEN);
  if (inputDevice != &VInputManager::s_NoInputDevice)
  {
    m_pTouchScreen = static_cast<IVMultiTouchInput*>(inputDevice);
  }

  // Enable motion input
#if defined(SUPPORTS_MOTIONINPUT)
  m_pMotionInput = static_cast<VMotionInput*>(&VInputManager::GetInputDevice(INPUT_DEVICE_MOTION_SENSOR));
  m_pMotionInput->SetEnabled(true);
#endif 

  VGUIManager::GlobalManager().LoadResourceFile("GUI/MenuSystem.xml");
}

bool AccelerometerApp::Run()
{
#if defined(WIN32) && !defined(_VISION_WINRT)
  m_pRemoteInput->DebugDrawTouchPoints(VColorRef(255,0,0));

  const hkvVec3 vBallPos = m_pBallEntity->GetPosition();
  m_pRemoteInput->UpdateVariable("ballXPos", vBallPos.x);
  m_pRemoteInput->UpdateVariable("ballYPos", vBallPos.y);
#endif

  if (m_pGameStateEntity->GetCurrentState() == GAME_STATE_RUN)
  {
#if defined(SUPPORTS_MOTIONINPUT) || defined(_VISION_WIIU)
    hkvVec3 vAcceleration;

#if defined(_VISION_WIIU)
    vAcceleration.x = -VInputManager::GetDRC(V_DRC_FIRST).GetControlValue(CT_PAD_WIIU_ACCELERATION_X, 0.0f);
    vAcceleration.y =  VInputManager::GetDRC(V_DRC_FIRST).GetControlValue(CT_PAD_WIIU_ACCELERATION_Z, 0.0f);
    vAcceleration.z =  VInputManager::GetDRC(V_DRC_FIRST).GetControlValue(CT_PAD_WIIU_ACCELERATION_Y, 0.0f);
#else
    vAcceleration = m_pMotionInput->GetAcceleration();
#endif

    hkvVec2 vMult;
    vMult.x = vAcceleration.x * fabs( vAcceleration.x );
    vMult.y = vAcceleration.y * fabs( vAcceleration.y );
    vMult *= BALL_MASS;

    hkvMat3 mat(hkvNoInitialization);
    m_pCameraEntity->GetRotationMatrix(mat);

    hkvVec3 vRight = -mat.getColumn( 1 );
    hkvVec3 vUp = mat.getColumn( 2 );

    hkvVec3 vForce = (vUp * vMult.y) + (vRight * vMult.x);
    vForce *= 100.0f / 60.0f;
    m_pBallEntity->ApplyForce(vForce, Vision::GetTimer()->GetTimeDifference());
#endif
  }
  else if (m_pGameStateEntity->GetCurrentState() == GAME_STATE_RESTART)
  {
    if (m_spDialog == NULL)
    {
      m_spDialog = GetContext()->ShowDialog("GUI/MessageBox.xml");
      m_pCameraEntity->SetThinkFunctionStatus(FALSE);
    }
    else if (m_spDialog->GetDialogResult() == VGUIManager::ID_YES)
    {
      GetContext()->CloseDialog(m_spDialog);
      m_spDialog = NULL;

      m_pGameStateEntity->SetCurrentState(GAME_STATE_RUN);
      m_pBallEntity->SetPosition(BALL_START_POSITION);
      m_pCameraEntity->SetThinkFunctionStatus(TRUE);
    }
    else if (m_spDialog->GetDialogResult() != 0)
    {
      return false;
    }
  }

#if defined(WIN32) && !defined(_VISION_WINRT)
  if (VInputManagerPC::GetTouchScreen().IsActive() && VAppImpl::GetInputMap()->GetTrigger(TOUCHDEBUG))
  {
    m_bRenderDebug = !m_bRenderDebug;
  }

#elif defined(_VISION_PSP2)
  if (VAppImpl::GetInputMap()->GetTrigger(TOUCHDEBUG))
  {
    m_bRenderDebug = !m_bRenderDebug;
  }
  //Vision::Game.GetDebugRenderInterface()->DrawText2D(10, 10, "Press Cross to toggle touchpoint visual feedback", V_RGBA_WHITE);

#endif

  // Touch point visual feedback (if enabled)
  if(m_bRenderDebug && (m_pTouchScreen != NULL))
  {        
    int count = m_pTouchScreen->GetMaximumNumberOfTouchPoints();
    for(int i = 0; i < count; i++)
    {
      if(!m_pTouchScreen->IsActiveTouch(i))
        continue;

      // Get touch point values and deltas
      m_vValue.x = m_pTouchScreen->GetTouchPointValue(i, CT_TOUCH_ABS_X);
      m_vValue.y = m_pTouchScreen->GetTouchPointValue(i, CT_TOUCH_ABS_Y);
      m_vDiff.x = m_pTouchScreen->GetTouchPointValue(i, CT_TOUCH_ABS_DELTA_X);
      m_vDiff.y = m_pTouchScreen->GetTouchPointValue(i, CT_TOUCH_ABS_DELTA_Y);

      // Draw Cross hair
      Vision::Game.DrawSingleLine2D(m_vValue.x - OFFSET, m_vValue.y - OFFSET, m_vValue.x + OFFSET, m_vValue.y - OFFSET, V_RGBA_WHITE, 3.f);
      Vision::Game.DrawSingleLine2D(m_vValue.x - OFFSET, m_vValue.y + OFFSET, m_vValue.x + OFFSET, m_vValue.y + OFFSET, V_RGBA_WHITE, 3.f);
      Vision::Game.DrawSingleLine2D(m_vValue.x - OFFSET, m_vValue.y + OFFSET, m_vValue.x - OFFSET, m_vValue.y - OFFSET, V_RGBA_WHITE, 3.f);
      Vision::Game.DrawSingleLine2D(m_vValue.x + OFFSET, m_vValue.y + OFFSET, m_vValue.x + OFFSET, m_vValue.y - OFFSET, V_RGBA_WHITE, 3.f);
      Vision::Game.DrawSingleLine2D(m_vValue.x - OFFSET, m_vValue.y, m_vValue.x + OFFSET, m_vValue.y, V_RGBA_WHITE, 3.f);
      Vision::Game.DrawSingleLine2D(m_vValue.x, m_vValue.y + OFFSET, m_vValue.x, m_vValue.y - OFFSET, V_RGBA_WHITE, 3.f);
      
      // Draw delta vector
      Vision::Game.DrawSingleLine2D(m_vValue.x, m_vValue.y, m_vValue.x + m_vDiff.x * 100.f, m_vValue.y + m_vDiff.y * 100.f, V_RGBA_WHITE, 3.f);

      const int BufferSize = 30;
      char debugID[BufferSize];
      char debugX[BufferSize];
      char debugY[BufferSize];

      // Print ID & position for every active touchpoint          
      vis_snprintf(debugID, BufferSize, "ID: %i", i);
      Vision::Game.GetDebugRenderInterface()->DrawText2D(m_vValue.x + 30, m_vValue.y - 30, debugID, V_RGBA_WHITE);
      vis_snprintf(debugX, BufferSize, "X:  %.0f", m_vValue.x);
      Vision::Game.GetDebugRenderInterface()->DrawText2D(m_vValue.x + 30, m_vValue.y - 20, debugX, V_RGBA_WHITE);
      vis_snprintf(debugY, BufferSize, "Y:  %.0f", m_vValue.y);
      Vision::Game.GetDebugRenderInterface()->DrawText2D(m_vValue.x + 30, m_vValue.y - 10, debugY, V_RGBA_WHITE);
    }
  }

  return true;
}

void AccelerometerApp::DeInit()
{
  m_pGameStateEntity = NULL;
  m_pCameraEntity = NULL;
  m_pBallEntity = NULL;
  m_pTouchScreen = NULL;

#if defined(SUPPORTS_MOTIONINPUT)
  m_pMotionInput = NULL;
#endif
#if defined(WIN32) 
  m_pRemoteInput = NULL;
#endif
}

VAPP_IMPLEMENT_SAMPLE(AccelerometerApp);

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
