// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#ifdef STORMKIT_BUILD_MODULES
module;
#endif

#include <stormkit/Core/PlatformMacro.hpp>

#ifdef STORMKIT_OS_WINDOWS
    #if defined(STORMKIT_OS_WINDOWS)
        #define VK_USE_PLATFORM_WIN32_KHR 1
    #elif defined(STORMKIT_OS_MACOS)
        #define VK_USE_PLATFORM_MACOS_MVK 1
    #elif defined(STORMKIT_OS_IOS)
        #define VK_USE_PLATFORM_IOS_MVK 1
    #endif
#endif

#define VOLK_IMPLEMENTATION
#include <volk.h>
