/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// ***********************************************************************************************
// Tutorial 04 : Custom Entities
// Copyright (C) Havok.com Inc. All rights reserved.
// ***********************************************************************************************
// How to create a custom entity class
// ***********************************************************************************************
#include <Vision/Samples/Engine/Tutorial04/Tutorial04PCH.h>
#include <Vision/Runtime/Framework/VisionApp/VAppImpl.hpp>

#define MY_VERSION_MAJOR 0x01
#define MY_VERSION_MINOR 0x00

// Module to register our classes with
DECLARE_THIS_MODULE( g_MyModule, MAKE_VERSION(MY_VERSION_MAJOR, MY_VERSION_MINOR), "Engine Sample", "Havok", "Vision Engine Sample", NULL );

class Tutorial04App : public VAppImpl
{
public:
  Tutorial04App() : m_pEntity(NULL) {}
  virtual ~Tutorial04App() {}

  virtual void Init() HKV_OVERRIDE
  {
    // Register the module so the engine knows about the classes
    Vision::RegisterModule(&g_MyModule);

    LoadScene(VisAppLoadSettings("ViewerMap.vscene", ":havok_sdk/Data/Vision/Samples/Engine/Maps/ViewerMap"));
  }

  virtual void AfterSceneLoaded(bool bLoadingSuccessful) HKV_OVERRIDE
  {
    Vision::Game.CreateEntity("VFreeCamera", hkvVec3::ZeroVector());

    // Create entities of type MyEntity_cl in front of the camera
    for (int i=-1; i<=1; ++i)
    {
      MyEntity_cl * pEntity = (MyEntity_cl*) Vision::Game.CreateEntity("MyEntity_cl", hkvVec3 (250.0f, (float)i * 150.0f , -120.0f), "Models/Warrior/Warrior.MODEL");
      VASSERT(pEntity);
      pEntity->RotateSpeed = 20.f + (float)i * 10.0f; // each entity has a different speed
      pEntity->SetCastShadows(true);
    }
  }

private:
  VisBaseEntity_cl* m_pEntity;
};

VAPP_IMPLEMENT_SAMPLE(Tutorial04App);

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
