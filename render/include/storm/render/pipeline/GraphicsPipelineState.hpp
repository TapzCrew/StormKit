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
#include <storm/render/pipeline/RenderPass.hpp>
#include <storm/render/pipeline/VertexBindingDescription.hpp>
#include <storm/render/pipeline/VertexInputAttributeDescription.hpp>
#include <storm/render/resource/Fwd.hpp>

namespace storm::render {
    struct GraphicsPipelineVertexInputState {
        VertexBindingDescriptionConstSpan binding_descriptions;
        VertexInputAttributeDescriptionConstSpan input_attribute_descriptions;
    };

    struct GraphicsPipelineInputAssemblyState {
        PrimitiveTopology topology    = PrimitiveTopology::Triangle_List;
        bool primitive_restart_enable = false;
    };

    struct GraphicsPipelineViewportState {
        std::vector<Viewport> viewports;
        std::vector<Scissor> scissors;
    };

    struct GraphicsPipelineRasterizationState {
        bool depth_clamp_enable        = false;
        bool rasterizer_discard_enable = false;
        PolygonMode polygon_mode       = PolygonMode::Fill;
        float line_width               = 1;
        CullMode cull_mode             = CullMode::Back;
        FrontFace front_face           = FrontFace::Clockwise;
    };

    struct GraphicsPipelineMultiSampleState {
        bool sample_shading_enable            = false;
        SampleCountFlag rasterization_samples = SampleCountFlag::C1_BIT;
    };

    struct GraphicsPipelineColorBlendAttachmentState {
        ColorComponent color_write_mask = ColorComponent::RGBA;
        bool blend_enable               = false;

        BlendFactor src_color_blend_factor   = BlendFactor::One;
        BlendFactor dst_color_blend_factor   = BlendFactor::Zero;
        BlendOperation color_blend_operation = BlendOperation::Add;

        BlendFactor src_alpha_blend_factor   = BlendFactor::One;
        BlendFactor dst_alpha_blend_factor   = BlendFactor::Zero;
        BlendOperation alpha_blend_operation = BlendOperation::Substract;
    };

    struct GraphicsPipelineColorBlendState {
        bool logic_operation_enable    = false;
        LogicOperation logic_operation = LogicOperation::Copy;
        std::vector<GraphicsPipelineColorBlendAttachmentState> attachments;
        std::array<float, 4> blend_constants = { 0.f, 0.f, 0.f, 0.f };
    };

    struct GraphicsPipelineDynamicState {
        std::vector<DynamicState> dynamics;
    };

    struct GraphicsPipelineShaderState {
        std::vector<ShaderConstPtr> shaders;
    };

    struct GraphicsPipelineDepthStencilState {
        bool depth_test_enable  = false;
        bool depth_write_enable = false;

        CompareOperation depth_compare_op = CompareOperation::Less;

        bool depth_bounds_test_enable = false;

        float min_depth_bounds = 0.f;
        float max_depth_bounds = 1.f;
    };

    struct GraphicsPipelineLayout {
        std::vector<DescriptorSetLayoutConstPtr> descriptor_set_layouts;
        std::vector<PushConstantRange> push_constant_ranges;
    };

    struct GraphicsPipelineState {
        GraphicsPipelineInputAssemblyState input_assembly_state;
        GraphicsPipelineViewportState viewport_state;
        GraphicsPipelineRasterizationState rasterization_state;
        GraphicsPipelineMultiSampleState multisample_state;
        GraphicsPipelineColorBlendState color_blend_state;
        GraphicsPipelineDynamicState dynamic_state;
        GraphicsPipelineShaderState shader_state;
        GraphicsPipelineVertexInputState vertex_input_state;
        GraphicsPipelineDepthStencilState depth_stencil_state;
        GraphicsPipelineLayout layout;
    };

} // namespace storm::render

HASH_FUNC(storm::render::GraphicsPipelineVertexInputState)
HASH_FUNC(storm::render::GraphicsPipelineInputAssemblyState)
HASH_FUNC(storm::render::GraphicsPipelineViewportState)
HASH_FUNC(storm::render::GraphicsPipelineRasterizationState)
HASH_FUNC(storm::render::GraphicsPipelineMultiSampleState)
HASH_FUNC(storm::render::GraphicsPipelineColorBlendAttachmentState)
HASH_FUNC(storm::render::GraphicsPipelineColorBlendState)
HASH_FUNC(storm::render::GraphicsPipelineDynamicState)
HASH_FUNC(storm::render::GraphicsPipelineShaderState)
HASH_FUNC(storm::render::GraphicsPipelineDepthStencilState)
HASH_FUNC(storm::render::GraphicsPipelineLayout)
HASH_FUNC(storm::render::GraphicsPipelineState)
