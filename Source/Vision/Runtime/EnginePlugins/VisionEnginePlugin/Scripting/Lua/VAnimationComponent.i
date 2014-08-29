/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VLUA_APIDOC

%nodefaultctor VAnimationComponent;
%nodefaultdtor VAnimationComponent;
class VAnimationComponent : public IVObjectComponent
{
public:

  bool Play(const char * szAnimationName, bool bLoop=true, const char * szEndEventName = NULL, bool bEndEventOnce = true);
  bool Resume(const char * szAnimationName = NULL, bool bLoop = true, const char * szEndEventName = NULL, bool bEndEventOnce = true);
  bool Stop();
  bool Pause();
  bool AddAnimationSequence(const char * szAnimSequence);
  bool SetTime(float fTime, bool bRelativeTime = false);
  float GetTime(bool bRelativeTime = false);
  bool IsPlaying();
  bool IsPaused();
  const char * GetCurrentAnimation();
  float GetSpeed();
  void SetSpeed(float fSpeedFactor);
  %extend{
    //the following cast is just for our test cases
    VSWIG_CREATE_CAST_UNSAFE(VAnimationComponent)
  }
};

//add lua tostring and concat operators
VSWIG_CREATE_CONCAT(VAnimationComponent, 256, "%s [%s]", self->GetComponentName()==NULL?self->GetClassTypeId()->m_lpszClassName:self->GetComponentName(), self->GetCurrentAnimation())
VSWIG_CREATE_TOSTRING(VAnimationComponent, "%s: %s [%s]", self->GetClassTypeId()->m_lpszClassName, self->GetComponentName(), self->GetCurrentAnimation())

    
//implement GetSequences native becuase it should return an array of strings,
//which is almost impossible with SWIG
%native(VAnimationComponent_GetSequences) int VAnimationComponent_GetSequences(lua_State *L);
%{
  SWIGINTERN int VAnimationComponent_GetSequences(lua_State *L)
  {
    IS_MEMBER_OF(VAnimationComponent) //this will move this function to the method table of the specified class
    
    SWIG_CONVERT_POINTER(L, 1, IVObjectComponent, pSelf)

    lua_newtable(L); //create an empty table (or an array if you would like to see it this way)
    
    VDynamicMesh * pMesh = ((VisBaseEntity_cl *)pSelf->m_pOwner)->GetMesh(); //we know that it is at least a base entity...

    if(pMesh)
    {
      VisAnimSequenceSetCollection_cl * pSequences = pMesh->GetSequenceSetCollection();

      int iOverallCount = 1;

      for(int iSet=0;iSet<pSequences->Count();++iSet)
      {
        VisAnimSequenceSet_cl * pSet = pSequences->GetSequenceSet(iSet);

        for(int j=0;j<pSet->GetSequenceCount();++j)
        {
          const char* szName = pSet->GetSequence(j)->GetName();

          if(szName)
          {
            float fLength = pSet->GetSequence(j)->GetLength();
            
            lua_newtable(L);                    // create a new sub table             stack: table, table, TOP
            
            lua_pushstring(L, "Name");          //push the key                        stack: table, table, key, TOP
            lua_pushstring(L, szName);          //push the value                      stack: table, table, key, value, TOP
            lua_settable(L, -3);                //set key and value as entry          stack: table, table, TOP
            
            lua_pushstring(L, "Length");         //push the key                       stack: table, table, key, TOP
            lua_pushnumber(L, (lua_Number)fLength);     //push the value              stack: table, table, key, value, TOP
            lua_settable(L, -3);                //set key and value as entry          stack: table, table, TOP
            
            lua_rawseti(L, -2, iOverallCount);  //add to overall table                stack: table, TOP
            
            iOverallCount++;
          }
        }
      }
    }
    
    return 1; //the table is always on the stack (even if empty)
  }
%}

#else

/// \brief Lua animation component.
/// \see VisGame_cl::CreateComponent
/// \see VisTypedEngineObject_cl::AddComponentOfType
/// \see VisTypedEngineObject_cl::GetComponentOfType
/// \see VisTypedEngineObject_cl::GetComponentOfBaseType
class VAnimationComponent : public IVObjectComponent {
public:

  /// @name Common Animation Functions
  /// @{
  
  /// \brief Play an animation from the beginning.
  /// \param aniName The name of the skeletal or vertex animation.
  /// \param loop (\b optional) Loop this animation.
  /// \param eventName (\b optional) Add an animation event at the end with the specified name.
  /// \param endEventOnce (\b optional) Remove end event after one execution.
  /// \return true on success, otherwise false.
  /// \note Animation events will be handeld in the OnAnimationEvent callback.
  /// \par Example
  ///   \code
  ///   function OnAnimationEvent(self, eventName, animSeqName)
  ///     Debug:PrintLine(self:GetKey()..": Animation event '" .. eventName .. "' in sequence " .. animSeqName)
  ///   end
  ///   \endcode
  /// \see VTransitionStateMachine::OnAnimationEvent
  boolean Play(string aniName, boolean loop=true, string eventName = nil, boolean endEventOnce = true);

  /// \brief Resume animation from the current position.
  /// \param aniName (\b optional) The name of the skeletal or vertex animation.
  /// \param loop (\b optional) Loop this animation.
  /// \param eventName (\b optional) Add an animation event at the end with the specified name.
  /// \param endEventOnce (\b optional) Remove end event after one execution.
  /// \return true on success, otherwise false.
  /// \note Animation events will be handeld in the OnAnimationEvent callback.
  /// \par Example
  ///   \code
  ///   function OnAnimationEvent(self, eventName, animSeqName)
  ///     Debug:PrintLine(self:GetKey()..": Animation event '" .. eventName .. "' in sequence " .. animSeqName)
  ///   end
  ///   \endcode
  /// \see VTransitionStateMachine::OnAnimationEvent
  boolean Resume(string aniName = nil, boolean loop=true, string eventName = nil, boolean endEventOnce = true);
  
  /// \brief Stop animation an move the current position to the beginning.
  /// \return true on success, otherwise false.
  boolean Stop();
  
  /// \brief Stop animation without modifying the current position (use Resume to continue animation from this point).
  /// \return true on success, otherwise false.
  /// \see Resume
  boolean Pause();
  
  /// \brief Add the specified animation (*.anim) to the internal sequence set collection.
  /// \param animSequence The file containing the sequence.
  /// \return true on success, otherwise false.
  boolean AddAnimationSequence(string animSequence);
  
  /// \brief Set the position within the animation.
  /// \param time The position inside the animation as time in seconds (or from 0 to 1 if relativeTime == true).
  /// \param relativeTime (\b optional) Use relative time for positioning instead of absolute seconds.
  /// \return true on success, otherwise false.
  /// \see Resume
  boolean SetTime(number time, boolean relativeTime = false);
  
  ///  \brief Get time in current animation.
  /// Absolute time, the default behavior, returns a value between [0, DURATION].
  /// Relative time (bRelativeTime == true) returns [0,1].
  /// If no control is currently active, the value returned is -1.f.
  /// \param bRelativeTime (\b optional) Controls scale of returned value.
  float GetTime(bool bRelativeTime = false);

  /// \brief Check the animation state.
  /// \return true if playing, false if paused, and false if no anim controller exists.
  ///  Please note this behavior is NOT simply the opposite of IsPaused.
  boolean IsPlaying();
  
  /// \brief Check the animation state.
  /// \return true if paused, false if playing, and false if no anim controller exists.
  ///  Please note this behavior is NOT simply the opposite of IsPlaying.
  boolean IsPaused();
  
  /// \brief
  ///   Get playback speed for current animation.
  ///   If no control is currently active, the value returned is 0.
  float GetSpeed();

  /// \brief
  ///   Set playback speed for current animation.
  ///   \param fSpeedFactor 1.0 indicates normal forward playback.
  ///   The fSpeedFactor can also be set to negatives to reverse playback.
  ///   For example, fSpeedFactor = -2.0f will make the animation play 2x speed in reverse.
  void SetSpeed(float fSpeedFactor);


  /// \brief Get the name of the current animation.
  /// \return The name of the animation or nil.
  string GetCurrentAnimation();
  
  /// \brief Get all sequences of the current model.
  /// \return A table containing all sequences from index 1 to LEN, where each entry consists of Name and Length.
  /// \par Example
  ///   \code
  ///   local anims = self.Animation:GetSequences()
  ///   for i=1,#anims do
  ///     Debug:PrintLine("Anim Sequence '" .. anims.Name .. "' : " .. anims.Length .. "sec (" .. i .. ")");
  ///   end
  ///   \endcode
  table GetSequences();
  
  /// @}
};

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
