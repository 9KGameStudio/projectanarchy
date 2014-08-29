/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef RPG_PLUGIN_DUMMY_ENTITY_H__
#define RPG_PLUGIN_DUMMY_ENTITY_H__

#include <Vision/Samples/Engine/RPGPlugin/BaseEntity.h>

class RPG_DummyEntity : public RPG_BaseEntity
{
public:
	RPG_PLUGIN_IMPEXP RPG_DummyEntity();

private:
	V_DECLARE_SERIAL_DLLEXP(RPG_DummyEntity, RPG_PLUGIN_IMPEXP);
	V_DECLARE_VARTABLE(RPG_DummyEntity, RPG_PLUGIN_IMPEXP);
};

#endif

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
