/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Samples/Engine/Accelerometer/AccelerometerPCH.h>
#include <Vision/Samples/Engine/Accelerometer/GameState.hpp>

void GameState::InitFunction()
{
  // Add target component
  VisTriggerTargetComponent_cl *pTargetComp = new VisTriggerTargetComponent_cl();
  AddComponent(pTargetComp);

  // Get trigger box entity
  VisBaseEntity_cl *pTriggerBoxEntity = Vision::Game.SearchEntity("TriggerBox");
  VASSERT(pTriggerBoxEntity);

  // Find source component
  VisTriggerSourceComponent_cl* pSourceComp = vstatic_cast<VisTriggerSourceComponent_cl*>(
    pTriggerBoxEntity->Components().GetComponentOfTypeAndName(VisTriggerSourceComponent_cl::GetClassTypeId(), "OnObjectEnter"));
  VASSERT(pSourceComp != NULL);

  // Link source and target component 
  IVisTriggerBaseComponent_cl::OnLink(pSourceComp, pTargetComp);
  
  m_eState = GAME_STATE_RUN;
}

void GameState::DeInitFunction()
{
}

void GameState::ThinkFunction()
{
}

void GameState::MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB)
{
  VisObject3D_cl::MessageFunction(iID,iParamA,iParamB);

  if (iID==VIS_MSG_TRIGGER)
    m_eState = GAME_STATE_RESTART;
}

void GameState::SetCurrentState(GAME_STATE eState)
{
  m_eState = eState;
}

GAME_STATE GameState::GetCurrentState() const
{
  return m_eState;
}

V_IMPLEMENT_SERIAL(GameState, VisBaseEntity_cl, 0, Vision::GetEngineModule());

START_VAR_TABLE(GameState, VisBaseEntity_cl, "GameState", 0, "")
END_VAR_TABLE

/*
 * Havok SDK - Base file, BUILD(#20131218)
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
