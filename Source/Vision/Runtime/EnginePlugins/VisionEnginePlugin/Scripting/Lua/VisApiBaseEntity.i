/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VLUA_APIDOC

%nodefaultctor VisBaseEntity_cl;
%nodefaultdtor VisBaseEntity_cl;

class VisBaseEntity_cl : public VisObject3D_cl
{
public:
 
  void Remove();
  
  virtual void InitFunction();
  
  virtual void DeInitFunction();

  VDynamicMesh* GetMesh() const;

  %extend{
    bool SetMesh(const char *szMeshFile)
    {
      return self->SetMesh(szMeshFile) == TRUE;
    }
    
    bool SetMesh(VDynamicMesh* pMesh)
    {
      self->SetMesh(pMesh);
      return true; //unify the return signature
    }

    void SetThinkFunctionStatus(bool bEnable)
    {
      for(int i=0; i<self->Components().Count();i++)
      {
        if( self->Components().GetPtrs()[i]->IsOfType(VScriptComponent::GetClassTypeId()) )
        {
          ((VScriptComponent *)(self->Components().GetPtrs()[i]))->SetThinkFunctionStatus(bEnable);
        }
      }
      self->SetThinkFunctionStatus(bEnable?TRUE:FALSE);
    }
    
    VSWIG_CONVERT_BOOL_GETTER_CONST(GetThinkFunctionStatus);
    
    VSWIG_CONVERT_BOOL_GETTER_CONST(HasMesh);
    
    VSWIG_CONVERT_BOOL_SETTER(SetCastShadows);
    VSWIG_CONVERT_BOOL_GETTER_CONST(GetCastShadows);
   
    bool SetScaling(float fScaling)
    {
      return self->SetScaling(fScaling)==TRUE;
    }
    
    bool SetScaling(float x, float y, float z)
    {
      return self->SetScaling(hkvVec3(x,y,z))==TRUE;
    }
    
    bool SetScaling(const hkvVec3* pScale)
    {
      if(pScale) return self->SetScaling(*pScale)==TRUE;
      return false;
    }
    
    VSWIG_RETURN_BY_VALUE_CONST(hkvVec3, GetScaling);
  }

  void SetAlwaysInForeGround(bool status = true);
  bool IsObjectAlwaysInForegroundEnabled();

  inline void SetCastStaticShadows(bool bStatus);
  inline bool GetCastStaticShadows();

  //no 4x4 matrices for now
  //void GetWorldMatrix( hkvMat4& WorldMatrix); 

  void SetCustomTraceBBox(const hkvAlignedBBox *pCustomTraceBBox);

  //hide context from Lua interface
  %extend{
    hkvAlignedBBox GetCustomTraceBBox() 
    {
      return *(self->GetCustomTraceBBox()); //return by value
    }
    
    void SetTraceAccuracy(int iTraceAccuracy)
    {
      self->SetTraceAccuracy((VisTraceAccuracy_e) iTraceAccuracy);
    }
    
    bool WasVisibleInLastFrame() const
    {
      return self->WasVisibleLastFrame(NULL) == TRUE;
    }

    bool WasVisibleInAnyLastFrame() const
    {
      return self->WasVisibleInAnyLastFrame() == TRUE;
    }
    
    hkvAlignedBBox GetBoundingBox() 
    {
      hkvAlignedBBox newBox;
      self->GetCurrentVisBoundingBox(newBox);
      return newBox; //return by value
    }
    
    hkvAlignedBBox GetCollisionBoundingBox()
    {
      hkvAlignedBBox newBox;
      self->GetCollisionBoundingBox(newBox);
      return newBox; //return by value
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

    VSWIG_BITMASK_SETTER_RENAME(SetLightInfluenceBitMask, SetLightInfluenceBitmask);
    VSWIG_BITMASK_GETTER_CONST_RENAME(GetLightInfluenceBitMask, GetLightInfluenceBitmask);

	bool SetEffectForAllSurfaces(const char* szShaderLib, const char* szEffect, const char* szParamString = "")
	{
	  if(!self->HasMesh()) return false;
	  return VScriptRenderer_wrapper::SetEffectForEntity(self, -1, szShaderLib, szEffect, szParamString);
	}

	bool SetEffectForAllSurfaces(const char* szShaderLib, const char* szEffect, const VMap<VString,VString>& params)
	{
	  if(!self->HasMesh()) return false;
	  return VScriptRenderer_wrapper::SetEffectForEntity(self, -1, szShaderLib, szEffect, params);
	}

    bool SetEffect(VisSurface_cl *pSurfaceToChange, const char * szShaderLib, const char * szEffect, const char * szParamString = "")
    {
      if(!self->HasMesh()) return false;
	  int iSurfaceToChangeIndex = VScriptRenderer_wrapper::FindSurfaceIndex(self, pSurfaceToChange);
	  if(iSurfaceToChangeIndex < 0)
	    return false;
      return VScriptRenderer_wrapper::SetEffectForEntity(self, iSurfaceToChangeIndex, szShaderLib, szEffect, szParamString);
    }

    bool SetEffect(int iSurfaceToChangeIndex, const char * szShaderLib, const char * szEffect, const char * szParamString = "")
    {
      if(!self->HasMesh()) return false;
      if(iSurfaceToChangeIndex >= self->GetMesh()->GetSurfaceCount() || iSurfaceToChangeIndex < 0) return false;
      
      return VScriptRenderer_wrapper::SetEffectForEntity(self, iSurfaceToChangeIndex, szShaderLib, szEffect, szParamString);
    }

    bool SetEffect(const char * szSurfaceName, const char * szShaderLib, const char * szEffect, const char * szParamString = "")
    {
      if(!self->HasMesh()) return false;
      VisSurface_cl *pSurfaceToChange = self->GetMesh()->GetSurfaceByName(szSurfaceName);
	  int iSurfaceToChangeIndex = VScriptRenderer_wrapper::FindSurfaceIndex(self, pSurfaceToChange);
	  if(iSurfaceToChangeIndex < 0)
	    return false;
      return VScriptRenderer_wrapper::SetEffectForEntity(self, iSurfaceToChangeIndex, szShaderLib, szEffect, szParamString);
    }

    bool SetEffect(VisSurface_cl *pSurfaceToChange, const char * szShaderLib, const char * szEffect, const VMap<VString,VString>& params)
    {
      if(!self->HasMesh()) return false;
	  int iSurfaceToChangeIndex = VScriptRenderer_wrapper::FindSurfaceIndex(self, pSurfaceToChange);
	  if(iSurfaceToChangeIndex < 0)
	    return false;
      return VScriptRenderer_wrapper::SetEffectForEntity(self, iSurfaceToChangeIndex, szShaderLib, szEffect, params);
    }

    bool SetEffect(int iSurfaceToChangeIndex, const char * szShaderLib, const char * szEffect, const VMap<VString,VString>& params)
    {
      if(!self->HasMesh()) return false;
      if(iSurfaceToChangeIndex>=self->GetMesh()->GetSurfaceCount() || iSurfaceToChangeIndex<0) return false;
      return VScriptRenderer_wrapper::SetEffectForEntity(self, iSurfaceToChangeIndex, szShaderLib, szEffect, params);
    }

    bool SetEffect(const char * szSurfaceName, const char * szShaderLib, const char * szEffect, const VMap<VString,VString>& params)
    {
      if(!self->HasMesh()) return false;
      VisSurface_cl *pSurfaceToChange = self->GetMesh()->GetSurfaceByName(szSurfaceName);
	  int iSurfaceToChangeIndex = VScriptRenderer_wrapper::FindSurfaceIndex(self, pSurfaceToChange);
	  if(iSurfaceToChangeIndex < 0)
	    return false;
      return VScriptRenderer_wrapper::SetEffectForEntity(self, iSurfaceToChangeIndex, szShaderLib, szEffect, params);
    }

	bool RemoveCustomTextureSet()
	{
	  if(!self->HasMesh()) return false;
	  self->SetCustomTextureSet(NULL);
	  return true;
	}

    void GetEffectParams(VMap<VString, VString>& OUTPUT, int iSurfaceIndex)
    {
      if(!self->HasMesh())
        return;
      if(iSurfaceIndex >= self->GetMesh()->GetSurfaceCount() || iSurfaceIndex < 0)
        return;
      VisSurface_cl *pSurface = self->GetMesh()->GetSurface(iSurfaceIndex);
      return VScriptRenderer_wrapper::GetParamsFromSurface(pSurface, OUTPUT);
    }

    void GetEffectParams(VMap<VString, VString>& OUTPUT, const char * szSurfaceName)
    {
      if(!self->HasMesh())
        return;
      VisSurface_cl *pSurface = self->GetMesh()->GetSurfaceByName(szSurfaceName);
      return VScriptRenderer_wrapper::GetParamsFromSurface(pSurface, OUTPUT);
    }

    void GetEffectParams(VMap<VString, VString>& OUTPUT,  VisSurface_cl *pSurface)
    {
      return VScriptRenderer_wrapper::GetParamsFromSurface(pSurface, OUTPUT);
    }
    
    bool SetTextureForSurface(VTextureObject *pTexture, VisSurface_cl *pSurface, int iTextureType = 0)
    {
      return VScriptRenderer_wrapper::SetTextureForSurface(self, pTexture, pSurface, NULL, -1, iTextureType);
    }
    
    bool SetTextureForSurface(VTextureObject *pTexture, const char *szSurfaceName, int iTextureType = 0)
    {
      return VScriptRenderer_wrapper::SetTextureForSurface(self, pTexture, NULL, szSurfaceName, -1, iTextureType);
    }
    
    bool SetTextureForSurface(VTextureObject *pTexture, int iSurfaceIndex, int iTextureType = 0)
    {
      return VScriptRenderer_wrapper::SetTextureForSurface(self, pTexture, NULL, NULL, iSurfaceIndex, iTextureType);
    }
    
    bool SetTextureForSurface(const char *szTextureFile, VisSurface_cl *pSurface, int iTextureType = 0)
    {
      VTextureObject *pTexture = Vision::TextureManager.Load2DTexture(szTextureFile);
      return VScriptRenderer_wrapper::SetTextureForSurface(self, pTexture, pSurface, NULL, -1, iTextureType);
    }
    
    bool SetTextureForSurface(const char *szTextureFile, const char *szSurfaceName, int iTextureType = 0)
    {
      VTextureObject *pTexture = Vision::TextureManager.Load2DTexture(szTextureFile);
      return VScriptRenderer_wrapper::SetTextureForSurface(self, pTexture, NULL, szSurfaceName, -1, iTextureType);
    }
    
    bool SetTextureForSurface(const char *szTextureFile, int iSurfaceIndex, int iTextureType = 0)
    {
      VTextureObject *pTexture = Vision::TextureManager.Load2DTexture(szTextureFile);
      return VScriptRenderer_wrapper::SetTextureForSurface(self, pTexture, NULL, NULL, iSurfaceIndex, iTextureType);
    }
    
    void ClearShaderSet()
    {
      self->SetShaderSet(NULL);
    }

    void SetPrimarySortingKey (int iKey) 
    {
      self->SetPrimarySortingKey((signed char)iKey);
    }
    
    int GetPrimarySortingKey() 
    {
      return self->GetPrimarySortingKey();
    } 
  }

  void  SetFarClipDistance(float farClipDistance);
  float  GetFarClipDistance () const ;

  //void SetAmbientColor(VColorRef iColor);
  //VColorRef GetAmbientColor() const;
};

//add lua tostring and concat operators
VSWIG_CREATE_CONCAT(VisBaseEntity_cl, 128, "[%s/%s/%s : %1.2f,%1.2f,%1.2f]", (self->GetVisibleBitmask() & VIS_ENTITY_VISIBLE)!=FALSE?"V":"-", self->GetMesh()?"M":"-", self->GetThinkFunctionStatus()?"T":"-", self->GetPosition().x, self->GetPosition().y, self->GetPosition().z )
VSWIG_CREATE_TOSTRING_EX(VisBaseEntity_cl, hkvVec3 vPos = self->GetPosition();, "%s: '%s' [%s/%s/%s : %1.2f,%1.2f,%1.2f]", self->GetClassTypeId()->m_lpszClassName, self->GetObjectKey(), (self->GetVisibleBitmask() & VIS_ENTITY_VISIBLE)!=FALSE?"V":"-", self->GetMesh()?"M":"-", self->GetThinkFunctionStatus()?"T":"-", vPos.x, vPos.y, vPos.z)

#else

/// \brief Base class for objects with models.
/// \see VisGame_cl::GetEntity
/// \see VisGame_cl::CreateEntity
class VisBaseEntity_cl : public VisObject3D_cl {
public:

  /// @name Common Entity Functions
  /// @{
  
  /// \brief Removes this entity from the scene and destroys it.
  void Remove();
  
  /// \brief Execute init code of your enity class.
  /// \note The InitFunction is automatically called for dynamically created entities. It is NOT called for
  ///   entities loaded from a VSCENE file, the constructor and Serialize function will be called
  ///   instead.
  ///   vForge recreates the entity instances when switching between edit and play-the-game mode.
  ///   This will also trigger the InitFunction of the entities from within vForge.
  ///   However it could be useful to call this method manully. 
  /// \see DeInitFunction
  virtual void InitFunction();
  
  /// \brief Execute de-init code of your enity class
  ///  \note It is guaranteed that the virtual DeInitFunction gets called before the entity is deleted.
  ///  However it could be useful to call this method manully.
  /// \see InitFunction
  virtual void DeInitFunction();
  
  /// \brief Sets the scaling of the entity in x,y,z directions.
  /// \param scaling The scaling in x,y,z directions as hkvVec3.
  /// \return true if scaling was successful, otherwise false.
  bool SetScaling(hkvVec3 scaling);
  
  /// \brief Sets the scaling of the entity in x,y,z directions.
  /// \param x The scaling in x direction.
  /// \param y The scaling in y direction.
  /// \param z The scaling in z direction.
  /// \return true if scaling was successful, otherwise false.
  void SetScaling(number x, number y, number z);
  
  /// \brief Sets the uniform scaling of the entity.
  /// \param scaling Uniform scaling factor.
  /// \return true if scaling was successful, otherwise false.
  void SetScaling(number scaling);

  /// \brief Returns the scaling of the entity.
  /// \return The scaling of the entity.
  hkvVec3 GetScaling();
  
  /// \brief Sets the sorting key for the entity. A lower number causes the entity to be rendered first.
  /// \param sortKey The new sorting key (0 to 255).
  void SetPrimarySortingKey(number sortKey);

  /// \brief Returns the sorting key.
  /// \return The current sorting key of the entity.
  number GetPrimarySortingKey();

  /// \brief Enables or disables the virtual ThinkFunction called for the entity.
  /// Additionally it will call SetThinkFunctionStatus on \b ALL \b Script \b Components of the entity!
  /// \param status true for enabling the ThinkFunction, false for disabling it.
  void SetThinkFunctionStatus(boolean status);
  
  /// \brief Checks if the virtual ThinkFunction is enabled or disabled.
  /// \return true = virtual ThinkFunction is enabled: false virtual ThinkFunction is disabled.
  boolean GetThinkFunctionStatus();
  
  /// @}
  /// @name Model Mesh
  /// @{
  
  /// \brief Sets the mesh of the entity.
  /// \param file The mesh file to load.
  void SetMesh(string file);
  
  /// \brief Sets the mesh of the entity.
  /// \param mesh The mesh going to be used for this entity.
  void SetMesh(VDynamicMesh mesh);

  /// \brief Gets the current mesh of the entity.
  /// \return The mesh of the entity.
  VDynamicMesh GetMesh();

  /// \brief Checks whether the entity has a model mesh.
  /// \return true if it has a mesh; otherwise false.
  boolean HasMesh();
  
  /// @}
  /// @name Visiblity / Lighting / ShadowCasting
  /// @{
  
  /// \brief Makes the entity visible or invisible in all contexts.
  /// \param visible The new visible status.
  /// \note This is a convenience function for SetVisibleBitmask.
  /// \see SetVisibleBitmask
  void SetVisible(boolean visible);

  /// \brief Checks whether the entity is visible in all contexts.
  /// \return true if visible; other false.
  /// \note This is a convenience function for GetVisibleBitmask.
  /// \see GetVisibleBitmask
  boolean IsVisible();

  /// \brief Makes the entity visible in certain rendering contexts only.
  /// \details
  ///    This entity will only be rendered in render contexts, if the logical AND result of this bitmask and the context's bitmask is not zero.
  /// \param mask The new visible bitmask.
  /// \see VBitmask
  void SetVisibleBitmask(VBitmask mask);

  /// \brief Gets the visible bitmask of the entity.
  /// \return The visible bitmask as VBitmask.
  /// \see VBitmask
  VBitmask GetVisibleBitmask();

  /// \brief Sets a bitmask that defines the light influence behaviour.
  /// \details
  ///    The entity will only be influenced by light sources that have a matching bit in their bitmask.
  /// \param mask The light influence bitmask for this entity.
  /// \see VBitmask
  void SetLightInfluenceBitmask(VBitmask mask);

  /// \brief Returns the bitmask that defines the light influence behaviour. 
  /// \details
  ///    The entity will only be influenced by light sources that have a matching bit in their bitmask.
  /// \return The light influence bitmask for this entity.
  /// \see VBitmask
  VBitmask GetLightInfluenceBitmask();
  
  /// \brief Sets whether this entity casts dynamic shadows.
  /// \param status true = yes; false = no.
  void SetCastShadows(boolean status);
  
  /// \brief Returns whether the entity casts dynamic shadows.
  /// \return true = yes; false = no.
  boolean GetCastShadows();
  
  /// \brief Sets whether this entity casts static shadows.
  /// \param status true = yes; false = no.
  void SetCastStaticShadows(boolean status);

  /// \brief Returns whether the entity casts static shadows.
  /// \return true = yes; false = no.
  boolean GetCastStaticShadows();
  
  /// \brief Draw entity always in foreground.
  /// \param status (\b optional) Use true to display entity always in foreground, otherwise false.
  void SetAlwaysInForeGround(boolean status = true);
  
  /// \brief Returns whether an entity is flagged as always in the foreground via SetAlwaysInForeGround(true). 
  /// \return true if the entity is always in the foreground, otherwise false.
  boolean IsObjectAlwaysInForegroundEnabled();
  
  /// \brief Gets the visibility status of this entity in the last rendered frame of the main render context. 
  /// \return If the object has been visible in the last frame rendered in the main render context, the function returns true, otherwise false.
  boolean WasVisibleInLastFrame();

  /// \brief Gets the visibility status of this entity in the last rendered frame of all active render contexts. 
  /// \return If the object has been visible in the last frame rendered in any of the contexts, the function returns true, otherwise false.
  boolean WasVisibleInAnyLastFrame();
  
  /// @}
  /// @name Bounding Boxes / Tracing
  /// @{

  /// \brief Set the accuracy of tracing entities with tracelines.
  /// \param accuracy The new trace accuracy, possible values are:
  ///   - Vision.TRACE_IGNORE
  ///   - Vision.TRACE_AABOX
  ///   - Vision.TRACE_OBOX
  ///   - Vision.TRACE_SPHERE
  ///   - Vision.TRACE_POLYGON
  void SetTraceAccuracy(number accuracy);

  /// \brief Gets the visibility bounding box of the entity.
  /// \return The visibility bounding box of the entity.
  hkvAlignedBBox GetBoundingBox();
  
  /// \brief Gets the collision bounding box of the entity.
  /// \return The collision bounding box of the entity.
  hkvAlignedBBox GetCollisionBoundingBox();

  /// \brief Sets a custom bounding box for tracing.
  /// \param bbox The new custom trace bounding box, use nil to reset the custom trace bounding box.
  void SetCustomTraceBBox(hkvAlignedBBox bbox);
  
  /// \brief Gets the custom bounding box for tracing.
  /// \return The custom bounding box for tracing.
  hkvAlignedBBox GetCustomTraceBBox();

  /// \brief Sets the far clip distance of the entity.
  /// \param dist The new far clip distance.
  void SetFarClipDistance(number dist);

  /// \brief Returns the far clip distance of the entity.
  /// \return The current far clip distance of the entity.
  number GetFarClipDistance();
  
  /// @}
  /// @name Shortcut for Texture Assignment
  /// @{
  
  /// \brief assign a texture to a specific surface
  /// \param texture Specifiy a VTextureObject or a string specifiying the path of the texture file
  /// \param surface The surface where the texture will be applied. You can specify the surface name as string, the surface index as number or a VisSurface_cl object.
  /// \param type (\b optional) The texture type to be set (TEXTURE_TYPE_DIFFUSE, TEXTURE_TYPE_NORMAL_MAP, TEXTURE_TYPE_SPEC_MAP), default is TEXTURE_TYPE_DIFFUSE.
  /// \return true if the asignment was successful, otherwise false (invalid texture path, invalid surface, ...)
  /// \see VTextureObject
  /// \see VisSurface_cl
  bool SetTextureForSurface(mixed texture, mixed surface, number type = TEXTURE_TYPE_DIFFUSE);
    
  /// @}
  /// @name Shader Assignment
  /// @{

  /// \brief Apply an effect to all surfaces of the entity
  /// \details This function will override the current surfaces of the entity by creating a custom texture set.
  /// \param shaderLib The shader lib, where the effect is defined (the file).
  /// \param effect The name of the effect to be used.
  /// \param params (\b optional) The parameters for the used shader. Either a string or a key/value table.
  /// \return true if the assignment was successful, otherwise false (invalid surface, effect not found).
  /// \par Example
  ///   \code
  ///     ...
  ///     local success_string = self:SetEffectForAllSurfaces("Scripting/LuaDemo", "Color", "Color=0,1,0,0")
  ///     local success_table = self:SetEffectForAllSurfaces("Scripting/LuaDemo", "Color", {Color = "0,1,0,0"})
  ///     ...
  ///   \endcode
  /// \see vForge Sampe "Scripting/LuaShader.scene".
  /// \see VisBaseEntity_cl::SetEffect, VisBaseEntity_cl::GetEffectParams, VisBaseEntity_cl::RemoveCustomTextureSet
  boolean SetEffectForAllSurfaces(string shaderLib, string effect, mixed params = "");
  
  /// \brief Apply an effect to a specified surface of the entity (the default technique will be applied)
  /// \details This function will override the current surface of the entity by creating a custom texture set.
  /// \note If wou would like to apply the shader to all entites with the same mesh use VDynamicMesh::SetEffect
  /// \param surface The surface to assign the effect specified by one of the following types:
  ///   - \b VisSurface_cl instance (of the mesh)
  ///   - \b number indicating the index of the surface
  ///   - \b string containing the name of the surface.
  /// \param shaderLib The shader lib, where the effect is defined (the file).
  /// \param effect The name of the effect to be used.
  /// \param params (\b optional) The parameters for the used shader. Either a string or a key/value table.
  /// \return true if the assignment was successful, otherwise false (invalid surface, effect not found).
  /// \par Example
  ///   \code
  ///     ...
  ///     local success_string = self:SetEffect("SurfaceTexture", "Scripting/LuaDemo", "Color", "Color=0,1,0,0")
  ///     local success_table = self:SetEffect(surfaceInstance, "Scripting/LuaDemo", "Color", {Color = "0,1,0,0"})
  ///     ...
  ///   \endcode
  /// \see vForge Sampe "Scripting/LuaShader.scene".
  /// \see VisBaseEntity_cl::SetEffect, VisBaseEntity_cl::GetEffectParams, VisBaseEntity_cl::RemoveCustomTextureSet
  boolean SetEffect(mixed surface, string shaderLib, string effect, mixed params = "");

  /// \brief Returns the param string of a given surface.
  /// \param surface The surface to add the effect specified by one of the following types:
  ///   - \b VisSurface_cl instance (of the mesh)
  ///   - \b number indicating the index of the surface
  ///   - \b string containing the name of the surface.
  /// \return Key/value dictionary of all parameters of the surface's effect.
  /// \par Example
  ///   \code
  ///     ...
  ///     local effectParams = self:GetEffectParams(0)
  ///     effectParams["Color"] = "1,1,1,1"
  ///     self:SetEffect(0, "Shaders/BaseShaders", "BS_TintColor", effectParams)
  ///     ...
  ///   \endcode
  /// \see VisBaseEntity_cl::SetEffect
  table GetEffectParams(mixed surface);

  /// \brief Remove any custom texture sets of the entity.
  /// \details After this function call, the original materials of the mesh will be used.
  boolean RemoveCustomTextureSet();

  /// \brief Clear all shader assignments of this entity.
  /// \details After this function call, the shader assignment of the mesh will be used.
  void ClearShaderSet();
  
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
