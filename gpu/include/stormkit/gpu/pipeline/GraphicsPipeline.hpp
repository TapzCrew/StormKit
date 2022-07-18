// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/PlatformMacro.hpp>

#define STATE(TYPE, NAME, MEMBER)              \
    auto NAME() noexcept->TYPE & {             \
        STORMKIT_EXPECTS(m_is_baked == false); \
        return m_state.MEMBER;                 \
    }                                          \
    [[nodiscard]] auto NAME() const noexcept->const TYPE & { return m_state.MEMBER; }

#include <stormkit/core/Memory.hpp>
#include <stormkit/core/Types.hpp>

#include <stormkit/gpu/core/Fwd.hpp>
#include <stormkit/gpu/core/Types.hpp>
#include <stormkit/gpu/core/Vulkan.hpp>

#include <stormkit/gpu/pipeline/AbstractPipeline.hpp>
#include <stormkit/gpu/pipeline/GraphicsPipelineState.hpp>

#include <stormkit/gpu/pipeline/Fwd.hpp>

namespace stormkit::gpu {
    class STORMKIT_PUBLIC GraphicsPipeline final: public AbstractPipeline {
      public:
        explicit GraphicsPipeline(const Device &device,
                                  const PipelineCache *cache = nullptr) noexcept;

        ~GraphicsPipeline() override;

        GraphicsPipeline(const GraphicsPipeline &) = delete;
        auto operator=(const GraphicsPipeline &) -> GraphicsPipeline & = delete;

        GraphicsPipeline(GraphicsPipeline &&) noexcept;
        auto operator=(GraphicsPipeline &&) noexcept -> GraphicsPipeline &;

        STATE(GraphicsPipelineVertexInputState, vertexInputState, vertex_input_state)
        STATE(GraphicsPipelineInputAssemblyState, inputAssemblyState, input_assembly_state)
        STATE(GraphicsPipelineViewportState, viewportState, viewport_state)
        STATE(GraphicsPipelineRasterizationState, rasterizationState, rasterization_state)
        STATE(GraphicsPipelineMultiSampleState, multisampleState, multisample_state)
        STATE(GraphicsPipelineColorBlendState, colorBlendState, color_blend_state)
        STATE(GraphicsPipelineDynamicState, dynamicState, dynamic_state)
        STATE(GraphicsPipelineShaderState, shaderState, shader_state)
        STATE(GraphicsPipelineDepthStencilState, depthStencilState, depth_stencil_state)
        STATE(GraphicsPipelineLayout, layout, layout)

        auto bake() -> void override;

        auto setRenderPass(const RenderPass &render_pass) noexcept -> void;
        auto renderPass() const noexcept -> const RenderPass &;

        auto setState(const GraphicsPipelineState &state) noexcept -> void;
        [[nodiscard]] auto state() noexcept -> GraphicsPipelineState &;
        [[nodiscard]] auto state() const noexcept -> const GraphicsPipelineState &;

      private:
        GraphicsPipelineState m_state = {};

        const RenderPass *m_render_pass = nullptr;
    };
} // namespace stormkit::gpu

#undef STATE

#include "GraphicsPipeline.inl"
