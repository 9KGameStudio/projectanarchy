/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// /file vHavokCharacterControllerHelpers.hpp

#ifndef vHavokCharacterControllerHelpers_HPP_INCLUDED
#define vHavokCharacterControllerHelpers_HPP_INCLUDED

#include <Physics2012/Utilities/CharacterControl/CharacterProxy/hkpCharacterProxy.h>
#include <Physics2012/Utilities/CharacterControl/CharacterProxy/hkpCharacterProxyListener.h>
#include <Physics2012/Utilities/CharacterControl/StateMachine/hkpCharacterContext.h>

class vHavokCharacterController;

#if defined(HAVOK_SDK_VERSION_MAJOR) && (HAVOK_SDK_VERSION_MAJOR >= 2012)
struct hkSimplexSolverInput;
typedef hkSimplexSolverInput hkpSimplexSolverInput;
#else
struct hkpSimplexSolverInput;
#endif

#if !defined(_VISION_DOC)

#if defined (__SNC__)
#pragma diag_push
#pragma diag_suppress=1011
#endif

class vHavokCharacterPushableProxy : public hkpCharacterProxy, public hkpCharacterProxyListener
{
public:
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_CHARACTER);
	
	vHavokCharacterPushableProxy(const hkpCharacterProxyCinfo& info, const vHavokCharacterController *pOwner, hkReal strength = 1.0f);
	virtual ~vHavokCharacterPushableProxy();

	virtual void processConstraintsCallback(const hkpCharacterProxy* proxy, 
    const hkArray<hkpRootCdPoint>& manifold, hkpSimplexSolverInput& input) HKV_OVERRIDE;
  virtual void objectInteractionCallback(hkpCharacterProxy* proxy, 
    const hkpCharacterObjectInteractionEvent& input, hkpCharacterObjectInteractionResult& output) HKV_OVERRIDE;

	bool handleSteps(const vHavokCharacterController* character, 
    const hkpCharacterInput& input, 
    hkpCharacterOutput& output);
	bool isOnStep() const { return (m_onStepCnt > 0); }

  void clearPushedRigidBodies();
  int getIndexOfPushedRigidBody(const hkpRigidBody* pRigidBody) const;

	void drawDebug(const vHavokCharacterController* character) const;

	hkReal m_strength;	// Relative push strength
	int m_onStepCnt;	// indicates if handleStep() projected the controller onto a step

	class DebugHavokCharacterController* m_visDebug;
  float m_fCurrentTime;

private:
  struct PushedRigidBody
  {
    const hkpRigidBody* m_pRigidBody;
    float m_fTimeStamp;
    hkVector4 m_orgVelocityDir;
    hkSimdReal m_orgVelocityLen;
  };

  hkArray<PushedRigidBody> m_pushedRigidBodies;
};

#if defined (__SNC__)
#pragma diag_pop
#endif

#endif // !_VISION_DOC

#endif // vHavokCharacterController_HPP_INCLUDED

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
