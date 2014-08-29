/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_MANUAL_SELECTOR_TRANSITION_EFFECT_H
#define HKB_MANUAL_SELECTOR_TRANSITION_EFFECT_H

#include <Behavior/Behavior/TransitionEffect/hkbTransitionEffect.h>

extern const class hkClass hkbManualSelectorTransitionEffectClass;

class hkbContext;
class hkbCustomIdSelector;

class hkbManualSelectorTransitionEffect : public hkbTransitionEffect
{   
	//+hk.Ui(hideBaseClassMembers="selfTransitionMode,eventMode")
	// m_ is prepended to these member names when resolved by HAT.
	
	// Note: The attribute is looked for in the *very* specific format of:
	// class Klass : public KlassBase /*can't be next line*/
	// {
	//     //+hk.Ui(...)

	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbManualSelectorTransitionEffect();
		~hkbManualSelectorTransitionEffect();

	protected:

		hkbManualSelectorTransitionEffect( const hkbManualSelectorTransitionEffect& te );

	public:

			/// Get a pointer to the current state machine we are executing in
		const hkbStateMachine* getCurrentStateMachine() const { return m_parentStateMachine; }

		//////////////////////
		// hkbNode interface 
		//////////////////////

		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

		virtual void deactivate( const hkbContext& context ) HK_OVERRIDE;

		virtual int getMaxNumChildren( GetChildrenFlags flags ) HK_OVERRIDE;

		virtual void getChildren( GetChildrenFlags flags, ChildrenInfo& childrenInfo ) HK_OVERRIDE;

		virtual bool isValid( const hkbCharacter* character, hkStringPtr& errorString ) const HK_OVERRIDE;

		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

		////////////////////////////
		// hkbGenerator interface
		////////////////////////////

		virtual void generate(
			const hkbContext& context,
			const hkbGeneratorOutput** activeChildrenOutput,
			hkbGeneratorOutput& output,
			hkReal timeOffset = 0.0f ) const HK_OVERRIDE;

		virtual bool canRecycleOutput() const HK_OVERRIDE { return true; }

		virtual void updateSync( const hkbContext& context ) HK_OVERRIDE;

		//////////////////////////////////
		// hkbTransitionEffect interface
		//////////////////////////////////

		virtual bool isDone() HK_OVERRIDE;

		virtual void setParentStateMachine( const hkbContext& context, const hkbStateMachine& parentStateMachine ) HK_OVERRIDE;

		virtual hkReal getFromGeneratorBlendOutTime() HK_OVERRIDE;

		virtual hkReal getToGeneratorBlendInTime() HK_OVERRIDE;

			// For internal use only
		virtual void computePatchedBindings( const hkbContext& context, const hkbBehaviorGraph& referenceBehavior );

		/// This is called right after setInternalState().  Whereas setInternalState() is usually auto-generated,
		/// setInternalStateUser() is not.
		
		//virtual void setInternalStateUser(	const hkbContext& context,
			//const hkReferencedObject& internalState,
			//hkPointerMap<hkInt16, const hkbNodeInternalStateInfo*>& nodeIdToInternalStateMap ) {}

	public:
		
			/// Child transition effects to select
		hkArray<hkbTransitionEffect*> m_transitionEffects; //+hk.Description("The transition effects that can be selected.")

			/// The index into m_transitionEffect to use
		hkUint8 m_selectedIndex; //+default(0)
								 //+hk.Description("The index of the selected transition effect.")		

			/// A custom selector that chooses the m_selectedIndex
		hkRefPtr<hkbCustomIdSelector> m_indexSelector; //+hk.Description("A pointer to a custom id selector for choosing the transition effect id.")

	protected:

		HKB_BEGIN_INTERNAL_STATE(0);

			/// The index into m_transitionEffect that was chosen.
			/// This can be different than m_selectedIndex if a selector is used or the index is out of range.
		hkUint8 m_currentTransitionEffectIndex; //+nosave

		HKB_END_INTERNAL_STATE();

			/// Select the current TE
		void selectCurrentTransitionEffect( const hkbContext& context );

			/// During the actual transition, the effect cannot be changed.
			/// This points to the selected transition at the time of activation.
		hkbTransitionEffect* m_currentTransitionEffect; //+nosave
		
			/// The state machine in which the effect is being used
		const hkbStateMachine* m_parentStateMachine; //+nosave

			/// The referenced behavior for executing this TE in a different behavior (for global transitions)
		const hkbBehaviorGraph* m_referenceBehavior; //+nosave

	public:
		
		hkbManualSelectorTransitionEffect( hkFinishLoadedObjectFlag flag );
};

#endif // HKB_MANUAL_SELECTOR_TRANSITION_EFFECT_H

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
