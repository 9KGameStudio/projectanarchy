/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/BaseComponent.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionSerializer.h>

V_IMPLEMENT_SERIAL(RPG_BaseComponent, IVObjectComponent, 0, &g_RPGPluginModule);

START_VAR_TABLE(RPG_BaseComponent, IVObjectComponent, "RPG Base Component", VFORGE_HIDECLASS, "")
END_VAR_TABLE

RPG_BaseComponent::RPG_BaseComponent(int id, int componentFlags)
  : IVObjectComponent(id, componentFlags)
{
}

void RPG_BaseComponent::Serialize(VArchive& ar)
{
  IVObjectComponent::Serialize(ar);

  if(ar.IsLoading())
  {
    RPG_VisionSerializer::ReadVariableList(this, ar);
  }
  else
  {
    RPG_VisionSerializer::WriteVariableList(this, ar);
  }
}

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
