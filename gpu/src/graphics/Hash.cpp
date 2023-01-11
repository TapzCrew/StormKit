// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#ifdef STORMKIT_BUILD_MODULES
module stormkit.Gpu:Graphics;

import :RasterPipeline;
import :RasterPipelineState;
#else
    #include <stormkit/std.hpp>

    #include <stormkit/Core.hpp>
    #include <stormkit/Gpu.hpp>
#endif

using namespace stormkit;

namespace std {
    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<gpu::RasterPipelineVertexInputState>::operator()(
        const gpu::RasterPipelineVertexInputState& state) const noexcept -> core::Hash64 {
        auto hash = core::Hash64 { 0 };
        core::hashCombineRange(hash, state.binding_descriptions);
        core::hashCombineRange(hash, state.input_attribute_descriptions);

        return hash;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<gpu::RasterPipelineInputAssemblyState>::operator()(
        const gpu::RasterPipelineInputAssemblyState& state) const noexcept -> core::Hash64 {
        auto hash = core::Hash64 { 0 };
        core::hashCombine(hash, state.topology);
        core::hashCombine(hash, state.primitive_restart_enable);

        return hash;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<gpu::RasterPipelineViewportState>::operator()(
        const gpu::RasterPipelineViewportState& state) const noexcept -> core::Hash64 {
        auto hash = core::Hash64 { 0 };
        core::hashCombineRange(hash, state.viewports);
        core::hashCombineRange(hash, state.scissors);

        return hash;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<gpu::RasterPipelineRasterizationState>::operator()(
        const gpu::RasterPipelineRasterizationState& state) const noexcept -> core::Hash64 {
        auto hash = core::Hash64 { 0 };
        core::hashCombine(hash, state.depth_clamp_enable);
        core::hashCombine(hash, state.rasterizer_discard_enable);
        core::hashCombine(hash, state.polygon_mode);
        core::hashCombine(hash, state.line_width);
        core::hashCombine(hash, state.cull_mode);
        core::hashCombine(hash, state.front_face);

        return hash;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<gpu::RasterPipelineMultiSampleState>::operator()(
        const gpu::RasterPipelineMultiSampleState& state) const noexcept -> core::Hash64 {
        auto hash = core::Hash64 { 0 };
        core::hashCombine(hash, state.sample_shading_enable);
        core::hashCombine(hash, state.rasterization_samples);

        return hash;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<gpu::RasterPipelineColorBlendAttachmentState>::operator()(
        const gpu::RasterPipelineColorBlendAttachmentState& state) const noexcept -> core::Hash64 {
        auto hash = core::Hash64 { 0 };
        core::hashCombine(hash, state.color_write_mask);
        core::hashCombine(hash, state.blend_enable);
        core::hashCombine(hash, state.src_color_blend_factor);
        core::hashCombine(hash, state.dst_color_blend_factor);
        core::hashCombine(hash, state.color_blend_operation);
        core::hashCombine(hash, state.src_alpha_blend_factor);
        core::hashCombine(hash, state.dst_alpha_blend_factor);
        core::hashCombine(hash, state.alpha_blend_operation);

        return hash;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<gpu::RasterPipelineColorBlendState>::operator()(
        const gpu::RasterPipelineColorBlendState& state) const noexcept -> core::Hash64 {
        auto hash = core::Hash64 { 0 };
        core::hashCombine(hash, state.logic_operation_enable);
        core::hashCombine(hash, state.logic_operation);

        core::hashCombineRange(hash, state.attachments);
        core::hashCombineRange(hash, state.blend_constants);

        return hash;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<gpu::RasterPipelineDynamicState>::operator()(
        [[maybe_unused]] const gpu::RasterPipelineDynamicState& state) const noexcept
        -> core::Hash64 {
        return 0;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<gpu::RasterPipelineShaderState>::operator()(
        [[maybe_unused]] const gpu::RasterPipelineShaderState& state) const noexcept
        -> core::Hash64 {
        auto hash = core::Hash64 { 0 };
        core::hashCombineRange(hash, state.shaders);

        return 0;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<gpu::RasterPipelineDepthStencilState>::operator()(
        const gpu::RasterPipelineDepthStencilState& state) const noexcept -> core::Hash64 {
        auto hash = core::Hash64 { 0 };
        core::hashCombine(hash, state.depth_test_enable);
        core::hashCombine(hash, state.depth_write_enable);
        core::hashCombine(hash, state.depth_compare_op);
        core::hashCombine(hash, state.depth_bounds_test_enable);
        core::hashCombine(hash, state.min_depth_bounds);
        core::hashCombine(hash, state.max_depth_bounds);

        return hash;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<gpu::RasterPipelineLayout>::operator()(
        const gpu::RasterPipelineLayout& state) const noexcept -> core::Hash64 {
        if (std::empty(state.descriptor_set_layouts)) return 0;

        auto hash = core::Hash64 { 0 };
        for (const auto& layout : state.descriptor_set_layouts)
            core::hashCombine(hash, layout->hash());

        return hash;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<gpu::RasterPipelineState>::operator()(
        const gpu::RasterPipelineState& state) const noexcept -> core::Hash64 {
        auto hash = core::Hash64 { 0 };
        core::hashCombine(hash, state.input_assembly_state);
        core::hashCombine(hash, state.viewport_state);
        core::hashCombine(hash, state.rasterization_state);
        core::hashCombine(hash, state.multisample_state);
        core::hashCombine(hash, state.color_blend_state);
        core::hashCombine(hash, state.dynamic_state);
        core::hashCombine(hash, state.shader_state);
        core::hashCombine(hash, state.vertex_input_state);
        core::hashCombine(hash, state.depth_stencil_state);
        core::hashCombine(hash, state.layout);

        return hash;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<gpu::VertexBindingDescription>::operator()(
        const gpu::VertexBindingDescription& description) const noexcept -> core::Hash64 {
        auto hash = core::Hash64 { 0 };
        core::hashCombine(hash, description.binding);
        core::hashCombine(hash, description.stride);
        core::hashCombine(hash, description.input_rate);

        return hash;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<gpu::VertexInputAttributeDescription>::operator()(
        const gpu::VertexInputAttributeDescription& description) const noexcept -> core::Hash64 {
        auto hash = core::Hash64 { 0 };
        core::hashCombine(hash, description.binding);
        core::hashCombine(hash, description.location);
        core::hashCombine(hash, description.format);
        core::hashCombine(hash, description.offset);

        return hash;
    }
} // namespace std
