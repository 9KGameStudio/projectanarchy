/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef UPDOWNCAMERA_HPP_INCLUDED
#define UPDOWNCAMERA_HPP_INCLUDED

#include <Vision/Runtime/Framework/VisionApp/VAppImpl.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Input/VFreeCamera.hpp>

/// \brief
///   A free camera that uses the up vector when 'moving forward'.
class UpDownCamera : public VFreeCamera
{
public:
  virtual void InitFunction() HKV_OVERRIDE;
  virtual void DeInitFunction() HKV_OVERRIDE;

  // place camera control-buttons on screen
  void SetArrowControlPosition(int x, int y, int areaSize);

  // callback handler implementation
  virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData) HKV_OVERRIDE;

protected:
  virtual void GetCurrentMoveAxes(hkvVec3& vForward, hkvVec3& vRight, hkvVec3& vUp) const HKV_OVERRIDE;

private:
  // Private Variables
  IVMultiTouchInput *m_pMultiTouchInput;
  VisScreenMaskPtr m_spArrows;

  V_DECLARE_SERIAL(UpDownCamera,)
  V_DECLARE_VARTABLE(UpDownCamera,)
};

#endif

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
