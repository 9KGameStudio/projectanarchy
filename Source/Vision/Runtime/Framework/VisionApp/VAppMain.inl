/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/Framework/VisionApp/VAppBase.hpp>

/// \brief
///   Platform specific main routines
#if defined(WIN32)
  #pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' " "version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
  #include <Vision/Runtime/Framework/VisionApp/Win/VAppWin.hpp> 
  #define VAPP_IMPLEMENT_SAMPLE(vapp_impl_class_name)                                                   \
    int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow ) \
    {                                                                                                   \
      VAppBase::VAppBasePtr spApp = new VAppWin(hInstance, hPrevInstance, lpCmdLine, nCmdShow);         \
      spApp->PlatformInit();                                                                            \
      spApp->ProcessStartupModules();                                                                   \
      spApp->Execute(new vapp_impl_class_name());                                                       \
      spApp->PlatformDeInit();                                                                          \
      spApp = NULL;                                                                                     \
      return 0;                                                                                         \
    }                                                                                                   \

#elif defined(_VISION_IOS)
  #include <Vision/Runtime/Framework/VisionApp/iOS/VAppIOS.hpp> 
  #define VAPP_IMPLEMENT_SAMPLE(vapp_impl_class_name)                                                   \
    VAppImpl* CreateInstance()                                                                          \
    {                                                                                                   \
      return new vapp_impl_class_name();                                                                \
    }                                                                                                   \

#elif defined(_VISION_ANDROID)
  #include <Vision/Runtime/Framework/VisionApp/Android/VAppAndroid.hpp> 
  #define VAPP_IMPLEMENT_SAMPLE(vapp_impl_class_name)                                                   \
    extern "C" void android_main(struct android_app* state)                                             \
    {                                                                                                   \
      app_dummy();                                                                                      \
      VAppBase::VAppBasePtr spApp = new VAppAndroid(state);                                             \
      spApp->PlatformInit();                                                                            \
      spApp->ProcessStartupModules();                                                                   \
      spApp->Execute(new vapp_impl_class_name());                                                       \
      spApp->PlatformDeInit();                                                                          \
      spApp = NULL;                                                                                     \
    }                                                                                                   \

#elif defined(_VISION_TIZEN)
  #include <Vision/Runtime/Framework/VisionApp/Tizen/VAppTizen.hpp> 
  #define VAPP_IMPLEMENT_SAMPLE(vapp_impl_class_name)                                                   \
    extern "C" _EXPORT_ int OspMain(int argc, char* argv[])                                             \
    {                                                                                                   \
      Tizen::Base::Collection::ArrayList* pArgs = new Tizen::Base::Collection::ArrayList();             \
      pArgs->Construct();                                                                               \
      for (int i = 0; i < argc; i++)                                                                    \
        pArgs->Add(*(new Tizen::Base::String(argv[i])));                                                \
                                                                                                        \
      VAppTizen::SetAppImpl(new vapp_impl_class_name);                                                  \
      Tizen::App::UiApp::Execute(VAppTizen::CreateInstance, pArgs);                                     \
      return 0;                                                                                         \
    }                                                                                                   \

#elif defined(_VISION_PS3)
  #include <Vision/Runtime/Framework/VisionApp/PS3/VAppPS3.hpp> 
  #define VAPP_IMPLEMENT_SAMPLE(vapp_impl_class_name)                                                   \
    SYS_PROCESS_PARAM (1001, 512*1024)                                                                  \
    int main(int argc, char* argv[])                                                                    \
    {                                                                                                   \
      VAppBase::VAppBasePtr spApp = new VAppPS3(argc, argv);                                            \
      spApp->PlatformInit();                                                                            \
      spApp->ProcessStartupModules();                                                                   \
      spApp->Execute(new vapp_impl_class_name());                                                       \
      spApp->PlatformDeInit();                                                                          \
      spApp = NULL;                                                                                     \
    }                                                                                                   \

#elif defined(_VISION_XENON)
  #include <Vision/Runtime/Framework/VisionApp/Xbox360/VAppXbox360.hpp> 
  #define VAPP_IMPLEMENT_SAMPLE(vapp_impl_class_name)                                                   \
    int main(int argc, char* argv[])                                                                    \
    {                                                                                                   \
      VAppBase::VAppBasePtr spApp = new VAppXbox360(argc, argv);                                        \
      spApp->PlatformInit();                                                                            \
      spApp->ProcessStartupModules();                                                                   \
      spApp->Execute(new vapp_impl_class_name());                                                       \
      spApp->PlatformDeInit();                                                                          \
      spApp = NULL;                                                                                     \
    }                                                                                                   \

#elif defined(_VISION_PSP2)
  #include <Vision/Runtime/Framework/VisionApp/PSVita/VAppPSVita.hpp> 

  #ifndef VISION_PSP2_MAIN_STACK_SIZE
    #define VISION_PSP2_MAIN_STACK_SIZE SCE_KERNEL_STACK_SIZE_DEFAULT_USER_MAIN
  #endif

  #ifndef VISION_PSP2_MAIN_THREAD_NAME
    #define VISION_PSP2_MAIN_THREAD_NAME "VisionMainThread"
  #endif

  #ifndef VISION_PSP2_LIBC_HEAP_SIZE
    #pragma warning("VISION_PSP2_LIBC_HEAP_SIZE not defined, using default (230mb)")

    #ifdef HK_DEBUG
      #define VISION_PSP2_LIBC_HEAP_SIZE (230 * 1024 * 1024)
    #else
      #define VISION_PSP2_LIBC_HEAP_SIZE (190 * 1024 * 1024)
    #endif
  #endif

  // NOTE: When changing the main thread priority and/or the CPU affinity mask, 
  //       certain limitations apply. Please refer to the SDK documentation,
  //       section 5.6 "Thread Scheduling" in the System chapter, for further details.
  #define VAPP_IMPLEMENT_SAMPLE(vapp_impl_class_name)                                                   \
    int sceUserMainThreadPriority        = SCE_KERNEL_DEFAULT_PRIORITY_USER;                            \
    int sceUserMainThreadCpuAffinityMask = SCE_KERNEL_THREAD_CPU_AFFINITY_MASK_DEFAULT;                 \
    unsigned int sceUserMainStackSize    = VISION_PSP2_MAIN_STACK_SIZE;                                 \
    unsigned int sceLibcHeapSize         = VISION_PSP2_LIBC_HEAP_SIZE;                                  \
    char sceUserMainThreadName[]         = VISION_PSP2_MAIN_THREAD_NAME;                                \
    int main(int argc, char* argv[])                                                                    \
    {                                                                                                   \
      VAppBase::VAppBasePtr spApp = new VAppPSVita(argc, argv);                                         \
      spApp->PlatformInit();                                                                            \
      spApp->ProcessStartupModules();                                                                   \
      spApp->Execute(new vapp_impl_class_name());                                                       \
      spApp->PlatformDeInit();                                                                          \
      spApp = NULL;                                                                                     \
    }                                                                                                   \

#elif defined(_VISION_WIIU)
  #include <Vision/Runtime/Framework/VisionApp/WiiU/VAppWiiU.hpp> 
  #define VAPP_IMPLEMENT_SAMPLE(vapp_impl_class_name)                                                   \
    int main(int argc, char* argv[])                                                                    \
    {                                                                                                   \
      VAppBase::VAppBasePtr spApp = new VAppWiiU(argc, argv);                                           \
      spApp->PlatformInit();                                                                            \
      spApp->ProcessStartupModules();                                                                   \
      spApp->Execute(new vapp_impl_class_name());                                                       \
      spApp->PlatformDeInit();                                                                          \
      spApp = NULL;                                                                                     \
    }                                                                                                   \

#endif


#define REGISTER_STARTUP_MODULE(class_name, prio)                                                       \
  static bool module_dummy##class_name = VAppBase::RegisterStartupModule(new class_name, prio);         \

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
