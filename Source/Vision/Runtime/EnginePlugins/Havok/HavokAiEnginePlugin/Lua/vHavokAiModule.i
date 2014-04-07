/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// vHavokAiUtil Lua Modeul - SWIG Interface
// This file generates vHavokAiUtil_wrapper.cpp
// It is represented in Lua as the "HavokAI" module.

#ifndef VLUA_APIDOC

%nodefaultctor vHavokAiModule;
%nodefaultdtor vHavokAiModule;

// get access to all vision specific things...
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/vision_types.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/hkvVec3.i>

%module HavokAi
%{
	
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/HavokAiEnginePlugin.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/vHavokAiIncludes.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/vHavokAiModule.hpp>
#include <Ai/Pathfinding/Collide/NavMesh/hkaiNavMeshQueryMediator.h>
#include <Ai/Pathfinding/Utilities/hkaiPathfindingUtil.h>
#include <Ai/Pathfinding/World/hkaiWorld.h>

%}

class vHavokAiModule
{
public:
	hkvVec3 *GetClosestPoint(const hkvVec3 *position, float radius = 5.0f);
	hkvVec3 *CastRay(const hkvVec3 *start, const hkvVec3 *end);
	hkvVec3 *PickPoint(float x, float y, float fMaxDist = 10000.f);
	
  %extend{
    VSWIG_CREATE_CAST_UNSAFE(vHavokAiModule)
  }
};

%native(vHavokAiModule_GetPointOnPath) int vHavokAiModule_GetPointOnPath(lua_State *L);
%{
  SWIGINTERN int vHavokAiModule_GetPointOnPath(lua_State *L)
  {
    IS_MEMBER_OF(vHavokAiModule)
	  //we dont need the self parameter (index 1)
	  const int pathArgIndex = 2;
	  hkvVec3 *vResult = NULL;

    float progress;
    if( !LUA_GetValue(L, 3, progress) ) 
	  {
      luaL_argerror(L, 3, "Expected a number");
    }
    else if (lua_istable(L, pathArgIndex))
	  {
		  int index = 0;
		  hkvVec3 vPrevious;
		  float pp = 0.0f;
		
		  lua_pushnil(L);
		  while (lua_next(L, 2) != 0)
		  {
        hkvVec3 vPoint;
        if( LUA_GetValue(L, -1, vPoint) ) 
			  {
			    if (index > 0)
			    {
				    float length = (vPoint - vPrevious).getLength();
				    float npp = pp + length;
				    if (progress >= pp && progress <= npp)
				    {
					    float d = (progress - pp) / length;
					    vResult = new hkvVec3();
					    *vResult = vPrevious + (vPoint - vPrevious) * d;
					    break;
				    }
				    else
				    {
					    pp = npp;
				    }
			    }

			    vPrevious = vPoint;			
			    index++;
        }
			  lua_pop(L, 1);
		  }
	  }
	  else
	  {
		  luaL_argerror(L, pathArgIndex, "Expected table of hkvVec3");
	  }
	
	  SWIG_Lua_NewPointerObj(
		  L,
		  vResult,
		  SWIGTYPE_p_hkvVec3,
		  VLUA_MANAGE_MEM_BY_LUA);

    return 1;
  }
%}

%native(vHavokAiModule_GetPathLength) int vHavokAiModule_GetPathLength(lua_State *L);
%{
  SWIGINTERN int vHavokAiModule_GetPathLength(lua_State *L)
  {
    IS_MEMBER_OF(vHavokAiModule)
	  //we dont need the self parameter (index 1)
	  float length = 0.f;
	  const int pathArgIndex = 2;

	  if (lua_istable(L, pathArgIndex))
	  {
		  lua_pushnil(L);

		  int index = 0;
		  hkvVec3 vPrevious;

		  while (lua_next(L, 2) != 0)
		  {
        hkvVec3 vPoint;
        if ( LUA_GetValue(L, -1, vPoint) ) 
        {
			    if (index > 0)
			    {
				    length += (vPoint - vPrevious).getLength();
			    }

			    vPrevious = vPoint;
			
			    index++;
        }
			  lua_pop(L, 1);
		  }
	  }
	  else
	  {
		  luaL_argerror(L, pathArgIndex, "Expected table of hkvVec3");
	  }
	
	  lua_pushnumber(L, length);

    return 1;
  }
%}

%native(vHavokAiModule_FindPath) int vHavokAiModule_FindPath(lua_State *L);
%{
  SWIGINTERN int vHavokAiModule_FindPath(lua_State *L)
  {
    IS_MEMBER_OF(vHavokAiModule)
    
    SWIG_CONVERT_POINTER(L, 1, vHavokAiModule, pSelf);
	  DECLARE_ARGS_OK

    hkvVec3 startPoint;
    hkvVec3 endPoint;
    float radius;

    //check the call parameters
    if( !LUA_GetValue(L, 2, startPoint) ) 
	  {
      luaL_argerror(L, 2, "Expected a Vision.hkvVec3");
      lua_pushnil(L);
      return 1;
    }
    if( !LUA_GetValue(L, 3, endPoint) ) 
	  {
      luaL_argerror(L, 3, "Expected a Vision.hkvVec3");
      lua_pushnil(L);
      return 1;
    }
    if( !LUA_GetValue(L, 4, radius) ) 
	  {
      luaL_argerror(L, 4, "Expected a number");
      lua_pushnil(L);
      return 1;
    }

	  lua_settop(L, 0);

	  hkaiWorld *world = pSelf->GetAiWorld();
	  hkaiPathfindingUtil::FindPathInput input;

	  hkVector4 hkStartPoint;
	  vHavokConversionUtils::VisVecToPhysVecWorld(startPoint, hkStartPoint);
	  hkVector4 hkEndPoint;
	  vHavokConversionUtils::VisVecToPhysVecWorld(endPoint, hkEndPoint);

	  hkVector4 startPosition;
	  hkaiPackedKey startFaceKey = world->getDynamicQueryMediator()->getClosestPoint(
		hkStartPoint, 5.0f, startPosition );

	  hkVector4 endPosition;
	  hkaiPackedKey endFaceKey = world->getDynamicQueryMediator()->getClosestPoint(
		hkEndPoint, 5.0f, endPosition );

	  if (startFaceKey != HKAI_INVALID_PACKED_KEY && endFaceKey != HKAI_INVALID_PACKED_KEY)
	  {
		  input.m_startPoint = startPosition;
		  input.m_startFaceKey = startFaceKey;
		  input.m_goalPoints.pushBack(endPosition);
		  input.m_goalFaceKeys.pushBack(endFaceKey);
		  input.m_agentInfo.m_diameter = VIS2HK_FLOAT_SCALED(radius * 2.f);
		  input.m_searchParameters.m_up.set(0, 0, 1);

		  hkaiPathfindingUtil::FindPathOutput output;
		  hkaiPathfindingUtil::findPath(*world->getStreamingCollection(), input, output);

		  // create an empty table (or an array if you would like to see it this way)
		  lua_newtable(L);
    
		  for (int pointIndex = 0; pointIndex < output.m_pathOut.getSize(); pointIndex++)
		  {
			  const hkVector4 &position = output.m_pathOut[pointIndex].m_position;

			  hkvVec3 *result = new hkvVec3();
			  vHavokConversionUtils::PhysVecToVisVecWorld(position, *result);

			  SWIG_Lua_NewPointerObj(
				  L,
				  result,
				  SWIGTYPE_p_hkvVec3,
				  VLUA_MANAGE_MEM_BY_LUA);
			  lua_rawseti(L, -2, pointIndex + 1);
		  }
	  }
	  else
	  {
		  SWIG_Lua_NewPointerObj(
			  L,
			  NULL,
			  SWIGTYPE_p_hkvVec3,
			  VLUA_MANAGE_MEM_BY_LUA);
	  }
    return 1;
  }
%}

#else

/// \brief Lua Wrapper class for vHavokAiModule accessible via the global \b Ai.
/// \par Example
///   \code
///     ...
///     local myPath = { Vision.hkvVec3(0,0,0), Vision.hkvVec3(100,0,0), Vision.hkvVec3(100,100,0), Vision.hkvVec3(0,100,0) }
///     local myPathLength = AI:GetPathLength(myPath)
///     ...
///   \endcode
class vHavokAiModule
{
public:

  /// @name Havok Ai Functions
  /// @{

  /// \brief
  ///   Finds a path (table holding points as hkvVec3) that gets you from startPoint to endPoint.
  ///
  /// \param startPoint The point from which to start the path search.
  /// \param endPoint The end point where to search the path to.
  /// \param radius The size of the character for which to search a path.
  ///
  /// \return table of hkvVec3 or nil if a path couldn't be found.
  ///
  /// \par Example
  ///   \code
  ///     ...
  ///     local path = AI:FindPath(self:GetPosition(), myGoal, 20)
  ///     
  ///     if path = nil then
  ///       Debug:Log("Couldn't find path for '" .. self:GetKey() .. "' to " .. myGoal)
  ///     else
  ///       Debug:Log("Path for '" .. self:GetKey() .. "' to " .. myGoal .. " contains " .. #path .. " positions")
  ///     end
  ///     ...
  ///   \endcode
  table FindPath(hkvVec3 startPoint, hkvVec3 endPoint, number radius);
  
  /// \brief
  ///   Takes a path (table holding points as hkvVec3) and a progress from 0.0 to the length of
  ///   the path and returns the point on that path.
  ///
  /// \param pathPoints The table of points that represent a path as it was returned by FindPath().
  /// \param progress The distance along the path at which to get the sample position. See example below.
  ///
  /// \return Point on the path (can be nil if the provided path is nil).
  /// \par Example
  ///   \code
  ///     ...
  ///     local myPath = { Vision.hkvVec3(0,0,0), Vision.hkvVec3(100,50,0), Vision.hkvVec3(40,80,0), Vision.hkvVec3(70,10,0) }
  ///     local myPathLength = AI:GetPathLength(myPath)
  ///     
  ///     -- sample to evaluate the position on the path when reaching 65% of the path length
  ///     local pointAt65Percent = AI:GetPointOnPath(myPath, myPathLength * 0.65)
  ///     ...
  ///   \endcode
  /// \see GetPathLength
  hkvVec3 GetPointOnPath(table pathPoints, number progress);
  
  /// \brief
  ///   Takes a path (table holding points as hkvVec3) and returns the length of that path
  ///
  /// \param pathPoints The table of points that represent a path as it was returned by FindPath().
  ///
  /// \return Length of the path
  /// \see GetPointOnPath
  number GetPathLength(table pathPoints);
  
  /// \brief
  ///   Takes a position in world space and returns the closest point on the nav mesh within
  ///   the given radius.
  ///
  /// \param position The position in world space for which to return the closest point on the navmesh.
  /// \param radius The radius in which to search for the closest point on the navmesh.
  ///
  /// \return The closest point on the nav mesh to the point passed in with the specified radius (can be nil).
  ///
  /// \par Example
  ///   \code
  ///     ...
  ///     local beamMeTo = Vision.hkvVec3(700,400,230);
  ///     
  ///     -- let get a point that is really on the navmesh
  ///     local beamMeToNavMeshPoint = AI:GetClosestPoint(beamMeTo, 100)
  ///     ...
  ///   \endcode
  hkvVec3 GetClosestPoint(hkvVec3 position, number radius = 5);
	  
  /// \brief
  ///   Casts a ray to find a point in nav mesh and returns nil if nothing is found.
  ///
  /// \param start The start position from where to cast the ray.
  /// \param end The end position of the ray.
  ///
  /// \return Point on the nav mesh or nil
  /// \par Example
  ///   \code
  ///     function OnAfterSceneLoaded(self)
  ///       local pos = self:GetPosition()
  ///     
  ///       -- adjust the height of this entity so that it is on the navmesh
  ///       local posOnNavMesh = AI:CastRay(Vision.hkvVec3(pos.x, pos.y, pos.z+500), Vision.hkvVec3(pos.x, pos.y, pos.z-500))
  ///       self:SetPosition(posOnNavMesh)
  ///     end
  ///   \endcode
  hkvVec3 CastRay(hkvVec3 start, hkvVec3 end);
  
  /// \brief
  ///   Takes a screen position and casts a ray into the AI world to find a point, but will
  ///   return nil if nothing is found.
  ///
  /// \param x The x screen coordinate from which to pick.
  /// \param y The y screen coordinate from which to pick.
  /// \param maxDist The maximum distance to pick from the camera position into the world.
  ///
  /// \return The point under the cursor or nil if nothing is there
  ///
  /// \par Example
  ///   \code
  ///     --setup some basic input
  ///     function OnAfterSceneLoaded(self)
  ///       local width, height = Screen:GetViewportSize()
  ///       self.InputMap = Input:CreateMap("InputMap")
  ///
  ///       self.InputMap:MapTrigger("X", {0,0, width, height}, "CT_TOUCH_ABS_X" )
  ///       self.InputMap:MapTrigger("Y", {0,0, width, height}, "CT_TOUCH_ABS_Y" )
  ///       -- add some PC controls for prototyping
  ///       self.InputMap:MapTrigger("X", "MOUSE", "CT_MOUSE_ABS_X" )
  ///       self.InputMap:MapTrigger("Y", "MOUSE", "CT_MOUSE_ABS_Y" )
  ///     end 
  ///
  ///     function OnThink(self)
  ///       local touchX = InputMap:GetTrigger("X")
  ///       local touchY = InputMap:GetTrigger("Y")
  ///       -- get the position the user wants to go...
  ///       local posOnNavMesh = AI:PickPoint(touchX, touchY)
  ///    
  ///       if posOnNavMesh ~= nil then
  ///         --call some code to move the character to the goal
  ///         ...
  ///       end
  ///     end
  ///   \endcode
  hkvVec3 PickPoint(number x, number y, number maxDist = 10000);

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
