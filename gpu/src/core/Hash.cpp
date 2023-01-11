// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#ifdef STORMKIT_BUILD_MODULES
module stormkit.Gpu:Core;
#else
    #include <stormkit/std.hpp>

    #include <stormkit/Core.hpp>
    #include <stormkit/Gpu.hpp>
#endif

using namespace stormkit;

namespace std {
    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<gpu::Viewport>::operator()(const gpu::Viewport& viewport) const noexcept
        -> core::Hash64 {
        auto hash = core::Hash64 { 0 };
        core::hashCombine(hash, viewport.depth);
        core::hashCombine(hash, viewport.extent);
        core::hashCombine(hash, viewport.position);

        return hash;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<gpu::Scissor>::operator()(const gpu::Scissor& scissor) const noexcept
        -> core::Hash64 {
        auto hash = core::Hash64 { 0 };
        core::hashCombine(hash, scissor.offset);
        core::hashCombine(hash, scissor.extent);

        return hash;
    }
} // namespace std
