/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Input/VStringInputMap.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Input/VStringInputMapManager.hpp>

VStringInputMapManager VStringInputMapManager::s_globalManager;

VStringInputMapManager::VStringInputMapManager()
{
  m_instances.Reserve(4);
  m_instances.SetGrowBy(4);
}

VStringInputMapManager::~VStringInputMapManager()
{
}

void VStringInputMapManager::OneTimeInit()
{
}

void VStringInputMapManager::OneTimeDeInit()
{
  Release();
}

void VStringInputMapManager::Release()
{
  for(unsigned int i=0; i<(unsigned int)m_instances.GetSize(); i++)
  {
    V_SAFE_DELETE(m_instances[i]);
  }
}

unsigned int VStringInputMapManager::AddInstance(VStringInputMap *pInstance)
{
  for(unsigned int i=0; i<(unsigned int)m_instances.GetSize(); i++)
  {
    if(m_instances[i] == NULL)
    {
      m_instances[i] = pInstance;
      return i;
    }
  }
  return m_instances.Append(pInstance);
}

void VStringInputMapManager::RemoveInstance(unsigned int iIndex)
{
  VASSERT(iIndex < (unsigned int)m_instances.GetSize());
  m_instances[iIndex] = NULL;
}

void VStringInputMapManager::RemoveInstance(VStringInputMap *pInstance)
{
  if(pInstance == NULL)
    return;
  RemoveInstance(pInstance->GetIndex());
}

VStringInputMap* VStringInputMapManager::GetInstance(unsigned int iIndex) const
{
  VASSERT(iIndex < (unsigned int)m_instances.GetSize());
  return m_instances[iIndex];
}

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
