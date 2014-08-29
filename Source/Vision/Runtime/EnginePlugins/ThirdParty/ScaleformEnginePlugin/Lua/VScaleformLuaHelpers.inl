/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

bool VScaleform_ConvertScaleformValue(lua_State* L, int argnum, VScaleformValue* pArg)
{
  switch (lua_type(L, argnum))
  {
  case LUA_TNIL:
    pArg->SetNull();
    break;
  case LUA_TBOOLEAN:
    pArg->SetBool(lua_toboolean(L, argnum) != 0);
    break;
  case LUA_TNUMBER:
    pArg->SetNumber(static_cast<float>(lua_tonumber(L, argnum)));
    break;
  case LUA_TSTRING:
    pArg->SetString(lua_tolstring(L, argnum, NULL));
    break;
  default:
    // VScaleformValue*
    {
      VScaleformValue* temp;
      if (!SWIG_IsOK(SWIG_ConvertPtr(L, argnum, (void**)&temp, SWIGTYPE_p_VScaleformValue, 0)))
        return false;
      *pArg = *temp;
    }
    break;
  }

  return true;
}

//-----------------------------------------------------------------------------------
// Generic implementation of Invoke call (may be used by VScaleformMovie and VScaleformValue)

#define VScaleform_Invoke(L, ObjectType) \
  _VScaleform_Invoke<ObjectType>(L, SWIGTYPE_p_ ## ObjectType, #ObjectType)

template<class ObjectType>
int _VScaleform_Invoke(lua_State *L, swig_type_info* pType, const char* szTypeName)
{
  const int iArgCount = lua_gettop(L);
  if (iArgCount < 2)
  {
    SWIG_push_fail_check_num_args_info(L, "Invoke", 2, 99, iArgCount);
    lua_error(L); 
    return 0;
  }

  ObjectType* pObject;
  if (!SWIG_isptrtype(L, 1) || !SWIG_IsOK(SWIG_ConvertPtr(L, 1, (void**)&pObject, pType, 0))) 
  {
    SWIG_push_fail_arg_info(L, "Invoke", 1, szTypeName, SWIG_Lua_typename(L, 1));
    lua_error(L); 
    return 0;
  }
  if (VTraits::IsBaseOf<VTypedObject, ObjectType>::value && pObject == NULL)
  {
    SWIG_push_fail_arg_info(L, "Invoke", 1, szTypeName, "deleted native object");
    lua_error(L); 
    return 0;
  }

  const char* szMethodName = lua_tostring(L, 2);

  if(!szMethodName)
  {
    SWIG_push_fail_arg_info(L, "Invoke", 2, "string", SWIG_Lua_typename(L, 2));
    lua_error(L); 
    return 0;
  }

  const int iMethodArgCount = iArgCount - 2;
  VScaleformValue returnValue;
  if (iMethodArgCount > 0)
  {
    VScaleformValue* pMethodArgs = new VScaleformValue[iMethodArgCount];
    for (int i = 0; i < iMethodArgCount; i++)
    {
      if (!VScaleform_ConvertScaleformValue(L, 3 + i, &pMethodArgs[i]))
      {
        delete [] pMethodArgs;
        SWIG_push_fail_arg_info(L, "Invoke", 3 + i, "VScaleformValue", SWIG_Lua_typename(L, 3 + i));
        lua_error(L); 
        return 0;
      }
    }

    returnValue = pObject->Invoke(szMethodName, pMethodArgs, static_cast<unsigned int>(iMethodArgCount));
    delete [] pMethodArgs;
  }
  else
  {
    returnValue = pObject->Invoke(szMethodName, NULL, 0);
  }

  VScaleformValue* pReturnValue = new VScaleformValue(returnValue);
  SWIG_NewPointerObj(L, pReturnValue, SWIGTYPE_p_VScaleformValue, 1 /* Let Lua delete the return value */);
  return 1;
}

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
