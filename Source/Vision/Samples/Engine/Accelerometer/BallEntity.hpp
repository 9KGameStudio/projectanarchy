/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef BALLENTITY_HPP_INCLUDED
#define BALLENTITY_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokRigidBody.hpp>

#define BALL_MASS 10000.0f

class BallEntity : public VisBaseEntity_cl
{
public:
  // constructor
  BallEntity();

  // destructor
  ~BallEntity();

  // is called upon entity initialization
  virtual void InitFunction();

  // is called upon entity de-initialization
  virtual void DeInitFunction();

  // is called each frame
  virtual void ThinkFunction();

  void ApplyForce(hkvVec3 &vForce, float deltaT);

  void SetPosition(const hkvVec3 &vPosition);

private:
  // private variables
  vHavokRigidBody *m_pRigidBody;
  

  V_DECLARE_SERIAL( BallEntity, )
  IMPLEMENT_OBJ_CLASS(BallEntity);
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
