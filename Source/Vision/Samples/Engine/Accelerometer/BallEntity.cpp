/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Samples/Engine/Accelerometer/AccelerometerPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Entities/TriggerBoxEntity.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>
#include <Vision/Samples/Engine/Accelerometer/BallEntity.hpp>

BallEntity::BallEntity()
{
  TriggerBoxEntity_cl::AddObservedEntity(this);
}

BallEntity::~BallEntity()
{
  TriggerBoxEntity_cl::RemoveObservedEntity(this);
}

// InitFunction: Is called upon entity initialization.
void BallEntity::InitFunction()
{
  // create a rigid body and attach it to the entity
  m_pRigidBody = new vHavokRigidBody();
  m_pRigidBody->Initialize();
  vHavokRigidBody::InitTemplate initTempl;
  
  const float fScale = 1.0f;
  m_pRigidBody->InitConvexRb(GetMesh(), hkvVec3(fScale), initTempl);

  AddComponent(m_pRigidBody);
}

// DeInitFunction:: Is called upon entity de-initialization.
void BallEntity::DeInitFunction()
{
}

// ThinkFunction: Is called once per frame.
void BallEntity::ThinkFunction()
{
}

void BallEntity::ApplyForce(hkvVec3 &vForce, float deltaT)
{
  m_pRigidBody->ApplyForce(vForce, deltaT);
}

void BallEntity::SetPosition(const hkvVec3 &vPosition)
{
  m_pRigidBody->SetPosition(vPosition);
}

V_IMPLEMENT_SERIAL(BallEntity, VisBaseEntity_cl, 0, Vision::GetEngineModule());

START_VAR_TABLE(BallEntity, VisBaseEntity_cl, "BallEntity", 0, "")
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
