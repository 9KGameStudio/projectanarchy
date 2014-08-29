/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_QUERY_UTILS_H
#define HKB_QUERY_UTILS_H

#include <Behavior/Behavior/Node/hkbNode.h>
#include <Behavior/Behavior/Utils/hkbUtils.h>

class hkbAnimationBindingSet;
class hkbAnimationBindingWithTriggers;
class hkbBehaviorGraph;
class hkbSymbolLinker;
struct hkbAssetBundleStringData;
class hkbCharacter;
class hkbAssetBundle;
class hkbClipGenerator;
struct hkbHandIkTransforms;
struct hkbFootIkTransforms;

	/// Utils for querying the current state of the system.
class hkbBehaviorQueryUtils
{
public:

		/// Coordinate space for returned transforms
	enum CoordinateSpace
	{
		LOCAL,
		MODEL,
		WORLD,
	};
	
		/// Retrieve the animation bindings that are currently loaded AND linked into the binding set.
		/// Also returns the size of loadedAnimationsOut for convenience.
	static int HK_CALL getLoadedAnimations(
		const hkbAnimationBindingSet* bindingSet,
		hkArray<const hkbAnimationBindingWithTriggers*>& loadedAnimationsOut );

		/// Retrieve the animation bundles that are currently loaded AND linked into the binding set.
		/// Also returns the size of loadedBundlesOut for convenience.
	static int HK_CALL getLoadedBundles(
		const hkbAnimationBindingSet* bindingSet,
		hkArray<const hkbAssetBundle*>& loadedBundlesOut );

		/// Retrieve the asset names that are referenced by the Behavior. This will be a list of bundles or animations.
		/// Also returns the size of referencedAssetsToLoadOut for convenience.
		/// Returned hkStringPtrs point to internal string data, should copy if you want to reference later.
	static int HK_CALL getReferencedAssetsToLoad(
		const hkbBehaviorGraph* behavior,
		hkArray<const char*>& referencedAssetsToLoadOut );

		/// Retrieve the animation names that are referenced by the Behavior. The animations may be loose or bundled.
		/// Also returns the size of referencedAnimationsOut for convenience.
		/// Returned hkStringPtrs point to internal string data, should copy if you want to reference later.
	static int HK_CALL getReferencedAnimations(
		const hkbBehaviorGraph* behavior,
		hkArray<const char*>& referencedAnimationsOut );

		/// Retrieve the bundle information referenced by the Behavior.
		/// Also returns the size of referencedBundlesOut for convenience.
	static int HK_CALL getReferencedBundles(
		const hkbBehaviorGraph* behavior,
		hkArray<hkbAssetBundleStringData>& referencedBundlesOut );

		/// Is a particular node active in the graph.
		/// If you already have a pointer to the template node use behavior->isActive().
	static bool HK_CALL isNodeActive( const hkbBehaviorGraph* behavior, const char* nodeName );

		/// Is a state active in the graph.
		/// includeTransitioningStates indicates whether the result includes states which aren't current but being transitioned to/from.
	static bool HK_CALL isStateActive( const hkbBehaviorGraph* behavior, const char* stateName, bool includeTransitioningStates = true );

		/// Determine if the clip is in range of the external event ids passed in.
		/// You can use clip->getLocalTime() if you already know the begin/end times.
		/// defaultValue is returned if either event is not found in the clip.
		/// Note: This utility assumes the events only occur once in the clip and does not consider clip echos.
	static bool HK_CALL isClipInRange(
		const hkbBehaviorGraph* behavior,
		const hkbClipGenerator* clip,
		hkbEvent::Id externalStartEventId,
		hkbEvent::Id externalEndEventId,
		bool defaultValue = false );
	
		/// Retrieve the current transforms for HandIk bones. Invalid bones will return an invalid tranform (use isOk()).
		/// HandIk is performed at the end of the world step by a scene modifier, so having a node in the graph update
		/// a variable will only give you the previous frames transform.
		/// Also returns the size of handIndexToTransformsOut for convenience.
	static int HK_CALL getHandIkTransforms(
		const hkbCharacter* character,
		hkArray<hkbHandIkTransforms>& handIndexToTransformsOut,
		CoordinateSpace space = WORLD );

		/// Retrieve the current transforms for FootIk bones. Invalid bones will return an invalid tranform (use isOk()).
		/// FootIk is performed at the end of the world step by a scene modifier, so having a node in the graph update
		/// a variable will only give you the previous frames transform.
		/// Also returns the size of footIndexToTransformsOut for convenience.
	static int HK_CALL getFootIkTransforms(
		const hkbCharacter* character,
		hkArray<hkbFootIkTransforms>& footIndexToTransformsOut, 
		CoordinateSpace space = WORLD );

		/// Gather info telling which events and variables are available in a character's graph right now.
	static void HK_CALL getActiveEventsAndVariables(
		const hkbBehaviorGraph* behavior,
		hkArray<hkInt16>& activeEvents,
		hkArray<hkInt16>& activeVariables,
		bool returnGlobalVariableIds = true );

		/// Retrieve all the nodes with the given name.
		/// You can optionally prune by type and/or flags (ignores FLAG_IGNORE_REFERENCED_BEHAVIORS when collecting active nodes).
		/// The nodes returned are always templates. (see hkbBehaviorGraph::getNodeTemplate/getNodeClone/isActive).
		/// Also returns the size of nodesOut for convenience.
	static int HK_CALL getNodesByName(
		const hkbBehaviorGraph* behavior,
		const char* nodeName,
		hkArray<hkbUtils::NodeBehaviorPair>& nodesOut,
		hkbNodeType type = HKB_NODE_TYPE_INVALID, 
		hkbNode::GetChildrenFlags flags = 0 );
};

	/// Struct for transforms associated with an Iked hand.
struct hkbHandIkTransforms
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbHandIkTransforms );

	hkQsTransform m_shoulderTransform;
	hkQsTransform m_shoulderSiblingTransform;
	hkQsTransform m_elbowTransform;
	hkQsTransform m_elbowSiblingTransform;
	hkQsTransform m_wristTransform;
};

	/// Struct for transforms associated with an Iked foot.
struct hkbFootIkTransforms
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbFootIkTransforms );

	hkQsTransform m_hipTransform;
	hkQsTransform m_kneeTransform;
	hkQsTransform m_ankleTransform;
};

#endif // HKB_QUERY_UTILS_H

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
