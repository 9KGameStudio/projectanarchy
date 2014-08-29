/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_ROCKETBOX_CHARACTER_CONTROLLER_H
#define HKB_ROCKETBOX_CHARACTER_CONTROLLER_H

#include <Behavior/Behavior/hkbBehavior.h>
#include <Behavior/Behavior/Generator/Modifier/hkbModifierGenerator.h>

extern const class hkClass hkbRocketboxCharacterControllerClass;


class hkbRocketboxCharacterController : public hkbModifier
{
public:
	//+vtable(true)
	//+version(0)
	HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
	HK_DECLARE_REFLECTION();

	hkbRocketboxCharacterController();
	~hkbRocketboxCharacterController();
	hkbRocketboxCharacterController(hkFinishLoadedObjectFlag flag);

protected:
	hkbRocketboxCharacterController(hkbRocketboxCharacterController const& other);

	enum MovementSpeedsEnum
	{
		SL_Walk,
		SL_WalkFast,
		SL_RunSlow,
		SL_Run
	};

public:

	//////////////////////
	// hkbNode interface 
	//////////////////////

	// hkbNode interface implementation.
	virtual void activate( const hkbContext& context ) HK_OVERRIDE;

	// hkbNode interface implementation.
	virtual void update( const hkbContext& context, hkReal timestep ) HK_OVERRIDE;

	// hkbNode interface implementation.
	virtual void handleEvent( const hkbContext& context, const hkbEvent e ) HK_OVERRIDE;

	// hkbNode interface implementation.
	virtual void deactivate( const hkbContext& context ) HK_OVERRIDE;

	// hkbNode interface implementation.
	virtual hkbRocketboxCharacterController* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

	virtual bool isValid(const hkbCharacter* character, hkStringPtr& errorString) const HK_OVERRIDE;

	//////////////////////////
	// hkbModifier interface
	//////////////////////////

	// hkbModifier interface implementation
	virtual void modify( const hkbContext& context, hkbGeneratorOutput& inOut ) HK_OVERRIDE;

private:
	//////////////////////////
	// Graph control logic
	//////////////////////////
	void updateCharacterControls( const hkbContext& context, hkReal timestep );
	void customCharacterUpdate( const hkbContext& context, hkReal timestep );
	void updateState( const hkbContext& context, hkReal timestep );
	void detectStop();
	void detectAutoRapidTurn( const hkbContext& context );
	void calculateCharacterAngle( const hkbContext& context );
	void updateMovementSpeed( const hkbContext& context, hkReal deltaTime );
	void updateAngularSpeed( hkReal deltaTime );
	void updateActivePose( const hkbContext& context );
	void updateAiming( hkReal timestep );
	void handleImmediateStop( const hkbContext& context, const hkbEvent e );
	void recognizePlantedFoot( const hkbContext& context, const hkbEvent e );
	void handleRapidTurn( const hkbContext& context, const hkbEvent e );

	//////////////////////////
	// Helper methods
	//////////////////////////
	void triggerEvent( const hkbContext& context, const hkbEventProperty& event ) const;
	hkReal angleDist( hkReal a, hkReal b ) const;
	hkReal driveValueTo( hkReal currentValue, hkReal targetValue, hkReal speed, hkReal deltaTime ) const;
	bool isNodeActive( const hkbContext& context, const char* nodeName ) const;
	hkReal getSpeedLimit( MovementSpeedsEnum movementSpeed  ) const;

public:
	///////////////
	// Properties
	///////////////

	/// The child node.
	hkRefPtr<hkbGenerator>	m_child;	//+hk.Description("The child node.")

	bool					m_autoTurnsAllowed;			//+hk.Description("Should U-turns be automatically executed")
	int						m_desiredAIMovementMode;	//+hk.Description("Should the character be walking (0), or running (1) by default")

	hkReal					m_effectiveLinearSpeed;		//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
														//+hk.Description("Linear speed the character is moving with")

	hkReal					m_effectiveAngularSpeed;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
														//+hk.Description("Angular speed the character is moving with")

	hkReal					m_effectiveHorizontalAim;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
														//+hk.Description("Horizontal aim output")

	hkReal					m_effectiveVerticalAim;		//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
														//+hk.Description("Vertical aim output")

	hkReal					m_torsoTiltAngle;			//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
														//+hk.Description("Character torso tilt angle")

	hkReal					m_desiredAIMovementSpeed;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
														//+hk.Description("Desired movement speed for AI")

	hkReal					m_currentMaximumSpeed;		//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
														//+hk.Description("Maximum speed available for the AI")

	bool					m_stopRequest;				//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
														//+hk.Description("Was a stop requested")

	int						m_idleToMoveAnimIdx;		//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
														//+hk.Description("Transition animation index")

	hkReal					m_linearSpeed;				//hk.description("Linear speed input.")
	hkReal					m_angularSpeed;				//hk.description("Angular speed input.")
	hkReal					m_horizontalAim;			//hk.description("Horizontal aim input.")
	hkReal					m_verticalAim;				//hk.description("Vertical aim input.")

	bool					m_rotationAllowed;			//hk.description("Can the character rotate.")
	bool					m_poseChangeAllowed;		//hk.description("Can the character change its pose.")
	bool					m_modifyLinearSpeed;		//hk.description("External speed modifier is active.")
	int						m_poseIdx;					//hk.description("Desired pose index.")

	hkbEventProperty		m_leftFootDownEvent;		//hk.description("Event emitted when the character plants its left foot on the ground.")
	hkbEventProperty		m_rightFootDownEvent;		//hk.description("Event emitted when the character plants its right foot on the ground.")
	hkbEventProperty		m_immediateStopEvent;		//hk.description("Event received when we immediately need to stop the character.")
	hkbEventProperty		m_rapidTurnEvent;			//hk.description("Event that makes the character perform a rapid turn.")
	hkbEventProperty		m_changeMovementDirectionEvent;	//hk.description("Event that makes the character perform a movement direction change maneuver.")
	hkbEventProperty		m_changePoseEvent;			//hk.description("Event that makes the character assume a different pose.")
	hkbEventProperty		m_moveEvent;				//hk.description("Event that makes the character start moving.")
	hkbEventProperty		m_stopEvent;				//hk.description("Event that makes the character stop moving.")
	hkbEventProperty		m_characterHasHalted;		//hk.description("Event triggered when the character finally stops moving.")

	hkArray<hkReal>			m_moveVelocities;			//hk.description("Movement velocities.")

private:
	HKB_BEGIN_INTERNAL_STATE(0);
	bool					m_rapidTurnRequest;			//+nosave

	int						m_currPose;					//+nosave
	int						m_prevPose;					//+nosave

	hkReal					m_noVelocityTimer;			//+nosave

	hkReal					m_linearSpeedModifier;		//+nosave

	hkReal					m_characterAngle;			//+nosave

	int						m_plantedFootIdx;			//+nosave

	// Time slice information - used by the modify() method.
	hkReal					m_timeStep;					//+nosave
	HKB_END_INTERNAL_STATE();
};

#endif // HKB_ROCKETBOX_CHARACTER_CONTROLLER_H

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
