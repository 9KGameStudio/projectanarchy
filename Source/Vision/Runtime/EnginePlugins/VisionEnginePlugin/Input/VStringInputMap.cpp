/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>      
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Input/VStringInputMapManager.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Input/VStringInputMap.hpp>


VStringInputMap::VStringInputMap(int iNumTriggers, int iNumAlternatives):
  VInputMap(iNumTriggers, iNumAlternatives),
  m_hashMap(iNumTriggers)
{
  m_iManagerIndex = VStringInputMapManager::GlobalManager().AddInstance(this);
}

VStringInputMap::~VStringInputMap()
{
  VStringInputMapManager::GlobalManager().RemoveInstance(m_iManagerIndex);
}

int VStringInputMap::MapTrigger(const char* szTriggerName, IVInputDevice &inputDevice, unsigned int uiControl, const VInputOptions &options, int iOptTriggerIndex)
{
  int iTriggerIndex = 0;

  //check if the string key exists in the hash map
  if(!m_hashMap.Lookup(szTriggerName, iTriggerIndex))
  {
    if(iOptTriggerIndex!=-1)
    {
      iTriggerIndex = iOptTriggerIndex;
    }
    else
    {
      iTriggerIndex = GetNextFreeTriggerIndex();

      if(iTriggerIndex<0)
        return -1;
    }

    //store the new trigger index assignment in the hash map and map it
    m_hashMap.SetAt(szTriggerName, iTriggerIndex);
    return VInputMap::MapTrigger(iTriggerIndex, inputDevice, uiControl, options);
  }

  //we have to de-reference the pointer, since we are interested into the value and the hash map just handles pointers
  return VInputMap::MapTrigger(iTriggerIndex, inputDevice, uiControl, options);
}

int VStringInputMap::MapTrigger(const char* szTriggerName, VTouchArea* pArea, unsigned int uiControl, const VInputOptions &options, int iOptTriggerIndex)
{
  int iTriggerIndex = 0;

  //check if the string key exists in the hash map
  if(!m_hashMap.Lookup(szTriggerName, iTriggerIndex))
  {
    if(iOptTriggerIndex!=-1)
    {
      iTriggerIndex = iOptTriggerIndex;
    }
    else
    {
      iTriggerIndex = GetNextFreeTriggerIndex();

      if(iTriggerIndex<0)
        return -1;
    }

    m_hashMap.SetAt(szTriggerName, iTriggerIndex);

    //store the new trigger index assignment in the hash map and map it
    return VInputMap::MapTrigger(iTriggerIndex, pArea, uiControl, options);
  }

  //we have to de-reference the pointer, since we are interested into the value and the hash map just handles pointers
  return VInputMap::MapTrigger(iTriggerIndex, pArea, uiControl, options);
}

int VStringInputMap::MapTriggerAxis(const char* szTriggerName, IVInputDevice &inputDevice, unsigned int uiControlNegative, unsigned int uiControlPositive, const VInputOptions &options, int iOptTriggerIndex)
{
  int iTriggerIndex = 0;

  //check if the string key exists in the hash map
  if(!m_hashMap.Lookup(szTriggerName, iTriggerIndex))
  {
    if(iOptTriggerIndex!=-1)
    {
      iTriggerIndex = iOptTriggerIndex;
    }
    else
    {
      iTriggerIndex = GetNextFreeTriggerIndex();

      if(iTriggerIndex<0)
        return -1;
    }

    m_hashMap.SetAt(szTriggerName, iTriggerIndex);

    //store the new trigger index assignment in the hash map and map it
    return VInputMap::MapTriggerAxis(iTriggerIndex, inputDevice, uiControlNegative, uiControlPositive, options);
  }

  //we have to de-reference the pointer, since we are interested into the value and the hash map just handles pointers
  return VInputMap::MapTriggerAxis(iTriggerIndex, inputDevice, uiControlNegative, uiControlPositive, options);
}

int VStringInputMap::GetNextFreeTriggerIndex() const
{
  //check all trigger indices
  for (int iTriggerIndex=0;iTriggerIndex<m_iNumMappedInputs;iTriggerIndex++)
  {
    bool bIsFree = true;

    //a trigger index is considered as free (available) if no alternative is in use
    for(int iAlt=0;iAlt<m_iNumAlternatives;iAlt++)
    {
      if(m_ppMappedInputs[iTriggerIndex*m_iNumAlternatives + iAlt]!=NULL)
      {
        bIsFree = false;
        break; //break inner loop
      }
    }

    if(bIsFree)
      return iTriggerIndex;
  }

  // No free trigger index was found.
  return -1;
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
