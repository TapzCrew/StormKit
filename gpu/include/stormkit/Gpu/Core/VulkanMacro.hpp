// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <stormkit/Core/PlatformMacro.hpp>

#ifdef STORMKIT_BUILD_DEBUG
    #define STORMKIT_RENDER_VERBOSE_VULKAN
#endif

#ifdef STORMKIT_RENDER_VERBOSE_VULKAN
    #define CHECK_VK_ERROR(line)                             \
        do {                                                 \
            const auto _result = line;                       \
            checkVkError(_result, STORMKIT_STRINGIFY(line)); \
        } while (false);
#else
    #define CHECK_VK_ERROR(line)                  \
        {                                         \
            const auto _result = line;            \
            core::ensures(_result == VK_SUCCESS); \
        }
#endif
