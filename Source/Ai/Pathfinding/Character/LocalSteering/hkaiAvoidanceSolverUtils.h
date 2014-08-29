/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_AI_AVOIDANCE_SOLVER_UTILS_H
#define HK_AI_AVOIDANCE_SOLVER_UTILS_H

#include <Ai/Pathfinding/hkaiBaseTypes.h>
#include <Ai/Pathfinding/Character/LocalSteering/hkaiAvoidanceSolver.h>
#include <Common/Base/Types/Geometry/Aabb/hkAabb.h>

class hkaiCharacter;
class hkaiObstacleCollector;
class hkaiObstacleGenerator;
struct hkaiStreamingCollectionInstanceInfo;

/// Set of utility methods for hkaiAvoidanceSolver
class HK_EXPORT_AI hkaiAvoidanceSolverUtils
{
	public:

		/// Set up all the data to solve local avoidance for one character
		static void HK_CALL setupAgent(const hkaiLocalSteeringInput& steeringInput,
			const hkaiStreamingCollectionInstanceInfo* sectionInfo,
			hkaiPackedKey navMeshFaceKey, hkReal erosionRadius,
			const hkArrayBase< const hkaiCharacter* >& neighbors, 
			const hkArrayBase< const hkaiObstacleGenerator* >& obstacles, 
			const hkaiAvoidancePairProperties* avoidancePairProps, 
			hkaiAvoidanceSolver::SteeringAgent& agentOut, 
			hkaiObstacleCollector& collector);

		/// Apply simple kinematic and dynamic constraints to the desired velocity
		/// \param[out] constrainedVelocity Resulting velocity after kinematic constraints are applied
		static void HK_CALL applyKinematicConstraints(	hkSimdRealParameter timestep, 
			const hkaiAvoidanceSolver::MovementProperties& movementLimits, 
			hkVector4Parameter up, hkVector4Parameter oldVelocity, hkVector4Parameter oldForward, 
			hkVector4Parameter desiredVelocity, hkVector4& constrainedVelocity, bool limitEnabled = true);

		/// Calculate the resulting velocity for the agent to avoid the given set of obstacles and agents.
		/// \param[out] steerVelocityOut Resulting linear and angular velocity
		/// \param[out] steerForwardOut Resulting forward direction, perpendicular to up
		static hkResult HK_CALL calcSteeringVelocity(
			hkSimdRealParameter timestep, 
			hkVector4Parameter up, 
			const hkaiAvoidanceSolver::SteeringAgent& agentIn, 
			hkVector4& steerVelocityOut,
			hkVector4& steerForwardOut);

		/// Calculate the resulting velocity from the desired velocity, without applying any avoidance.
		/// If steeringInput.m_applyKinematicConstraints is true, this call will apply kinematic constraints and angular acceleration limits.
		/// \param[out] steerVelocityOut Resulting linear and angular velocity
		/// \param[out] steerForwardOut Resulting forward direction, perpendicular to up
		static void HK_CALL calcUnsteeredVelocity(
			hkSimdRealParameter timestep, 
			hkVector4Parameter up, 
			const hkaiAvoidanceSolver::MovementProperties& movementProperties, 
			const hkaiLocalSteeringInput& steeringInput, 
			hkVector4& steerVelocityOut,
			hkVector4& steerForwardOut);

		/// Fill angular velocity (measured in terms of the angle between the forward direction and the new velocity direction)
		/// into the W component of the velocity vector. If movementProperties.m_useAngularConstraints is true, this will 
		/// limit angular velocity and acceleration.
		static void HK_CALL calcAngularVelocityAndForward(
			hkVector4Parameter currentVelocity,
			hkVector4Parameter newVelocity, 
			hkVector4Parameter forward, 
			hkVector4Parameter up, 
			hkaiAvoidanceSolver::MovementProperties const& movementProperties,
			hkSimdRealParameter timestep,
			hkSimdReal & angularVelocity,
			hkVector4& steerForward);

		static bool HK_CALL clipSegmentInsideAabb( const hkAabb& aabb, hkVector4& startOut, hkVector4& endOut);

		// This method computes the collision time for boundary obstacle in local coordinate system of the agent
		static hkReal HK_CALL intersectVelLine( const hkVector4& vel, const hkVector4& start, const hkVector4& end, const hkVector4& up);

		// This method calculates 2x2 determinant (p1*q2-p2*q1) according to input up vector
		static hkReal HK_CALL determinant2d(const hkVector4& p, const hkVector4& q, const hkVector4& up); 


};

#endif // HK_AI_AVOIDANCE_SOLVER_UTILS_H

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
