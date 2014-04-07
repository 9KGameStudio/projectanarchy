/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \brief
///   Class for the persitent storage of data. The global instance in Lua is called PersistentData
/// \par Example
///   \code
///     PersistentData:SetString("myCategory", "myString", "Hello world!")
///   \endcode
class VisPersistentData_cl
{
public:

  /// @name Storing Data
  /// @{

  /// \brief Set the value of a string field in persistent data
  /// \param category the persistent data category containing this field ('default' if unspecified)
  /// \param field the string field to create/modify
  /// \param stringVal the value to assign to the field
  /// \return whether the operation succeeded
  /// \par Example
  ///   \code
  ///     PersistentData:SetString("myCategory", "myString", "Hello world!")
  ///   \endcode
  bool SetString(const char *category, const char *field, const char *stringVal);

  /// \brief Set the value of a numerical field in persistent data
  /// \param category the persistent data category containing this field ('default' if unspecified)
  /// \param field the number field to create/modify
  /// \param numberVal the value to assign to the field
  /// \return whether the operation succeeded
  /// \par Example
  ///   \code
  ///     PersistentData:SetNumber("myCategory", "myFloat", 4.3)
  ///     PersistentData:SetNumber("myFile", "myInt", 2)
  ///   \endcode
  bool SetNumber(const char *category, const char *field, float numberVal);

  /// \brief Set the value of a boolean field in persistent data
  /// \param category the persistent data category containing this field ('default' if unspecified)
  /// \param field the boolean field to create/modify
  /// \param boolVal the value to assign to the field
  /// \return whether the operation succeeded
  /// \par Example
  ///   \code
  ///     PersistentData:SetBoolean("myCategory", "myFlag", true)
  ///   \endcode
  bool SetBoolean(const char *category, const char *field, bool boolVal);
  
  /// @}
  /// @name Accessing Data
  /// @{

  /// \brief Gets the current value of the specified persistent data field, or default if it can't be found
  /// \param category the persistent data category containing this field ('default' if unspecified)
  /// \param field the name of the field to retrieve
  /// \param default the value to return if the feld can't be found
  /// \return the value retrieved, or default if not found
  /// \par Example
  ///   \code
  ///     local myString = PersistentData:GetString("myFile", "myString", "myString could not be found")
  ///   \endcode
  const char *GetString(const char *category, const char *field, const char *default);
  
  /// \brief Gets the current value of the specified persistent data field, or default if it can't be found
  /// \param category the persistent data category containing this field ('default' if unspecified)
  /// \param field the name of the field to retrieve
  /// \param default the value to return if the field can't be found
  /// \return the value retrieved, or default if not found
  /// \par Example
  ///   \code
  ///     local myFloat = PersistentData:GetNumber("myFile", "myFloat", 3.14)
  ///   \endcode
  float GetNumber(const char *category, const char *field, float default);
  
  /// \brief Gets the current value of the specified persistent data field, or default if it can't be found
  /// \param category the persistent data category containing this field ('default' if unspecified)
  /// \param field the name of the field to retrieve
  /// \param default the value to return if the field can't be found
  /// \return the value retrieved, or default if not found
  /// \par Example
  ///   \code
  ///     local myFlag = PersistentData:GetBoolean("myFile", "myFlag", true)
  ///   \endcode
  bool GetBoolean(const char *category, const char *field, bool default);

  /// \brief Tests if a specified persistent data field exists
  /// \param category the persistent data category containing this field ('default' if unspecified)
  /// \param field the name of the field to test
  /// \return true if a value exists, false otherwise
  /// \par Example
  ///   \code
  ///     if PersistentData:Exists("myFile", "myFlag") then
  ///       ...
  ///     end
  ///   \endcode
  bool Exists(const char* category, const char* field);

  /// \brief Returns the type of the entry
  /// \param category the persistent data category containing this field ('default' if unspecified)
  /// \param field the name of the field to retrieve
  /// \return "none" if the field does not exist, "boolean" if the field is a boolean value, 
  ///         "string" if the field is a string value and "number" if the field is a number value
  /// \par Example
  ///   \code
  ///     if PersistentData:GetType("myFile", "myFlag") == "string" then
  ///       local storedString = PersistentData:GetString("myFile", "myFlag", "")
  ///       ...
  ///     end
  ///   \endcode
  const char* GetType(const char* category, const char* field);

  /// @}
  /// @name Saving/Loading
  /// @{
  
  /// \brief save the data in the specified category out to disk as \<output dir\>/Data/\<category\>.data
  /// \param category the category to save ('default' if unspecified)
  /// \return whether the operation succeeded
  bool Save(const char* category);
  
  /// \brief load the data in the specified category from disk into persistent data
  /// \param category the category to load ('default' if unspecified)
  /// \return whether the operation succeeded
  bool Load(const char* category); 
  
  /// \brief save the data from all categories out to disk as \<output dir\>/Data/\<category\>.data files
  /// \return whether the operation succeeded
  bool SaveAll();

  /// @}
};

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
