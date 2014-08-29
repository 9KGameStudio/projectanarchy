/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VLUA_APIDOC

%nodefaultctor VTypedObject;
%nodefaultdtor VTypedObject;

%{

// A helper class that binds to an arbitrary Lua value as a function argument. Currently only used by VTypedObject::SetProperty
class VLuaStackRef
{
public:
  /// \brief Constructor. Internal, do not use.
  VLuaStackRef(hkvInit_None) {}

  /// \brief Constructor. Internal, do not use.
  VLuaStackRef(lua_State* L, int iStackIndex) : L(L)
  {
    // Make stack index absolute so it's not invalidated by pushing something else onto the stack
    m_iStackIndex = (iStackIndex > 0 || iStackIndex <= LUA_REGISTRYINDEX) ? iStackIndex : lua_gettop(L) + iStackIndex + 1;
  }

  /// \brief Converts the Lua value to a boolean.
  bool ToBoolean() const
  {
    return lua_toboolean(L, m_iStackIndex) == 1;
  }

  /// \brief Converts the Lua value to an integer.
  lua_Integer ToInteger() const
  {
    return lua_tointeger(L, m_iStackIndex);
  }

  /// \brief Converts the Lua value to a number.
  lua_Number ToNumber() const
  {
    return lua_tonumber(L, m_iStackIndex);
  }

  /// \brief Converts the Lua value to a string.
  const char* ToString() const
  {
    return lua_tostring(L, m_iStackIndex);
  }

  /// \brief Converts the Lua value to an object pointer. Caution, no type check is currently performed.
  template<typename T> const T* ToObject() const
  {
    if(swig_lua_userdata* usr = (swig_lua_userdata*) lua_touserdata(L, m_iStackIndex))
    {
      return static_cast<T*>(usr->ptr);
    }

    return NULL;
  }

private:
  lua_State* L;
  int m_iStackIndex;
};

%}

%typemap(in) const VLuaStackRef& (VLuaStackRef temp(hkvNoInitialization))
{
  temp = VLuaStackRef(L, $argnum);
  $1 = &temp;
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const VLuaStackRef& {
   $1 = 1;
}

class VTypedObject
{
public:
  %extend {

    const char* GetType() const
    {
      return self->GetTypeId()->m_lpszClassName;
    }
    
    bool IsOfType(const char* szType)
    {
      return self->IsOfType(szType) == TRUE;
    }
    
    const char* GetPropertyType(const char* propName)
    {
      if(propName == NULL)
        return NULL; // avoid problems with nil values
      VisVariable_cl* pVar = self->GetVariable(propName);
      int type = pVar ? pVar->type : -1;
      switch (type) {
      case VULPTYPE_STRING:
      case VULPTYPE_VSTRING:
      case VULPTYPE_MODEL:
      case VULPTYPE_PSTRING:
      case VULPTYPE_ENTITY_KEY:
      case VULPTYPE_PRIMITIVE_KEY:
      case VULPTYPE_VERTEX_KEY:
      case VULPTYPE_LIGHTSOURCE_KEY:
      case VULPTYPE_WORLDANIMATION_KEY:
      case VULPTYPE_PATH_KEY:
        return "string";
      case VULPTYPE_ENUM:
      case VULPTYPE_INT:
      case VULPTYPE_FLOAT:
      case VULPTYPE_DOUBLE:
        return "number";
      case VULPTYPE_BOOL:
        return "boolean";
      case VULPTYPE_VECTOR_INT:
      case VULPTYPE_VECTOR_FLOAT:
      case VULPTYPE_VECTOR_DOUBLE:
        return "hkvVec3";
      case VULPTYPE_BYTE_COLOR4:
        return "VColorRef";
      default:
        hkvLog::Warning("Type of property '%s' is unknown in Lua.", propName);
        return NULL;
      }
    }
    
    bool operator == (const VTypedObject & other)
    {
      return self == &other;
    }
    
    
    void GetProperty(const char* pszName, VCaptureSwigEnvironment * env)
    {
      VisVariable_cl* pVar = self->GetVariable(pszName);

      // if variable can't be found by name try with display name
      if(!pVar)
        pVar = self->GetVariableByDisplayName(pszName);

      if(!pVar)
      {
        hkvLog::Warning("Called getter of unknown property '%s'", pszName);
        env->AddReturnValueNil();
        return;
      }
      
      switch(pVar->type)
      {
      case VULPTYPE_STRING:
      case VULPTYPE_MODEL:
      case VULPTYPE_PSTRING:
      case VULPTYPE_ENTITY_KEY:
      case VULPTYPE_PRIMITIVE_KEY:
      case VULPTYPE_VERTEX_KEY:
      case VULPTYPE_LIGHTSOURCE_KEY:
      case VULPTYPE_WORLDANIMATION_KEY:
      case VULPTYPE_PATH_KEY:
      {
        const char* pStr = NULL;
        pVar->GetValueDirect(self, (void*) &pStr, true);
        env->AddReturnValueString(pStr);
      }
      break;
      case VULPTYPE_VSTRING:
      {
        VString vstr;
        pVar->GetValueDirect(self, (void*) &vstr, true);
        env->AddReturnValueString(vstr.AsChar());
      }
      break;
      case VULPTYPE_ENUM:
      case VULPTYPE_INT:
      {
        int n;
        pVar->GetValueDirect(self, (void*) &n, true);
        env->AddReturnValueNumber((lua_Number)n);
      }
      break;
      case VULPTYPE_FLOAT:
      {
        float n;
        pVar->GetValueDirect(self, (void*) &n, true);
        env->AddReturnValueNumber((lua_Number)n);
      }
      break;
      case VULPTYPE_DOUBLE:
      {
        double n;
        pVar->GetValueDirect(self, (void*) &n, true);
        env->AddReturnValueNumber((lua_Number)n);
      }
      break;
      case VULPTYPE_BOOL:
      {
        BOOL b;
        pVar->GetValueDirect(self, (void*) &b, true);
        env->AddReturnValueBool(b != 0);
      }
      break;
      case VULPTYPE_VECTOR_INT:
      {
        int pN[3];
        pVar->GetValueDirect(self, (void*) pN, true);
        hkvVec3 vector((float)pN[0], (float)pN[1], (float)pN[2]);
        SWIG_Lua_NewPodObj(env->GetLuaState(), &vector, SWIGTYPE_p_hkvVec3);
        env->SetNumReturnValues(1);
      }
      break;
      case VULPTYPE_VECTOR_FLOAT:
      {
        float pN[3];
        pVar->GetValueDirect(self, (void*) pN, true);
        hkvVec3 vector(pN[0], pN[1], pN[2]);
        SWIG_Lua_NewPodObj(env->GetLuaState(), &vector, SWIGTYPE_p_hkvVec3);
        env->SetNumReturnValues(1);
      }
      break;
      case VULPTYPE_VECTOR_DOUBLE:
      {
        double pN[3];
        pVar->GetValueDirect(self, (void*) pN, true);
        hkvVec3 vector((float)pN[0], (float)pN[1], (float)pN[2]);
        SWIG_Lua_NewPodObj(env->GetLuaState(), &vector, SWIGTYPE_p_hkvVec3);
        env->SetNumReturnValues(1);
      }
      break;
      case VULPTYPE_BYTE_COLOR4:
      {
        VColorRef color;
        pVar->GetValueDirect(self, (void*) &color, true);
        SWIG_Lua_NewPodObj(env->GetLuaState(), &color, SWIGTYPE_p_VColorRef);
        env->SetNumReturnValues(1);
      }
      break;
      default:
        hkvLog::Warning("Called getter of unknown type %d (%s)", pVar->type, pszName);
        env->AddReturnValueNil();
        break;
      }
    }
    
    void GetProperties(VCaptureSwigEnvironment* env)
    {
      lua_State* L = env->GetLuaState();
      
      int iCount = self->GetNumVariables();
      
      lua_createtable(L, iCount, 0); //create an empty table (or an array if you would like to see it this way)
      
      env->SetNumReturnValues(1);
      
      for(int i = 0; i < iCount; ++i)
      {
        const char* szName = self->GetVariableName(i);
        
        lua_createtable(L, 0, 2);    // create a new sub table             stack: table, table, TOP
        lua_pushstring(L, "Name");   // push the key                       stack: table, table, key, TOP
        lua_pushstring(L, szName);   // push the value                     stack: table, table, key, value, TOP
        lua_settable(L, -3);         // set key and value as entry         stack: table, table, TOP
        
        lua_pushstring(L, "Type");   // push the key                       stack: table, table, key, TOP
        
        switch(self->GetVariable(szName)->type)
        {
        case VULPTYPE_STRING:
        case VULPTYPE_VSTRING:
        case VULPTYPE_MODEL:
        case VULPTYPE_PSTRING:
        case VULPTYPE_ENTITY_KEY:
        case VULPTYPE_PRIMITIVE_KEY:
        case VULPTYPE_VERTEX_KEY:
        case VULPTYPE_LIGHTSOURCE_KEY:
        case VULPTYPE_WORLDANIMATION_KEY:
        case VULPTYPE_PATH_KEY:
          lua_pushstring(L, "string");
          break;
        case VULPTYPE_ENUM:
        case VULPTYPE_INT:
        case VULPTYPE_FLOAT:
        case VULPTYPE_DOUBLE:
          lua_pushstring(L, "number");
          break;
        case VULPTYPE_BOOL:
          lua_pushstring(L, "boolean");
          break;
        case VULPTYPE_VECTOR_INT:
        case VULPTYPE_VECTOR_FLOAT:
        case VULPTYPE_VECTOR_DOUBLE:
          lua_pushstring(L, "hkvVec3");
          break;
        case VULPTYPE_BYTE_COLOR4:
          lua_pushstring(L, "VColorRef");
          break;
        default:
          lua_pushstring(L, "unknown");
          break;
        }
        //after switch:                                                 stack: table, table, key, value, TOP
        
        lua_settable(L, -3);      //set key and value as entry          stack: table, table, TOP
        lua_rawseti(L, -2, i + 1); //add to overall table                stack: table, TOP
      }
    }
    
    void SetProperty(const char* pszName, const VLuaStackRef& value, VCaptureSwigEnvironment* env)
    {
      VisVariable_cl* pVar = self->GetVariable(pszName);
      
      // if variable can't be found by name try with display name
      if(!pVar)
        pVar = self->GetVariableByDisplayName(pszName);
        
      if(!pVar)
      {
        hkvLog::Warning("Called setter of unknown property '%s'", pszName);
        return;
      }

      lua_State* L = env->GetLuaState();
      
      VVarChangeRes_e result = self->OnVariableValueChanging(pVar, pszName);
      if(result == VCHANGE_IS_ALLOWED)
      {
        switch(pVar->type)
        {
        case VULPTYPE_STRING:
        case VULPTYPE_VSTRING:
        case VULPTYPE_MODEL:
        case VULPTYPE_PSTRING:
        case VULPTYPE_ENTITY_KEY:
        case VULPTYPE_PRIMITIVE_KEY:
        case VULPTYPE_VERTEX_KEY:
        case VULPTYPE_LIGHTSOURCE_KEY:
        case VULPTYPE_WORLDANIMATION_KEY:
        case VULPTYPE_PATH_KEY:
        {
          const char* pszVal = value.ToString();
          pVar->SetValue(self, pszVal, true);
        }
        break;
        case VULPTYPE_ENUM:
        case VULPTYPE_INT:
        {
          int i = (int) value.ToNumber();
          pVar->SetValueDirect(self, (void*) &i, true);
        }
        break;
        case VULPTYPE_FLOAT:
        {
          float f = (float) value.ToNumber();
          pVar->SetValueDirect(self, (void*) &f, true);
        }
        break;
        case VULPTYPE_DOUBLE:
        {
          double d = (double) value.ToNumber();
          pVar->SetValueDirect(self, (void*) &d, true);
        }
        break;
        case VULPTYPE_BOOL:
        {
          BOOL b = (BOOL) value.ToBoolean();
          pVar->SetValueDirect(self, (void*) &b, true);
        }
        break;
        case VULPTYPE_VECTOR_INT:
        {
          const hkvVec3* pVec = value.ToObject<hkvVec3>();

          if(!pVec)
          {
            env->Fail("Expected argument of type hkvVec3");
            return;
          }

          int pI[3] = {(int) pVec->x, (int) pVec->y, (int) pVec->z};
          pVar->SetValueDirect(self, (void*) pI, true);
        }
        break;
        case VULPTYPE_VECTOR_FLOAT:
        {
          const hkvVec3* pVec = value.ToObject<hkvVec3>();

          if(!pVec)
          {
            env->Fail("Expected argument of type hkvVec3");
            return;
          }

          float pF[3] = {pVec->x, pVec->y, pVec->z};
          pVar->SetValueDirect(self, (void*) pF, true);
        }
        break;
        case VULPTYPE_VECTOR_DOUBLE:
        {
          const hkvVec3* pVec = value.ToObject<hkvVec3>();

          if(!pVec)
          {
            env->Fail("Expected argument of type hkvVec3");
            return;
          }

          double pD[3] = {(double) pVec->x, (double) pVec->y, (double) pVec->z};
          pVar->SetValueDirect(self, (void*) pD, true);
        }
        break;
        case VULPTYPE_BYTE_COLOR4:
        {
          const VColorRef* pColor = value.ToObject<VColorRef>();

          if(!pColor)
          {
            env->Fail("Expected argument of type VColorRef");
            return;
          }

          pVar->SetValueDirect(self, (void*) pColor, true);
        }
        default:
          hkvLog::Warning("Called setter of unknown type %d (%s)", pVar->type, pszName);
          break;
        }
        self->OnVariableValueChanging(pVar, pszName);
      }
    }
  }
  
  %rename(GetNumProperties) GetNumVariables() const;
  int GetNumVariables() const;
  
  %rename(UpdateProperty) SetVariable(const char* name, const char* value);
  bool SetVariable(const char* name, const char* value);
};

//add lua tostring and concat operators
VSWIG_CREATE_CONCAT(VTypedObject, 128, "%s", self->GetClassTypeId()->m_lpszClassName)
VSWIG_CREATE_TOSTRING(VTypedObject, "%s: 0x%p", self->GetClassTypeId()->m_lpszClassName, self)

#else

/// \brief Base class for any typed object.
class VTypedObject
{
public:

  /// @name Base Functions
  /// @{

  /// \brief Returns the type name of this instance.
  /// \note Consider using IsOfType to check if the instance is derived from the type.
  /// \return The type name of this instance.
  /// \par Example
  ///   \code
  ///     local obj = Game:GetObject("object1")
  ///     if obj:GetType() == "VisLightSource_cl" then
  ///       obj:SetMultiplier(2)
  ///     end
  ///   \endcode
  /// \see IsOfType
  string GetType();

  /// \brief Test if the instance is of the requested type or is derived from that type.
  /// \param className The class name (type) to test.
  /// \return True if the current instance is of the requested type, otherwise false.
  /// \par Example
  ///   \code
  ///     local obj = Game:GetObject("object1")
  ///     if obj:IsOfType("VisLightSource_cl") then
  ///       obj:SetMultiplier(2)
  ///     end
  ///   \endcode
  boolean IsOfType(string className);

  /// @}
  /// @name Properties
  /// @{

  /// \brief Directly assigns a new value to an exposed object property (public member variables, defined inside the START_VAR_TABLE - END_VAR_TABLE block).
  /// \details
  ///   Assigns a new value to an exposed property. These are the same as the properties in the EntityProperties category in vForge.
  ///   The function will trigger callbacks.
  ///   If you are not sure about the correct property value, you should prefere UpdateProperty because it will check if the value is valid.
  /// \param propName The name or display name of the property.
  /// \param val The new value for the property. Note that this value MUST be of the same type as the property otherwise an error will occur.
  /// \par Example
  ///   \code
  ///     entity:SetProperty("MyStringProperty", "Hello, Lua.")
  ///     typedObj:SetProperty("MyBooleanProperty", true)
  //      anotherObj:SetProperty("MyColorProperty", Vision.VColorRef(255,0,0))
  ///   \endcode
  /// \see UpdateProperty
  void SetProperty(string propName, mixed val);

  /// \brief (Deprecated) Assigns a new value to an exposed object property (public member variable), checks if this new value is valid and notifies all sub systems.
  /// \details
  ///   This function is deprecated and will be removed in the future release. SetProperty should be used instead.
  /// \param propName The name of the property.
  /// \param propValue The property values as string representation.
  /// \par Example
  ///   \code
  ///     entity:UpdateProperty("MyStringProperty", "Hello, Lua.")
  ///     typedObj:UpdateProperty("MyBooleanProperty", "AnotherValue")
  ///   \endcode
  boolean UpdateProperty(string propName, string propValue);

  /// \brief Gets the value of an exposed property (public member varaibles, defined inside the START_VAR_TABLE - END_VAR_TABLE block).
  /// \details
  ///   Gets the value from an exposed property. These are the same as the properties in the EntityProperties category in vForge.
  /// \param propName The name of the property.
  /// \return The value of the property. Its type is the same as the property's type, for example a string property will return a string and a floating point property will return a number. If the property doesn't exist, \b nil is returned.
  /// \par Example
  ///   \code
  ///     local str = entity:GetProperty("MyStringProperty")
  ///     local num = typedObj:GetProperty("MyNumberProperty")
  ///     local vector = anotherObj:GetProperty("MyPositionProperty")
  ///   \endcode
  /// \see GetProperties
  mixed GetProperty(string propName);

  /// \brief Gets the type of an exposed property (public member varaibles, defined inside the START_VAR_TABLE - END_VAR_TABLE block).
  /// \param propName The name of the property.
  /// \return
  ///   The type of a property. The type of the property is translated to a Lua type and represented as a string.
  ///   Currently, this function can return one of the following types:
  ///   - "string"
  ///   - "number"
  ///   - "boolean"
  ///   - "hkvVec3"
  ///   - "VColorRef"
  ///   (or "unkknown" for any other type)
  /// \par Example
  ///   \code
  ///     function OnCreate(self)
  ///       local propType = self:GetPropertyType("MyProperty")
  ///       if (propType == "string") then
  ///         Debug:PrinLine("MyProperty: " .. self:GetProperty("MyProperty"))
  ///       end
  ///     end
  ///   \endcode
  /// \see GetProperties
  string GetPropertyType(string propName);

  /// \brief Get all properties of the current object.
  /// \return A table containing all properties from index 1 to LEN, where each entry consists of Name and Type.
  /// \par Example
  ///   \code
  ///     function OnAfterSceneLoaded(self)
  ///       local properties = self:GetProperties()
  ///       for i=1,#properties do
  ///         Debug:PrintLine("'" .. properties[i].Name .. "': " .. properties[i].Type)
  ///       end
  ///     end
  ///   \endcode
  table GetProperties();

  /// \brief Returns the number of properties associated with the object's type.
  /// \return The number of properties.
  /// \par Example
  ///   \code
  ///    local numProps = self:GetNumProperties();
  ///    Debug:PrintLine("The entity has " .. numProps .. " prop(s)")
  ///   \endcode
  number GetNumProperties();

  /// @}
  /// @name Operators
  /// @{

  /// \brief Compare with another VTypedObject, e.g. obj1 == obj2
  /// \param other The other typed object.
  /// \return true if the internal pointers are equal, otherwise false.
  boolean operator == (VTypedObject other);
};

#endif

/*
 * Havok SDK - Base file, BUILD(#20140717)
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
