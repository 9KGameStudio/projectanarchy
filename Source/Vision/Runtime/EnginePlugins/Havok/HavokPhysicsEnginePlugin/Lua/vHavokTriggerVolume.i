/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifdef VLUA_APIDOC

/// \brief Physics module class (Havok): 
///   Provides Lua scripts, that are attached to custom volume object with a vHavokTriggerVolume component,
///   with the below described trigger volume callbacks. Please note that this is not a wrapper class.
class vHavokTriggerVolume : public IVObjectComponent
{
public:
	
	/// @}
  /// @name Trigger Volume Callbacks
  /// @{

  /// \brief  
  ///   Called when a rigid body enters the trigger volume.
  ///
  /// \param self 
  ///   The custom volume object, to which this script is attached.
  ///
  /// \param object 
  ///   The object, to which the rigid body belongs, which had caused the trigger event.
  ///
  /// \par Example
  ///   \code
  ///     function OnObjectEnter(self, object)
  ///       Debug:PrintLine(tostring(object) .. " entered the trigger volume!")
  ///     end
  ///   \endcode
  void OnObjectEnter(mixed self, mixed object);

  /// \brief  
  ///   Called when a rigid body leaves the trigger volume.
  ///
  /// \param self 
  ///   The custom volume object, to which this script is attached.
  ///
  /// \param object 
  ///   The object, to which the rigid body belongs, which had caused the trigger event.
  ///
  /// \par Example
  ///   \code
  ///     function OnObjectLeave(self, object)
  ///       Debug:PrintLine(tostring(object) .. " left the trigger volume!")
  ///     end
  ///   \endcode
  void OnObjectLeave(mixed self, mixed object);
  
  /// \brief  
  ///   Called when a character controller enters the trigger volume.
  ///
  /// \param self 
  ///   The custom volume object, to which this script is attached.
  ///
  /// \param object 
  ///   The object, to which the character controller belongs, which had caused the trigger event.
  ///
  /// \par Example
  ///   \code
  ///     function OnCharacterEnter(self, object)
  ///       Debug:PrintLine(tostring(object) .. " entered the trigger volume!")
  ///     end
  ///   \endcode
  void OnCharacterEnter(mixed self, mixed object);

  /// \brief  
  ///   Called when a character controller leaves the trigger volume.
  ///
  /// \param self 
  ///   The custom volume object, to which this script is attached.
  ///
  /// \param object 
  ///   The object, to which the character controller belongs, which had caused the trigger event.
  ///
  /// \par Example
  ///   \code
  ///     function OnCharacterLeave(self, object)
  ///       Debug:PrintLine(tostring(object) .. " left the trigger volume!")
  ///     end
  ///   \endcode
  void OnCharacterLeave(mixed self, mixed object);

  ///
  /// @}
  ///
  
};

#endif

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
