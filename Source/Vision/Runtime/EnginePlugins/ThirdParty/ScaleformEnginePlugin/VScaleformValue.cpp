/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/ScaleformEnginePlugin.hpp>
#include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/VScaleformValue.hpp>
#include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/vScaleformManager.hpp>
#include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/VScaleformMovie.hpp>
#include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/VScaleformVariableManager.hpp>
#include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/VScaleformArgumentsHelper.hpp>

#include <GFx/GFx_Player.h>

using namespace Scaleform;

// Store object reference list in order to set object values to Undefined when the movie gets unloaded.
VArray<VScaleformValue*> VScaleformValue::s_objectReferences;

//-----------------------------------------------------------------------------------
// VScaleformValue Construction
// 
// We manually call the constructor / destructor for the GFx::Value data block.
// This way we can forward-declare GFx::Value in the header without performing any allocations on the heap.

VScaleformValue::VScaleformValue(const GFx::Value& gfxValue, const VScaleformMovieInstance* pMovieInstance) 
  : m_pMovieInstance(pMovieInstance) 
  , m_sStringBuffer()
#if defined(HK_DEBUG)
  , m_gfxValue(InternalGetGFxValue())
#endif
{
  // Make sure our data block for the Scaleform::GFx::Value has the correct size.
  V_COMPILE_ASSERT(VScaleformValue::GFxValueDataSize == sizeof(GFx::Value));

  VASSERT_MSG(!gfxValue.IsObject() || m_pMovieInstance != NULL, "VScaleformValue: Object references need a movie instance.");

  // String memory is always managed by this object, since we don't want to keep Scaleform references for them.
  // These would need to be invalidated when the movie gets destroyed (just as for object references).
  if (gfxValue.IsString())
  {
    m_sStringBuffer = gfxValue.GetString();
    new (m_gfxValueData) GFx::Value(m_sStringBuffer.AsChar());
  }
  else
  {
    new (m_gfxValueData) GFx::Value(gfxValue);
  }

  AddToObjectReferences();
}

void VScaleformValue::SetGFxValue(const Scaleform::GFx::Value& gfxValue, const VScaleformMovieInstance* pMovieInstance)
{
  const bool bOldIsObject = IsObject();
  if (bOldIsObject && !gfxValue.IsObject())
  {
    RemoveFromObjectReferences();
  }

  // (Re)Initialize
  InternalGetGFxValue() = gfxValue;
  m_pMovieInstance = pMovieInstance;
  m_sStringBuffer.Reset();

  // Set string value from local string buffer.
  if (gfxValue.IsString())
  {
    m_sStringBuffer = gfxValue.GetString();
    InternalGetGFxValue().SetString(m_sStringBuffer);
  }

  if (!bOldIsObject && gfxValue.IsObject())
  {
    AddToObjectReferences();
  }
}

//-----------------------------------------------------------------------------------

VScaleformValue::VScaleformValue()
  : m_pMovieInstance(NULL)
  , m_sStringBuffer()
#if defined(HK_DEBUG)
  , m_gfxValue(InternalGetGFxValue())
#endif
{
  new (m_gfxValueData) GFx::Value();
}

VScaleformValue::VScaleformValue(bool bValue)
  : m_pMovieInstance(NULL)
  , m_sStringBuffer()
#if defined(HK_DEBUG)
  , m_gfxValue(InternalGetGFxValue())
#endif
{
  new (m_gfxValueData) GFx::Value(bValue);
}

VScaleformValue::VScaleformValue(int iValue)
  : m_pMovieInstance(NULL)
  , m_sStringBuffer()
#if defined(HK_DEBUG)
  , m_gfxValue(InternalGetGFxValue())
#endif
{
  new (m_gfxValueData) GFx::Value(static_cast<Scaleform::SInt32>(iValue));
}

VScaleformValue::VScaleformValue(unsigned int uiValue)
  : m_pMovieInstance(NULL)
  , m_sStringBuffer()
#if defined(HK_DEBUG)
  , m_gfxValue(InternalGetGFxValue())
#endif
{
  new (m_gfxValueData) GFx::Value(static_cast<Scaleform::UInt32>(uiValue));
}

VScaleformValue::VScaleformValue(float fValue)
  : m_pMovieInstance(NULL)
  , m_sStringBuffer()
#if defined(HK_DEBUG)
  , m_gfxValue(InternalGetGFxValue())
#endif
{
  new (m_gfxValueData) GFx::Value(fValue);
}

VScaleformValue::VScaleformValue(const char* szString)
  : m_pMovieInstance(NULL)
  , m_sStringBuffer(szString)
#if defined(HK_DEBUG)
  , m_gfxValue(InternalGetGFxValue())
#endif
{
  new (m_gfxValueData) GFx::Value(m_sStringBuffer.AsChar());
}

VScaleformValue::~VScaleformValue()
{
  RemoveFromObjectReferences();

  // Manually call destructor of GFx::Value object.
  InternalGetGFxValue().~Value();
}

VScaleformValue::VScaleformValue(const VScaleformValue& op)
  : m_pMovieInstance(op.m_pMovieInstance)
  , m_sStringBuffer(op.m_sStringBuffer)
#if defined(HK_DEBUG)
  , m_gfxValue(InternalGetGFxValue())
#endif
{
  new (m_gfxValueData) GFx::Value(op.GetGFxValue());

  // Set string value from local string buffer.
  if (InternalGetGFxValue().IsString())
  {
    InternalGetGFxValue().SetString(m_sStringBuffer);
  }

  AddToObjectReferences();
}

VScaleformValue& VScaleformValue::operator=(const VScaleformValue& op)
{
  // Handle self-assignment.
  if (this == &op)
    return *this;

  SetGFxValue(op.GetGFxValue(), op.m_pMovieInstance);

  return *this;
}

//-----------------------------------------------------------------------------------
// Type Checks

bool VScaleformValue::IsUndefined() const
{
  return GetGFxValue().IsUndefined();
}

bool VScaleformValue::IsNull() const
{
  return GetGFxValue().IsNull();
}

bool VScaleformValue::IsBool() const
{
  return GetGFxValue().IsBool();
}

bool VScaleformValue::IsInt() const
{
  return GetGFxValue().IsInt();
}

bool VScaleformValue::IsUInt() const
{
  return GetGFxValue().IsUInt();
}

bool VScaleformValue::IsNumber() const 
{
  return GetGFxValue().IsNumber();
}

bool VScaleformValue::IsNumeric() const
{
  return GetGFxValue().IsNumeric();
}

bool VScaleformValue::IsString() const
{
  return GetGFxValue().IsString();
}

bool VScaleformValue::IsArray() const
{
  return GetGFxValue().IsArray();
}

bool VScaleformValue::IsObject() const
{
  return GetGFxValue().IsObject();
}

bool VScaleformValue::IsDisplayObject() const
{
  return GetGFxValue().IsDisplayObject();
}

//-----------------------------------------------------------------------------------
// Primitive Type Getters

const VString VScaleformValue::ToString() const
{
  if (IsObject())
    m_pMovieInstance->WaitForAdvanceFinished();

  return GetGFxValue().ToString().ToCStr();
}

bool VScaleformValue::GetBool() const
{ 
  if (!IsBool())
  {
    hkvLog::Warning("VScaleformValue: Value is not boolean.");
    return false;
  }

  return GetGFxValue().GetBool();
}

int VScaleformValue::GetInt() const
{ 
  return GetNumericValue<int>();
}

unsigned int VScaleformValue::GetUInt() const
{
  return GetNumericValue<unsigned int>();
}

float VScaleformValue::GetNumber() const
{
  return GetNumericValue<float>();
}

template<typename NumericType>
inline NumericType VScaleformValue::GetNumericValue() const
{
  switch (GetGFxValue().GetType())
  {
  case GFx::Value::VT_Int:
    return static_cast<NumericType>(GetGFxValue().GetInt());
  case GFx::Value::VT_UInt:
    return static_cast<NumericType>(GetGFxValue().GetUInt());
  case GFx::Value::VT_Number:
    return static_cast<NumericType>(GetGFxValue().GetNumber());
  }

  hkvLog::Warning("VScaleformValue: Value is not numeric.");
  return static_cast<NumericType>(0);
}

const char* VScaleformValue::GetString() const 
{
  if (!IsString())
  {
    hkvLog::Warning("VScaleformValue: Value is not a string.");
    return "";
  }

  return GetGFxValue().GetString();
}

//-----------------------------------------------------------------------------------
// Display Info Getters

inline bool VScaleformValue::PerformDisplayInfoCheck() const
{
  if (!IsObject())
  {
    hkvLog::Warning("VScaleformValue: Value does not reference a display object.");
    return false;
  }

  VASSERT(m_pMovieInstance != NULL);
  m_pMovieInstance->WaitForAdvanceFinished();

  if (!IsDisplayObject())
  {
    hkvLog::Warning("VScaleformValue: Value does not reference a display object.");
    return false;
  }
  else
  {
    return true;
  }
}

bool VScaleformValue::Display_GetVisible() const
{
  if (!PerformDisplayInfoCheck())
    return false;

  GFx::Value::DisplayInfo info;
  GetGFxValue().GetDisplayInfo(&info);
  return info.GetVisible();
}

float VScaleformValue::Display_GetX() const
{
  if (!PerformDisplayInfoCheck())
    return 0.0f;

  GFx::Value::DisplayInfo info;
  GetGFxValue().GetDisplayInfo(&info);
  return static_cast<float>(info.GetX());
}

float VScaleformValue::Display_GetY() const
{
  if (!PerformDisplayInfoCheck())
    return 0.0f;

  GFx::Value::DisplayInfo info;
  GetGFxValue().GetDisplayInfo(&info);
  return static_cast<float>(info.GetY());
}

float VScaleformValue::Display_GetZ() const
{
  if (!PerformDisplayInfoCheck())
    return 0.0f;

  GFx::Value::DisplayInfo info;
  GetGFxValue().GetDisplayInfo(&info);
  return static_cast<float>(info.GetZ());
}

float VScaleformValue::Display_GetXRotation() const
{
  if (!PerformDisplayInfoCheck())
    return 0.0f;

  GFx::Value::DisplayInfo info;
  GetGFxValue().GetDisplayInfo(&info);
  return static_cast<float>(info.GetXRotation());
}

float VScaleformValue::Display_GetYRotation() const
{
  if (!PerformDisplayInfoCheck())
    return 0.0f;

  GFx::Value::DisplayInfo info;
  GetGFxValue().GetDisplayInfo(&info);
  return static_cast<float>(info.GetYRotation());
}

float VScaleformValue::Display_GetRotation() const
{
  if (!PerformDisplayInfoCheck())
    return 0.0f;

  GFx::Value::DisplayInfo info;
  GetGFxValue().GetDisplayInfo(&info);
  return static_cast<float>(info.GetRotation());
}

float VScaleformValue::Display_GetXScale() const
{
  if (!PerformDisplayInfoCheck())
    return 0.0f;

  GFx::Value::DisplayInfo info;
  GetGFxValue().GetDisplayInfo(&info);
  return static_cast<float>(info.GetXScale());
}

float VScaleformValue::Display_GetYScale() const
{
  if (!PerformDisplayInfoCheck())
    return 0.0f;

  GFx::Value::DisplayInfo info;
  GetGFxValue().GetDisplayInfo(&info);
  return static_cast<float>(info.GetYScale());
}

float VScaleformValue::Display_GetZScale() const
{
  if (!PerformDisplayInfoCheck())
    return 0.0f;

  GFx::Value::DisplayInfo info;
  GetGFxValue().GetDisplayInfo(&info);
  return static_cast<float>(info.GetZScale());
}

float VScaleformValue::Display_GetAlpha() const
{
  if (!PerformDisplayInfoCheck())
    return 0.0f;

  GFx::Value::DisplayInfo info;
  GetGFxValue().GetDisplayInfo(&info);
  return static_cast<float>(info.GetAlpha());
}

float VScaleformValue::Display_GetFOV() const
{
  if (!PerformDisplayInfoCheck())
    return 0.0f;

  GFx::Value::DisplayInfo info;
  GetGFxValue().GetDisplayInfo(&info);
  return static_cast<float>(info.GetFOV());
}

//-----------------------------------------------------------------------------------
// Primitive Type Setters

void VScaleformValue::SetUndefined()
{
  m_sStringBuffer.Reset();
  RemoveFromObjectReferences();

  InternalGetGFxValue().SetUndefined();
}

void VScaleformValue::SetNull()
{
  m_sStringBuffer.Reset();
  RemoveFromObjectReferences();

  InternalGetGFxValue().SetNull();
}

void VScaleformValue::SetBool(bool bValue)
{
  m_sStringBuffer.Reset();
  RemoveFromObjectReferences();

  InternalGetGFxValue().SetBoolean(bValue);
}

void VScaleformValue::SetInt(int iValue)
{
  m_sStringBuffer.Reset();
  RemoveFromObjectReferences();

  InternalGetGFxValue().SetInt(iValue);
}

void VScaleformValue::SetUInt(unsigned int uiValue)
{
  m_sStringBuffer.Reset();
  RemoveFromObjectReferences();

  InternalGetGFxValue().SetUInt(uiValue);
}

void VScaleformValue::SetNumber(float fValue)
{
  m_sStringBuffer.Reset();
  RemoveFromObjectReferences();

  InternalGetGFxValue().SetNumber(fValue);
}

void VScaleformValue::SetString(const char* szString)
{
  RemoveFromObjectReferences();

  m_sStringBuffer = szString;
  InternalGetGFxValue().SetString(m_sStringBuffer);
}

//-----------------------------------------------------------------------------------
// Display Info Setters

void VScaleformValue::Display_SetVisible(bool bVisible)
{ 
  if (!PerformDisplayInfoCheck())
    return;

  GFx::Value::DisplayInfo info;
  InternalGetGFxValue().GetDisplayInfo(&info);
  info.SetVisible(bVisible);
  InternalGetGFxValue().SetDisplayInfo(info);
}

void VScaleformValue::Display_SetX(float fX)
{ 
  if (!PerformDisplayInfoCheck())
    return;

  GFx::Value::DisplayInfo info;
  InternalGetGFxValue().GetDisplayInfo(&info);
  info.SetX(fX);
  InternalGetGFxValue().SetDisplayInfo(info);
}

void VScaleformValue::Display_SetY(float fY)
{ 
  if (!PerformDisplayInfoCheck())
    return;

  GFx::Value::DisplayInfo info;
  InternalGetGFxValue().GetDisplayInfo(&info);
  info.SetY(fY);
  InternalGetGFxValue().SetDisplayInfo(info);
}

void VScaleformValue::Display_SetXY(float fX, float fY)
{ 
  if (!PerformDisplayInfoCheck())
    return;

  GFx::Value::DisplayInfo info;
  InternalGetGFxValue().GetDisplayInfo(&info);
  info.SetX(fX);
  info.SetY(fY);
  InternalGetGFxValue().SetDisplayInfo(info);
}

void VScaleformValue::Display_SetZ(float fZ)
{ 
  if (!PerformDisplayInfoCheck())
    return;

  GFx::Value::DisplayInfo info;
  InternalGetGFxValue().GetDisplayInfo(&info);
  info.SetZ(fZ);
  InternalGetGFxValue().SetDisplayInfo(info);
}

void VScaleformValue::Display_SetXRotation(float fDeg)
{ 
  if (!PerformDisplayInfoCheck())
    return;

  GFx::Value::DisplayInfo info;
  InternalGetGFxValue().GetDisplayInfo(&info);
  info.SetXRotation(fDeg);
  InternalGetGFxValue().SetDisplayInfo(info);
}

void VScaleformValue::Display_SetYRotation(float fDeg)
{ 
  if (!PerformDisplayInfoCheck())
    return;

  GFx::Value::DisplayInfo info;
  InternalGetGFxValue().GetDisplayInfo(&info);
  info.SetYRotation(fDeg);
  InternalGetGFxValue().SetDisplayInfo(info);
}

void VScaleformValue::Display_SetXYRotation(float fXDeg, float fYDeg)
{
  if (!PerformDisplayInfoCheck())
    return;

  GFx::Value::DisplayInfo info;
  InternalGetGFxValue().GetDisplayInfo(&info);
  info.SetXRotation(fXDeg);
  info.SetYRotation(fYDeg);
  InternalGetGFxValue().SetDisplayInfo(info);
}

void VScaleformValue::Display_SetRotation(float fDeg)
{ 
  if (!PerformDisplayInfoCheck())
    return;

  GFx::Value::DisplayInfo info;
  InternalGetGFxValue().GetDisplayInfo(&info);
  info.SetRotation(fDeg);
  InternalGetGFxValue().SetDisplayInfo(info);
}

void VScaleformValue::Display_SetXScale(float fXScale)
{ 
  if (!PerformDisplayInfoCheck())
    return;

  GFx::Value::DisplayInfo info;
  InternalGetGFxValue().GetDisplayInfo(&info);
  info.SetXScale(fXScale);
  InternalGetGFxValue().SetDisplayInfo(info);
}

void VScaleformValue::Display_SetYScale(float fYScale)
{ 
  if (!PerformDisplayInfoCheck())
    return;

  GFx::Value::DisplayInfo info;
  InternalGetGFxValue().GetDisplayInfo(&info);
  info.SetYScale(fYScale);
  InternalGetGFxValue().SetDisplayInfo(info);
}

void VScaleformValue::Display_SetXYScale(float fXScale, float fYScale)
{
  if (!PerformDisplayInfoCheck())
    return;

  GFx::Value::DisplayInfo info;
  InternalGetGFxValue().GetDisplayInfo(&info);
  info.SetXScale(fXScale);
  info.SetYScale(fYScale);
  InternalGetGFxValue().SetDisplayInfo(info);
}

void VScaleformValue::Display_SetZScale(float fZScale)
{ 
  if (!PerformDisplayInfoCheck())
    return;

  GFx::Value::DisplayInfo info;
  InternalGetGFxValue().GetDisplayInfo(&info);
  info.SetZScale(fZScale);
  InternalGetGFxValue().SetDisplayInfo(info);
}

void VScaleformValue::Display_SetAlpha(float fAlpha)
{ 
  if (!PerformDisplayInfoCheck())
    return;

  GFx::Value::DisplayInfo info;
  InternalGetGFxValue().GetDisplayInfo(&info);
  info.SetAlpha(fAlpha);
  InternalGetGFxValue().SetDisplayInfo(info);
}

void VScaleformValue::Display_SetFOV(float fFov)
{ 
  if (!PerformDisplayInfoCheck())
    return;

  GFx::Value::DisplayInfo info;
  InternalGetGFxValue().GetDisplayInfo(&info);
  info.SetFOV(fFov);
  InternalGetGFxValue().SetDisplayInfo(info);
}

bool VScaleformValue::Display_GotoFrame(unsigned int uiFrameIndex, bool bPlay)
{
  if (!PerformDisplayInfoCheck())
    return false;

  if (bPlay)
    return InternalGetGFxValue().GotoAndPlay(uiFrameIndex);
  else
    return InternalGetGFxValue().GotoAndStop(uiFrameIndex);
}

bool VScaleformValue::Display_GotoLabeledFrame(const char* szLabel, bool bPlay)
{
  if (!PerformDisplayInfoCheck())
    return false;

  if (bPlay)
    return InternalGetGFxValue().GotoAndPlay(szLabel);
  else
    return InternalGetGFxValue().GotoAndStop(szLabel);
}

//-----------------------------------------------------------------------------------
// Arrays

bool VScaleformValue::PerformArrayCheck() const
{
  if (!IsObject())
  {
    hkvLog::Warning("VScaleformValue: Value does not reference an array.\n");
    return false;
  }

  VASSERT(m_pMovieInstance != NULL);
  m_pMovieInstance->WaitForAdvanceFinished();

  if (!IsArray())
  {
    hkvLog::Warning("VScaleformValue: Value does not reference an array.\n");
    return false;
  }
  else
  {
    return true;
  }
}

unsigned int VScaleformValue::GetArraySize() const
{
  if (!PerformArrayCheck())
    return 0;

  return GetGFxValue().GetArraySize();
}

void VScaleformValue::SetArraySize(unsigned int uiSize)
{
  if (!PerformArrayCheck())
    return;

  InternalGetGFxValue().SetArraySize(uiSize);
}

const VScaleformValue VScaleformValue::GetArrayElement(unsigned int uiIndex) const
{
  if (!PerformArrayCheck())
    return VScaleformValue();

  GFx::Value gfxValue;
  if (!GetGFxValue().GetElement(uiIndex, &gfxValue))
    return VScaleformValue();

  return VScaleformValue(gfxValue, m_pMovieInstance);
}

void VScaleformValue::SetArrayElement(unsigned int uiIndex, const VScaleformValue& value)
{
  if (!PerformArrayCheck())
    return;

  InternalGetGFxValue().SetElement(uiIndex, value.GetGFxValue());
}

//-----------------------------------------------------------------------------------

bool VScaleformValue::IsInstanceOf(const char* szClassName) const
{
  if (!IsObject())
    return false;

  VASSERT(m_pMovieInstance != NULL);
  m_pMovieInstance->WaitForAdvanceFinished();
  return GetGFxValue().IsInstanceOf(szClassName);
}

//-----------------------------------------------------------------------------------

bool VScaleformValue::HasMember(const char* szMemberName) const
{
  if (!IsObject() || VStringUtil::IsEmpty(szMemberName))
    return false;

  VASSERT(m_pMovieInstance != NULL);
  m_pMovieInstance->WaitForAdvanceFinished();
  return (GetGFxValue().HasMember(szMemberName));
}

const VScaleformVariable VScaleformValue::GetMemberVariable(const char* szMemberName)
{
  if (!IsObject() || VStringUtil::IsEmpty(szMemberName))
    return VScaleformVariable();

  // Wait until the advance task has finished before retrieving the value.
  m_pMovieInstance->WaitForAdvanceFinished();

  GFx::Value memberGFxValue;
  if (!GetGFxValue().GetMember(szMemberName, &memberGFxValue))
    return VScaleformVariable(); // Return invalid variable.

  VASSERT(m_pMovieInstance != NULL);
  return m_pMovieInstance->m_pVariableManager->CreateVariable(memberGFxValue, szMemberName, m_pMovieInstance, GetGFxValue());
}

const VScaleformValue VScaleformValue::GetMemberValue(const char* szMemberName)
{
  if (!IsObject() || VStringUtil::IsEmpty(szMemberName))
    return VScaleformValue();

  // Wait until the advance task has finished before retrieving the value.
  m_pMovieInstance->WaitForAdvanceFinished();

  GFx::Value memberGFxValue;
  if (!GetGFxValue().GetMember(szMemberName, &memberGFxValue))
    return VScaleformValue(); // Return Undefined.

  VASSERT(m_pMovieInstance != NULL);
  return VScaleformValue(memberGFxValue, m_pMovieInstance);
}

bool VScaleformValue::SetMember(const char* szMemberName, const VScaleformValue& value)
{
  if (!IsObject() || VStringUtil::IsEmpty(szMemberName))
    return false;

  m_pMovieInstance->WaitForAdvanceFinished();
  return InternalGetGFxValue().SetMember(szMemberName, value.GetGFxValue());
}

bool VScaleformValue::DeleteMember(const char* szMemberName)
{
  if (!IsObject() || VStringUtil::IsEmpty(szMemberName))
    return false;

  m_pMovieInstance->WaitForAdvanceFinished();
  return InternalGetGFxValue().DeleteMember(szMemberName);
}

//-----------------------------------------------------------------------------------

const VScaleformValue VScaleformValue::Invoke(const char* szFunctionName, const VScaleformValue* pArgs, unsigned int uiNumArgs)
{
  if (!IsObject())
  {
    hkvLog::Warning("VScaleformValue: Invoke('%s') can only be called on object references.\n", szFunctionName);
    return VScaleformValue();
  }

  m_pMovieInstance->WaitForAdvanceFinished();

  GFx::Value gfxResultValue;
  if (uiNumArgs > 0)
  {
    // Convert to GFx argument array.
    const VScaleformArgumentsHelper args(pArgs, uiNumArgs);

    // If the call is not successful the return value will be undefined.
    InternalGetGFxValue().Invoke(szFunctionName, &gfxResultValue, args.GetGFxArguments(), uiNumArgs);
  }
  else
  {
    InternalGetGFxValue().Invoke(szFunctionName, &gfxResultValue);
  }
  
  VASSERT(m_pMovieInstance != NULL);
  return VScaleformValue(gfxResultValue, m_pMovieInstance);
}

//-----------------------------------------------------------------------------------

bool VScaleformValue::operator==(const VScaleformValue& op) const
{
  return (GetGFxValue() == op.GetGFxValue());
}

bool VScaleformValue::operator!=(const VScaleformValue& op) const
{
  return !(GetGFxValue() == op.GetGFxValue());
}

//-----------------------------------------------------------------------------------

void VScaleformValue::AddToObjectReferences()
{
  VASSERT(s_objectReferences.Find(this) == -1);

  if (IsObject())
  {
    s_objectReferences.Add(this);
  }
}

void VScaleformValue::RemoveFromObjectReferences()
{
  if (IsObject())
  {
    const int iIndex = s_objectReferences.Find(this);
    VASSERT(iIndex != -1);

    // Swap with last element.
    s_objectReferences.RemoveAtSwapWithLast(iIndex);
  }
  else
  {
    VASSERT(s_objectReferences.Find(this) == -1);
  }
}

/*static*/ void VScaleformValue::InvalidateAllObjectReferences(const VScaleformMovieInstance* pMovieInstance)
{
  for (int i = 0; i < s_objectReferences.GetSize(); i++)
  {
    VScaleformValue* pValue = s_objectReferences[i];
    VASSERT(pValue->IsObject());

    if (pValue->m_pMovieInstance == pMovieInstance)
    {
      pValue->RemoveFromObjectReferences();
      pValue->InternalGetGFxValue().SetUndefined();
      pValue->m_pMovieInstance = NULL;
      i--;
    }
  }
}

//-----------------------------------------------------------------------------------

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
