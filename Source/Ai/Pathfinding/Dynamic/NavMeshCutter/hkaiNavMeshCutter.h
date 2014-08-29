/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HK_NAVMESH_CUTTER_H
#define HK_NAVMESH_CUTTER_H

#include <Ai/Pathfinding/NavMesh/hkaiNavMesh.h>
#include <Ai/Pathfinding/Dynamic/Silhouette/hkaiCompoundSilhouette.h>
#include <Ai/Internal/NavMesh/hkaiNavMeshEdgeMatchingParams.h>
#include <Ai/Pathfinding/NavMesh/hkaiNavMeshUtils.h>
#include <Ai/Pathfinding/Dynamic/NavMeshCutter/hkaiNavMeshSilhouetteSelector.h>
#include <Common/Base/Container/Set/hkSet.h>


extern HK_EXPORT_AI const class hkClass hkaiNavMeshCutterMeshInfoClass;

extern HK_EXPORT_AI const class hkClass hkaiNavMeshCutterSavedConnectivityClass;

extern HK_EXPORT_AI const class hkClass hkaiNavMeshCutterOriginalEdgeInfoClass;

extern HK_EXPORT_AI const class hkClass hkaiNavMeshCutterClass;

extern HK_EXPORT_AI const class hkClass hkSetUint32Class;

extern HK_EXPORT_AI const class hkClass hkSetUint64Class;

class hkaiStreamingCollection;
class hkaiGeneralAccessor;

// Uncomment this to enable more detailed timers in the cutter
//#define HKAI_CUTTER_DETAILED_TIMERS
#ifdef HKAI_CUTTER_DETAILED_TIMERS
#	define HKAI_CUTTER_TIME_CODE_BLOCK(X,Y) HK_TIME_CODE_BLOCK(X, Y)
#	define HKAI_CUTTER_TIMER_BEGIN_LIST(X,Y) HK_TIMER_BEGIN_LIST(X, Y)
#	define HKAI_CUTTER_TIMER_SPLIT_LIST(X) HK_TIMER_SPLIT_LIST(X)
#	define HKAI_CUTTER_TIMER_END_LIST() HK_TIMER_END_LIST()
#else
#	define HKAI_CUTTER_TIME_CODE_BLOCK(X,Y)
#	define HKAI_CUTTER_TIMER_BEGIN_LIST(X,Y)
#	define HKAI_CUTTER_TIMER_SPLIT_LIST(X)
#	define HKAI_CUTTER_TIMER_END_LIST()
#endif


// Uncomment this to add extra validation checks to the nav mesh cutter
//#define HKAI_CUTTER_EXTRA_CHECKS

struct hkaiFaceCutResults;

/// A face key which can be resolved to the same face (cut or not) after other faces are re-cut, as long
/// as the face itself is not re-cut. Fill the structure with hkaiNavMeshCutter::makePersistentFaceKey,
/// and resolve it to a face key with hkaiNavMeshCutter::resolvePersistentFaceKey.
struct hkaiPersistentFaceKey
{
	//+version(0)
	HK_DECLARE_POD_TYPE();
	HK_DECLARE_REFLECTION();

	hkaiPackedKey m_key;
	hkInt16 m_offset;

	static const hkInt16 OFFSET_UNCUT_FACE = -1;

	hkaiRuntimeIndex getRuntimeId() const { return hkaiGetRuntimeIdFromPacked(m_key); }

	hkBool32 isValid() const { return m_key != HKAI_INVALID_PACKED_KEY; }

	hkBool32 operator==(hkaiPersistentFaceKey const& other) const
	{
		return m_key == other.m_key && m_offset == other.m_offset;
	}
	hkBool32 operator!=(hkaiPersistentFaceKey const& other) const
	{
		return m_key != other.m_key || m_offset != other.m_offset;
	}

	void setInvalid() { m_key = HKAI_INVALID_PACKED_KEY; m_offset = -1; }
};

	/// Edge information that tracks an edge key relative to the start of its corresponding face.
	/// This is valid if the face changes index (due to other faces being cut or uncut),
	/// but if this face gets cut or recut, the hkaiPersistentEdgeKey is invalid.
struct hkaiPersistentEdgeKey
{
	HK_DECLARE_POD_TYPE();
	HK_DECLARE_REFLECTION();

	hkaiPersistentFaceKey m_faceKey;
	hkInt16 m_edgeOffset;

		/// A value for m_edgeOffset which specifies that this "edge key" is to be
		/// used only for its contained face key.
	static const hkInt16 EDGE_OFFSET_FACE_ONLY = -1;

	hkaiRuntimeIndex getRuntimeId() const { return m_faceKey.getRuntimeId(); }

	hkBool32 hasValidFace() const { return m_faceKey.isValid(); }
	hkBool32 isValidEdge() const { return m_faceKey.isValid() && m_edgeOffset != EDGE_OFFSET_FACE_ONLY; }

	hkBool32 operator==(hkaiPersistentEdgeKey const& other) const
	{
		return m_faceKey == other.m_faceKey && m_edgeOffset == other.m_edgeOffset;
	}
	hkBool32 operator!=(hkaiPersistentEdgeKey const& other) const
	{
		return m_faceKey != other.m_faceKey || m_edgeOffset != other.m_edgeOffset;
	}

	void setInvalid() { m_faceKey.setInvalid(); m_edgeOffset = EDGE_OFFSET_FACE_ONLY; }

	void setFaceOnly() { m_edgeOffset = EDGE_OFFSET_FACE_ONLY; }
};


	/// Dynamically and incrementally cuts 2d shapes from a nav mesh.
	/// Starting with a base nav mesh, this class can cut a subset of
	/// faces to allow for additional obstacles. Cutting is always done from the
	/// base nav mesh which is modified in place. Each original face is
	/// replaced by one or more smaller "cut" faces.
	/// Cut faces, edges and vertices appear after the original faces,
	/// edges and vertices. Note that their indices will change after each cut
	/// as empty space is compacted.
	/// Original faces, edges and vertices remain at their original indices.
	/// Faces which have been cut and therefore not logically part of the nav mesh
	/// have hkaiNavMesh::FLAG_HIDDEN set.
class HK_EXPORT_AI hkaiNavMeshCutter : public hkReferencedObject
{
public:

	//+vtable(true)
	//+version(15)
	HK_DECLARE_REFLECTION();
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_NAVMESH);

		/// Controls how nav mesh clearance values are reset after cutting.
	enum ClearanceResetMethod
	{
			/// Always reset all clearance values. This is fast, but will increase path search costs.
		CLEARANCE_RESET_ALL,

			/// Selectively reset clearance values by querying the mediator for nearby faces.
		CLEARANCE_RESET_MEDIATOR,

			/// Selectively reset clearance values by flood filling from cut faces.
		CLEARANCE_RESET_FLOODFILL,
	};

		/// List of original edge keys that have been instanced and overwritten due to cutting.
	struct HK_EXPORT_AI SavedConnectivity
	{
		//+version(1)
		HK_DECLARE_REFLECTION();
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_NAVMESH, SavedConnectivity);
		SavedConnectivity();
		SavedConnectivity( hkFinishLoadedObjectFlag f) : m_storage(f) {}
		~SavedConnectivity();

		void addConnectivity( hkaiPackedKey edgeKey );
		void popConnectivityForFace( hkaiStreamingCollection* collection, hkaiPackedKey faceKey );
		hkBool32 popConnectivityForEdgeAndFace( hkaiStreamingCollection* collection, hkaiPackedKey edgeKey );
		void popConnectivityForOppositeFace( hkaiStreamingCollection* collection, hkaiPackedKey faceKey );

		void restore( hkaiStreamingCollection* collection, hkaiPackedKey edgeKey );
		void append( const SavedConnectivity& other);

#ifdef HKAI_64BIT_PACKED_KEYS
		typedef hkSetUint64 StorageMap;
#else
		typedef hkSetUint32 StorageMap;
#endif
		StorageMap m_storage;
	};

		/// Special values in the m_magic array
	enum
	{
			/// Face has not been cut
		FACE_NOT_CUT = -1,
			/// Face is entirely removed and has no corresponding cut faces
		FACE_COMPLETELY_CUT = -2
	};

	typedef hkaiNavMeshSilhouetteSelector SilhouetteSelector;

	/// Map an original edge to an array of new edges
	struct HK_EXPORT_AI FaceEdgePair
	{
		HK_DECLARE_POD_TYPE();
		hkaiPackedKey m_edgeKey;
		hkaiPackedKey m_faceKey;

		inline void set(hkaiPackedKey faceKey, hkaiPackedKey edgeKey)
		{
			m_faceKey = faceKey;
			m_edgeKey = edgeKey;
		}

		inline void setWithSection( hkaiRuntimeIndex sectionId, hkaiNavMesh::FaceIndex faceIndex, hkaiNavMesh::EdgeIndex edgeIndex )
		{
			m_faceKey = hkaiGetPackedKey(sectionId, faceIndex);
			m_edgeKey = hkaiGetPackedKey(sectionId, edgeIndex);
		}

	};

	typedef hkArray<hkaiPackedKey> FaceList;
	typedef hkPointerMap< hkaiPackedKey, FaceList*, hkContainerTempAllocator > EdgeMap;

	//
	// Methods
	//

		/// Default
	hkaiNavMeshCutter();
	hkaiNavMeshCutter(hkFinishLoadedObjectFlag f);
	~hkaiNavMeshCutter();


		/// Use the supplied mesh manager as the base mesh.
	void init( hkVector4Parameter up, class hkaiStreamingCollection* collection );

		/// Only set up one section
	void initSection( hkaiRuntimeIndex sectionIdx, class hkaiNavMeshInstance* meshInstance, bool doChecks = true );

		/// Cut faces returned by the selector.
		/// Note that cut (but not original) face, edge and vertex indices will be invalid after this call.
		/// If the task queue and thread pool are specified, the cutting will be performed multithreaded.
	hkBool32 cutSilhouettesIncremental( SilhouetteSelector* selector, const struct hkaiSilhouetteGenerationParameters& genParams,
		hkArray<hkaiPackedKey>::Temp& cutFacesKeys, hkArray<hkaiPackedKey>::Temp& uncutFacesKeys, const hkBitField* sectionsToStep,
		class hkTaskQueue* taskQueue = HK_NULL, class hkThreadPool* threadPool = HK_NULL );

		/// Get the lowest-numbered cut face corresponding to the given original face, or HKAI_INVALID_PACKED_KEY
		/// if the face is not cut or is copmpletely cut.
	hkaiPackedKey getFirstCutKeyFromOriginal(hkaiPackedKey orig) const;

		/// Get the keys of the faces which were cut from an original face.
		/// Returns false if the face was not cut.
	hkBool32 getCutKeysFromOriginal( hkaiPackedKey orig, hkArray<hkaiPackedKey>& cutOut ) const;

		/// Get the start and end indices of the faces which were cut from an original face.
		/// Returns false if the face was not cut.
		/// The range is inclusive, so iterating over the faces would look like:
		/// hkaiNavMesh::FaceIndex faceStart = hkaiNavMesh::INVALID_FACE_INDEX, faceEnd = = hkaiNavMesh::INVALID_FACE_INDEX;
		/// for (hkaiNavMesh::FaceIndex f = faceStart; f <= faceEnd; f++) { ... }
	hkBool32 getCutKeyRangeFromOriginal( hkaiPackedKey orig, hkaiNavMesh::FaceIndex& faceStartOut, hkaiNavMesh::FaceIndex& faceEndOut) const;

		/// Returns the number of faces which were cut from an original face.
		/// Returns 0 if the face was not cut.
	int getNumCutKeysFromOriginal( hkaiPackedKey orig ) const;

		/// Get the original key of the face from a cut face.
		/// If the face isn't cut, returns HKAI_INVALID_FACE_KEY
	hkaiPackedKey getOriginalFromCutKey( hkaiPackedKey cutFaceKey) const;

		/// Get the original number of faces for a mesh section
	int getOriginalNumberOfFaces( hkaiRuntimeIndex sectionIdx ) const;

		/// Gets the closest point on the specified face. If the face is hidden, its corresponding hit faces are checked.
	hkaiPackedKey getClosestPointOnFaceLocal( hkVector4Parameter position, hkaiPackedKey faceKey, hkVector4& closestPointOut ) const;

		/// Gets the closest boundary edge on the specified face. If the face is hidden, its corresponding hit faces are checked.
	hkaiPackedKey getClosestBoundaryOnFaceLocal( hkVector4Parameter position, hkaiPackedKey faceKey, hkVector4Parameter up, hkVector4& closestPointOut ) const;


		/// Undoes all cuts for the specified section
	void uncutSection( hkaiRuntimeIndex sectionIndex, bool doChecks = true );

		/// Access the mesh being cut.
	const hkaiStreamingCollection& getManager() const { return *(m_streamingCollection.val()); }

		/// Performs edge matching between sets of new (cut edges)
		/// All edges in pairsA came from the same original edge, and all the edges in pairB came from the original edge
		/// which was opposite it.
	void matchNewEdgesFromOriginalEdge( hkaiStreamingCollection* collection, hkArrayBase<FaceEdgePair>& pairsA, hkArrayBase<FaceEdgePair>& pairsB );

		/// Get the array that maps between original and cut edges.
	const hkArray<int>& getMappingForSection( hkaiRuntimeIndex secIdx ) const { return m_meshInfos[secIdx].m_magic; }

		/// Whether gatherCutEdges() should get all edges or just boundary edges.
	enum GatherCutEdgesMode
	{
		GATHER_ALL_EDGES,
		GATHER_BOUNDARY_EDGES
	};

		/// Collects the cut edges corresponding to the originalEdgeKey. If GATHER_BOUNDARY_EDGES is used, only boundary
		/// edges are returned.
	void gatherCutEdges( hkaiPackedKey originalEdgeKey, hkaiPackedKey originalFaceKey, hkArray<hkaiNavMeshCutter::FaceEdgePair>::Temp& cutPairs, GatherCutEdgesMode gatherMode = GATHER_ALL_EDGES) const;

		/// Collects the cut edges corresponding to the originalEdgeKey that are in the cut faces give by cutfaceKeys.
		/// The cut faces must have come from the original face that contained originalEdgeKey.
	void gatherCutEdgesFromCutFaces( hkaiPackedKey originalEdgeKey, const hkArrayBase<hkaiPackedKey>& cutfaceKeys,  hkArray<hkaiNavMeshCutter::FaceEdgePair>::Temp& cutPairs ) const;

		/// Merge contiguous boundary edges on the cut face that came from the same original edge
	void mergeCutBoundaryEdges( hkaiPackedKey originalFaceKey, hkaiPackedKey originalEdgeKey );

	//
	// Clearance methods
	//

		/// Set the clearance reset method.
	void setClearanceResetMethod( ClearanceResetMethod crt ) { m_clearanceResetMethod = crt; }

		/// Get the clearance reset method.
	ClearanceResetMethod getClearanceResetMethod( ) const { return m_clearanceResetMethod; }

		/// Resets the clearance values for the faces in cutFaceKeys, according to the clearance reset method.
	void resetClearanceForCutFaces( const hkArrayBase<hkaiRuntimeIndex>& updatedSections, const hkSet<hkaiRuntimeIndex>& userEdgeModifiedSections, const hkArrayBase<hkaiPackedKey>& cutFaceKeys  );

		/// Recomputes all dirty global clearance values for the instance.
		/// A value is considered dirty if it is greater than the maxGlobalClearance for the instance.
	void recomputeDirtyGlobalClearances( hkaiNavMeshInstance& instance );

		/// Resets edge clearances and recomputes global clearances for the faces in the set.
	void recomputeGlobalClearancesForFaces( const hkSet<hkaiPackedKey>& facesToReset );

	//
	// Persistent edge keys
	//

		/// Make a persistent record to the given (possibly cut) face, which can be resolved to the equivalent face
		/// even after face re-cuts move it to a different place, as long as the original face is not itself re-cut.
	void makePersistentFaceKey(hkaiPackedKey key, hkaiPersistentFaceKey & persistentFaceKeyOut) const;

		/// Make a persistent record to the given edge (which may belong to a cut face), which can be resolved to the
		/// equivalent edge even after face re-cuts move it to a different place, as long as neither face of the edge
		/// is itself re-cut.
	void makePersistentEdgeKey(hkaiPackedKey edgeKey, hkaiPackedKey faceKey, hkaiPersistentEdgeKey & persistentEdgeKeyOut) const;

		/// Resolve a previously created persistent face key to its current face key. This face key is valid until
		/// the next cutting operation.
	hkaiPackedKey resolvePersistentFaceKey(hkaiPersistentFaceKey const& persistentFaceKey) const;

		/// Resolve a previously created persistent edge key to its current edge key. This edge key is valid until
		/// the next cutting operation.
	hkaiPackedKey resolvePersistentEdgeKey(hkaiPersistentEdgeKey const& persistentEdgeKey) const;

		/// Static version of makePersistentFaceKey which allows the cutter to be NULL. If the cutter is NULL, the
		/// key is assumed to be un-cut.
	static void makePersistentFaceKey(hkaiNavMeshCutter const* cutter, hkaiPackedKey key, hkaiPersistentFaceKey & persistentFaceKeyOut);

		/// Static version of makePersistentEdgeKey which allows the cutter to be NULL. If the cutter is NULL, the
		/// key is assumed to be un-cut.
	static void makePersistentEdgeKey(hkaiNavMeshCutter const* cutter, hkaiGeneralAccessor const& accessor, hkaiPackedKey edgeKey, hkaiPackedKey faceKey, hkaiPersistentEdgeKey & persistentEdgeKeyOut);

		/// Static version of resolvePersistentFaceKey which allows the cutter to be NULL. If the cutter is NULL, the
		/// key must be un-cut.
	static hkaiPackedKey resolvePersistentFaceKey(hkaiNavMeshCutter const* cutter, hkaiPersistentFaceKey const& persistentFaceKey);

		/// Static version of resolvePersistentEdgeKey which allows the cutter to be NULL. If the cutter is NULL, the
		/// key must be un-cut.
	static hkaiPackedKey resolvePersistentEdgeKey(hkaiNavMeshCutter const* cutter, hkaiGeneralAccessor const& accessor, hkaiPersistentEdgeKey const& persistentEdgeKey);

		/// Static version of resolvePersistentEdgeKey which allows the cutter to be NULL. If the cutter is NULL, the
		/// key must be un-cut. Additionally, returns the face key.
	static hkaiPackedKey resolvePersistentEdgeKey(hkaiNavMeshCutter const* cutter, hkaiGeneralAccessor const& accessor, hkaiPersistentEdgeKey const& persistentEdgeKey, hkaiPackedKey & faceKeyOut);

protected:

		/// Validate mapping information and nav mesh instance flags.
	hkBool32 isValid() const;

		/// Runs the face cutter on the specified face keys (single threaded). Outputs are saved in the results array.
	void intersectMeshWithSilhouettes( const hkArrayBase<hkaiPackedKey>& faceKeys, hkaiNavMeshCutter::SilhouetteSelector* selector, const struct hkaiSilhouetteGenerationParameters& genParams, hkArrayBase<hkaiFaceCutResults>& results);

		/// Runs the face cutter on the specified face keys (multi threaded). Outputs are saved in the results array.
	void intersectMeshWithSilhouettesMT( const hkArrayBase<hkaiPackedKey>& faceKeys, hkaiNavMeshCutter::SilhouetteSelector* selector, const struct hkaiSilhouetteGenerationParameters& genParams, class hkTaskQueue* taskQueue, class hkThreadPool* threadPool, hkArrayBase<hkaiFaceCutResults>& results );

	/// Apply the hkaiFaceCutResults from intersectMeshWithSilhouettes (or intersectMeshWithSilhouettesMT) to the streaming collection.
	void applyCutResults( const hkArrayBase<hkaiRuntimeIndex>& cutSections, hkArray<hkaiFaceCutResults>::Temp& results, hkArray<hkaiPackedKey>::Temp& cutFaces );

		/// Finds all cut faces corresponding to the original faces in updatedFaceKeys, and removes them from their hkaiNavMeshInstance.
	void removeCutFacesFromOriginals( const hkArrayBase<hkaiRuntimeIndex>& sections, const hkArrayBase<hkaiPackedKey>& updatedFaceKeys, hkaiNavMeshUtils::RemoveOwnedFacesMode rofMode );
	
		/// Calls compactInstancedEdges on all sections specified in cutSections.
	void removeUnusedCutEdges( const hkArrayBase<hkaiRuntimeIndex>& cutSections );

		/// Do edge matching between faces that are marked for streaming.
		
	void matchStreamingFaces( const hkArrayBase<hkaiRuntimeIndex>& cutSections, const hkSet<hkaiPackedKey>::Temp& cutFaces );
	
		/// Checks whether the edges "match", i.e. is hkaiNavMeshCutterHelper::edgeEndpointDistance() < m_cutEdgeTolerance.
	hkBool32 doEdgesMatch( hkaiPackedKey edgeKeyA, hkaiPackedKey edgeKeyB ) const;

		/// Get new connectivity informatoin when uncutting faces.
	void calcNewConnectivity( const hkArrayBase<hkaiPackedKey>& uncutFaceKeys, const hkArrayBase<hkaiPackedKey>& cutFaceKeys, SavedConnectivity& tempConnectivity, hkArray<hkaiPackedKey>::Temp& forceRecutFaceKeys );

		/// Roll back modifications to the instanced edges for faces in the updatedFaceKeys array
	static void HK_CALL restoreConnectivity( hkaiStreamingCollection* collection, const hkArrayBase<hkaiPackedKey>& updatedFaceKeys, SavedConnectivity& connectivity);

		/// Determine which faces to reset, using the dynamic mediator.
	void getFacesForResetByMediator ( hkSet< hkaiPackedKey >& facesToReset, const hkArrayBase< struct AabbAndLayer >& faceAabbs ) const;

		/// Determine which faces to reset, floodfilling from the faces.
	void getFacesForResetByFloodFill( hkSet< hkaiPackedKey >& facesToReset, const hkArrayBase<hkaiPackedKey>& cutFaceKeys, const hkArrayBase< struct AabbAndLayer >& faceAabbs ) const;

		/// Set the face flags and force-cut the face (called during loading).
	void setFaceAsStreaming( hkaiPackedKey faceKey );


public:
		/// Cutting information for a nav mesh. One MeshInfo is created for each nav mesh instance that is loaded in the hkaiStreamingCollection.
	struct HK_EXPORT_AI MeshInfo
	{
		HK_DECLARE_REFLECTION();
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_NAVMESH, MeshInfo);

		MeshInfo() {}

			/// Number of original faces in the mesh
		int m_originalNumFaces;

			/// Number of original edges in the mesh
		int m_originalNumEdges;

			/// Number of original vertices in the mesh
			
		int m_originalNumVertices; 

			/// A bidirectional mapping between original faces and cut faces.
			///
			/// For indices between 0 and m_originalNumFaces, each element i
			/// either -1, indicating the face has not been cut, or the index of
			/// the first cut face corresponding to this original face.
			///
			/// For indices greater than or equal to m_originalNumFaces, each
			/// element holds the index of the original face corresponding to the
			/// cut face.
		hkArray<int> m_magic;

		MeshInfo(hkFinishLoadedObjectFlag f)
			: m_magic(f)
		{
		}
			/// Intialize the MeshInfo from the hkaiNavMeshInstance.
		void init(const hkaiNavMeshInstance* meshInstance);

			/// Get the number of faces before any cuts.
		int getOriginalNumFaces() const { return m_originalNumFaces; }

			/// Get the number of edges before any cuts.
		int getOriginalNumEdges() const { return m_originalNumEdges; }

			/// Get the number of vertices before any cuts.
		int getOriginalNumVertices() const { return m_originalNumVertices; }

			/// Validate mapping information and nav mesh instance flags.
		hkBool32 isValid(const hkaiNavMeshInstance*, const SavedConnectivity& connectivityInfo) const;

		private:
			MeshInfo& operator=( const MeshInfo& other);
	};

protected:


	hkArray<struct MeshInfo> m_meshInfos;

 		/// List of original edge keys that have been instanced and overwritten due to cutting.
	struct SavedConnectivity m_connectivityInfo;

		/// World's streaming collection.
	hkRefPtr<hkaiStreamingCollection> m_streamingCollection;

		/// List of faces which have to be recut next frame
	hkArray<hkaiPackedKey> m_forceRecutFaceKeys;

		/// List of faces who need their clearance reset
	hkArray<hkaiPackedKey> m_forceClearanceCalcFaceKeys;

public:
		/// Used during edge matching
	hkVector4 m_up;

		/// Edge matching parameters. They should be set as least as large as the ones used in nav mesh generation.
	hkaiNavMeshEdgeMatchingParameters m_edgeMatchParams;

		// Optional pointer to hkaiNavMeshGenerationSettings, used during erosion and paged generation for edge matching.
		// If non-null, m_edgeMatchParams will be ignored.
	hkRefPtr< const struct hkaiNavMeshGenerationSettings > m_generationSettings; //+nosave

		/// Tolerance used to determine when a pair of edges can be easily matched with each other.
	hkReal m_cutEdgeTolerance; //+default(1e-4f)

		/// Cut edges below this length will not be considered for matching (and thus left blocked).
		/// Setting this to non-zero can decrease cutting time, but may leave some small boundary edges in the cut mesh.
	hkReal m_minEdgeMatchingLength; //+default(0.0f)

		/// When edge matching, interval start and end points that are within this tolerance of 0 or 1 respectively will
		/// be snapped to that value. Also, adjacent intervals whose start and endpoints are within this tolerance of
		/// each other will have the start and end value replaced with the average, eliminating small boundaries.
	hkReal m_smallGapFixupTolerance; //+default(.001f)

		/// Whether or not to do nav mesh validation during cutting.
		/// These are always disabled during release builds, but potentially slow in debug builds
	hkBool m_performValidationChecks; //+default(true)

		/// How to reset the clearance cache after cutting.
	hkEnum<ClearanceResetMethod, hkUint8> m_clearanceResetMethod; //+default(hkaiNavMeshCutter::CLEARANCE_RESET_ALL)

		/// Whether to compute the clearance values after cutting, or on the fly during A*
		/// See hkaiWorld::m_precomputeNavMeshClearance for a description of the tradeoffs.
	hkBool m_recomputeClearanceAfterCutting; //+default(false)

		/// Whether or not to use the new sweep-line based cutter, or the deprecated triangulator-based cutter.
	hkBool m_useNewCutter; //+default(true)

		/// A forced domain quantum for the face cutter. This is used internally by
		/// nav mesh erosion, and should not be used otherwise.
	hkReal m_domainQuantum; //+default(0)
};

	/// Utility methods for hkaiNavMeshCutter.
class hkaiNavMeshCutterHelper
{
public:
		/// Face patching
	static void HK_CALL connectCutFaceToUncutFace(hkaiStreamingCollection* collection, hkaiNavMeshCutter::SavedConnectivity& connectivityInfo,
		hkaiPackedKey uncutEdgeKey, hkaiPackedKey originalCutEdgeKey,
		const hkArrayBase<hkaiNavMeshCutter::FaceEdgePair>& cutPairs);

		/// Makes a copy of the original face, and makes the new face "cut" from the original
	static void HK_CALL replaceFaceWithCutFace(hkaiStreamingCollection* collection, hkaiPackedKey faceKey, hkaiPackedKey skipEdgeKey, hkaiNavMeshCutter::SavedConnectivity& connectivityInfo,
		hkArray<int>& magic, hkaiNavMeshCutter::EdgeMap& edgeMap);

		/// Returns the sum of the projected distances between the edge endpoints
	static hkSimdReal HK_CALL edgeEndpointDistance(const hkaiNavMeshInstance& meshA, const hkaiNavMeshInstance& meshB, hkVector4Parameter up, hkaiNavMesh::EdgeIndex edgeA, hkaiNavMesh::EdgeIndex edgeB);

		/// Merge contiguous boundary edges on the cut face that came from the same original edge
	static void HK_CALL mergeContiguousBoundaryEdges( hkaiStreamingCollection* collection, const hkaiNavMesh::Face& originalFace, hkaiPackedKey originalEdgeKey, hkaiNavMesh::FaceIndex cutFaceIndex );

};

#endif // HK_NAVMESH_CUTTER_H

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
