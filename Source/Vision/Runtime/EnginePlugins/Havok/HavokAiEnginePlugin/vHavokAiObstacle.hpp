/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef __VHAVOK_AI_OBSTACLE_HPP
#define __VHAVOK_AI_OBSTACLE_HPP

#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/vHavokAiIncludes.hpp>

class hkaiSimpleObstacleGenerator;

/// This component will cause the entity it's attached to to be treated as a spherical avoidance obstacle for local steering.
class vHavokAiObstacle : public IVObjectComponent, public IVisCallbackHandler_cl
{
	public:

		V_DECLARE_SERIAL_DLLEXP( vHavokAiObstacle, VHAVOKAI_IMPEXP );
		V_DECLARE_VARTABLE( vHavokAiObstacle, VHAVOKAI_IMPEXP )

		VHAVOKAI_IMPEXP vHavokAiObstacle();
		VHAVOKAI_IMPEXP virtual ~vHavokAiObstacle();

			/// 
			/// IVObjectComponent implementation
			/// 
		VOVERRIDE void SetOwner( VisTypedEngineObject_cl *pOwner );
		VOVERRIDE BOOL CanAttachToObject( VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut );

			/// 
			/// IVisCallbackHandler_cl implementation
			///
		VHAVOKAI_IMPEXP VOVERRIDE void OnHandleCallback( IVisCallbackDataObject_cl* pData );

		VHAVOKAI_IMPEXP VOVERRIDE void Serialize( VArchive &ar );
		VHAVOKAI_IMPEXP VOVERRIDE VBool WantsDeserializationCallback( const VSerializationContext &context );

	protected:

		void InitComponent();
		void DeinitComponent();

		// Exposed to vForge:
		float ExpansionFactor;						// Multiplier to increase/decrease size of avoidance sphere
		BOOL DebugEnabled;							// Whether or not the obstacle's sphere and velocity should be visualized

		VisBaseEntity_cl* m_entity;					// Owning entity
		hkaiSimpleObstacleGenerator* m_generator;	// Havok AI avoidance obstacle
		hkVector4 m_prevPos;						// Entity position on previous frame that is used to estimate velocity
};

#endif	// __VHAVOK_AI_OBSTACLE_HPP

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
