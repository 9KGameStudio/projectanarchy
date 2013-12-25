/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// This file will be part of a future Vision SDK and is not relevant yet

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>

#include <Vision/Runtime/Base/RemoteComm/VMessage.hpp>
#include <Vision/Runtime/Base/RemoteComm/VConnection.hpp>
#include <Vision/Runtime/Base/RemoteComm/VTarget.hpp>

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/VScriptIncludes.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/RSDClient/VRSDClient.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/RSDClient/VRSDLuaImplementation.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VisionLuaModule_wrapper.hpp>

#include <Vision/Runtime/Base/String/VStringTokenizerInPlace.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>


VRSDClientLuaImplementation::VRSDClientLuaImplementation()
  : m_bDebuggerRetrievingValues(false)
{
  m_pLuaState = NULL;
  m_pActivationRecord = NULL;

  VRSDClient::GetGlobalClient().OnConnected += this;
  VRSDClient::GetGlobalClient().OnDisconnected += this;
  ControlDebugHook(true);
}

VRSDClientLuaImplementation::~VRSDClientLuaImplementation()
{
  VRSDClient::GetGlobalClient().OnConnected -= this;
  VRSDClient::GetGlobalClient().OnDisconnected -= this;
}

bool VRSDClientLuaImplementation::GetCallstack(DynArray_cl<VRSDClientCallstackEntry>& Callstack, unsigned int& CallstackEntryCount)
{
  VASSERT(m_pLuaState);

  if(!m_pLuaState)
    return false;

  CallstackEntryCount = 0;

  // get the callstack from Lua
  lua_Debug AR;
  
  while(lua_getstack(m_pLuaState, CallstackEntryCount, &AR))
  {
    if(lua_getinfo(m_pLuaState, "nSl", &AR))
    {
      Callstack[CallstackEntryCount].iLineNumber = AR.currentline;
      Callstack[CallstackEntryCount].pFileName = GetCorrectLuaSourceString(AR.source);
      Callstack[CallstackEntryCount].pFunctionName = AR.name;
      Callstack[CallstackEntryCount].iLineDefined = AR.linedefined;

      CallstackEntryCount++;
    }
  }

  return true;
}

bool VRSDClientLuaImplementation::GetLocalSymbols(DynArray_cl<VRSDScriptSymbol>& LocalSymbols, unsigned int& LocalSymbolCount)
{
  VASSERT(m_pLuaState);

  if(!m_pLuaState || !m_pActivationRecord)
    return false;

  LocalSymbolCount = 0;

  // we can only get local symbols without a crash if we are really in a Lua code execution path
  if(strcmp(m_pActivationRecord->what, "Lua"))
    return true;

  const char* pSymbolName = NULL;
  int iLocalIndex = 1;

  while((pSymbolName = lua_getlocal(m_pLuaState, m_pActivationRecord, iLocalIndex)) != NULL)
  {
    // skip lua internal temporary variables (they start with an '(')
    if(pSymbolName[0] != '(')
    {
      // if the variable is a table, get all child variables (one level deep only)
      if(lua_istable(m_pLuaState, -1))
      {
        // add a symbol for the table
        AddSymbol(LocalSymbols, LocalSymbolCount, pSymbolName, "table", VRSDScriptSymbol::SYMBOL_TABLE);
      }
      // numeric member variable
      else if(lua_type(m_pLuaState, -1) == LUA_TNUMBER)
      {
        char buffer[32];
        sprintf(buffer, "%f", lua_tonumber(m_pLuaState, -1));
        AddSymbol(LocalSymbols, LocalSymbolCount, pSymbolName, buffer, VRSDScriptSymbol::SYMBOL_NUMBER);
      }
      // string member variable
      else if(lua_type(m_pLuaState, -1) == LUA_TSTRING)
      {
        AddSymbol(LocalSymbols, LocalSymbolCount, pSymbolName, lua_tostring(m_pLuaState, -1), VRSDScriptSymbol::SYMBOL_STRING);
      }
      else if(lua_isfunction(m_pLuaState, -1))
      {
        AddSymbol(LocalSymbols, LocalSymbolCount, pSymbolName, "function", VRSDScriptSymbol::SYMBOL_FUNCTION);
      }
      else if(lua_isuserdata(m_pLuaState, -1))
      {
        char buffer[128];
        swig_type_info* type = (swig_type_info *)LUA_GetSwigType(m_pLuaState, -1);
        void * pUserData = lua_touserdata(m_pLuaState, -1);

        if(type)
        {
          vis_snprintf(buffer, 128, "userdata:0x%p [%s: 0x%p]", pUserData, type->str, ((swig_lua_userdata*)pUserData)->ptr);
        }
        else
        {
          vis_snprintf(buffer, 128, "userdata:0x%p", lua_touserdata(m_pLuaState, -1));
        }
        AddSymbol(LocalSymbols, LocalSymbolCount, pSymbolName, buffer, VRSDScriptSymbol::SYMBOL_USERDATA);
      }
      else if(lua_isboolean(m_pLuaState, -1))
      {
        int iBoolVal = lua_toboolean(m_pLuaState, -1);
        AddSymbol(LocalSymbols, LocalSymbolCount, pSymbolName, iBoolVal ? "true" : "false", VRSDScriptSymbol::SYMBOL_BOOLEAN);
      }
      else if(lua_isnil(m_pLuaState, -1))
      {
        AddSymbol(LocalSymbols, LocalSymbolCount, pSymbolName, "nil", VRSDScriptSymbol::SYMBOL_CLASS);
      }
    }

    // clean up the stack and increment the index to get the next local variable
    lua_pop(m_pLuaState, 1);
    iLocalIndex++;
  }


  return true;
}

bool VRSDClientLuaImplementation::UpdateLocalVariable(const char* Variable, const char* NewValue)
{
  if(!Variable || !NewValue)
    return false;

  VASSERT(m_pLuaState);

  if(!m_pLuaState || !m_pActivationRecord)
    return false;

  // we can only get local symbols without a crash if we are really in a Lua code execution path
  if(strcmp(m_pActivationRecord->what, "Lua"))
    return true;

  VLuaStackCleaner stackCleaner(m_pLuaState);
  ScopedBooleanToTrue disableDebugHook(m_bDebuggerRetrievingValues);

  const char* pSymbolName = NULL;
  int iLocalIndex = 1;

  VMemoryTempBuffer<512> copyBuffer(Variable); // operate on a copy string in the tokenizer
  
  VStringTokenizerInPlace Tokenizer(copyBuffer.AsChar(), '.');
  char* pCurrent = Tokenizer.Next();
  int i = 0;
  const char* pLastField = NULL;

  // go through all local variables
  while((pSymbolName = lua_getlocal(m_pLuaState, m_pActivationRecord, iLocalIndex)) != NULL)
  {
    // check if this local variable is the one we want
    if(!strcmp(pSymbolName, pCurrent))
    {
      VLuaStackCleaner innerStackCleaner(m_pLuaState);
      if(LookupPath(Tokenizer, &pLastField) != HKV_SUCCESS)
        return false;

      // now the variable is at the top of the stack, update its value
      VRSDScriptSymbol::SymbolType Type = VRSDScriptSymbol::SYMBOL_USERDATA;

      if(lua_isnumber(m_pLuaState, -1))
        Type = VRSDScriptSymbol::SYMBOL_NUMBER;
      else if(lua_isstring(m_pLuaState, -1))
        Type = VRSDScriptSymbol::SYMBOL_STRING;
      else if(lua_isboolean(m_pLuaState, -1))
        Type = VRSDScriptSymbol::SYMBOL_BOOLEAN;

      // pop off the field again
      lua_pop(m_pLuaState, 1);

      bool bIsIntegerKey = false;
      if(pLastField && VStringUtil::IsIntegerString(pLastField))
      {
        bIsIntegerKey = true;
        lua_pushnumber(m_pLuaState, (LUA_NUMBER)atoi(pLastField));
      }

      // depending on the type push the correct stuff on the lua stack
      if(Type == VRSDScriptSymbol::SYMBOL_NUMBER)
      {
        lua_Number TempVar;
        sscanf(NewValue, "%f", &TempVar);
        lua_pushnumber(m_pLuaState, TempVar);
      }
      else if(Type == VRSDScriptSymbol::SYMBOL_STRING)
      {
        lua_pushstring(m_pLuaState, NewValue);
      }
      else if(Type == VRSDScriptSymbol::SYMBOL_BOOLEAN)
      {
        if(!strcmp(NewValue, "true") || !strcmp(NewValue, "1"))
          lua_pushboolean(m_pLuaState, 1);
        else
          lua_pushboolean(m_pLuaState, 0);
      }
      else
        return false;

      if(Tokenizer.GetTokenCount() > 1)
      {
        VASSERT(pLastField != NULL);
        if(bIsIntegerKey)
        {
          lua_settable(m_pLuaState, -3);
        }
        else
        {
          lua_setfield(m_pLuaState, -2, pLastField);
        }
      }
      else
      {
        lua_setlocal(m_pLuaState, m_pActivationRecord, iLocalIndex);
      }

      break;
    }

    // clean up the stack and increment the index to get the next local variable
    lua_pop(m_pLuaState, 1);
    iLocalIndex++;
  }

  return true;
}

bool VRSDClientLuaImplementation::UpdateGlobalVariable(const char* szVarName, const char* szNewValue)
{
  if(!szVarName || !szNewValue || szVarName[0]==0)
    return false;

  // we can only get global symbols without a crash if we are really in a Lua code execution path
  if(strcmp(m_pActivationRecord->what, "Lua"))
    return true;

  VMemoryTempBuffer<512> copyBuffer(szVarName); // operate on a copy string in the tokenizer

  VLuaStackCleaner stackCleaner(m_pLuaState);
  ScopedBooleanToTrue disableDebugHook(m_bDebuggerRetrievingValues);

  VStringTokenizerInPlace Tokenizer(copyBuffer.AsChar(), '.');
  const char* pCurrent = Tokenizer.Next();
  unsigned int i = 0;

  const char* pLastField = NULL;

  lua_getfield(m_pLuaState, LUA_GLOBALSINDEX, pCurrent);
  if(lua_isnil(m_pLuaState, -1))
    return false;
  if(LookupPath(Tokenizer, &pLastField) != HKV_SUCCESS)
    return false;

  // now the variable is at the top of the stack, update its value
  int iLuaType = lua_type(m_pLuaState, -1);
  lua_pop(m_pLuaState, 1);

  bool bIsIntegerKey = false;
  if(pLastField && VStringUtil::IsIntegerString(pLastField))
  {
    bIsIntegerKey = true;
    lua_pushnumber(m_pLuaState, (LUA_NUMBER)atoi(pLastField));
  }

  switch(iLuaType)
  {
    case LUA_TNUMBER:
      {
        lua_Number tempNumber=0;
        sscanf(szNewValue, "%f", &tempNumber);
        lua_pushnumber(m_pLuaState, tempNumber);
      }
      break;
    case LUA_TSTRING:
      lua_pushstring(m_pLuaState, szNewValue);
      break;
    case LUA_TBOOLEAN:
      if( !VStringHelper::SafeCompare(szNewValue, "true") || !VStringHelper::SafeCompare(szNewValue, "1") )
        lua_pushboolean(m_pLuaState, 1);
      else
        lua_pushboolean(m_pLuaState, 0);
      break;

    //we do not support the following types to edit:
    //case LUA_TTABLE
    //case LUA_TFUNCTION
    //case LUA_TUSERDATA
    //case LUA_TTHREAD
    //case LUA_TLIGHTUSERDATA
    default:
      return false;
  }

  if( Tokenizer.GetTokenCount() > 1 )
  {
    VASSERT(pLastField != NULL);
    if(bIsIntegerKey)
    {
      lua_settable(m_pLuaState, -3);
    }
    else
    {
      lua_setfield(m_pLuaState, -2, pLastField);
    }
  }
  else
  {
    lua_setglobal(m_pLuaState, szVarName);
  }

  return true;
}

bool VRSDClientLuaImplementation::UpdateHiddenGlobalVariable(const void* pUserDataParent, 
                                                             const char* szVarName, 
                                                             const char* szNewValue)
{
  const char* szFormatStr = "G."LUA_HIDDEN_GLOBAL_PREFIX"-%p-%s$";

  // build global variable string
  VString sHiddenGlobalName;
  sHiddenGlobalName.Format(szFormatStr, ((swig_lua_userdata *)pUserDataParent)->ptr, szVarName);

  // try to update the global variable
  return UpdateGlobalVariable(sHiddenGlobalName.AsChar(), szNewValue);
}

bool VRSDClientLuaImplementation::GetUserDataPointerFromGlobal(const char* szVariable, void** ppUserData, void ** ppEnvironment)
{
  VASSERT(m_pLuaState);

  if(!m_pLuaState || !m_pActivationRecord)
    return false;

  // we can only get local symbols without a crash if we are really in a Lua code execution path
  if(strcmp(m_pActivationRecord->what, "Lua"))
    return true;

  *ppEnvironment = m_pLuaState;

  int iStackItems = LUA_PushGlobalUserData(m_pLuaState, szVariable);

  if(iStackItems>0)
  {
    *ppUserData = lua_touserdata(m_pLuaState,-1);
    
    //clean up the stack
    lua_pop(m_pLuaState, iStackItems);

    return true;
  }

  return false;
}

bool VRSDClientLuaImplementation::GetUserDataPointerFromLocal(const char* szVariable, void** ppUserData, void ** ppEnvironment)
{
  VASSERT(m_pLuaState);

  if(!m_pLuaState || !m_pActivationRecord)
    return false;

  // we can only get local symbols without a crash if we are really in a Lua code execution path
  if(strcmp(m_pActivationRecord->what, "Lua"))
    return true;

  ScopedBooleanToTrue disableDebugHook(m_bDebuggerRetrievingValues);
  VLuaStackCleaner stackCleaner(m_pLuaState);


  char* pSymbolName = NULL;
  int iLocalIndex = 1;

  VMemoryTempBuffer<512> copyBuffer(szVariable); // operate on a copy string in the tokenizer
  
  VStringTokenizerInPlace Tokenizer(copyBuffer.AsChar(), '.');
  char* pCurrent = Tokenizer.Next();

  while((pSymbolName = (char*)lua_getlocal(m_pLuaState, m_pActivationRecord, iLocalIndex)) != NULL)
  {                                                       //Stack: ..., localX, TOP
    // check if this local variable is the one we want
    if(!strcmp(pSymbolName, pCurrent))
    {
      VLuaStackCleaner innerStackCleaner(m_pLuaState);
      //there is already the local on the stack...
      if(LookupPath(Tokenizer) != HKV_SUCCESS)
        return false;

      // now the variable is at the top of the stack
      *ppUserData = lua_touserdata(m_pLuaState, -1);    //Stack: ..., localX, {field}, TOP
      *ppEnvironment = m_pLuaState;
      return true;
    }

    // remove the value and update the index to get the next local variable
    lua_pop(m_pLuaState, 1);
    iLocalIndex++;
  }

  return false;
}

bool VRSDClientLuaImplementation::IsLocalUserDataOfType(const char* Name, const char* Type)
{
  VASSERT(m_pLuaState);

  if(!m_pLuaState || !m_pActivationRecord)
    return false;

  // we can only get local symbols without a crash if we are really in a Lua code execution path
  if(strcmp(m_pActivationRecord->what, "Lua"))
    return true;

  VLuaStackCleaner stackCleaner(m_pLuaState);
  ScopedBooleanToTrue disableDebugHook(m_bDebuggerRetrievingValues);

  const char* pSymbolName = NULL;
  int iLocalIndex = 1;

  VMemoryTempBuffer<512> copyBuffer(Name); // operate on a copy string in the tokenizer
  
  VStringTokenizerInPlace Tokenizer(copyBuffer.AsChar(), '.');
  char* pCurrent = Tokenizer.Next();

  while((pSymbolName = lua_getlocal(m_pLuaState, m_pActivationRecord, iLocalIndex)) != NULL)
  {                                                             //stack: ..., localX, TOP
    // check if this local variable is the one we want
    if(!strcmp(pSymbolName, pCurrent))
    {
      VLuaStackCleaner innerStackCleaner(m_pLuaState);
      // there is already the local on the stack

      if(LookupPath(Tokenizer) != HKV_SUCCESS)
        return false;

      // now the variable is at the top of the stack
      return LUA_TestUserData(m_pLuaState, -1, Type) != NULL;
    }

    // increment the index to get the next local variable
    iLocalIndex++;
    lua_pop(m_pLuaState, 1);
  }
 
  return false;
}

bool VRSDClientLuaImplementation::IsGlobalUserDataOfType(const char* Name, const char* Type)
{
  VASSERT(m_pLuaState);

  if(!m_pLuaState || !m_pActivationRecord)
    return false;

  // we can only get local symbols without a crash if we are really in a Lua code execution path
  if(strcmp(m_pActivationRecord->what, "Lua"))
    return true;

  VLuaStackCleaner stackCleaner(m_pLuaState);
  ScopedBooleanToTrue disableDebugHook(m_bDebuggerRetrievingValues);

  VMemoryTempBuffer<512> copyBuffer(Name); // operate on a copy string in the tokenizer
  
  VStringTokenizerInPlace Tokenizer(copyBuffer.AsChar(), '.');

  lua_getfield(m_pLuaState, LUA_GLOBALSINDEX, Tokenizer.Next());
  if(LookupPath(Tokenizer) != HKV_SUCCESS)
    return false;

  // now the variable should be at the top of the stack
  return LUA_TestUserData(m_pLuaState, -1, Type) != NULL;
}

bool VRSDClientLuaImplementation::GetGlobalType(const char*pVariableName, char * pUserDataTypeName)
{
  VASSERT(m_pLuaState);

  if(!m_pLuaState || !m_pActivationRecord)
    return false;

  // we can only get local symbols without a crash if we are really in a Lua code execution path
  if(strcmp(m_pActivationRecord->what, "Lua"))
    return true;

  VLuaStackCleaner stackCleaner(m_pLuaState);
  ScopedBooleanToTrue disableDebugHook(m_bDebuggerRetrievingValues);

  VMemoryTempBuffer<512> copyBuffer(pVariableName); // operate on a copy string in the tokenizer

  VStringTokenizerInPlace Tokenizer(copyBuffer.AsChar(), '.');
  lua_getfield(m_pLuaState, LUA_GLOBALSINDEX, Tokenizer.Next());

  if(LookupPath(Tokenizer) != HKV_SUCCESS)
    return false;

  const char * szName = VSWIG_Lua_typename(m_pLuaState, -1);
  sprintf(pUserDataTypeName, "%s", szName);
  
  return pUserDataTypeName[0] != 0;
}

bool VRSDClientLuaImplementation::GetLocalType(const char* pVariableName, char * pUserDataTypeName)
{
  VASSERT(m_pLuaState);

  if(!m_pLuaState || !m_pActivationRecord)
    return false;

  // we can only get local symbols without a crash if we are really in a Lua code execution path
  if(strcmp(m_pActivationRecord->what, "Lua"))
    return true;

  VLuaStackCleaner stackCleaner(m_pLuaState);
  ScopedBooleanToTrue disableDebugHook(m_bDebuggerRetrievingValues);

  const char* pSymbolName = NULL;
  int iLocalIndex = 1;

  VMemoryTempBuffer<512> copyBuffer(pVariableName); // operate on a copy string in the tokenizer

  VStringTokenizerInPlace Tokenizer(copyBuffer.AsChar(), '.');
  char* pCurrent = Tokenizer.Next();

  pUserDataTypeName[0] = 0;

  while((pSymbolName = lua_getlocal(m_pLuaState, m_pActivationRecord, iLocalIndex)) != NULL)
  {                                                             //stack: ..., localX, TOP
    // check if this local variable is the one we want
    if(!strcmp(pSymbolName, pCurrent))
    {
      // there is already the local on the stack
      if(LookupPath(Tokenizer) != HKV_SUCCESS)
        return false;

      const char * szName = VSWIG_Lua_typename(m_pLuaState, -1);
      sprintf(pUserDataTypeName, "%s", szName);

      return pUserDataTypeName[0] != 0;
    }

    // clean up the stack and increment the index to get the next local variable
    lua_pop(m_pLuaState, 1);                                    //stack: ..., TOP
    iLocalIndex++;
  }

  return false;
}

bool VRSDClientLuaImplementation::GetGlobalSymbols(DynArray_cl<VRSDScriptSymbol>& GlobalSymbols, unsigned int& GlobalSymbolCount)
{
  VASSERT(m_pLuaState);

  if(!m_pLuaState || !m_pActivationRecord)
    return false;

  GlobalSymbolCount = 0;

  // we can only get local symbols without a crash if we are really in a Lua code execution path
  if(strcmp(m_pActivationRecord->what, "Lua"))
    return true;

  VLuaStackCleaner stackCleaner(m_pLuaState);
  // iterate through the globals table to get its keys
  
  // first key for the iteration
  lua_pushnil(m_pLuaState);
  
  while (lua_next(m_pLuaState, LUA_GLOBALSINDEX) != 0)
  {
    // after this the key is at -2 and the value at -1
    
    // we only want string fields and no other keys
    if (lua_isstring(m_pLuaState, -2))
    {  
      char* pSymbolName = (char*)lua_tostring(m_pLuaState, -2);

      //filter generated dynamic properties, starting with $node-
      if(pSymbolName != NULL && pSymbolName!=strstr(pSymbolName, LUA_HIDDEN_GLOBAL_PREFIX))
      {
        // if the variable is a table, get all child variables (one level deep only)
        if(lua_istable(m_pLuaState, -1))
        {
          // add a symbol for the table
          AddSymbol(GlobalSymbols, GlobalSymbolCount, pSymbolName, "table", VRSDScriptSymbol::SYMBOL_TABLE);
        }
        // numeric member variable
        else if(lua_type(m_pLuaState, -1) == LUA_TNUMBER)
        {
          char buffer[32];
          vis_snprintf(buffer, 32, "%f", lua_tonumber(m_pLuaState, -1));
          AddSymbol(GlobalSymbols, GlobalSymbolCount, pSymbolName, buffer, VRSDScriptSymbol::SYMBOL_NUMBER);
        }
        // string member variable
        else if(lua_type(m_pLuaState, -1) == LUA_TSTRING)
        {
          AddSymbol(GlobalSymbols, GlobalSymbolCount, pSymbolName, lua_tostring(m_pLuaState, -1), VRSDScriptSymbol::SYMBOL_STRING);
        }
        else if(lua_isfunction(m_pLuaState, -1))
        {
          AddSymbol(GlobalSymbols, GlobalSymbolCount, pSymbolName, "function", VRSDScriptSymbol::SYMBOL_FUNCTION);
        }
        else if(lua_isuserdata(m_pLuaState, -1))
        {
          char buffer[128];
          swig_type_info* type = (swig_type_info *)LUA_GetSwigType(m_pLuaState, -1);
          void * pUserData = lua_touserdata(m_pLuaState, -1);

          if(type)
          {
            vis_snprintf(buffer, 128, "userdata:0x%p [%s: 0x%p]", pUserData, type->str, ((swig_lua_userdata*)pUserData)->ptr);
          }
          else
          {
            vis_snprintf(buffer, 128, "userdata:0x%p", lua_touserdata(m_pLuaState, -1));
          }
          AddSymbol(GlobalSymbols, GlobalSymbolCount, pSymbolName, buffer, VRSDScriptSymbol::SYMBOL_USERDATA);
        }
        else if(lua_isboolean(m_pLuaState, -1))
        {
          int iBoolVal = lua_toboolean(m_pLuaState, -1);
          AddSymbol(GlobalSymbols, GlobalSymbolCount, pSymbolName, iBoolVal ? "true" : "false", VRSDScriptSymbol::SYMBOL_BOOLEAN);
        }
        else if(lua_isnil(m_pLuaState, -1))
        {
          AddSymbol(GlobalSymbols, GlobalSymbolCount, pSymbolName, "nil", VRSDScriptSymbol::SYMBOL_CLASS);
        }
      }
    }
    lua_pop(m_pLuaState, 1);  // remove the value, keep the key for the next iteration
  }

  return true;
}

bool VRSDClientLuaImplementation::GetSubSymbolsForLocal(char* LocalName, DynArray_cl<VRSDScriptSymbol>& SubSymbols, unsigned int& SubSymbolCount)
{
  VASSERT(m_pLuaState);

  if(!m_pLuaState || !m_pActivationRecord)
    return false;
  
  SubSymbolCount = 0;

  // we can only get local symbols without a crash if we are really in a Lua code execution path
  if(strcmp(m_pActivationRecord->what, "Lua"))
    return true;

  VLuaStackCleaner stackCleaner(m_pLuaState);
  ScopedBooleanToTrue disableDebugCallback(m_bDebuggerRetrievingValues);

  char* pSymbolName = NULL;
  int iLocalIndex = 1;

  VMemoryTempBuffer<512> copyBuffer(LocalName); // operate on a copy string in the tokenizer
  
  VStringTokenizerInPlace Tokenizer(copyBuffer.AsChar(), '.');
  char* pCurrent = Tokenizer.Next();

  while((pSymbolName = (char*)lua_getlocal(m_pLuaState, m_pActivationRecord, iLocalIndex)) != NULL)
  {                                                       //stack: .., localX, TOP
    // check if this local variable is the one we want
    if(!strcmp(pSymbolName, pCurrent))
    {
      //the local is already on the stack
      if(LookupPath(Tokenizer) != HKV_SUCCESS)
        return false;

      // now we can iterate over the contents of the table
      // first key for the iteration
      lua_pushnil(m_pLuaState);                           //stack: .., localX, {field}, nil, TOP

      //access the last field
      while (lua_next(m_pLuaState, -2) != 0)              //stack: .., localX, {field}, key, value TOP
      {
        // we only want string fields and numeric fields
        // (lua_isstring returns also true for numbers, using
        // tostring later on will cast the number to a string)
        int iKeyType = lua_type(m_pLuaState, -2);
        if (iKeyType==LUA_TNUMBER || iKeyType==LUA_TSTRING)
        {  
          VString sKeyBuffer;

          //this if prevents a conversion of number on the Lua stack
          if(iKeyType==LUA_TNUMBER) sKeyBuffer.Format("%1.0f", lua_tonumber(m_pLuaState, -2));
          else                      sKeyBuffer = lua_tostring(m_pLuaState, -2);

          if(!sKeyBuffer.IsEmpty())
          {
            int iValueType = lua_type(m_pLuaState, -1);
            VString sValueBuffer;

            // table member variable
            switch (iValueType) 
            {
              case LUA_TTABLE:
                // add a symbol for the table
                AddSymbol(SubSymbols, SubSymbolCount, sKeyBuffer.AsChar(), "table", VRSDScriptSymbol::SYMBOL_TABLE);
                break;
            
              case LUA_TNUMBER:
                // numeric member variable
                sValueBuffer.Format("%f", lua_tonumber(m_pLuaState, -1));
                AddSymbol(SubSymbols, SubSymbolCount, sKeyBuffer.AsChar(), sValueBuffer.AsChar(), VRSDScriptSymbol::SYMBOL_NUMBER);
                break;

              case LUA_TSTRING:
                AddSymbol(SubSymbols, SubSymbolCount, sKeyBuffer.AsChar(), lua_tostring(m_pLuaState, -1), VRSDScriptSymbol::SYMBOL_STRING);
                break;

              case LUA_TFUNCTION:
                sValueBuffer.Format("function:0x%p", lua_tocfunction(m_pLuaState, -1));
                AddSymbol(SubSymbols, SubSymbolCount, sKeyBuffer.AsChar(), sValueBuffer.AsChar(), VRSDScriptSymbol::SYMBOL_FUNCTION);
                break;
              
              case LUA_TBOOLEAN:
                AddSymbol(SubSymbols, SubSymbolCount, sKeyBuffer.AsChar(), lua_toboolean(m_pLuaState, -1) ? "true" : "false", VRSDScriptSymbol::SYMBOL_BOOLEAN);
                break;

              case LUA_TNIL:
                AddSymbol(SubSymbols, SubSymbolCount, sKeyBuffer.AsChar(), "nil", VRSDScriptSymbol::SYMBOL_CLASS);
                break;

              case LUA_TTHREAD:
                sValueBuffer.Format("thread:0x%p", lua_tothread(m_pLuaState, -1));
                AddSymbol(SubSymbols, SubSymbolCount, sKeyBuffer.AsChar(), sValueBuffer.AsChar(), VRSDScriptSymbol::SYMBOL_CLASS);
                break;

              case LUA_TUSERDATA:
                sValueBuffer.Format("userdata:0x%p", lua_touserdata(m_pLuaState, -1));
                AddSymbol(SubSymbols, SubSymbolCount, sKeyBuffer.AsChar(), sValueBuffer.AsChar(), VRSDScriptSymbol::SYMBOL_USERDATA);
                break;
            
              default:
                AddSymbol(SubSymbols, SubSymbolCount, sKeyBuffer.AsChar(), "unknown", VRSDScriptSymbol::SYMBOL_STRING);
                break;
            }
          }
        }

        // remove the value, keep the key for the next iteration
        lua_pop(m_pLuaState, 1);                        //stack: .., localX, {field}, key, TOP
      }
      return true;
    }

    // clean up the stack and increment the index to get the next local variable
    lua_pop(m_pLuaState, 1);                            //stack: .., TOP
    iLocalIndex++;
  }

  return true;
}

bool VRSDClientLuaImplementation::GetSubSymbolsForGlobal(char* GlobalName, DynArray_cl<VRSDScriptSymbol>& SubSymbols, unsigned int& SubSymbolCount)
{
  VASSERT(m_pLuaState);

  if(!m_pLuaState || !m_pActivationRecord)
    return false;

  SubSymbolCount = 0;

  // we can only get local symbols without a crash if we are really in a Lua code execution path
  if(strcmp(m_pActivationRecord->what, "Lua"))
    return true;

  ScopedBooleanToTrue disableDebugCallback(m_bDebuggerRetrievingValues);
  VLuaStackCleaner stackCleaner(m_pLuaState);

  VMemoryTempBuffer<512> copyBuffer(GlobalName); // operate on a copy string in the tokenizer
  
  VStringTokenizerInPlace Tokenizer(copyBuffer.AsChar(), '.');
  lua_getfield(m_pLuaState, LUA_GLOBALSINDEX, Tokenizer.Next());
  if(LookupPath(Tokenizer) != HKV_SUCCESS)
    return false;

  // now the variable should be at the top of the stack and we can get the subvariables of it
  
  // first key for the iteration
  lua_pushnil(m_pLuaState);
  
  while (lua_next(m_pLuaState, -2) != 0)
  {
    // after this the key is at -2 and the value at -1
    
    // we only want string fields and numeric fields
    // (lua_isstring returns also true for numbers, using
    // tostring later on will cast the number to a string)
    int iKeyType = lua_type(m_pLuaState, -2);
    if (iKeyType==LUA_TNUMBER || iKeyType==LUA_TSTRING)
    {  
      VString sKeyBuffer;

      //this if prevents a conversion of number on the Lua stack
      if(iKeyType==LUA_TNUMBER) sKeyBuffer.Format("%1.0f", lua_tonumber(m_pLuaState, -2));
      else                      sKeyBuffer = lua_tostring(m_pLuaState, -2);

      const char* pSymbolName = sKeyBuffer.AsChar();

      if(pSymbolName && pSymbolName!=strstr(pSymbolName, LUA_HIDDEN_GLOBAL_PREFIX)) //remove the second condition if you would like to see the $node vars in the G table (vRSD)
      {
        // table member variable
        if(lua_istable(m_pLuaState, -1))
        {
          // add a symbol for the table
          AddSymbol(SubSymbols, SubSymbolCount, pSymbolName, "table", VRSDScriptSymbol::SYMBOL_TABLE);
        }
        // numeric member variable
        else if(lua_type(m_pLuaState, -1) == LUA_TNUMBER)
        {
          char buffer[32];
          sprintf(buffer, "%f", lua_tonumber(m_pLuaState, -1));
          AddSymbol(SubSymbols, SubSymbolCount, pSymbolName, buffer, VRSDScriptSymbol::SYMBOL_NUMBER);
        }
        // string member variable
        else if(lua_type(m_pLuaState, -1) == LUA_TSTRING)
        {
          AddSymbol(SubSymbols, SubSymbolCount, pSymbolName, lua_tostring(m_pLuaState, -1), VRSDScriptSymbol::SYMBOL_STRING);
        }
        // function member variable
        else if(lua_isfunction(m_pLuaState, -1))
        {
          AddSymbol(SubSymbols, SubSymbolCount, pSymbolName, "function", VRSDScriptSymbol::SYMBOL_FUNCTION);
        }
        // userdata member variable
        else if(lua_isuserdata(m_pLuaState, -1))
        {
          char buffer[128];
          swig_type_info* type = (swig_type_info *)LUA_GetSwigType(m_pLuaState, -1);
          void * pUserData = lua_touserdata(m_pLuaState, -1);

          if(type)
          {
            vis_snprintf(buffer, 128, "userdata:0x%p [%s: 0x%p]", pUserData, type->str, ((swig_lua_userdata*)pUserData)->ptr);
          }
          else
          {
            vis_snprintf(buffer, 128, "userdata:0x%p", lua_touserdata(m_pLuaState, -1));
          }
          AddSymbol(SubSymbols, SubSymbolCount, pSymbolName, buffer, VRSDScriptSymbol::SYMBOL_USERDATA);
        }
        else if(lua_isboolean(m_pLuaState, -1))
        {
          int iBoolVal = lua_toboolean(m_pLuaState, -1);
          AddSymbol(SubSymbols, SubSymbolCount, pSymbolName, iBoolVal ? "true" : "false", VRSDScriptSymbol::SYMBOL_BOOLEAN);
        }
        else if(lua_isnil(m_pLuaState, -1))
        {
          AddSymbol(SubSymbols, SubSymbolCount, pSymbolName, "nil", VRSDScriptSymbol::SYMBOL_CLASS);
        }

      }
    }
    lua_pop(m_pLuaState, 1);  // remove the value, keep the key for the next iteration
  }

  return true;
}

void VRSDClientLuaImplementation::SetDebugInformation(lua_State* L, lua_Debug* ar)
{
  m_pLuaState = L;
  m_pActivationRecord = ar;
}

void VRSDClientLuaImplementation::AddSymbol(DynArray_cl<VRSDScriptSymbol>& dynArrSymbols, unsigned int& iSymbolCount, const char* szSymbolName, const char* szSymbolContent, VRSDScriptSymbol::SymbolType eSymbolType)
{
  dynArrSymbols[iSymbolCount].m_eSymbolType = eSymbolType;
  dynArrSymbols[iSymbolCount].SetSymbolName(szSymbolName);
  dynArrSymbols[iSymbolCount].SetSymbolContent(szSymbolContent);

  iSymbolCount++;
}

void VRSDClientLuaImplementation::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
}

void VRSDClientLuaImplementation::ControlDebugHook(bool Enable)
{
  if(Enable)
    lua_sethook(VScriptResourceManager::GlobalManager().GetMasterState(), Lua_DebugHook, LUA_MASKRET | LUA_MASKCALL | LUA_MASKLINE, 0 );
  else
    lua_sethook(VScriptResourceManager::GlobalManager().GetMasterState(), Lua_DebugHook, 0, 0);
}

bool VRSDClientLuaImplementation::IsImplementation(const char* Name)
{
  return !strcmp(Name, "Lua");
}

void VRSDClientLuaImplementation::ErrorScriptEvent(VRSDScriptEvent* Event)
{
    VRSDScriptEventCallbackItem SECI(&ScriptEvent, Event);
    ScriptEvent.TriggerCallbacks(&SECI);

    // the data is now invalid
    m_pLuaState = NULL;
    m_pActivationRecord = NULL;
}

hkvResult VRSDClientLuaImplementation::LookupPath(VStringTokenizerInPlace& Tokenizer, const char** pLastField)
{
  const char* pCurrent = Tokenizer.Next();
  while(pCurrent)
  {
    if(VStringUtil::IsIntegerString(pCurrent))
    {
      lua_pushnumber(m_pLuaState, (LUA_NUMBER)atoi(pCurrent)); //stack: ..., localX, key, TOP
      lua_gettable(m_pLuaState, -2);               //stack:  ..., localX, {field}, TOP
    }
    else
    {
      lua_getfield(m_pLuaState, -1, pCurrent);     //stack: .., localX, {field}, TOP
    }
    if(lua_isnil(m_pLuaState, -1))
      return HKV_FAILURE;

    const char* pNext = Tokenizer.Next();
    if(pLastField != NULL && pNext == NULL)
      *pLastField = pCurrent;
    pCurrent = pNext;
  }
  return HKV_SUCCESS;
}


#if defined (_VISION_PS3)
#pragma diag_push
#pragma diag_suppress=1669
#endif


void VRSDClientLuaImplementation::Lua_DebugHook(lua_State* L, lua_Debug* ar)
{
  // get the implementation pointer
  VRSDClientLuaImplementation* Impl = (VRSDClientLuaImplementation*)VRSDClient::GetGlobalClient().GetClientLanguageImplementation();
  VASSERT(Impl);

  if(Impl->m_bDebuggerRetrievingValues)
    return;

  // gather information from the activation record
  if(lua_getinfo(L, "nSl", ar))
  {
    // save the current values for usage by the various data retrieval functions
    Impl->m_pLuaState = L;
    Impl->m_pActivationRecord = ar;

    // fields necessary for the script event
    VRSDScriptEvent::EventType eEventType = VRSDScriptEvent::EVENT_UNKNOWN;
    if(ar->event == LUA_HOOKCALL)
      eEventType = VRSDScriptEvent::EVENT_ENTER_FUNCTION;
    else if(ar->event == LUA_HOOKRET || ar->event == LUA_HOOKTAILRET)
      eEventType = VRSDScriptEvent::EVENT_LEAVE_FUNCTION;
    else if(ar->event == LUA_HOOKLINE)
      eEventType = VRSDScriptEvent::EVENT_LINE;
    else
      VASSERT(NULL && "Unknown lua debug event!");

    VRSDScriptEvent::ExecutionType eExecutionType;
    
    if(ar->what && ar->what[0] == 'C')
      eExecutionType = VRSDScriptEvent::EXECUTION_TYPE_NATIVE;
    else
      eExecutionType = VRSDScriptEvent::EXECUTION_TYPE_SCRIPT;

    VRSDScriptEvent SE(eEventType, ar->currentline, eExecutionType, GetCorrectLuaSourceString(ar->source), ar->name, ar->linedefined);

    // create callback item and trigger the callback which in turn will send the event to the remote debugger
    VRSDScriptEventCallbackItem SECI(&Impl->ScriptEvent, &SE);
    Impl->ScriptEvent.TriggerCallbacks(&SECI);

    // the data is now invalid
    Impl->m_pLuaState = NULL;
    Impl->m_pActivationRecord = NULL;
  }
}

#if defined (_VISION_PS3)
#pragma diag_pop
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
