// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/PlatformMacro.hpp>

#ifdef STORMKIT_BUILD_DEBUG
    #define STORMKIT_RENDER_VERBOSE_VULKAN true
#endif

#ifdef STORMKIT_RENDER_VERBOSE_VULKAN
    #define CHECK_VK_ERROR(line)                    \
        do {                                        \
            const auto _result = line;              \
            checkVkError(_result,                   \
                         STORMKIT_STRINGIFY(line),  \
                         STORMKIT_CURRENT_FILE,     \
                         STORMKIT_CURRENT_FUNCTION, \
                         STORMKIT_CURRENT_LINE);    \
        } while (false);
#else
    #define CHECK_VK_ERROR(line)                     \
        {                                            \
            const auto _result = line;               \
            STORMKIT_ENSURES(_result == VK_SUCCESS); \
        }
#endif
