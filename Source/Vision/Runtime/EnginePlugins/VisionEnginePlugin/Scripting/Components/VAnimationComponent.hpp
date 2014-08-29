/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VAnimationComponent.hpp

#ifndef __VANIMAION_COMPONENT_HPP
#define __VANIMAION_COMPONENT_HPP
#ifndef _VISION_DOC
//since this component is used only internally
//for the scripting system we do not generate a C++ documentation for it

#include <Vision/Runtime/Engine/SceneElements/VisApiObjectComponent.hpp>

/// 
/// \brief
///   The VAnimationComponent is a facade for the animation system in order to expose a simple interface to the scripting system
/// 
class VAnimationComponent : public IVObjectComponent
{
public: 

  ///
  /// @name Constructor
  /// @{
  ///

  ///
  /// \brief
  ///   Component Constructor
  /// 
  /// \param iComponentFlags
  ///   See IVObjectComponent
  /// 
  /// \see
  ///   IVObjectComponent
  /// 
  SCRIPT_IMPEXP VAnimationComponent(int iComponentFlags=VIS_OBJECTCOMPONENTFLAG_NOSERIALIZE);

  /// 
  /// \brief
  ///   Component Destructor
  /// 
  SCRIPT_IMPEXP virtual ~VAnimationComponent();

  ///
  /// @}
  ///

  /// \brief
  ///   Play the animation from the beginning.
  SCRIPT_IMPEXP bool Play(const char * szAnimationName, bool bLoop = true, const char * szEndEventName = NULL, bool bEndEventOnce = true);

  /// \brief
  ///   Continue animation playback.
  SCRIPT_IMPEXP bool Resume(const char * szAnimationName = NULL, bool bLoop = true, const char * szEndEventName = NULL, bool bEndEventOnce = true);

  /// \brief
  ///   Adds the specified animation sequence to the sequence set collection
  SCRIPT_IMPEXP bool AddAnimationSequence(const char * szAnimSequence);

  /// \brief
  ///   Stops animation playback and performs a reset.
  ///   Following Resume calls will start the animation from the beginning.
  SCRIPT_IMPEXP bool Stop();

  /// \brief
  ///   Pause animation playback. Call resume to continue from this point.
  SCRIPT_IMPEXP bool Pause();

  /// \brief
  ///   Set (or jump to) time in current animation (absolute between 0 and DURATION or relative between 0 and 1).
  SCRIPT_IMPEXP bool SetTime(float fTime, bool bRelativeTime = false);

  /// \brief
  ///   Get time in current animation.
  ///   Absolute time (bRelativeTime == false) returns a value between [0, DURATION]
  ///   Relative time returns [0,1]
  ///   If no control is currently active, the value returned is -1.f.
  SCRIPT_IMPEXP float GetTime(bool bRelativeTime = false);

  /// \brief
  ///   Set playback speed for current animation.
  ///   \param fSpeedFactor 1.0 indicates normal forward playback.
  ///   The fSpeedFactor can also be set to negatives to reverse playback.
  ///   For example, fSpeedFactor = -2.0f will make the animation play 2x speed in reverse.
  SCRIPT_IMPEXP void SetSpeed(float fSpeedFactor);

  /// \brief
  ///   Get playback speed for current animation.
  ///   If no control is currently active, the value returned is 0.
  SCRIPT_IMPEXP float GetSpeed();
  
  /// \brief
  ///   Get the number of present animation sequences
  SCRIPT_IMPEXP int GetNumSequences();

  /// \brief
  ///   Returns true if the animation playback is running, false if paused and false if no anim controller exists.
  ///   Please note this behavior is NOT simply the opposite of IsPaused.
  SCRIPT_IMPEXP inline bool IsPlaying() { return m_pAnimCtrl && m_pAnimCtrl->IsPlaying(); }

  /// \brief
  ///   Returns false if the animation playback is running, true if paused and false if no anim controller exists.
  ///   Please note this behavior is NOT simply the opposite of IsPlaying.
  SCRIPT_IMPEXP inline bool IsPaused() { return m_pAnimCtrl && !m_pAnimCtrl->IsPlaying(); }

  /// \brief
  ///   Returns the name of the currently active animation or NULL
  SCRIPT_IMPEXP inline const char * GetCurrentAnimation() { return m_pAnimCtrl?m_pAnimCtrl->GetAnimSequence()->GetName():NULL; }

  ///
  /// @name IVObjectComponent Overrides
  /// @{
  ///

  SCRIPT_IMPEXP virtual BOOL CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut);

  SCRIPT_IMPEXP virtual void SetOwner(VisTypedEngineObject_cl *pOwner) HKV_OVERRIDE;

  V_DECLARE_SERIAL(VAnimationComponent, SCRIPT_IMPEXP)

  V_DECLARE_VARTABLE(VAnimationComponent, SCRIPT_IMPEXP)

  SCRIPT_IMPEXP virtual void Serialize( VArchive &ar );

  ///
  /// @}
  ///

protected:

  /// \brief
  ///    Add an animation event to the end of the current animation
  void AddEndEvent(const char * szEventName, bool bAutoRemove);

  VisAnimControl_cl *m_pAnimCtrl;
  VString m_sCurrentAnim;
};

#endif // _VISION_DOC
#endif // __ANIMAION_COMPONENT_HPP

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
