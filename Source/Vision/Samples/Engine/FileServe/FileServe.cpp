/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// ***********************************************************************************************
// FileServe sample application
// Copyright (C) Havok.com Inc. All rights reserved.
// ***********************************************************************************************
// Shows how to set up an application to consume data using a file serving client.
// ***********************************************************************************************

#include <Vision/Samples/Engine/FileServe/FileServePCH.h>
#include <Vision/Runtime/Framework/VisionApp/VAppImpl.hpp>
#include <Vision/Runtime/Framework/VisionApp/Modules/VDebugOptions.hpp>

#if defined(USE_FILESERVE)
  // Register fileserve startup module with priority 0
  #include <Vision/Runtime/Framework/VisionApp/Modules/VFileServe.hpp>
  REGISTER_STARTUP_MODULE(VFileServeStartupModule, 0);
#endif

class FileServeApp : public VAppImpl
{
public:
  FileServeApp() {}
  virtual ~FileServeApp() {}

  virtual void Init() HKV_OVERRIDE
  {
    LoadScene(VisAppLoadSettings("Scenes/FileServeSample.vscene", ":havok_sdk/Data/Vision/Samples/Engine/FileServe"));
  }

  virtual void AfterSceneLoaded(bool bLoadingSuccessful) HKV_OVERRIDE
  {
    VisBaseEntity_cl* pCamera = Vision::Game.CreateEntity("VFreeCamera", hkvVec3(-150.0f, -150.0f, 150.0f));
    pCamera->SetDirection(hkvVec3(1.5f, 1.0f, -0.7f));
  }
};

VAPP_IMPLEMENT_SAMPLE(FileServeApp);

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
