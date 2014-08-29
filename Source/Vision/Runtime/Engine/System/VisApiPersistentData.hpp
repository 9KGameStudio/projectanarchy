/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef DEFINE_VISAPIPERSISTENTDATA
#define DEFINE_VISAPIPERSISTENTDATA

#ifndef cJSON__h
#include <Vision/Runtime/Base/ThirdParty/cJSON/cJSON.h>
#endif //cJSON__h

/// \class  VisPersistentData_cl
///
/// \brief  Manages persistent serializable data fields accessible from LUA
///
class VisPersistentData_cl
{
public:
  VISION_APIFUNC VisPersistentData_cl();
  ~VisPersistentData_cl() { Clear(); }

  /// \brief Set the value of a string field in persistent data
  /// \param category the persistent data category containing this field ('default' if unspecified)
  /// \param field the string field to create/modify
  /// \param stringVal the value to assign to the field
  /// \return whether the operation succeeded
  VISION_APIFUNC bool SetString(const char *category, const char *field, const char *stringVal);

  /// \brief Set the value of a numerical field in persistent data
  /// \param category the persistent data category containing this field ('default' if unspecified)
  /// \param field the number field to create/modify
  /// \param floatVal the value to assign to the field
  /// \return whether the operation succeeded
  VISION_APIFUNC bool SetNumber(const char *category, const char *field, float floatVal);

  /// \brief Set the value of a boolean field in persistent data
  /// \param category the persistent data category containing this field ('default' if unspecified)
  /// \param field the boolean field to create/modify
  /// \param boolVal the value to assign to the field
  /// \return whether the operation succeeded
  VISION_APIFUNC bool SetBoolean(const char *category, const char *field, bool boolVal);

  /// \brief Gets the current value of the specified persistent data field, or default if it can't be found
  /// \param category the persistent data category containing this field ('default' if unspecified)
  /// \param field the name of the field to retrieve
  /// \param defaultValue the value to return if the field can't be found
  /// \return the value retrieved, or default if not found
  VISION_APIFUNC const char *GetString(const char *category, const char *field, const char *defaultValue);

  /// \brief Gets the current value of the specified persistent data field, or default if it can't be found
  /// \param category the persistent data category containing this field ('default' if unspecified)
  /// \param field the name of the field to retrieve
  /// \param defaultValue the value to return if the field can't be found
  /// \return the value retrieved, or default if not found
  VISION_APIFUNC float GetNumber(const char *category, const char *field, float defaultValue);

  /// \brief Gets the current value of the specified persistent data field, or default if it can't be found
  /// \param category the persistent data category containing this field ('default' if unspecified)
  /// \param field the name of the field to retrieve
  /// \param defaultValue the value to return if the field can't be found
  /// \return the value retrieved, or default if not found
  VISION_APIFUNC bool GetBoolean(const char *category, const char *field, bool defaultValue);

  /// \brief Tests if a specified persistent data field exists
  /// \param category the persistent data category containing this field ('default' if unspecified)
  /// \param field the name of the field to test
  /// \return true if a value exists, false otherwise
  VISION_APIFUNC bool Exists(const char* category, const char* field);

  /// \brief Returns the type of the entry
  /// \param category the persistent data category containing this field ('default' if unspecified)
  /// \param field the name of the field to retrieve
  /// \return "none" if the field does not exist, "boolean" if the field is a boolean value, 
  ///         "string" if the field is a string value and "number" if the field is a number value
  VISION_APIFUNC const char* GetType(const char* category, const char* field);

  /// \brief save the data in the specified category out to disk as \<output dir\>/Data/\<category\>.data
  /// \param category the category to save ('default' if unspecified)
  /// \return whether the operation succeeded
  VISION_APIFUNC bool Save(const char* category);

  /// \brief load the data in the specified category from disk into persistent data
  /// \param category the category to load ('default' if unspecified)
  /// \return whether the operation succeeded
  VISION_APIFUNC bool Load(const char* category); 

  /// \brief save the data from all categories out to disk as \<output dir\>/Data/\<category\>.data files
  /// \return whether the operation succeeded
  VISION_APIFUNC bool SaveAll();

  /// \brief Clears the in memory storage. If the data is not saved it will be lost!
  VISION_APIFUNC void Clear();

private:

  void CheckFilename(const char *&filename) { if (filename == NULL || filename[0] == 0) filename = "default"; }

  cJSON *GetData() { if(!data) data = cJSON_CreateObject(); return data; }

  bool SetValue(const char *category, const char *field, cJSON *val);
  cJSON *GetValue(const char *category, const char *field);

  bool SaveImpl(cJSON *fileData);

  cJSON *data;
};

#endif //DEFINE_VISAPIPERSISTENTDATA

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
