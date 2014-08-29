/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VLUA_APIDOC

%nodefaultctor VisLightSource_cl;
%nodefaultdtor VisLightSource_cl;

class VisLightSource_cl : public VisObject3D_cl
{
public:
  void Remove();

  void SetColor(VColorRef iColor);
  VColorRef GetColor() const;

  inline void SetIntensity(float fNewIntensity);
  float GetIntensity() const;

  inline void SetMultiplier(float fMultiplier);
  inline float GetMultiplier() const;
 
  void SetProjectionAngle(float angle);
  float GetProjectionAngle() const;

  %extend{
  
    VSWIG_CONVERT_BOOL_GETTER_CONST(GetTriggered);
    
    int GetType() const
    {
      return (int)self->GetType();
    }
    
    void SetType(int iType)
    {
      self->SetType((VisLightSourceType_e) iType);
    }
    
  }

  void Trigger();

  inline float GetRadius() const;
  void SetRadius(float fRadius);

  void SetProjectionTexture(const char *szTexName);
  const char* GetProjectedTextureFileName() const;


 
  %extend{
    bool WasVisibleInLastFrame() const
    {
      return self->WasVisibleLastFrame(NULL) == TRUE;
    }

    bool WasVisibleInAnyLastFrame() const
    {
      return self->WasVisibleInAnyLastFrame() == TRUE;
    }
    
    void SetVisible(bool bVisible)
    {
      self->SetVisibleBitmask(bVisible ? VIS_ENTITY_VISIBLE : VIS_ENTITY_INVISIBLE);
    }
    
    bool IsVisible()
    {
      return (self->GetVisibleBitmask() & VIS_ENTITY_VISIBLE) != FALSE;
    }
    
    VSWIG_BITMASK_SETTER(SetVisibleBitmask);
    VSWIG_BITMASK_GETTER_CONST(GetVisibleBitmask);
       
    void SetLightInfluenceBitmasks(VBitmask * pWorldMask, VBitmask * pObjectMask)
    {
      if(pWorldMask && pObjectMask)
        self->SetLightInfluenceBitMasks(pWorldMask->Get(), pObjectMask->Get());
    }
    
    VBitmask GetLightInfluenceBitmask(bool bWorld = true)
    {
      if(bWorld)
        return VBitmask(self->GetLightInfluenceBitMaskWorld());
      else
        return VBitmask(self->GetLightInfluenceBitMaskEntity());
    }
    
    hkvAlignedBBox GetBoundingBox() 
    {
      hkvAlignedBBox newBox;
      self->GetBoundingBox(newBox);
      return newBox; //return by value
    }
    
    void SetColorCurve(const char * pszTex = NULL, float fTiming = 10.f, float fPhase = 0.f)
    {
      if (pszTex==NULL || pszTex[0]==0)
        self->DisableColorAnimations(); // disable anim
      else
        self->ColorAnimation().SetAnimCurve(pszTex, fTiming, fPhase); // enable anim
    }
  } 
};

//add lua tostring and concat operators
VSWIG_CREATE_CONCAT(VisLightSource_cl, 128, "[%s %s I:%1.1f M:%1.2f : %1.2f,%1.2f,%1.2f]", self->GetType()==VIS_LIGHT_POINT?"PointL":(self->GetType()==VIS_LIGHT_DIRECTED?"DirectedL":"SpotL"), (self->GetVisibleBitmask() & VIS_ENTITY_VISIBLE)!=FALSE?"V":"-", self->GetIntensity(), self->GetMultiplier(), self->GetPosition().x, self->GetPosition().y, self->GetPosition().z)
VSWIG_CREATE_TOSTRING(VisLightSource_cl, "%s: '%s' [%s %s I:%1.1f M:%1.2f : %1.2f,%1.2f,%1.2f]", self->GetClassTypeId()->m_lpszClassName, self->GetObjectKey(), self->GetType()==VIS_LIGHT_POINT?"PointLight":(self->GetType()==VIS_LIGHT_DIRECTED?"DirectedLight":"SpotLight"), (self->GetVisibleBitmask() & VIS_ENTITY_VISIBLE)!=FALSE?"V":"-", self->GetIntensity(), self->GetMultiplier(), self->GetPosition().x, self->GetPosition().y, self->GetPosition().z)


#else

/// \brief Wrapper class for light sources.
/// \see VisGame_cl::GetLight
/// \see VisGame_cl::CreateLight
class VisLightSource_cl : public VisObject3D_cl {
public:

  /// @name Common Light Source Functions
  /// @{
  
  /// \brief Removes this light source from the scene and destroys it.
  void Remove();
  
  /// \brief Sets the color of the light source. 
  /// \param color The new color. 
  void SetColor(VColorRef color);

  /// \brief Gets the color of the light source. 
  /// \return The color of the light source. 
  VColorRef GetColor();

  /// \brief Gets the radius of the light source.
  /// \param radius The new radius.
  void SetRadius(number radius);

  /// \brief Returns the scaling of the entity.
  /// \return The radius of the light source. 
  number GetRadius();
  
  /// \brief Get the type of the light source (LIGHT_POINT, LIGHT_DIRECTED or LIGHT_SPOT).
  /// \return The light source type (LIGHT_POINT, LIGHT_DIRECTED or LIGHT_SPOT).
  number GetType();
    
  /// \brief Set the light source type (LIGHT_POINT, LIGHT_DIRECTED or LIGHT_SPOT).
  /// \param type The light source type (LIGHT_POINT, LIGHT_DIRECTED or LIGHT_SPOT).
  /// \par Example
  ///   \code
  ///     function OnCreate(self) -- lua script attached to a light source
  ///       self:SetType(Vision.LIGHT_SPOT)
  ///     end
  ///   \endcode
  void SetType(number type);
  
  /// \brief Sets the intensity of the light source. 
  /// \param intensity The new intensity. 
  void SetIntensity(number intensity);
  
  /// \brief Gets the intensity of the light source.
  /// \return The intensity of the light source. 
  number GetIntensity();
  
  /// \brief Sets the multiplier of the light source.
  /// \param mul The new multiplier.
  void SetMultiplier(number mul);
  
  /// \brief Gets the multiplier of the light source.
  /// \return The multiplier of the light source. 
  number GetMultiplier();
  
  /// @}
  /// @name Visiblity / Lighting / ShadowCasting
  /// @{
  
  /// \brief Makes the light source visible or invisible in all contexts.
  /// \param visible The new visible status.
  /// \note This is a convenience function for SetVisibleBitmask.
  /// \see SetVisibleBitmask
  void SetVisible(boolean visible);

  /// \brief Checks whether the light source is visible in all contexts.
  /// \return true if visible; other false.
  /// \note This is a convenience function for GetVisibleBitmask.
  /// \see GetVisibleBitmask
  boolean IsVisible();

  /// \brief Makes the light source visible in certain rendering contexts only.
  /// \details
  ///    This light source will only be rendered in render contexts, if the logical AND result of this bitmask and the context's bitmask is not zero.
  /// \param mask The new visible bitmask.
  /// \see VBitmask
  void SetVisibleBitmask(VBitmask mask);

  /// \brief Gets the visible bitmask of the light source.
  /// \return The visible bitmask as VBitmask.
  /// \see VBitmask
  VBitmask GetVisibleBitmask();

  /// \brief Sets a bitmask that defines the light influence behaviour.
  /// \details
  ///    The light source will only influence objects have a matching bit in their bitmask.
  /// \param mask The light influence bitmask for this light source.
  /// \see VBitmask
  void SetLightInfluenceBitmask(VBitmask mask);

  /// \brief Returns the bitmask that defines the light influence behaviour. 
  /// \details
  ///    The light source will only influence objects have a matching bit in their bitmask.
  /// \return The light influence bitmask for this light source.
  /// \see VBitmask
  VBitmask GetLightInfluenceBitmask();
    
  /// \brief Gets the visibility status of this light source in the last rendered frame of the main render context. 
  /// \return If the light source has been visible in the last frame rendered in the main render context, the function returns true, otherwise false.
  boolean WasVisibleInLastFrame();

  /// \brief Gets the visibility status of this light source in the last rendered frame of all active render contexts. 
  /// \return If the light source has been visible in the last frame rendered in any of the contexts, the function returns true, otherwise false.
  boolean WasVisibleInAnyLastFrame();
  
  /// @}
  /// @name Appearance Details
  /// @{
  
  /// \brief Set the apex angle of a spotlight or projected light.
  /// \param angle The new angle.
  void SetProjectionAngle(number angle);
  
  /// \brief Returns the apex angle of a light source. 
  /// \return The angle of a spotlight or projected light. 
  number GetProjectionAngle();
  
  /// \brief Set the projection texture to be used for a spotlight. 
  /// \param texture The texture file used for projection.
  void SetProjectionTexture(string texture);
  
  /// \brief Get the projection texture file name of a spotlight.
  /// \return The texture file name used for projection.
  string GetProjectedTextureFileName();
  
  /// @}
  /// @name Light Animation
  /// @{
      
  /// \brief Trigger the light source's intensity and/or color animations. 
  /// \note Running intensity and/or color animations will pause/continue. 
  void Trigger();
  
  /// \brief Get the trigger status of the light source. Triggering can be used to pause and continue color and intensity animations.
  /// \return true if the light source is currently triggered. 
  boolean GetTriggered(); 
  
  /// \brief Enables (or disables - call parameterless) a color curve for this light source. Similar to the vForge light properties. 
  /// \param texture The texture file used for the animation curve. Can be nil to disable color curve.
  /// \param timing The time (in seconds) for one animation cycle.
  /// \param phase The animation phase (in 0..1 range) to start the animation.
  void SetColorCurve(string texture = nil, number timing = 10, number phase = 0);
      
  /// @}
  /// @name Bounding Boxe
  /// @{

  /// \brief Returns a bounding box around the sphere of influence of this light source. 
  /// \return The influence bounding box of the light source.
  hkvAlignedBBox GetBoundingBox();

  /// @}
  
};

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
