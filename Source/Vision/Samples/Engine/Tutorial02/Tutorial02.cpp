/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// ***********************************************************************************************
// Tutorial 02 : Entities
// Copyright (C) Havok.com Inc. All rights reserved.
// ***********************************************************************************************
// Shows how to create and rotate an entity using code
// ***********************************************************************************************
#include <Vision/Samples/Engine/Tutorial02/Tutorial02PCH.h>
#include <Vision/Runtime/Framework/VisionApp/VAppImpl.hpp>

class Tutorial02App : public VAppImpl
{
public:
  Tutorial02App() : m_pEntity(NULL) {}
  virtual ~Tutorial02App() {}

  virtual void Init() HKV_OVERRIDE
  {
    LoadScene(VisAppLoadSettings("ViewerMap.vscene", ":havok_sdk/Data/Vision/Samples/Engine/Maps/ViewerMap"));
  }

  virtual void AfterSceneLoaded(bool bLoadingSuccessful) HKV_OVERRIDE
  {
    Vision::Game.CreateEntity("VFreeCamera", hkvVec3::ZeroVector());

    // Create an entity of class VisBaseEntity_cl infront of the camera with a model
    m_pEntity = Vision::Game.CreateEntity("VisBaseEntity_cl", hkvVec3 (250.0f, 0.0f, -120.0f), "Models/Warrior/Warrior.MODEL");
    m_pEntity->SetCastShadows(true);
  }

  virtual bool Run() HKV_OVERRIDE
  {
    // Rotate the entity 20 degrees per second using this simulation frame's time delta
    float fTime = Vision::GetTimer()->GetTimeDifference();
    m_pEntity->IncOrientation(hkvVec3(20.0f * fTime, 0.0f, 0.0f));
    return true;
  }

private:
  VisBaseEntity_cl* m_pEntity;
};

VAPP_IMPLEMENT_SAMPLE(Tutorial02App);

/*
 * Havok SDK - Base file, BUILD(#20131218)
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
