/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VLUA_APIDOC

class VScriptWorld_wrapper
{
public:
  VScriptWorld_wrapper();
  ~VScriptWorld_wrapper(); 

  void SetDepthFogMode(int fogMode);

  int GetDepthFogMode();

  void SetDepthFogStart(float depthStart);

  float GetDepthFogStart() const;

  void SetDepthFogEnd(float depthEnd);

  float GetDepthFogEnd() const;

  void SetDepthFogColor(VColorRef color);

  VColorRef GetDepthFogColor() const;

  void SetMaskSky(bool maskSky);

  bool IsMaskSkyEnabled() const;

  void SetVirtualSkyDepth(float virtualSkyDepth);

  float GetVirtualSkyDepth() const;

};

#else

/// \brief This wrapper contains functions related to depth fog and appears in Lua as \b World. It is analog to the
///   Vision::World object and exposes a subset of its methods.
/// \par Example
///   \code
///     function OnAfterSceneLoaded(self)
///       World:SetDepthFogMode(Vision.DEPTH_FOG_ON)
///       local color = Vision.VColorRef(255,0,0)
///       World:SetDepthFogColor(color)
///       World:SetDepthFogStart(100)
///       World:SetDepthFogEnd(1000)
///       World:SetVirtualSkyDepth(900) --sky almost completely in fog
///       World:SetMaskSky(false) --fog is applied to sky
///     end
///
///     function OnThink(self)
///       if(Input:IsKeyPressed(Vision.KEY_SPACE)) then
///         local color = Vision.VColorRef(0,255,0)
///         World:SetDepthFogColor(color)
///       end
///     end
///   \endcode
class VScriptWorld_wrapper {
public:

  /// @name Depth Fog Control
  /// @{

  /// \brief Set the depth fog mode (Vision.DEPTH_FOG_OFF, Vision.DEPTH_FOG_ON or Vision.DEPTH_FOG_TIME_OF_DAY)
  /// \param fogMode The depth fog mode, specified by one of the following types:
  ///   - \b Vision.DEPTH_FOG_OFF
  ///   - \b Vision.DEPTH_FOG_ON
  ///   - \b Vision.DEPTH_FOG_TIME_OF_DAY
  /// \par Example
  ///   \code
  ///     function OnAfterSceneLoaded(self)
  ///       World:SetDepthFogMode(Vision.DEPTH_FOG_TIME_OF_DAY)
  ///     end
  ///   \endcode
  void SetDepthFogMode(number fogMode);

  /// \brief Returns the current depth fog mode (Vision.DEPTH_FOG_OFF, Vision.DEPTH_FOG_ON or Vision.DEPTH_FOG_TIME_OF_DAY)
  /// \return The current depth fog mode (Vision.DEPTH_FOG_OFF, Vision.DEPTH_FOG_ON or Vision.DEPTH_FOG_TIME_OF_DAY)
  /// \par Example
  ///   \code
  ///     function OnAfterSceneLoaded(self)
  ///       local fogMode = World:GetDepthFogMode()
  ///       if(fogMode == Vision.DEPTH_FOG_ON) then
  ///         World:SetDepthFogColor(Vision.V_RGBA_PURPLE)
  ///       end
  ///     end
  ///   \endcode
  number GetDepthFogMode();

  /// \brief Set the near z value for depth fog
  /// \param depthStart The new start value for depth fog
  /// \par Example
  ///   \code
  ///     ...
  ///     World:SetDepthFogStart(100)
  ///     ...
  ///   \endcode
  void SetDepthFogStart(number depthStart);

  /// \brief Returns the current near z value of depth fog
  /// \return The current near z value of depth fog
  /// \par Example
  ///   \code
  ///     ...
  ///     local depthFogStart = World:GetDepthFogStart()
  ///     ...
  ///   \endcode
  number GetDepthFogStart();

  /// \brief Set the far z value for depth fog
  /// \param depthEnd The new end value for depth fog
  /// \par Example
  ///   \code
  ///     ...
  ///     World:SetDepthFogEnd(1000)
  ///     ...
  ///   \endcode
  void SetDepthFogEnd(number depthEnd);

  /// \brief Returns the current far z value of depth fog
  /// \return The current far z value of depth fog
  /// \par Example
  ///   \code
  ///     ...
  ///     local depthFogEnd = World:GetDepthFogEnd()
  ///     ...
  ///   \endcode
  number GetDepthFogEnd();

  /// \brief Set the depth fog color
  /// \param color The new color for depth fog
  /// \par Example
  ///   \code
  ///     ...
  ///     World:SetDepthFogColor(Vision.V_RGBA_WHITE)
  ///     ...
  ///   \endcode  
  void SetDepthFogColor(VColorRef color);

  /// \brief Returns the current depth fog color
  /// \return The current depth fog color
  /// \par Example
  ///   \code
  ///     ...
  ///     local fogColor = World:GetDepthFogColor()
  ///     if(fogColor == Vision.V_RGBA_CYAN) then
  ///       ...
  ///     end
  ///     ...
  ///   \endcode  
  VColorRef GetDepthFogColor();

  /// \brief Sets if depth fog is applied to sky
  /// \param maskSky If true, fog will NOT be applied to the sky
  /// \par Example
  ///   \code
  ///     function OnAfterSceneLoaded(self)
  ///       World:SetDepthFogMode(Vision.DEPTH_FOG_ON)
  ///       World:SetMaskSky(false) -- Apply fog to sky
  ///     end
  ///   \endcode
  void SetMaskSky(boolean maskSky);

  /// \brief Returns if depth fog is applied to the sky
  /// \return If true, the sky is masked out from fog, meaning that the fog is NOT applied to the sky
  /// \par Example
  ///   \code
  ///     ...
  ///     local maskSky = World:IsMaskSkyEnabled() --if true, fog is not applied to sky
  ///     ...
  ///   \endcode  
  boolean IsMaskSkyEnabled();

  /// \brief Sets the virtual depth of the sky (only applies when mask sky is disabled).
  ///    When fog is rendered over regular surfaces the perceived fog density is determined by the distance from the viewer to the surface point.
  ///    Since the sky does not have a defined distance from the viewer, the virtual depth of the sky
  ///    can be set to allow determining the perceived density of the fog against the sky.
  /// \param virtualSkyDepth Virtual depth of the sky. Use -1 to set to the far plane.
  /// \par Example
  ///   \code
  ///     function OnAfterSceneLoaded()
  ///       World:SetDepthFogStart(100)
  ///       World:SetDepthFogEnd(1000)
  ///       World:SetVirtualSkyDepth(900) --sky is almost completely in fog
  ///     end
  ///   \endcode
  void SetVirtualSkyDepth(number virtualSkyDepth);

  /// \brief Returns the virtual depth of the sky. When fog is rendered over regular surfaces
  ///    the perceived fog density is determined by the distance from the viewer to the surface point.
  ///    Since the sky does not have a defined distance from the viewer, the virtual depth of the sky
  ///    can be set to allow determining the perceived density of the fog against the sky.
  /// \return Virtual depth of the sky. -1 means that the far plane is used as virtual depth.
  /// \par Example
  ///   \code
  ///     ...
  ///     local virtualSkyDepth = World:GetVirtualSkyDepth()
  ///     ...
  ///   \endcode
  number GetVirtualSkyDepth();

  /// @}
};

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
