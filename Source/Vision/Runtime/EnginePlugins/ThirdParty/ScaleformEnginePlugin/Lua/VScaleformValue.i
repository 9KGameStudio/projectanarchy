/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VLUA_APIDOC

class VScaleformValue
{
public:
  // Constructors
  VScaleformValue();
  VScaleformValue(bool bValue);
  VScaleformValue(float fValue);
  VScaleformValue(const char* szString);

  %extend
  {
    void Set(const VScaleformValue &value)
    {
      *self = value;
    }
  }

  void SetUndefined();
  void SetNull();

  bool GetBool() const;
  void SetBool(bool bValue);

  const char* GetString() const;
  void SetString(const char* szString);

  float GetNumber() const;
  void SetNumber(float fValue);

  bool IsUndefined() const;
  bool IsNull() const;
  bool IsBool() const;
  bool IsNumeric() const;
  bool IsString() const;
  bool IsArray() const;
  bool IsObject() const;
  bool IsDisplayObject() const;

  float Display_GetX() const;
  float Display_GetY() const;
  float Display_GetRotation() const;
  float Display_GetXScale() const;
  float Display_GetYScale() const;
  float Display_GetAlpha() const;
  bool  Display_GetVisible() const;
  float Display_GetZ() const;
  float Display_GetXRotation() const;
  float Display_GetYRotation() const;
  float Display_GetZScale() const;
  float Display_GetFOV() const;

  void Display_SetX(float fX);
  void Display_SetY(float fY);
  void Display_SetXY(float fX, float fY);
  void Display_SetRotation(float fDeg);
  void Display_SetXScale(float fXScale);
  void Display_SetYScale(float fYScale);
  void Display_SetXYScale(float fXScale, float fYScale);
  void Display_SetAlpha(float fAlpha);
  void Display_SetVisible(bool fVisible);
  void Display_SetZ(float fZ);
  void Display_SetXRotation(float fDeg);
  void Display_SetYRotation(float fDeg);
  void Display_SetXYRotation(float fXDeg, float fYDeg);
  void Display_SetZScale(float fZScale);
  void Display_SetFOV(float fFov);

  bool Display_GotoFrame(unsigned int uiFrameIndex, bool bPlay = false);
  bool Display_GotoLabeledFrame(const char* szLabel, bool bPlay = false);

  unsigned int GetArraySize() const;
  void SetArraySize(unsigned int uiSize);
  const VScaleformValue GetArrayElement(unsigned int uiIndex) const;
  void SetArrayElement(unsigned int uiIndex, const VScaleformValue& value);

  bool IsInstanceOf(const char* szClassName) const;

  bool HasMember(const char* szMemberName) const;
  const VScaleformVariable GetMemberVariable(const char* szMemberName);
  const VScaleformValue GetMemberValue(const char* szMemberName);
  bool SetMember(const char* szMemberName, const VScaleformValue& value);
  bool DeleteMember(const char* szMemberName);
};

VSWIG_CREATE_CONCAT(VScaleformValue, 256, "%s", self->ToString().AsChar())
VSWIG_CREATE_TOSTRING(VScaleformValue, "VScaleformValue: '%s'", self->ToString().AsChar())

// Implement Invoke() as native as it takes a variable number of arguments.
%native(VScaleformValue_Invoke) int VScaleformValue_Invoke(lua_State *L);
%{
  SWIGINTERN int VScaleformValue_Invoke(lua_State *L)
  {
    IS_MEMBER_OF(VScaleformValue) // This will move this function to the method table of the specified class.

    return VScaleform_Invoke(L, VScaleformValue);
  }
%}

#else

/// \brief Wrapper class for VScaleformValue.
class VScaleformValue
{
public:

  /// @name Value Access
  /// @{

  /// \brief
  ///   Sets a value of type nil, boolean, number, string or VScaleformValue.
  void Set(mixed value);
  
  /// \brief Get boolean value.
  /// \see IsBool
  boolean GetBool();

  /// \brief Set boolean value.
  /// \param value The value to set.
  /// \sa IsBool
  void SetBool(boolean value);
  
  /// \brief Get string value.
  /// \sa IsString
  string GetString();

  /// \brief Set string value.
  /// \param value The value to set.
  /// \sa IsString
  void SetString(string value);
  
  /// \brief Get numeric value.
  /// \sa IsNumeric
  number GetNumber();

  /// \brief Set numeric value.
  /// \param value The value to set.
  /// \sa IsNumeric
  void SetNumber(number value);

  /// @}

  /// @name Type Check
  /// @{
  
  /// \brief Returns whether the value is undefined.
  boolean IsUndefined();
  
  /// \brief Returns whether the internal type is NULL/nil.
  boolean IsNull();
  
  /// \brief Returns whether the internal type is boolean.
  boolean IsBool();
  
  /// \brief Returns whether the internal type has a numeric representation (int, uint or float).
  boolean IsNumeric();
  
  /// \brief Returns whether the internal type is a String (not StringW).
  boolean IsString();

  /// \brief Returns whether the internal type is an Array.
  boolean IsArray();

  /// \brief Returns whether the internal type points to an Object.
  boolean IsObject();

  /// \brief Returns whether the internal type points to a Display Object.
  boolean IsDisplayObject();

  /// @}

  /// @name Access To The Internal Display Object
  /// @{
  
  /// \brief Returns the x position of the display object.
  number Display_GetX();
  /// \brief Returns the y position of the display object.
  number Display_GetY();
  /// \brief Returns the rotation on the XY plane of the display object.
  number Display_GetRotation();
  /// \brief Returns the x scaling factor of the display object.
  number Display_GetXScale();
  /// \brief Returns the y scaling factor of the display object.
  number Display_GetYScale();
  /// \brief Get alpha value of the display object.
  number Display_GetAlpha();
  /// \brief Returns whether the display object is visible.
  boolean  Display_GetVisible();
  /// \brief Returns the z depth value of the display object.
  number Display_GetZ();
  /// \brief Returns the rotation around the x axis of the display object.
  number Display_GetXRotation();
  /// \brief Returns the rotation around the y axis of the display object.
  number Display_GetYRotation();
  /// \brief Returns the z scaling value of the display object.
  number Display_GetZScale();
  /// \brief Returns the current field of view of the display object.
  number Display_GetFOV();

  /// \brief Sets the x position of the display object.
  /// \param x The x position.
  void Display_SetX(number x);

  /// \brief Sets the y position of the display object.
  /// \param y The y position.
  void Display_SetY(number y);

  /// \brief Sets x and y position of the display object.
  /// \param x The x position.
  /// \param y The y position.
  void Display_SetXY(number x, number y);

  /// \brief Sets the rotation on the XY plane of the display object.
  /// \param degrees The rotation angle.
  void Display_SetRotation(number degrees);

  /// \brief Sets the x scaling factor of the display object.
  /// \param scaleX The scaling factor.
  void Display_SetXScale(number scaleX);

  /// \brief Sets the y scaling factor of the display object.
  /// \param scaleY The scaling factor.
  void Display_SetYScale(number scaleY);

  /// \brief Sets x and y scaling factor of the display object.
  /// \param scaleX The x scaling factor.
  /// \param scaleY The y scaling factor.
  void Display_SetXYScale(number scaleX, number scaleY);

  /// \brief Sets alpha value of the display object.
  /// \param alpha The alpha value.
  void Display_SetAlpha(number alpha);

  /// \brief Sets the visibility state of the display object.
  /// \param visible The visibility state.
  void Display_SetVisible(boolean visible);

  /// \brief Sets the z depth value of the display object.
  /// \param z The depth value.
  void Display_SetZ(number z);

  /// \brief Sets the rotation around the x axis of the display object.
  /// \param degX The rotation angle.
  void Display_SetXRotation(number degX);

  /// \brief Sets the rotation around the y axis of the display object.
  /// \param degY The rotation angle.
  void Display_SetYRotation(number degY);

  /// \brief Sets the rotation around the x and y axes of the display object.
  /// \param degX The rotation angle around the x axis.
  /// \param degY The rotation angle around the y axis.
  void Display_SetXYRotation(number degX, number degY);

  /// \brief Sets the z scaling value of the display object.
  /// \param scaleZ The scaling factor.
  void Display_SetZScale(number scaleZ);

  /// \brief Sets the current field of view of the display object.
  /// \param fov The field of view angle.
  void Display_SetFOV(number fov);

  ///
  /// \brief
  ///   Moves the display object to the specified frame.
  ///
  /// \param frameIndex
  ///   The 1-based frame index.
  /// \param play
  ///   Specifies if the playback should start after moving to the frame independent of the previous play state.
  ///
  /// \return
  ///   true if the display object was moved to the frame.
  ///
  boolean Display_GotoFrame(number frameIndex, boolean play = false);

  ///
  /// \brief
  ///   Moves the display object to the frame with the specified label.
  ///
  /// \param label
  ///   The label of the frame.
  /// \param play
  ///   Specifies if the playback should start after moving to the frame independent of the previous play state.
  ///
  /// \return
  ///   true if the display object was moved to the frame.
  ///
  boolean Display_GotoLabeledFrame(string label, boolean play = false);

  /// @}

  /// @name Arrays
  /// @{

  /// \brief
  ///   Returns the size of the array.
  number GetArraySize();

  /// \brief
  ///   Resizes the array.
  /// \param size
  ///   The new size of the array,
  void SetArraySize(number size);

  /// \brief
  ///   Returns the value of an element in the array.
  /// \param index
  ///   The index of the element.
  VScaleformValue GetArrayElement(number index);

  /// \brief
  ///   Sets the value of an element in the array.
  /// \param index
  ///   The index of the element. Automatically resizes the array if it is too small.
  /// \param value
  ///   The value to set.
  void SetArrayElement(number index, VScaleformValue value);

  /// @}

  /// @name Object interface
  /// @{

  /// \brief 
  ///   Returns whether the object value is an instance of the given class.
  /// \param className
  ///   The name of the class.
  boolean IsInstanceOf(string className);

  /// \brief
  ///   Returns whether an object has a member with the specified name.
  /// \param memberName
  ///   The name of the member variable.
  boolean HasMember(string memberName);

  /// \brief 
  ///   Returns the member variable with the specified name.
  ///
  /// As opposed to GetMemberValue, this function returns a direct handle to the variable.
  /// This means that the value of the handle will always be in sync with the ActionScript state.
  ///
  /// \param memberName  
  ///   The name of the member variable.
  /// \return 
  ///   Undefined variable if not present, else the wrapped variable.
  ///
  /// \sa GetMemberValue
  VScaleformVariable GetMemberVariable(string memberName);

  /// \brief
  ///   Returns the value of a member variable with the specified name.
  ///
  /// As opposed to GetMemberVariable, this method only returns the value of the variable.
  /// If the value of the variable is only needed once, this method should be preferred.
  ///
  /// \param memberName  
  ///   The name of the member variable.
  /// \return 
  ///   Undefined value if not present, else the value.
  ///
  /// \sa GetMemberVariable
  VScaleformValue GetMemberValue(string memberName);

  /// \brief
  ///   Creates or modifies the member variable with the specified name and the value.
  ///
  /// \param memberName  
  ///   The name of the member variable.
  /// \param value
  ///   The value to be set.
  /// \return
  ///   true if the member has been set.
  ///
  /// \note
  ///   Additional members can only be set on dynamic ActionScript classes.
  /// \sa
  ///   DeleteMember
  boolean SetMember(string memberName, VScaleformValue value);

  /// \brief
  ///   Deletes the dynamic member variable with the specified name.
  ///
  /// \param memberName  
  ///   The name of the member variable.
  /// \return
  ///   true if the member existed and has been deleted.
  ///
  /// \note
  ///   The member variable must have previously been added by SetMember().
  /// \sa
  ///   SetMember
  boolean DeleteMember(string memberName);

  /// \brief
  ///   Invokes the method on the object and returns the return value.
  ///
  /// \param methodName
  ///   The name of the method.
  /// \param ...
  ///   Optional arguments for the method call.
  VScaleformValue Invoke(string methodName, ...);

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
