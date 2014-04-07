/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VLUA_APIDOC

%nodefaultctor VScaleformMovieInstance;
%nodefaultdtor VScaleformMovieInstance;

class VScaleformMovieInstance
{
public:
  void SetOpacity(float fAlpha);

  void SetDimensions(int iPosX = 0, int iPosY = 0, int iWidth = SF_MOVIE_SIZE_RENDERCONTEXT, int iHeight = SF_MOVIE_SIZE_RENDERCONTEXT);
  
  void SetPosition(int iPosX, int iPosY);

  void SetSize(int iWidth, int iHeight);

  void SetAuthoredSize();

  void SetVisibleBitmask(unsigned int iMask);

  unsigned int GetVisibleBitmask() const;

  bool IsVisibleInAnyContext() const;
  
  const char* GetFileName() const;

  void SetPaused(bool bPaused);

  bool IsPaused() const;

  void Restart();

  bool IsFocused() const;

  void SetHandleInput(bool bEnable);
  
  const VScaleformVariable GetVariable(const char* szVarName);
  const VScaleformValue GetVariableValue(const char* szVarName);
};

//add lua tostring and concat operators
VSWIG_CREATE_CONCAT(VScaleformMovieInstance, 256, "[%s]", self->GetFileName())
VSWIG_CREATE_TOSTRING(VScaleformMovieInstance, "VScaleformMovieInstance: '%s'", self->GetFileName())

// Implement Invoke() as native as it takes a variable number of arguments.
%native(VScaleformMovieInstance_Invoke) int VScaleformMovieInstance_Invoke(lua_State *L);
%{
  SWIGINTERN int VScaleformMovieInstance_Invoke(lua_State *L)
  {
    IS_MEMBER_OF(VScaleformMovieInstance) // This will move this function to the method table of the specified class.

    return VScaleform_Invoke(L, VScaleformMovieInstance);
  }
%}

//implement GetPosition native because we would like to return two values at once
%native(VScaleformMovieInstance_GetPosition) int VScaleformMovieInstance_GetPosition(lua_State *L);
%{
  SWIGINTERN int VScaleformMovieInstance_GetPosition(lua_State *L)
  {
    IS_MEMBER_OF(VScaleformMovieInstance) //this will move this function to the method table of the specified class
    
    SWIG_CONVERT_POINTER(L, 1, VScaleformMovieInstance, pSelf)
    
    int x, y;
    pSelf->GetPosition(x, y);
    
    lua_pushnumber(L, (lua_Number)x);
    lua_pushnumber(L, (lua_Number)y);
      
    return 2; //in every case we leave two values at the stack
  }
%}

//implement GetSize native because we would like to return two values at once
%native(VScaleformMovieInstance_GetSize) int VScaleformMovieInstance_GetSize(lua_State *L);
%{
  SWIGINTERN int VScaleformMovieInstance_GetSize(lua_State *L)
  {
    IS_MEMBER_OF(VScaleformMovieInstance) //this will move this function to the method table of the specified class
    
    SWIG_CONVERT_POINTER(L, 1, VScaleformMovieInstance, pSelf)
    
    int x, y;
    pSelf->GetSize(x, y);
    
    lua_pushnumber(L, (lua_Number)x);
    lua_pushnumber(L, (lua_Number)y);
      
    return 2; //in every case we leave two values at the stack
  }
%}

//implement GetAuthoredSize native because we would like to return two values at once
%native(VScaleformMovieInstance_GetAuthoredSize) int VScaleformMovieInstance_GetAuthoredSize(lua_State *L);
%{
  SWIGINTERN int VScaleformMovieInstance_GetAuthoredSize(lua_State *L)
  {
    IS_MEMBER_OF(VScaleformMovieInstance) //this will move this function to the method table of the specified class
    
    SWIG_CONVERT_POINTER(L, 1, VScaleformMovieInstance, pSelf)
    
    int x, y;
    pSelf->GetAuthoredSize(x, y);
    
    lua_pushnumber(L, (lua_Number)x);
    lua_pushnumber(L, (lua_Number)y);
      
    return 2; //in every case we leave two values at the stack
  }
%}

#else

/// \brief  Wrapper class for VScaleformMovieInstance.
/// \par Example
///   \code
///     ...
///     mainMovie = Scaleform:LoadMovie("pause_menu.swf")
///  
///     -- e.g. temporary deactivate a menu
///     mainMenu:SetVisible(false)
///     mainMenu:SetHandleInput(false)
///     ...
///
///   \endcode
class VScaleformMovieInstance
{
public:

  ///
  /// @name Render Setup
  /// @{

  /// \brief  Sets the background opacity for this instance.
  /// \param  fAlpha  The alpha value - 0.0 means the scene background shines through.
  void SetOpacity(float fAlpha);

  /// \brief Sets new dimensions for the movie.
  /// \note Call parameterless to reset to it's defaults.
  /// \param  x   (\b optional) the x position coordinate.
  /// \param  y   (\b optional) the y position coordinate.
  /// \param  width  (\b optional) the width.
  /// \param  height (\b optional) the height.
  /// \see VScaleformManager::GetMovieRenderContextSize
  void SetDimensions(number x = 0, number y = 0, number width = SF_MOVIE_SIZE_RENDERCONTEXT, number height = SF_MOVIE_SIZE_RENDERCONTEXT);
  
  /// \brief Set the position of the movie.
  /// \param  x  the x position coordinate.
  /// \param  y  the y position coordinate.
  void SetPosition(number x, number y);

  /// \brief  Set the size of the movie.
  /// \param  width  the width.
  /// \param  height the height.
  void SetSize(number width, number height);

  /// \brief Sets the movie to the authored size.
  void SetAuthoredSize();
  
  /// \brief  Gets the current size of the movie.
  /// \return Two numbers defining the current size.
  /// \par Example
  ///   \code
  ///     ...
  ///     mainMovie = Scaleform:LoadMovie("menu.swf")
  ///     local width, height = mainMenu:GetSize()
  ///     ...
  ///   \endcode
  multiple GetSize();
  
  /// \brief  Gets the current position of the movie.
  /// \return Two numbers defining the current x and y position.
  /// \par Example
  ///   \code
  ///     ...
  ///     mainMovie = Scaleform:LoadMovie("menu.swf", 30, 90)
  ///     local x, y = mainMenu:GetPosition()
  ///     Vision.Assert(x==30, "The x position should be 30!")
  ///     Vision.Assert(y==90, "The y position should be 90!")
  ///     ...
  ///   \endcode
  multiple GetPosition();
  
  /// \brief  Gets the size which was used when authoring the movie.
  /// \return Two numbers defining the authored size.
  /// \par Example
  ///   \code
  ///     ...
  ///     mainMovie = Scaleform:LoadMovie("menu.swf")
  ///     local width, height = mainMenu:GetAuthoredSize()
  ///     ...
  ///   \endcode
  multiple GetAuthoredSize();
  
  /// @}
  /// @name Movie Control
  /// @{
  
  /// \brief  Query if this movie is visible.
  /// \return true if visible, false if not.
  string GetFileName();

  /// \brief  Sets the visibility bitmask for this movie instance.
  ///
  /// It will only be rendered into render contexts, if the logical AND result of this bitmask and 
  /// the context's bitmask (see VisRenderContext_cl::SetRenderFilterMask) is not zero.
  /// To hide an object completely in all contexts, use SetVisibleBitmask(0).
  /// By default, only the first bit is set.
  ///
  /// \param iMask
  ///   the visibility bitmask to use
  void SetVisibleBitmask(unsigned int iMask);

  /// \brief  Returns the previously set visibility bitmask for this movie instance.
  unsigned int GetVisibleBitmask() const;

  /// \brief  Query if this movie is visible in any render context.
  bool IsVisibleInAnyContext() const;

  /// \brief  Pauses the playback of the movie.
  /// \param paused set to true for pause, false for play.
  void SetPaused(boolean paused);

  /// \brief  Query if this object is paused.
  /// \return true if paused, false if not.
  boolean IsPaused();

  /// \brief  Restarts the movie playback.
  void Restart();

  /// \brief  Query if this object is focused.
  /// \return true if focused, false if not.
  boolean IsFocused();

  /// \brief Enable or disable input handling for this movie instance.
  /// \param enable Set to false to stop further input handling. Default is true.
  void SetHandleInput(boolean enable);
  
  /// \brief  Returns a variable from the Scaleform movie.
  ///
  /// As opposed to GetVariableValue, this function returns a direct handle to the variable.
  /// This means that the value of the handle will always be in sync with the ActionScript state.
  ///
  /// \param  varName  The name of the variable including path in the movie (e.g. '_root/MyVar').
  /// \return  Variable with undefined value if not present, else the wrapped variable.
  ///
  /// \par Example
  ///   \code
  ///     ...
  ///     variable = movie:GetVariable("_root.mainMenu") -- Returns the handle to the variable that currently holds a reference to a display object.
  ///     variable:Display_SetXYRotation(x, y); -- Rotate the display object.
  ///     ...
  ///     variabler2 = movie:GetVariable("_root.counter) -- Counter is an integer value.
  ///     variable2.SetNumber(1.5) -- The value is in sync with the ActionScript variable.
  ///     ...
  ///   \endcode
  VScaleformValue GetVariable(string varName);

  /// \brief  Returns the value of a variable from the Scaleform movie.
  ///
  /// As opposed to GetVariable, this method only returns the value of the variable.
  /// If the value of the variable is only needed once, this method should be preferred.
  ///
  /// \param  varName  The name of the variable including path in the movie (e.g. '_root/MyVar').
  /// \return  Value of the variable. Undefined value if not present.
  ///
  /// \par Example
  ///   \code
  ///     ...
  ///     value = movie:GetVariableValue("_root.mainMenu") -- Returns the reference to a display object as the value.
  ///     value:Display_SetXYRotation(x, y); -- Rotate the display object.
  ///     ...
  ///     value2 = movie:GetVariableValue("_root.counter) -- Counter is an integer value.
  ///     value2.SetNumber(1.5) -- The value is only a copy from the ActionScript state, so this call will only modify the LUA object.
  ///     ...
  ///   \endcode
  VScaleformValue GetVariableValue(string varName);

  /// \brief
  ///   Invokes the function on the movie and returns the return value.
  ///
  /// \param functionName
  ///   The name of the function.
  /// \param ...
  ///   Optional arguments for the function call.
  VScaleformValue Invoke(string functionName, ...);
  
  /// @}
  ///

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
