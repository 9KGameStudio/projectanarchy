/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Input/VFreeCamera.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Input/VVirtualThumbStick.hpp>

#define CAMERA_CONTROL_ALTERNATIVES 20
#define CAMERA_CONTROL_USER_SPACE 16

V_IMPLEMENT_SERIAL(VFreeCamera, VisBaseEntity_cl, 0, &g_VisionEngineModule);

VFreeCamera::VFreeCamera()
  : m_fSensitivity(200.0f)
  , m_bWASDEnabled(false)
  , m_bActive(false)
  , m_pInputMap(NULL)
#if defined(SUPPORTS_MULTITOUCH)
  , m_pVirtualThumbStick(NULL)
  , m_spLookTouchArea(NULL)
#endif
{
  m_fMoveSpeed = 350.0f * Vision::World.GetGlobalUnitScaling();
  ClearWASDAlternativeIndices();
}

VFreeCamera::~VFreeCamera()
{
}

VFreeCamera* VFreeCamera::Create(const char* szCameraRefObjectKey, float fMoveSpeed)
{
  VFreeCamera* pCamera = Vision::Game.CreateEntity<VFreeCamera>(hkvVec3::ZeroVector());
  if (pCamera != NULL)
  {
    if (szCameraRefObjectKey != NULL)
    {
      VisBaseEntity_cl* pCameraRef = Vision::Game.SearchEntity(szCameraRefObjectKey);
      if (pCameraRef != NULL)
      {
        pCamera->SetPosition(pCameraRef->GetPosition());
        pCamera->SetOrientation(pCameraRef->GetOrientation());
      }
    }

    if (fMoveSpeed >= 0.0f)
      pCamera->SetMoveSpeed(fMoveSpeed);
  }

  return pCamera;
}

void VFreeCamera::InitFunction()
{
  BaseInit();

  VisBaseEntity_cl::InitFunction();
}

void VFreeCamera::DeInitFunction()
{
  VisBaseEntity_cl::DeInitFunction();

  // Deregister from callbacks.
  SetThinkFunctionStatus(FALSE);

  V_SAFE_DELETE(m_pInputMap);
  ClearWASDAlternativeIndices();

#if defined(SUPPORTS_MULTITOUCH)
  if (m_pVirtualThumbStick != NULL)
  {
    delete m_pVirtualThumbStick;
    m_pVirtualThumbStick = NULL;
    Vision::Callbacks.OnVideoChanged -= this;
  }

  m_spLookTouchArea = NULL;
#endif
}

void VFreeCamera::BaseInit()
{
  m_pInputMap = new VInputMap(CONTROL_LAST_ELEMENT + 1 + CAMERA_CONTROL_USER_SPACE, CAMERA_CONTROL_ALTERNATIVES);

#if defined(_VISION_XENON)

  m_pInputMap->MapTrigger(CONTROL_MOVE_FORWARD,  VInputManager::GetPad(0), CT_PAD_LEFT_THUMB_STICK_UP,    VInputOptions::DeadZone(0.25f));
  m_pInputMap->MapTrigger(CONTROL_MOVE_BACKWARD, VInputManager::GetPad(0), CT_PAD_LEFT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.25f));
  m_pInputMap->MapTrigger(CONTROL_MOVE_LEFT,     VInputManager::GetPad(0), CT_PAD_LEFT_THUMB_STICK_LEFT,  VInputOptions::DeadZone(0.25f));
  m_pInputMap->MapTrigger(CONTROL_MOVE_RIGHT,    VInputManager::GetPad(0), CT_PAD_LEFT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.25f));

  m_pInputMap->MapTrigger(CONTROL_ANY_ACTION,   VInputManager::GetPad(0), CT_PAD_ANY_KEY);
  m_pInputMap->MapTrigger(CONTROL_ANY_ACTION,   VInputManager::GetPad(0), CT_PAD_LEFT_THUMB_STICK_CHANGED);

  m_pInputMap->MapTriggerAxis(CONTROL_HORIZONTAL_LOOK, VInputManager::GetPad(0), CT_PAD_RIGHT_THUMB_STICK_LEFT, CT_PAD_RIGHT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.15f, true));
  m_pInputMap->MapTriggerAxis(CONTROL_VERTICAL_LOOK,   VInputManager::GetPad(0), CT_PAD_RIGHT_THUMB_STICK_UP,   CT_PAD_RIGHT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.15f, true));

  m_pInputMap->MapTrigger(CONTROL_SPEED_FAST, VInputManager::GetPad(0), CT_PAD_RIGHT_SHOULDER, VInputOptions::DeadZone(0.25f));
  m_pInputMap->MapTrigger(CONTROL_SPEED_FASTER, VInputManager::GetPad(0), CT_PAD_RIGHT_THUMB);

#endif

#if defined(_VISION_PS3)
  
  m_pInputMap->MapTrigger(CONTROL_MOVE_FORWARD,  VInputManager::GetPad(0), CT_PAD_LEFT_THUMB_STICK_UP,    VInputOptions::DeadZone(0.25f));
  m_pInputMap->MapTrigger(CONTROL_MOVE_BACKWARD, VInputManager::GetPad(0), CT_PAD_LEFT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.25f));
  m_pInputMap->MapTrigger(CONTROL_MOVE_LEFT,     VInputManager::GetPad(0), CT_PAD_LEFT_THUMB_STICK_LEFT,  VInputOptions::DeadZone(0.25f));
  m_pInputMap->MapTrigger(CONTROL_MOVE_RIGHT,    VInputManager::GetPad(0), CT_PAD_LEFT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.25f));

  m_pInputMap->MapTrigger(CONTROL_ANY_ACTION,    VInputManager::GetPad(0), CT_PAD_ANY_KEY);
  m_pInputMap->MapTrigger(CONTROL_ANY_ACTION,    VInputManager::GetPad(0), CT_PAD_LEFT_THUMB_STICK_CHANGED);
  m_pInputMap->MapTrigger(CONTROL_ANY_ACTION,    VInputManager::GetPad(0), CT_PAD_RIGHT_THUMB_STICK_CHANGED);

  m_pInputMap->MapTriggerAxis(CONTROL_HORIZONTAL_LOOK, VInputManager::GetPad(0), CT_PAD_RIGHT_THUMB_STICK_LEFT, CT_PAD_RIGHT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.15f, true));
  m_pInputMap->MapTriggerAxis(CONTROL_VERTICAL_LOOK,   VInputManager::GetPad(0), CT_PAD_RIGHT_THUMB_STICK_UP,   CT_PAD_RIGHT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.15f, true));

  m_pInputMap->MapTrigger(CONTROL_SPEED_FAST, VInputManager::GetPad(0), CT_PAD_RIGHT_SHOULDER, VInputOptions::DeadZone(0.25f));
  m_pInputMap->MapTrigger(CONTROL_SPEED_FASTER, VInputManager::GetPad(0), CT_PAD_RIGHT_THUMB);

#endif

#if defined(_VISION_PSP2)

  m_pInputMap->MapTrigger(CONTROL_MOVE_FORWARD,  VInputManager::GetPad(0), CT_PAD_LEFT_THUMB_STICK_UP, VInputOptions::DeadZone(0.25f));
  m_pInputMap->MapTrigger(CONTROL_MOVE_BACKWARD, VInputManager::GetPad(0), CT_PAD_LEFT_THUMB_STICK_DOWN, VInputOptions::DeadZone(0.25f));
  m_pInputMap->MapTrigger(CONTROL_MOVE_LEFT,     VInputManager::GetPad(0), CT_PAD_LEFT_THUMB_STICK_LEFT, VInputOptions::DeadZone(0.25f));
  m_pInputMap->MapTrigger(CONTROL_MOVE_RIGHT,    VInputManager::GetPad(0), CT_PAD_LEFT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.25f));

  m_pInputMap->MapTrigger(CONTROL_ANY_ACTION,    VInputManager::GetPad(0), CT_PAD_ANY_KEY);
  m_pInputMap->MapTrigger(CONTROL_ANY_ACTION,    VInputManager::GetPad(0), CT_PAD_LEFT_THUMB_STICK_CHANGED);
  m_pInputMap->MapTrigger(CONTROL_ANY_ACTION,    VInputManager::GetPad(0), CT_PAD_RIGHT_THUMB_STICK_CHANGED);

  m_pInputMap->MapTriggerAxis(CONTROL_HORIZONTAL_LOOK, VInputManager::GetPad(0), CT_PAD_RIGHT_THUMB_STICK_LEFT, CT_PAD_RIGHT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.15f, true));
  m_pInputMap->MapTriggerAxis(CONTROL_VERTICAL_LOOK,   VInputManager::GetPad(0), CT_PAD_RIGHT_THUMB_STICK_UP,   CT_PAD_RIGHT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.15f, true));

  m_pInputMap->MapTrigger(CONTROL_SPEED_FASTER,   VInputManager::GetPad(0), CT_PAD_RIGHT_SHOULDER, VInputOptions::DeadZone(0.25f));

#endif

#if defined(SUPPORTS_KEYBOARD)

  m_pInputMap->MapTrigger(CONTROL_MOVE_FORWARD,  V_KEYBOARD, CT_KB_UP);
  m_pInputMap->MapTrigger(CONTROL_MOVE_BACKWARD, V_KEYBOARD, CT_KB_DOWN);
  m_pInputMap->MapTrigger(CONTROL_MOVE_LEFT,     V_KEYBOARD, CT_KB_LEFT);
  m_pInputMap->MapTrigger(CONTROL_MOVE_RIGHT,    V_KEYBOARD, CT_KB_RIGHT);

  m_pInputMap->MapTrigger(CONTROL_ANY_ACTION,    V_KEYBOARD, CT_KB_UNKNOWN);

#endif

#if defined(SUPPORTS_MOUSE)
  
  m_pInputMap->MapTrigger(CONTROL_HORIZONTAL_LOOK, V_MOUSE, CT_MOUSE_ABS_DELTA_X, VInputOptions::Sensitivity(0.15f / Vision::Video.GetDeviceDpi()));
  m_pInputMap->MapTrigger(CONTROL_VERTICAL_LOOK,   V_MOUSE, CT_MOUSE_ABS_DELTA_Y, VInputOptions::Sensitivity(0.15f / Vision::Video.GetDeviceDpi()));

  m_pInputMap->MapTrigger(CONTROL_SPEED_FAST, V_MOUSE, CT_MOUSE_RIGHT_BUTTON);
  m_pInputMap->MapTrigger(CONTROL_SPEED_FASTER, V_MOUSE, CT_MOUSE_MIDDLE_BUTTON);
#endif

#if defined(_VISION_WIIU)

  // DRC mapping
  m_pInputMap->MapTrigger(CONTROL_MOVE_FORWARD,  VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_THUMB_STICK_UP,    VInputOptions::DeadZone(0.25f));
  m_pInputMap->MapTrigger(CONTROL_MOVE_BACKWARD, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.25f));
  m_pInputMap->MapTrigger(CONTROL_MOVE_LEFT,     VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_THUMB_STICK_LEFT,  VInputOptions::DeadZone(0.25f));
  m_pInputMap->MapTrigger(CONTROL_MOVE_RIGHT,    VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.25f));

  m_pInputMap->MapTrigger(CONTROL_ANY_ACTION,    VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_ANY_KEY);
  m_pInputMap->MapTrigger(CONTROL_ANY_ACTION,    VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_THUMB_STICK_CHANGED);
  m_pInputMap->MapTrigger(CONTROL_ANY_ACTION,    VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_RIGHT_THUMB_STICK_CHANGED);

  m_pInputMap->MapTriggerAxis(CONTROL_HORIZONTAL_LOOK, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_RIGHT_THUMB_STICK_LEFT, CT_PAD_RIGHT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.15f, true));
  m_pInputMap->MapTriggerAxis(CONTROL_VERTICAL_LOOK,   VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_RIGHT_THUMB_STICK_UP,   CT_PAD_RIGHT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.15f, true));

  m_pInputMap->MapTrigger(CONTROL_SPEED_FAST, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_RIGHT_SHOULDER, VInputOptions::DeadZone(0.25f));
  m_pInputMap->MapTrigger(CONTROL_SPEED_FASTER, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_SHOULDER,  VInputOptions::DeadZone(0.4f));

  // Pro controller mapping
  m_pInputMap->MapTrigger(CONTROL_MOVE_FORWARD,  VInputManagerWiiU::GetRemote(0), CT_PAD_LEFT_THUMB_STICK_UP,    VInputOptions::DeadZone(0.25f));
  m_pInputMap->MapTrigger(CONTROL_MOVE_BACKWARD, VInputManagerWiiU::GetRemote(0), CT_PAD_LEFT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.25f));
  m_pInputMap->MapTrigger(CONTROL_MOVE_LEFT,     VInputManagerWiiU::GetRemote(0), CT_PAD_LEFT_THUMB_STICK_LEFT,  VInputOptions::DeadZone(0.25f));
  m_pInputMap->MapTrigger(CONTROL_MOVE_RIGHT,    VInputManagerWiiU::GetRemote(0), CT_PAD_LEFT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.25f)); 

  m_pInputMap->MapTrigger(CONTROL_ANY_ACTION,    VInputManagerWiiU::GetRemote(0), CT_PAD_ANY_KEY);
  m_pInputMap->MapTrigger(CONTROL_ANY_ACTION,    VInputManagerWiiU::GetRemote(0), CT_PAD_LEFT_THUMB_STICK_CHANGED);
  m_pInputMap->MapTrigger(CONTROL_ANY_ACTION,    VInputManagerWiiU::GetRemote(0), CT_PAD_RIGHT_THUMB_STICK_CHANGED);

  m_pInputMap->MapTriggerAxis(CONTROL_HORIZONTAL_LOOK, VInputManagerWiiU::GetRemote(0), CT_PAD_RIGHT_THUMB_STICK_LEFT, CT_PAD_RIGHT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.05f, true));
  m_pInputMap->MapTriggerAxis(CONTROL_VERTICAL_LOOK,   VInputManagerWiiU::GetRemote(0), CT_PAD_RIGHT_THUMB_STICK_UP,   CT_PAD_RIGHT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.05f, true));

  m_pInputMap->MapTrigger(CONTROL_SPEED_FAST, VInputManagerWiiU::GetRemote(0), CT_PAD_RIGHT_SHOULDER, VInputOptions::DeadZone(0.1f));
  m_pInputMap->MapTrigger(CONTROL_SPEED_FASTER, VInputManagerWiiU::GetRemote(0), CT_PAD_LEFT_SHOULDER,  VInputOptions::DeadZone(0.1f));

#endif

  // Additional input possibilities for devices with gamepad controls.
#if defined(_VISION_ANDROID) 

  m_pInputMap->MapTrigger(CONTROL_MOVE_FORWARD,  VInputManager::GetPad(0), CT_PAD_LEFT_THUMB_STICK_UP,    VInputOptions::DeadZone(0.1f, false, 1));
  m_pInputMap->MapTrigger(CONTROL_MOVE_BACKWARD, VInputManager::GetPad(0), CT_PAD_LEFT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.1f, false, 1));
  m_pInputMap->MapTrigger(CONTROL_MOVE_LEFT,     VInputManager::GetPad(0), CT_PAD_LEFT_THUMB_STICK_LEFT,  VInputOptions::DeadZone(0.1f, false, 1));
  m_pInputMap->MapTrigger(CONTROL_MOVE_RIGHT,    VInputManager::GetPad(0), CT_PAD_LEFT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.1f, false, 1));

  m_pInputMap->MapTriggerAxis(CONTROL_HORIZONTAL_LOOK, VInputManager::GetPad(0), CT_PAD_RIGHT_THUMB_STICK_LEFT, CT_PAD_RIGHT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.1f, true, 1));
  m_pInputMap->MapTriggerAxis(CONTROL_VERTICAL_LOOK, VInputManager::GetPad(0), CT_PAD_RIGHT_THUMB_STICK_UP, CT_PAD_RIGHT_THUMB_STICK_DOWN, VInputOptions::DeadZone(0.1f, true, 1));

  m_pInputMap->MapTrigger(CONTROL_SPEED_FAST, VInputManager::GetPad(0), CT_PAD_RIGHT_SHOULDER, VInputOptions::DeadZone(0.1f));
  m_pInputMap->MapTrigger(CONTROL_SPEED_FASTER, VInputManager::GetPad(0), CT_PAD_LEFT_SHOULDER,  VInputOptions::DeadZone(0.1f));

  // Also map D-pad on Android since analog sticks might not be present.
  m_pInputMap->MapTrigger(CONTROL_MOVE_FORWARD,  VInputManager::GetPad(0), CT_PAD_UP,    VInputOptions::Alternative(2));
  m_pInputMap->MapTrigger(CONTROL_MOVE_BACKWARD, VInputManager::GetPad(0), CT_PAD_DOWN,  VInputOptions::Alternative(2));
  m_pInputMap->MapTrigger(CONTROL_MOVE_LEFT,     VInputManager::GetPad(0), CT_PAD_LEFT,  VInputOptions::Alternative(2));
  m_pInputMap->MapTrigger(CONTROL_MOVE_RIGHT,    VInputManager::GetPad(0), CT_PAD_RIGHT, VInputOptions::Alternative(2));

#endif

#if defined(SUPPORTS_MULTITOUCH) && (defined(_VISION_MOBILE) || defined(_VISION_WINRT))
  VASSERT(m_pVirtualThumbStick == NULL);
  CreateVirtualThumbStick();
#endif

  // WASD is enabled by default.
  SetWASDControlsEnabled(true);

  // Initialize active state and attach camera to entity.
  OnThinkFunctionStatusChanged();
}

void VFreeCamera::SetMoveSpeed(const float fMoveSpeed)
{ 
  m_fMoveSpeed = hkvMath::Max(0.0f, fMoveSpeed);

#if defined (SUPPORTS_MULTITOUCH)
  UpdateVirtualThumbstickVisibility();
#endif
}

#if defined (SUPPORTS_MULTITOUCH)

void VFreeCamera::CreateVirtualThumbStick()
{
  if (m_pVirtualThumbStick != NULL)
    return;

  VASSERT_MSG(m_pInputMap != NULL, "Make sure to initialize the free camera before creating the virtual thumb stick.");

  // Virtual Thumb Stick
  m_pVirtualThumbStick = new VVirtualThumbStick();
  UpdateVirtualThumbstickVisibility();

  Vision::Callbacks.OnVideoChanged += this; // update valid area when resolution changes

  m_pInputMap->MapTrigger(CONTROL_MOVE_LEFT, *m_pVirtualThumbStick, CT_PAD_LEFT_THUMB_STICK_LEFT, VInputOptions::DeadZone(0.2f));
  m_pInputMap->MapTrigger(CONTROL_MOVE_RIGHT, *m_pVirtualThumbStick, CT_PAD_LEFT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.2f));
  m_pInputMap->MapTrigger(CONTROL_MOVE_FORWARD, *m_pVirtualThumbStick, CT_PAD_LEFT_THUMB_STICK_UP, VInputOptions::DeadZone(0.2f));
  m_pInputMap->MapTrigger(CONTROL_MOVE_BACKWARD, *m_pVirtualThumbStick, CT_PAD_LEFT_THUMB_STICK_DOWN, VInputOptions::DeadZone(0.2f));

  // Look Touch Area
  VASSERT(m_spLookTouchArea == NULL);
  m_spLookTouchArea = new VTouchArea(VInputManager::GetTouchScreen(), VRectanglef(), -2000.0f);
  m_pInputMap->MapTrigger(CONTROL_HORIZONTAL_LOOK, m_spLookTouchArea, CT_TOUCH_ABS_DELTA_X, VInputOptions::Sensitivity(0.25f / Vision::Video.GetDeviceDpi()));
  m_pInputMap->MapTrigger(CONTROL_VERTICAL_LOOK,   m_spLookTouchArea, CT_TOUCH_ABS_DELTA_Y, VInputOptions::Sensitivity(0.25f / Vision::Video.GetDeviceDpi()));

  m_pInputMap->MapTrigger(CONTROL_TAP_X, m_spLookTouchArea, CT_TOUCH_TAP_X);
  m_pInputMap->MapTrigger(CONTROL_TAP_Y, m_spLookTouchArea, CT_TOUCH_TAP_Y);
}

#endif

void VFreeCamera::SetWASDControlsEnabled(bool bEnabled)
{
  if (m_bWASDEnabled == bEnabled)
    return;

  m_bWASDEnabled = bEnabled;

#if defined(SUPPORTS_KEYBOARD)

  if (bEnabled)
  {
    m_iWASDAlternativeIndices[0] = m_pInputMap->MapTrigger(CONTROL_MOVE_FORWARD,  V_KEYBOARD, CT_KB_W);
    m_iWASDAlternativeIndices[1] = m_pInputMap->MapTrigger(CONTROL_MOVE_BACKWARD, V_KEYBOARD, CT_KB_S);
    m_iWASDAlternativeIndices[2] = m_pInputMap->MapTrigger(CONTROL_MOVE_LEFT,     V_KEYBOARD, CT_KB_A);
    m_iWASDAlternativeIndices[3] = m_pInputMap->MapTrigger(CONTROL_MOVE_RIGHT,    V_KEYBOARD, CT_KB_D);
    m_iWASDAlternativeIndices[4] = m_pInputMap->MapTrigger(CONTROL_MOVE_UP,       V_KEYBOARD, CT_KB_E);
    m_iWASDAlternativeIndices[5] = m_pInputMap->MapTrigger(CONTROL_MOVE_DOWN,     V_KEYBOARD, CT_KB_Q);
  }
  else
  {
    m_pInputMap->UnmapInput(CONTROL_MOVE_FORWARD,  m_iWASDAlternativeIndices[0]);
    m_pInputMap->UnmapInput(CONTROL_MOVE_BACKWARD, m_iWASDAlternativeIndices[1]);
    m_pInputMap->UnmapInput(CONTROL_MOVE_LEFT,     m_iWASDAlternativeIndices[2]);
    m_pInputMap->UnmapInput(CONTROL_MOVE_RIGHT,    m_iWASDAlternativeIndices[3]);
    m_pInputMap->UnmapInput(CONTROL_MOVE_UP,       m_iWASDAlternativeIndices[4]);
    m_pInputMap->UnmapInput(CONTROL_MOVE_DOWN,     m_iWASDAlternativeIndices[5]);

    ClearWASDAlternativeIndices();
  }

#endif
}

void VFreeCamera::ClearWASDAlternativeIndices()
{
  for (int i = 0; i < V_ARRAY_SIZE(m_iWASDAlternativeIndices); i++)
  {
    m_iWASDAlternativeIndices[i] = -1;
  }
}

void VFreeCamera::OnThinkFunctionStatusChanged()
{
  // Do nothing when in editor and not playing the game.
  if (!Vision::Editor.IsPlayingTheGame())
    return;

  if (GetThinkFunctionStatus() == TRUE)
  {
    // Attach camera to entity.
    Vision::Camera.AttachToEntity(this, hkvVec3::ZeroVector());
  }

  UpdateActiveState();
}

void VFreeCamera::OnVariableValueChanged(VisVariable_cl* pVar, const char* value)
{
  if (strcmp(pVar->GetName(), "m_fMoveSpeed") == 0)
  {
    SetMoveSpeed(m_fMoveSpeed);
  }
}

void VFreeCamera::ModSysAddChild(class VisModuleSystemChild_cl *child)
{
  VisBaseEntity_cl::ModSysAddChild(child);

  UpdateActiveState();
}

void VFreeCamera::ModSysRemoveChild(class VisModuleSystemChild_cl *child, BOOL deleteChild)
{
  VisBaseEntity_cl::ModSysRemoveChild(child, deleteChild);

  UpdateActiveState();
}

void VFreeCamera::UpdateActiveState()
{
  // Check if active state changed.
  const bool bActive = (GetThinkFunctionStatus() == TRUE) && (Vision::Camera.GetMainCamera()->GetParent() == this);
  if (bActive == m_bActive)
    return;
  m_bActive = bActive;

  if (m_bActive)
  {
    Vision::Callbacks.OnFrameUpdatePreRender += this;
  }
  else
  {
    Vision::Callbacks.OnFrameUpdatePreRender -= this;
  }

#if defined(SUPPORTS_MULTITOUCH)
  UpdateVirtualThumbstickVisibility();
#endif

  // Notify derived class.
  OnActiveStateChanged(m_bActive);
}

#if defined(SUPPORTS_MULTITOUCH)

void VFreeCamera::UpdateVirtualThumbstickVisibility()
{
  if (m_pVirtualThumbStick == NULL)
    return;

  // Disable / Enable virtual thumb stick.
  m_pVirtualThumbStick->Show(m_bActive && m_fMoveSpeed != 0.0f);
}

#endif

void VFreeCamera::ProcessInput(float fTimeDiff)
{
  if (!m_bActive)
    return;

  hkvVec3 vMoveDelta = hkvVec3::ZeroVector();

  // Handle movement.
  hkvVec3 vForward(hkvNoInitialization), vRight(hkvNoInitialization), vUp(hkvNoInitialization);
  GetCurrentMoveAxes(vForward, vRight, vUp);

  float fMaxSpeed = m_fMoveSpeed;
  if (m_pInputMap->GetTrigger(CONTROL_SPEED_FAST))
    fMaxSpeed *= 3.0f;
  else if (m_pInputMap->GetTrigger(CONTROL_SPEED_FASTER))
    fMaxSpeed *= 9.0f;

  // Accumulate move directions (multiply in order to take analog input into account).
  vMoveDelta += vForward * m_pInputMap->GetTrigger(CONTROL_MOVE_FORWARD);
  vMoveDelta -= vForward * m_pInputMap->GetTrigger(CONTROL_MOVE_BACKWARD);
  vMoveDelta -= vRight * m_pInputMap->GetTrigger(CONTROL_MOVE_RIGHT);
  vMoveDelta += vRight * m_pInputMap->GetTrigger(CONTROL_MOVE_LEFT);
  vMoveDelta += vUp *  m_pInputMap->GetTrigger(CONTROL_MOVE_UP);
  vMoveDelta -= vUp * m_pInputMap->GetTrigger(CONTROL_MOVE_DOWN);
  vMoveDelta *= fMaxSpeed;
   
  // Clamp movement, so that moving diagonally is not faster than moving straight when using digital input.
  const float fSpeed = vMoveDelta.getLength();
  if (fSpeed > fMaxSpeed)
    vMoveDelta.setLength(fMaxSpeed);
  vMoveDelta *= fTimeDiff;

  // Look around.
  const float dx = m_pInputMap->GetTrigger(CONTROL_HORIZONTAL_LOOK);
  const float dy = m_pInputMap->GetTrigger(CONTROL_VERTICAL_LOOK);

  hkvVec3 vOrientation = GetOrientation();
  vOrientation.x += -dx * m_fSensitivity;
  vOrientation.y = hkvMath::clamp(vOrientation.y + dy * m_fSensitivity, -89.5f, 89.5f);
  SetOrientation(vOrientation);

  // Apply delta.
  if (GetPhysicsObject() != NULL)
  {
    IncMotionDeltaWorldSpace(vMoveDelta);
  }
  else
  {
    IncPosition(vMoveDelta);
  }
}

void VFreeCamera::GetCurrentMoveAxes(hkvVec3& vForward, hkvVec3& vRight, hkvVec3& vUp) const
{
  vUp.set(0.0f, 0.0f, 1.0f);

  hkvMat3 mat(hkvNoInitialization);
  GetRotationMatrix(mat);
  vForward = mat.getAxis(0);
  vRight = mat.getAxis(1);
}

void VFreeCamera::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  // Input is processed once per frame, not once per simulation tick.
  if (pData->m_pSender == &Vision::Callbacks.OnFrameUpdatePreRender)
  {
    const float fTimeDiff = Vision::GetUITimer()->GetTimeDifference();
    ProcessInput(fTimeDiff);
  }
  else if (pData->m_pSender == &Vision::Callbacks.OnVideoChanged)
  {
#if defined(SUPPORTS_MULTITOUCH)
    // Re-initialize valid area
    if (m_pVirtualThumbStick != NULL)
      m_pVirtualThumbStick->SetValidArea(VRectanglef());
#endif
  }
}

// Serialization Versions
#define VFREECAMERA_VERSION_0       0 // Initial version
#define VFREECAMERA_VERSION_2       2 // Replaced integer value which was in the range [0, 2] by a version number.
#define VFREECAMERA_VERSION_3       3 // Sensitivity and speed.
#define VFREECAMERA_VERSION_CURRENT VFREECAMERA_VERSION_3

void VFreeCamera::Serialize(VArchive& ar)
{
  VisBaseEntity_cl::Serialize(ar);

  if (ar.IsLoading())
  {
    int iLocalVersion;
    ar >> iLocalVersion;
    VASSERT_MSG(iLocalVersion <= VFREECAMERA_VERSION_CURRENT && iLocalVersion >= VFREECAMERA_VERSION_0, "Invalid local version. Please re-export");

    BaseInit();

    if (iLocalVersion >= VFREECAMERA_VERSION_3)
    {
      ar >> m_fSensitivity;
      ar >> m_fMoveSpeed;
    }
  }
  else
  {
    const int iLocalVersion = VFREECAMERA_VERSION_CURRENT;
    ar << iLocalVersion;

    ar << m_fSensitivity;
    ar << m_fMoveSpeed;
  }
}

START_VAR_TABLE(VFreeCamera, VisBaseEntity_cl, "VFreeCamera", 0, "")  
  DEFINE_VAR_FLOAT_AND_NAME(VFreeCamera, m_fSensitivity, "LookSensitivity", "Sensitivity when looking around. Set to 0 to use a fixed view direction.", "200.0", 0, "Clamp(0.0, 1e20)");
  DEFINE_VAR_FLOAT_AND_NAME(VFreeCamera, m_fMoveSpeed, "MoveSpeed", "The speed at which the camera moves. Set to 0 to use a fixed camera position.", "350.0", DISPLAY_HINT_GLOBALUNITSCALED, "Clamp(0.0, 1e20)");
END_VAR_TABLE

// VisMouseCamera_cl serialization fall back
V_IMPLEMENT_SERIAL(VisMouseCamera_cl, VFreeCamera, 0, &g_VisionEngineModule);

START_VAR_TABLE(VisMouseCamera_cl, VFreeCamera, "VFreeCamera", VFORGE_HIDECLASS, "")
END_VAR_TABLE

/*
 * Havok SDK - Base file, BUILD(#20140726)
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
