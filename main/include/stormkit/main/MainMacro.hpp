// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/PlatformMacro.hpp>

#if defined(STORMKIT_OS_IOS)

    #define main iosMain

#elif defined(STORMKIT_OS_MACOS)

    #define main macosMain

#endif
