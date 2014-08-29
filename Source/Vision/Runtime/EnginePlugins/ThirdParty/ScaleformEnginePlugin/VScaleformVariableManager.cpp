/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/ScaleformEnginePlugin.hpp>
#include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/VScaleformVariableManager.hpp>
#include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/VScaleformMovie.hpp>
#include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/VScaleformVariable.hpp>

using namespace Scaleform;

//-----------------------------------------------------------------------------------

VScaleformVariableManager::VScaleformVariableManager()
  : m_infos()
{
}

VScaleformVariableManager::~VScaleformVariableManager()
{
  Reset();
}

//-----------------------------------------------------------------------------------

const VScaleformVariable VScaleformVariableManager::CreateVariable(const GFx::Value& gfxValue, const char* szVarName,
  const VScaleformMovieInstance* pMovieInstance, const GFx::Value& parentGFxValue)
{
  // Check if an info has already been created.
  VScaleformVariableInfo* pInfo = FindInfo(szVarName, parentGFxValue);
  if (pInfo != NULL)
    return VScaleformVariable(pInfo);

  // Create new info object.
  pInfo = new VScaleformVariableInfo();
  pInfo->m_value = VScaleformValue(gfxValue, pMovieInstance);
  pInfo->m_sVarName = szVarName;
  pInfo->m_parentGFxValue = parentGFxValue;
  pInfo->m_pMovieInstance = pMovieInstance;

  m_infos.Add(pInfo);

  return VScaleformVariable(pInfo);
}

void VScaleformVariableManager::Reset()
{
  for (int i = 0; i < m_infos.GetSize(); i++)
  {
    VScaleformVariableInfo* pInfo = m_infos[i];

    // Invalidate infos. Remaining VScaleformValue objects may still hold a reference, especially when using LUA.
    pInfo->m_value.SetUndefined();
    pInfo->m_sVarName.Reset();
    pInfo->m_parentGFxValue.SetUndefined();
  }

  m_infos.RemoveAll();
}

VScaleformVariableInfo* VScaleformVariableManager::FindInfo(const char* szName, const GFx::Value& parentGFxValue)
{
  for (int i = 0; i < m_infos.GetSize(); i++)
  {
    VScaleformVariableInfo* pInfo = m_infos[i];

    // Variables need to have the same parent value.
    // Note: If there is no parent, both values are Undefined and thus the same.
    if (!(parentGFxValue == pInfo->m_parentGFxValue))
      continue;

    if (pInfo->m_sVarName == szName)
      return pInfo;
  }
  return NULL;
}

void VScaleformVariableManager::SynchronizeRead()
{
  for (int i = 0; i < m_infos.GetSize(); i++)
  {
    VScaleformVariableInfo* pInfo = m_infos[i];

    // If no VScaleformVariable object references the control anymore, remove it.
    if (pInfo->GetRefCount() == 1)
    {
      m_infos.RemoveAtSwapWithLast(i);
      i--;
    }
    else
    {
      ReadSingleValueFromAS(*pInfo);
    }
  }
}

void VScaleformVariableManager::ReadSingleValueFromAS(VScaleformVariableInfo& info)
{
  // Re-read value.
  Scaleform::GFx::Value newGFxValue;
  if (info.m_parentGFxValue.IsObject())
  {
    info.m_parentGFxValue.GetMember(info.m_sVarName, &newGFxValue);
  }
  else
  {
    info.m_pMovieInstance->GetGFxMovieInstance()->GetVariable(&newGFxValue, info.m_sVarName);
  }

  // Re-assign new value. This way, type changes are handled correctly.
  if (!(info.m_value.GetGFxValue() == newGFxValue))
  {
    info.m_value.SetGFxValue(newGFxValue, info.m_pMovieInstance);
  }
}

void VScaleformVariableManager::WriteSingleValueToAS(VScaleformVariableInfo& info)
{
  // Re-set value.
  if (info.m_parentGFxValue.IsObject())
  {
    info.m_parentGFxValue.SetMember(info.m_sVarName, info.m_value.GetGFxValue());
  }
  else
  {
    info.m_pMovieInstance->GetGFxMovieInstance()->SetVariable(info.m_sVarName, info.m_value.GetGFxValue());
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
