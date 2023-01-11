// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#ifdef STORMKIT_BUILD_MODULES
module stormkit.Gpu:Compute;

import :ComputePipeline;
import :ComputePipelineState;
#else
    #include <stormkit/std.hpp>

    #include <stormkit/Core.hpp>
    #include <stormkit/Gpu.hpp>
#endif

using namespace stormkit;

namespace std {
    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<gpu::ComputePipelineShaderState>::operator()(
        const gpu::ComputePipelineShaderState& state) const noexcept -> core::Hash64 {
        auto hash = core::Hash64 { 0 };
        core::hashCombine(hash, state.shader);

        return 0;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<gpu::ComputePipelineLayout>::operator()(
        const gpu::ComputePipelineLayout& state) const noexcept -> core::Hash64 {
        if (std::empty(state.descriptor_set_layouts)) return 0;

        auto hash = core::Hash64 { 0 };
        for (const auto& layout : state.descriptor_set_layouts)
            core::hashCombine(hash, layout->hash());

        return hash;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<gpu::ComputePipelineState>::operator()(
        const gpu::ComputePipelineState& state) const noexcept -> core::Hash64 {
        auto hash = core::Hash64 { 0 };
        core::hashCombine(hash, state.shader_state);
        core::hashCombine(hash, state.layout);

        return hash;
    }
} // namespace std
