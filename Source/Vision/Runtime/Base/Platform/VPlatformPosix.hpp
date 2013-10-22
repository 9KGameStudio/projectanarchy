/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VPlatformPosix.hpp

#ifndef VPLATFORM_POSIX_INCLUDED
#define VPLATFORM_POSIX_INCLUDED


// Check compiler (using another toolchain will most certainly crash or simply not compile).
#if !defined(__GNUC__)
#error Unsupported C/C++ compiler!
#endif

////////////////////////////////////////////////////////////////
//  Platform-specific headers
////////////////////////////////////////////////////////////////

#include <unistd.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifdef _VISION_IOS
  #include <libkern/OSAtomic.h>
#elif defined(_VISION_ANDROID)
  #include <sys/atomics.h>
#elif defined(_VISION_TIZEN)
#else
  #warning "Missing header for atomic operations!"
#endif

#if defined(_VISION_IOS)
#include <cmath>
#endif

#include <wchar.h>
#include <assert.h>
#include <sys/time.h>
#include <pthread.h>

////////////////////////////////////////////////////////////////
//  Defines for cross-platform interoperability
////////////////////////////////////////////////////////////////

//@@@ linux is lp64, so check on 64bit for long only
typedef long long int int64;
typedef unsigned long long int uint64;
typedef long long int __int64;
typedef long long int _int64;

typedef int64 __time64_t;
typedef struct utimbuf __utimbuf64;
        
inline struct tm* _localtime64(const __time64_t* t)
{
  time_t t_ = (time_t)*t;
  return localtime(&t_);//@@@L check if this works
}

inline struct tm* _gmtime64(const __time64_t* t)
{
  time_t t_ = (time_t)*t;
  return gmtime(&t_);//@@@L check if this works
}

inline __time64_t _time64(__time64_t* t)
{
  time_t t_;
  time( &t_ );
  __time64_t t64 = t_;
  if ( t != NULL )
  {
    *t = t_;
  }
  return t64;
}

#define __cdecl


typedef unsigned int HANDLE;
typedef int INT_PTR;
typedef unsigned int UINT_PTR;
#ifndef _VISION_DOC
typedef union _LARGE_INTEGER { 
  struct { 
    unsigned long LowPart; 
    long HighPart; 
  };  
  struct { 
    unsigned long LowPart; 
    long HighPart; 
  } u; 
  long long QuadPart;
} LARGE_INTEGER, *PLARGE_INTEGER;
#endif //_VISION_DOC
typedef long LONG;
typedef unsigned char byte;
typedef void* HMODULE;
typedef void* HINSTANCE;
typedef void* WNDPROC;

//@@@ ADJUST!!!
const int THREAD_PRIORITY_LOWEST       = -2;
const int THREAD_PRIORITY_BELOW_NORMAL = -1;
const int THREAD_PRIORITY_NORMAL       =  0;
const int THREAD_PRIORITY_ABOVE_NORMAL =  1;
const int THREAD_PRIORITY_HIGHEST      =  2;


#define DDPF_ALPHAPIXELS      0x00000001l
#define DDPF_ALPHA            0x00000002l
#define DDPF_RGB              0x00000040l
#define DDPF_PALETTEINDEXED8  0x00000020l
#define DDPF_FOURCC           0x00000004l
#define DDPF_LUMINANCE        0x00020000l





//@@@ ADAPT!
const int FS_MAX_FILE  = 512; // Didn't find a predefined macro
#if defined(DVD_PATH_MAX)     // Defined for Mac emulator
const int FS_MAX_PATH = DVD_PATH_MAX;
#else
const int FS_MAX_PATH  = 512; // Didn't find a predefined macro
#endif
const int FS_MAX_DRIVE = 16;  // Didn't find equivalent - stay modest

#define VFILE_CHAR_SEPARATOR '/'
#define VFILE_STR_SEPARATOR "/"

//@@@#define LINUX_DVD_PATH_PREFIX "/dvd"

//@@@#define FS_STRCMP strcmp
//@@@#define FS_STRNCMP strncmp


#define _getcwd     getcwd
#define _chdir      chdir
#define _mkdir(x)   mkdir((x), 0777)
#define _rmdir      rmdir
#define _stricmp    strcasecmp
#define _strnicmp   strncasecmp
#define vsnwprintf  vswprintf

#ifndef _VISION_TIZEN
char* strlwr(char *string);
#endif
int wcsicmp(const wchar_t *s1, const wchar_t *s2);
wchar_t *wcstok(wchar_t *s1, const wchar_t *s2);

#include <Vision/Runtime/Base/String/VSafeStringFunctions.hpp>

inline void Sleep(unsigned int iMilliseconds) { usleep(iMilliseconds * 1000); }
inline void MicroSleep(unsigned int iMicroSeconds) { usleep(iMicroSeconds); }

#define VISION_LIKELY(COND) COND
#define VISION_UNLIKELY(COND) COND

//
// *** Structs
//

#ifndef _VISION_DOC
typedef struct _D3DMATRIX { // TODO: USE OWN MATRIX CLASS ?
    union {
        struct {
            float        _11, _12, _13, _14;
            float        _21, _22, _23, _24;
            float        _31, _32, _33, _34;
            float        _41, _42, _43, _44;
        };
        float m[4][4];
    };
} D3DMATRIX;
#endif //_VISION_DOC


// glibc only defines int abs(int)
#ifndef _VISION_TIZEN
template <typename T>
T abs(T x) 
{
  return (x >= 0) ? x : -x;
}
#endif

#define DECLSPEC_DLLEXPORT 


#ifdef _VISION_ANDROID

  #include <sys/types.h>
  #include <sys/stat.h>
  #include <fcntl.h>

  #include <jni.h>
  #include <android/log.h>
  #include <android/native_app_glue/android_native_app_glue.h>

  #define printf(...) __android_log_print(ANDROID_LOG_WARN, "printf", __VA_ARGS__)
  
  extern android_app *AndroidApplication;
  extern void AndroidHandleCmd(struct android_app* app, int32_t cmd);
  extern int PollAndroidOnce();
  extern bool PollAndroidNativeEnvironment();
  extern void InitAndroidNativeEnvironment(char* szAPKDirectory, char* szSDCardDirectory, char* szCacheDirectory, int Size = FS_MAX_PATH);
  extern void DeinitAndroidNativeEnvironment();
  extern void SetOverrideCacheDirectory(const char* szOverrideCacheDirectory);
  extern "C" char g_szDeviceModel[256]; // model of the Android device, the application is currently running on

  extern void AndroidShowAlert(const char* pTitle, const char* pMessage);
#endif

#if defined(_VISION_TIZEN)

	#include <osp/egl.h>
	#include <osp/eglext.h>
	#include <osp/gl2.h>
	#include <osp/gl2ext.h>

  #define printf(...) AppLogTag("Vision", __VA_ARGS__)

#endif

#if defined(_VISION_IOS)
  extern "C" char g_szDeviceName[256];
#endif


#endif //VPLATFORM_POSIX_INCLUDED

/*
 * Havok SDK - Base file, BUILD(#20131019)
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
