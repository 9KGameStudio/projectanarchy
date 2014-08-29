/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_GENERATOR_OUTPUT_H
#define HKB_GENERATOR_OUTPUT_H

#include <Behavior/Behavior/Generator/hkbGeneratorPartitionInfo.h>
#include <Animation/Animation/Rig/hkaSkeleton.h>
#include <Common/Base/Container/BitField/hkBitField.h>

/// Stores the output produced by an hkbGenerator.
class hkbGeneratorOutput
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbGeneratorOutput );

			// Tracks for pose generation and blending only.
			
		enum PoseTracks
		{
			START_OF_POSE_TRACKS,
				TRACK_WORLD_FROM_MODEL = START_OF_POSE_TRACKS,
				TRACK_EXTRACTED_MOTION,
				TRACK_POSE,
			END_OF_POSE_TRACKS,

			START_OF_POSE_DATA_TRACKS = END_OF_POSE_TRACKS,
				TRACK_FLOAT_SLOTS = START_OF_POSE_DATA_TRACKS,
				TRACK_ATTRIBUTES, 
			END_OF_POSE_DATA_TRACKS,
		};

			// Extra feature tracks.
			
		enum FeatureTracks
		{
			START_OF_FEATURE_TRACKS = END_OF_POSE_DATA_TRACKS,
				TRACK_DOCKING_CONTROLS = START_OF_FEATURE_TRACKS,
			END_OF_FEATURE_TRACKS,
		};

			// Physics-optional tracks.
			// These tracks may be used in such a way that requires physics, but can be used w/o physics.
			
		enum PhysicsOptionalTracks
		{
			START_OF_OPTIONAL_PHYSICS_TRACKS = END_OF_FEATURE_TRACKS,
				START_OF_HAND_IK_TRACKS = START_OF_OPTIONAL_PHYSICS_TRACKS,
					TRACK_HAND_IK_CONTROLS_0 = START_OF_HAND_IK_TRACKS,
					TRACK_HAND_IK_CONTROLS_1,
					TRACK_HAND_IK_CONTROLS_2,
					TRACK_HAND_IK_CONTROLS_3,
				END_OF_HAND_IK_TRACKS,
			END_OF_OPTIONAL_PHYSICS_TRACKS = END_OF_HAND_IK_TRACKS,

			// This needs to be set to the number of tracks above that have the prefix TRACK_HAND_IK_CONTROLS.
			HAND_IK_CONTROLS_TRACK_COUNT = (TRACK_HAND_IK_CONTROLS_3 - TRACK_HAND_IK_CONTROLS_0) + 1,
		};

			// Physics-required tracks.
			// These tracks are used in a way that requires physics support.
			
		enum PhysicsRequiredTracks
		{
			START_OF_PHYSICS_TRACKS = END_OF_OPTIONAL_PHYSICS_TRACKS,
				TRACK_CHARACTER_CONTROLLER_CONTROLS = START_OF_PHYSICS_TRACKS,
				START_OF_RAGDOLL_TRACKS, 
					TRACK_RIGID_BODY_RAGDOLL_CONTROLS = START_OF_RAGDOLL_TRACKS,
					TRACK_RIGID_BODY_RAGDOLL_BLEND_TIME,
					TRACK_POWERED_RAGDOLL_CONTROLS,
					TRACK_POWERED_RAGDOLL_WORLD_FROM_MODEL_MODE,
					TRACK_KEYFRAMED_RAGDOLL_BONES,
					TRACK_KEYFRAME_TARGETS,
					TRACK_ANIMATION_BLEND_FRACTION,
				END_OF_RAGDOLL_TRACKS,
				TRACK_FOOT_IK_CONTROLS = END_OF_RAGDOLL_TRACKS,
			END_OF_PHYSICS_TRACKS,
		};

			// Ai-required tracks.
			// These tracks are used in a way that requires ai support.
			
		enum AiRequiredTracks
		{
			START_OF_AI_TRACKS = END_OF_PHYSICS_TRACKS,
				TRACK_AI_CONTROL_CONTROLS_BLENDABLE = START_OF_AI_TRACKS,
				TRACK_AI_CONTROL_CONTROLS_NON_BLENDABLE,
			END_OF_AI_TRACKS,
		};

			/// Convenience counts for various track capabilities.
			/// Each count implies support for the previous.
		enum TrackCounts
		{
				/// Only support pose generation and blending (no extra data like attributes and float data)
			NUM_POSE_TRACKS = END_OF_POSE_TRACKS,

				/// Support pose generation and blending along with extra data like attributes and float data
			NUM_POSE_DATA_TRACKS = END_OF_POSE_DATA_TRACKS,

				/// Support Docking.
			NUM_DOCKING_TRACKS = TRACK_DOCKING_CONTROLS + 1,

				/// Support HandIk.
			NUM_HAND_IK_TRACKS = END_OF_HAND_IK_TRACKS,

				/// Support Character Control.
			NUM_CHARACTER_CONTROL_TRACKS = TRACK_CHARACTER_CONTROLLER_CONTROLS + 1,

				/// Support Ragdoll.
			NUM_RAGDOLL_TRACKS = END_OF_RAGDOLL_TRACKS,

				/// Support FootIk.
			NUM_FOOT_IK_TRACKS = TRACK_FOOT_IK_CONTROLS + 1,

				/// Support all physics related behavior.
			NUM_PHYSICS_TRACKS = END_OF_PHYSICS_TRACKS,

				/// The standard tracks used by behavior graphs.
			NUM_STANDARD_TRACKS = NUM_PHYSICS_TRACKS,

				/// Support all ai related behavior.
			NUM_AI_TRACKS = END_OF_AI_TRACKS,

				/// All tracks.
			NUM_TRACKS = NUM_AI_TRACKS,
		};

			/// The different types of data in the tracks.
		enum TrackTypes
		{
				/// The track should be blended as hkReals.
			TRACK_TYPE_REAL,

				/// The track should be blended as hkQsTransforms.
			TRACK_TYPE_QSTRANSFORM,

				/// The track should not be blended.
			TRACK_TYPE_BINARY,
		};

		enum TrackFlags
		{
			TRACK_FLAG_ADDITIVE_POSE = 1<<0,
			TRACK_FLAG_ADDITIVE_DEPRECATED_POSE = 1<<1,
			TRACK_FLAG_PALETTE = 1<<2,
			TRACK_FLAG_SPARSE = 1<<3,
			TRACK_FLAG_DISABLED = 1<<4,
		};

			/// The header of a track describes the track.
		struct TrackHeader
		{
			//+version(1)

			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbGeneratorOutput::TrackHeader );

				/// The capacity of this track (in number of elements, not bytes).
			hkInt16 m_capacity;

				/// The number of data elements (not bytes) currently in the track.
			hkInt16 m_numData;

				/// The offset of the data for this track from the start of the track buffer.
			hkInt16 m_dataOffset;

				/// The size of each element in the track.
			hkInt16 m_elementSizeBytes;

				/// How much track data has been blended in.  If this is 0, it means the data is invalid.
			hkReal m_onFraction;

				/// The flags for this track.
			hkFlags<TrackFlags,hkInt8> m_flags;

				/// The type of data in the track.
			hkEnum<TrackTypes,hkInt8> m_type;
		};

			/// The master header describes the track buffer as a whole.
		struct TrackMasterHeader
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbGeneratorOutput::TrackMasterHeader );

				/// The total number of bytes allocated to the track.
			hkInt32 m_numBytes;

				/// The number of tracks.
			hkInt32 m_numTracks;

				/// Unused memory to make this struct 16 bytes.
			hkInt8 m_unused[8];
		};

			/// This type is used to access the master header and individual track headers.  We never allocate
			/// one of these directly.  Instead we allocate a block of memory and then cast it to a Tracks.
			/// The array of a single TrackHeader represents a larger array depending on the number of tracks.
			/// You can use m_trackheaders to access all of them, not just one.  Beyond the last TrackHeader
			/// the buffer contains the actual track data.
		struct Tracks
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbGeneratorOutput::Tracks );

				/// The master header contains information about the tracks.
			struct TrackMasterHeader m_masterHeader;

				/// An array of headers for each track (usually there is actually more than 1 track).
			struct TrackHeader m_trackHeaders[1];
		};

			/// An accessor class used for const access to a track (both its header and data).
		class ConstTrack
		{
			public:

				HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbGeneratorOutput::ConstTrack );

					/// Construction given the header and data.
				ConstTrack( TrackHeader* header, hkReal* data );

					/// Get the track header.
				const TrackHeader* getTrackHeader() const;

					/// Get the track data as hkReals.
				const hkReal* getDataReal() const;

					/// Get the track data as hkQsTransforms.
				const hkQsTransform* getDataQsTransform() const;

					/// Return the palette/sparseness indices associated with this track (they are after the data).
				const hkInt8* getIndices() const;

					/// Is the track data valid (m_onFraction > 0)?
				bool isValid() const;

					/// Returns the number of elements of data in the track.
				int getNumData() const;

					/// Returns the capacity of the track.
				int getCapacity() const;

					/// Returns the number of bytes in an element.
				int getElementSizeBytes() const;

					/// Returns the type of data in the track.
				TrackTypes getType() const;

					/// Returns the m_onFraction for the track.
				hkReal getOnFraction() const;

					/// Whether or not this track is a palette (TRACK_FLAG_PALETTE).
				bool isPalette() const;

					/// Whether or not this track is a sparse (TRACK_FLAG_SPARSE).
				bool isSparse() const;

			protected:

				TrackHeader* m_header;
				hkReal* m_data;
		};

			/// An accessor class used for non-const access to a track (both its header and data).
		class Track : public hkbGeneratorOutput::ConstTrack
		{
			public:

				HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbGeneratorOutput::Track );

				Track( TrackHeader* header, hkReal* data );

					/// Writable access to the track data as hkReals.
				hkReal* accessDataReal();

					/// Writable access to the track as hkQsTransforms.
				hkQsTransform* accessDataQsTransform();

					/// Return the palette/sparseness indices associated with this track (they are after the data).
				hkInt8* accessIndices() const;

					/// Set the track data as hkReals.
				void setDataReal( const hkReal* data, int count );

					/// Set the track data as hkQsTransforms.
				void setDataQsTransform( const hkQsTransform* data, int count );

					/// Set one element of the track data as an hkReal.
				void setDataReal( int index, const hkReal& data );

					/// Set one element of the track data as an hkQsTransform.
				void setDataQsTransform( int index, const hkQsTransform& data );

					/// Set the track to be valid (m_onFraction = 1.0).
				void setValid();

					/// Set the m_onFraction for the track.
				void setOnFraction( hkReal onFraction );

					/// Sets the number of elements in the track.
				void setNumData( hkInt16 numData );
		};

			/// Struct for setting up an hkbGeneratorOutput
		struct OutputSetup {

			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbGeneratorOutput::OutputSetup );

				// Ctor.
			OutputSetup() : m_numBones(1), m_numRagdollBones(0), m_numAttributes(0), m_numHands(0), m_numFloatSlots(0), m_numTracks(NUM_STANDARD_TRACKS) {}

				/// The number of pose bones in the output.
				/// Default is 1.
			int m_numBones;

				/// The number of ragdoll bones in the output.
				/// Default is 0.
			int m_numRagdollBones;

				/// The number of attributes in the output.
				/// Default is 0.
			int m_numAttributes;

				/// The number of ik hand data in the output.
				/// Default is 0.
			int m_numHands;

				/// The number of float slots in the output.
				/// Default is 0.
			int m_numFloatSlots;

				/// The number of output tracks.
				/// Default is NUM_STANDARD_TRACKS.
			int m_numTracks;

				/// The enabled tracks.
				/// Default enables all tracks up to numTracks.
			hkBitField m_enabledTracks;
		};

	public:

			/// Construct with a single track for the world-from-model transform.
		hkbGeneratorOutput();

			/// Construct with internal memory allocation and standard track layout.
			/// enabledTracks contains enough bits up to numTracks.  Each bit signifies if the track is enabled or not.
		hkbGeneratorOutput(	const OutputSetup& setup );

			/// Construct given a pose buffer, which will not be deleted on destruction.
			/// Use this if you want to allocate your own track memory instead of having
			/// it allocated on the heap.
			/// This is for advanced use only and requires extra setup for the allocated Tracks data.
		hkbGeneratorOutput( Tracks* tracks );
		
			/// Construct given the track buffer size.  Track memory will be allocated internally.
			/// The track buffer will be totally uninitialized, so be careful.
			/// This is for advanced use only and requires extra setup for the allocated Tracks data.
		hkbGeneratorOutput( int trackBufferSizeBytes );

		~hkbGeneratorOutput();

			/// Set all of the tracks to be invalid (no data).
		void clearTracks();

			/// Set a track to contain valid data.
		void setValid( int trackId );

			/// Whether or not a track contains valid data.
		bool isValid( int trackId ) const;

			/// Whether the track exists
		bool trackExists( int trackId ) const;

			/// Const access to a track.
		ConstTrack getTrack( int trackId ) const;

			/// Non-const access to a track.
		Track accessTrack( int trackId );

			/// Non-const access to a track header.
		TrackHeader& accessTrackHeader( int trackId );

			/// Non-const access to all of the track data.
		Tracks* accessTracks();

			/// Const access to all of the track data.
		const Tracks* getTracks() const;

			/// Const access to track data as hkReals.
		const hkReal* getTrackDataReal( int trackId ) const;

			/// Const access to track data as hkQsTransforms.
		const hkQsTransform* getTrackDataQsTransform( int trackId ) const;

			/// Const access to the world-from-model (track).
		const hkQsTransform& getWorldFromModel() const;

			/// Const access to the extracted motion (track).
		const hkQsTransform& getExtractedMotion() const;

			/// Const access to the pose track.
		const hkQsTransform* getPoseLocal() const;

			/// Const access to the bone weights.
		const hkReal* getBoneWeights() const;

			/// Const access to partitionInfo. This is a pointer to the generator's partitionInfo
		const hkbGeneratorPartitionInfo& getPartitionInfo() const;

			/// Const access to the float track
		const hkReal* getFloats() const;

			/// Const access to the attributes track
		const hkReal* getAttributes() const;

			/// Returns the number of tracks.
		int getNumTracks() const;

			/// Returns the number of enabled tracks.
		int getNumEnabledTracks() const;

			/// Returns the size of the pose track.
		int getNumPoseLocal() const;

			/// Returns the number of ragdoll bones used to allocate.
		int getNumRagdollBones() const;

			/// Returns the size of the bone weights.
		int getNumBoneWeights() const;

			/// Returns the size of the float track.
		int getNumFloats() const;

			/// Returns the size of the attribute track.
		int getNumAttributes() const;

			/// Returns the number of datas for a generic track.
		int getNumData( int trackId ) const;

			/// Returns the capacity for a generic track.
		int getCapacity( int trackId ) const;

			/// Non-const access to track data as hkReals.
		hkReal* accessTrackDataReal( int trackId );

			/// Non-const access to track data as hkQsTransforms.
		hkQsTransform* accessTrackDataQsTransform( int trackId );

			/// Non-const access to the world-from-model (track).
		hkQsTransform& accessWorldFromModel();

			/// Non-const access to the extracted motion (track).
		hkQsTransform& accessExtractedMotion();

			/// non-const access to the pose track.
		hkQsTransform* accessPoseLocal();

			/// non-const access to the bone weights track.
		hkReal* accessBoneWeights();

			//Sets the partition info
		hkbGeneratorPartitionInfo& accessPartitionInfo();

			/// Set the pose to be additive.
		void setPoseAdditive( hkInt8 additiveFlag ); 

			/// Returns whether or not the pose is additive.
		bool isPoseAdditive() const;

			/// Returns the type of additive pose.
		hkInt8 getPoseAdditive() const;

			/// Set the pose to be dense.
		void setPoseLocalDense();

			/// Clear pose data
		void clearPoseData();

	private:

			// the track data (including headers)
		struct Tracks* m_tracks; //+nosave

			// whether or not to delete the track data
		bool m_deleteTracks;
};

#include <Behavior/Behavior/Generator/hkbGeneratorOutput.inl>

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
