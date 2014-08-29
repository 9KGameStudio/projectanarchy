/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/ScaleformEnginePlugin.hpp>
#include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/VScaleformVariable.hpp>
#include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/VScaleformVariableManager.hpp>
#include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/VScaleformMovie.hpp>

//-----------------------------------------------------------------------------------

VScaleformVariable::VScaleformVariable(VScaleformVariableInfo* pInfo)
  : m_spInfo(pInfo)
{
  VASSERT(m_spInfo != NULL);
}

VScaleformVariable::VScaleformVariable()
  : m_spInfo(new VScaleformVariableInfo()) // Create dummy info with an undefined value.
{
}

VScaleformVariable::VScaleformVariable(const VScaleformVariable& op)
  : m_spInfo(op.m_spInfo)
{
  VASSERT(m_spInfo != NULL);
}

VScaleformVariable& VScaleformVariable::operator=(const VScaleformVariable &op)
{
  m_spInfo = op.m_spInfo;

  return *this;
}

VScaleformVariable::~VScaleformVariable()
{
}

//-----------------------------------------------------------------------------------

const VString VScaleformVariable::GetName() const
{
  return m_spInfo->m_sVarName;
}

const VScaleformValue& VScaleformVariable::GetValue() const
{
  return m_spInfo->m_value;
}

VScaleformValue& VScaleformVariable::InternalGetValue()
{
  return m_spInfo->m_value;
}

void VScaleformVariable::SetValue(const VScaleformValue& value)
{
  WaitForAdvanceFinished();
  m_spInfo->m_value = value;
  UpdateActionScript();
}

const VScaleformMovieInstance* VScaleformVariable::GetMovieInstance() const
{ 
  return m_spInfo->m_pMovieInstance; 
}

//-----------------------------------------------------------------------------------

void VScaleformVariable::WaitForAdvanceFinished()
{
  m_spInfo->m_pMovieInstance->WaitForAdvanceFinished();
}

void VScaleformVariable::UpdateActionScript()
{
  // Write new value to the Scaleform movie.
  VScaleformVariableManager::WriteSingleValueToAS(*m_spInfo);
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
