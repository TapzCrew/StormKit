// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#ifdef STORMKIT_BUILD_DEBUG
    #define STORMKIT_RENDER_VERBOSE_VULKAN true
#endif

#ifdef STORMKIT_RENDER_VERBOSE_VULKAN
    #define CHECK_VK_ERROR(line)                                                  \
        do {                                                                      \
            const auto _result = line;                                            \
            checkVkError(_result, __FILE__, STORMKIT_CURRENT_FUNCTION, __LINE__); \
        } while (false);
#else
    #define CHECK_VK_ERROR(line)                     \
        {                                            \
            const auto _result = line;               \
            STORMKIT_ENSURES(_result == VK_SUCCESS); \
        }
#endif
