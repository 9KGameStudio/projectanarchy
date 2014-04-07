/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VSCALEFORMVALUE_HPP_INCLUDED
#define VSCALEFORMVALUE_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/VScaleformImpExp.hpp>

namespace Scaleform
{
  namespace GFx
  {
    class Value;
  }
}

class VScaleformVariable;
class VScaleformMovieInstance;

/// \brief  Wrapper class for Scaleform::GFx::Value
///         (Get and Set functions work like in the Scaleform API)
/// \note   Instances can be created via VScaleformMovieInstance::GetVariableValue() and VScaleformValue::GetMemberValue().
/// \sa     VScaleformMovieInstance::GetVariableValue() VScaleformValue::GetMemberValue()
class VScaleformValue
{
public:
  /// \brief Internal Constructor - Create a wrapper for a Scaleform::GFx::Value.
  VScaleformValue(const Scaleform::GFx::Value& gfxValue, const VScaleformMovieInstance* pMovieInstance);
  /// \brief Internal assignment method.
  void SetGFxValue(const Scaleform::GFx::Value& gfxValue, const VScaleformMovieInstance* pMovieInstance);

  // Constructors
  SCALEFORM_IMPEXP VScaleformValue();
  SCALEFORM_IMPEXP VScaleformValue(bool bValue);
  SCALEFORM_IMPEXP VScaleformValue(int iValue);
  SCALEFORM_IMPEXP VScaleformValue(unsigned int uiValue);
  SCALEFORM_IMPEXP VScaleformValue(float fValue);
  SCALEFORM_IMPEXP VScaleformValue(const char* szString);

  SCALEFORM_IMPEXP VScaleformValue(const VScaleformValue& op);
  SCALEFORM_IMPEXP VScaleformValue& operator=(const VScaleformValue& op);

  SCALEFORM_IMPEXP ~VScaleformValue();

  SCALEFORM_IMPEXP const VString ToString() const;

  /// \brief
  ///   Returns the wrapped GFx value.
  inline const Scaleform::GFx::Value& GetGFxValue() const 
  { 
    return *reinterpret_cast<const Scaleform::GFx::Value*>(m_gfxValueData); 
  }

  // Types
  /// \brief Returns whether the value is undefined.
  SCALEFORM_IMPEXP bool IsUndefined() const;
  SCALEFORM_IMPEXP bool IsNull() const;
  SCALEFORM_IMPEXP bool IsBool() const;
  SCALEFORM_IMPEXP bool IsInt() const;
  SCALEFORM_IMPEXP bool IsUInt() const;
  SCALEFORM_IMPEXP bool IsNumber() const;
  SCALEFORM_IMPEXP bool IsNumeric() const;
  SCALEFORM_IMPEXP bool IsString() const;
  SCALEFORM_IMPEXP bool IsArray() const;
  SCALEFORM_IMPEXP bool IsObject() const;
  SCALEFORM_IMPEXP bool IsDisplayObject() const;

  // Primitive Type Getters
  SCALEFORM_IMPEXP bool         GetBool() const;
  SCALEFORM_IMPEXP int          GetInt() const;
  SCALEFORM_IMPEXP unsigned int GetUInt() const;  
  SCALEFORM_IMPEXP const char*  GetString() const;
  SCALEFORM_IMPEXP float        GetNumber() const;

  // Display Info Getters
  SCALEFORM_IMPEXP bool  Display_GetVisible() const;
  SCALEFORM_IMPEXP float Display_GetX() const;
  SCALEFORM_IMPEXP float Display_GetY() const;
  SCALEFORM_IMPEXP float Display_GetZ() const;
  SCALEFORM_IMPEXP float Display_GetXRotation() const;
  SCALEFORM_IMPEXP float Display_GetYRotation() const;
  SCALEFORM_IMPEXP float Display_GetRotation() const;
  SCALEFORM_IMPEXP float Display_GetXScale() const;
  SCALEFORM_IMPEXP float Display_GetYScale() const;
  SCALEFORM_IMPEXP float Display_GetZScale() const;
  SCALEFORM_IMPEXP float Display_GetAlpha() const;
  SCALEFORM_IMPEXP float Display_GetFOV() const;

  // Primitive Type Setters
  /// \brief Sets the value to undefined.
  SCALEFORM_IMPEXP void SetUndefined();
  SCALEFORM_IMPEXP void SetNull();
  SCALEFORM_IMPEXP void SetBool(bool bValue);
  SCALEFORM_IMPEXP void SetInt(int iValue);
  SCALEFORM_IMPEXP void SetUInt(unsigned int uiValue);  
  SCALEFORM_IMPEXP void SetNumber(float fValue);
  SCALEFORM_IMPEXP void SetString(const char* szString);

  // Display Info Setters
  SCALEFORM_IMPEXP void Display_SetVisible(bool bVisible);
  SCALEFORM_IMPEXP void Display_SetX(float fX);
  SCALEFORM_IMPEXP void Display_SetY(float fY);
  SCALEFORM_IMPEXP void Display_SetXY(float fX, float fY);
  SCALEFORM_IMPEXP void Display_SetZ(float fZ);
  SCALEFORM_IMPEXP void Display_SetXRotation(float fDeg);
  SCALEFORM_IMPEXP void Display_SetYRotation(float fDeg);
  SCALEFORM_IMPEXP void Display_SetXYRotation(float fXDeg, float fYDeg);
  SCALEFORM_IMPEXP void Display_SetRotation(float fDeg);
  SCALEFORM_IMPEXP void Display_SetXScale(float fXScale);
  SCALEFORM_IMPEXP void Display_SetYScale(float fYScale);
  SCALEFORM_IMPEXP void Display_SetXYScale(float fXScale, float fYScale);
  SCALEFORM_IMPEXP void Display_SetZScale(float fZScale);
  SCALEFORM_IMPEXP void Display_SetAlpha(float fAlpha);
  SCALEFORM_IMPEXP void Display_SetFOV(float fFov);

  // Arrays
  SCALEFORM_IMPEXP unsigned int GetArraySize() const;
  SCALEFORM_IMPEXP void SetArraySize(unsigned int uiSize);

  SCALEFORM_IMPEXP const VScaleformValue GetArrayElement(unsigned int uiIndex) const;
  SCALEFORM_IMPEXP void SetArrayElement(unsigned int uiIndex, const VScaleformValue& value);

  /// @name Object interface
  /// @{

  /// \brief 
  ///   Returns whether the object value is an instance of the given class.
  /// \param szClassName
  ///   The name of the class.
  SCALEFORM_IMPEXP bool IsInstanceOf(const char* szClassName) const;

  /// \brief
  ///   Returns whether an object has a member with the specified name.
  /// \param szMemberName
  ///   The name of the member.
  SCALEFORM_IMPEXP bool HasMember(const char* szMemberName) const;

  /// \brief 
  ///   Returns the member variable with the specified name.
  ///
  /// As opposed to GetMemberValue, this function returns a direct handle to the variable.
  /// This means that the value of the handle will always be in sync with the ActionScript state.
  ///
  /// \param szMemberName  
  ///   The name of the member variable.
  /// \return 
  ///   Undefined variable if not present, else the wrapped variable.
  ///
  /// \sa GetMemberValue
  SCALEFORM_IMPEXP const VScaleformVariable GetMemberVariable(const char* szMemberName);

  /// \brief
  ///   Returns the value of a member variable with the specified name.
  ///
  /// As opposed to GetMemberVariable, this method only returns the value of the variable.
  /// If the value of the variable is only needed once, this method should be preferred.
  ///
  /// \param szMemberName  
  ///   The name of the member variable.
  /// \return 
  ///   Undefined value if not present, else the value.
  ///
  /// \sa GetMemberVariable
  SCALEFORM_IMPEXP const VScaleformValue GetMemberValue(const char* szMemberName);

  /// \brief
  ///   Creates or modifies the member variable with the specified name and the value.
  ///
  /// \param szMemberName  
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
  SCALEFORM_IMPEXP bool SetMember(const char* szMemberName, const VScaleformValue& value);

  /// \brief
  ///   Deletes the dynamic member variable with the specified name.
  ///
  /// \param szMemberName  
  ///   The name of the member variable.
  /// \return
  ///   true if the member existed and has been deleted.
  ///
  /// \note
  ///   The member variable must have previously been added by SetMember().
  /// \sa
  ///   SetMember
  SCALEFORM_IMPEXP bool DeleteMember(const char* szMemberName);

  /// \brief
  ///   Invokes a method on a object.
  /// 
  /// \param szMethodName
  ///   The name of the method.
  /// \param pArgs
  ///   Array of the arguments passed to the method. May be NULL.
  /// \param uiNumArgs
  ///   The number of the arguments in the passed array. May be zero.
  SCALEFORM_IMPEXP const VScaleformValue Invoke(const char* szMethodName, const VScaleformValue *pArgs, unsigned int uiNumArgs);

  inline const VScaleformValue Invoke(const char* szMethodName)
  {
    return Invoke(szMethodName, NULL, 0);
  }

  template<int N>
  inline const VScaleformValue Invoke(const char* szMethodName, const VScaleformValue (&args)[N])
  {
    return Invoke(szMethodName, args, N);
  }

  // @}

  /// @name Comparison operators
  /// @{

  SCALEFORM_IMPEXP bool operator==(const VScaleformValue& op) const;
  SCALEFORM_IMPEXP bool operator!=(const VScaleformValue& op) const;

  /// @}

  /// \brief
  ///   Internal functions which sets all object references which belong to the given movie instance to Undefined.
  static void InvalidateAllObjectReferences(const VScaleformMovieInstance* pMovieInstance);

private:
  // Private functions
  // Non-const version.
  inline Scaleform::GFx::Value& InternalGetGFxValue()
  { 
    return *reinterpret_cast<Scaleform::GFx::Value*>(m_gfxValueData); 
  }

  template<typename NumericType>
  inline NumericType GetNumericValue() const;

  inline bool PerformDisplayInfoCheck() const;
  inline bool PerformArrayCheck() const;

  void AddToObjectReferences();
  void RemoveFromObjectReferences();

  // Member variables
  const VScaleformMovieInstance* m_pMovieInstance;
  VString m_sStringBuffer;

  // Define size of Scaleform::GFx::Value.
  static const size_t GFxValueDataSize = 
#if defined(_VISION_ANDROID_X86) || defined(_VISION_IOS)
#   if defined(HK_DEBUG_SLOW)
      28;
#   else
      20;
#   endif
#elif defined(_WIN64)
#   if defined(HK_DEBUG_SLOW)
      48;
#   else
      32;
#   endif
#else
#   if defined(HK_DEBUG_SLOW)
      32;
#   else
      24;
#   endif
#endif
  unsigned char m_gfxValueData[GFxValueDataSize];

#if defined(HK_DEBUG)
  Scaleform::GFx::Value &m_gfxValue; // Reference to the actual GFx::Value for debugging purposes.
#endif

  static VArray<VScaleformValue*> s_objectReferences;
};

#endif // __VSCALEFORM_UTIL_HPP

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
