/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

HK_FORCE_INLINE void hkbCharacter::setName( const char* name )
{
	m_name = name;
}

HK_FORCE_INLINE const hkbCharacterSetup* hkbCharacter::getSetup() const
{
	return m_setup;
}

HK_FORCE_INLINE hkbCharacterSetup* hkbCharacter::accessSetup() const
{
	return m_setup;
}

HK_FORCE_INLINE hkbRagdollDriver* hkbCharacter::getRagdollDriver() const
{
	return m_ragdollDriver;
}

HK_FORCE_INLINE hkbCharacterControllerDriver* hkbCharacter::getCharacterControllerDriver() const
{
	return m_characterControllerDriver;
}

HK_FORCE_INLINE hkbFootIkDriver* hkbCharacter::getFootIkDriver() const
{
	return m_footIkDriver;
}

HK_FORCE_INLINE hkbHandIkDriver* hkbCharacter::getHandIkDriver() const
{
	return m_handIkDriver;
}

HK_FORCE_INLINE hkbDockingDriver* hkbCharacter::getDockingDriver() const
{
	return m_dockingDriver;
}

HK_FORCE_INLINE void hkbCharacter::setAiControlDriver( hkbAiControlDriver* aiControlDriver )
{
	m_aiControlDriver = reinterpret_cast<hkReferencedObject*>(aiControlDriver);
}

HK_FORCE_INLINE hkbAiControlDriver* hkbCharacter::getAiControlDriver() const
{
	return reinterpret_cast<hkbAiControlDriver*>(m_aiControlDriver.val());
}

HK_FORCE_INLINE hkbCharacter::Capabilities hkbCharacter::getCapabilities() const
{
	// We always ensure that the capabilities we return reflect our internal state,
	// Not what we want it to be.  This means that it can be used in place of null checks.
	Capabilities currentCapabilities = m_effectiveCapabilities;
	clearUnsupportedBits( currentCapabilities, false );
	return currentCapabilities;
}

HK_FORCE_INLINE bool hkbCharacter::isCapableOf( Capabilities capabilities ) const
{
	return ( getCapabilities() & capabilities ) == capabilities;
}

HK_FORCE_INLINE const hkbAnimationBindingSet* hkbCharacter::getAnimationBindingSet() const
{
	if ( m_animationBindingSet != HK_NULL )
	{
		return m_animationBindingSet;
	}
	else if ( m_setup != HK_NULL )
	{
		return m_setup->m_animationBindingSet;
	}
	else
	{
		return HK_NULL;
	}
}

HK_FORCE_INLINE void hkbCharacter::setSetup( hkbCharacterSetup* setup )
{
	m_setup = setup;
	refreshOutput();
}

HK_FORCE_INLINE hkbBehaviorGraph* hkbCharacter::getBehavior() const
{
	return m_behaviorGraph;
}

HK_FORCE_INLINE const hkbProjectData* hkbCharacter::getProjectData() const
{
	return m_projectData;
}

HK_FORCE_INLINE void hkbCharacter::setProjectData(hkbProjectData* projectData)
{
	m_projectData = projectData;
}

HK_FORCE_INLINE hkReal hkbCharacter::getWorldFromModelFeedback() const
{
	if ( m_footIkDriver != HK_NULL )
	{
		return m_footIkDriver->getWorldFromModelFeedback();
	}

	return 0.0f;
}

HK_FORCE_INLINE void hkbCharacter::setLod( hkInt16 lodLevel, hkInt16 lodTrackLimit )
{
	m_currentLod = lodLevel;
	m_numTracksInLod = lodTrackLimit;
}

HK_FORCE_INLINE hkbEventQueue* hkbCharacter::getEventQueue() const
{
	return m_eventQueue;
}

HK_FORCE_INLINE hkbGeneratorOutput* hkbCharacter::getGeneratorOutput() const
{
	return m_generatorOutput;
}

HK_FORCE_INLINE void hkbCharacter::getOutputSetup( hkbGeneratorOutput::OutputSetup& setup ) const
{
	getOutputSetupInternal( setup );
	HK_ON_DEBUG( checkOutputConsistency( setup ); )
}

HK_FORCE_INLINE void hkbCharacter::setWorldFromModel( const hkQsTransform& worldFromModel )
{
	HK_ASSERT2( 0x22440320, m_generatorOutput != HK_NULL, "The character must be activated to set the worldFromModel");
	m_generatorOutput->accessWorldFromModel() = worldFromModel;	
}

HK_FORCE_INLINE const hkQsTransform& hkbCharacter::getWorldFromModel() const
{
	HK_ASSERT2( 0x22440321, m_generatorOutput != HK_NULL, "The character must be activated to get the worldFromModel");
	return m_generatorOutput->getWorldFromModel();
}

HK_FORCE_INLINE hkQsTransform& hkbCharacter::accessWorldFromModel() const
{
	HK_ASSERT2( 0x22440322, m_generatorOutput != HK_NULL, "The character must be activated to access the worldFromModel");
	return m_generatorOutput->accessWorldFromModel();
}

HK_FORCE_INLINE const hkQsTransform* hkbCharacter::getPoseLocal() const
{
	HK_ASSERT2( 0x22440323, m_generatorOutput != HK_NULL, "The character must be activated to get the local pose");
	return m_generatorOutput->getPoseLocal();
}

HK_FORCE_INLINE hkQsTransform* hkbCharacter::accessPoseLocal() const
{
	HK_ASSERT2( 0x22440324, m_generatorOutput != HK_NULL, "The character must be activated to access the local pose");
	return m_generatorOutput->accessPoseLocal();
}

HK_FORCE_INLINE int hkbCharacter::getNumPoseLocal() const
{
	HK_ASSERT2( 0x22440325, m_generatorOutput != HK_NULL, "The character must be activated to get info about the local pose");
	// Character pose should always be full no matter what comparable output is made (getOutputSetup).
	// This is because we always force a dense pose for the character output.
	HK_ASSERT( 0x22440235, m_generatorOutput->getNumPoseLocal() == m_setup->m_animationSkeleton->m_bones.getSize() );
	return m_generatorOutput->getNumPoseLocal();
}

HK_FORCE_INLINE hkbRagdollInterface* hkbCharacter::getRagdollInterface() const
{
	HK_ASSERT2( 0x22440232, ( m_ragdollDriver == HK_NULL ) || ( m_ragdollDriver->getRagdollInterface() == m_ragdollInterface ), "Inconsistent ragdolls." );
	return m_ragdollInterface;
}

HK_FORCE_INLINE hkbWorld* hkbCharacter::getWorld() const
{
	return m_world;
}

HK_FORCE_INLINE const hkArray<hkaBoneAttachment*>& hkbCharacter::getBoneAttachments() const
{
	return m_boneAttachments;
}

HK_FORCE_INLINE hkbProjectAssetManager* hkbCharacter::getAssetManager() const
{
	return m_assetManager;
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
