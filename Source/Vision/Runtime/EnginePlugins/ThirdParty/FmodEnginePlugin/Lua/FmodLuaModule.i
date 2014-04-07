/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// ===========================================================
// Fmod Lua Module - SWIG Interface
// ===========================================================
// This interface file generates FmodLuaModule_wrapper.cpp
// and is represented in Lua as the "Fmod" module.

#ifndef VLUA_APIDOC

//get access to all vision specific things...
%include <windows.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/vision_types.i>

//dependencies of ObjectComponent:
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VColor.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VTypedObject.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VisApiTypedEngineObject.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VisApiObject3D.i>

//custom headers for generated source file
%module FireLight
%{
  #include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/VFmodManager.hpp>
%}
  
%include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/Lua/VFmodManager.i>;
%include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/Lua/VFmodSoundObject.i>;
%include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/Lua/VFmodEvent.i>;

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
