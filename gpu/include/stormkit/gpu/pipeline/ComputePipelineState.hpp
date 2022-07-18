// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/HashMacro.hpp>

#include <stormkit/core/Types.hpp>

#include <stormkit/gpu/core/Fwd.hpp>
#include <stormkit/gpu/core/Types.hpp>

#include <stormkit/gpu/resource/Fwd.hpp>

#include <stormkit/gpu/pipeline/Fwd.hpp>

namespace stormkit::gpu {
    struct ComputePipelineShaderState {
        const Shader *shader = nullptr;
    };

    struct ComputePipelineLayout {
        std::vector<const DescriptorSetLayout *> descriptor_set_layouts;
        std::vector<PushConstantRange> push_constant_ranges;
    };

    struct ComputePipelineState {
        ComputePipelineShaderState shader_state;
        ComputePipelineLayout layout;
    };

} // namespace stormkit::gpu

HASH_FUNC(stormkit::gpu::ComputePipelineShaderState)
HASH_FUNC(stormkit::gpu::ComputePipelineLayout)
HASH_FUNC(stormkit::gpu::ComputePipelineState)
