/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/VScriptIncludes.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VisionLuaModule_wrapper.hpp>

#include <Vision/Runtime/Base/System/DisableStaticAnalysis.hpp>


#define VLUA_TYPE_VARCHIVE		"VArchive"
#define VLUA_TTYPEDOBJECT     99

void LUA_CreateVArchive(lua_State* L, VArchive* pArchive)
{
  VArchive** ppAr = (VArchive**) lua_newuserdata(L,sizeof(VArchive*));
  luaL_getmetatable(L,VLUA_TYPE_VARCHIVE); //set the metatable of the userdata
  lua_setmetatable(L,-2);
  
  *ppAr = pArchive;
}

VArchive* LUA_GetVArchive(lua_State* L, int i)
{
  if (!LUA_TestUserData(L,i,VLUA_TYPE_VARCHIVE)) 
  {
    luaL_typerror(L,i,VLUA_TYPE_VARCHIVE);
    return NULL;
  }
  
  VArchive** ppAr = (VArchive**) lua_touserdata(L,i);
  
  return *ppAr;
}


static int VArchive_IsLoading(lua_State* L)
{
  VArchive *pArchive = LUA_GetVArchive(L,1);
 
  if (pArchive) {
    lua_pop(L,1); //remove value
    int iLoading = (int)pArchive->IsLoading();

    lua_pushboolean(L, iLoading);
    return 1;
  }
 
  return 0;
}

static int VArchive_IsSaving(lua_State* L)
{
  VArchive *pArchive = LUA_GetVArchive(L,1);
 
  if (pArchive) {
    lua_pop(L,1); //remove value
    int iSaving = (int)pArchive->IsSaving();

    lua_pushboolean(L, iSaving);
    return 1;
  }
 
  return 0;
}

static int VArchive_WriteNumber(lua_State* L)
{
  VArchive *pArchive = LUA_GetVArchive(L,1);

 if(pArchive != NULL)
 {
   lua_Number value = lua_tonumber(L, 2);
   lua_pop(L, 2); //remove values
   *pArchive << LUA_TNUMBER;
   *pArchive << value;
 }
 
  return 0;
}

static int VArchive_ReadNumber(lua_State* L)
{
  VArchive *pArchive = LUA_GetVArchive(L,1);
 
  if (pArchive) {
    lua_pop(L,1); //remove value
    lua_Number value;
    *pArchive >> value;

    lua_pushnumber(L, value);
    return 1;
  }
 
  return 0;
}

static int VArchive_WriteString(lua_State* L)
{
  VArchive *pArchive = LUA_GetVArchive(L,1);

  if (pArchive != NULL) {
    const char * pszString = lua_tostring(L, 2);
    *pArchive << LUA_TSTRING;
    *pArchive << pszString;
    lua_pop(L,2); //remove values
  }
 
  return 0;
}

static int VArchive_ReadString(lua_State* L)
{
  VArchive *pArchive = LUA_GetVArchive(L,1);
 
  if (pArchive) {
    lua_pop(L,1); //remove value
    VString sString;
    *pArchive >> sString;

    lua_pushstring(L, sString.AsChar());

    return 1;
  }
 
  return 0;
}

static int VArchive_WriteBool(lua_State* L)
{
  bool ARGS_OK = true;
  VArchive *pArchive = LUA_GetVArchive(L,1);
  if (pArchive != NULL && ARGS_OK) {
    int iBool = lua_toboolean(L, 2);
    lua_pop(L,2); //remove values
    *pArchive << LUA_TBOOLEAN;
    *pArchive << iBool;
  }
 
  return 0;
}

static int VArchive_ReadBool(lua_State* L)
{
  VArchive *pArchive = LUA_GetVArchive(L,1);
 
  if (pArchive) {
    lua_pop(L,1); //remove value
    int iBool;
    *pArchive >> iBool;

    lua_pushboolean(L, iBool);
    return 1;
  }
 
  return 0;
}

static int VArchive_WriteNil(lua_State* L)
{
  bool ARGS_OK = true;
  VArchive *pArchive = LUA_GetVArchive(L,1);

  VASSERT(pArchive);

  if (pArchive && ARGS_OK && lua_isnil(L,2)) {
    lua_pop(L,2); //remove values
    *pArchive << LUA_TNIL;
  }

  return 0;
}

static int VArchive_ReadNil(lua_State* L)
{
  VArchive *pArchive = LUA_GetVArchive(L,1);

  if (pArchive) {
    lua_pop(L,1); //remove value
    lua_pushnil(L);
    return 1;
  }

  return 0;
}

template <typename T>
static int VArchive_WriteSwigTypeFast(lua_State* L, int iTypeNumber)
{
  VArchive *pArchive = LUA_GetVArchive(L,1);
  
  if (pArchive == NULL) //we cannot write anything to NULL
    return 0;

  T var;
  if (!LUA_GetValue(L, 2, var))
  {
    luaL_argerror (L, 2, "VArchive_Write: Unexpected type on stack index 2");
  }
  else
  {
    lua_pop(L,2); //remove values
    *pArchive << iTypeNumber;
    *pArchive << var;
  }
  return 0;
}

template <>
int VArchive_WriteSwigTypeFast<hkvVec3>(lua_State* L, int iTypeNumber)
{
  VArchive *pArchive = LUA_GetVArchive(L,1);

  if (pArchive == NULL) //we cannot write anything to NULL
    return 0;

  hkvVec3 var;
  if (!LUA_GetValue(L, 2, var))
  {
    luaL_argerror (L, 2, "VArchive_Write: Unexpected type on stack index 2");
  }
  else
  {
    lua_pop(L,2); //remove values
    *pArchive << iTypeNumber;
    var.SerializeAsVisVector(*pArchive);
  }
  return 0;
}

template <typename T>
static int VArchive_SerializeXSwigTypeFast(lua_State* L, int iTypeNumber)
{
  VArchive *pArchive = LUA_GetVArchive(L,1);

  if (pArchive == NULL) //we cannot write anything to NULL
    return 0;

  T var;
  if (!LUA_GetValue(L, 2, var))
  {
    luaL_argerror (L, 2, "VArchive_Write: Unexpected type on stack index 2");
  }
  else
  {
    lua_pop(L,2); //remove values
    *pArchive << iTypeNumber;
    SerializeX(*pArchive, var);
  }
  return 0;
}

template <typename T>
static int VArchive_ReadSwigTypeFast(lua_State* L, swig_type_info * pSwigType)
{
  VArchive *pArchive = LUA_GetVArchive(L,1);
  lua_pop(L,1); //remove archive from stack

  if (pArchive == NULL) 
    return 0;

  T v;
  *pArchive >> v;
  T* pInstance = new T(v);
  VSWIG_Lua_NewPointerObj(L, pInstance, pSwigType, VLUA_MANAGE_MEM_BY_LUA);

  return 1;
}

template <>
int VArchive_ReadSwigTypeFast<hkvVec3>(lua_State* L, swig_type_info * pSwigType)
{
  VArchive *pArchive = LUA_GetVArchive(L,1);
  lua_pop(L,1); //remove archive from stack

  if (pArchive == NULL) 
    return 0;

  hkvVec3 v;
  v.SerializeAsVisVector(*pArchive);
  hkvVec3* pInstance = new hkvVec3(v);
  VSWIG_Lua_NewPointerObj(L, pInstance, pSwigType, VLUA_MANAGE_MEM_BY_LUA);

  return 1;
}

#define VArchive_ReadSwigType(L, visionType) VArchive_ReadSwigTypeFast<visionType>(L, SWIGTYPE_p_ ## visionType)

template <typename T>
static int VArchive_DeSerializeXSwigTypeFast(lua_State* L, swig_type_info * pSwigType)
{
  VArchive *pArchive = LUA_GetVArchive(L,1);
  lua_pop(L,1); //remove archive from stack

  if (pArchive == NULL) 
    return 0;

  T v;
  SerializeX(*pArchive, v);
  T* pInstance = new T(v);
  VSWIG_Lua_NewPointerObj(L, pInstance, pSwigType, VLUA_MANAGE_MEM_BY_LUA);

  return 1;
}
#define VArchive_DeSerializeXSwigType(L, visionType) VArchive_DeSerializeXSwigTypeFast<visionType>(L, SWIGTYPE_p_ ## visionType)

static int VArchive_WriteTable(lua_State* L)
{
  VArchive *pArchive = LUA_GetVArchive(L,1);
 
  if (pArchive != NULL && lua_istable(L,2)) {
    *pArchive << LUA_TTABLE;
    
    // write out the size of the table
    int iSize = (int)lua_objlen(L, 2);
    *pArchive << iSize;

    // recursively write out the table elements (note that such a table may only contain serializable keys and values)
    lua_pushnil(L);  // first key
    while (lua_next(L,2) != 0) {  // uses 'key' (at index 3) and 'value' (at index 4)
      // we need to write out both the key and the value

      // get the Write function from VArchive's meta table and put it on the stack
      lua_getmetatable(L, 1);
      lua_getfield(L, 5, "Write");
      // argument 1: the VArchive object itself
      lua_pushvalue(L, 1);
      // argument 2: the key from the current table element
      lua_pushvalue(L, 3);
      // call the function
      lua_call(L, 2, 0);
      
      // redo the above steps for the table value
      lua_getfield(L, 5, "Write");
      lua_pushvalue(L, 1);
      // argument 2: the value
      lua_pushvalue(L, 4);
      lua_call(L, 2, 0);

      // remove the meta table and value; keep 'key' for next iteration
      lua_pop(L, 2);
    }
  }
 
  return 0;
}

static int VArchive_ReadTable(lua_State* L)
{
  VArchive *pArchive = LUA_GetVArchive(L,1);
 
  if (pArchive) {
    // read the table's length
    int iSize;
    *pArchive >> iSize;
    
    // create the resulting table
    lua_newtable(L);

    int i;
    lua_getmetatable(L, 1);
    for (i=0; i<iSize; i++) {
      // we need to write out both the key and the value

      // get the Read function from VArchive's meta table and put it on the stack
      lua_getfield(L, 3, "Read");
      // argument 1: the VArchive object itself
      lua_pushvalue(L, 1);
      // call the function
      lua_call(L, 1, 1);
      
      // redo the above steps for the table value
      lua_getfield(L, 3, "Read");
      lua_pushvalue(L, 1);
      lua_call(L, 1, 1);
      
      // add the key and value pair into the table
      lua_settable(L, 2);
    }

    // remove the meta table from the stack
    lua_pop(L, 1);

    return 1;
  }
 
  return 0;
}

// write the native VTypedObject 
static int VArchive_WriteObject(lua_State* L)
{
  VArchive *pArchive = LUA_GetVArchive(L,1);

  VisTypedEngineObject_cl* pObj = NULL;
  LUA_GetValue(L, 2, pObj);

  if (pArchive != NULL)
  {
    *pArchive << VLUA_TTYPEDOBJECT;
    pArchive->WriteObject(pObj);
  }

  return 0;
}

// write the native VTypedObject 
static int VArchive_ReadObject(lua_State* L)
{
  VArchive *pArchive = LUA_GetVArchive(L,1);
  lua_pop(L,1); //remove archive from stack

  if (pArchive)
  {
    VisTypedEngineObject_cl* pObj = pArchive->ReadObject<VisTypedEngineObject_cl>();
    if (pObj)
    {
      VScriptComponent* pComp = VScriptResourceManager::GetScriptComponent(pObj);
      LUA_PushObjectProxy(L,pComp);
      return 1;  
    }
  }
  return 0;
}


// Generic write function for any (primitive) LUA type
static int VArchive_Write(lua_State* L)
{
  int iType = lua_type(L,2);

  if (iType == LUA_TTABLE)
  {
    VArchive_WriteTable(L);
    return 0;
  }
  if (iType == LUA_TNUMBER)
  {
    VArchive_WriteNumber(L);
    return 0;
  }
  if (iType == LUA_TSTRING)
  {
    VArchive_WriteString(L);
    return 0;
  }
  if (iType == LUA_TBOOLEAN)
  {
    VArchive_WriteBool(L);
    return 0;
  }
  if (iType == LUA_TNIL)
  {
    VArchive_WriteNil(L);
    return 0;
  }

  //check userdata
  swig_lua_userdata *pUserData = NULL;
  if(lua_isuserdata(L,2))
  {
    pUserData = (swig_lua_userdata *)lua_touserdata(L,2);
  }

  if (LUA_TestUserData(L,2,"hkvVec2") )
  {
    VArchive_WriteSwigTypeFast<hkvVec2>(L, VLUA_TVECTOR2);
    return 0;
  }
  if (LUA_TestUserData(L,2,"hkvVec3") )
  {
    VArchive_WriteSwigTypeFast<hkvVec3>(L, VLUA_TVECTOR3);
    return 0;
  }
  if (LUA_TestUserData(L,2,"hkvVec4") )
  {
    VArchive_WriteSwigTypeFast<hkvVec4>(L, VLUA_TVECTOR4);
    return 0;
  }
  if (LUA_TestUserData(L,2,"hkvMat3") )
  {
    VArchive_WriteSwigTypeFast<hkvMat3>(L, VLUA_TMATRIX3);
    return 0;
  }
  if (LUA_TestUserData(L,2,"hkvMat4") )
  {
    VArchive_WriteSwigTypeFast<hkvMat4>(L, VLUA_TMATRIX4);
    return 0;
  }
  if (LUA_TestUserData(L,2,"hkvPlane") )
  {
    VArchive_SerializeXSwigTypeFast<hkvPlane>(L, VLUA_TPLANE);
    return 0;
  }
  if (LUA_TestUserData(L,2,"hkvQuat") )
  {
    VArchive_WriteSwigTypeFast<hkvQuat>(L, VLUA_TQUATERNION);
    return 0;
  }
  if (LUA_TestUserData(L,2,"hkvAlignedBBox") )
  {
    VArchive_SerializeXSwigTypeFast<hkvAlignedBBox>(L, VLUA_TBOUNDINGBOX);
    return 0;
  }
  if (LUA_TestUserData(L,2,"hkvBoundingSphere") )
  {
    VArchive_WriteSwigTypeFast<hkvBoundingSphere>(L, VLUA_TBOUNDINGSPHERE);

    return 0;
  }
  if (LUA_TestUserData(L,2,"VColorRef") )
  {
    VArchive_WriteSwigTypeFast<VColorRef>(L, VLUA_TCOLOR);
    return 0;
  }
  if (pUserData != NULL && pUserData->type &&
    //currently we have no support for these data types:
    !LUA_TestUserData(L,2,"VBitmask") )
  {
    //at this point we should have something derived from VisTypedEngineObject_cl
    //(but it's still possible to corrupt the system, eg. by serializing the VSoundManager or similar stupid things...)
    VArchive_WriteObject(L);
    return 0;
  }

  //TODO: add any new types here

  // Check if there is a user data serializer registered with the script manager
  VArchive* pArchive = LUA_GetVArchive(L, 1);
  VScriptUserDataSerializeObject Obj(&VScriptResourceManager::OnUserDataSerialize, pArchive, L, -1);
  if(pArchive)
    Obj.Trigger();

  if(!Obj.bProcessed)
  {
    char errMsg[256];
    const char *pszTypeName = lua_typename(L, iType);

    const char * pszSwigType = (pUserData != NULL && pUserData->type) ? pUserData->type->str : "no swig type";
    sprintf(errMsg, "The following type is not serializable: %s (%s)", pszTypeName, pszSwigType);

    luaL_argerror(L, 2, errMsg);
  }
 
 
  return 0;
}

// Generic read function for any (primitive) LUA type
static int VArchive_Read(lua_State* L)
{
  VArchive *pArchive = LUA_GetVArchive(L,1);
 
  if (pArchive) {
    int iType;
    *pArchive >> iType;

    switch(iType)
    {
      case VLUA_TTYPEDOBJECT:     return VArchive_ReadObject(L);
      case LUA_TTABLE:            return VArchive_ReadTable(L);
      case LUA_TNUMBER:           return VArchive_ReadNumber(L);
      case LUA_TSTRING:           return VArchive_ReadString(L);
      case LUA_TBOOLEAN:          return VArchive_ReadBool(L);
      case VLUA_TVECTOR2:         return VArchive_ReadSwigType(L, hkvVec2);
      case VLUA_TVECTOR3:         return VArchive_ReadSwigType(L, hkvVec3);
      case VLUA_TVECTOR4:         return VArchive_ReadSwigType(L, hkvVec4);
      case VLUA_TMATRIX3:         return VArchive_ReadSwigType(L, hkvMat3);
      case VLUA_TMATRIX4:         return VArchive_ReadSwigType(L, hkvMat4);
      case VLUA_TPLANE:           return VArchive_DeSerializeXSwigType(L, hkvPlane);
      case VLUA_TQUATERNION:      return VArchive_ReadSwigType(L, hkvQuat);
      case VLUA_TBOUNDINGBOX:     return VArchive_DeSerializeXSwigType(L, hkvAlignedBBox);
      case VLUA_TBOUNDINGSPHERE:  return VArchive_ReadSwigType(L, hkvBoundingSphere);
      case VLUA_TCOLOR:           return VArchive_ReadSwigType(L, VColorRef);
      case LUA_TNIL:              return VArchive_ReadNil(L);
      default:
        {
          // Trigger the callback for the user data serialize function
          ANALYSIS_IGNORE_WARNING_ONCE(6246)
          VArchive* pArchive = LUA_GetVArchive(L, 1);
          VScriptUserDataSerializeObject Obj(&VScriptResourceManager::OnUserDataSerialize, pArchive, L, iType);
          if(pArchive)
            Obj.Trigger();

          // If the userdata type was not processed by one of the callback listeners
          if(!Obj.bProcessed)
          {
            char errMsg[256];
            const char *pszTypeName = iType >= 0 ? lua_typename(L, iType) : "<Unknown Type>";
            sprintf(errMsg, "Found an unknown type number in the archive: %s", pszTypeName);
            luaL_argerror(L, 2, errMsg);
          }
        }
    }
  }
 
  return 0;
}

static const luaL_reg VArchive_methods[] =
{
  { "IsLoading", VArchive_IsLoading },
  { "IsSaving", VArchive_IsSaving },
  { "Write", VArchive_Write },
  { "Read", VArchive_Read },

  { NULL, NULL }
};


void VLUA_OpenLibraryVArchive(lua_State* L)
{
                                           //stack: ..., TOP
  LUA_NewMetatable(L, VLUA_TYPE_VARCHIVE); //stack: ..., metatable, TOP

  //Add all the methods for this class
  const luaL_Reg* pReg = VArchive_methods;
  LUA_RegMetatable(L, pReg);               //stack: ..., metatable, TOP

  lua_pop(L, 1);                           //stack: ..., TOP
}


#ifdef VLUA_APIDOC

/// \brief Provides support for serialization (saving and loading) of basic data types in Lua like numbers, strings, tables, vectors, etc.
class VArchive {
public:

  /// \brief Returns whether the object must be saved or loaded.
  /// \return true = object is being loaded; false = object is being saved.
  /// \see VisTypedEngineObject_cl::OnSerialize
  boolean IsLoading();

  /// \brief Returns whether the object must be loaded or saved.
  /// \return true = object is being saved; false = object is being loaded.
  /// \see VisTypedEngineObject_cl::OnSerialize
  boolean IsSaving();

  /// \brief Writes data of various types out to disk. Both standard Lua types are supported and engine native object proxies
  /// \param data The data to write.
  /// \note In case a native object proxy is passed (e.g. entity) it will use full native serialization of the object.
  /// \par Example
  ///   \code
  /// function OnSerialize(self, ar)
  ///   if (ar:IsLoading()) then
  ///     local s = ar:Read()
  ///   else
  ///     ar:Write("Hello, Lua")
  ///   end
  /// end
  ///   \endcode
  /// \see VisTypedEngineObject_cl::OnSerialize
  void Write(mixed data);

  /// \brief Reads one piece of data from disk. The returned object can be a basic type (string, number,..) or even a fully de-serialized native object proxy (e.g. entity)
  /// \return The piece of data that was read.
  /// \note Native engine objects can be fully (de-)serialized and the object association is even shared with the parent native archive
  /// \par Example
  ///   \code
  /// function OnSerialize(self, ar)
  ///   if (ar:IsLoading()) then
  ///     local s = ar:Read()
  ///   else
  ///     ar:Write("Hello, Lua")
  ///   end
  /// end
  ///   \endcode
  /// \see VisTypedEngineObject_cl::OnSerialize
  mixed Read();

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
