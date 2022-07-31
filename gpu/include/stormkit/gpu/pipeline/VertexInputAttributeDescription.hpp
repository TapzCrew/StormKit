// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/HashMacro.hpp>

#include <stormkit/core/Types.hpp>

#include <stormkit/gpu/core/Fwd.hpp>
#include <stormkit/gpu/core/Types.hpp>

namespace stormkit::gpu {
    struct VertexInputAttributeDescription {
        core::UInt32 location;
        core::UInt32 binding;
        Format format;
        core::UInt32 offset;
    };
} // namespace stormkit::gpu

HASH_FUNC(stormkit::gpu::VertexInputAttributeDescription)