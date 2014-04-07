/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef MERGEMODELSMENUSYSTEM_HPP_INCLUDED
#define MERGEMODELSMENUSYSTEM_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VMenuIncludes.hpp>
#include <Vision/Samples/Engine/ModelMerging/MergedModelFactory.hpp>

class MergeModelsMainMenu : public VDialog
{
public:
  virtual ~MergeModelsMainMenu();

  virtual void OnInitDialog() HKV_OVERRIDE;
  virtual void OnActivate() HKV_OVERRIDE;
  virtual void OnDeactivate() HKV_OVERRIDE;
  virtual void OnItemClicked(VMenuEventDataObject *pEvent) HKV_OVERRIDE;
  virtual void OnValueChanged(VItemValueChangedEvent *pEvent) HKV_OVERRIDE;
  virtual void OnTick(float dtime) HKV_OVERRIDE;

  void SetMergeModelFactory(MergedModelFactory_cl *factory);

protected:
  V_DECLARE_SERIAL_DLLEXP(MergeModelsMainMenu, DECLSPEC_DLLEXPORT)
  virtual void Serialize(VArchive &ar) HKV_OVERRIDE {}

  VCheckBox   *m_pArmArmorCB;
  VCheckBox   *m_pShoulderArmorCB;
  VCheckBox   *m_pLegsArmorCB;
  VCheckBox   *m_pKneeArmorCB;
  VCheckBox   *m_pAccessoireCB;
  VCheckBox   *m_pBeltCB;
  VCheckBox   *m_pClothCB;
  VCheckBox   *m_pBeardCB;
  VCheckBox   *m_pAxeCB;
  VCheckBox   *m_pSwordCB;

  // Merged model entity
  MergedModelFactory_cl *m_pMergedModelFactory;
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
