// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/Types.hpp>

#include <stormkit/gpu/core/Enums.hpp>
#include <stormkit/gpu/core/Fwd.hpp>

namespace stormkit::gpu {
    struct QueueFamily {
        QueueFlag flags;
        core::UInt32 count;

        bool has_present_support = false;
    };
} // namespace stormkit::gpu
