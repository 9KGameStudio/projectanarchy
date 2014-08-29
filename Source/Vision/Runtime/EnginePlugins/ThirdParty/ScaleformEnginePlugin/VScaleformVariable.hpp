/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VSCALEFORMVARIABLE_HPP_INCLUDED
#define VSCALEFORMVARIABLE_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/VScaleformValue.hpp>

struct VScaleformVariableInfo;
class VScaleformVariableManager;

/// \brief  Handle to an ActionScript variable which contains a VScaleformValue.
/// \note   Instances can be created via VScaleformMovieInstance::GetVariable() and VScaleformValue::GetMemberVariable().
/// \sa     VScaleformMovieInstance::GetVariable() VScaleformValue::GetMemberVariable()
class VScaleformVariable
{
  friend class VScaleformVariableManager;
  friend class VScaleformValueAccessor;

private:
  /// \brief Internal Constructor - Create a wrapper of an ActionScript variable.
  VScaleformVariable(VScaleformVariableInfo* pInfo);

public:
  // Constructor / Destructor / Assignment
  SCALEFORM_IMPEXP VScaleformVariable();
  SCALEFORM_IMPEXP VScaleformVariable(const VScaleformVariable& op);
  SCALEFORM_IMPEXP VScaleformVariable& operator=(const VScaleformVariable&op);
  SCALEFORM_IMPEXP virtual ~VScaleformVariable();

  /// \brief
  ///   Returns the name of the variable.
  SCALEFORM_IMPEXP const VString GetName() const;

  /// \brief
  ///   Returns the value object of the variable.
  SCALEFORM_IMPEXP const VScaleformValue& GetValue() const;

  /// \brief
  ///   Returns the value object of the variable.
  SCALEFORM_IMPEXP void SetValue(const VScaleformValue& value);

  /// \brief
  ///   Returns the associated movie instance.
  SCALEFORM_IMPEXP const VScaleformMovieInstance* GetMovieInstance() const;

  inline const VString ToString() const { return GetValue().ToString(); }

  // Types
  /// \copydoc VScaleformValue::IsUndefined
  inline bool IsUndefined() const { return GetValue().IsUndefined(); }
  inline bool IsNull() const { return GetValue().IsNull(); }
  inline bool IsBool() const { return GetValue().IsBool(); }
  inline bool IsInt() const { return GetValue().IsInt(); }
  inline bool IsUInt() const { return GetValue().IsUInt(); }
  inline bool IsNumber() const { return GetValue().IsNumber(); }
  inline bool IsNumeric() const { return GetValue().IsNumeric(); }
  inline bool IsString() const { return GetValue().IsString(); }
  inline bool IsArray() const { return GetValue().IsArray(); }
  inline bool IsObject() const { return GetValue().IsObject(); }
  inline bool IsDisplayObject() const { return GetValue().IsDisplayObject(); }

  // Primitive Type Getters
  inline bool         GetBool() const { return GetValue().GetBool(); }
  inline int          GetInt() const { return GetValue().GetInt(); }
  inline unsigned int GetUInt() const  { return GetValue().GetUInt(); }
  inline const char*  GetString() const { return GetValue().GetString(); }
  inline float        GetNumber() const { return GetValue().GetNumber(); }

  // Display Info Getters
  inline bool  Display_GetVisible() const { return GetValue().Display_GetVisible(); }
  inline float Display_GetX() const { return GetValue().Display_GetX(); }
  inline float Display_GetY() const { return GetValue().Display_GetY(); }
  inline float Display_GetZ() const { return GetValue().Display_GetZ(); }
  inline float Display_GetXRotation() const { return GetValue().Display_GetXRotation(); }
  inline float Display_GetYRotation() const { return GetValue().Display_GetYRotation(); }
  inline float Display_GetRotation() const { return GetValue().Display_GetRotation(); }
  inline float Display_GetXScale() const { return GetValue().Display_GetXScale(); }
  inline float Display_GetYScale() const { return GetValue().Display_GetYScale(); }
  inline float Display_GetZScale() const { return GetValue().Display_GetZScale(); }
  inline float Display_GetAlpha() const { return GetValue().Display_GetAlpha(); }
  inline float Display_GetFOV() const { return GetValue().Display_GetFOV(); }

  // Primitive Type Setters
  /// \copydoc VScaleformValue::SetUndefined
  inline void SetUndefined() 
  { 
    WaitForAdvanceFinished(); 
    InternalGetValue().SetUndefined(); 
    UpdateActionScript(); 
  }
  inline void SetNull() 
  { 
    WaitForAdvanceFinished(); 
    InternalGetValue().SetNull(); 
    UpdateActionScript(); 
  }
  inline void SetBool(bool bValue) 
  { 
    WaitForAdvanceFinished(); 
    InternalGetValue().SetBool(bValue); 
    UpdateActionScript(); 
  }
  inline void SetInt(int iValue) 
  { 
    WaitForAdvanceFinished(); 
    InternalGetValue().SetInt(iValue); 
    UpdateActionScript(); 
  }
  inline void SetUInt(unsigned int uiValue) 
  { 
    WaitForAdvanceFinished(); 
    InternalGetValue().SetUInt(uiValue); 
    UpdateActionScript(); 
  }  
  inline void SetNumber(float fValue) 
  { 
    WaitForAdvanceFinished(); 
    InternalGetValue().SetNumber(fValue); 
    UpdateActionScript();
  }
  inline void SetString(const char* szString) 
  { 
    WaitForAdvanceFinished(); 
    InternalGetValue().SetString(szString); 
    UpdateActionScript(); 
  }

  // Display Info Setters (Object references are set directly).
  inline void Display_SetVisible(bool bVisible) { InternalGetValue().Display_SetVisible(bVisible); }
  inline void Display_SetX(float fX) { InternalGetValue().Display_SetX(fX); }
  inline void Display_SetY(float fY) { InternalGetValue().Display_SetY(fY); }
  inline void Display_SetXY(float fX, float fY) { InternalGetValue().Display_SetXY(fX, fY); }
  inline void Display_SetZ(float fZ) { InternalGetValue().Display_SetZ(fZ); }
  inline void Display_SetXRotation(float fDeg) { InternalGetValue().Display_SetXRotation(fDeg); }
  inline void Display_SetYRotation(float fDeg) { InternalGetValue().Display_SetYRotation(fDeg); }
  inline void Display_SetXYRotation(float fXDeg, float fYDeg) { InternalGetValue().Display_SetXYRotation(fXDeg, fYDeg); }
  inline void Display_SetRotation(float fDeg) { InternalGetValue().Display_SetRotation(fDeg); }
  inline void Display_SetXScale(float fXScale) { InternalGetValue().Display_SetXScale(fXScale); }
  inline void Display_SetYScale(float fYScale) { InternalGetValue().Display_SetYScale(fYScale); }
  inline void Display_SetXYScale(float fXScale, float fYScale) { InternalGetValue().Display_SetXYScale(fXScale, fYScale); }
  inline void Display_SetZScale(float fZScale) { InternalGetValue().Display_SetZScale(fZScale); }
  inline void Display_SetAlpha(float fAlpha) { InternalGetValue().Display_SetAlpha(fAlpha); }
  inline void Display_SetFOV(float fFov) { InternalGetValue().Display_SetFOV(fFov); }

  /// \copydoc VScaleformValue::Display_GotoFrame
  inline bool Display_GotoFrame(unsigned int uiFrameIndex, bool bPlay = false) { return InternalGetValue().Display_GotoFrame(uiFrameIndex, bPlay); }
  
  /// \copydoc VScaleformValue::Display_GotoLabeledFrame
  inline bool Display_GotoLabeledFrame(const char* szLabel, bool bPlay = false) { return InternalGetValue().Display_GotoLabeledFrame(szLabel, bPlay); }

  // Arrays
  inline unsigned int GetArraySize() const { return GetValue().GetArraySize(); }
  inline void SetArraySize(unsigned int uiSize) { InternalGetValue().SetArraySize(uiSize); }

  inline const VScaleformValue GetArrayElement(unsigned int uiIndex) const { return GetValue().GetArrayElement(uiIndex); }
  inline void SetArrayElement(unsigned int uiIndex, const VScaleformValue& value) { InternalGetValue().SetArrayElement(uiIndex, value); }

  /// @name Object interface
  /// @{

  /// \copydoc VScaleformValue::IsInstanceOf()
  inline bool IsInstanceOf(const char* szClassName) const { return GetValue().IsInstanceOf(szClassName); }

  /// \copydoc VScaleformValue::HasMember()
  inline bool HasMember(const char* szMemberName) const  { return GetValue().HasMember(szMemberName); }

  /// \copydoc VScaleformValue::GetMemberVariable()
  inline const VScaleformVariable GetMemberVariable(const char* szMemberName) { return InternalGetValue().GetMemberVariable(szMemberName); }

  /// \copydoc VScaleformValue::GetMemberValue()
  inline const VScaleformValue GetMemberValue(const char* szMemberName) { return InternalGetValue().GetMemberValue(szMemberName); }

  /// \copydoc VScaleformValue::SetMember()
  inline bool SetMember(const char* szMemberName, const VScaleformValue& value) { return InternalGetValue().SetMember(szMemberName, value); }

  /// \copydoc VScaleformValue::DeleteMember()
  inline bool DeleteMember(const char* szMemberName) { return InternalGetValue().DeleteMember(szMemberName); }

  /// \copydoc VScaleformValue::Invoke()
  inline const VScaleformValue Invoke(const char* szMethodName, const VScaleformValue *pArgs, unsigned int uiNumArgs)
  {
    return InternalGetValue().Invoke(szMethodName, pArgs, uiNumArgs);
  }

  inline const VScaleformValue Invoke(const char* szMethodName)
  {
    return Invoke(szMethodName, NULL, 0);
  }

  template<int N>
  inline const VScaleformValue Invoke(const char* szMethodName, const VScaleformValue (&args)[N])
  {
    return Invoke(szMethodName, args, N);
  }

  /// @}

  /// @name Comparison operators
  /// @{

  inline bool operator==(const VScaleformVariable& op) const { return (m_spInfo == op.m_spInfo); }
  inline bool operator!=(const VScaleformVariable& op) const { return (m_spInfo != op.m_spInfo); }

  /// @}

private:
  SCALEFORM_IMPEXP VScaleformValue& InternalGetValue();
  SCALEFORM_IMPEXP void WaitForAdvanceFinished();
  SCALEFORM_IMPEXP void UpdateActionScript();

  // Member variables
  VSmartPtr<VScaleformVariableInfo> m_spInfo;
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
