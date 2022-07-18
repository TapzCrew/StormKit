// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <stormkit/gpu/core/Device.hpp>

#include <stormkit/gpu/resource/Shader.hpp>

#include <stormkit/gpu/pipeline/ComputePipeline.hpp>

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    GraphicsPipeline::GraphicsPipeline(const gpu::Device &device,
                                       const PipelineCache *cache) noexcept
        : AbstractPipeline { device, cache } {}

    /////////////////////////////////////
    /////////////////////////////////////
    GraphicsPipeline::~GraphicsPipeline() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    GraphicsPipeline::GraphicsPipeline(GraphicsPipeline &&other) noexcept
        : AbstractPipeline { std::move(other) }, m_state { std::exchange(other.m_state, {}) },
          m_render_pass { std::exchange(other.m_render_pass, {}) } {}

    /////////////////////////////////////
    /////////////////////////////////////
    auto GraphicsPipeline::operator=(GraphicsPipeline &&other) noexcept -> GraphicsPipeline & {
        if (&other == this) [[unlikely]]
            return *this;

        AbstractPipeline::operator=(std::move(other));

        m_state       = std::exchange(other.m_state, {});
        m_render_pass = std::exchange(other.m_render_pass, {});

        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto GraphicsPipeline::bake() -> void {
        STORMKIT_EXPECTS(m_is_baked == false);
        STORMKIT_EXPECTS(m_render_pass != nullptr);

        const auto &vk = device().table();

        const auto binding_descriptions =
            core::transform(m_state.vertex_input_state.binding_descriptions, [](const auto &b) {
                return VkVertexInputBindingDescription { .binding   = b.binding,
                                                         .stride    = b.stride,
                                                         .inputRate = core::as<VkVertexInputRate>(
                                                             b.input_rate) };
            });

        auto attribute_descriptions =
            core::transform(m_state.vertex_input_state.input_attribute_descriptions,
                            [](const auto &a) {
                                return VkVertexInputAttributeDescription {
                                    .location = a.location,
                                    .binding  = a.binding,
                                    .format   = core::as<VkFormat>(a.format),
                                    .offset   = a.offset
                                };
                            });

        const auto vertex_input_info = VkPipelineVertexInputStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .vertexBindingDescriptionCount =
                core::as<core::UInt32>(std::size(binding_descriptions)),
            .pVertexBindingDescriptions = std::data(binding_descriptions),
            .vertexAttributeDescriptionCount =
                core::as<core::UInt32>(std::size(attribute_descriptions)),
            .pVertexAttributeDescriptions = std::data(attribute_descriptions)

        };

        const auto input_assembly = VkPipelineInputAssemblyStateCreateInfo {
            .sType    = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .topology = core::as<VkPrimitiveTopology>(m_state.input_assembly_state.topology),
            .primitiveRestartEnable = m_state.input_assembly_state.primitive_restart_enable
        };

        const auto viewports = core::transform(m_state.viewport_state.viewports, [](const auto &v) {
            return VkViewport {
                .x        = v.position.x,
                .y        = v.position.y,
                .width    = v.extent.width,
                .height   = v.extent.height,
                .minDepth = v.depth.x,
                .maxDepth = v.depth.y,
            };
        });

        const auto scissors = core::transform(m_state.viewport_state.scissors, [](const auto &s) {
            return VkRect2D { .offset = VkOffset2D { s.offset.x, s.offset.y },
                              .extent = VkExtent2D { s.extent.width, s.extent.height } };
        });

        const auto viewport_state = VkPipelineViewportStateCreateInfo {
            .sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .viewportCount = core::as<core::UInt32>(std::size(viewports)),
            .pViewports    = std::data(viewports),
            .scissorCount  = core::as<core::UInt32>(std::size(scissors)),
            .pScissors     = std::data(scissors)
        };

        const auto rasterizer = VkPipelineRasterizationStateCreateInfo {
            .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable        = m_state.rasterization_state.depth_clamp_enable,
            .rasterizerDiscardEnable = m_state.rasterization_state.rasterizer_discard_enable,
            .polygonMode = core::as<VkPolygonMode>(m_state.rasterization_state.polygon_mode),
            .cullMode    = core::as<VkCullModeFlags>(m_state.rasterization_state.cull_mode),
            .frontFace   = core::as<VkFrontFace>(m_state.rasterization_state.front_face),
            .lineWidth   = m_state.rasterization_state.line_width,
        };

        const auto multisampling = VkPipelineMultisampleStateCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .rasterizationSamples =
                core::as<VkSampleCountFlagBits>(m_state.multisample_state.rasterization_samples),
            .sampleShadingEnable = m_state.multisample_state.sample_shading_enable
        };

        auto blend_attachments =
            core::transform(m_state.color_blend_state.attachments, [](const auto &a) {
                return VkPipelineColorBlendAttachmentState {
                    .blendEnable         = a.blend_enable,
                    .srcColorBlendFactor = core::as<VkBlendFactor>(a.src_color_blend_factor),
                    .dstColorBlendFactor = core::as<VkBlendFactor>(a.dst_color_blend_factor),
                    .colorBlendOp        = core::as<VkBlendOp>(a.color_blend_operation),
                    .srcAlphaBlendFactor = core::as<VkBlendFactor>(a.src_alpha_blend_factor),
                    .dstAlphaBlendFactor = core::as<VkBlendFactor>(a.dst_alpha_blend_factor),
                    .alphaBlendOp        = core::as<VkBlendOp>(a.alpha_blend_operation),
                    .colorWriteMask      = core::as<VkColorComponentFlags>(a.color_write_mask)
                };
            });

        const auto color_blending = VkPipelineColorBlendStateCreateInfo {
            .sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .logicOpEnable   = m_state.color_blend_state.logic_operation_enable,
            .logicOp         = core::as<VkLogicOp>(m_state.color_blend_state.logic_operation),
            .attachmentCount = core::as<core::UInt32>(std::size(blend_attachments)),
            .pAttachments    = std::data(blend_attachments),
            .blendConstants  = { m_state.color_blend_state.blend_constants[0],
                                m_state.color_blend_state.blend_constants[1],
                                m_state.color_blend_state.blend_constants[2],
                                m_state.color_blend_state.blend_constants[3] }
        };

        const auto states = core::transform(m_state.dynamic_state.dynamics, [](const auto &s) {
            return core::as<VkDynamicState>(s);
        });

        const auto dynamic_state = VkPipelineDynamicStateCreateInfo {
            .sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .dynamicStateCount = core::as<core::UInt32>(std::size(states)),
            .pDynamicStates    = std::data(states)
        };

        const auto shaders = core::transform(m_state.shader_state.shaders, [](const auto &s) {
            return VkPipelineShaderStageCreateInfo {
                .sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage  = core::as<VkShaderStageFlagBits>(s->type()),
                .module = s->vkHandle(),
                .pName  = "main"
            };
        });

        const auto depth_stencil = VkPipelineDepthStencilStateCreateInfo {
            .sType            = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .depthTestEnable  = m_state.depth_stencil_state.depth_test_enable,
            .depthWriteEnable = m_state.depth_stencil_state.depth_write_enable,
            .depthCompareOp   = core::as<VkCompareOp>(m_state.depth_stencil_state.depth_compare_op),
            .depthBoundsTestEnable = m_state.depth_stencil_state.depth_bounds_test_enable,
            .stencilTestEnable     = false,
            .minDepthBounds        = m_state.depth_stencil_state.min_depth_bounds,
            .maxDepthBounds        = m_state.depth_stencil_state.max_depth_bounds
        };

        const auto set_layout = core::transform(m_state.layout.descriptor_set_layouts,
                                                [](const auto &l) { return l->vkHandle(); });

        const auto push_constant_ranges =
            core::transform(m_state.layout.push_constant_ranges, [](const auto &r) {
                return VkPushConstantRange { .stageFlags = core::as<VkShaderStageFlags>(r.stages),
                                             .offset     = r.offset,
                                             .size       = core::as<core::UInt32>(r.size) };
            });

        const auto pipeline_layout_create_info = VkPipelineLayoutCreateInfo {
            .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .setLayoutCount         = core::as<core::UInt32>(std::size(set_layout)),
            .pSetLayouts            = std::data(set_layout),
            .pushConstantRangeCount = core::as<core::UInt32>(std::size(push_constant_ranges)),
            .pPushConstantRanges    = std::data(push_constant_ranges)
        };

        CHECK_VK_ERROR(vk.vkCreatePipelineLayout(device(),
                                                 &pipeline_layout_create_info,
                                                 nullptr,
                                                 &m_pipeline_layout));

        const auto create_info =
            VkGraphicsPipelineCreateInfo { .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
                                           .stageCount = core::as<core::UInt32>(std::size(shaders)),
                                           .pStages    = std::data(shaders),
                                           .pVertexInputState   = &vertex_input_info,
                                           .pInputAssemblyState = &input_assembly,
                                           .pViewportState      = &viewport_state,
                                           .pRasterizationState = &rasterizer,
                                           .pMultisampleState   = &multisampling,
                                           .pDepthStencilState  = &depth_stencil,
                                           .pColorBlendState    = &color_blending,
                                           .pDynamicState       = &dynamic_state,
                                           .layout              = m_pipeline_layout,
                                           .renderPass          = *m_render_pass,
                                           .subpass             = 0,
                                           .basePipelineHandle  = VK_NULL_HANDLE,
                                           .basePipelineIndex   = -1 };

        const auto pipeline_cache =
            (m_pipeline_cache) ? m_pipeline_cache->vkHandle() : VkPipelineCache { VK_NULL_HANDLE };

        CHECK_VK_ERROR(vk.vkCreateGraphicsPipelines(device(),
                                                    pipeline_cache,
                                                    1,
                                                    &create_info,
                                                    nullptr,
                                                    &m_pipeline));
        m_is_baked = true;
    }
} // namespace stormkit::gpu
