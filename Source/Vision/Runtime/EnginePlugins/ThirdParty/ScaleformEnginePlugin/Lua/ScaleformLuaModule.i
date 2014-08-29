/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// ===========================================================
// Scaleform Lua Module - SWIG Interface
// ===========================================================
// This interface file generates ScaleformLuaModule_wrapper.cpp
// and is represented in Lua as the "GFx" (Scaleform) module.

#ifndef VLUA_APIDOC

%include <windows.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/vision_types.i>

// Custom headers for generated source file
%module GFx
%{
  // GFx includes
  #include <GFx/GFx_Player.h>
  #include <GFx/GFx_Loader.h>
  #include <GFx/GFx_Log.h>
  #include <Render/Renderer2D.h>
  #include <Kernel/SF_Threads.h>

  #if defined (_VISION_WIN32) && defined(_VR_DX9)
    #include <GFx_Renderer_D3D9.h>
  #elif defined(_VISION_WIN32) && defined(_VR_DX11)
    #include <GFx_Renderer_D3D1x.h>
  #elif defined (_VISION_XENON)
    #include <Render/X360/X360_HAL.h>
  #elif defined (_VISION_PS3)
    #include <GFx_Renderer_PS3.h>
  #elif defined (_VISION_MOBILE)
    #include <GFx_Renderer_GL.h>
  #endif

  #ifdef SF_AMP_SERVER
    #include <GFx_AMP.h>
  #endif

  // Scaleform Engine Plugin includes
  #include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/vScaleformAlloc.hpp>
  #include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/vScaleformManager.hpp>
  #include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/VScaleformMovie.hpp>
  #include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/vScaleformInternal.hpp>
  #include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/VScaleformValue.hpp>
  #include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/VScaleformVariable.hpp>

  #include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/Lua/VScaleformLuaHelpers.inl>
%}

// Define auto-conversion to VScaleformValue.
// Needs to be defined before everything else. Otherwise the conversion will not be used.
%typemap(in) VScaleformValue
%{
  if (!VScaleform_ConvertScaleformValue(L, $argnum, &$1))
  {
     SWIG_fail_ptr("$symname", $argnum, SWIGTYPE_p_VScaleformValue);
  }
%}

%typemap(in) const VScaleformValue &(VScaleformValue temp)
{
  if (!VScaleform_ConvertScaleformValue(L, $argnum, &temp))
  {
     SWIG_fail_ptr("$symname", $argnum, SWIGTYPE_p_VScaleformValue);
  }
  $1 = &temp;
}
  
%include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/Lua/vScaleformManager.i>
%include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/Lua/vScaleformMovie.i>
%include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/Lua/VScaleformValue.i>
%include <Vision/Runtime/EnginePlugins/ThirdParty/ScaleformEnginePlugin/Lua/VScaleformVariable.i>

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
