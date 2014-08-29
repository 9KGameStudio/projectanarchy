/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/VisionAssetsPCH.h>

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetManager.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetPluginManager.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvStateSync.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetBrowserQt/PropertyBrowser/hkvPropertyEditingSupport.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/Assets/hkvAnimationAsset.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/Assets/hkvCollectionAsset.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/Assets/hkvCollisionMeshAsset.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/Assets/hkvModelAsset.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/Assets/hkvParticleAsset.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/Assets/hkvPrefabAsset.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/Assets/hkvStaticMeshAsset.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/Assets/hkvTextureAsset.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/Assets/hkvAnimatedTextureAsset.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/Assets/hkvScriptAsset.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/Assets/hkvFbxAsset.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/Assets/hkvTagfileAsset.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/PropertyEditors/hkvVisionPropertyEditingSupport.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/TransformationRules/hkvCollectionTransformationRule.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/TransformationRules/hkvTextureTransformationRule.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/TransformationRules/hkvFbxTransformationRule.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/TransformationRules/hkvTagfileTransformationRule.hpp>

class hkvVisionAssetsPluginInterface : public hkvAssetPluginInterface
{
  virtual void shutDown() HKV_OVERRIDE;
  virtual hkvPropertyEditingProvider* getPropertyEditingProvider() HKV_OVERRIDE;
};


static bool s_bIsInitialized = false;
static hkvVisionAssetsPluginInterface* s_pluginInterface = NULL;
static hkvVisionPropertyEditingProvider* s_visionPropertyEditingProvider = NULL;

void OneTimeInit()
{
  if (s_bIsInitialized)
    return;

  hkvStateSync::notifyModuleLoaded();

  s_bIsInitialized = true;
}

extern "C" __declspec(dllexport) hkvAssetPluginInterface* InitAssetPlugin()
{
  OneTimeInit();

  hkvTextureAsset::StaticInit();
  hkvStaticMeshAsset::StaticInit();
  hkvModelAsset::StaticInit();
  hkvParticleAsset::StaticInit();
  hkvPrefabAsset::StaticInit();
  hkvAnimationAsset::StaticInit();
  hkvCollisionMeshAsset::StaticInit();
  hkvCollectionAsset::StaticInit();
  hkvAnimatedTextureAsset::StaticInit();
  hkvScriptAsset::StaticInit();
  hkvFbxAsset::StaticInit();
  hkvTagfileAsset::StaticInit();

  hkvTextureTransformationRule::staticInit();
  hkvCollectionTransformationRule::staticInit();
  hkvFbxTransformationRule::staticInit();
  hkvTagfileTransformationRule::staticInit();
  hkvFilterManagerTransformationRule::staticInit();

  if (s_visionPropertyEditingProvider == NULL)
  {
    s_visionPropertyEditingProvider = new hkvVisionPropertyEditingProvider();
  }
  if (s_pluginInterface == NULL)
  {
    s_pluginInterface = new hkvVisionAssetsPluginInterface();
  }
  return s_pluginInterface;
}

void hkvVisionAssetsPluginInterface::shutDown()
{
  hkvTextureAsset::StaticDeInit();
  hkvStaticMeshAsset::StaticDeInit();
  hkvModelAsset::StaticDeInit();
  hkvParticleAsset::StaticDeInit();
  hkvPrefabAsset::StaticDeInit();
  hkvAnimationAsset::StaticDeInit();
  hkvCollisionMeshAsset::StaticDeInit();
  hkvCollectionAsset::StaticDeInit();
  hkvAnimatedTextureAsset::StaticDeInit();  
  hkvScriptAsset::StaticDeInit();
  hkvFbxAsset::StaticDeInit();
  hkvTagfileAsset::StaticDeInit();

  hkvTextureTransformationRule::staticDeInit();
  hkvCollectionTransformationRule::staticDeInit();
  hkvFbxTransformationRule::staticDeInit();
  hkvTagfileTransformationRule::staticDeInit();
  hkvFilterManagerTransformationRule::staticDeInit();

  delete s_visionPropertyEditingProvider;
  s_visionPropertyEditingProvider = NULL;

  s_pluginInterface = NULL;
  delete this;

  hkvStateSync::notifyModuleUnloading();
}


hkvPropertyEditingProvider* hkvVisionAssetsPluginInterface::getPropertyEditingProvider()
{
  return s_visionPropertyEditingProvider;
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
