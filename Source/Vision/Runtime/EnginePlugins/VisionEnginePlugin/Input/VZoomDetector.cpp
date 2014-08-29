/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Input/VZoomDetector.hpp>

#if defined(SUPPORTS_MULTITOUCH)

#include <Vision/Runtime/Base/Input/VInputTouch.hpp>

VZoomDetector::VZoomDetector(const VRectanglef& validArea, float fPriority, float fMaxDistance) 
  : m_validArea(validArea)
  , m_fPriority(fPriority)
  , m_fMaxDistance(fMaxDistance)
  , m_fLastValue(0.0f)
  , m_fValue(0.0f)
  , m_bIsZooming(false)
  , m_bActive(true)
{
  if (!m_validArea.IsValid())
  {
    m_validArea.Set(0.0f, 0.0f, (float)Vision::Video.GetXRes(), (float)Vision::Video.GetYRes());
  }
  
  if (m_fMaxDistance < 0.0f)
  {
    
    m_fMaxDistance = hkvVec2(m_validArea.GetSizeX(), m_validArea.GetSizeY()).getLength();
  }

  Vision::Callbacks.OnFrameUpdatePreRender += this;
}

VZoomDetector::~VZoomDetector()
{
  Vision::Callbacks.OnFrameUpdatePreRender -= this;
}

void VZoomDetector::Reset()
{
  m_fLastValue = 0.0f;
  m_fValue = 0.0f;
  m_bIsZooming = false;
}
  
void VZoomDetector::Update(float timeDiff)
{
  m_bIsZooming = false;

  if (!m_bActive)
    return;
  
  // Only allow zooming if there are exactly two touch points.
  IVMultiTouchInput& touchInput = VInputManager::GetTouchScreen();
  if (touchInput.GetNumberOfTouchPoints() != 2)
  {
    m_fLastValue = 0.0f;
    m_fValue = 0.0f;
    return;
  }
  
  // Find two valid touch points.
  int iTouchPoints[2];
  int iNumValidTouchPoints = 0;
  const VPListT<VTouchArea>& touchAreas = touchInput.GetTouchAreas();
  
  for (int i = 0; i < touchInput.GetMaximumNumberOfTouchPoints() && iNumValidTouchPoints < 2; i++)
  {
    const IVMultiTouchInput::VTouchPoint& touchPoint = touchInput.GetTouch(i);

    if (touchInput.IsActiveTouch(i) && touchPoint.IsInArea(m_validArea))
    {
      // Check if there is a touch area with a higher priority, which obscures the touch point.
      int iTouchAreaIndex = 0;
      for (; iTouchAreaIndex < touchAreas.GetLength(); iTouchAreaIndex++)
      {
        const VTouchArea* pTouchArea = touchAreas.Get(iTouchAreaIndex);
        const VRectanglef& rect = pTouchArea->GetArea();

        if (pTouchArea->GetPriority() > m_fPriority && touchPoint.IsInArea(rect))
          break;
      }

      // All touch areas passed?
      if (iTouchAreaIndex == touchAreas.GetLength())
      {
        iTouchPoints[iNumValidTouchPoints++] = i;
      }
    }
  }
  
  if (iNumValidTouchPoints < 2)
    return;
  
  const float dx = touchInput.GetTouch(iTouchPoints[0]).fXAbsolute - touchInput.GetTouch(iTouchPoints[1]).fXAbsolute;
  const float dy = touchInput.GetTouch(iTouchPoints[0]).fYAbsolute - touchInput.GetTouch(iTouchPoints[1]).fYAbsolute;
  const float fDistance = hkvMath::sqrt(dx * dx + dy * dy);
  
  if (m_fLastValue == 0.0f)
    m_fLastValue = fDistance;
  else
    m_fLastValue = m_fValue;
  
  m_fValue = fDistance;
  m_bIsZooming = true;
}
  
bool VZoomDetector::IsActive()
{
  return m_bActive;
}

int VZoomDetector::GetRawControlValue(unsigned int uiControl)
{
  const int iDiff = static_cast<int>(m_fValue - m_fLastValue + 0.5f);

  switch (uiControl)
  {
    case CT_TOUCH_ZOOM:
      return iDiff;
    case CT_TOUCH_ZOOM_IN:
      return hkvMath::Max(iDiff, 0);
    case CT_TOUCH_ZOOM_OUT:
      return hkvMath::Max(-iDiff, 0);
  }
  return 0;
}

float VZoomDetector::GetControlValue(unsigned int uiControl, float fDeadZone, bool bTimeScaled)
{
  return hkvMath::clamp(GetRawControlValue(uiControl) / m_fMaxDistance, -1.0f, 1.0f);  
}

const char* VZoomDetector::GetName()
{
  return "Zoom Detector";
}

void VZoomDetector::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  // OnFrameUpdatePreRender
  if (pData->m_pSender == &Vision::Callbacks.OnFrameUpdatePreRender)
  {
    Update();
  }
}

#endif

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
