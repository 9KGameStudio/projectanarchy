/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VLUA_APIDOC

%nodefaultctor vHavokBehaviorComponent;
%nodefaultdtor vHavokBehaviorComponent;

class vHavokBehaviorComponent : public IVObjectComponent
{
public:
   %rename(Remove) DisposeObject();
   void DisposeObject();
	
   bool IsNodeActive(const char* nodeName);
   bool HasVariable(const char *variableName);
   bool SetFloatVar(const char* variableName, float value);
   float GetFloatVar(const char* variableName);
   bool SetWordVar(const char* variableName, int value);
   int GetWordVar(const char* variableName);
   bool SetBoolVar(const char* variableName, bool value);
   bool GetBoolVar(const char* variableName) const;

   bool TriggerEvent(const char* eventName) const;
   void RegisterEventHandler(const char* eventName);
   bool WasEventTriggered(const char* eventName) const;
   
   // Treat HKV_FAILURE by returning nil
   %exception
   {
     hkvResult result = $action

     if(result == HKV_FAILURE)
     {
       lua_pushnil(L);
       return 1;
     }
   }
   void GetBoneTransform( const char* boneName, hkvVec3& OUTPUT, hkvMat3& OUTPUT);
};

#else

/// \brief Behavior component class (Havok): Object component wrapper class that provides a Behavior functionality for an entity.
class vHavokBehaviorComponent : public IVObjectComponent
{
public:
	
  /// @}
  /// @name Variables management
  /// @{

  /// \brief Gets if the current node is active in the Behavior graph.
  /// \param nodeName The name of the behavior node to be checked.
  /// \returns ture if the node is active, otherwise false.
  boolean IsNodeActive(string nodeName);

  /// \brief Returns whether there is a behavior variable of the specified name
  /// \param variableName the name of the variable
  /// \returns if a variable of this name exists
  bool HasVariable(const char *variableName);

  /// \brief Sets the value of the selected Behavior float variable
  /// \param variableName  Name of the Behavior variable
  /// \param value  value we want to assign to it
  /// \returns true on success otherwise false.
  boolean SetFloatVar(string variableName, number value);

  /// \brief Checks the value of the selected Behavior float variable
  /// \param variableName  Name of the Behavior variable
  float GetFloatVar(const char* variableName) const;

  /// \brief Returns the value of the selected Behavior float variable
  /// \param variableName  Name of the Behavior variable
  float GetFloatVar(const char* variableName);

  /// \brief Sets the value of the selected Behavior word variable
  /// \param variableName  Name of the Behavior variable
  /// \param value  value we want to assign to it
  /// \returns true on success otherwise false.
  boolean SetWordVar(string variableName, number value);
  
  /// \brief Checks the value of the selected Behavior word variable
  /// \param variableName  Name of the Behavior variable
  int GetWordVar(const char* variableName) const;
  
  /// \brief Returs the value of the selected Behavior word variable
  /// \param variableName  Name of the Behavior variable
  int GetWordVar(const char* variableName);

  /// \brief Sets the value of the selected Behavior boolean variable
  /// \param variableName  Name of the Behavior variable
  /// \param value  value we want to assign to it
  /// \returns true on success otherwise false.
  /// \par Example
  ///   \code
  ///  function OnThink(self)
  ///    if Input:IsKeyPressed(Vision.KEY_SPACE) then
  ///      self.Enabled = not self.Enabled
  ///      self.BehaviorCmp:SetBoolVar("EnableAttachment", self.Enabled)
  ///    end
  ///  end
  ///  function OnAfterSceneLoaded(self)
  ///    self.Enabled = false
  ///    self.BehaviorCmp = self:GetComponentOfType("vHavokBehaviorComponent")
  ///    Vision.Assert(self.BehaviorCmp~=nil, "behavior component not found")
  ///    Input:SetKeyAsSingleHit(Vision.KEY_SPACE)
  ///  end
  ///   \endcode
  boolean SetBoolVar(string variableName, boolean value);
  
  /// \brief Checks the value of the selected Behavior bool variable
  /// \param variableName  Name of the Behavior variable
  /// \par Example  
  ///   \code
  ///  function OnAfterSceneLoaded(self)
  ///    self.BehaviorCmp = self:GetComponentOfType("vHavokBehaviorComponent")
  ///    Vision.Assert(self.BehaviorCmp~=nil, "behavior component not found")
  ///    local enabled = self.BehaviorCmp:GetBoolVar("AttachmentEnabled")
  ///    Debug:PrintLine("AttachmentEnabled: "..tostring(enabled))
  ///  end
  ///   \endcode
  boolean GetBoolVar(string variableName);

  /// @}
  /// @name Utilities
  /// @{

  /// \brief Returns world space transform of the selected bone
  /// \param boneName  Name of the selected bone
  /// \par Example  
  ///   \code
  ///  local position, rotMatrix = GetBoneTransform("BoneName")
  ///   \endcode
  /// \returns hkvVec3 outPos, hkvMat3 outRot
  multiple GetBoneTransform(string boneName);

  /// \brief Triggers a Behavior event
  /// \param eventName  Name of the Behavior event
  /// \returns true on success otherwise false.
  boolean TriggerEvent(string eventName);

  /// \brief Registers a Behavior event handler
  /// \param eventName	Name of the Behavior event
  void RegisterEventHandler(string eventName);

  /// \brief Checks if a given event was triggered
  /// \param eventName	Name of the Behavior event
  boolean WasEventTriggered(string eventName);

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
