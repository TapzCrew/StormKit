// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <storm/render/core/Device.hpp>
#include <storm/render/core/Enums.hpp>

#include <storm/render/pipeline/DescriptorSetLayout.hpp>
#include <storm/render/pipeline/GraphicsPipeline.hpp>
#include <storm/render/pipeline/PipelineCache.hpp>
#include <storm/render/pipeline/RenderPass.hpp>

#include <storm/render/resource/Shader.hpp>

using namespace storm;
using namespace storm::render;

/////////////////////////////////////
/////////////////////////////////////
GraphicsPipeline::GraphicsPipeline(const render::Device &device,
                                   PipelineCacheConstPtr cache) noexcept
    : AbstractPipeline { device, cache } {
}

/////////////////////////////////////
/////////////////////////////////////
GraphicsPipeline::~GraphicsPipeline() = default;

/////////////////////////////////////
/////////////////////////////////////
GraphicsPipeline::GraphicsPipeline(GraphicsPipeline &&) = default;

/////////////////////////////////////
/////////////////////////////////////
GraphicsPipeline &GraphicsPipeline::operator=(GraphicsPipeline &&) = default;

/////////////////////////////////////
/////////////////////////////////////
void GraphicsPipeline::build() {
    STORMKIT_EXPECTS(m_is_builded == false);
    STORMKIT_EXPECTS(m_render_pass != nullptr);

    const auto &device      = static_cast<const Device &>(*m_device);
    const auto &render_pass = static_cast<const RenderPass &>(*m_render_pass);

    auto binding_descriptions = std::vector<vk::VertexInputBindingDescription> {};
    binding_descriptions.reserve(std::size(m_state.vertex_input_state.binding_descriptions));
    for (const auto &binding : m_state.vertex_input_state.binding_descriptions) {
        const auto binding_desc = vk::VertexInputBindingDescription {}
                                      .setBinding(binding.binding)
                                      .setStride(gsl::narrow_cast<core::Int32>(binding.stride))
                                      .setInputRate(toVK(binding.input_rate));
        binding_descriptions.emplace_back(std::move(binding_desc));
    }

    auto attribute_descriptions = std::vector<vk::VertexInputAttributeDescription> {};
    attribute_descriptions.reserve(
        std::size(m_state.vertex_input_state.input_attribute_descriptions));
    for (const auto &attribute : m_state.vertex_input_state.input_attribute_descriptions) {
        const auto attribute_desc = vk::VertexInputAttributeDescription {}
                                        .setLocation(attribute.location)
                                        .setBinding(attribute.binding)
                                        .setFormat(toVK(attribute.format))
                                        .setOffset(gsl::narrow_cast<core::Int32>(attribute.offset));

        attribute_descriptions.emplace_back(std::move(attribute_desc));
    }

    const auto vertex_input_info =
        vk::PipelineVertexInputStateCreateInfo {}
            .setVertexBindingDescriptionCount(
                gsl::narrow_cast<core::Int32>(std::size(binding_descriptions)))
            .setPVertexBindingDescriptions(std::data(binding_descriptions))
            .setVertexAttributeDescriptionCount(
                gsl::narrow_cast<core::Int32>(std::size(attribute_descriptions)))
            .setPVertexAttributeDescriptions(std::data(attribute_descriptions));

    const auto input_assembly =
        vk::PipelineInputAssemblyStateCreateInfo {}
            .setTopology(toVK(m_state.input_assembly_state.topology))
            .setPrimitiveRestartEnable(m_state.input_assembly_state.primitive_restart_enable);

    auto viewports = std::vector<vk::Viewport> {};
    viewports.reserve(std::size(m_state.viewport_state.viewports));

    for (const auto &viewport : m_state.viewport_state.viewports) {
        const auto vk_viewport = vk::Viewport {}
                                     .setX(viewport.position.x)
                                     .setY(viewport.position.y)
                                     .setWidth(viewport.extent.width)
                                     .setHeight(viewport.extent.height)
                                     .setMinDepth(viewport.depth.x)
                                     .setMaxDepth(viewport.depth.y);

        viewports.emplace_back(std::move(vk_viewport));
    }

    auto scissors = std::vector<vk::Rect2D> {};
    viewports.reserve(std::size(m_state.viewport_state.scissors));

    for (const auto &scissor : m_state.viewport_state.scissors) {
        const auto vk_scissor =
            vk::Rect2D {}
                .setOffset(vk::Offset2D { scissor.offset.x, scissor.offset.y })
                .setExtent(vk::Extent2D { scissor.extent.width, scissor.extent.height });

        scissors.emplace_back(std::move(vk_scissor));
    }

    const auto viewport_state =
        vk::PipelineViewportStateCreateInfo {}
            .setViewportCount(gsl::narrow_cast<core::Int32>(std::size(viewports)))
            .setPViewports(std::data(viewports))
            .setScissorCount(gsl::narrow_cast<core::Int32>(std::size(scissors)))
            .setPScissors(std::data(scissors));

    const auto rasterizer =
        vk::PipelineRasterizationStateCreateInfo {}
            .setDepthClampEnable(m_state.rasterization_state.depth_clamp_enable)
            .setRasterizerDiscardEnable(m_state.rasterization_state.rasterizer_discard_enable)
            .setPolygonMode(toVK(m_state.rasterization_state.polygon_mode))
            .setCullMode(toVK(m_state.rasterization_state.cull_mode))
            .setFrontFace(toVK(m_state.rasterization_state.front_face))
            .setDepthBiasEnable(false)
            .setLineWidth(m_state.rasterization_state.line_width);

    const auto multisampling =
        vk::PipelineMultisampleStateCreateInfo {}
            .setRasterizationSamples(toVKBits(m_state.multisample_state.rasterization_samples))
            .setSampleShadingEnable(m_state.multisample_state.sample_shading_enable);

    auto blend_attachments = std::vector<vk::PipelineColorBlendAttachmentState> {};
    blend_attachments.reserve(std::size(m_state.color_blend_state.attachments));

    for (const auto &attachment : m_state.color_blend_state.attachments) {
        const auto vk_attachment =
            vk::PipelineColorBlendAttachmentState {}
                .setBlendEnable(attachment.blend_enable)
                .setSrcColorBlendFactor(toVK(attachment.src_color_blend_factor))
                .setDstColorBlendFactor(toVK(attachment.dst_color_blend_factor))
                .setColorBlendOp(toVK(attachment.color_blend_operation))
                .setSrcAlphaBlendFactor(toVK(attachment.src_alpha_blend_factor))
                .setDstAlphaBlendFactor(toVK(attachment.dst_alpha_blend_factor))
                .setAlphaBlendOp(toVK(attachment.alpha_blend_operation))
                .setColorWriteMask(toVK(attachment.color_write_mask));

        blend_attachments.emplace_back(std::move(vk_attachment));
    }

    const auto color_blending =
        vk::PipelineColorBlendStateCreateInfo {}
            .setLogicOpEnable(m_state.color_blend_state.logic_operation_enable)
            .setLogicOp(toVK(m_state.color_blend_state.logic_operation))
            .setAttachmentCount(gsl::narrow_cast<core::Int32>(std::size(blend_attachments)))
            .setPAttachments(std::data(blend_attachments))
            .setBlendConstants(m_state.color_blend_state.blend_constants);

    auto states = std::vector<vk::DynamicState> {};
    states.reserve(std::size(m_state.dynamic_state.dynamics));

    for (auto state : m_state.dynamic_state.dynamics) { states.emplace_back(toVK(state)); }

    const auto dynamic_state = vk::PipelineDynamicStateCreateInfo {}
                                   .setDynamicStateCount(std::size(states))
                                   .setPDynamicStates(std::data(states));

    auto shaders = std::vector<vk::PipelineShaderStageCreateInfo> {};
    shaders.reserve(std::size(m_state.shader_state.shaders));

    for (const auto &_shader : m_state.shader_state.shaders) {
        const auto &shader = *_shader;

        const auto vk_shader = vk::PipelineShaderStageCreateInfo {}
                                   .setStage(toVKBits(shader.type()))
                                   .setModule(shader)
                                   .setPName("main");

        shaders.emplace_back(std::move(vk_shader));
    }

    const auto depth_stencil =
        vk::PipelineDepthStencilStateCreateInfo {}
            .setDepthTestEnable(m_state.depth_stencil_state.depth_test_enable)
            .setDepthWriteEnable(m_state.depth_stencil_state.depth_write_enable)
            .setDepthCompareOp(toVK(m_state.depth_stencil_state.depth_compare_op))
            .setDepthBoundsTestEnable(m_state.depth_stencil_state.depth_bounds_test_enable)
            .setStencilTestEnable(false)
            .setMinDepthBounds(m_state.depth_stencil_state.min_depth_bounds)
            .setMaxDepthBounds(m_state.depth_stencil_state.max_depth_bounds);

    auto set_layout = std::vector<vk::DescriptorSetLayout> {};
    set_layout.reserve(std::size(m_state.layout.descriptor_set_layouts));
    for (const auto &layout : m_state.layout.descriptor_set_layouts)
        set_layout.emplace_back(*layout);

    auto push_constant_ranges = std::vector<vk::PushConstantRange> {};
    push_constant_ranges.reserve(std::size(m_state.layout.push_constant_ranges));
    for (const auto &push_constant_range : m_state.layout.push_constant_ranges) {
        const auto vk_push_constant_range = vk::PushConstantRange {}
                                                .setStageFlags(toVK(push_constant_range.stages))
                                                .setOffset(push_constant_range.offset)
                                                .setSize(push_constant_range.size);

        push_constant_ranges.emplace_back(std::move(vk_push_constant_range));
    }

    const auto pipeline_layout_create_info =
        vk::PipelineLayoutCreateInfo {}
            .setSetLayoutCount(gsl::narrow_cast<core::Int32>(std::size(set_layout)))
            .setPSetLayouts(std::data(set_layout))
            .setPushConstantRangeCount(
                gsl::narrow_cast<core::Int32>(std::size(push_constant_ranges)))
            .setPPushConstantRanges(std::data(push_constant_ranges));

    m_vk_pipeline_layout = m_device->createVkPipelineLayout(pipeline_layout_create_info);

    const auto create_info = vk::GraphicsPipelineCreateInfo {}
                                 .setStageCount(gsl::narrow_cast<core::Int32>(std::size(shaders)))
                                 .setPStages(std::data(shaders))
                                 .setPVertexInputState(&vertex_input_info)
                                 .setPInputAssemblyState(&input_assembly)
                                 .setPViewportState(&viewport_state)
                                 .setPRasterizationState(&rasterizer)
                                 .setPMultisampleState(&multisampling)
                                 .setPDepthStencilState(&depth_stencil)
                                 .setPColorBlendState(&color_blending)
                                 .setPDynamicState(&dynamic_state)
                                 .setLayout(*m_vk_pipeline_layout)
                                 .setRenderPass(render_pass)
                                 .setSubpass(0)
                                 .setBasePipelineHandle(VK_NULL_HANDLE)
                                 .setBasePipelineIndex(-1);

    if (m_pipeline_cache)
        m_vk_pipeline = device.createVkGraphicsPipeline(create_info, m_pipeline_cache->vkHandle());
    else
        m_vk_pipeline = device.createVkGraphicsPipeline(create_info);

    m_is_builded = true;
}
