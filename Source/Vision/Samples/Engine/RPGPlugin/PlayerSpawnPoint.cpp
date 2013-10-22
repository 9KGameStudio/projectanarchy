/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/PlayerSpawnPoint.h>

#include <Vision/Samples/Engine/RPGPlugin/PlayerControllerComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/GameManager.h>
#include <Vision/Samples/Engine/RPGPlugin/LevelInfo.h>

static const VString s_playerSpawnKey = "Player Spawn";       ///< static name used for the Object Key property

V_IMPLEMENT_SERIAL(RPG_PlayerSpawnPoint, RPG_SpawnPoint, 0, &g_RPGPluginModule);

START_VAR_TABLE(RPG_PlayerSpawnPoint, RPG_SpawnPoint, "Player Spawn Entity", 0, "")  
END_VAR_TABLE


RPG_PlayerSpawnPoint::RPG_PlayerSpawnPoint()
: RPG_SpawnPoint()
{

}

void RPG_PlayerSpawnPoint::Initialize()
{
  RPG_SpawnPoint::Initialize();

  if(Vision::Editor.IsInEditor())
  {
#if !defined(_VISION_MOBILE)
    Vision::Editor.SetVariableInEditor(this, "ShapeName", s_playerSpawnKey, false, false);
    Vision::Editor.SetVariableInEditor(this, "ObjectKey", s_playerSpawnKey, false, false);
#endif	
  }
}

void RPG_PlayerSpawnPoint::PostInitialize()
{
  RPG_SpawnPoint::PostInitialize();

  if(!Vision::Editor.IsPlayingTheGame() && 
     RPG_GameManager::s_instance.GetLevelInfo() &&
     !RPG_GameManager::s_instance.GetLevelInfo()->GetInitialPlayerSpawnPoint())
  {
    // automatically hookup the player spawn if we have a level info object and no player spawn set.
    RPG_GameManager::s_instance.GetLevelInfo()->SetInitialPlayerSpawnPoint(this);
  }
}

/*
 * Havok SDK - Base file, BUILD(#20131019)
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
