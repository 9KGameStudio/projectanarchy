/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VAPPLYFORCESACTION_HPP_INCLUDED
#define VAPPLYFORCESACTION_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/IvHavokForcesQuerierComponent.hpp>
#include <Physics2012/Dynamics/Action/hkpUnaryAction.h>

//class hkpUnaryAction;

/// \brief
///   Class that queries the parent component for an array of forces to be applied to an entity.
///
class vApplyForcesAction : public hkpUnaryAction
{
public:

	/// \brief
  ///   Default Constructor
  VHAVOK_IMPEXP vApplyForcesAction();

  ///   Virtual destructor 
  VHAVOK_IMPEXP virtual ~vApplyForcesAction();

  //VHAVOK_IMPEXP void SetParent(IvHavokForcesQuerierComponent* pParent);
  VHAVOK_IMPEXP void SetEntity(vHavokRigidBody *pRB);

  // Pure virtual inherited methods
  VHAVOK_IMPEXP virtual void applyAction(const hkStepInfo& stepInfo) HKV_OVERRIDE;
  
	VHAVOK_IMPEXP virtual void entityRemovedCallback(hkpEntity* entity) HKV_OVERRIDE;
	VHAVOK_IMPEXP virtual hkpAction* clone( const hkArray<hkpEntity*>& newEntities, const hkArray<hkpPhantom*>& newPhantoms ) const HKV_OVERRIDE;

  IvHavokForcesQuerierComponent* m_pParent;

  bool m_bActionAdded;
  
};

#endif // IVHAVOKFORCESQUERIERCOMPONENT_HPP_INCLUDED

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
