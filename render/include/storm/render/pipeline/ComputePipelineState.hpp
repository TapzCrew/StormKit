// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <array>
#include <vector>

#include <storm/core/Hash.hpp>
#include <storm/core/Math.hpp>

#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Types.hpp>
#include <storm/render/pipeline/DescriptorSetLayout.hpp>
#include <storm/render/pipeline/Fwd.hpp>
#include <storm/render/resource/Fwd.hpp>

namespace storm::render {
    struct ComputePipelineShaderState {
        ShaderConstPtr shader;
    };

    struct ComputePipelineLayout {
        std::vector<DescriptorSetLayoutConstPtr> descriptor_set_layouts;
        std::vector<PushConstantRange> push_constant_ranges;
    };

    struct ComputePipelineState {
        ComputePipelineShaderState shader_state;
        ComputePipelineLayout layout;
    };

} // namespace storm::render

HASH_FUNC(storm::render::ComputePipelineShaderState)
HASH_FUNC(storm::render::ComputePipelineLayout)
HASH_FUNC(storm::render::ComputePipelineState)
