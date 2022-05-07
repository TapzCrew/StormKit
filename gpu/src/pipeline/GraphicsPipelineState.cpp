#include <stormkit/gpu/pipeline/DescriptorSetLayout.mpp>
#include <stormkit/gpu/pipeline/GraphicsPipelineState.mpp>

using namespace stormkit;

namespace std {
    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<gpu::GraphicsPipelineVertexInputState>::operator()(
        const gpu::GraphicsPipelineVertexInputState &state) const noexcept -> core::Hash64 {
        auto hash = core::Hash64 { 0 };
        core::hashCombine(hash, state.binding_descriptions);
        core::hashCombine(hash, state.input_attribute_descriptions);

        return hash;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<gpu::GraphicsPipelineInputAssemblyState>::operator()(
        const gpu::GraphicsPipelineInputAssemblyState &state) const noexcept -> core::Hash64 {
        auto hash = core::Hash64 { 0 };
        core::hashCombine(hash, state.topology);
        core::hashCombine(hash, state.primitive_restart_enable);

        return hash;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<gpu::GraphicsPipelineViewportState>::operator()(
        const gpu::GraphicsPipelineViewportState &state) const noexcept -> core::Hash64 {
        auto hash = core::Hash64 { 0 };
        for (const auto &viewport : state.viewports) core::hashCombine(hash, viewport);
        for (const auto &scissor : state.scissors) core::hashCombine(hash, scissor);

        return hash;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<gpu::GraphicsPipelineRasterizationState>::operator()(
        const gpu::GraphicsPipelineRasterizationState &state) const noexcept -> core::Hash64 {
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
    auto hash<gpu::GraphicsPipelineMultiSampleState>::operator()(
        const gpu::GraphicsPipelineMultiSampleState &state) const noexcept -> core::Hash64 {
        auto hash = core::Hash64 { 0 };
        core::hashCombine(hash, state.sample_shading_enable);
        core::hashCombine(hash, state.rasterization_samples);

        return hash;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<gpu::GraphicsPipelineColorBlendAttachmentState>::operator()(
        const gpu::GraphicsPipelineColorBlendAttachmentState &state) const noexcept
        -> core::Hash64 {
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
    auto hash<gpu::GraphicsPipelineColorBlendState>::operator()(
        const gpu::GraphicsPipelineColorBlendState &state) const noexcept -> core::Hash64 {
        auto hash = core::Hash64 { 0 };
        core::hashCombine(hash, state.logic_operation_enable);
        core::hashCombine(hash, state.logic_operation);

        for (const auto &attachment : state.attachments) core::hashCombine(hash, attachment);
        for (const auto blend_constant : state.blend_constants)
            core::hashCombine(hash, blend_constant);

        return hash;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<gpu::GraphicsPipelineDynamicState>::operator()(
        [[maybe_unused]] const gpu::GraphicsPipelineDynamicState &state) const noexcept
        -> core::Hash64 {
        return 0;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<gpu::GraphicsPipelineShaderState>::operator()(
        [[maybe_unused]] const gpu::GraphicsPipelineShaderState &state) const noexcept
        -> core::Hash64 {
        auto hash = core::Hash64 { 0 };
        for (const auto &shader : state.shaders) core::hashCombine(hash, shader);

        return 0;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<gpu::GraphicsPipelineDepthStencilState>::operator()(
        const gpu::GraphicsPipelineDepthStencilState &state) const noexcept -> core::Hash64 {
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
    auto hash<gpu::GraphicsPipelineLayout>::operator()(
        const gpu::GraphicsPipelineLayout &state) const noexcept -> core::Hash64 {
        if (std::empty(state.descriptor_set_layouts)) return 0;

        auto hash = core::Hash64 { 0 };
        for (const auto &layout : state.descriptor_set_layouts)
            core::hashCombine(hash, layout->hash());

        return hash;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<gpu::GraphicsPipelineState>::operator()(
        const gpu::GraphicsPipelineState &state) const noexcept -> core::Hash64 {
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
} // namespace std
