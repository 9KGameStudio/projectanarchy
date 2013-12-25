/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef __VISION_SIMPLESKELETALANIMATEDOBJECT_HPP
#define __VISION_SIMPLESKELETALANIMATEDOBJECT_HPP

#include <Vision/Runtime/Engine/Animation/VisApiAnimLayerMixerNode.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiAnimNormalizeMixerNode.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiAnimBoneModifierNode.hpp>

// ***********************************************************************************************
// Class: SimpleSkeletalAnimatedObject_cl
// Copyright (C) Havok.com Inc. All rights reserved.
// ***********************************************************************************************
// 
// The SimpleSkeletalAnimatedObject_cl class covers the skeletal animation topic and shows you how
// to start skeletal animations, blend skeletal animation, layer skeletal animations, listen to
// animation events, set the animation time manually and apply forward kinematics to a bone.
//
// ***********************************************************************************************

class SimpleSkeletalAnimatedObject_cl : public VisBaseEntity_cl
{
public:
  V_DECLARE_SERIAL( SimpleSkeletalAnimatedObject_cl, )
  IMPLEMENT_OBJ_CLASS(SimpleSkeletalAnimatedObject_cl);

  enum SampleMode_e
  {
    MODE_SINGLEANIM,
    MODE_BLENDTWOANIMS,
    MODE_LISTENTOEVENTS,
    MODE_SETANIMATIONTIME,
    MODE_LAYERTWOANIMATIONS,
    MODE_FORWARDKINEMATICS,
    MODE_LOOKAT
  };

  // constructor
  SimpleSkeletalAnimatedObject_cl(void);

  // Toggles visibility and set/clears the animation mode
  void SetActivate(bool bStatus);

  // Sample Configuration
  void SetMode(SampleMode_e newMode);
  SampleMode_e GetMode() const { return m_eSampleMode; }

  // Public Accessor Functions
  VisAnimNormalizeMixerNode_cl *GetNormalizeMixerNode() const { return m_spNormalizeMixerNode; }
  VisAnimLayerMixerNode_cl *GetLayerMixerNode() const { return m_spLayerMixerNode; }
  VisSkeletalAnimControl_cl *GetSingleAnimControl() const { return m_spSingleAnimControl; }
  VisAnimBoneModifierNode_cl *GetBoneModifiedNode() const { return m_spBoneModifierNode; }
  int GetNeckBoneIndex() const { return m_iNeckBoneIndex; }

  void UpdateLookatHeadRotation(float fTimeDelta);
  bool GetHeadRotation(const hkvVec3 &vTargetPos, hkvQuat &targetRot, float fMaxViewAngle);

  // functions for special modes
  void SetBlendWalkToRun(bool bDirection);

  // VisBaseEntity_cl overrides
  virtual void InitFunction() HKV_OVERRIDE;
  virtual void DeInitFunction() HKV_OVERRIDE;
  virtual void ThinkFunction() HKV_OVERRIDE;
  virtual void MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB) HKV_OVERRIDE;

protected:
  // sample mode functions
  void StartSingleAnimation(bool bLooped=false);
  void BlendTwoAnimations();
  void ListenToEvents();
  void SetAnimationTime();
  void LayerTwoAnimations();
  void ForwardKinematics();
  void LookAt();

  // Clears the smart pointers for the publicly available data
  // (internally called before a new mode is set)
  void ClearData();

private:
  // private data
  VSmartPtr<VisAnimNormalizeMixerNode_cl> m_spNormalizeMixerNode;
  VSmartPtr<VisAnimLayerMixerNode_cl> m_spLayerMixerNode;
  VSmartPtr<VisSkeletalAnimControl_cl> m_spSingleAnimControl;
  VSmartPtr<VisAnimBoneModifierNode_cl> m_spBoneModifierNode;
  int m_iHeadBoneIndex;
  int m_iNeckBoneIndex;
  int m_iMixerInputWalk, m_iMixerInputRun;

  VString m_sHighlightedBoneName;
  float m_fBoneHighlightDuration;
  int m_iAnimEventFootStepLeft;
  int m_iAnimEventFootStepRight;

  SampleMode_e m_eSampleMode;

  // lookat specific
  VisBaseEntity_cl *m_pLookAtTarget;
  float m_fLookAtRotationPhase;
  hkvMat3 m_RelativeHeadOrientation;
  hkvQuat m_CurrentNeckRotation;
};

#endif //__VISION_SIMPLESKELETALANIMATEDOBJECT_HPP

/*
 * Havok SDK - Base file, BUILD(#20131218)
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2013
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
