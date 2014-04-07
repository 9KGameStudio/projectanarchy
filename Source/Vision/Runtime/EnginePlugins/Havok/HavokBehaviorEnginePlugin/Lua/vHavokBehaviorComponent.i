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
   void GetBoneTransform( const char* boneName, hkvVec3& outPos, hkvMat3& outRot );
   bool TriggerEvent(const char* eventName) const;
   void RegisterEventHandler(const char* eventName);
   bool WasEventTriggered(const char* eventName) const;
    
   %extend{  
   void TempMeth( hkvVec3 center )
		{
		}
		
	VSWIG_CREATE_CAST(vHavokBehaviorComponent)
   }
};

#else

/// \brief Behavior component class (Havok): Object component wrapper class that provides a Behavior functionality for an entity.
class vHavokBehaviorComponent : public IVObjectComponent
{
public:
	
  /// @}
  /// @name Variables management
  /// @{

  /// \brief Gets if the current node is active in the Behavior graph
  /// \param nodeName The name of the node.
  /// \returns if the node is active
  bool IsNodeActive(const char* nodeName);

  /// \brief Returns whether there is a behavior variable of the specified name
  /// \param variableName the name of the variable
  /// \returns if a variable of this name exists
  bool HasVariable(const char *variableName);

  /// \brief Sets the value of the selected Behavior float variable
  /// \param variableName  Name of the Behavior variable
  /// \param value  value we want to assign to it
  void SetFloatVar(const char* variableName, float value);

  /// \brief Checks the value of the selected Behavior float variable
  /// \param variableName  Name of the Behavior variable
  float GetFloatVar(const char* variableName) const;

  /// \brief Sets the value of the selected Behavior word variable
  /// \param variableName  Name of the Behavior variable
  /// \param value  value we want to assign to it
  void SetWordVar(const char* variableName, int value);
  
  /// \brief Checks the value of the selected Behavior word variable
  /// \param variableName  Name of the Behavior variable
  int GetWordVar(const char* variableName) const;
  
  /// \brief Sets the value of the selected Behavior boolean variable
  /// \param variableName  Name of the Behavior variable
  /// \param value  value we want to assign to it
  void SetBoolVar(const char* variableName, bool value);
  
  /// \brief Checks the value of the selected Behavior bool variable
  /// \param variableName  Name of the Behavior variable
  bool GetBoolVar(const char* variableName) const;

  /// \brief Returns world space transform of the selected bone
  /// \param boneName  Name of the selected bone
  /// \param outPos    Bone's position
  /// \param outRot    Bone's rotation matrix
  void GetBoneTransform(const char* boneName, hkvVec3& outPos, hkvMat3& outRot );

  /// \brief Triggers a Behavior event
  /// \param eventName  Name of the Behavior event
  void TriggerEvent(const char* eventName) const;

  /// \brief Registers a Behavior event handler
  /// \param eventName	Name of the Behavior event
  void RegisterEventHandler(const char* eventName);

  /// \brief Checks if a given event was triggered
  /// \param eventName	Name of the Behavior event
  bool WasEventTriggered(const char* eventName) const;

  /// @}

};

#endif

/*
 * Havok SDK - Base file, BUILD(#20140327)
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
