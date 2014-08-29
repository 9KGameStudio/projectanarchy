/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/VScriptIncludes.hpp>
#include <Vision/Runtime/Base/String/VStringTokenizerInPlace.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VisionLuaModule_wrapper.hpp>


#define SCRIPTINSTANCES  VScriptResourceManager::GlobalManager().Instances()

#define SAFE_ADD_DYNARRAY(arr,element,counter) \
{\
  int iIndex = arr.GetElementPos(element);\
  if (iIndex<0)\
  {\
  iIndex = arr.GetFreePos();\
  counter = hkvMath::Max(counter,iIndex+1);\
  arr[iIndex] = element;\
  }\
}

#define SAFE_REMOVE_DYNARRAY(arr,element) \
{\
  int iIndex = arr.GetElementPos(element);\
  if (iIndex>=0)\
  arr.GetDataPtr()[iIndex] = NULL;\
}



VScriptInstance::VScriptInstance()
{
  m_bDisposeOnFinish = false;
  m_bFlagForDisposal = false;
  m_bHasSuspendedThreads = false;
  m_iCreatedThreads = 0;
}

VScriptInstance::~VScriptInstance()
{
  DiscardCreatedThreads();
}





bool VScriptInstance::Init(bool disposeOnFinsh)
{
  m_bDisposeOnFinish = disposeOnFinsh;
  return true;
}

bool VScriptInstance::DeInit()
{
  return true;
}

void VScriptInstance::WaitSeconds(lua_State *pState, float seconds)
{
  if (seconds <= 0.f)
    return;

  VLuaThreadInfo* pInfo = FindThread(pState);
  pInfo->fWaitTime += seconds;
  SCRIPTINSTANCES.AddWaitingObject(this);
}



void VScriptInstance::DisposeObject()
{
  // Make sure that the script instance is not assigned to a streaming zone anymore
  IVScriptInstance::DisposeObject();

  // And remove it from the manager
  SCRIPTINSTANCES.SafeRemove(this);
}



void VScriptInstance::OnDeserializationCallback(const VSerializationContext &context)
{
  if (m_spResource)
    ExecuteFunctionArg("OnDeserializationCallback", "*");
}


V_IMPLEMENT_SERIAL(VScriptInstance, VisTypedEngineObject_cl, 0, &g_VisionEngineModule);
void VScriptInstance::Serialize(VArchive &ar)
{
  IVScriptInstance::Serialize(ar);
  if (ar.IsLoading())
  {
    char iVersion;
    ar >> iVersion; VVERIFY(iVersion == 0 && "invalid version number");
    VScriptResource *pRes = (VScriptResource *)ar.ReadProxyObject();
    Init();
    SetResource(pRes);
    if (pRes)
    {
      VScriptResourceManager *pManager = (VScriptResourceManager *)pRes->GetParentManager();
      pManager->Instances().Add(this);
    }
  }
  else
  {
    ar << (char)0; // version
    ar.WriteProxyObject(m_spResource);
  }
}

VScriptResource* VScriptInstance::GetResource()
{
  return m_spResource;
}

bool VScriptInstance::SetResource(VScriptResource *pRes)
{
  m_spResource = pRes;
  if (m_spResource == NULL)
    return true;

  VASSERT(m_iCreatedThreads == 0);

  return true;
}


// for debugging:
const char *VScriptInstance::VLuaThreadInfo::GetStatusString(char *szBuffer) const
{
  char *szStart = szBuffer;
  szBuffer[0] = 0;
  switch (eState)
  {
  case Finished:szBuffer += sprintf(szBuffer, "Status:Finished"); break;
  case Running:szBuffer += sprintf(szBuffer, "Status:Running"); break;
  case Reuse:szBuffer += sprintf(szBuffer, "Status:Re-use"); break;
  }

  int iStackPos = lua_gettop(pThread);
  szBuffer += sprintf(szBuffer, " Stackpos:%i Sleep:%.2fs", iStackPos, fWaitTime);
  return szStart;
}



VScriptInstance::VLuaThreadInfo* VScriptInstance::FindThread(lua_State *pThread) const
{
  for (int i = 0; i<m_iCreatedThreads; i++)
    if (m_CreatedThreads.GetDataPtr()[i].pThread == pThread)
      return &m_CreatedThreads.GetDataPtr()[i];
  VASSERT(!"Invalid thread passed");
  return NULL;
}


void VScriptInstance::DiscardCreatedThreads()
{
  while (m_iCreatedThreads>0)
    DiscardThread(m_CreatedThreads.GetDataPtr()[m_iCreatedThreads - 1].pThread, false);
}


void VScriptInstance::DiscardThread(lua_State *pThread, bool bForReuse)
{
  VISION_PROFILE_FUNCTION(PROFILING_SCRIPTOBJ_DISCARDTHREAD);

  if (bForReuse)
  {
    VScriptInstance::VLuaThreadInfo* pInfo = FindThread(pThread);
    pInfo->eState = Reuse;
  }
  else
  {
    // clean up the list
    int iOldCount = m_iCreatedThreads;
    m_iCreatedThreads = 0;
    for (int i = 0; i < iOldCount; i++)
    {
      VLuaThreadInfo &info = m_CreatedThreads.GetDataPtr()[i];
      if (info.pThread == pThread)
      {
        //if we do not delete the thread a light user data object
        //will remain inside the Lua globals
        VScriptResourceManager::DiscardThread(info.pThread);

        luaL_unref(info.pParentThread, LUA_REGISTRYINDEX, info.iRefKey);
        info.pThread = NULL;
        continue;
      }
      m_CreatedThreads[m_iCreatedThreads++] = info; //comment by Thomas: When does it happen...?
    }
  }
}


VScriptInstance::VLuaThreadInfo* VScriptInstance::CreateNewThread()
{
  VISION_PROFILE_FUNCTION(PROFILING_SCRIPTOBJ_CREATETHREAD);

  // see if there are still cached threads
  for (int i = 0; i < m_iCreatedThreads; i++)
  {
    VLuaThreadInfo &info = m_CreatedThreads.GetDataPtr()[i];
    if (info.eState == Reuse)
    {
      info.eState = Running;
      info.fWaitTime = 0.f;
#ifdef PROFILING
      VScriptResourceManager::g_iThreadsRecycled++;
#endif
      return &info;
    }
  }

  // create new thread
  //  lua_State* pParentState = m_MainState.pThread;
  lua_State* pParentState = m_spResource->m_pResourceState;
  if (pParentState == NULL)
    return NULL;

  VLuaThreadInfo &info = m_CreatedThreads[m_iCreatedThreads++];
  info.pParentThread = pParentState;
  info.pThread = lua_newthread(info.pParentThread);
  info.iRefKey = luaL_ref(info.pParentThread, LUA_REGISTRYINDEX); //reference the thread from the registry for GC
  info.eState = Running;
  info.fWaitTime = 0.f;

  // Associate the thread with this script instance
  // We need this so we can find out which object a native library function is called from
  VScriptResourceManager::SetScriptInstanceForState(info.pThread, this);

#ifdef PROFILING
  VScriptResourceManager::g_iThreadsCreated++;
#endif

  return &info;
}


void VScriptInstance::Tick(float dtime)
{
  VISION_PROFILE_FUNCTION(PROFILING_SCRIPTOBJ_TICK);

  m_bHasSuspendedThreads = false;
  VLuaThreadInfo *pInfo = m_CreatedThreads.GetDataPtr();
  for (int i = 0; i<m_iCreatedThreads; i++, pInfo++)
  {
    if (pInfo->eState != Running)
      continue;

    float &fTime = pInfo->fWaitTime;
    if (fTime>0.f)
    {
      fTime -= dtime;
      if (fTime <= 0.f)
      {
        fTime = 0.f;
        int status = lua_resume(pInfo->pThread, 0);
        if (status != LUA_YIELD)
        {
          pInfo->eState = Reuse; // same as in DiscardThread
          VScriptResourceManager::LuaErrorCheck(pInfo->pThread, status);
          continue;
        }
        pInfo->eState = Running;
      }
    }
    m_bHasSuspendedThreads = true;
  }
}

void VScriptInstance::ResumeAll(float dtime)
{
  VASSERT(!"TODO");

}

#define LUA_WARNING(s)             {hkvLog::Warning(s);}
#define LUA_WARNING_1(s,a1)        {hkvLog::Warning(s,a1);}
#define LUA_WARNING_2(s,a1,a2)     {hkvLog::Warning(s,a1,a2);}
#define LUA_WARNING_3(s,a1,a2,a3)  {hkvLog::Warning(s,a1,a2,a3);}

BOOL VScriptInstance::HasFunction(const char *szFunction)
{
  if (m_spResource == NULL || m_spResource->m_pResourceState == NULL)
    return FALSE;
  lua_State* L = m_spResource->m_pResourceState;

  lua_getfield(L, LUA_GLOBALSINDEX, szFunction);
  BOOL bFound = !lua_isnil(L, -1);
  lua_pop(L, 1);
  return bFound;
}

VScriptInstance::VLuaThreadInfo *VScriptInstance::PrepareFunctionCall(const char *szFunction)
{
  // create new LUA thread based on resource's main state
  VLuaThreadInfo *pInfo = CreateNewThread(); // TODO: maybe have a thread pool per resource?
  if (pInfo == NULL)
    return NULL;
  lua_State *pLuaState = pInfo->pThread;


  VMemoryTempBuffer<256> copyBuffer(szFunction); // operate on a copy string in the tokenizer

  VStringTokenizerInPlace Tokenizer(copyBuffer.AsChar(), '.');
  char* pCurrent = Tokenizer.Next();
  int i = 0;

  while (pCurrent)
  {
    if (i == 0)
    {
      lua_getfield(pLuaState, LUA_GLOBALSINDEX, pCurrent);
    }
    else
    {
      lua_pushstring(pLuaState, pCurrent);
      lua_gettable(pLuaState, -2);
    }


    if (lua_isnil(pLuaState, -1))
    {
      //not found
      lua_pop(pLuaState, (i + 1));
      DiscardThread(pLuaState, false);
      return NULL;
    }

    pCurrent = Tokenizer.Next();
    i++;
  }

  /*
    lua_getfield(pLuaState, LUA_GLOBALSINDEX, szFunction);
    if (lua_isnil(pLuaState, -1))
    {
    //not found
    lua_pop(pLuaState, 1);
    return NULL;
    }

    */

  return pInfo;
}

bool VScriptInstance::DoFunctionCall(VLuaThreadInfo *pInfo, int narg)
{
  bool bResult = true;
  lua_State *pLuaState = pInfo->pThread;

  int status = lua_resume(pLuaState, narg); // always use resume (behaves like call the first time)

  // process function call result
  if (status == LUA_YIELD)
  {
    // TODO: Add non-waiting objects to yield list of collection
    m_bHasSuspendedThreads = true;
    pInfo->eState = Running;
  }
  else
  {
    // remove again from list
    bResult = VScriptResourceManager::LuaErrorCheck(pLuaState, status);
    pInfo->eState = Reuse;

    // Note: upon an error, the thread saves the error state and the instruction pointer of the called
    // body function. This function can't be resumed next time and clearing this state requires altering
    // the Lua State itself.
    // For now we just discard such a thread. Since people shouldn't program errors this shouldn't have
    // much of a performance impact.
    if (!bResult) {
      //pLuaState->status = 0; // clear error status
      DiscardThread(pLuaState, false);
    }
  }

  return bResult;
}

BOOL VScriptInstance::ExecuteFunctionArgV(const char *szFunction, const char *szArgFormat, va_list argPtr)
{
  VISION_PROFILE_FUNCTION(PROFILING_SCRIPTOBJ_EXECUTEFUNCTION);
#ifdef PROFILING
  VScriptResourceManager::g_iFunctionsCalled++;
#endif

  int narg; // number of arguments

  //TODO: early out before creating thread?
  //if (!HasFunction(szFunction)) return FALSE;

  VLuaThreadInfo *pInfo = PrepareFunctionCall(szFunction);
  if (!pInfo)
    return FALSE;

  lua_State *pLuaState = pInfo->pThread;

  int iNestedTable = 0;
  int pNestedTables[ARGV_MAX_NESTED_TABLES];

  int iArrayIndex = 0;
  void **ppArray = NULL;

  // put parameter on the stack:
  narg = 0;
  while (szArgFormat && szArgFormat[0])
  {
    char argFormat = *szArgFormat++;
    switch (argFormat)
    {
    case '*':  // method call so push "this". (Should be the first argument)
    {
      LUA_PushObjectProxy(pLuaState, (VScriptComponent*)m_pParentComponent);
    }
      break;
    case 'u':  // light user data as void *
    {
      lua_pushlightuserdata(pLuaState, ppArray == NULL ? va_arg(argPtr, void*) : (ppArray[iArrayIndex++]));
    }
      break;
    case 'o':  // typed object
    {
      VisTypedEngineObject_cl* pObj = (VisTypedEngineObject_cl*)(ppArray == NULL ? va_arg(argPtr, VisTypedEngineObject_cl*) : (ppArray[iArrayIndex++]));
      VScriptComponent* pComp = NULL;
      if (pObj)
        pComp = VScriptResourceManager::GetScriptComponent(pObj);

      if (pComp)
        LUA_PushObjectProxy(pLuaState, pComp);
      else
        lua_pushnil(pLuaState);
    }
      break;
    case 't': // something derived from VTypedObject with an existing wrapper and cast
      LUA_CreateNewWrapper(pLuaState, ppArray == NULL ? va_arg(argPtr, VTypedObject*) : (VTypedObject*)(ppArray[iArrayIndex++]));
      break;
    case 'r':  // object by registry index
    {
      int id = ppArray == NULL ? va_arg(argPtr, int) : *((int *)(ppArray[iArrayIndex++]));
      if (id >= 0)
        lua_rawgeti(pLuaState, LUA_REGISTRYINDEX, id);
      else
        lua_pushnil(pLuaState);
    }
      break;
    case 'T': // arbitrary type specified with 'T' MODULENAME + ':' + TYPENAME + ';'
    {
      const char * pColon = Contains(szArgFormat, ":");
      const char * pSemiColon = Contains(szArgFormat, ";");

      if (pColon != NULL && pSemiColon != NULL)
      {
        char szModuleName[128];
        char szTypeName[128];

        vstrncpy(szModuleName, szArgFormat, (size_t)(pColon - szArgFormat) + 1);
        vstrncpy(szTypeName, &pColon[1], (size_t)(pSemiColon - pColon));

        int iRetParams = LUA_CallStaticFunction(pLuaState, szModuleName, szTypeName, "Cast", "v>v", ppArray == NULL ? va_arg(argPtr, unsigned long*) : (ppArray[iArrayIndex++]));
        VASSERT_MSG(iRetParams == 1, "LUA_CallStaticFunction returned an unexpected amount of results!");

        szArgFormat = &pSemiColon[1]; //advance to the last position
      }
      else
      {
        LUA_WARNING_1("Invalid type specified after T '%s'", szArgFormat);
        lua_pushnil(pLuaState);
        goto endwhile;
      }
    }
      break;
    case 'd':  // double argument
      lua_pushnumber(pLuaState, (lua_Number)(ppArray == NULL ? va_arg(argPtr, double) : *((double *)(ppArray[iArrayIndex++]))));
      break;
    case 'f':  // float argument
      lua_pushnumber(pLuaState, (lua_Number)(ppArray == NULL ? va_arg(argPtr, double /* float is promoted to double when passing via '...' */) : *((float *)(ppArray[iArrayIndex++]))));
      break;
    case 'i': // int argument
      lua_pushnumber(pLuaState, (lua_Number)(ppArray == NULL ? va_arg(argPtr, int) : *((int *)(ppArray[iArrayIndex++]))));
      break;
    case 'b': // boolean argument
      lua_pushboolean(pLuaState, ppArray == NULL ? va_arg(argPtr, int) : *((int *)(ppArray[iArrayIndex++])));
      break;
    case 's': // string argument
      lua_pushstring(pLuaState, ppArray == NULL ? va_arg(argPtr, char *) : (char *)(ppArray[iArrayIndex++]));
      break;
    case 'm': // bitmask argument
      LUA_PushBitmask(pLuaState, ppArray == NULL ? va_arg(argPtr, int) : *((int *)(ppArray[iArrayIndex++])));
      break;
    case 'v': // vector argument 
    {
      if (strlen(szArgFormat) > 0)
      {
        const char nextChar = *szArgFormat++;
        switch (nextChar)
        {
        case '2':
          LUA_PushObjectProxy(pLuaState, ppArray == NULL ? va_arg(argPtr, hkvVec2*) : (hkvVec2 *)(ppArray[iArrayIndex++]));
          break;
        case '3':
          LUA_PushObjectProxy(pLuaState, ppArray == NULL ? va_arg(argPtr, hkvVec3*) : (hkvVec3 *)(ppArray[iArrayIndex++]));
          break;
        case '4':
          LUA_PushObjectProxy(pLuaState, ppArray == NULL ? va_arg(argPtr, hkvVec4*) : (hkvVec4 *)(ppArray[iArrayIndex++]));
          break;
        default:
          LUA_PushObjectProxy(pLuaState, ppArray == NULL ? va_arg(argPtr, hkvVec3*) : (hkvVec3 *)(ppArray[iArrayIndex++]));
          szArgFormat--;
        }
      }
      else
      {
        LUA_PushObjectProxy(pLuaState, ppArray == NULL ? va_arg(argPtr, hkvVec3*) : (hkvVec3 *)(ppArray[iArrayIndex++]));
      }
    }
      break;
    case 'x': // matrix argument 
    {
      if (strlen(szArgFormat) > 0)
      {
        const char nextChar = *szArgFormat++;
        switch (nextChar)
        {
        case '3':
          LUA_PushObjectProxy(pLuaState, ppArray == NULL ? va_arg(argPtr, hkvMat3*) : (hkvMat3 *)(ppArray[iArrayIndex++]));
          break;
        case '4':
          LUA_PushObjectProxy(pLuaState, ppArray == NULL ? va_arg(argPtr, hkvMat4*) : (hkvMat4 *)(ppArray[iArrayIndex++]));
          break;
        default:
          LUA_WARNING_1("Invalid type specified after x: '%c'", nextChar);
          szArgFormat--;
          lua_pushnil(pLuaState);
        }
      }
      else
      {
        LUA_WARNING("No type specified after x");
        lua_pushnil(pLuaState);
      }
    }
      break;
    case 'p': // plane argument
      LUA_PushObjectProxy(pLuaState, ppArray == NULL ? va_arg(argPtr, hkvPlane*) : (hkvPlane *)(ppArray[iArrayIndex++]));
      break;
    case 'q': // quaternion argument
      LUA_PushObjectProxy(pLuaState, ppArray == NULL ? va_arg(argPtr, hkvQuat*) : (hkvQuat *)(ppArray[iArrayIndex++]));
      break;
    case '#': // bounding box argument
      LUA_PushObjectProxy(pLuaState, ppArray == NULL ? va_arg(argPtr, hkvAlignedBBox*) : (hkvAlignedBBox *)(ppArray[iArrayIndex++]));
      break;
    case '@': // bounding sphere argument
      LUA_PushObjectProxy(pLuaState, ppArray == NULL ? va_arg(argPtr, hkvBoundingSphere*) : (hkvBoundingSphere *)(ppArray[iArrayIndex++]));
      break;
    case 'c': // color argument
      LUA_PushObjectProxy(pLuaState, ppArray == NULL ? va_arg(argPtr, VColorRef*) : (VColorRef *)(ppArray[iArrayIndex++]));
      break;
    case '[':
      VASSERT_MSG(ppArray == NULL, "Nested array parameters are not allowed!");
      ppArray = va_arg(argPtr, void**);
      VASSERT_MSG(ppArray != NULL, "NULL array parameters is not allowed!");
      pNestedTables[iNestedTable] = 0;
      iNestedTable++;
      break;
    case ']':
      VASSERT_MSG(ppArray != NULL, "Closed array before opening it! Check '[' and ']' in your argument string!");
      ppArray = NULL;
      iNestedTable--;
      narg--; //this is no real argument...!
      break;
    case '{':
      VASSERT_MSG(iNestedTable<ARGV_MAX_NESTED_TABLES, "Exceeded max depth for nested Lua tables!");
      pNestedTables[iNestedTable] = 0;
      iNestedTable++;
      break;
    case '}':
      VASSERT_MSG(iNestedTable>0, "Closed table before opening it! Check '{' and '}' in your argument string!");
      iNestedTable--;
      narg--; //this is no real argument...!
      break;
    case '>': // now start with output
      goto endwhile;

    default:
      LUA_WARNING_2("Invalid type specifier '%c' for function call '%s'", argFormat, szFunction)
    }

    //create table or set a table element
    if (iNestedTable > 0)
    {
      if (pNestedTables[iNestedTable - 1] == 0)
      {
        lua_newtable(pLuaState);
      }
      else
      {
        lua_rawseti(pLuaState, -2, pNestedTables[iNestedTable - 1]);
        narg--; //table internal argument (does not count for the overall amount)
      }
      pNestedTables[iNestedTable - 1]++;
    }

    narg++;

    luaL_checkstack(pLuaState, 1, "too many arguments");
  }
endwhile:

  VASSERT_MSG(iNestedTable == 0, "Nested table structure invalid, check '{' / '[' and '}' / ']' in your argument string!");
  const bool bResult = DoFunctionCall(pInfo, narg);
  if (!bResult)
    return FALSE;

  // calculate number of expected results
  int numResults = 0;
  if (szArgFormat != NULL)
  {
    const size_t iLength = strlen(szArgFormat);
    for (size_t i = 0; i < iLength; i++)
    {
      if (isdigit(szArgFormat[i]) == 0)
        numResults++;
    }
  }

  int iStackIndex = -numResults;

  // now read back expected results
  unsigned int iIndex = 0;
  while (szArgFormat && szArgFormat[0])
  {
    char argFormat = *szArgFormat++;
    switch (argFormat)
    {
    case 'u':  // light user data result
      if (!lua_islightuserdata(pLuaState, iStackIndex))
        LUA_WARNING_2("Return type 'light user data (void *)' expected for function call '%s', return value %i", szFunction, iIndex)
      else
      *va_arg(argPtr, void **) = lua_touserdata(pLuaState, iStackIndex); //there is no lua_tolightuserdata, so lua_touserdata is the only way
      break;
    case 'd':  // double result
      if (!lua_isnumber(pLuaState, iStackIndex))
        LUA_WARNING_2("Return type 'double' expected for function call '%s', return value %i", szFunction, iIndex)
      else
      *va_arg(argPtr, double *) = lua_tonumber(pLuaState, iStackIndex);
      break;
    case 'f':  // float result
      if (!lua_isnumber(pLuaState, iStackIndex))
        LUA_WARNING_2("Return type 'float' expected for function call '%s', return value %i", szFunction, iIndex)
      else
      *va_arg(argPtr, float *) = (float)lua_tonumber(pLuaState, iStackIndex);
      break;

    case 'i':  // int result
      if (!lua_isnumber(pLuaState, iStackIndex))
        LUA_WARNING_2("Return type 'int' expected for function call '%s', return value %i", szFunction, iIndex)
      else
      *va_arg(argPtr, int *) = (int)lua_tonumber(pLuaState, iStackIndex);
      break;

    case 's':  // string result
      if (!lua_isstring(pLuaState, iStackIndex))
        LUA_WARNING_2("Return type 'string' expected for function call '%s', return value %i", szFunction, iIndex)
      else
      *va_arg(argPtr, const char **) = lua_tostring(pLuaState, iStackIndex);
      break;

    case 'v': // vector result
    {
      const char *szWarning = NULL;
      if (strlen(szArgFormat) > 0)
      {
        const char nextChar = *szArgFormat++;
        switch (nextChar)
        {
        case '2':
          szWarning = LUA_GetValue(pLuaState, iStackIndex, *((hkvVec2*)va_arg(argPtr, hkvVec2*))) ? NULL : "hkvVec2";
          break;
        case '3':
          szWarning = LUA_GetValue(pLuaState, iStackIndex, *((hkvVec3*)va_arg(argPtr, hkvVec3*))) ? NULL : "hkvVec3";
          break;
        case '4':
          szWarning = LUA_GetValue(pLuaState, iStackIndex, *((hkvVec4*)va_arg(argPtr, hkvVec4*))) ? NULL : "hkvVec4";
          break;
        default:
          szWarning = LUA_GetValue(pLuaState, iStackIndex, *((hkvVec3*)va_arg(argPtr, hkvVec3*))) ? NULL : "hkvVec3";
          szArgFormat--;
        }
      }
      else
      {
        szWarning = LUA_GetValue(pLuaState, iStackIndex, *((hkvVec3*)va_arg(argPtr, hkvVec3*))) ? NULL : "hkvVec3";
      }
      if (szWarning != NULL)
        LUA_WARNING_3("Return type '%s' expected for function call '%s', return value %i", szWarning, szFunction, iIndex)
    }
      break;

    case 'x': // matrix result 
    {
      const char *szWarning = NULL;
      if (strlen(szArgFormat) > 0)
      {
        const char nextChar = *szArgFormat++;
        switch (nextChar)
        {
        case '3':
          szWarning = LUA_GetValue(pLuaState, iStackIndex, *((hkvMat3*)va_arg(argPtr, hkvMat3*))) ? NULL : "hkvMat3";
          break;
        case '4':
          szWarning = LUA_GetValue(pLuaState, iStackIndex, *((hkvMat4*)va_arg(argPtr, hkvMat4*))) ? NULL : "hkvMat4";
          break;
        default:
          LUA_WARNING_1("Invalid type specified after x: '%c'", nextChar);
          szArgFormat--;
        }
      }
      else
      {
        LUA_WARNING_1("No type specified after x '%s'", szArgFormat);
      }
      if (szWarning != NULL)
        LUA_WARNING_3("Return type '%s' expected for function call '%s', return value %i", szWarning, szFunction, iIndex)
    }
      break;

    case 'p': // plane result
      if (!LUA_GetValue(pLuaState, iStackIndex, *((hkvPlane*)va_arg(argPtr, hkvPlane *))))
        LUA_WARNING_2("Return type 'hkvPlane' expected for function call '%s', return value %i", szFunction, iIndex)
        break;

    case 'q': // quaternion result
      if (!LUA_GetValue(pLuaState, iStackIndex, *((hkvQuat*)va_arg(argPtr, hkvQuat *))))
        LUA_WARNING_2("Return type 'hkvQuat' expected for function call '%s', return value %i", szFunction, iIndex)
        break;

    case '#': // bounding box result
      if (!LUA_GetValue(pLuaState, iStackIndex, *((hkvAlignedBBox*)va_arg(argPtr, hkvAlignedBBox *))))
        LUA_WARNING_2("Return type 'hkvAlignedBBox' expected for function call '%s', return value %i", szFunction, iIndex)
        break;

    case '@': // bounding sphere result
      if (!LUA_GetValue(pLuaState, iStackIndex, *((hkvBoundingSphere*)va_arg(argPtr, hkvBoundingSphere *))))
        LUA_WARNING_2("Return type 'hkvBoundingSphere' expected for function call '%s', return value %i", szFunction, iIndex)
        break;

    case 't': // typed engine object or derived
    {
      VTypedObject *pTypedObject = NULL;
      if (!LUA_GetValue(pLuaState, iStackIndex, pTypedObject))
      {
        LUA_WARNING_2("Return type 'VTypedObject' expected for function call '%s', return value %i", szFunction, iIndex)
      }
      *((VTypedObject**)va_arg(argPtr, VTypedObject**)) = pTypedObject;
    }
      break;

    case 'b':  // bool result
      if (!lua_isboolean(pLuaState, iStackIndex))
        LUA_WARNING_2("Return type 'bool' expected for function call '%s', return value %i", szFunction, iIndex)
      else
      *va_arg(argPtr, int *) = lua_toboolean(pLuaState, iStackIndex) != 0;
      break;

    case 'c': // color result
      if (!LUA_GetValue(pLuaState, iStackIndex, *((VColorRef*)va_arg(argPtr, VColorRef *))))
        LUA_WARNING_2("Return type 'VColorRef' expected for function call '%s', return value %i", szFunction, iIndex)
        break;

    default:
      LUA_WARNING_2("Invalid return type specifier '%c' for function call '%s'", argFormat, szFunction)

    }
    iIndex++;
    iStackIndex++;
  }

  lua_settop(pLuaState, 0);  //clear the stack

#ifdef PROFILING
  if (!bResult)
    VScriptResourceManager::g_iFunctionsFailed++;
#endif

  return bResult == true;
}


BOOL VScriptInstance::RunScriptCode(const char *szText, bool bUseGlobals /* = false */)
{
  if (!szText)
    return TRUE;

  int iLen = (int)strlen(szText);
  if (iLen == 0)
    return TRUE;

  VASSERT_MSG(m_spResource, "Script manager not initialized");
  lua_State* pResourceState = m_spResource->m_pResourceState;

  //TODO: Use the thread pool instead?  That would allow co-routines to be started

  //Create a new thread 
  lua_State *pTempThread = lua_newthread(pResourceState);

  //Do we want a locals table? Without a locals table the script will be the same as
  //a function call (ExecuteFunctionArg) and be able to modify the resource's state. 
  //Make own locals table for pTempThread so it doesn't pollute the parent state
  if (!bUseGlobals) LUA_CreateLocalsTable(pTempThread);

  // Set "self" as a global object on the new state, so that the script code can access it
  LUA_PushObjectProxy(pTempThread, (VScriptComponent*)m_pParentComponent);  // push self (LUA_PushObjectProxy will push the owner of the component)
  lua_setglobal(pTempThread, "self");

  // load the string
  if (!VScriptResourceManager::LuaErrorCheck(pTempThread, luaL_loadstring(pTempThread, szText)))
    goto FAIL;

  // run the string
  if (!VScriptResourceManager::LuaErrorCheck(pTempThread, lua_pcall(pTempThread, 0, LUA_MULTRET, 0)))
    goto FAIL;

  // To be safe: remove the global "self" variable again, since I am not sure whether it might
  // otherwise be available in the master state as well 
  lua_pushnil(pTempThread);
  lua_setglobal(pTempThread, "self");

  lua_pop(pResourceState, 1); // do not leave thread on the stack
  return TRUE;

FAIL:
  lua_pop(pResourceState, 1); // do not leave thread on the stack
  return FALSE;
}

void VScriptInstance::ExecuteCustomMembers(LinkedList_cl<VScriptMember> &members)
{
  VASSERT_MSG(m_spResource, "Script manager not initialized");

  if (!m_pParentComponent)
    return;

  int iSize = members.GetSize();

  if (iSize < 1)
    return;

  lua_State* m_pLuaState = m_spResource->m_pResourceState;
  VASSERT(m_pLuaState);

  //generate Lua code
  VString sBuffer = "";
  for (int i = 0; i < iSize; i++)
  {
    sBuffer += "self.";
    sBuffer += members[i].GetName();
    sBuffer += "=";
    sBuffer += members[i].GetValue();
    sBuffer += "\n\r";
  }

  //execute the Lua code
  RunScriptCode(sBuffer.AsChar(), true);
}

void VScriptInstance::TriggerOnExpose(LinkedList_cl<VScriptMember>& existingMembers)
{
   VLuaThreadInfo* pInfo = PrepareFunctionCall("OnExpose");                                                // OnExpose, TOP

   if(!pInfo)
     return;

   lua_State* L = pInfo->pThread;

   VLuaStackCleaner stackCleaner(L);

  // Create a temp table to place the new properties into
  lua_newtable(L);                                                                                         // OnExpose, temptable

  // Duplicate function call and table so we can keep the table after the function call
  lua_pushvalue(L, -2);
  lua_pushvalue(L, -2);                                                                                    // OnExpose, temptable, OnExpose
                                                                                                           // OnExpose, temptable, OnExpose, temptable
  if(!DoFunctionCall(pInfo, 1))
    return;
                                                                                                           // OnExpose, temptable
  LUA_PushObjectProxy(L, static_cast<VScriptComponent*>(m_pParentComponent));                              // OnExpose, temptable, wrapper

  lua_pushnil(L);                                                                                          // OnExpose, temptable, wrapper, nil
  while (lua_next(L, -3))                                                                                  // OnExpose, temptable, wrapper, key, value
  {
    bool bKeyExists = false;

    const char* pKey = lua_tostring(L, -2);
    // Is there an existing dynamic property table?
    for (LinkedList_Element_cl<VScriptMember>* pCurrent = existingMembers.first; pCurrent; pCurrent = pCurrent->next)
    {
      if (strcmp(pCurrent->value.GetName(), pKey) == 0)
      {
        bKeyExists = true;
        hkvLog::Error("Dynamic property '%s' in '%s' was previously exposed by another script component - new value will be ignored.", pKey, this->GetResource()->GetFilename());
        break;
      }
    }

    if (!bKeyExists)
    {
      lua_pushvalue(L, -2);                                                                                  // OnExpose, temptable, wrapper, key, value, key
      lua_pushvalue(L, -2);                                                                                  // OnExpose, temptable, wrapper, key, value, key, value

      // Copy the value to the wrapper object
      lua_settable(L, -5);                                                                                   // OnExpose, temptable, wrapper, key, value
    }
    lua_pop(L, 1);
  }
}

#define PROFILE_GLOBALS_IN_GET_MEMBERS
#undef PROFILE_GLOBALS_IN_GET_MEMBERS
void VScriptInstance::GetMembers(LinkedList_cl<VScriptMember> &members)
{
  VASSERT_MSG(m_spResource, "Script manager not initialized");

  if (!m_pParentComponent || m_pParentComponent->GetOwner() == NULL)
  {
    return;
  }

  lua_State* L = m_spResource->m_pResourceState;
  VASSERT(L);

  VLuaStackCleaner stackCleaner(L);

  LUA_LookupObjectProxy(L, m_pParentComponent->GetOwner());

  if (lua_isnil(L, -1))
  {
    return;
  }

  LUA_FetchDynPropertyTable(L);

  if (lua_isnil(L, -1))
  {
    return;
  }

  //start global iter
  VLuaTableIterator globalsIter(L, -1);
  while (globalsIter.HasNext())
  {
    VScriptMember* globalVar = globalsIter.Next();

    // Skip internal members
    if (strstr(globalVar->GetName(), "__") == globalVar->GetName())
    {
      continue;
    }

    const int iBufferLength = 256;
    char szBuffer[iBufferLength] = "";


    //create a stub element to search inside the dyn array
    const char * szFormatString = "Lua Type: %s";
    VASSERT_MSG(strlen(globalVar->GetType()) < iBufferLength - strlen(szFormatString), "Max buffer size for data type name exceeded!");
    sprintf(szBuffer, szFormatString, globalVar->GetType());

    VScriptMember elem(globalVar->GetName(), NULL, szBuffer);

    //VScriptMember compares just names
    int iIndex = -1;
    for (int i = 0; i < members.GetSize(); i++)
    {
      if (members[i] == elem)
      {
        if (VStringHelper::SafeCompare(members[i].GetType(), elem.GetType(), true) == 0)
          iIndex = i;
        else
          members.Remove(members[i]);//detected a different type!!
        break;
      }
    }

    //element not present
    if (iIndex < 0)
    {
      members.Add(elem);
      iIndex = members.GetSize() - 1; //do not cache the size, maybe one element has been removed
    }

    //type detection
    switch (lua_type(L, -1))
    {
    case LUA_TBOOLEAN:
    {
      //extra handling of boolean since the tostring operation does not work here
      members[iIndex].SetValue(lua_toboolean(L, -1) == TRUE ? "true" : "false");
    }
      break;

    case LUA_TSTRING:
    {
      //add quotes to separate string values from other values
      const char *szString = lua_tostring(L, -1);
      //because we quote the string the buffer needs to more characters
      VASSERT_MSG(strlen(szString) < iBufferLength - 2, "Lua string parameter buffer size exceeded!");
      sprintf(szBuffer, "\"%s\"", szString);

      members[iIndex].SetValue(szBuffer);
    }
      break;

    default:
    {
      //the rest except user data
      members[iIndex].SetValue(lua_tostring(L, -1));
    }
      break;

    case LUA_TUSERDATA:
    {
      //in case we found a wrapped object... get the pointer of it
      swig_type_info* type = (swig_type_info *)LUA_GetSwigType(L, -1);
      if (type)
      {
        swig_lua_userdata *pUserData = (swig_lua_userdata *)lua_touserdata(L, -1);

        if (pUserData == NULL || pUserData->ptr == NULL)
        {
          members[iIndex].SetValue("nil");
        }
        else if (type == SWIGTYPE_p_hkvVec2)
        {
          //set the pointer as value
          hkvVec2 *pVec = (hkvVec2 *)pUserData->ptr;
          sprintf(szBuffer, "Vision.hkvVec2(%0.0f,%0.0f)", pVec->x, pVec->y);
          //the rest except user data
          members[iIndex].SetValue(szBuffer);
        }
        else if (type == SWIGTYPE_p_hkvVec3)
        {
          //set the pointer as value
          hkvVec3 *pVec = (hkvVec3 *)pUserData->ptr;
          sprintf(szBuffer, "Vision.hkvVec3(%0.0f,%0.0f,%0.0f)", pVec->x, pVec->y, pVec->z);
          //the rest except user data
          members[iIndex].SetValue(szBuffer);
        }
        else if (type == SWIGTYPE_p_hkvVec4)
        {
          //set the pointer as value
          hkvVec4 *pVec = (hkvVec4 *)pUserData->ptr;
          sprintf(szBuffer, "Vision.hkvVec4(%0.0f,%0.0f,%0.0f,%0.0f)", pVec->x, pVec->y, pVec->z, pVec->w);
          //the rest except user data
          members[iIndex].SetValue(szBuffer);
        }
        else if (type == SWIGTYPE_p_hkvMat3)
        {
          //set the pointer as value
          hkvMat3 *pMat = (hkvMat3 *)pUserData->ptr;
          sprintf(szBuffer,
            "Vision.hkvMat3(%0.0f,%0.0f,%0.0f, %0.0f,%0.0f,%0.0f, %0.0f,%0.0f,%0.0f, true)",
            pMat->m_ElementsCM[0], pMat->m_ElementsCM[1], pMat->m_ElementsCM[2],
            pMat->m_ElementsCM[3], pMat->m_ElementsCM[4], pMat->m_ElementsCM[5],
            pMat->m_ElementsCM[6], pMat->m_ElementsCM[7], pMat->m_ElementsCM[8]);
          //the rest except user data
          members[iIndex].SetValue(szBuffer);
        }
        else if (type == SWIGTYPE_p_hkvMat4)
        {
          //set the pointer as value
          hkvMat4 *pMat = (hkvMat4 *)pUserData->ptr;
          sprintf(szBuffer,
            "Vision.hkvMat4(%0.0f,%0.0f,%0.0f,%0.0f, %0.0f,%0.0f,%0.0f,%0.0f, %0.0f,%0.0f,%0.0f,%0.0f, %0.0f,%0.0f,%0.0f,%0.0f, true)",
            pMat->m_ElementsCM[0], pMat->m_ElementsCM[1], pMat->m_ElementsCM[2], pMat->m_ElementsCM[3],
            pMat->m_ElementsCM[4], pMat->m_ElementsCM[5], pMat->m_ElementsCM[6], pMat->m_ElementsCM[7],
            pMat->m_ElementsCM[8], pMat->m_ElementsCM[9], pMat->m_ElementsCM[10], pMat->m_ElementsCM[11],
            pMat->m_ElementsCM[12], pMat->m_ElementsCM[13], pMat->m_ElementsCM[14], pMat->m_ElementsCM[15]);
          //the rest except user data
          members[iIndex].SetValue(szBuffer);
        }
        else if (type == SWIGTYPE_p_hkvPlane)
        {
          //set the pointer as value
          hkvPlane *pPlane = (hkvPlane *)pUserData->ptr;
          sprintf(szBuffer, "Vision.hkvPlane(%0.0f,%0.0f,%0.0f,%0.0f)", pPlane->m_vNormal.x, pPlane->m_vNormal.y,
            pPlane->m_vNormal.z, pPlane->m_fNegDist);
          //the rest except user data
          members[iIndex].SetValue(szBuffer);
        }
        else if (type == SWIGTYPE_p_hkvQuat)
        {
          //set the pointer as value
          hkvQuat *pQuat = (hkvQuat *)pUserData->ptr;
          sprintf(szBuffer, "Vision.hkvQuat(%0.0f,%0.0f,%0.0f,%0.0f)", pQuat->getX(), pQuat->getY(), pQuat->getZ(), pQuat->getW());
          //the rest except user data
          members[iIndex].SetValue(szBuffer);
        }
        else if (type == SWIGTYPE_p_hkvAlignedBBox)
        {
          //set the pointer as value
          hkvAlignedBBox *pBBox = (hkvAlignedBBox *)pUserData->ptr;
          sprintf(szBuffer, "Vision.hkvAlignedBBox(%0.0f,%0.0f,%0.0f, %0.0f,%0.0f,%0.0f)", pBBox->m_vMin.x, pBBox->m_vMin.y,
            pBBox->m_vMin.z, pBBox->m_vMax.x, pBBox->m_vMax.y, pBBox->m_vMax.z);
          //the rest except user data
          members[iIndex].SetValue(szBuffer);
        }
        else if (type == SWIGTYPE_p_hkvBoundingSphere)
        {
          //set the pointer as value
          hkvBoundingSphere *pBSphere = (hkvBoundingSphere *)pUserData->ptr;
          sprintf(szBuffer, "Vision.hkvBoundingSphere(%0.0f,%0.0f,%0.0f, %0.0f)", pBSphere->m_vCenter.x,
            pBSphere->m_vCenter.y, pBSphere->m_vCenter.z, pBSphere->m_fRadius);
          //the rest except user data
          members[iIndex].SetValue(szBuffer);
        }
        else if (type == SWIGTYPE_p_VColorRef)
        {
          //set the pointer as value
          VColorRef *pColor = (VColorRef *)pUserData->ptr;
          sprintf(szBuffer, "Vision.VColorRef(%d,%d,%d, %d)", pColor->r, pColor->g, pColor->b, pColor->a);
          //the rest except user data
          members[iIndex].SetValue(szBuffer);
        }
        else if (type == SWIGTYPE_p_VBitmask)
        {
          //set the pointer as value
          VBitmask *pMask = (VBitmask *)pUserData->ptr;
          sprintf(szBuffer, "Vision.VBitmask(\"%x\")", pMask->Get());
          //the rest except user data
          members[iIndex].SetValue(szBuffer);
        }
        else
        {
          members.Remove(elem); //we do not support this right now
        }
      }
      else
      {
        members.Remove(elem); //we do not support this right now
      } // if type detection
    }
    }
  } // while
}


void VScriptInstance::DebugOutput(IVRenderInterface *pRI, const char *szPrepend, float &x, float &y, bool b3DPos)
{
  char szLine[1024];
  char szBuffer[1024];
  char *szPos = szLine;

  VType *pType = NULL;
  if (m_pParentComponent)
    pType = m_pParentComponent->GetOwner() ? m_pParentComponent->GetOwner()->GetTypeId() : NULL;

  szPos += sprintf(szPos, "%s [%s] '%s': ", szPrepend, pType ? pType->m_lpszClassName : "notype", m_spResource ? m_spResource->GetFilename() : "<none>");

  if (m_iCreatedThreads < 1) // nothing to output?
  {
    szPos += sprintf(szPos, "No threads");
    pRI->DrawText2D(x, y, szLine, V_RGBA_GREY); y += 10.f;
    return;
  }
  pRI->DrawText2D(10.f, y, szLine, V_RGBA_WHITE);
  y += 10.f;
  bool bRender3D = false;
  float x3D, y3D;
  hkvVec3 vPos;
  if (b3DPos && pType != NULL && pType->IsDerivedFrom(VisObject3D_cl::GetClassTypeId()))
  {
    bRender3D = true;
    vPos = ((VisObject3D_cl *)m_pParentComponent->GetOwner())->GetPosition();
    if (!Vision::Contexts.GetCurrentContext()->Project2D(vPos, x3D, y3D))
      bRender3D = false;
  }

  // output information per single lua thread
  for (int iThread = 0; iThread < m_iCreatedThreads; iThread++)
  {
    const VScriptInstance::VLuaThreadInfo &info = m_CreatedThreads.GetDataPtr()[iThread];
    sprintf(szLine, "Thread %i. %s", iThread, info.GetStatusString(szBuffer));
    pRI->DrawText2D(x + 40.f, y, szLine, V_RGBA_WHITE);
    y += 10.f;

    if (bRender3D)
    {
      pRI->DrawText2D(x3D, y3D, szLine, V_RGBA_WHITE);
      y3D += 10.f;
    }
  }
  y += 2.f;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////
// class VScriptInstanceCollection
////////////////////////////////////////////////////////////////////////////////////////////////////////

VScriptInstanceCollection::VScriptInstanceCollection() : m_WaitingObjects(0, NULL), m_SuspendedObjects(0, NULL)
{
  m_iWaitingObjectCount = m_iSuspendedObjectCount = 0;
  m_bAnyFlaggedForDisposal = false;
}

void VScriptInstanceCollection::FlagForDisposal(VScriptInstance *pObj)
{
  pObj->m_bFlagForDisposal = true;
  m_bAnyFlaggedForDisposal = true;
}


void VScriptInstanceCollection::SafeRemove(VScriptInstance *pElement)
{
  // remove from all cached collections
  SAFE_REMOVE_DYNARRAY(m_WaitingObjects, pElement);
  SAFE_REMOVE_DYNARRAY(m_SuspendedObjects, pElement);
  VRefCountedCollection<VScriptInstance>::SafeRemove(pElement);
}


void VScriptInstanceCollection::RemoveFlagged()
{
  m_bAnyFlaggedForDisposal = false;

  for (int i = 0; i < Count(); i++)
  {
    if (!GetAt(i)->m_bFlagForDisposal)
      continue;
    GetAt(i)->DisposeObject();
    //RemoveAt(i);
    i--;
  }
}




void VScriptInstanceCollection::AddWaitingObject(VScriptInstance *pObj)
{
  SAFE_ADD_DYNARRAY(m_WaitingObjects, pObj, m_iWaitingObjectCount);
}

void VScriptInstanceCollection::HandleWaitingObjects(float dtime)
{
  int iOldCount = m_iWaitingObjectCount;
  m_iWaitingObjectCount = 0;
  for (int i = 0; i < iOldCount; i++)
  {
    VScriptInstance *pObj = m_WaitingObjects.GetDataPtr()[i];
    if (!pObj) continue;
    pObj->Tick(dtime);
    if (!pObj->m_bHasSuspendedThreads) // nothing to handle anymore, so remove it from handle list
      m_WaitingObjects.GetDataPtr()[i] = NULL;
    else
      m_iWaitingObjectCount = hkvMath::Max(m_iWaitingObjectCount, i + 1); // the m_iWaitingObjectCount member might be updated inside the Tick function
  }
}

void VScriptInstanceCollection::AddSuspendedObject(VScriptInstance *pObj)
{
  SAFE_ADD_DYNARRAY(m_SuspendedObjects, pObj, m_iSuspendedObjectCount);
}

void VScriptInstanceCollection::HandleSuspendedObjects(float dtime)
{
  for (int i = 0; i < m_iSuspendedObjectCount; i++)
  {
    VScriptInstance *pObj = m_SuspendedObjects.GetDataPtr()[i];
    if (!pObj) continue;
    pObj->ResumeAll(dtime);
  }
}

void VScriptInstanceCollection::DiscardDependentScriptInstanceThreads(const VScriptResource* pResource)
{
  for (int i = 0; i < Count(); i++)
  {
    VScriptInstance* pInstance = GetAt(i);
    if (pInstance->GetResource() == pResource)
    {
      pInstance->DiscardCreatedThreads();
    }
  }
}


void LUA_CreateVArchive(lua_State* L, VArchive * pArchive);

void VScriptInstance::ScriptSerialize(VArchive &ar)
{
  VScriptResource* pRes = GetResource();
  VMemoryInStream* pIn = NULL;
  VMemoryOutStream* pOut = NULL;
  VMemoryStreamPtr spMem;

  short iArchiveVersion = Vision::GetArchiveVersion();
  char iLocalVersion = 1; // vers 1: support for deferred object loading

  int iDeferredObjectCount = 0;
  VMemoryTempBuffer<128 * sizeof(void *)> m_DeferredPtrBuffer;
  VTypedObject **pDeferredObjectList = NULL;

  if (ar.IsLoading())
  {
    //Load the data
    int iLen = 0;
    ar >> iLen;

    // -1 indicates that we have a valid local version number
    if (iLen < 0)
    {
      VASSERT_MSG(iLen == -1, "Archive corruption");
      ar >> iLocalVersion; VASSERT(iLocalVersion >= 1)
        ar >> iArchiveVersion;
    }
    else iLocalVersion = 0; // old version

    // these versions do support object serialization from inside the script
    if (iLocalVersion >= 1)
    {
      // load the deferred object list
      ar >> iDeferredObjectCount;
      m_DeferredPtrBuffer.EnsureCapacity(iDeferredObjectCount*sizeof(void *));
      pDeferredObjectList = (VTypedObject **)m_DeferredPtrBuffer.GetBuffer();
      for (int i = 0; i < iDeferredObjectCount; i++)
        ar >> pDeferredObjectList[i];

      ar >> iLen; // buffer length
    }


    //No serialize function ?
    if (!pRes || !HasFunction("OnSerialize"))
      return; // TODO: Need to skip iLen bytes...

    //still call the OnSerialize function when there's no data to load
    //if (iLen == 0) return;  // No data to load

    //read the chunk
    spMem = new VMemoryStream(NULL);
    void *pBuf = spMem->AllocateBytes(iLen);
    if (iLen > 0)
    {
      if (iLocalVersion == 0)
        ar >> iArchiveVersion; // for new version we need it earlier
      ar.Read(pBuf, iLen);
    }

    pIn = new VMemoryInStream(NULL, spMem);
  }
  else // saving
  {
    if (!pRes || !HasFunction("OnSerialize"))
    {
      int iLen = 0; // can still use old version scheme here
      ar << iLen;  //write out 0 so we know we don't have data to load
      return;
    }
    spMem = new VMemoryStream(NULL);
    pOut = new VMemoryOutStream(NULL, spMem);
  }

  // loading or saving:
  VScriptArchive tmp(pIn, pOut);
  tmp.SetLoadingVersion(iArchiveVersion);

  if (ar.IsLoading())
  {
    // prepare object list for access through this archive
    tmp.m_iDeferredObjectCount = iDeferredObjectCount;
    tmp.m_pDeferredObjLoaded = pDeferredObjectList;
  }

  //Call the script function (loading and saving)

  lua_State *L = pRes->m_pResourceState;
  VASSERT(L);

  LUA_CreateVArchive(L, &tmp); //create a script object
  int id = LUA_RegisterObject(L); //anchor it to prevent GC and get registry ID
  ExecuteFunctionArg("OnSerialize", "*r", id);
  LUA_DeregisterObject(L, id); //release it

  //Write the resulting chunk
  if (ar.IsSaving())
  {
    tmp.Flush();

    // version number:
    ar << (int)-1; // new local version scheme
    ar << iLocalVersion;
    ar << (short)tmp.GetLoadingVersion(); // in case the script set the version

    // write out deferred object list: (vers >=1)
    ar << tmp.m_iDeferredObjectCount;
    VScriptArchive::VObjectClassPair_t *pPair = tmp.m_DeferredObjStored.GetDataPtr();
    for (int i = 0; i < tmp.m_iDeferredObjectCount; i++, pPair++)
      ar.WriteObject(pPair->m_pObj, pPair->m_pClass);

    int iLen = spMem->GetSize();
    ar << iLen;
    if (iLen == 0)
    {
      V_SAFE_DELETE(pOut);
      V_SAFE_DELETE(pIn);
      return;
    }

    ar.Write(spMem->GetDataPtr(), iLen);
  }

  V_SAFE_DELETE(pOut);
  V_SAFE_DELETE(pIn);
}


bool VScriptInstance::ExecuteCollisionFunc(const char *pszFunctionName, VScriptCollisionInfo *pColInfo)
{
  VISION_PROFILE_FUNCTION(PROFILING_SCRIPTOBJ_EXECUTEFUNCTION);
#ifdef PROFILING
  VScriptResourceManager::g_iFunctionsCalled++;
#endif

  // prepare the function call (pushes function name)
  VLuaThreadInfo *pInfo = PrepareFunctionCall(pszFunctionName);
  if (!pInfo) return false;
  lua_State *L = pInfo->pThread;

  // now start pushing the arguments on the stack
  LUA_PushObjectProxy(L, (VScriptComponent*)m_pParentComponent);  // push self

  // create the collision info table that will be given as the only other argument
  lua_newtable(L);

  //add the required fields to the lua table
  LUA_PushObjectProxy(L, (hkvVec3*)&(pColInfo->m_vPoint));
  lua_setfield(L, -2, "HitPoint");

  LUA_PushObjectProxy(L, (hkvVec3*)&(pColInfo->m_vNormal));
  lua_setfield(L, -2, "HitNormal");

  lua_pushnumber(L, pColInfo->m_fForce);
  lua_setfield(L, -2, "Force");

  lua_pushnumber(L, pColInfo->m_fRelativeVelocity);
  lua_setfield(L, -2, "RelativeVelocity");

  lua_pushstring(L, pColInfo->m_colliderInfo.GetColliderType());
  lua_setfield(L, -2, "ColliderType");

  if (pColInfo->m_colliderInfo.m_pObjectComponent)
  {
    IVObjectComponent *pComponent = pColInfo->m_colliderInfo.m_pObjectComponent;
    LUA_PushObjectProxy(L, pComponent->GetOwner());
  }
  else if (pColInfo->m_colliderInfo.m_pStaticMesh)
  {
    VSWIG_PUSH_PROXY(L, VisStaticMeshInstance_cl, pColInfo->m_colliderInfo.m_pStaticMesh);
  }
  else
  {
    lua_pushnil(L);
  }
  lua_setfield(L, -2, "ColliderObject");

  //LUA_STACK_DUMP(L);

  //then call the collision function
  bool bResult;
  bResult = DoFunctionCall(pInfo, 2);
  lua_settop(L, 0);  //clear the stack

  if (!bResult)
  {
    LUA_WARNING_1("Failed to call collision callback '%s'.", pszFunctionName);

#ifdef PROFILING
    VScriptResourceManager::g_iFunctionsFailed++;
#endif
  }

  return bResult;
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
