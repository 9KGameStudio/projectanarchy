/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_AI_OVERLAP_MANAGER
#define HK_AI_OVERLAP_MANAGER

#include <Ai/Pathfinding/Dynamic/Partitioning/hkaiReferenceFrameAndExtrusion.h>
#include <Ai/Pathfinding/Dynamic/Partitioning/hkaiSilhouetteGeneratorSectionContext.h>
#include <Ai/Pathfinding/Dynamic/NavMeshCutter/hkaiNavMeshSilhouetteSelector.h>
#include <Ai/Pathfinding/Dynamic/Partitioning/hkaiSilhouettePriorityController.h>
#include <Ai/Pathfinding/Collide/hkaiSpatialQueryHitFilter.h>
#include <Ai/Pathfinding/NavMesh/Streaming/hkaiStreamingCollection.h>

#include <Common/Base/Container/Set/hkSet.h>

// Uncomment the following line to enable debug display code
//#define HKAI_DEBUG_DISPLAY_OVERLAP_MANAGER

class hkBitField;

/// Mapping information for one hkaiNavMeshInstance
struct HK_EXPORT_AI hkaiOverlapManagerSection
{
	//+version(1)
	HK_DECLARE_REFLECTION();
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI,hkaiOverlapManagerSection);

	hkaiOverlapManagerSection()
	:	m_generatorDataMap(HK_NULL),
		m_numOriginalFaces(0)
	{}

	hkaiOverlapManagerSection( hkFinishLoadedObjectFlag f)
		:	m_generatorData(f),
			m_facePriorities(f)
	{}

	~hkaiOverlapManagerSection();

	/// Data for a particular active silhouette generator, with respect to this nav mesh section.
	class HK_EXPORT_AI GeneratorData : public hkReferencedObject
	{
	public:
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI);

		inline GeneratorData( const hkaiSilhouetteGenerator* gen, const hkQTransform& initialTransform );
		inline GeneratorData( hkFinishLoadedObjectFlag f)
			:	hkReferencedObject(f),
			m_context(f),
			m_overlappedFaces(f)
		{
		}

			/// If possible, reduce amount of memory owned by members
		void optimizeCapacity();

			/// Context data (used in multithreaded tasks)
		hkaiSilhouetteGeneratorSectionContext m_context;

			/// List of all faces (in this section) currently overlapped by the generator
		hkArray< hkaiNavMesh::FaceIndex > m_overlappedFaces;
	};

		/// Prepare the section information to use the specified instance.
	void init( const hkaiNavMeshInstance* instance );

		/// Deletes all data for the section. Uses when unloading a section or shutting down.
	void clearGeneratorData();
	
		/// If possible, reduce amount of memory owned by members
	void optimizeCapacity();

		/// Returns the index of the first non-cutting generator context if some painting silhouettes need to be 
		/// treated as cutting. If no special handling no special handling is needed, returns -1.
	int sortGeneratorsForFace(const hkaiSilhouettePriorityController* controller, hkaiNavMesh::FaceIndex f);

		/// Get the generator data (e.g. list of overlapped faces) for the specified generator.
	inline GeneratorData* getGeneratorData( const hkaiSilhouetteGenerator* gen);

		/// Get the generator data (e.g. list of overlapped faces) for the specified generator.
	inline const GeneratorData* getGeneratorData( const hkaiSilhouetteGenerator* gen) const;

		/// Set the face dirty if it isn't already.
		/// If it already is, increase its priority by priorityDelta
	inline void setFaceDirty( hkaiNavMesh::FaceIndex f, hkReal priorityDelta );

		/// Set the face to be clean (and implicitly set its priority to zero)
	inline void setFaceClean( hkaiNavMesh::FaceIndex f );

		/// Returns true if the face is dirty.
	inline bool isFaceDirty( hkaiNavMesh::FaceIndex f) const;

		/// Mark all faces as dirty. Not recommended as it will trigger a recut of all the faces (and is slow on its own)
	void dirtyAllFaces();

		/// Returns true if the section is empty (i.e. the corresponding hkaiNavMeshInstance was unloaded)
	inline bool isEmpty() const;

		/// Returns the number of faces in the section.
	inline int getNumFaces() const;

		/// Set up the GeneratorData for the generator (called when adding a generator or loading an instance).
	GeneratorData* addDataForGenerator( const hkaiSilhouetteGenerator*, const hkQTransform& initialTransform );

		/// Remove the GeneratorData for the generator (called when removing a generator or unloading an instance).
		/// Overlapping facse are dirtied and their priority is increased by generatorPriority.
	void removeDataForGenerator( const hkaiSilhouetteGenerator*, hkReal generatorPriority );

		/// Consistency check.
	bool isOk() const;

		/// Called after serialization to rebuild generator->face map
	void rebuildMap(bool updateContexts = true);

public:

	/// Data for each active generator, with respect to this section
	typedef hkPointerMap< const hkaiSilhouetteGenerator*, int > GeneratorDataMap;
	GeneratorDataMap* m_generatorDataMap; //+nosave 

	int m_numOriginalFaces; 

	hkArray< hkRefPtr<GeneratorData> > m_generatorData;

	/// For each original face, contexts for all the generators that currently overlap the face
	hkArray< hkArray<const hkaiSilhouetteGeneratorSectionContext*> > m_faceToGeneratorsMap; //+nosave

	typedef hkSetIntFloatPair PriorityMap;
	PriorityMap m_facePriorities; 
};

	/// An AI "broadphase" that tracks overlaps between nav mesh faces and silhouette generators.
class HK_EXPORT_AI hkaiOverlapManager : public hkReferencedObject,  public hkaiNavMeshSilhouetteSelector
{
public:
	//+version(1)
	HK_DECLARE_REFLECTION();
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_NAVMESH);

		/// Flags to control the updateOverlapsOfAllGenerators
	enum UpdateFlags
	{
		UPDATE_NORMAL = 0,
			/// Force all silhouettes' overlapping sets to be updated (useful for profiling)
		UPDATE_FORCE_ALL = 1,
	};

	friend class hkaiWorld;

	hkaiOverlapManager( const hkaiStreamingCollection* navMeshManager, const hkaiReferenceFrameAndExtrusion& referenceFrameAndExtrusion );
	hkaiOverlapManager( hkFinishLoadedObjectFlag f );
	virtual ~hkaiOverlapManager();

	typedef hkArray< hkRefPtr<hkaiSilhouetteGenerator> > GeneratorArray;
	typedef hkaiOverlapManagerSection Section;

		/// This should be called after a section is loaded to update internal data.
	void loadSection(hkaiRuntimeIndex sectionIndex, const GeneratorArray* generators = HK_NULL);

		/// This should be called after a section is unloaded to update internal data.
	void unloadSection(hkaiRuntimeIndex sectionIndex);

		/// This will update the cell<->generator maps accordingly.
	void addSilhouetteGenerator(const hkaiSilhouetteGenerator* silGen);

		/// Remove a silhouette generator. This will update the cell<->generator maps accordingly.
	void removeSilhouetteGenerator(const hkaiSilhouetteGenerator* silGen);

		/// Manually re-query the overlaps for a specifc generator.
		/// This should be done if e.g. the filter is changed, otherwise the new filtering won't take 
		/// effect until the next time the silhouetre generator moves.
	void recomputeOverlapsForGenerator( const hkaiSilhouetteGenerator* silGen );

		/// Step all the silhouette generators, and record what faces they overlap.
	void updateOverlapsOfAllGenerators( const GeneratorArray& generators, const hkBitField* sectionsToUpdate, hkUint32 updateFlags = UPDATE_NORMAL );

		/// Let the manager know that all updates (bodies removed) are starting anew by clearing the m_faceIsDirty map.
	void markFrameDone();

		/// Should be called whenever silhouette parameters change. Flags all faces as dirty so that their silhouettes will be regenerated on the next frame.
	void updateReferenceFrameAndExtrusion();

		/// Update the hkaiSpatialQueryHitFilter that is used to filter silhouette/face interactions
	void setSilhouetteFilter( const hkaiSpatialQueryHitFilter* filter );

		/// Returns the current hkaiSpatialQueryHitFilter
	inline const hkaiSpatialQueryHitFilter* getSilhouetteFilter() const;

		/// Updates the early-out tolerance used to determine if a silhouette has moved.
	inline void setHasMovedTolerance( hkReal tolerance );

		/// Returns the early-out tolerance used to determine if a silhouette has moved.
	inline hkReal getHasMovedTolerance( ) const;

		/// Sets the maximum number of faces per step that can be cut.
	inline void setMaxCutFacesPerStep( int numFaces );

		/// Gets the maximum number of faces per step that can be cut.
	inline int getMaxCutFacesPerStep( ) const;

		/// Returns whether or not there is a limit on the number of cut faces per step.
	inline bool isLimitingMaxCutFaces() const;

		/// Gets the silhouette priority controller.
	inline const hkaiSilhouettePriorityController* getSilhouettePriorityController() const;
	
		/// Sets the silhouette priority controller.
	void setSilhouettePriorityController( const hkaiSilhouettePriorityController* controller);

		/// If possible, reduce amount of memory owned by members
	void optimizeCapacity();


	//
	// hkaiNavMeshSilhouetteSelector interface
	//

		/// Return a list of face keys which have changed since the last cut.
	virtual void getUpdatedFaces( const class hkaiNavMeshCutter* cutter, hkArray<hkaiPackedKey>::Temp& cutFaceKeysOut, hkArray<hkaiPackedKey>::Temp& uncutFaceKeysOut, const hkBitField* sectionsToUpdate ) HK_OVERRIDE;
		/// For a given face, return a list of silhouettes which are relevant to this face.
	virtual void gatherSilhouettesForFace( const class hkaiNavMeshCutter* cutter, hkaiPackedKey faceKey, const struct hkaiSilhouetteGenerationParameters& genParams, hkArray<hkaiCompoundSilhouette>::Temp& silsOut, hkArray<int>::Temp& silMaterialIds ) HK_OVERRIDE;
		/// Sets up the task
	virtual void setupTaskForFace( const class hkaiNavMeshCutter* cutter, hkaiPackedKey faceKey, class hkaiNavMeshCutFaceTask& task ) HK_OVERRIDE;

#ifdef HKAI_DEBUG_DISPLAY_OVERLAP_MANAGER
	void debugDraw(hkVector4Parameter offset) const;
#endif

		/// Set the hkaiReferenceFrameAndExtrusion used for silhouette generation
	void setReferenceFrameAndExtrusion( const hkaiReferenceFrameAndExtrusion& refFrame );

		/// Get the array of section data.
	const inline hkArray<hkaiOverlapManager::Section>& getSectionData() const;

protected:

		/// Does an AABB query on the section's mediator for the generator, and calls updateSectionFacesForGenerator
		/// on the query hits.
	void queryAndUpdateFacesForGenerator( hkaiRuntimeIndex sectionIdx, Section::GeneratorData* generatorData, 
		const hkaiSilhouetteGenerator* gen, const hkQTransform& silhouetteRelativeTransform );

		/// Udpates the list of overlapping faces for the generator. Dirties the faces of new and removed overlaps.
	void updateSectionFacesForGenerator( hkaiRuntimeIndex sectionIdx, Section::GeneratorData* generatorData, 
		const hkaiSilhouetteGenerator* gen, const hkQTransform& silhouetteRelativeTransform, const hkAabb& genAabb, 
		const hkArrayBase<hkaiPackedKey>& faceKeys );

		/// Gets the AABB for the generator, expanded as specified by the hkaiReferenceFrameAndExtrusion.
	void getExtrudedAabbForGenerator(const hkaiSilhouetteGenerator* gen, const hkaiReferenceFrameAndExtrusion& refFrame, const hkQTransform& localTransform, const hkaiNavMeshInstance* instance, hkAabb& aabb) const;

		/// Extrude a (local-space) AABB and find out if the mesh face overlaps it.
		
	static hkBool32 HK_CALL testLocalAabbAgainstFace(const hkAabb& aabbLocal, const hkaiNavMeshInstance* meshInstance, hkaiNavMesh::FaceIndex faceIndex);

		/// Called after serialization to rebuild generator->face maps
	void rebuildMaps(bool updateContexts = true);

protected:
	hkaiReferenceFrameAndExtrusion m_referenceFrameAndExtrusion;
	
	hkRefPtr<const hkaiStreamingCollection> m_navMeshCollection;

		/// Per-section information, created for each loaded nav mesh instance
	hkArray<Section> m_sections;

		/// Simulation step count, for time-slicing the set of faces to uncut
	int m_stepCount;

		/// Early-out epsilon used when comparing old and new silhouette generator transforms
	hkReal m_hasMovedTolerance; //+default(1e-3f);

		/// Maximum number of faces per frame that are cut.
		/// If the value is 0 or negative, the limit is disabled.
	int m_maxCutFacesPerStep; //+default(-1)

	hkRefPtr<const hkaiSpatialQueryHitFilter> m_silhouetteFilter; //+nosave

		/// hkaiSilhouettePriorityController used to modify the amount each generator contributes to a face,
		/// and also which silhouttes are highest priority on a given face.
	hkRefPtr< const hkaiSilhouettePriorityController > m_priorityController; //+nosave
};

#include <Ai/Pathfinding/Dynamic/Partitioning/hkaiOverlapManager.inl>
#endif // HK_AI_OVERLAP_MANAGER

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
