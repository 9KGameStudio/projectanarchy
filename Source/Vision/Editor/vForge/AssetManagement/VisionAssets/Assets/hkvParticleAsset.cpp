/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/VisionAssetsPCH.h>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/Assets/hkvParticleAsset.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetManager.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Assets/hkvAssetTypeManager.hpp>

hkvAssetTypeInfo* hkvParticleAsset::s_typeInfo = NULL;
const hkvAssetTypeInfoHandle* hkvParticleAsset::s_typeInfoHandle = NULL;

/////////////////////////////////////////////////////////////////////////////
// hkvParticleAssetType static functions
/////////////////////////////////////////////////////////////////////////////

void hkvParticleAsset::StaticInit()
{  
  s_typeInfo = new hkvAssetTypeInfo();
  s_typeInfo->m_name = "Particle";
  s_typeInfo->m_createFunc = &CreateAsset;
  s_typeInfo->m_supportedFileExtensions.pushBack("vpfx");
  s_typeInfo->m_szTypeIconQt = ":/Icons/Icons/ParticleEffectAsset.png";

  s_typeInfo->m_resourceManagerName = "Particles";
  s_typeInfo->m_useEngineForDependencies = true;
  s_typeInfo->m_useEngineForThumbnails = true;
  s_typeInfo->m_useEngineForPropertyHint = false;

  // register at the hkvAssetTypeManager and store the asset type handle in static variable.
  s_typeInfoHandle = hkvAssetTypeManager::getGlobalInstance()->addAssetType(*s_typeInfo);  
}


void hkvParticleAsset::StaticDeInit()
{ 
  // de-register at the hkvAssetTypeManager
  hkvAssetTypeManager::getGlobalInstance()->removeAssetType(*s_typeInfoHandle);
  s_typeInfoHandle = NULL;

  delete s_typeInfo;
  s_typeInfo = NULL;  
}


hkvAsset* hkvParticleAsset::CreateAsset()
{
  hkvAsset* pAsset = new hkvParticleAsset;
  return pAsset;
}



/////////////////////////////////////////////////////////////////////////////
// hkvParticleAssetType public functions
/////////////////////////////////////////////////////////////////////////////

hkvParticleAsset::hkvParticleAsset() : hkvAsset(s_typeInfo)
{

}


hkvParticleAsset::~hkvParticleAsset()
{

}


/////////////////////////////////////////////////////////////////////////////
// hkvParticleAssetType public override functions
/////////////////////////////////////////////////////////////////////////////

const hkvAssetTypeInfoHandle& hkvParticleAsset::getTypeInfoHandle() const
{
  return *s_typeInfoHandle;
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
