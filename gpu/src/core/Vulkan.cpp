#define VOLK_IMPLEMENTATION

#include <stormkit/core/PlatformMacro.hpp>

#if STORMKIT_WSI_ENABLED
    #if defined(STORMKIT_OS_LINUX)
        #if STORMKIT_WSI_BUILD_X11
            #define VK_USE_PLATFORM_XCB_KHR 1
        #endif
        #if STORMKIT_WSI_BUILD_WAYLAND
            #define VK_USE_PLATFORM_WAYLAND_KHR 1
        #endif
    #elif defined(STORMKIT_OS_WINDOWS)
        #define VK_USE_PLATFORM_WIN32_KHR 1
    #elif defined(STORMKIT_OS_MACOS)
        #define VK_USE_PLATFORM_MACOS_MVK 1
    #elif defined(STORMKIT_OS_IOS)
        #define VK_USE_PLATFORM_IOS_MVK 1
    #endif
#endif

#include <volk.h>
