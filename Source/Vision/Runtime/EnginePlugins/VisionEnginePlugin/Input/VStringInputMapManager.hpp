/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VSTRING_INPUT_MAP_MANAGER_HPP
#define VSTRING_INPUT_MAP_MANAGER_HPP

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>

class VStringInputMap;

/// \brief 
///   Manager for VStringInputMap. 
class VStringInputMapManager 
{
public:

  EFFECTS_IMPEXP void OneTimeInit();

  EFFECTS_IMPEXP void OneTimeDeInit();

  /// \brief 
  ///   Removes all instances.
  EFFECTS_IMPEXP void Release();

  /// \brief
  ///   Adds an instance to the manager.
  ///
  /// \param pInstance
  ///   Instance to add.
  ///
  /// \return 
  ///   Index of the instance.
  EFFECTS_IMPEXP unsigned int AddInstance(VStringInputMap *pInstance);  
  
  /// \brief
  ///   Removes an instance from the manager.
  ///
  /// \param iIndex
  ///   Index of the instance to remove.
  EFFECTS_IMPEXP void RemoveInstance(unsigned int iIndex);

  /// \brief
  ///   Removes an instance from the manager.
  ///
  /// \param pInstance
  ///   Pointer to the instance to remove.
  EFFECTS_IMPEXP void RemoveInstance(VStringInputMap *pInstance);

  /// \brief
  ///   Gets an instance from the manager.
  /// 
  /// \param iIndex
  ///   Index of the instance to retrieve.
  /// 
  /// \return 
  ///   Pointer to instance.
  EFFECTS_IMPEXP VStringInputMap* GetInstance(unsigned int iIndex) const;

  /// \brief
  ///   Returns a reference to manager.
  inline static VStringInputMapManager& GlobalManager()
  {
    return s_globalManager;
  }

protected:
  VStringInputMapManager();

  ~VStringInputMapManager();

  VStringInputMapManager(const VStringInputMapManager &rhs);

  void operator= (const VStringInputMapManager &rhs);

  VArray<VStringInputMap*> m_instances;

  static VStringInputMapManager s_globalManager;

};

#endif

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
