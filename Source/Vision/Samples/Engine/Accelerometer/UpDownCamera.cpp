/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Samples/Engine/Accelerometer/AccelerometerPCH.h>
#include <Vision/Samples/Engine/Accelerometer/UpDownCamera.hpp>

void UpDownCamera::InitFunction()
{
  VFreeCamera::InitFunction();

#if defined(SUPPORTS_MULTITOUCH)
  // Force the creation of the virtual thumb stick.
  // (Not created by VFreeCamera on Windows by default)
  CreateVirtualThumbStick();
#endif

  m_spArrows = NULL;

#if defined(_VISION_WIIU)
  // Use arrows to navigate
  m_spArrows = new VisScreenMask_cl("GUI/arrows.dds");
  m_spArrows->SetTransparency(VIS_TRANSP_ALPHA);

  m_pMultiTouchInput = &VInputManager::GetTouchScreen(V_DRC_FIRST);
  VTouchInputWiiU* pInput = static_cast<VTouchInputWiiU*>(m_pMultiTouchInput);
  pInput->SetEnabled(true);

  SetArrowControlPosition(0, Vision::Video.GetVideoConfig()->uiDRCRenderHeight - 120, 160);
#endif

}

void UpDownCamera::DeInitFunction()
{
  m_spArrows = NULL;

  VFreeCamera::DeInitFunction();
}

void UpDownCamera::GetCurrentMoveAxes(hkvVec3& vForward, hkvVec3& vRight, hkvVec3& vUp) const
{
  // check all movement keys
  hkvMat3 mat(hkvNoInitialization);
  GetRotationMatrix(mat);
  vForward = mat.getAxis(2); // forward is up
  vRight = mat.getAxis(1);
  vUp = hkvVec3::ZeroVector();
}

void UpDownCamera::SetArrowControlPosition(int x, int y, int areaSize)
{
#if defined(_VISION_WIIU)
  float fXScale = ((float)Vision::Video.GetXRes() / (float)Vision::Video.GetVideoConfig()->uiDRCRenderWidth);
  float fYScale = ((float)Vision::Video.GetYRes() / (float)Vision::Video.GetVideoConfig()->uiDRCRenderHeight);
  m_spArrows->SetPos((float)x * fXScale, (float)y * fYScale);
  m_spArrows->SetTargetSize((float)areaSize * fXScale, (float)areaSize * fXScale);
#else
  m_spArrows->SetPos((float)x, (float)y);
  m_spArrows->SetTargetSize((float)areaSize, (float)areaSize);
#endif

  int btnSize = areaSize>>2;
 
  GetInputMap()->UnmapInput(VFreeCamera::CONTROL_MOVE_FORWARD);
  GetInputMap()->UnmapInput(VFreeCamera::CONTROL_MOVE_BACKWARD);
  GetInputMap()->UnmapInput(VFreeCamera::CONTROL_MOVE_LEFT);
  GetInputMap()->UnmapInput(VFreeCamera::CONTROL_MOVE_RIGHT);

  VTouchArea* pForwardArea = new VTouchArea(*m_pMultiTouchInput, VRectanglef((float)(x + btnSize), (float)y, (float)(x + 2*btnSize), (float)(y + btnSize)), -900.0f);
  VTouchArea* pBackwardArea = new VTouchArea(*m_pMultiTouchInput, VRectanglef((float)(x + btnSize), (float)(y + 2*btnSize), (float)(x + 2*btnSize), (float)(y + 3*btnSize)), -900.0f);
  VTouchArea* pLeftArea = new VTouchArea(*m_pMultiTouchInput, VRectanglef((float)x, (float)(y + btnSize), (float)(x + btnSize), (float)(y + 2*btnSize)), -900.0f);
  VTouchArea* pRightArea = new VTouchArea(*m_pMultiTouchInput, VRectanglef((float)(x + 2*btnSize), (float)(y + btnSize), (float)(x + 3*btnSize), (float)(y + 2*btnSize)), -900.0f);

  GetInputMap()->MapTrigger(VFreeCamera::CONTROL_MOVE_FORWARD, pForwardArea, CT_TOUCH_ANY);
  GetInputMap()->MapTrigger(VFreeCamera::CONTROL_MOVE_BACKWARD, pBackwardArea, CT_TOUCH_ANY);
  GetInputMap()->MapTrigger(VFreeCamera::CONTROL_MOVE_LEFT, pLeftArea, CT_TOUCH_ANY);
  GetInputMap()->MapTrigger(VFreeCamera::CONTROL_MOVE_RIGHT, pRightArea, CT_TOUCH_ANY);
}

void UpDownCamera::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if (pData->m_pSender == &Vision::Callbacks.OnVideoChanged)
  {
    if (m_spArrows != NULL)
      SetArrowControlPosition(15, Vision::Video.GetYRes() - 214, 256);
  }

  VFreeCamera::OnHandleCallback(pData);
}

V_IMPLEMENT_SERIAL(UpDownCamera, VFreeCamera, 0, Vision::GetEngineModule());
START_VAR_TABLE(UpDownCamera, VFreeCamera, "UpDownCamera", 0, "")
END_VAR_TABLE

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
