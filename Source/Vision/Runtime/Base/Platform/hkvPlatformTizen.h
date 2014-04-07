/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKVPLATFORMDEFINESTIZEN_H
#define HKVPLATFORMDEFINESTIZEN_H

  #define VERSION_TYPE "TIZEN"

  #define _VR_GLES2
  #define MAX_NUM_RENDERTARGETS 1
  #define MAX_TEXTURE_UNIT_COUNT     16
  #define RESTRICT
  #define SUPPORTS_SKINNED_TANGENTS
  #define PLATFORM_HOME_DIR ""
  #define SUPPORTS_USERPOINTER_RENDERING
  #define SUPPORTS_MODULATEXN_LIGHTMAPPING
  #define SCREEN_MESSAGE_DEFAULT_SCALING 1.0f
  #define RESOURCE_MANAGER_DEFAULT_MEMORY_TEXTURES (128*1024*1024)
  #define CLIPSPACE_DEPTH_IS_MINUSONE_TO_ONE
  #define SUPPORTS_PARTIAL_BUFFER_LOCKS
  #define VERTEX_RGBA8_ATTRIB_SWIZZLE_MODE SWIZZLE_VERTEX_RGBA8_TO_BGRA8

  #define SUPPORTS_RESOURCEVIEWER
  #define VSTREAMPROCESS_RUN_IN_THREADMANAGER
  #define V_DEVICE_MAX_TEXTURE_SIZE VVideo::GetMaxTextureSizeSupported()
  #define SUPPORTS_PVR
  #define SUPPORTS_MULTITHREADING
  #define NEEDS_SCENE_STREAMING
  #define NEEDS_SCENE_PREWARMING
  #define SUPPORTS_MULTITOUCH
  #define SUPPORTS_MOTIONINPUT
  #define NATIVE_PATH_SEPARATOR_FWDSLASH
  #define _VISION_MOBILE
  #define SUPPORTS_RESOURCE_TIMESTAMP
  #define SUPPORTS_VTARGET
  #define SUPPORTS_ALPHATEST_BLENDSTATE
  #define SUPPORTS_SOFTKEYBOARD
  #define SUPPORTS_SHADOW_MAPS
  #define SUPPORTS_LOD_HYSTERESIS_THRESHOLDING

  #define SUPPORTS_GPU_DATA_UPLOAD
  #define SUPPORTS_FILESERVE_CLIENT

  #define NEEDS_SHADER_SOURCE_PATCHER
  #define SUPPORTS_DEBUG_SHADING

  #if !defined(_VR_GLES2)
    #define _VR_GLES2
  #endif

#endif // HKVPLATFORMDEFINESTIZEN_H

/*
 * Havok SDK - Base file, BUILD(#20140328)
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
