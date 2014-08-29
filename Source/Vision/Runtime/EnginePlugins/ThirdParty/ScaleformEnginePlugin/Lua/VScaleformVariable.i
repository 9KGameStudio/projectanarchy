/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VLUA_APIDOC

%nodefaultctor VScaleformVariable;

class VScaleformVariable
{
public:
  %extend
  {
    const char* GetName() const
    {
      return self->GetName().AsChar();
    }
  }

  const VScaleformValue GetValue() const;
  void SetValue(const VScaleformValue& value);

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

VSWIG_CREATE_CONCAT(VScaleformVariable, 256, "%s = %s", 
  self->GetName().IsEmpty() ? "<noname>" : self->GetName().AsChar(), 
  self->ToString().AsChar())
VSWIG_CREATE_TOSTRING(VScaleformVariable, "VScaleformVariable: '%s = %s'", 
  self->GetName().IsEmpty() ? "<noname>" : self->GetName().AsChar(), 
  self->ToString().AsChar())

// Implement Invoke() as native as it takes a variable number of arguments.
%native(VScaleformVariable_Invoke) int VScaleformVariable_Invoke(lua_State *L);
%{
  SWIGINTERN int VScaleformVariable_Invoke(lua_State *L)
  {
    IS_MEMBER_OF(VScaleformVariable) // This will move this function to the method table of the specified class.

    return VScaleform_Invoke(L, VScaleformVariable);
  }
%}

#else

/// \brief Wrapper class for VScaleformVariable.
class VScaleformVariable
{
public:

  /// \brief
  ///   Returns the name of the variable.
  string GetName();

  /// @name Value Access
  /// @{

  /// \brief
  ///   Returns the VScaleformValue object of the variable.
  VScaleformValue GetValue();

  /// \brief
  ///   Returns the VScaleformValue object of the variable.
  void SetValue(VScaleformValue value);
  
  /// \copydoc VScaleformValue::GetBool()
  boolean GetBool();

  /// \copydoc VScaleformValue::SetBool()
  void SetBool(boolean value);

  /// \copydoc VScaleformValue::GetString()
  string GetString();

  /// \copydoc VScaleformValue::SetString()
  void SetString(string value);

  /// \copydoc VScaleformValue::GetNumber()
  number GetNumber();

  /// \copydoc VScaleformValue::SetNumber()
  void SetNumber(number value);

  /// @}

  /// @name Type Check
  /// @{
  
  /// \copydoc VScaleformValue::IsUndefined()
  boolean IsUndefined();
  
  /// \copydoc VScaleformValue::IsNull()
  boolean IsNull();
  
  /// \copydoc VScaleformValue::IsBool()
  boolean IsBool();
  
  /// \copydoc VScaleformValue::IsNumeric()
  boolean IsNumeric();
  
  /// \copydoc VScaleformValue::IsString()
  boolean IsString();

  /// \copydoc VScaleformValue::IsArray()
  boolean IsArray();

  /// \copydoc VScaleformValue::IsObject()
  boolean IsObject();

  /// \copydoc VScaleformValue::IsDisplayObject()
  boolean IsDisplayObject();

  /// @}

  /// @name Access To The Internal Display Object
  /// @{
  
  /// \copydoc VScaleformValue::Display_GetX()
  number Display_GetX();
  /// \copydoc VScaleformValue::Display_GetY()
  number Display_GetY();
  /// \copydoc VScaleformValue::Display_GetRotation()
  number Display_GetRotation();
  /// \copydoc VScaleformValue::Display_GetXScale()
  number Display_GetXScale();
  /// \copydoc VScaleformValue::Display_GetYScale()
  number Display_GetYScale();
  /// \copydoc VScaleformValue::Display_GetAlpha()
  number Display_GetAlpha();
  /// \copydoc VScaleformValue::Display_GetVisible()
  boolean  Display_GetVisible();
  /// \copydoc VScaleformValue::Display_GetZ()
  number Display_GetZ();
  /// \copydoc VScaleformValue::Display_GetXRotation()
  number Display_GetXRotation();
  /// \copydoc VScaleformValue::Display_GetYRotation()
  number Display_GetYRotation();
  /// \copydoc VScaleformValue::Display_GetZScale()
  number Display_GetZScale();
  /// \copydoc VScaleformValue::Display_GetFOV()
  number Display_GetFOV();

  /// \copydoc VScaleformValue::Display_SetX()
  void Display_SetX(number x);
  /// \copydoc VScaleformValue::Display_SetY()
  void Display_SetY(number y);
  /// \copydoc VScaleformValue::Display_SetXY()
  void Display_SetXY(number x, number y);
  /// \copydoc VScaleformValue::Display_SetRotation()
  void Display_SetRotation(number degrees);
  /// \copydoc VScaleformValue::Display_SetXScale()
  void Display_SetXScale(number scaleX);
  /// \copydoc VScaleformValue::Display_SetYScale()
  void Display_SetYScale(number scaleY);
  /// \copydoc VScaleformValue::Display_SetXYScale()
  void Display_SetXYScale(number scaleX, number scaleY);
  /// \copydoc VScaleformValue::Display_SetAlpha()
  void Display_SetAlpha(number alpha);
  /// \copydoc VScaleformValue::Display_SetVisible()
  void Display_SetVisible(boolean visible);
  /// \copydoc VScaleformValue::Display_SetZ()
  void Display_SetZ(number z);
  /// \copydoc VScaleformValue::Display_SetXRotation()
  void Display_SetXRotation(number degX);
  /// \copydoc VScaleformValue::Display_SetYRotation()
  void Display_SetYRotation(number degY);
  /// \copydoc VScaleformValue::Display_SetXYRotation()
  void Display_SetXYRotation(number degX, number degY);
  /// \copydoc VScaleformValue::Display_SetZScale()
  void Display_SetZScale(number scaleZ);
  /// \copydoc VScaleformValue::Display_SetFOV()
  void Display_SetFOV(number fov);

  /// \copydoc VScaleformValue::Display_GotoFrame()
  boolean Display_GotoFrame(number frameIndex, boolean play = false);
  /// \copydoc VScaleformValue::Display_GotoLabeledFrame()
  boolean Display_GotoLabeledFrame(string label, boolean play = false);

  /// @}

  /// @name Arrays
  /// @{

  /// \copydoc VScaleformValue::GetArraySize
  number GetArraySize();

  /// \copydoc VScaleformValue::SetArraySize
  void SetArraySize(number size);

  /// \copydoc VScaleformValue::GetArrayElement
  VScaleformValue GetArrayElement(number index);

  /// \copydoc VScaleformValue::SetArrayElement
  void SetArrayElement(number index, VScaleformValue value);

  /// @}

  /// @name Object interface
  /// @{

  /// \copydoc VScaleformValue::IsInstanceOf()
  boolean IsInstanceOf(string className);

  /// \copydoc VScaleformValue::HasMember()
  boolean HasMember(string memberName);

  /// \copydoc VScaleformValue::GetMemberVariable()
  VScaleformVariable GetMemberVariable(string memberName);

  /// \copydoc VScaleformValue::GetMemberValue()
  VScaleformValue GetMemberValue(string memberName);

  /// \copydoc VScaleformValue::SetMember()
  boolean SetMember(string memberName, VScaleformValue value);

  /// \copydoc VScaleformValue::DeleteMember()
  boolean DeleteMember(string memberName);

  /// \copydoc VScaleformValue::Invoke
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
