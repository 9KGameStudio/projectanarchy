/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Samples/Engine/Tutorial01/Tutorial01PCH.h>
#include <Vision/Runtime/Framework/VisionApp/VAppImpl.hpp>

class Tutorial01App : public VAppImpl
{
public:
  Tutorial01App() {}
  virtual ~Tutorial01App() {}

  virtual void SetupAppConfig(VisAppConfig_cl& config) HKV_OVERRIDE
  {
    // Fullscreen mode with current desktop resolution
    /*DEVMODEA deviceMode;
    deviceMode = Vision::Video.GetAdapterMode(config.m_videoConfig.m_iAdapter);
    config.m_videoConfig.m_iXRes = deviceMode.dmPelsWidth;
    config.m_videoConfig.m_iYRes = deviceMode.dmPelsHeight;
    config.m_videoConfig.m_bFullScreen = true;*/
  }

  virtual void Init() HKV_OVERRIDE
  {
    LoadScene(VisAppLoadSettings("ViewerMap.vscene", ":havok_sdk/Data/Vision/Samples/Engine/Maps/ViewerMap"));
  }

  virtual void AfterSceneLoaded(bool bLoadingSuccessful) HKV_OVERRIDE
  {
    Vision::Game.CreateEntity("VFreeCamera", hkvVec3::ZeroVector());
  }
};

VAPP_IMPLEMENT_SAMPLE(Tutorial01App);

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
