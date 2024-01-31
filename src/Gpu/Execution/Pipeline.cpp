// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module stormkit.Gpu;

import std;

import stormkit.Core;

import :Vulkan;
import :Core;
import :Execution.Pipeline;
import :Execution.RenderPass;
import :Execution.RasterPipelineState;

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    auto Pipeline::doInitRasterPipeline(
        const PipelineLayout                             & layout,
        const RenderPass                                 & render_pass,
        std::optional<core::NakedRef<const PipelineCache>> pipeline_cache) noexcept
        -> VulkanExpected<void> {
        const auto& state = core::as<RasterPipelineState>(m_state);

        const auto binding_descriptions =
            state.vertex_input_state.binding_descriptions |
            std::views::transform([](auto&& binding_description) noexcept {
                return vk::VertexInputBindingDescription {}
                    .setBinding(binding_description.binding)
                    .setStride(binding_description.stride)
                    .setInputRate(core::narrow<vk::VertexInputRate>(binding_description.input_rate));
            }) |
            std::ranges::to<std::vector>();

        const auto attribute_descriptions =
            state.vertex_input_state.input_attribute_descriptions |
            std::views::transform([](auto&& input_attribute_description) noexcept {
                return vk::VertexInputAttributeDescription {}
                    .setLocation(input_attribute_description.location)
                    .setBinding(input_attribute_description.binding)
                    .setFormat(core::narrow<vk::Format>(input_attribute_description.format))
                    .setOffset(input_attribute_description.offset);
            }) |
            std::ranges::to<std::vector>();

        const auto vertex_input_info = vk::PipelineVertexInputStateCreateInfo {}
                                           .setVertexBindingDescriptions(binding_descriptions)
                                           .setVertexAttributeDescriptions(attribute_descriptions);

        const auto input_assembly =
            vk::PipelineInputAssemblyStateCreateInfo {}
                .setTopology(core::narrow<vk::PrimitiveTopology>(state.input_assembly_state.topology))
                .setPrimitiveRestartEnable(state.input_assembly_state.primitive_restart_enable);

        const auto viewports = state.viewport_state.viewports |
                               std::views::transform([](auto&& viewport) noexcept {
                                   return vk::Viewport {}
                                       .setX(viewport.position.x)
                                       .setY(viewport.position.y)
                                       .setWidth(viewport.extent.width)
                                       .setHeight(viewport.extent.height)
                                       .setMinDepth(viewport.depth.x)
                                       .setMaxDepth(viewport.depth.y);
                               }) |
                               std::ranges::to<std::vector>();

        const auto scissors =
            state.viewport_state.scissors | std::views::transform([](auto&& scissor) noexcept {
                return vk::Rect2D {}
                    .setOffset(vk::Offset2D { scissor.offset.x, scissor.offset.y })
                    .setExtent(vk::Extent2D { scissor.extent.width, scissor.extent.height });
            }) |
            std::ranges::to<std::vector>();

        const auto viewport_state =
            vk::PipelineViewportStateCreateInfo {}.setViewports(viewports).setScissors(scissors);

        const auto rasterizer =
            vk::PipelineRasterizationStateCreateInfo {}
                .setDepthClampEnable(state.rasterization_state.depth_clamp_enable)
                .setRasterizerDiscardEnable(state.rasterization_state.rasterizer_discard_enable)
                .setPolygonMode(core::narrow<vk::PolygonMode>(state.rasterization_state.polygon_mode))
                .setCullMode(core::narrow<vk::CullModeFlagBits>(state.rasterization_state.cull_mode))
                .setFrontFace(core::narrow<vk::FrontFace>(state.rasterization_state.front_face))
                .setLineWidth(state.rasterization_state.line_width);

        const auto multisampling =
            vk::PipelineMultisampleStateCreateInfo {}
                .setRasterizationSamples(core::narrow<vk::SampleCountFlagBits>(
                    state.multisample_state.rasterization_samples))
                .setSampleShadingEnable(state.multisample_state.sample_shading_enable);

        const auto blend_attachments =
            state.color_blend_state.attachments |
            std::views::transform([](auto&& attachment) noexcept {
                return vk::PipelineColorBlendAttachmentState {}
                    .setBlendEnable(attachment.blend_enable)
                    .setSrcColorBlendFactor(
                        core::narrow<vk::BlendFactor>(attachment.src_color_blend_factor))
                    .setDstColorBlendFactor(
                        core::narrow<vk::BlendFactor>(attachment.dst_color_blend_factor))
                    .setColorBlendOp(core::narrow<vk::BlendOp>(attachment.color_blend_operation))
                    .setSrcAlphaBlendFactor(
                        core::narrow<vk::BlendFactor>(attachment.src_alpha_blend_factor))
                    .setDstAlphaBlendFactor(
                        core::narrow<vk::BlendFactor>(attachment.dst_alpha_blend_factor))
                    .setAlphaBlendOp(core::narrow<vk::BlendOp>(attachment.alpha_blend_operation))
                    .setColorWriteMask(
                        core::narrow<vk::ColorComponentFlagBits>(attachment.color_write_mask));
            }) |
            std::ranges::to<std::vector>();

        const auto color_blending =
            vk::PipelineColorBlendStateCreateInfo {}
                .setLogicOpEnable(state.color_blend_state.logic_operation_enable)
                .setLogicOp(core::narrow<vk::LogicOp>(state.color_blend_state.logic_operation))
                .setAttachments(blend_attachments)
                .setBlendConstants({ state.color_blend_state.blend_constants[0],
                                     state.color_blend_state.blend_constants[1],
                                     state.color_blend_state.blend_constants[2],
                                     state.color_blend_state.blend_constants[3] });

        const auto states =
            state.dynamic_state.dynamics |
            std::views::transform([](auto&& s) noexcept { return core::narrow<vk::DynamicState>(s); }) |
            std::ranges::to<std::vector>();

        const auto dynamic_state = vk::PipelineDynamicStateCreateInfo {}.setDynamicStates(states);

        const auto shaders = state.shader_state.shaders |
                             std::views::transform([](auto&& shader) noexcept {
                                 static auto NAME = "main";
                                 return vk::PipelineShaderStageCreateInfo {}
                                     .setStage(core::narrow<vk::ShaderStageFlagBits>(shader->type()))
                                     .setModule(toVkHandle()(shader))
                                     .setPName(NAME);
                             }) |
                             std::ranges::to<std::vector>();

        const auto depth_stencil =
            vk::PipelineDepthStencilStateCreateInfo {}
                .setDepthTestEnable(state.depth_stencil_state.depth_test_enable)
                .setDepthWriteEnable(state.depth_stencil_state.depth_write_enable)
                .setDepthCompareOp(
                    core::narrow<vk::CompareOp>(state.depth_stencil_state.depth_compare_op))
                .setDepthBoundsTestEnable(state.depth_stencil_state.depth_bounds_test_enable)
                .setStencilTestEnable(false)
                .setMinDepthBounds(state.depth_stencil_state.min_depth_bounds)
                .setMaxDepthBounds(state.depth_stencil_state.max_depth_bounds);

        const auto create_info = vk::GraphicsPipelineCreateInfo {}
                                     .setStages(shaders)
                                     .setPVertexInputState(&vertex_input_info)
                                     .setPInputAssemblyState(&input_assembly)
                                     .setPViewportState(&viewport_state)
                                     .setPRasterizationState(&rasterizer)
                                     .setPMultisampleState(&multisampling)
                                     .setPDepthStencilState(&depth_stencil)
                                     .setPColorBlendState(&color_blending)
                                     .setPDynamicState(&dynamic_state)
                                     .setLayout(toVkHandle()(layout))
                                     .setRenderPass(toVkHandle()(render_pass))
                                     .setSubpass(0)
                                     .setBasePipelineHandle(nullptr)
                                     .setBasePipelineIndex(-1);

        using OptCache = vk::Optional<const vk::raii::PipelineCache>;

        const auto vk_pipeline_cache =
            core::either(pipeline_cache,
                         core::monadic::map(toRaiiVkHandle(),
                                                            core::monadic::init<OptCache>()),
                         core::monadic::init<OptCache>(nullptr));

        return vkCreate<vk::raii::Pipeline>(toRaiiVkHandle()(device()),
                                            vk_pipeline_cache,
                                            create_info)
            .transform(core::monadic::set(m_vk_pipeline));
    }
} // namespace stormkit::gpu
