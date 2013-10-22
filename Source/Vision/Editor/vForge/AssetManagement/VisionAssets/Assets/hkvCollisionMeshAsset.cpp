/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/VisionAssetsPCH.h>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/Assets/hkvCollisionMeshAsset.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Assets/hkvAssetTypeManager.hpp>
#include <Vision/Runtime/Engine/System/Vision.hpp>
#include <Vision/Runtime/Engine/System/Resource/VisApiResource.hpp>

unsigned int hkvCollisionMeshAsset::s_iAssetTypeIndex = HKV_INVALID_INDEX;

/////////////////////////////////////////////////////////////////////////////
// hkvCollisionMeshAsset static functions
/////////////////////////////////////////////////////////////////////////////

void hkvCollisionMeshAsset::StaticInit()
{
  hkvAssetTypeInfo ti;
  ti.m_name = "CollisionMesh";
  ti.m_createFunc = &CreateAsset;
  ti.m_supportedFileExtensions.pushBack("vcolmesh");
  ti.m_szTypeIconQt = ":/Icons/Icons/CollisionMeshAsset.png";

  ti.m_resourceManagerName = VIS_RESOURCEMANAGER_COLLISIONMESHES;

  // register at the hkvAssetTypeManager and store the asset type index in static variable.
  s_iAssetTypeIndex = hkvAssetTypeManager::getGlobalInstance()->addAssetType(ti);
}


void hkvCollisionMeshAsset::StaticDeInit()
{
  // de-register at the hkvAssetTypeManager
  hkvAssetTypeManager::getGlobalInstance()->removeAssetType(s_iAssetTypeIndex);
  s_iAssetTypeIndex = HKV_INVALID_INDEX;
}


hkvAsset* hkvCollisionMeshAsset::CreateAsset()
{
  hkvAsset* pAsset = new hkvCollisionMeshAsset;
  VASSERT(pAsset)

  return pAsset;
}



/////////////////////////////////////////////////////////////////////////////
// hkvCollisionMeshAsset public functions
/////////////////////////////////////////////////////////////////////////////

hkvCollisionMeshAsset::hkvCollisionMeshAsset()
{

}


hkvCollisionMeshAsset::~hkvCollisionMeshAsset()
{

}


/////////////////////////////////////////////////////////////////////////////
// hkvCollisionMeshAsset public override functions
/////////////////////////////////////////////////////////////////////////////

unsigned int hkvCollisionMeshAsset::getTypeIndex() const
{
  return s_iAssetTypeIndex;
}


const char* hkvCollisionMeshAsset::getTypeName() const
{
  return "CollisionMesh";
}

// testing
void hkvCollisionMeshAsset::getSpecificProperties(hkvPropertyList& properties, hkvProperty::Purpose purpose) const
{
  // TODO
}

void hkvCollisionMeshAsset::setSpecificProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, unsigned int iStackIndex, hkvProperty::Purpose purpose)
{
  // TODO
}

/*
 * Havok SDK - Base file, BUILD(#20131019)
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
