/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// ===========================================================
// Havok Physics Lua Module - SWIG Interface
// ===========================================================
// This interface file generates PhysicsLuaModule_wrapper.cpp
// and is represented in Lua as the "Physics" module.
 
#ifndef VLUA_APIDOC
 
//get access to all vision specific things...
%include <windows.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/vision_types.i>

//dependencies of ObjectComponent:
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VColor.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VTypedObject.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VisApiTypedEngineObject.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VisApiObjectComponent.i>

//custom headers for generated source file
%module Physics
%{
  #include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>
  #include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConstraint.hpp>
  #include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>
  
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VBitmask.hpp>
  #include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokCharacterController.hpp>
  #include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokRigidBody.hpp>
  #include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokRagdoll.hpp>
  
  #define MOTIONTYPE_DYNAMIC hkpMotion::MOTION_DYNAMIC
  #define MOTIONTYPE_KEYFRAMED hkpMotion::MOTION_KEYFRAMED
  #define MOTIONTYPE_FIXED hkpMotion::MOTION_FIXED
  #define MOTIONTYPE_SPHERE_INERTIA hkpMotion::MOTION_SPHERE_INERTIA
  #define MOTIONTYPE_BOX_INERTIA hkpMotion::MOTION_BOX_INERTIA
  #define MOTIONTYPE_THIN_BOX_INERTIA hkpMotion::MOTION_THIN_BOX_INERTIA
  #define MOTIONTYPE_CHARACTER hkpMotion::MOTION_CHARACTER

  #define QUALITY_FIXED HK_COLLIDABLE_QUALITY_FIXED
  #define QUALITY_KEYFRAMED HK_COLLIDABLE_QUALITY_KEYFRAMED
  #define QUALITY_DEBRIS HK_COLLIDABLE_QUALITY_DEBRIS
  #define QUALITY_DEBRIS_SIMPLE_TOI HK_COLLIDABLE_QUALITY_DEBRIS_SIMPLE_TOI
  #define QUALITY_MOVING HK_COLLIDABLE_QUALITY_MOVING
  #define QUALITY_CRITICAL HK_COLLIDABLE_QUALITY_CRITICAL
  #define QUALITY_BULLET HK_COLLIDABLE_QUALITY_BULLET
  #define QUALITY_CHARACTER HK_COLLIDABLE_QUALITY_CHARACTER
  #define QUALITY_KEYFRAMED_REPORTING HK_COLLIDABLE_QUALITY_KEYFRAMED_REPORTING

  #define LAYER_ALL                      vHavokPhysicsModule::HK_LAYER_ALL 
  #define LAYER_COLLIDABLE_DYNAMIC	     vHavokPhysicsModule::HK_LAYER_COLLIDABLE_DYNAMIC
  #define LAYER_COLLIDABLE_STATIC	     vHavokPhysicsModule::HK_LAYER_COLLIDABLE_STATIC
  #define LAYER_COLLIDABLE_TERRAIN	     vHavokPhysicsModule::HK_LAYER_COLLIDABLE_TERRAIN
  #define LAYER_COLLIDABLE_CONTROLLER	 vHavokPhysicsModule::HK_LAYER_COLLIDABLE_CONTROLLER
  #define LAYER_COLLIDABLE_TERRAIN_HOLE  vHavokPhysicsModule::HK_LAYER_COLLIDABLE_TERRAIN_HOLE
  #define LAYER_COLLIDABLE_DISABLED	     vHavokPhysicsModule::HK_LAYER_COLLIDABLE_DISABLED
  #define LAYER_COLLIDABLE_RAGDOLL	     vHavokPhysicsModule::HK_LAYER_COLLIDABLE_RAGDOLL
  #define LAYER_COLLIDABLE_ATTACHMENTS   vHavokPhysicsModule::HK_LAYER_COLLIDABLE_ATTACHMENTS
  #define LAYER_COLLIDABLE_FOOT_IK	     vHavokPhysicsModule::HK_LAYER_COLLIDABLE_FOOT_IK
  #define LAYER_COLLIDABLE_DEBRIS		 vHavokPhysicsModule::HK_LAYER_COLLIDABLE_DEBRIS
  #define LAYER_COLLIDABLE_CUSTOM0	     vHavokPhysicsModule::HK_LAYER_COLLIDABLE_CUSTOM0
  #define LAYER_COLLIDABLE_CUSTOM1	     vHavokPhysicsModule::HK_LAYER_COLLIDABLE_CUSTOM1
  #define LAYER_COLLIDABLE_CUSTOM2	     vHavokPhysicsModule::HK_LAYER_COLLIDABLE_CUSTOM2
  #define LAYER_COLLIDABLE_CUSTOM3	     vHavokPhysicsModule::HK_LAYER_COLLIDABLE_CUSTOM3 
  #define LAYER_COLLIDABLE_CUSTOM4	     vHavokPhysicsModule::HK_LAYER_COLLIDABLE_CUSTOM4
%}

%immutable;

//motion types
static const int MOTIONTYPE_DYNAMIC;
static const int MOTIONTYPE_KEYFRAMED;
static const int MOTIONTYPE_FIXED;
static const int MOTIONTYPE_SPHERE_INERTIA;
static const int MOTIONTYPE_BOX_INERTIA;
static const int MOTIONTYPE_THIN_BOX_INERTIA;
static const int MOTIONTYPE_CHARACTER;


//quality types
static const int QUALITY_FIXED;
static const int QUALITY_KEYFRAMED;
static const int QUALITY_DEBRIS;
static const int QUALITY_DEBRIS_SIMPLE_TOI;
static const int QUALITY_MOVING;
static const int QUALITY_CRITICAL;
static const int QUALITY_BULLET;
static const int QUALITY_CHARACTER;
static const int QUALITY_KEYFRAMED_REPORTING;


//collision layers
static const int LAYER_ALL;                     
static const int LAYER_COLLIDABLE_DYNAMIC;			
static const int LAYER_COLLIDABLE_STATIC;				
static const int LAYER_COLLIDABLE_TERRAIN;			
static const int LAYER_COLLIDABLE_CONTROLLER;		
static const int LAYER_COLLIDABLE_TERRAIN_HOLE; 
static const int LAYER_COLLIDABLE_DISABLED;			
static const int LAYER_COLLIDABLE_RAGDOLL;			
static const int LAYER_COLLIDABLE_ATTACHMENTS;	
static const int LAYER_COLLIDABLE_FOOT_IK;			
static const int LAYER_COLLIDABLE_DEBRIS;				
static const int LAYER_COLLIDABLE_CUSTOM0;			
static const int LAYER_COLLIDABLE_CUSTOM1;			
static const int LAYER_COLLIDABLE_CUSTOM2;			
static const int LAYER_COLLIDABLE_CUSTOM3;			
static const int LAYER_COLLIDABLE_CUSTOM4;			

%mutable;
  
  
%include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/Lua/vHavokCharacterController.i>;
%include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/Lua/vHavokRigidBody.i>;
%include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/Lua/vHavokRagdoll.i>;

/*
%native(EnableDebugRendering) int PhysicsLuaModule_EnableDebugRendering(lua_State *L);
%{
  int PhysicsLuaModule_EnableDebugRendering(lua_State *L) {
    if(lua_isboolean(L,-1))
    {
      bool bEnable = lua_toboolean(L,-1)==TRUE;
      lua_pop(L,1);
      if(Vision::GetApplication()->GetPhysicsModule())
        ((vHavokPhysicsModule*)Vision::GetApplication()->GetPhysicsModule())->SetEnabledDebug(bEnable);
    } 
    return 0;
  }
%}
*/

%native(EnableVisualDebugger) int PhysicsLuaModule_EnableVisualDebugger(lua_State *L);
%{
  int PhysicsLuaModule_EnableVisualDebugger(lua_State *L) {
    if(lua_isboolean(L,-1))
    {
      bool bEnable = lua_toboolean(L,-1)==TRUE;
      lua_pop(L,1);
      if(Vision::GetApplication()->GetPhysicsModule())
        ((vHavokPhysicsModule*)Vision::GetApplication()->GetPhysicsModule())->SetEnabledVisualDebugger(bEnable);
    } 
    return 0;
  }
%}

%native(EnableError) float PhysicsLuaModule_EnableError(lua_State *L);
%{
  int PhysicsLuaModule_EnableError(lua_State *L) 
  {
    DECLARE_ARGS_OK
	  GET_ARG(1, const char *, errorString);
    GET_ARG(2, bool, enableError);

    hkError& error = hkError::getInstance();

    if(errorString!=NULL && errorString[0]!=NULL)
    {
      int errorId;
      if(sscanf(errorString, "%x", &errorId)==1)
      {
        hkError& error = hkError::getInstance();
			  error.setEnabled( errorId, enableError );
        lua_pushboolean(L, error.isEnabled( errorId )==enableError);
      }
      else
      {
        lua_pushboolean(L, false);
      }
    }
    else
    {
      lua_pushboolean(L, false);
    }
	  return 1;
  }
%}

%native(GetHavokToVisionScale) float PhysicsLuaModule_GetHavokToVisionScale(lua_State *L);
%{
  int PhysicsLuaModule_GetHavokToVisionScale(lua_State *L) 
  {
	float scale = float(vHavokPhysicsModule::GetInstance()->GetVisionWorldScale());
	lua_pushnumber(L, (lua_Number) scale);
	return 1;
  }
%}

%native(SetHavokToVisionScale) float PhysicsLuaModule_SetHavokToVisionScale(lua_State *L);
%{
  int PhysicsLuaModule_SetHavokToVisionScale(lua_State *L) 
  {
    if(lua_isnumber(L,-1))
	{
		  float scale = lua_tonumber(L,-1);
		  lua_pop(L,1);
		  vHavokPhysicsModule::GetInstance()->SetVisionWorldScale(scale);
	}
	return 0;
  }
%}

%native(SetGravity) float PhysicsLuaModule_SetGravity(lua_State *L);
%{
  int PhysicsLuaModule_SetGravity(lua_State *L) 
  {
	DECLARE_ARGS_OK
	GET_ARG(1, hkvVec3, gravity);
	vHavokPhysicsModule::GetInstance()->SetGravity( gravity );
	return 0;
  }
%}

%native(SetGroupsCollision) int PhysicsLuaModule_SetGroupsCollision(lua_State *L);
%{
  int PhysicsLuaModule_SetGroupsCollision(lua_State *L) 
  {
	DECLARE_ARGS_OK
	
	// Get arguments
	GET_ARG(1, int, group1);
	GET_ARG(2, int, group2);
	GET_ARG(3, bool, doEnable);
	
	vHavokPhysicsModule::GetInstance()->SetGroupsCollision(group1, group2, doEnable);
	return 0;
  }
%}

%native(CalcFilterInfo) int PhysicsLuaModule_CalcFilterInfo(lua_State *L);
%{
   int PhysicsLuaModule_CalcFilterInfo(lua_State *L) 
   {
     DECLARE_ARGS_OK
	
	   // Get arguments
	   GET_ARG(1, int, layer);
	   GET_ARG(2, int, group);
	   GET_ARG(3, int, subsystem);
	   GET_ARG(3, int, subsystemDontCollideWith);

	   // Calculate collision filter information
	   int iCollisionFilterInfo = hkpGroupFilter::calcFilterInfo(layer, group, subsystem, subsystemDontCollideWith);

     lua_pushinteger(L, (lua_Integer)iCollisionFilterInfo);
	   return 1;
   }
%}

%native(PerformRaycast) int PhysicsLuaModule_PerformRaycast(lua_State *L);
%{
   int PhysicsLuaModule_PerformRaycast(lua_State *L)
   {
	   DECLARE_ARGS_OK;
	
	   // Get arguments
	   GET_ARG(1, hkvVec3, vRayStart);
	   GET_ARG(2, hkvVec3, vRayEnd);
     GET_ARG(3, int, iCollisionFilterInfo);

	   // Perform raycast
	   VisPhysicsRaycastClosestResult_cl raycastData;
	   raycastData.vRayStart = vRayStart;
	   raycastData.vRayEnd = vRayEnd;
	   raycastData.iCollisionBitmask = iCollisionFilterInfo;
	   vHavokPhysicsModule::GetInstance()->PerformRaycast(&raycastData);
	
	   // Check hit
	   if (!raycastData.bHit)
	   { 
	     lua_pushboolean(L, false);
	     return 1;
	   }
	
	   lua_pushboolean(L, true);

     // Create an empty table for returning raycast results
	   lua_newtable(L); 

	   // Hit type
	   lua_pushstring(L, "HitType");
     switch(raycastData.closestHit.eHitType)
	   {
	   case VIS_TRACETYPE_ENTITYPOLY:
	     lua_pushstring(L, "Entity"); 
	     break;

	   case VIS_TRACETYPE_STATICGEOMETRY:
	     lua_pushstring(L, "Mesh"); 
	     break;

     case VIS_TRACETYPE_TERRAIN:
	     lua_pushstring(L, "Terrain"); 
	     break;

     default:
	     lua_pushstring(L, "Unknown"); 
	   }
     lua_settable(L, -3);  

	   // Hit object 
	   lua_pushstring(L, "HitObject");    
	   LUA_PushObjectProxy(L, raycastData.closestHit.pHitObject);
	   lua_settable(L, -3);  

	   // Hit fraction
	   lua_pushstring(L, "HitFraction");         
     lua_pushnumber(L, (lua_Number)raycastData.closestHit.fHitFraction); 
	   lua_settable(L, -3);  

	   // Impact point and normal
	   lua_pushstring(L, "ImpactPoint");          
	   LUA_PushObjectProxy(L, new hkvVec3(raycastData.closestHit.vImpactPoint));
     lua_settable(L, -3);            
	   lua_pushstring(L, "ImpactNormal");          
	   LUA_PushObjectProxy(L, new hkvVec3(raycastData.closestHit.vImpactNormal));
     lua_settable(L, -3);   

	   // Hit material information
	   lua_pushstring(L, "DynamicFriction");         
     lua_pushnumber(L, (lua_Number)raycastData.closestHit.hitMaterial.fDynamicFriction);    
	   lua_settable(L, -3); 
	   lua_pushstring(L, "Restitution");         
     lua_pushnumber(L, (lua_Number)raycastData.closestHit.hitMaterial.fRestitution);    
	   lua_settable(L, -3); 
	   lua_pushstring(L, "UserData");   
	   lua_pushstring(L, raycastData.closestHit.hitMaterial.szUserData.AsChar());
	   lua_settable(L, -3);
   
	   return 2;
   }
%}

#else

/// \brief MODULE: The Lua \b Physics module contains all generated wrapper classes of the vHavokEnginePlugin.
/// \par Example
///   \code
///     function OnCreate(self)
///       self:AddComponentOfType("vHavokRigidBody", "RigidBody")
///       self.RigidBody:SetMass(65)
///       self.RigidBody:SetDebugRendering(true)
///     
///       Physics.EnableVisualDebugger(true)
///     end
///   \endcode
class Physics {
public:

  /// @name Static Module Functions
  /// @{
  
  /// \brief Disable a specific erro or warning.
  /// \note  Be careful! If Havok raises an assert then it usually means that something
  ///        is fundamentally wrong. Disabling an assert may well let you continue development
  ///        but should only be done as a short term measure. You should ensure that you
  ///        address the underlying cause of the assert.
  /// \param identifier The id of the error/warning as hex string starting with "0x".
  /// \param enable Enable or disabe the specified error or warning.
  /// \returns true when succeeded to change the error/warning to the desried state, otherwise false.
  /// \par Example
  ///   \code
  ///     function OnAfterSceneLoaded(self)
  ///       Physics.EnableError("0x44444444", false)
  ///     end
  ///   \endcode
  static boolean EnableError(string identifier, boolean enable)

  /// \brief Setup possibility to attach the Havok visual debugger.
  /// \param enable Enable or disabe the visual debugger.
  /// \par Example
  ///   \code
  ///     function OnAfterSceneLoaded(self)
  ///       Physics.EnableVisualDebugger(true)
  ///     end
  ///   \endcode
  static void EnableVisualDebugger(boolean enable);
 
  /// note: temporary removed
  /// !brief Hide or show debug wireframe rendering.
  /// !param enable Enable or disabe debug rendering inside the viewport.
  /// !par Example
  ///   !code
  ///     ...
  ///     Physics.EnableDebugRendering(true)
  ///     ...
  ///   !endcode
  /// static void EnableDebugRendering(boolean enable);

  /// \brief Set the internal scaling from Havok to Vision.
  /// \param scale value to convert from Havok units to Vision units
  /// \par Example
  ///   \code
  ///     function OnBeforeSceneLoaded(self)
  ///       Physics.SetHavokToVisionScale( 0.01 )
  ///     end
  ///   \endcode
  static void SetHavokToVisionScale(float scale = 1.0f / 50.0f);

  /// \brief Gets the internal scaling from Havok to Vision.
  /// \par Example
  ///   \code
  ///     function OnBeforeSceneLoaded(self)
  ///       scale = Physics.GetHavokToVisionScale()
  ///     end
  ///   \endcode
  static float GetHavokToVisionScale();
  
  /// \brief Sets gravity
  /// \param gravity Gravity in Vision units
  /// \par Example
  ///   \code
  ///     function OnBeforeSceneLoaded(self)
  ///       Physics.SetGravity(0,0,-1000)
  ///     end
  ///   \endcode
  static void SetGravity( hkvVec3 gravity );
  
  /// \brief Enables / disables collisions between the given groups
  static void SetGroupsCollision(int group1, int group2, bool doEnable);

  /// \brief Calculates collision filter information.
  ///
  /// \param layer
  ///   The collision layer.
  ///
  /// \param group 
  ///   The collision group.
  ///
  /// \param subsystem 
  ///   The collision subsystem.
  ///
  /// \param subsystemDontCollideWith 
  ///    The collision subsystem this body shouldn't collide with.
  ///
  /// \note
  ///   See the Havok documentation for more information about what values to specify for these parameters.
  /// 
  /// \return
  ///   Collision filter information, used e.g. for PerformRaycast().
  int CalcFilterInfo(int layer, int group, int subsystem, int subsystemDontCollideWith);

  /// \brief Performs a raycast and returns the closest result.
  ///
  /// \param rayStart
  ///   Start of ray.
  /// 
  /// \param rayEnd
  ///   End of ray.
  ///
  /// \param collisionFilterInfo
  ///   Use CalcFilterInfo() to setup the collisionFilterInfo.
  ///
  /// \return
  ///   The first return parameter is a boolean that indicates whether the ray has hit something.
  ///   The second return parameter is a table that contains the raycast results for the closest hit:
  ///   - HitType [string]: Type of geometry that was hit, can be "Entity", "Mesh", "Terrain", "Unknown".
  ///   - HitObject [VisTypedEngineObject_cl*]: If an entity was hit, the corresponding VisObject3D_cl pointer is stored here. If a static mesh instance was hit, the corresponding VisStaticMeshInstance_cl is stored here.
  ///   - HitFraction [number]: Fraction between start and end of the trace at which the hit occurred.
  ///   - ImpactPoint [hkvVec3]: Point (position) of impact.
  ///   - ImpactNormal [hkvVec3]: Surface normal at the point of impact.
  ///   - DynamicFriction [number]: Coefficient for dynamic friction. 
  ///   - Restitution [number]: Coefficient for restitution.
  ///   - UserData [string]: Physics material user data string (currently ony supported for static meshes).
  ///
  /// \par Example
  ///   \code
  ///     function OnThink(self)
  ///       local rayStart = Vision.hkvVec3(100.0, 150.0, 0.0)
  ///       local rayEnd = Vision.hkvVec3(-140.0, 150.0, 0.0)
  ///       local iCollisionFilterInfo = Physics.CalcFilterInfo(Physics.LAYER_ALL, 0, 0, 0);
  ///		local isHit, result = Physics.PerformRaycast(rayStart, rayEnd, iCollisionFilterInfo)
  ///       if isHit == true then 
  ///         Debug:PrintLine("Hit type: " ..result["HitType"])
  ///         if result["HitType"] == "Entity" then
  ///           Debug:PrintLine("Mesh of hit entity: " ..result["HitObject"]:GetMesh():GetName())
  ///         end 
  ///         Debug:PrintLine("Hit fraction: " ..result["HitFraction"])
  ///         Debug:PrintLine("Hit position: " ..result["ImpactPoint"])
  ///         Debug:PrintLine("Hit normal: " ..result["ImpactNormal"])
  ///         Debug:PrintLine("Dynamic friction of hit object: " ..result["DynamicFriction"])
  ///         Debug:PrintLine("Restitution of hit object: " ..result["Restitution"])
  ///         Debug:PrintLine("Physics user data of hit object: " ..result["UserData"])
  ///       end
  ///     end
  ///   \endcode
  static boolean_table PerformRaycast(hkvVec3 rayStart, hkvVec3 rayEnd, int collisionFilterInfo); 

  /// @}
  /// @name Motion Type Constants
  /// @{
  
  /// \brief A fully-simulated, movable rigid body.
  static const int MOTIONTYPE_DYNAMIC;
  
  /// \brief Simulation is not performed as a normal rigid body, can be moved manually by code.
  static const int MOTIONTYPE_KEYFRAMED;

  /// \brief This motion type is used for the static elements of a game scene.
  static const int MOTIONTYPE_FIXED;

  /// \brief Simulation is performed using a sphere inertia tensor.
  static const int MOTIONTYPE_SPHERE_INERTIA;

  /// \brief Simulation is performed using a box inertia tensor.
  static const int MOTIONTYPE_BOX_INERTIA;

  /// \brief A box inertia motion which is optimized for thin boxes and has less stability problems.
  static const int MOTIONTYPE_THIN_BOX_INERTIA;
  
  /// \brief A specialized motion used for character controllers.
  static const int MOTIONTYPE_CHARACTER;
  
  /// @}
  /// @name Quality Constants
  /// @{
  
  /// \brief Use this for fixed bodies.
  static const number QUALITY_FIXED;

  /// \brief Use this for moving objects with infinite mass.
  static const number QUALITY_KEYFRAMED;

  /// \brief Use this for all your debris objects.
  static const number QUALITY_DEBRIS;

  /// \brief Use this for debris objects that should have simplified TOI collisions with fixed/landscape objects.
  static const number QUALITY_DEBRIS_SIMPLE_TOI;

  /// \brief Use this for moving bodies, which should not leave the world,
  /// but you rather prefer those objects to tunnel through the world than
  /// dropping frames because the engine.
  static const number QUALITY_MOVING;

  /// \brief Use this for all objects, which you cannot afford to tunnel through
  /// the world at all.
  static const number QUALITY_CRITICAL;

  /// \brief Use this for very fast objects.
  static const number QUALITY_BULLET;

  /// \brief Use this for rigid body character controllers.
  static const number QUALITY_CHARACTER;

  /// \brief Use this for moving objects with infinite mass which should report
  /// contact points and TOI-collisions against all other bodies, including other
  /// fixed and keyframed bodies.
  /// \note Note that only non-TOI contact points are reported in collisions against debris-quality objects.
	const number QUALITY_KEYFRAMED_REPORTING;

  /// @}
  /// @name Collision Layer Constants
  /// @{
  
  static const int LAYER_ALL;   
	                  
  static const int LAYER_COLLIDABLE_DYNAMIC;			

  static const int LAYER_COLLIDABLE_STATIC;				

  static const int LAYER_COLLIDABLE_TERRAIN;			

  static const int LAYER_COLLIDABLE_CONTROLLER;		

  static const int LAYER_COLLIDABLE_TERRAIN_HOLE; 

  static const int LAYER_COLLIDABLE_DISABLED;			

  static const int LAYER_COLLIDABLE_RAGDOLL;	
			
  static const int LAYER_COLLIDABLE_ATTACHMENTS;	

  static const int LAYER_COLLIDABLE_FOOT_IK;			

  static const int LAYER_COLLIDABLE_DEBRIS;		
			
  static const int LAYER_COLLIDABLE_CUSTOM0;		
		
  static const int LAYER_COLLIDABLE_CUSTOM1;		
		
  static const int LAYER_COLLIDABLE_CUSTOM2;		
		
  static const int LAYER_COLLIDABLE_CUSTOM3;		
		
  static const int LAYER_COLLIDABLE_CUSTOM4;			

  /// @}
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
