/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef IVHAVOKFORCESQUERIERCOMPONENT_HPP_INCLUDED
#define IVHAVOKFORCESQUERIERCOMPONENT_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>

/// \brief
///   Interface for components enabling them to be queried for forces.
class IvHavokForcesQuerierComponent
{
public:
  
  /// \brief
  ///   A pair of position and force direction/strength. See QueryForces
  struct Forces
  {
    hkvVec3 m_v3Point;
    hkvVec3 m_v3Dir;
  };

  /// \brief
  ///   Interface method that must return a set of forces.
  ///
  /// \param fET
  ///   Elapsed time since the last call to the function interface.
  ///
  /// \param iForcesCountOut
  ///   Amount of elements in the returned array.
  ///
  /// \return const IvHavokForcesQuerierComponent::Forces
  ///   Array containing the resulting forces.
  ///
  VHAVOK_IMPEXP virtual const IvHavokForcesQuerierComponent::Forces* QueryForces(float fET, int &iForcesCountOut) = 0;
  
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
