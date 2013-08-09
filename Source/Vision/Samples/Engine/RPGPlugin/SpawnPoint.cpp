/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/SpawnPoint.h>

V_IMPLEMENT_SERIAL(RPG_SpawnPoint, RPG_BaseEntity, 0, &g_RPGPluginModule);

START_VAR_TABLE(RPG_SpawnPoint, RPG_BaseEntity, "Spawn Point Entity", 0, "")  
  DEFINE_VAR_BOOL_AND_NAME(RPG_SpawnPoint, m_displayDebug, "Display Debug", "Display Debug Info", "FALSE", 0, 0);
END_VAR_TABLE


RPG_SpawnPoint::RPG_SpawnPoint()
: RPG_BaseEntity()
, m_displayDebug(false)
{

}

void RPG_SpawnPoint::PostInitialize()
{
  RPG_BaseEntity::PostInitialize();

  if(Vision::Editor.IsPlayingTheGame() && !m_displayDebug)
  {
    // Spawn Point should be invisible during the game unless we're displaying the mesh for debug purposes.
    SetVisibleBitmask(VIS_ENTITY_INVISIBLE);
  }
}

/*
 * Havok SDK - Base file, BUILD(#20130723)
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2013
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
