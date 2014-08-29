/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/HavokAiEnginePlugin.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/vHavokAiObstacle.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/vHavokAiModule.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokRigidBody.hpp>
#include <Ai/Pathfinding/Character/LocalSteering/Obstacle/hkaiSimpleObstacleGenerator.h>
#include <Ai/Pathfinding/World/hkaiWorld.h>

V_IMPLEMENT_SERIAL( vHavokAiObstacle, IVObjectComponent, 0, &g_vHavokAiModule );

START_VAR_TABLE( vHavokAiObstacle, IVObjectComponent, "Havok AI Obstacle", VVARIABLELIST_FLAGS_NONE, "Havok AI Obstacle" )
	DEFINE_VAR_FLOAT( vHavokAiObstacle, ExpansionFactor, "Multiplier to increase/decrease size of avoidance sphere.", "1.0", 0, NULL );
	DEFINE_VAR_BOOL( vHavokAiObstacle, DebugEnabled, "If true, the obstacle's sphere and velocity will be displayed in \"playing\" modes.", "FALSE", 0, NULL );
END_VAR_TABLE

///////////////////////////////////////////////////////////////////////////////

vHavokAiObstacle::vHavokAiObstacle()
	: ExpansionFactor(1.f)
	, DebugEnabled(FALSE)
	, m_entity(HK_NULL)
	, m_generator(HK_NULL)
{
}

///////////////////////////////////////////////////////////////////////////////

vHavokAiObstacle::~vHavokAiObstacle()
{
}

///////////////////////////////////////////////////////////////////////////////

void vHavokAiObstacle::SetOwner( VisTypedEngineObject_cl *pOwner )
{
	IVObjectComponent::SetOwner( pOwner );

	m_entity = (VisBaseEntity_cl*)pOwner;

	if (m_entity)
	{
		// component has just been attached to the entity
		InitComponent();
	}
	else
	{
		// component has just been detached from the entity
		DeinitComponent();
	}
}

///////////////////////////////////////////////////////////////////////////////

BOOL vHavokAiObstacle::CanAttachToObject( VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut )
{
	if (!IVObjectComponent::CanAttachToObject(pObject, sErrorMsgOut))
	{
		return FALSE;
	}

	if (!pObject->IsOfType(V_RUNTIME_CLASS(VisBaseEntity_cl)))
	{
		sErrorMsgOut = "Component can only be added to instances of VisBaseEntity_cl or derived classes.";
		return FALSE;
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////

void vHavokAiObstacle::OnHandleCallback( IVisCallbackDataObject_cl* pData )
{
	if (pData->m_pSender == &Vision::Callbacks.OnUpdateSceneBegin)
	{
		VASSERT(m_entity);

		hkvBoundingSphere bs = m_entity->GetBoundingBox().getBoundingSphere();
		hkVector4 pos; vHavokConversionUtils::VisVecToPhysVecLocal(bs.m_vCenter, pos);
		hkaiAvoidanceSolver::SphereObstacle& ob = m_generator->m_spheres.back();
		ob.m_sphere.setPositionAndRadius(pos, bs.m_fRadius*ExpansionFactor);
		vHavokRigidBody* pComponent = (vHavokRigidBody*)m_entity->Components().GetComponentOfType( vHavokRigidBody::GetClassTypeId() );
		if (pComponent)
		{
			hkVector4 linVel; vHavokConversionUtils::VisVecToPhysVec_noscale(pComponent->GetLinearVelocity(), linVel);
			hkVector4 angVel; vHavokConversionUtils::VisVecToPhysVec_noscale(pComponent->GetAngularVelocity(), angVel);
			m_generator->updateSphereVelocities(linVel, angVel);
		}
		else
		{
			ob.m_velocity.setSub(pos, m_prevPos);
			hkReal dt = Vision::GetTimer()->GetTimeDifference();
			ob.m_velocity.mul(dt > 0.f ? 1.f/dt : 0.f);
		}
		m_prevPos = pos;
		m_generator->recalcCachedData();

		// debug visualization
		if (DebugEnabled)
		{
			const int res = 32;
			const hkReal scale = bs.m_fRadius * ExpansionFactor * vHavokConversionUtils::GetHavok2VisionScale();
			hkvVec3 prevPos;
			for(int i = 0; i <= res; i++)
			{
				hkReal angle = HK_REAL_PI*2.f * hkReal(i)/res;
				hkvVec3 pos = hkvVec3(hkMath::sin(angle), hkMath::cos(angle), 0.f) * scale;
				if (i > 0)
				{
					Vision::Game.DrawSingleLine(bs.m_vCenter+prevPos, bs.m_vCenter+pos, V_RGBA_YELLOW);
					Vision::Game.DrawSingleLine(bs.m_vCenter+hkvVec3(prevPos.x,0.f,prevPos.y), bs.m_vCenter+hkvVec3(pos.x,0.f,pos.y), V_RGBA_YELLOW);
					Vision::Game.DrawSingleLine(bs.m_vCenter+hkvVec3(0.f,prevPos.x,prevPos.y), bs.m_vCenter+hkvVec3(0.f,pos.x,pos.y), V_RGBA_YELLOW);
				}
				prevPos = pos;
			}
			hkvVec3 vel; vHavokConversionUtils::PhysVecToVisVec_noscale(ob.m_velocity, vel);		
			Vision::Game.DrawSingleLine(bs.m_vCenter, bs.m_vCenter+vel, V_RGBA_GREEN);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

void vHavokAiObstacle::Serialize( VArchive &ar )
{
	IVObjectComponent::Serialize( ar );

	char iLocalVersion = 1;
	if (ar.IsLoading())
	{
		ar >> iLocalVersion;
		ar >> ExpansionFactor;
	}
	else
	{
		ar << iLocalVersion;
		ar << ExpansionFactor;
	}
}

///////////////////////////////////////////////////////////////////////////////

VBool vHavokAiObstacle::WantsDeserializationCallback( const VSerializationContext &context )
{
	return context.m_eType != VSerializationContext::VSERIALIZATION_EDITOR;
}

///////////////////////////////////////////////////////////////////////////////

void vHavokAiObstacle::InitComponent()
{
	// register self for updates
	if (Vision::Editor.IsAnimatingOrPlaying())
	{
		Vision::Callbacks.OnUpdateSceneBegin += this;
	}

	m_generator = new hkaiSimpleObstacleGenerator;
	hkaiWorld* aiWorld = vHavokAiModule::GetInstance()->GetAiWorld();
	VASSERT(aiWorld);
	aiWorld->addObstacleGenerator(m_generator);

	hkaiAvoidanceSolver::SphereObstacle& ob = m_generator->m_spheres.expandOne();
	hkvBoundingSphere bs = m_entity->GetBoundingBox().getBoundingSphere();
	hkVector4 pos; vHavokConversionUtils::VisVecToPhysVecLocal(bs.m_vCenter, pos);
	m_prevPos = pos;
	ob.m_sphere.setPositionAndRadius(pos, bs.m_fRadius*ExpansionFactor);
	ob.m_velocity.setZero();
	m_generator->recalcCachedData();
}

///////////////////////////////////////////////////////////////////////////////

void vHavokAiObstacle::DeinitComponent()
{
	if (Vision::Editor.IsAnimatingOrPlaying())
	{
		Vision::Callbacks.OnUpdateSceneBegin -= this;
	}

	if (m_generator)
	{
		hkaiWorld* aiWorld = vHavokAiModule::GetInstance()->GetAiWorld();
		VASSERT(aiWorld);
		aiWorld->removeObstacleGenerator(m_generator);
		m_generator->removeReference();
		m_generator = HK_NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////

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
