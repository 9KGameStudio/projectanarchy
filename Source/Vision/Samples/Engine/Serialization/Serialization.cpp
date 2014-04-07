/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// ***********************************************************************************************
// Serialization Demo
// Copyright (C) Havok.com Inc. All rights reserved.
// ***********************************************************************************************
// This sample shows how serialization works with the Vision engine. Serialization allows to
// save a current snapshot of your scene to file. This is typically used for "save game"
// implementations.
// ***********************************************************************************************

#include <Vision/Samples/Engine/Serialization/SerializationPCH.h>
#include <Vision/Runtime/Framework/VisionApp/VAppImpl.hpp>
#include <Vision/Runtime/Framework/VisionApp/Modules/VHelp.hpp>

#include <Vision/Samples/Engine/Serialization/SaveGameSlot.hpp>

#include <Vision/Samples/Engine/Serialization/SerializeBaseEntity.hpp>
#include <Vision/Samples/Engine/Serialization/SerializePlayer.hpp>

#include <Vision/Runtime/EnginePlugins/EnginePluginsImport.hpp>

// declare our "module", since we need that for serialization support
extern VModule g_serializeSampleModule;
#define SERIALIZESAMPLE_VERSION_MAJOR 0x01
#define SERIALIZESAMPLE_VERSION_MINOR 0x00
DECLARE_THIS_MODULE(g_serializeSampleModule, MAKE_VERSION(SERIALIZESAMPLE_VERSION_MAJOR, SERIALIZESAMPLE_VERSION_MINOR), "Engine Serialization Sample", "Havok", "Vision Engine Serialization Sample", NULL);

#define MAP_1         "Room.vscene"
#define MAP_2         "Crossing.vscene"

#define MAP_ICON_1    "Serialization/map01.dds"
#define MAP_ICON_2    "Serialization/map02.dds"
#define MAP_DATA_DIR  ":havok_sdk/Data/Vision/Samples/Engine/Maps/SciFi"
const char* g_pszMaps[2]      = { MAP_1, MAP_2 };
const char* g_pszMapIcons[2]  = { MAP_ICON_1, MAP_ICON_2 };
#define SAVE_ICON     "Serialization/SaveIcon.dds"
#define TTL_IMAGE     "Serialization/TouchToLoad.dds"
#define HTTS_IMAGE    "Serialization/HoldTouchToSave.dds"
#define CL_IMAGE      "Serialization/ChangeLevel.dds"

#define INITIAL_SAVEGAME_DIR  MAP_DATA_DIR"/Serialization"
#define ARCHIVE_START_TAG "SERIALIZE_API_SAMPLE"
#define ARCHIVE_END_TAG   "END_OF_ARCHIVE"
#define SAVEGAME_NAME     "SaveGames/save%1d.gam"

#define INFO_SCREEN_TIME    7.0f
#define INFO_FADEOUT_TIME   0.5f

class SerializationApp : public VAppImpl, public IVisCallbackHandler_cl
{
public:
  SerializationApp() 
    : m_iCurrentMap(0)
    , m_fInfoTime(0.0f)
    , m_bSavePending(false)
    , m_iGameNum(1)
#if defined(_VISION_MOBILE)
    , m_pSaveIconArea(NULL)
    , m_pNextMapArea(NULL)
#endif
  {}
  virtual ~SerializationApp() {}

  virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData) HKV_OVERRIDE;

  virtual void Init() HKV_OVERRIDE;

  virtual void PreloadPlugins() HKV_OVERRIDE { VISION_PLUGIN_LOAD_STATIC(vHavok); }
  virtual void AfterSceneLoaded(bool bLoadingSuccessful) HKV_OVERRIDE;
  virtual bool Run() HKV_OVERRIDE;

  virtual void DeInit() HKV_OVERRIDE;

private:
  void GetEntityParameters(VisBaseEntity_cl *pEnt, char *szBuf);
  void DumpEntityCount();
  void LoadMap(int iMap, BOOL bLoadEntities);
  void UpdateSlotStatus(int iCurrentSlot);
  void ResetWorld();
  VBool SaveGame(int iNum);
  VBool LoadGame(int iNum);

  inline void TriggerSave(int iGameNum)
  {
    m_bSavePending = true;
    m_iGameNum = iGameNum;
  }

  char m_pszCurrentMap[64];     // the current map name
  int  m_iCurrentMap;           // the current map number

  VisScreenMaskPtr m_spMap;
  VisScreenMaskPtr m_spSaveIcon;
  VisScreenMaskPtr m_spTouchToLoad;
  VisScreenMaskPtr m_spHoldToSave;
  VisScreenMaskPtr m_spChangeLevel;

  float m_fInfoTime;

  // four instances of our savegame slots
  SaveGameSlot_cl m_saveSlot[4];

  bool m_bSavePending;
  int m_iGameNum;

#if defined(_VISION_MOBILE)
  VTouchArea* m_pSaveIconArea;
  VTouchArea* m_pNextMapArea;
#endif
};

VAPP_IMPLEMENT_SAMPLE(SerializationApp);

void SerializationApp::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if (pData->m_pSender == &Vision::Callbacks.OnBeforeSwapBuffers)
  {
    if (m_bSavePending)
    {
      // Perform Save
      if (SaveGame(m_iGameNum) == TRUE)
      {
        m_saveSlot[m_iGameNum-1].ResetErrorState();
        Vision::Message.Add(1, "Saved game in slot %d.", m_iGameNum);
        UpdateSlotStatus(-1);
      }
      else
      {
        m_saveSlot[m_iGameNum-1].SetErrorState();
        Vision::Message.Add(1, "ERROR: Couldn't save game in slot %d!", m_iGameNum);
        UpdateSlotStatus(-1);
      }

      // Re-set
      m_bSavePending = false;
    }
  }
}

void SerializationApp::Init()
{
  // register our module with the vision type manager
  Vision::GetTypeManager()->RegisterModule(&g_serializeSampleModule);

  LoadMap(m_iCurrentMap, TRUE);

  // Create Save Game Callback Handler and register it at OnBeforeSwapBuffers to be 
  // able to get the back buffer just before it is presented.
  Vision::Callbacks.OnBeforeSwapBuffers += this;

  for (int i = 0; i < 4; i++)
  {
    m_saveSlot[i].OnInitOneTime(i);
  }

  UpdateSlotStatus(-1);
}

void SerializationApp::AfterSceneLoaded(bool bLoadingSuccessful)
{
  // Re-add the application data search path. This has the effect that it will precede all other paths for loading,
  // and thus saved games will (virtually) overwrite the shipped default saves.
  Vision::File.AddSearchPath(":app_data", VSearchPathFlags::WRITABLE);

  // Update the slots in order to load the preview pictures from the correct directory
  UpdateSlotStatus(-1);

  VArray<const char*> help;
#if defined(_VISION_MOBILE)
  help.Append("The icons at the upper right show which savegame slots hold a saved game,");
  help.Append("while the icon at the bottom right shows which map you are currently in.");
  help.Append("To save, hold the red button (left side) while tapping on a savegame slot.");
#else
  help.Append("The icons at the bottom left show which savegame slots hold a saved game,");
  help.Append("while the icon at the bottom right shows which map you are currently in.");
  help.Append("To load a savegame, press the number key of the savegame slot (1..4).");
  help.Append("To save, hold the shift key and press the number of the slot to save in.");
#endif
  help.Append("");

#ifdef SUPPORTS_MOUSE
  GetInputMap()->MapTrigger(SERIALIZE_LOOK_CHANGED, V_MOUSE, CT_MOUSE_DATA_CHANGED);

  GetInputMap()->MapTrigger(SERIALIZE_HORIZONTAL_LOOK, V_MOUSE, CT_MOUSE_NORM_DELTA_X, VInputOptions::Sensitivity(2.0f));
  GetInputMap()->MapTrigger(SERIALIZE_VERTICAL_LOOK,   V_MOUSE, CT_MOUSE_NORM_DELTA_Y, VInputOptions::Sensitivity(2.0f));

  GetInputMap()->MapTrigger(SERIALIZE_RUN,        V_MOUSE, CT_MOUSE_RIGHT_BUTTON);
  GetInputMap()->MapTrigger(SERIALIZE_RUN_FASTER, V_MOUSE, CT_MOUSE_MIDDLE_BUTTON);
#endif

#ifdef SUPPORTS_KEYBOARD
  // On the PC, register key controls that
  // differ from the defaults
  GetInputMap()->MapTrigger(SERIALIZE_SLOT_1,   V_KEYBOARD, CT_KB_1, VInputOptions::Once());
  GetInputMap()->MapTrigger(SERIALIZE_SLOT_2,   V_KEYBOARD, CT_KB_2, VInputOptions::Once());
  GetInputMap()->MapTrigger(SERIALIZE_SLOT_3,   V_KEYBOARD, CT_KB_3, VInputOptions::Once());
  GetInputMap()->MapTrigger(SERIALIZE_SLOT_4,   V_KEYBOARD, CT_KB_4, VInputOptions::Once());
  GetInputMap()->MapTrigger(SERIALIZE_NEXT_MAP, V_KEYBOARD, CT_KB_M, VInputOptions::Once());

  GetInputMap()->MapTrigger(SERIALIZE_SAVE_MODIFIER,  V_KEYBOARD, CT_KB_LSHIFT);
  GetInputMap()->MapTrigger(SERIALIZE_SAVE_MODIFIER,  V_KEYBOARD, CT_KB_RSHIFT);

  GetInputMap()->MapTrigger(SERIALIZE_ACTION_OCCURED, V_KEYBOARD, CT_KB_UNKNOWN);

  GetInputMap()->MapTrigger(SERIALIZE_MOVE_FORWARD,   V_KEYBOARD, CT_KB_UP);
  GetInputMap()->MapTrigger(SERIALIZE_MOVE_BACKWARD,  V_KEYBOARD, CT_KB_DOWN);
  GetInputMap()->MapTrigger(SERIALIZE_MOVE_LEFT,      V_KEYBOARD, CT_KB_LEFT);
  GetInputMap()->MapTrigger(SERIALIZE_MOVE_RIGHT,     V_KEYBOARD, CT_KB_RIGHT);

  help.Append("KEYBOARD - Shift + 1...4 : Save game to slot 1...4");
  help.Append("KEYBOARD - 1...4         : Load game from slot 1...4");
  help.Append("KEYBOARD - M : switch to next map");
  help.Append("");

#endif

#if defined (_VISION_XENON) || (defined(_VISION_WINRT) && !defined(_VISION_METRO) && !defined(_VISION_APOLLO))
  GetInputMap()->MapTrigger(SERIALIZE_SLOT_1,   V_XENON_PAD(0), CT_PAD_A, VInputOptions::Once());
  GetInputMap()->MapTrigger(SERIALIZE_SLOT_2,   V_XENON_PAD(0), CT_PAD_B, VInputOptions::Once());
  GetInputMap()->MapTrigger(SERIALIZE_SLOT_3,   V_XENON_PAD(0), CT_PAD_X, VInputOptions::Once());
  GetInputMap()->MapTrigger(SERIALIZE_SLOT_4,   V_XENON_PAD(0), CT_PAD_Y, VInputOptions::Once());
  GetInputMap()->MapTrigger(SERIALIZE_NEXT_MAP, V_XENON_PAD(0), CT_PAD_LEFT_SHOULDER, VInputOptions::Once());

  GetInputMap()->MapTrigger(SERIALIZE_SAVE_MODIFIER,  V_XENON_PAD(0), CT_PAD_LEFT_TRIGGER, VInputOptions::DeadZone(0.25f));

  GetInputMap()->MapTrigger(SERIALIZE_ACTION_OCCURED, V_XENON_PAD(0), CT_PAD_ANY_KEY);

  GetInputMap()->MapTrigger(SERIALIZE_MOVE_FORWARD,  V_XENON_PAD(0), CT_PAD_UP);
  GetInputMap()->MapTrigger(SERIALIZE_MOVE_BACKWARD, V_XENON_PAD(0), CT_PAD_DOWN);
  GetInputMap()->MapTrigger(SERIALIZE_MOVE_LEFT,     V_XENON_PAD(0), CT_PAD_LEFT);
  GetInputMap()->MapTrigger(SERIALIZE_MOVE_RIGHT,    V_XENON_PAD(0), CT_PAD_RIGHT);

  GetInputMap()->MapTrigger(SERIALIZE_MOVE_FORWARD,  V_XENON_PAD(0), CT_PAD_LEFT_THUMB_STICK_UP, VInputOptions::DeadZone(0.4f));
  GetInputMap()->MapTrigger(SERIALIZE_MOVE_BACKWARD, V_XENON_PAD(0), CT_PAD_LEFT_THUMB_STICK_DOWN, VInputOptions::DeadZone(0.4f));
  GetInputMap()->MapTrigger(SERIALIZE_MOVE_LEFT,     V_XENON_PAD(0), CT_PAD_LEFT_THUMB_STICK_LEFT, VInputOptions::DeadZone(0.4f));
  GetInputMap()->MapTrigger(SERIALIZE_MOVE_RIGHT,    V_XENON_PAD(0), CT_PAD_LEFT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.4f));

  GetInputMap()->MapTrigger(SERIALIZE_LOOK_CHANGED,  V_XENON_PAD(0), CT_PAD_RIGHT_THUMB_STICK_CHANGED);

  GetInputMap()->MapTriggerAxis(SERIALIZE_HORIZONTAL_LOOK, V_XENON_PAD(0), CT_PAD_RIGHT_THUMB_STICK_LEFT, CT_PAD_RIGHT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.25f, true));
  GetInputMap()->MapTriggerAxis(SERIALIZE_VERTICAL_LOOK,   V_XENON_PAD(0), CT_PAD_RIGHT_THUMB_STICK_UP, CT_PAD_RIGHT_THUMB_STICK_DOWN, VInputOptions::DeadZone(0.25f, true));

  GetInputMap()->MapTrigger(SERIALIZE_RUN,        V_XENON_PAD(0), CT_PAD_RIGHT_SHOULDER, VInputOptions::DeadZone(0.4f));
  GetInputMap()->MapTrigger(SERIALIZE_RUN_FASTER, V_XENON_PAD(0), CT_PAD_RIGHT_THUMB);

  help.Append("PAD1 - Left Trigger + PAD1 - A, B, X, Y : Save game to slot 1...4");
  help.Append("PAD1 - A, B, X, Y     : Load game from slot 1...4");
  help.Append("PAD1 - Left Shoulder : Switch to next map");

#elif defined (_VISION_PS3)
  GetInputMap()->MapTrigger(SERIALIZE_SLOT_1,   V_PS3_PAD(0), CT_PAD_CROSS, VInputOptions::Once());
  GetInputMap()->MapTrigger(SERIALIZE_SLOT_2,   V_PS3_PAD(0), CT_PAD_CIRCLE, VInputOptions::Once());
  GetInputMap()->MapTrigger(SERIALIZE_SLOT_3,   V_PS3_PAD(0), CT_PAD_SQUARE, VInputOptions::Once());
  GetInputMap()->MapTrigger(SERIALIZE_SLOT_4,   V_PS3_PAD(0), CT_PAD_TRIANGLE, VInputOptions::Once());
  GetInputMap()->MapTrigger(SERIALIZE_NEXT_MAP, V_PS3_PAD(0), CT_PAD_LEFT_SHOULDER, VInputOptions::Once());

  GetInputMap()->MapTrigger(SERIALIZE_SAVE_MODIFIER,  V_PS3_PAD(0), CT_PAD_LEFT_TRIGGER, VInputOptions::DeadZone(0.25f));

  GetInputMap()->MapTrigger(SERIALIZE_ACTION_OCCURED, V_PS3_PAD(0), CT_PAD_ANY_KEY);

  GetInputMap()->MapTrigger(SERIALIZE_MOVE_FORWARD,  V_PS3_PAD(0), CT_PAD_UP);
  GetInputMap()->MapTrigger(SERIALIZE_MOVE_BACKWARD, V_PS3_PAD(0), CT_PAD_DOWN);
  GetInputMap()->MapTrigger(SERIALIZE_MOVE_LEFT,     V_PS3_PAD(0), CT_PAD_LEFT);
  GetInputMap()->MapTrigger(SERIALIZE_MOVE_RIGHT,    V_PS3_PAD(0), CT_PAD_RIGHT);

  GetInputMap()->MapTrigger(SERIALIZE_MOVE_FORWARD,  V_PS3_PAD(0), CT_PAD_LEFT_THUMB_STICK_UP,    VInputOptions::DeadZone(0.4f));
  GetInputMap()->MapTrigger(SERIALIZE_MOVE_BACKWARD, V_PS3_PAD(0), CT_PAD_LEFT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.4f));
  GetInputMap()->MapTrigger(SERIALIZE_MOVE_LEFT,     V_PS3_PAD(0), CT_PAD_LEFT_THUMB_STICK_LEFT,  VInputOptions::DeadZone(0.4f));
  GetInputMap()->MapTrigger(SERIALIZE_MOVE_RIGHT,    V_PS3_PAD(0), CT_PAD_LEFT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.4f));

  GetInputMap()->MapTrigger(SERIALIZE_LOOK_CHANGED,  V_PS3_PAD(0), CT_PAD_RIGHT_THUMB_STICK_CHANGED);

  GetInputMap()->MapTriggerAxis(SERIALIZE_HORIZONTAL_LOOK, V_PS3_PAD(0), CT_PAD_RIGHT_THUMB_STICK_LEFT, CT_PAD_RIGHT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.25f, true));
  GetInputMap()->MapTriggerAxis(SERIALIZE_VERTICAL_LOOK,   V_PS3_PAD(0), CT_PAD_RIGHT_THUMB_STICK_UP,   CT_PAD_RIGHT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.25f, true));

  GetInputMap()->MapTrigger(SERIALIZE_RUN,        V_PS3_PAD(0), CT_PAD_RIGHT_SHOULDER, VInputOptions::DeadZone(0.4f));
  GetInputMap()->MapTrigger(SERIALIZE_RUN_FASTER, V_PS3_PAD(0), CT_PAD_RIGHT_THUMB);

  help.Append("PAD1 - Left Trigger + PAD1 - CROSS/CIRCLE/TRIANGLE/SQUARE : Save game to slot 1...4");
  help.Append("PAD1 - CROSS/CIRCLE/TRIANGLE/SQUARE     : Load game from slot 1...4");
  help.Append("PAD1 - Left Shoulder : Switch to next map");

#elif defined (_VISION_PSP2)
  GetInputMap()->MapTrigger(SERIALIZE_SLOT_1,   V_PSP2_PAD(0), CT_PAD_CROSS,         VInputOptions::Once());
  GetInputMap()->MapTrigger(SERIALIZE_SLOT_2,   V_PSP2_PAD(0), CT_PAD_CIRCLE,        VInputOptions::Once());
  GetInputMap()->MapTrigger(SERIALIZE_SLOT_3,   V_PSP2_PAD(0), CT_PAD_SQUARE,        VInputOptions::Once());
  GetInputMap()->MapTrigger(SERIALIZE_SLOT_4,   V_PSP2_PAD(0), CT_PAD_TRIANGLE,      VInputOptions::Once());
  GetInputMap()->MapTrigger(SERIALIZE_NEXT_MAP, V_PSP2_PAD(0), CT_PAD_LEFT_SHOULDER, VInputOptions::Once());

  GetInputMap()->MapTrigger(SERIALIZE_SAVE_MODIFIER,  V_PSP2_PAD(0), CT_PAD_LEFT_TRIGGER, VInputOptions::DeadZone(0.25f));
  GetInputMap()->MapTrigger(SERIALIZE_ACTION_OCCURED, V_PSP2_PAD(0), CT_PAD_ANY_KEY);

  GetInputMap()->MapTrigger(SERIALIZE_MOVE_FORWARD,   V_PSP2_PAD(0), CT_PAD_UP);
  GetInputMap()->MapTrigger(SERIALIZE_MOVE_BACKWARD,  V_PSP2_PAD(0), CT_PAD_DOWN);
  GetInputMap()->MapTrigger(SERIALIZE_MOVE_LEFT,      V_PSP2_PAD(0), CT_PAD_LEFT);
  GetInputMap()->MapTrigger(SERIALIZE_MOVE_RIGHT,     V_PSP2_PAD(0), CT_PAD_RIGHT);

  GetInputMap()->MapTrigger(SERIALIZE_MOVE_FORWARD,   V_PSP2_PAD(0), CT_PAD_LEFT_THUMB_STICK_UP,    VInputOptions::DeadZone(0.4f));
  GetInputMap()->MapTrigger(SERIALIZE_MOVE_BACKWARD,  V_PSP2_PAD(0), CT_PAD_LEFT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.4f));
  GetInputMap()->MapTrigger(SERIALIZE_MOVE_LEFT,      V_PSP2_PAD(0), CT_PAD_LEFT_THUMB_STICK_LEFT,  VInputOptions::DeadZone(0.4f));
  GetInputMap()->MapTrigger(SERIALIZE_MOVE_RIGHT,     V_PSP2_PAD(0), CT_PAD_LEFT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.4f));

  GetInputMap()->MapTrigger(SERIALIZE_LOOK_CHANGED,   V_PSP2_PAD(0), CT_PAD_RIGHT_THUMB_STICK_CHANGED);

  GetInputMap()->MapTriggerAxis(SERIALIZE_HORIZONTAL_LOOK, V_PSP2_PAD(0), CT_PAD_RIGHT_THUMB_STICK_LEFT, CT_PAD_RIGHT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.25f, true));
  GetInputMap()->MapTriggerAxis(SERIALIZE_VERTICAL_LOOK,   V_PSP2_PAD(0), CT_PAD_RIGHT_THUMB_STICK_UP,   CT_PAD_RIGHT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.25f, true));

  GetInputMap()->MapTrigger(SERIALIZE_RUN,        V_PSP2_PAD(0), CT_PAD_RIGHT_SHOULDER, VInputOptions::DeadZone(0.4f));
  GetInputMap()->MapTrigger(SERIALIZE_RUN_FASTER, V_PSP2_PAD(0), CT_PAD_RIGHT_THUMB);

  help.Append("PAD1 - Left Trigger + PAD1 - CROSS/CIRCLE/TRIANGLE/SQUARE : Save game to slot 1...4");
  help.Append("PAD1 - CROSS/CIRCLE/TRIANGLE/SQUARE     : Load game from slot 1...4");
  help.Append("PAD1 - Left Shoulder : Switch to next map");

#elif defined (_VISION_WIIU)
  GetInputMap()->MapTrigger(SERIALIZE_SLOT_1,   VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_B, VInputOptions::Once());
  GetInputMap()->MapTrigger(SERIALIZE_SLOT_2,   VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_A, VInputOptions::Once());
  GetInputMap()->MapTrigger(SERIALIZE_SLOT_3,   VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_Y, VInputOptions::Once());
  GetInputMap()->MapTrigger(SERIALIZE_SLOT_4,   VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_X, VInputOptions::Once());
  GetInputMap()->MapTrigger(SERIALIZE_NEXT_MAP, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_SHOULDER, VInputOptions::Once());

  GetInputMap()->MapTrigger(SERIALIZE_SAVE_MODIFIER,  VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_TRIGGER, VInputOptions::DeadZone(0.25f));

  GetInputMap()->MapTrigger(SERIALIZE_ACTION_OCCURED, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_ANY_KEY);

  GetInputMap()->MapTrigger(SERIALIZE_MOVE_FORWARD,  VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_UP);
  GetInputMap()->MapTrigger(SERIALIZE_MOVE_BACKWARD, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_DOWN);
  GetInputMap()->MapTrigger(SERIALIZE_MOVE_LEFT,     VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT);
  GetInputMap()->MapTrigger(SERIALIZE_MOVE_RIGHT,    VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_RIGHT);

  GetInputMap()->MapTrigger(SERIALIZE_MOVE_FORWARD,  VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_THUMB_STICK_UP,    VInputOptions::DeadZone(0.4f));
  GetInputMap()->MapTrigger(SERIALIZE_MOVE_BACKWARD, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.4f));
  GetInputMap()->MapTrigger(SERIALIZE_MOVE_LEFT,     VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_THUMB_STICK_LEFT,  VInputOptions::DeadZone(0.4f));
  GetInputMap()->MapTrigger(SERIALIZE_MOVE_RIGHT,    VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.4f));

  GetInputMap()->MapTrigger(SERIALIZE_LOOK_CHANGED,  VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_RIGHT_THUMB_STICK_CHANGED);

  GetInputMap()->MapTriggerAxis(SERIALIZE_HORIZONTAL_LOOK, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_RIGHT_THUMB_STICK_LEFT, CT_PAD_RIGHT_THUMB_STICK_RIGHT, VInputOptions::DeadZone(0.25f, true));
  GetInputMap()->MapTriggerAxis(SERIALIZE_VERTICAL_LOOK,   VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_RIGHT_THUMB_STICK_UP,   CT_PAD_RIGHT_THUMB_STICK_DOWN,  VInputOptions::DeadZone(0.25f, true));

  GetInputMap()->MapTrigger(SERIALIZE_RUN,        VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_RIGHT_SHOULDER, VInputOptions::DeadZone(0.4f));
  GetInputMap()->MapTrigger(SERIALIZE_RUN_FASTER, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_RIGHT_THUMB);

  help.Append("DRC - Left Trigger + DRC - B/A/Y/X : Save game to slot 1...4");
  help.Append("DRC - B/A/Y/X     : Load game from slot 1...4");
  help.Append("DRC - Left Shoulder : Switch to next map");

#endif

  const int iScreenWidth = Vision::Video.GetXRes();
  const int iScreenHeight = Vision::Video.GetYRes();
  const float fUiScale = VAppHelper::GetUIScalingFactor();
  int iTextureWidth, iTextureHeight;

  // Set up "current map" screen mask.
  m_spMap = new VisScreenMask_cl(g_pszMapIcons[m_iCurrentMap]);
  m_spMap->GetTextureSize(iTextureWidth, iTextureHeight);
  hkvVec2 vTargetSize = hkvVec2(static_cast<float>(iTextureWidth), static_cast<float>(iTextureHeight)) * fUiScale;
  m_spMap->SetTargetSize(vTargetSize.x, vTargetSize.y);

  hkvVec2 vPos = hkvVec2(static_cast<float>(iScreenWidth) - vTargetSize.x - 152.0f, static_cast<float>(iScreenHeight) - vTargetSize.y - 8.0f);
  m_spMap->SetPos(vPos.x, vPos.y);
  m_spMap->SetTransparency(VIS_TRANSP_ALPHA);
  m_spMap->SetColor(SaveGameSlot_cl::s_iOverlayColor);
  m_spMap->SetVisible(TRUE);

#if defined(_VISION_MOBILE)
  // Info text: Change level
  m_spChangeLevel = new VisScreenMask_cl(CL_IMAGE);
  m_spChangeLevel->GetTextureSize(iTextureWidth, iTextureHeight);
  m_spChangeLevel->SetPos(vPos.x + (vTargetSize.x / 2) - static_cast<float>(iTextureWidth), vPos.y - static_cast<float>(iTextureHeight));
  m_spChangeLevel->SetTransparency(VIS_TRANSP_ALPHA);
  m_spChangeLevel->SetColor(SaveGameSlot_cl::s_iOverlayColor);
  m_spChangeLevel->SetVisible(TRUE);

  // touch area
  if (m_pNextMapArea == NULL)
  {
    m_pNextMapArea = new VTouchArea(VInputManager::GetTouchScreen(), 
      VRectanglef(vPos.x, vPos.y, vPos.x + vTargetSize.x, vPos.y + vTargetSize.y));
    GetInputMap()->MapTrigger(SERIALIZE_NEXT_MAP, m_pNextMapArea, CT_TOUCH_ANY, VInputOptions::Once());
  }

  // Save icon
  m_spSaveIcon = new VisScreenMask_cl(SAVE_ICON);
  
  m_spSaveIcon->GetTextureSize(iTextureWidth, iTextureHeight);
  vTargetSize = hkvVec2(static_cast<float>(iTextureWidth), static_cast<float>(iTextureHeight)) * fUiScale * 1.5f;
  m_spSaveIcon->SetTargetSize(vTargetSize.x, vTargetSize.y);

  vPos = hkvVec2(8.0f, static_cast<float>(iScreenHeight) / 3.0f - iTextureHeight - 8.0f);
  m_spSaveIcon->SetPos(vPos.x, vPos.y);
  m_spSaveIcon->SetTransparency(VIS_TRANSP_ALPHA);
  m_spSaveIcon->SetColor(SaveGameSlot_cl::s_iOverlayColor);
  m_spSaveIcon->SetVisible(TRUE);

  // Info text: Hold to save
  m_spHoldToSave = new VisScreenMask_cl(HTTS_IMAGE);
  m_spHoldToSave->GetTextureSize(iTextureWidth, iTextureHeight);
  m_spHoldToSave->SetPos(vPos.x + (vTargetSize.x * 0.5f) - 21.0f, vPos.y - static_cast<float>(iTextureHeight));
  m_spHoldToSave->SetTransparency(VIS_TRANSP_ALPHA);
  m_spHoldToSave->SetColor(SaveGameSlot_cl::s_iOverlayColor);
  m_spHoldToSave->SetVisible(TRUE);

  // touch area
  if (m_pSaveIconArea == NULL)
  {
    m_pSaveIconArea = new VTouchArea(VInputManager::GetTouchScreen(), 
      VRectanglef(vPos.x, vPos.y, vPos.x + vTargetSize.x, vPos.y + vTargetSize.y), 0.0f);
    GetInputMap()->MapTrigger(SERIALIZE_SAVE_MODIFIER, m_pSaveIconArea, CT_TOUCH_ANY);
  }

  // Info text: Touch to load
  m_spTouchToLoad = new VisScreenMask_cl(TTL_IMAGE);
  m_spTouchToLoad->GetTextureSize(iTextureWidth, iTextureHeight);
  m_spTouchToLoad->SetPos(iScreenWidth - iTextureWidth - 132.0f, 130.0f + (96.0f - iTextureHeight) * 0.5f);
  m_spTouchToLoad->SetTransparency(VIS_TRANSP_ALPHA);
  m_spTouchToLoad->SetColor(SaveGameSlot_cl::s_iOverlayColor);
  m_spTouchToLoad->SetVisible(TRUE);

  m_fInfoTime = INFO_SCREEN_TIME + INFO_FADEOUT_TIME;
#endif

  VHelp* pHelp = GetAppModule<VHelp>();
  if (pHelp != NULL)
    pHelp->SetText(help);
  else
    RegisterAppModule(new VHelp(help));
}

bool SerializationApp::Run()
{
  int iGameNum = 0;
  bool bPerformSave = false;

  if (GetInputMap()->GetTrigger(SERIALIZE_SLOT_1)) iGameNum = 1;
  if (GetInputMap()->GetTrigger(SERIALIZE_SLOT_2)) iGameNum = 2;
  if (GetInputMap()->GetTrigger(SERIALIZE_SLOT_3)) iGameNum = 3;
  if (GetInputMap()->GetTrigger(SERIALIZE_SLOT_4)) iGameNum = 4;
  if (GetInputMap()->GetTrigger(SERIALIZE_SAVE_MODIFIER)) bPerformSave = true;

  if (iGameNum != 0)
  {
    if (bPerformSave)
    {
      // save (wait for next back buffer swap)
      TriggerSave(iGameNum);
    }
    else // load slot
    {
      if (m_saveSlot[iGameNum-1].IsSlotUsed(SAVEGAME_NAME))
      {
        // load
        if (LoadGame(iGameNum) == TRUE)
        {
          UpdateSlotStatus(iGameNum - 1);
          Vision::Message.Add(1, "Loaded game from slot %d.", iGameNum);
        }
        else
        {
          m_saveSlot[iGameNum-1].SetErrorState();
          Vision::Message.Add(1, "ERROR: Couldn't load game from slot %d!", iGameNum);
          UpdateSlotStatus(iGameNum - 1);
        }
      }
      else
      {
        Vision::Message.Add(1, "Save game slot %d is empty.", iGameNum);
      }
    }
    //DumpEntityCount();
  }

  if (GetInputMap()->GetTrigger(SERIALIZE_NEXT_MAP))
  {
    m_iCurrentMap = (m_iCurrentMap + 1) % 2;
    // load the next map (including entities)
    LoadMap(m_iCurrentMap, TRUE);
    UpdateSlotStatus(-1);
  }

  if (m_fInfoTime > 0.0f)
  {
    if (m_fInfoTime < INFO_FADEOUT_TIME)
    {
      VColorRef col(SaveGameSlot_cl::s_iOverlayColor);
      col.a = UBYTE(col.a * (m_fInfoTime / INFO_FADEOUT_TIME));
      m_spTouchToLoad->SetColor(col);
      m_spHoldToSave->SetColor(col);
      m_spChangeLevel->SetColor(col);
    }
    m_fInfoTime -= Vision::GetTimer()->GetTimeDifference();

    if (m_fInfoTime <= 0.0f)
    {
      m_spTouchToLoad->SetVisible(FALSE);
      m_spHoldToSave->SetVisible(FALSE);
      m_spChangeLevel->SetVisible(FALSE);
    }
  }

  return true;
}

void SerializationApp::DeInit()
{
  m_spMap = NULL;
  m_spSaveIcon = NULL;
  m_spHoldToSave = NULL;
  m_spTouchToLoad = NULL;
  m_spChangeLevel = NULL;

  // deinit the slots (screenmasks etc.)
  for (int i=0; i<4; ++i)
    m_saveSlot[i].OnDeInitOneTime();

  // unregister our module
  Vision::GetTypeManager()->UnregisterModule(&g_serializeSampleModule);

  // unregister callbacks
  Vision::Callbacks.OnBeforeSwapBuffers -= this;
}

// ---------------------------------------------------------------------------------
// Method: GetEntityParameters
// Notes: Gets the full parameter string of an entity. This is used for re-creating
//        certain entities after serialization.
// ---------------------------------------------------------------------------------

void SerializationApp::GetEntityParameters(VisBaseEntity_cl *pEnt, char *szBuf)
{
  char szValue[1000];
  char szVarString[1000];
  szBuf[0] = 0;
  for (int i = 0; i < pEnt->GetNumVariables(); i++)
  {
    const char *szVarName = pEnt->GetVariableName(i);
    pEnt->GetVariableValue(szVarName, szValue);
    sprintf(szVarString, " %s = \"%s\"", szVarName, szValue);
    szBuf[strlen(szBuf) + strlen(szVarString)] = 0;
    strcat(szBuf, szVarString);
  }
}

// ---------------------------------------------------------------------------------
// Method: DumpEntityCount
// Notes: Helper function to output the current number of entities
// ---------------------------------------------------------------------------------

void SerializationApp::DumpEntityCount()
{
  int iEntCount = VisBaseEntity_cl::ElementManagerGetUsedCount();
  int iLitCount = VisLightSource_cl::ElementManagerGetUsedCount();

  Vision::Message.Add(1, "Entity Count: %d -- LightSource Count: %d", iEntCount, iLitCount);
}

// ---------------------------------------------------------------------------------
// Method: LoadMap
// Notes: 
// ---------------------------------------------------------------------------------

void SerializationApp::LoadMap(int iMap, BOOL bLoadEntities)
{
  m_spMap = NULL;

  // load map
  strcpy(m_pszCurrentMap, g_pszMaps[iMap]);
  m_iCurrentMap = iMap;

  VisAppLoadSettings settings(m_pszCurrentMap);
  settings.m_customSearchPaths.Append(MAP_DATA_DIR);
  settings.m_customSearchPaths.Append(INITIAL_SAVEGAME_DIR);
  LoadScene(settings);

  hkvVec3 vStart;
  // try to find the Player_Start placeholder entity
  VisBaseEntity_cl* pStart = Vision::Game.SearchEntity("Player_Start");
  if (pStart)
    vStart = pStart->GetPosition();
  else
    vStart.set (-350.f, 200.f, 200.f);

  if (bLoadEntities)
  {
    // create player entity (in case the entities are not loaded from the savegame)
    Vision::Game.CreateEntity("SerializePlayer_cl", vStart);

    // create 3 spotlight entities
    hkvVec3 Placement;
    Placement.set (-200, 500, 200);
    Vision::Game.CreateEntity("SerializeSpotlight_cl", Placement);
    Placement.set (-300, 0, 100);
    Vision::Game.CreateEntity("SerializeSpotlight_cl", Placement);
    Placement.set (40, -600, 100);
    Vision::Game.CreateEntity("SerializeSpotlight_cl", Placement);
  }
}

// ---------------------------------------------------------------------------------
// Method: UpdateSlotStatus
// Notes: Update save slot screen masks
// ---------------------------------------------------------------------------------

void SerializationApp::UpdateSlotStatus(int iCurrentSlot)
{
  for (int i = 0; i < 4; i++)
    m_saveSlot[i].UpdateSlotStatus(iCurrentSlot, SAVEGAME_NAME);
}

// ---------------------------------------------------------------------------------
// Method: ResetWorld
// Notes: This function is used by the LoadGame function to get rid of all
//        serializeable entities.
// ---------------------------------------------------------------------------------

void SerializationApp::ResetWorld()
{
  int i;
  int iNumOfAllEntities = VisBaseEntity_cl::ElementManagerGetSize();
  
  // send CLEANUP messages to all the entities that are about to be freed, so
  // that they have a chance of cleaning up whatever is necessary before the
  // world is destroyed
  for (i = 0; i < iNumOfAllEntities; i++)
  {
    VisBaseEntity_cl *pEnt = VisBaseEntity_cl::ElementManagerGet(i);
    if (pEnt)
    {
      if (pEnt->IsOfType(SerializeBaseEntity_cl::GetClassTypeId()))
      {
        Vision::Game.SendMsg(pEnt, MSG_SER_CLEANUP, 0, 0);
      }
    }
  }

  // now actually free all the serializeable entities
  for (i = 0; i < iNumOfAllEntities; i++)
  {
    VisBaseEntity_cl *pEnt = VisBaseEntity_cl::ElementManagerGet(i);
    if (pEnt)
    {
      if (pEnt->IsOfType(SerializeBaseEntity_cl::GetClassTypeId()))
      {
        pEnt->DisposeObject();
      }
    }
  }
}

// ---------------------------------------------------------------------------------
// Method: SaveGame
// Notes: The file format for save games in this example is as follows:
//
//        "SERIALIZE_API_SAMPLE"  magic "number" (a string, really)
//        <version>
//        <map_number>            the current map's number
//        <time>                  the current game timer's value
//        <num_recreate_ent>      number of re-createable entities
//        <num_full_ent>          number of fully serialized entities
//        <recreate_ent>...       re-createable entity data
//        <full_ent>...           fully serialized entities
//        "END_OF_ARCHIVE"        ending key (useful for validating the file)
//
// ---------------------------------------------------------------------------------

VBool SerializationApp::SaveGame(int iNum)
{
  if ((iNum < 1) || (iNum > 4))
  {
    // we just allow 4 save games
    return FALSE;
  }

  int i;
  char pszSaveFileName[FS_MAX_PATH];

  sprintf(pszSaveFileName, SAVEGAME_NAME, iNum);
  IVFileOutStream* pOut = Vision::File.Create(pszSaveFileName);
  // creating the file didn't work!
  if (!pOut)
  {
    return FALSE;
  }

  VArchive ar(pszSaveFileName, pOut, Vision::GetTypeManager());

  // serialize global game data
  ar << ARCHIVE_START_TAG; 		      // magic number
  int iSavingVersion = Vision::GetArchiveVersion();
  ar << iSavingVersion;             // archive class version
  ar << m_iCurrentMap;              // current map number
  ar << Vision::GetTimer()->GetTime();		// current time

  // count entities
  SerializeBaseEntity_cl *pSerEnt = NULL;
  VisBaseEntity_cl *pEnt = NULL;
  int iFullCtr = 0;
  int iReCreateCtr = 0;
  int iNumOfAllEntities = VisBaseEntity_cl::ElementManagerGetSize();
  for (i = 0; i < iNumOfAllEntities; i++)
  {
    pEnt = VisBaseEntity_cl::ElementManagerGet(i);
    if (pEnt)
    {
      if (pEnt->IsOfType(SerializeBaseEntity_cl::GetClassTypeId()))
      {
        pSerEnt = static_cast<SerializeBaseEntity_cl*>(pEnt);
        if (pSerEnt->GetSerializeType() == SERIALIZE_FULL)
          iFullCtr++;
        else
          iReCreateCtr++;
      }
    }
  }

  // serialize number of entities
  ar << iReCreateCtr;
  ar << iFullCtr;

  hkvVec3 vTemp;

  // do ReCreate serialization of entities
  for (i = 0; i < iNumOfAllEntities; i++)
  {
    pEnt = VisBaseEntity_cl::ElementManagerGet(i);
    if (pEnt)
    {
      if (pEnt->IsOfType(SerializeBaseEntity_cl::GetClassTypeId()))
      {
        pSerEnt = (SerializeBaseEntity_cl *) pEnt;
        if (pSerEnt->GetSerializeType() == SERIALIZE_RECREATE)
        {
          char pszEntityParams[4000];
          GetEntityParameters(pSerEnt, pszEntityParams);
          VDynamicMesh* pMesh = pSerEnt->GetMesh();
          ar << pSerEnt->GetClassFullName() << pSerEnt->GetEntityKey();
          vTemp = pSerEnt->GetPosition();
          vTemp.SerializeAsVisVector (ar);
          vTemp = pSerEnt->GetOrientation();
          vTemp.SerializeAsVisVector (ar);
          const char *szFilename = pMesh ? pMesh->GetFilename() : NULL;
          ar << szFilename << pszEntityParams;
        }
      }
    }
  }

  // do full serialization of entities
  for (i = 0; i < iNumOfAllEntities; i++)
  {
    pEnt = VisBaseEntity_cl::ElementManagerGet(i);
    if (pEnt)
    {
      if (pEnt->IsOfType(SerializeBaseEntity_cl::GetClassTypeId()))
      {
        pSerEnt = (SerializeBaseEntity_cl *) pEnt;
        if (pSerEnt->GetSerializeType() == SERIALIZE_FULL)
        {
          ar << pSerEnt;
        }
      }
    }
  }

  // store end tag - useful to verify a valid archive
  ar << ARCHIVE_END_TAG;

  ar.Close();
  pOut->Close();

  m_saveSlot[iNum-1].SaveScreenShotPreview();

  return TRUE;
}

// ---------------------------------------------------------------------------------
// Method: LoadGame
// Notes: The file format for save games in this example is as follows:
//
//        "SERIALIZE_API_SAMPLE"  magic "number" (a string, really)
//        <version>
//        <map_number>            the current map number
//        <time>                  the current game timer's value
//        <num_recreate_ent>      number of re-creatable entities
//        <num_full_ent>          number of fully serialized entities
//        <recreate_ent>...       re-creatable entity data
//        <full_ent>...           fully serialized entities
//        "END_OF_ARCHIVE"        ending key (useful for validating the file)
//
// ---------------------------------------------------------------------------------

VBool SerializationApp::LoadGame(int iNum)
{
  if ((iNum < 1) || (iNum > 6))
  {
    return FALSE;
  }

  GameLoadData_t snap;

  int i;
  char pszLoadFileName[FS_MAX_PATH];
  char szArchiveTag[FS_MAX_PATH];
  
  sprintf(pszLoadFileName, SAVEGAME_NAME, iNum);

  VLoadingTask* pLoadingTask = Vision::File.PrecacheFile(pszLoadFileName);
  VASSERT(pLoadingTask != NULL);
  if (!pLoadingTask->IsValid())
    return FALSE;

  IVFileInStream* pIn = Vision::File.Open(pszLoadFileName);
  // opening the file didn't work!
  if (!pIn)
    return FALSE;

  VArchive ar(pszLoadFileName, pIn, Vision::GetTypeManager());

  ar.ReadStringBinary(szArchiveTag, FS_MAX_PATH);
  if (strcmp(szArchiveTag, ARCHIVE_START_TAG) != 0)
  { 
    return FALSE;
  }

  ar >> i;                        // loading version
  ar.SetLoadingVersion(i);
  if (! Vision::IsValidArchive(ar))
  {
    return FALSE;
  }
    
  ar >> snap.iMapNumber;          // map number
  if (snap.iMapNumber == m_iCurrentMap)
    snap.bJustReset = TRUE;
  else
    snap.bJustReset = FALSE;

  ar >> snap.fTime;               // game timer
  ar >> snap.iRecreateEntityCount;// number of re-createable entities
  ar >> snap.iFullEntityCount;    // number of fullly serialized entities

  //snap.bJustReset = FALSE;
  
  if (snap.bJustReset == TRUE)
  {
    // as it's the same map, we just clean up and re-use it :-)
    ResetWorld();
  }
  else
  {
    // load the other map (without entities)
    LoadMap(snap.iMapNumber,FALSE);
  }

  Vision::GetTimer()->SetTime(snap.fTime);

  // now re-create all the "RECREATE" entities
  VMemoryTempBuffer<1024> modelFileName;
  VMemoryTempBuffer<1024> entityParams;
  VMemoryTempBuffer<1024> entityKey;
  VMemoryTempBuffer<1024> className;

  // now re-create all the "RECREATE" entities
  for (i = 0; i < snap.iRecreateEntityCount; i++)
  {
    char* pszClassName;
    char* pszEntityKey;
    hkvVec3 vOrigin;
    hkvVec3 vOrientation;
    char* pszModelFile;
    char* pszEntityParams;
    // get entity data
    pszClassName = className.ReadStringBinary(ar);
    pszEntityKey = entityKey.ReadStringBinary(ar);
    vOrigin.SerializeAsVisVector (ar);
    vOrientation.SerializeAsVisVector (ar);
    pszModelFile = modelFileName.ReadStringBinary(ar);
    pszEntityParams = entityParams.ReadStringBinary(ar);
    SerializeBaseEntity_cl* pSerEnt = static_cast<SerializeBaseEntity_cl*>(
      Vision::Game.CreateEntity(pszClassName, vOrigin, pszModelFile,
      pszEntityParams));
    pSerEnt->SetOrientation(vOrientation);
    pSerEnt->SetEntityKey(pszEntityKey);
  }

  // finally, re-create all the "FULL" entities
  for (i = 0; i < snap.iFullEntityCount; i++)
  {
    SerializeBaseEntity_cl *pSerEnt;
    // get an entity from the archive...
    ar >> pSerEnt;
  }

  // check if the ARCHIVE_END_TAG exists
  ar.ReadStringBinary(szArchiveTag, FS_MAX_PATH);
  if (strcmp(szArchiveTag, ARCHIVE_END_TAG) != 0)
  {
    hkvLog::FatalError("No (or invalid) end tag found in save game!");
    ar.Close();
    pIn->Close();
    return FALSE;
  }
  
  ar.Close();
  pIn->Close();

  return TRUE;
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
