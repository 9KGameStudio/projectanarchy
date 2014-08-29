/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

hkaiNavVolumeMediator::QueryInputBase::QueryInputBase()
:	m_filterInfo(0),
	m_layers(HKAI_ALL_LAYERS),
	m_userData(0),
	m_hitFilter(HK_NULL),
	m_instance(HK_NULL),
	m_translation(HK_NULL)
{
}

void hkaiNavVolumeMediator::QueryInputBase::setInstance( const hkaiNavVolumeInstance* instance )
{
	m_instance = instance;
	m_translation = &instance->getTranslation();
}

//////////////////////////////////////////////////////////////////////////

hkaiNavVolumeMediator::GetClosestPointInput::GetClosestPointInput()
:	QueryInputBase(),
	m_queryRadius(hkSimdReal_5)
{
	m_position = hkVector4::getConstant<HK_QUADREAL_MAX>();
}

hkaiNavVolumeMediator::GetClosestPointInput::GetClosestPointInput( hkVector4Parameter position, hkSimdRealParameter radius )
:	QueryInputBase(),
	m_position(position),
	m_queryRadius(radius)
{
}

hkaiNavVolumeMediator::GetClosestPointInput::GetClosestPointInput( const QueryInputBase& base)
:	QueryInputBase(base),
	m_queryRadius(hkSimdReal_5)
{
	m_position = hkVector4::getConstant<HK_QUADREAL_MAX>();
}

//////////////////////////////////////////////////////////////////////////

inline hkaiNavVolumeMediator::AabbQueryInput::AabbQueryInput()	
{
	m_aabb.setEmpty();
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
