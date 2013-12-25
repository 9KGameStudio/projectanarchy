/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef SAVEGAMESLOT_HPP_INCLUDED
#define SAVEGAMESLOT_HPP_INCLUDED

//
// *** GameSnapshot_t ***
//
// Synopsis:
//    - Struct that holds some game state information and is used in the LoadGame
//      function
//
struct GameLoadData_t
{
  int iMapNumber;           // the map number
  float fTime;              // the game timer
  VBool bJustReset;         // TRUE if we just have to reset the current world and not load another one
  int iFullEntityCount;     // number of fully serialized entities
  int iRecreateEntityCount; // number of re-createable entities
};

//
// *** SaveGameSlot_cl ***
//
// Synopsis:
//    - A single instance of this class manages the HUD overlay for a single save game slot.
//      For instance, it creates a preview screenmask.
//
class SaveGameSlot_cl
{
public:
  // constructor
  SaveGameSlot_cl();

  // called one time after Vision initialization. Screenmasks are created here
  // Since we have a reference to them, they will survive a LoadWorld
  void OnInitOneTime(int iSlotIndex);

  // called directly before Vision de-initialization.
  void OnDeInitOneTime();

  // determines if a savegame corresponding to the slot exists
  BOOL IsSlotUsed(const char* szName) const;
  
  // updates the slot HUD
  void UpdateSlotStatus(int iCurrentSlot, const char* szName);

  // save a screenshot icon
  void SaveScreenShotPreview();

  inline void SetErrorState() {m_bErrorState=true;}
  inline void ResetErrorState() {m_bErrorState=false;}

  static const VColorRef s_iOverlayColor;

private:
  int m_iSlotIndex;   // 0-based savegame slot index
  bool m_bErrorState; // indicates whether this slot is currently in error state
  VisScreenMaskPtr m_spSlotNumMask;     // number mask of slot
  VisScreenMaskPtr m_spPreviewMask;     // preview picture
  VisScreenMaskPtr m_spBackgroundMask;  // "free" or "not available"
};

#endif //SAVEGAMESLOT_HPP_INCLUDED

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
