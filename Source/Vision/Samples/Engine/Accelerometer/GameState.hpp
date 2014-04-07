/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef GAMESTATE_HPP_INCLUDED
#define GAMESTATE_HPP_INCLUDED

enum GAME_STATE
{
  GAME_STATE_RUN,
  GAME_STATE_RESTART
};

class GameState : public VisBaseEntity_cl
{
public:
  virtual void InitFunction();

  virtual void DeInitFunction();

  virtual void ThinkFunction();

  virtual void MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB);

  void SetCurrentState(GAME_STATE eState);

  GAME_STATE GetCurrentState() const;

private:
  // private variables
  GAME_STATE m_eState;
  
  V_DECLARE_SERIAL(GameState, )
  IMPLEMENT_OBJ_CLASS(GameState);
};

#endif

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
