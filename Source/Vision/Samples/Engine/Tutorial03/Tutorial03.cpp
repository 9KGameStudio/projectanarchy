/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// ***********************************************************************************************
// Tutorial 03 : Dynamic light
// Copyright (C) Havok.com Inc. All rights reserved.
// ***********************************************************************************************
// How to create a dynamic light with code
// ***********************************************************************************************
#include <Vision/Samples/Engine/Tutorial03/Tutorial03PCH.h>
#include <Vision/Runtime/Framework/VisionApp/VAppImpl.hpp>

class Tutorial03App : public VAppImpl
{
public:
  Tutorial03App() : m_pEntity(NULL) {}
  virtual ~Tutorial03App() {}

  virtual void Init() HKV_OVERRIDE
  {
    LoadScene(VisAppLoadSettings("ViewerMap.vscene", ":havok_sdk/Data/Vision/Samples/Engine/Maps/ViewerMap"));

    // We need a forward renderer node to cast shadows (will do nothing on platforms that don't support renderer nodes)
    VAppHelper::CreateForwardRenderer();
  }

  virtual void AfterSceneLoaded(bool bLoadingSuccessful) HKV_OVERRIDE
  {
    Vision::Game.CreateEntity("VFreeCamera", hkvVec3::ZeroVector());

    // Create an entity of class VisBaseEntity_cl infront of the camera with a model
    m_pEntity = Vision::Game.CreateEntity("VisBaseEntity_cl", hkvVec3(250.0f, 0.0f, -120.0f), "Models/Warrior/Warrior.MODEL");
    m_pEntity->SetCastShadows(true);

    // Create a point light
    VisLightSource_cl *pLight = Vision::Game.CreateLight(hkvVec3(200.0f, 50.0f, 0.0f), VIS_LIGHT_POINT, 300.0f);
    VAppHelper::SetShadowsForLight(pLight, true);

    pLight->SetMultiplier(2.0f);
    pLight->SetColor(VColorRef(255, 0, 0));
  }

  virtual bool Run() HKV_OVERRIDE
  {
    // Rotate the entity 20 degrees per second using this simulation frame's time delta
    float fTime = Vision::GetTimer()->GetTimeDifference();
    m_pEntity->IncOrientation( hkvVec3 (20.0f * fTime, 0.0f, 0.0f) );
    return true;
  }

private:
  VisBaseEntity_cl* m_pEntity;
};

VAPP_IMPLEMENT_SAMPLE(Tutorial03App);

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
