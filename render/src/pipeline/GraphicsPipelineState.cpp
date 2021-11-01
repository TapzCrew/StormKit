#include <storm/render/pipeline/GraphicsPipelineState.hpp>

using namespace storm;
using namespace storm::render;

namespace std {
    core::Hash64 hash<GraphicsPipelineVertexInputState>::operator()(
        const GraphicsPipelineVertexInputState &state) const noexcept {
        auto hash = core::Hash64 { 0 };
        core::hashCombine(hash, state.binding_descriptions);
        core::hashCombine(hash, state.input_attribute_descriptions);

        return hash;
    }

    core::Hash64 hash<GraphicsPipelineInputAssemblyState>::operator()(
        const GraphicsPipelineInputAssemblyState &state) const noexcept {
        auto hash = core::Hash64 { 0 };
        core::hashCombine(hash, state.topology);
        core::hashCombine(hash, state.primitive_restart_enable);

        return hash;
    }

    core::Hash64 hash<GraphicsPipelineViewportState>::operator()(
        const GraphicsPipelineViewportState &state) const noexcept {
        auto hash = core::Hash64 { 0 };
        for (const auto &viewport : state.viewports) core::hashCombine(hash, viewport);
        for (const auto &scissor : state.scissors) core::hashCombine(hash, scissor);

        return hash;
    }

    core::Hash64 hash<GraphicsPipelineRasterizationState>::operator()(
        const GraphicsPipelineRasterizationState &state) const noexcept {
        auto hash = core::Hash64 { 0 };
        core::hashCombine(hash, state.depth_clamp_enable);
        core::hashCombine(hash, state.rasterizer_discard_enable);
        core::hashCombine(hash, state.polygon_mode);
        core::hashCombine(hash, state.line_width);
        core::hashCombine(hash, state.cull_mode);
        core::hashCombine(hash, state.front_face);

        return hash;
    }

    core::Hash64 hash<GraphicsPipelineMultiSampleState>::operator()(
        const GraphicsPipelineMultiSampleState &state) const noexcept {
        auto hash = core::Hash64 { 0 };
        core::hashCombine(hash, state.sample_shading_enable);
        core::hashCombine(hash, state.rasterization_samples);

        return hash;
    }

    core::Hash64 hash<GraphicsPipelineColorBlendAttachmentState>::operator()(
        const GraphicsPipelineColorBlendAttachmentState &state) const noexcept {
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

    core::Hash64 hash<GraphicsPipelineColorBlendState>::operator()(
        const GraphicsPipelineColorBlendState &state) const noexcept {
        auto hash = core::Hash64 { 0 };
        core::hashCombine(hash, state.logic_operation_enable);
        core::hashCombine(hash, state.logic_operation);

        for (const auto &attachment : state.attachments) core::hashCombine(hash, attachment);
        for (const auto blend_constant : state.blend_constants)
            core::hashCombine(hash, blend_constant);

        return hash;
    }

    core::Hash64 hash<GraphicsPipelineDynamicState>::operator()(
        [[maybe_unused]] const GraphicsPipelineDynamicState &state) const noexcept {
        return 0;
    }

    core::Hash64 hash<GraphicsPipelineShaderState>::operator()(
        [[maybe_unused]] const GraphicsPipelineShaderState &state) const noexcept {
        auto hash = core::Hash64 { 0 };
        for (const auto &shader : state.shaders) core::hashCombine(hash, shader);

        return 0;
    }

    core::Hash64 hash<GraphicsPipelineDepthStencilState>::operator()(
        const GraphicsPipelineDepthStencilState &state) const noexcept {
        auto hash = core::Hash64 { 0 };
        core::hashCombine(hash, state.depth_test_enable);
        core::hashCombine(hash, state.depth_write_enable);
        core::hashCombine(hash, state.depth_compare_op);
        core::hashCombine(hash, state.depth_bounds_test_enable);
        core::hashCombine(hash, state.min_depth_bounds);
        core::hashCombine(hash, state.max_depth_bounds);

        return hash;
    }

    core::Hash64 hash<GraphicsPipelineLayout>::operator()(
        const GraphicsPipelineLayout &state) const noexcept {
        if (std::empty(state.descriptor_set_layouts)) return 0;

        auto hash = core::Hash64 { 0 };
        for (const auto &layout : state.descriptor_set_layouts)
            core::hashCombine(hash, layout->hash());

        return hash;
    }

    core::Hash64
        hash<GraphicsPipelineState>::operator()(const GraphicsPipelineState &state) const noexcept {
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
