/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VScriptWorld_wrapper.hpp

#ifndef __VSCRIPTWORLD_WRAPPER_HPP
#define __VSCRIPTWORLD_WRAPPER_HPP

/// 
/// \brief
///   Small wrapper around the Vision::World object to allow access to e.g. the fog configuration
/// 
class VScriptWorld_wrapper
{
public:

  VScriptWorld_wrapper() {}
  ~VScriptWorld_wrapper() {}

  void SetDepthFogMode(int fogMode)
  {
    if(fogMode < (int)VFogParameters::Off || fogMode > (int)VFogParameters::TimeOfDay)
    {
      hkvLog::Warning("LUA method World::SetDepthFogMode - parameter iFogMode is not valid, not changing mode.");
      return;
    }

    VFogParameters fog = Vision::World.GetFogParameters();
    fog.depthMode = (VFogParameters::Mode)fogMode;
    Vision::World.SetFogParameters(fog);
  }

  int GetDepthFogMode()
  {
    VFogParameters fog = Vision::World.GetFogParameters();
    return (int)fog.depthMode;
  }

  void SetDepthFogStart(float depthStart)
  {
    VFogParameters fog = Vision::World.GetFogParameters();
    fog.fDepthStart = depthStart;
    Vision::World.SetFogParameters(fog);
  }

  float GetDepthFogStart() const
  {
    VFogParameters fog = Vision::World.GetFogParameters();
    return fog.fDepthStart;
  }

  void SetDepthFogEnd(float depthEnd)
  {
    VFogParameters fog = Vision::World.GetFogParameters();
    fog.fDepthEnd = depthEnd;
    Vision::World.SetFogParameters(fog);
  }

  float GetDepthFogEnd() const
  {
    VFogParameters fog = Vision::World.GetFogParameters();
    return fog.fDepthEnd;
  }

  void SetDepthFogColor(VColorRef color)
  {
    VFogParameters fog = Vision::World.GetFogParameters();
    fog.iDepthColor = color;
    Vision::World.SetFogParameters(fog);
  }

  VColorRef GetDepthFogColor() const
  {
    VFogParameters fog = Vision::World.GetFogParameters();
    return fog.iDepthColor;
  }

  void SetMaskSky(bool maskSky)
  {
    VFogParameters fog = Vision::World.GetFogParameters();
    fog.bMaskSky = maskSky;
    Vision::World.SetFogParameters(fog);
  }

  bool IsMaskSkyEnabled() const
  {
    VFogParameters fog = Vision::World.GetFogParameters();
    return fog.bMaskSky;
  }

  void SetVirtualSkyDepth(float virtualSkyDepth)
  {
    VFogParameters fog = Vision::World.GetFogParameters();
    fog.fVirtualSkyDepth = virtualSkyDepth;
    Vision::World.SetFogParameters(fog);
  }

  float GetVirtualSkyDepth() const
  {
    VFogParameters fog = Vision::World.GetFogParameters();
    return fog.fVirtualSkyDepth;
  }

};

#endif // __VSCRIPTWORLD_WRAPPER_HPP

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
