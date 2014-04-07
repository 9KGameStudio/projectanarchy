/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VZoomDetector.hpp

#ifndef V_VZOOMDETECTOR_HPP_INCLUDED
#define V_VZOOMDETECTOR_HPP_INCLUDED

#if defined(SUPPORTS_MULTITOUCH)

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>

enum VZoomGesture
{
  CT_TOUCH_ZOOM,
  CT_TOUCH_ZOOM_IN,
  CT_TOUCH_ZOOM_OUT
};

class VZoomDetector : public IVInputDevice, public IVisCallbackHandler_cl
{
public:
  /// \brief
  ///   Constructor
  ///
  /// \param validArea
  ///   Specifies the are in which touches are taken into account. An invalid rectangle 
  ///   sets the area to the whole screen.
  ///
  /// \param fPriority
  ///   Specifies the priority in terms of touch areas. This makes sure that no touches are registered
  ///   where touch areas with a higher priority are present.
  ///   Note that this will not prevent touches from being registered for other touch areas.
  ///
  /// \param fMaxDistance
  ///   Upper bound for the maximum zooming distance. The control value will be normalized with it, so
  ///   that the resulting control value is in the range [-1, 1].
  ///   Set this to less than zero if the whole screen should be used.
  ///
  EFFECTS_IMPEXP VZoomDetector(const VRectanglef& validArea = VRectanglef(), float fPriority = -1000.0f, float fMaxDistance = -1.0f);
  EFFECTS_IMPEXP virtual ~VZoomDetector();

  /// \brief
  ///   Returns true if the users currently performs a zooming gesture.
  inline bool IsZooming() const { return m_bIsZooming; }

  EFFECTS_IMPEXP virtual void Reset() HKV_OVERRIDE;
  
  EFFECTS_IMPEXP virtual void Update(float timeDiff = 0.0f) HKV_OVERRIDE;
  
  EFFECTS_IMPEXP virtual bool IsActive() HKV_OVERRIDE;
  EFFECTS_IMPEXP virtual int GetRawControlValue(unsigned int uiControl) HKV_OVERRIDE;
  
  EFFECTS_IMPEXP virtual float GetControlValue(unsigned int uiControl, float fDeadZone, bool bTimeScaled = false) HKV_OVERRIDE;
  
  EFFECTS_IMPEXP virtual const char* GetName() HKV_OVERRIDE;

  EFFECTS_IMPEXP virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData) HKV_OVERRIDE;

  
private:
  VRectanglef m_validArea;
  float m_fPriority;
  
  float m_fMaxDistance;
  
  float m_fLastValue;
  float m_fValue;
  bool m_bIsZooming;
  
  bool m_bActive;
};

#endif

#endif

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
