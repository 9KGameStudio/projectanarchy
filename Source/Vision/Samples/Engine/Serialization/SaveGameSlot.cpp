/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Samples/Engine/Serialization/SerializationPCH.h>
#include <Vision/Samples/Engine/Serialization/SaveGameSlot.hpp>
#include <Vision/Samples/Engine/Serialization/SerializePlayer.hpp>
#include <Vision/Runtime/Framework/VisionApp/VAppImpl.hpp>

#define SAVEGAME_PREVIEW  "SaveGames\\save%1d.bmp"

#define SAVESLOT_FILE "Serialization\\slot%01i.dds"
#define SAVESLOT_COL_ERROR VColorRef(255, 128, 128, 255)
#define SAVESLOT_COL_LOAD  VColorRef(128, 255, 128, 255)
#define SAVESLOT_COL_SAVE  VColorRef(255, 255, 128, 255)
#define SAVESLOT_COL_NORM  VColorRef(255, 255, 255, 255)
#define SAVESLOT_COL_EMPTY VColorRef(128, 128, 128, 255)

const VColorRef SaveGameSlot_cl::s_iOverlayColor = VColorRef(255, 255, 255, 190);

SaveGameSlot_cl::SaveGameSlot_cl()
{
  m_iSlotIndex = 0;
  m_bErrorState = false;
}

void SaveGameSlot_cl::OnInitOneTime(int iSlotIndex)
{
  m_iSlotIndex = iSlotIndex;
  char pszSlotFile[64];
  sprintf(pszSlotFile, SAVESLOT_FILE, m_iSlotIndex + 1);

#if defined( _VISION_MOBILE )
  // Arrange in two columns, if the screen is too small
  bool bTwoColumns = Vision::Video.GetYRes() < 520;
  int iRows = bTwoColumns ? 2 : 4;

  float fPosX = Vision::Video.GetXRes() - 132.0f;
  if (bTwoColumns && (iSlotIndex < iRows))
    fPosX -= 132.0f;
  float fThumbnailPosY = 8.0f + ((m_iSlotIndex % iRows) * 128.0f);
  float fSlotPosY = fThumbnailPosY + 56.0f;

  VTouchArea* pSlotArea = new VTouchArea( VInputManager::GetTouchScreen(), 
    VRectanglef( fPosX, fThumbnailPosY, fPosX + 128.0f, fThumbnailPosY + 96.0f ), 
    0.0f );

  VAppImpl::GetInputMap()->MapTrigger( SERIALIZE_SLOT_1 + m_iSlotIndex, pSlotArea, CT_TOUCH_ANY, VInputOptions::Once() );
#else
  float fPosX = 8.0f + m_iSlotIndex * 132.0f;
  float fThumbnailPosY = Vision::Video.GetYRes() - 112.0f;
  float fSlotPosY = fThumbnailPosY + 56.0f;
#endif

  // screenmask for the slot number
  m_spSlotNumMask = new VisScreenMask_cl(pszSlotFile);
  m_spSlotNumMask->SetPos( fPosX, fSlotPosY );
  m_spSlotNumMask->SetTransparency(VIS_TRANSP_ALPHA);
  m_spSlotNumMask->SetColor(s_iOverlayColor);
  m_spSlotNumMask->SetOrder(100);
  m_spSlotNumMask->SetVisible(FALSE);

  // screenmask for either "No Preview" or "Free"
  m_spBackgroundMask = new VisScreenMask_cl("Serialization\\NoPreview.dds");
  m_spBackgroundMask->SetPos( fPosX, fThumbnailPosY );
  m_spBackgroundMask->SetTargetSize(128.f,96.f);
  m_spBackgroundMask->SetTransparency(VIS_TRANSP_ALPHA);
  m_spBackgroundMask->SetColor(s_iOverlayColor);
  m_spBackgroundMask->SetOrder(110);
  m_spBackgroundMask->SetVisible(FALSE);

  // screenmask for the level preview screenshot
  m_spPreviewMask = new VisScreenMask_cl();
  m_spPreviewMask->SetPos( fPosX, fThumbnailPosY );
  m_spPreviewMask->SetTargetSize(128.f,96.f);
  m_spPreviewMask->SetTransparency(VIS_TRANSP_ALPHA);
  m_spPreviewMask->SetColor(s_iOverlayColor);
  m_spPreviewMask->SetOrder(110);
  m_spPreviewMask->SetVisible(FALSE);
}

void SaveGameSlot_cl::OnDeInitOneTime()
{
  // release smart pointer references
  m_spSlotNumMask = NULL;
  m_spBackgroundMask = NULL;
  m_spPreviewMask = NULL;
}

BOOL SaveGameSlot_cl::IsSlotUsed(const char* szName) const
{
  char pszSaveFileName[FS_MAX_PATH];
  sprintf(pszSaveFileName, szName, m_iSlotIndex + 1);
  return Vision::File.Exists(pszSaveFileName);
}

void SaveGameSlot_cl::UpdateSlotStatus(int iCurrentSlot, const char* szName)
{
  BOOL bSlotUsed = IsSlotUsed(szName);
  BOOL bShowPreview = FALSE;
  if (bSlotUsed)
  {
    // try to load preview bmp
    char pszPreviewFileName[FS_MAX_PATH];
    sprintf(pszPreviewFileName, SAVEGAME_PREVIEW, m_iSlotIndex + 1);
    if (Vision::File.Exists(pszPreviewFileName))
    {
      VTextureObjectPtr spScreenMaskTexture(m_spPreviewMask->GetTextureObject());
      if (spScreenMaskTexture)
      {
        spScreenMaskTexture->EnsureUnloaded();
      }
      m_spPreviewMask->Unload();
      m_spPreviewMask->LoadFromFile(pszPreviewFileName);
      bShowPreview = TRUE;
    }
    else
    {
      m_spBackgroundMask->LoadFromFile("Serialization\\NoPreview.dds");
    }
  }
  else
  {
    m_spBackgroundMask->LoadFromFile("Serialization\\Free.dds");
  }
  // target size gets reset in LoadFromFile 
  m_spBackgroundMask->SetTargetSize(128.f,96.f);
  m_spPreviewMask->SetTargetSize(128.f,96.f);

  // either show preview or "free"
  m_spBackgroundMask->SetVisible(!bShowPreview);
  m_spPreviewMask->SetVisible(bShowPreview);

  // set slot number color according to status 
  VColorRef iColor = SAVESLOT_COL_NORM;
  if (m_bErrorState)
    iColor = SAVESLOT_COL_ERROR;
  else if (!bSlotUsed)
    iColor = SAVESLOT_COL_EMPTY;
  else if (iCurrentSlot==m_iSlotIndex) 
    iColor = SAVESLOT_COL_LOAD;

  m_spSlotNumMask->SetColor(s_iOverlayColor);
  m_spSlotNumMask->SetVisible(TRUE);
}

void SaveGameSlot_cl::SaveScreenShotPreview()
{
  const int w = Vision::Video.GetXRes();
  const int h = Vision::Video.GetYRes();

  unsigned char *pTempBuffer = new unsigned char [w*h*3];
  char pszPreviewFileName[FS_MAX_PATH];
  sprintf(pszPreviewFileName, SAVEGAME_PREVIEW, m_iSlotIndex + 1);

  // write the screen buffer to memory buffer
  if (Vision::Game.WriteScreenToBuffer(0,0,w,h,(UBYTE *)pTempBuffer))
  {
    // use VTEX to scale and save the image
    Image_cl image;
    ImageMap_cl colorMap(w,h,24, (UBYTE*)pTempBuffer);
    image.AddColorMap(colorMap);
    image.Scale(128,128);
    image.SaveBMP(pszPreviewFileName);
  }
  delete []pTempBuffer;

  // make sure texture is reloaded next time
  if (m_spPreviewMask->GetTextureObject())
    m_spPreviewMask->GetTextureObject()->EnsureUnloaded();
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
