// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <stormkit/Core/PlatformMacro.hpp>

#if not defined(STORMKIT_OS_WINDOWS)
    #define main userMain
#endif