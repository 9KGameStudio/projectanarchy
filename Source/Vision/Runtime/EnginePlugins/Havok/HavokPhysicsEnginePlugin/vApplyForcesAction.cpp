/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vApplyForcesAction.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokRigidBody.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>
#include <Physics2012/Utilities/Dynamics/ImpulseAccumulator/hkpImpulseAccumulator.h>

vApplyForcesAction::vApplyForcesAction() : hkpUnaryAction(NULL)
  , m_pParent(NULL)
  , m_bActionAdded(false)
{
};

vApplyForcesAction::~vApplyForcesAction()
{
};

void vApplyForcesAction::SetEntity(vHavokRigidBody *pRigidBody)
{
  hkpWorld *pHavokWorld = vHavokPhysicsModule::GetInstance()->GetPhysicsWorld();
  pHavokWorld->lock();
  if (pRigidBody)
  {
    hkpRigidBody* pRB = pRigidBody->GetHkRigidBody();
    setEntity(pRB);
    pHavokWorld->addAction(this);
    m_bActionAdded = true;
  }
  else
  {
    if (hkpUnaryAction::getWorld())
    {
      pHavokWorld->removeAction(this);
      m_bActionAdded = false;
    }
  }
  pHavokWorld->unlock();
}

void vApplyForcesAction::applyAction(const hkStepInfo& stepInfo)
{ 
  int iForcesCount = 0;
  
  const IvHavokForcesQuerierComponent::Forces *pForces = m_pParent->QueryForces(stepInfo.m_deltaTime, iForcesCount);
  hkpRigidBody *pHkpRB = (hkpRigidBody *)getEntity();
  hkpImpulseAccumulator myAccum(pHkpRB, stepInfo.m_deltaTime);
  for(int i = 0; i < iForcesCount; i++)
  { 
    hkVector4 force; vHavokConversionUtils::VisVecToPhysVecLocal(pForces[i].m_v3Dir, force);
    //hkVector4 point; vHavokConversionUtils::VisVecToPhysVecLocal(pForces[i].m_v3Point, point);
    hkVector4 point; vHavokConversionUtils::VisVecToPhysVecWorld(pForces[i].m_v3Point, point);
    
    myAccum.addForce(point, force);
  }

  myAccum.flushImpulses();
}


void vApplyForcesAction::entityRemovedCallback(hkpEntity* entity)
{
  hkpUnaryAction::entityRemovedCallback(entity);
  m_bActionAdded = false;
}

hkpAction* vApplyForcesAction::clone( const hkArray<hkpEntity*>& newEntities, const hkArray<hkpPhantom*>& newPhantoms ) const
{
  return NULL;
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
