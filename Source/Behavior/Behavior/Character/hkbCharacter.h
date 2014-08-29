/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_CHARACTER_H
#define HKB_CHARACTER_H

#include <Behavior/Behavior/Character/hkbCharacterSetup.h>
#include <Behavior/Behavior/Driver/CharacterController/hkbCharacterControllerDriver.h>
#include <Behavior/Behavior/Driver/FootIk/hkbFootIkDriver.h>
#include <Behavior/Behavior/Driver/HandIk/hkbHandIkDriver.h>
#include <Behavior/Behavior/Driver/Ragdoll/hkbRagdollDriver.h>
#include <Behavior/Behavior/Driver/Docking/hkbDockingDriver.h>
#include <Behavior/Behavior/Generator/hkbGeneratorOutput.h>
#include <Behavior/Behavior/Project/hkbProjectData.h>
#include <Behavior/Utilities/Physics/hkbPhysicsBaseTypes.h>

#include <Common/Base/DebugUtil/DeterminismUtil/hkCheckDeterminismUtil.h>

extern const class hkClass hkbCharacterClass;

class hkaBoneAttachment;
class hkbAnimationBindingSet;
class hkbBehaviorGraph;
class hkbBehaviorGraphInternalState;
class hkbCharacterSetup;
class hkbEventQueue;
class hkbWorld;
class hkbAiControlDriver;
class hkbProjectAssetManager;

/// A collection of information associated with an instance of a character.
class hkbCharacter : public hkReferencedObject
{
	//+version(4)
	public:

			
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );

			/// See CapabilityTypes enum
		typedef int Capabilities;
		
			/// Setup the character to have the standard capabilities (see setCapabilities).
			/// Output will be allocated for standard capabilities when the character is activated
			/// OR when allocateOutput() is called.
			
		hkbCharacter();

			/// Setup the character to have the capabilities provided (see setCapabilities).
			/// Output will be allocated based on the provided capabilities when the character is activated
			/// OR when allocateOutput() is called.
		hkbCharacter( Capabilities capabilities );

			// Dtor
		~hkbCharacter();
		
			/// Gets the name of the character
		hkStringPtr getName() const;
			/// Sets the name of the character.
		void setName( const char* name );

		//////////////////////////////////////////////////////////////////////////
		// Context
		//////////////////////////////////////////////////////////////////////////
		
			/// Sets the spatial query interface for this character.
			/// If this is not set, the hkbPhysicsInterface from the hkbWorld will be used.
			/// There are often more efficient methods of doing spatial queries if the source character is known.
		void setSpatialQueryInterface( hkbSpatialQueryInterface* spatialQueryInterface );
			/// Gets the spatial query interface associated with this character.
		hkbSpatialQueryInterface* getSpatialQueryInterface() const;
		
			/// Sets which world the character is in. Do not call, this is set by the hkbWorld.
		void setWorld( hkbWorld* world );
			/// Gets the world the character is in, HK_NULL if the character is not in a hkbWorld.
		hkbWorld* getWorld() const;

			/// Returns all of the rigid bodies in the world that are within maxDistance of sensorPosWS.  The default
			/// implementation uses the character's spatial query interface, falling back on the provided physicsInterface.
		virtual void getNearbyRigidBodies(
			const hkVector4& sensorPosWS,
			hkReal maxDistance,
			hkbPhysicsInterface* physicsInterface,
			hkArray<hkbRigidBodyHandle>& rigidBodies,
			hkUint32 collisionFilterInfo,
			bool excludeThisCharacter,
			bool inCharactersOnly );

			/// Returns all of the characters that are within maxDistance of this character.  The
			/// distance is between the world-from-model positions in the world.
		virtual void getNearbyCharacters( hkReal maxDistance, hkArray<hkbCharacter*>& characters );

		//////////////////////////////////////////////////////////////////////////
		// Graph related
		//////////////////////////////////////////////////////////////////////////
		
			/// Set the behavior graph of the character.
			/// This is for advanced use only.  The character's animation binding set should be setup for any new behavior.
		void setBehavior( hkbBehaviorGraph* behavior );
			/// Gets the behavior graph associated with the character.
		hkbBehaviorGraph* getBehavior() const;

			/// Activate the character and its behavior graph.
			/// This will also allocate the generator output for the character based on character's capabilities (see setCapabilities()).
			/// If you wish to allocate the output ahead of activation, use allocateOutput().
		void activate();
			/// Activate the character given an internal state that you would like the behavior graph to be in.
			/// This will also allocate the generator output for the character based on character's capabilities (see setCapabilities()).
		void activateWithInternalState( const hkbBehaviorGraphInternalState& internalState );
			/// Deactivate the character and its behavior graph.
			/// Optionally cleanup character's generator output.
		void deactivate( bool cleanupOutput = false );
			/// Whether activate() has been called.
		bool isActive() const;

			/// Process an event right now.  It's better to put them into the character's event
			/// queue but if you want it done immediately call this.
		void handleEvent( const hkbEvent e );
			/// Get the event queue that stores events to be processed later by the character.
		hkbEventQueue* getEventQueue() const;
		
			/// Set the character setup containing the shared character assets. This will reallocate the
			/// the generator output if it's already been allocated based on character's capabilities (see setCapabilities()).
		void setSetup( hkbCharacterSetup* characterSetup );
			/// Get the character setup containing the shared character assets.
		const hkbCharacterSetup* getSetup() const;
			/// Get writable access to the character setup.  This should only be called at load and link
			/// time since the setup may be shared by many characters.
		hkbCharacterSetup* accessSetup() const;

		//////////////////////////////////////////////////////////////////////////
		// Output related
		//////////////////////////////////////////////////////////////////////////

			/// Get information for setting up a compatible generator output for the character.
			/// The general flow is getOutputSetup -> create output -> use the output -> copy to character output via getGeneratorOutput().
		void getOutputSetup( hkbGeneratorOutput::OutputSetup& setup ) const;
			/// Force the character's output to be allocated before activation.
			/// If the character is being setup, allocating the output early will likely cause unnecessary output reallocations as data is becoming available to the character.
			/// If you do need the character's output before it's been activated, it's recommended that this is called at the very end of the setup process.
		void allocateOutput();
			/// Get the hkbGeneratorOutput which stores the output of the character's behavior graph.  The
			/// generator output is only valid when the character is active (see isActive(), allocateOutput()).
		hkbGeneratorOutput* getGeneratorOutput() const;

			/// Set the world-from-model transform.
		void setWorldFromModel( const hkQsTransform& worldFromModel );
			/// Get the world-from-model transform.
		const hkQsTransform& getWorldFromModel() const;
			/// Get the world-from-model transform.
		hkQsTransform& accessWorldFromModel() const;
			/// Get the local pose of the character as read only.
		const hkQsTransform* getPoseLocal() const;
			/// Get the local pose of the character.
		hkQsTransform* accessPoseLocal() const;
			/// Get the number of bones in the current pose.
		int getNumPoseLocal() const;

		//////////////////////////////////////////////////////////////////////////
		// Drivers
		//////////////////////////////////////////////////////////////////////////

			/// Get the driver for the character ragdoll.
		hkbRagdollDriver* getRagdollDriver() const;
			/// Get the driver for the character controller.
		hkbCharacterControllerDriver* getCharacterControllerDriver() const;
			/// Get the driver for the footIk.
		hkbFootIkDriver* getFootIkDriver() const;
			/// Get the driver for the handIk.
		hkbHandIkDriver* getHandIkDriver() const;
			/// Get the docking driver.
		hkbDockingDriver* getDockingDriver() const;
			/// Get the AI control driver
		hkbAiControlDriver* getAiControlDriver() const;
			/// Set the AI control driver
		void setAiControlDriver(hkbAiControlDriver* aiControlDriver);

			/// Get the world from model feedback from the footIk. 
		hkReal getWorldFromModelFeedback() const;

		//////////////////////////////////////////////////////////////////////////
		// Capabilities
		//////////////////////////////////////////////////////////////////////////

			/// An enum for describing the capabilities of the character
			
		enum CapabilityTypes
		{
				// Basic features.
			FULL_POSE = 1<<0, // without this set, only the root bone will be simulated
			MIRRORING = 1<<1,
			DOCKING = 1<<2,

				// Physics-optional features.
				// These capabilities may require physics support depending on how they are used.
			HAND_IK = 1<<3,

				// Physics-required features.
				// These capabilities will require physics support.
			CHARACTER_CONTROL = 1<<4,
			RAGDOLL = 1<<5,
			FOOT_IK = 1<<6,

				// Ai-required features.
				// These capabilities will require ai support.
			AI_CONTROL = 1<<7,

				/// Basic feature set
			BASIC_CAPABILITIES = (FULL_POSE | MIRRORING | DOCKING),

				/// Physics feature set
			PHYSICS_CAPABILITIES = (HAND_IK | CHARACTER_CONTROL | RAGDOLL | FOOT_IK),

				/// Standard feature set
			STANDARD_CAPABILITIES = (BASIC_CAPABILITIES | PHYSICS_CAPABILITIES),
		};

			/// Return the current capabilities of the character. This computes the current capabilities of the character taking into account underlying data.
		Capabilities getCapabilities() const;
			/// Set the characters desired capabilities. This will determine what the character should be able to do.
			/// Missing data can curtail the character's capabilities further (see getCapabilities).
			/// It will delete/create drivers as needed. It will also reallocate the the generator output
			/// if it's already been allocated.
		void setCapabilities( Capabilities capabilities );
			/// This will optimize the desired capabilities of the character considering availability of underlying data.
			/// It cleans up unnecessary memory allocated for the output and drivers.
			/// This isn't done automatically as some systems may depend on some drivers and output data being available, even if unused.
			/// Additionally, if changes are made to the underlying data, you must call this again to reoptimize the capabilities. It
			/// will also reallocate the the generator output if it's already been allocated.
		void optimizeCapabilities();
			/// Return whether the character is currently capable of the specified capabilities. This takes into account availability of underlying data.
			/// This function uses getCapabilities().
		bool isCapableOf( Capabilities capabilities ) const;

			/// lodLevel is an index into hkbCharacterData::m_numBonesPerLod, which is exported by HAT.  It indicates how many bones 
			/// should be used for the current level-of-detail.  If this is -1 (the default) then all bones are used.
			///
			/// lodTrackLimit provides a bound on the number of animation tracks that are extracted by the hkbClipGenerator.
			/// You can use this member along with m_currentLod (or independently) to gain more control over which
			/// bones are processed for the current LOD.  If this is -1 (the default) then all tracks that are required
			/// for the current hkbCharacterData::m_numBonesPerLod[m_currentLod] are extracted.
		void setLod( hkInt16 lodLevel, hkInt16 lodTrackLimit = -1 );

		//////////////////////////////////////////////////////////////////////////
		// Asset management functions
		//////////////////////////////////////////////////////////////////////////
		
			/// Sets the asset loader that created this character
		void setAssetManager( hkbProjectAssetManager* assetManager );
			/// Gets the asset loader that created this character
		hkbProjectAssetManager* getAssetManager() const;
		
			/// Sets the project the character was loaded from
		void setProjectData( hkbProjectData* project );
			/// Gets the project associated with the character.
		const hkbProjectData* getProjectData() const;

			/// Get the animation bindings for this character.  This returns m_animationBindingSet if it is not null,
			/// and otherwise returns m_setup->m_animationBindingSet.
		const hkbAnimationBindingSet* getAnimationBindingSet() const;
			/// Access to the writable animation binding set.  You should only use this at load and link time
			/// since the animation binding set may be shared.
			/// This is for advanced use only.
		hkbAnimationBindingSet* accessAnimationBindingSet();
			/// Sets the animation binding set for this character instance.  You should only call this
			/// if your individual characters have different bindings sets (normally each character type
			/// has its own animation binding set stored in m_setup).
			/// This is for advanced use only.
		void setAnimationBindingSet( hkbAnimationBindingSet* animationBindingSet );

			/// Set the handle for the character's ragdoll.  This will update the hkbRagdollDriver and reallocate the
			/// the generator output if it's already been allocated based on character's capabilities (see setCapabilities()).
		void setRagdollInterface(hkbRagdollInterface* ragdoll);
			/// Returns the ragdoll handle from the hkbRagdollDriver.
		hkbRagdollInterface* getRagdollInterface() const;

			/// Returns a read only array of bone attachments on the character
		const hkArray<hkaBoneAttachment*>& getBoneAttachments() const;

		//////////////////////////////////////////////////////////////////////////
		// Script related functions
		//////////////////////////////////////////////////////////////////////////
		
			/// Gets the lua state used by the character.
		void* getLuaState() const;
			/// Sets the lua state used by the character.
		void setLuaState( void* luaState );
			/// Clears lua state of the character.
		void clearLuaState();
			/// Creates and initializes the lua state for the character.
			/// Uses all scripts defined in the character's string data (m_luaFiles) and calls dofile to register them.
		void initializeLua();
			/// If lua scripts have changed, calling this will refresh the lua state.
			/// This recreates the state, so any data in the state will be cleared.
		void rebuildLuaScripts();
			/// Connect the character to the Havok Script debugger. Port is defined by the world the character is in.
			/// Only one character can be connected at a time.
		bool connectLuaDebugger();
			/// Disconnects the character from the Havok Script debugger. Only one character can be connected at a time.
		void disconnectLuaDebugger();
			/// Returns whether the lua debugger is currently connected.
		bool isLuaDebuggerConnected() const;	
			/// Gets the filename from the function name.
		hkStringPtr getFilenameFromFunction( const hkStringPtr& functionName ) const;
			/// Gets the line number from the function name.
		int getLineNumberFromFunction( const hkStringPtr& functionName ) const;

		//////////////////////////////////////////////////////////////////////////
		// Deprecated Physics Implementation Specific Functions
		// Must link against hkbPhysics2012Bridge
		//////////////////////////////////////////////////////////////////////////

			/// Returns all of the rigid bodies in the world that are within maxDistance of sensorPosWS.  The default
			/// implementation calls hkpWorld::getClosestPoints() with a hkpSphereShape as input.  You may want to
			/// subclass hkbCharacter and implement this method in a way that is more efficient in your game.
			/// For example, you may want to use an hkpPhantom that follows the character around to collect
			/// nearby rigid bodies.  You may also want to assume that sensorPosWS is always near the character.
			/// But whether that assumption holds depends on how you configure your behavior graphs.
		void getNearbyRigidBodies(
			const hkVector4& sensorPosWS,
			hkReal maxDistance,
			hkbDeprecatedPhysicsWorldPtr world,
			hkArray<hkbRigidBodyHandle>& rigidBodies,
			hkUint32 collisionFilterInfo,
			bool excludeThisCharacter,
			bool inCharactersOnly );

			/// Set the handle for the character from a ragdoll pointer.
			/// This accessor will be deprecated in the future (see setRagdollInterface).
		void setRagdoll( hkbDeprecatedRagdollPtr ragdoll );
			/// Get the handle for the character as a ragdoll pointer.
			/// This accessor will be deprecated in the future (see getRagdollInterface).
		hkbDeprecatedRagdollPtr getRagdoll() const;

	public:

			/// The characters near this one.
		hkArray<hkbCharacter*> m_nearbyCharacters;

			/// For any use you want. Not used by the Havok system.
		hkUlong	m_userData;

	private:

			/// Initializes an array of script files
		void initScriptFiles();

			/// Clear the unsupported bits from the provided capabilities.
			/// Either ensure the driver support matches the capabilities or determine capabilities from driver support.
		void clearUnsupportedBits( Capabilities& capabilities, bool ensureDriverSupport ) const;

			/// Setup character drivers based on provided capabilities.
		void ensureDriversSupportCapabilities( Capabilities capabilities ) const;

			/// Get the output setup
		void getOutputSetupInternal( hkbGeneratorOutput::OutputSetup& setup ) const;

			/// Check setup consistency with internal output
		HK_ON_DEBUG( void checkOutputConsistency( hkbGeneratorOutput::OutputSetup& setup ) const; )

			/// Refresh the current output.  If the output was allocated it will be reallocated with the latest settings.
			/// Otherwise, this is a no-op.
		void refreshOutput();

			/// Filters out rigid body results from the Physics system based on bool parameters
		void filterRigidBodyResults(
			hkbPhysicsInterface* physicsInterface,
			hkArray<hkbRigidBodyHandle>& rbsInOut,
			bool excludeThisCharacter,
			bool inCharactersOnly );

			/// Using the capabilities, compute needed output tracks
		void computeOutputTracks( hkbGeneratorOutput::OutputSetup& setup ) const;

			/// Allocates and initializes the character's generator output.  If a hkbGeneratorOutput already exists
			/// it it deallocated.  However before deallocation, it's world from model and pose local tracks are
			/// stored and then copied to the new hkbGeneratorOutput.
		void allocateOutputInternal( hkbGeneratorOutput::OutputSetup& setup );

			/// Executes internal functionality when a character activates.
		void activateInternal( const hkbContext* context );

			/// Executes internal functionality when a character deactivates.
		void deactivateInternal( const hkbContext* context );
		
			/// Current lod index
		hkInt16 m_currentLod;

			/// Current limited on tracks for lod
		hkInt16 m_numTracksInLod; //+nosave

			/// The generator output used to store the output of the character's behavior graph.
		hkbGeneratorOutput* m_generatorOutput;	//+nosave+overridetype(void*)
		
			/// The name of this character instance.
		hkStringPtr m_name;

			/// The ragdoll driver
		mutable hkRefPtr<hkbRagdollDriver> m_ragdollDriver;	//+nosave
			/// The ragdoll instance
		hkRefPtr<hkbRagdollInterface> m_ragdollInterface;	//+nosave
			/// The character controller driver
		mutable hkRefPtr<hkbCharacterControllerDriver> m_characterControllerDriver; //+nosave
			/// The footIk driver
		mutable hkRefPtr<hkbFootIkDriver> m_footIkDriver; //+nosave
			/// The handIk driver
		mutable hkRefPtr<hkbHandIkDriver> m_handIkDriver; //+nosave
			/// The docking driver
		mutable hkRefPtr<hkbDockingDriver> m_dockingDriver; //+nosave
			/// The AI control driver. This must have dynamic type 
			/// hkbAiControlDriver if it is not null; it has static type 
			/// hkReferencedObject so that it does not depend on that header.
		hkRefPtr<hkReferencedObject> m_aiControlDriver; //+nosave

			/// The character rig and other shared data.
		hkRefPtr<hkbCharacterSetup> m_setup;
			/// Behavior graph for the character
		hkRefPtr<hkbBehaviorGraph> m_behaviorGraph;
			/// Project for the character
		hkRefPtr<hkbProjectData> m_projectData;
			/// The animation binding set for this character if it is different from the one
			/// in m_setup that is shared among all the characters of this type.
		hkRefPtr<hkbAnimationBindingSet> m_animationBindingSet;	//+nosave
		
			/// Spatial query interface for this character. If the user wants the character to have its own
			/// interface then this needs to be set, otherwise the hkbPhysicsInterface from the hkbWorld will be used.
		hkbSpatialQueryInterface* m_spatialQueryInterface; //+nosave
			/// World the character is in.
		hkbWorld* m_world;	//+nosave +owned(false)

			/// Array of bone attachments on this character.  In general, these attachments are instances 
			/// of the attachments stored in the character's hkbCharacterData.
		hkArray<hkaBoneAttachment*> m_boneAttachments;	//+nosave

			/// A queue for events that need to be processed by the character.
		hkbEventQueue* m_eventQueue; //+nosave+overridetype(void*)
					
			/// The character's Lua state
		void* m_characterLuaState; //+nosave

			/// The asset manager this character was loaded with.
			/// This is ref counted but we don't use refptr to avoid issues with HavokAssembly generated code
		hkbProjectAssetManager* m_assetManager; //+nosave

			/// The current set capabilities.
		Capabilities m_capabilities;

			/// The current effective capabilities.
			/// These can be different from m_capabilities if the data doesn't support the full set.
		Capabilities m_effectiveCapabilities;

	public:

		friend class hkbClipGenerator;

#ifdef HK_ENABLE_DETERMINISM_CHECKS
			/// Used for internal determinism testing.
		hkInt32 getUid();
#endif

		hkbCharacter( hkFinishLoadedObjectFlag flag );
};

#include <Behavior/Behavior/Character/hkbCharacter.inl>

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
