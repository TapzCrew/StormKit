// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <stormkit/engine/Engine.mpp>
#include <stormkit/engine/render/ForwardTasks.mpp>
#include <stormkit/engine/render/Renderer.mpp>
#include <stormkit/engine/render/core/RenderQueue.mpp>
#include <stormkit/engine/render/core/ShaderCache.mpp>
#include <stormkit/engine/render/core/VertexArray.mpp>
#include <stormkit/engine/render/framegraph/BakedFrameGraph.mpp>
#include <stormkit/engine/render/framegraph/FrameGraphBuilder.mpp>
#include <stormkit/engine/render/framegraph/GraphTaskBuilder.mpp>

#include <stormkit/gpu/core/Device.mpp>
#include <stormkit/gpu/core/Instance.mpp>
#include <stormkit/gpu/core/PhysicalDevice.mpp>
#include <stormkit/gpu/core/PhysicalDeviceInfo.mpp>
#include <stormkit/gpu/core/Queue.mpp>
#include <stormkit/gpu/core/WindowSurface.mpp>
#include <stormkit/gpu/pipeline/GraphicsPipeline.mpp>
#include <stormkit/gpu/pipeline/PipelineCache.mpp>

#include "shaders/ForwardShaders.hpp"

namespace stormkit::engine {

    /////////////////////////////////////
    /////////////////////////////////////
    auto forwardRenderer(Engine &engine) noexcept -> BuildFrameGraphCallback {
        return [&](auto &builder) -> void {
            auto &renderer = engine.renderer();

            auto *shader_cache = &renderer.shaderCache();

            if (!shader_cache->has(FORWARD_PASS_VERTEX_SHADER)) [[unlikely]]
                shader_cache->load(FORWARD_PASS_VERTEX_SHADER,
                                   VERTEX_SHADER_DATA,
                                   gpu::ShaderStageFlag::Vertex);

            if (!shader_cache->has(FORWARD_PASS_FRAGMENT_SHADER)) [[unlikely]]
                shader_cache->load(FORWARD_PASS_FRAGMENT_SHADER,
                                   FRAGMENT_SHADER_DATA,
                                   gpu::ShaderStageFlag::Vertex);

            const auto &extent = engine.window().size();
            struct ForwardPass {
                engine::GraphImage *output;
            };

            auto *pipeline_cache = &renderer.pipelineCache();

            const auto &task = builder.template addTask<ForwardPass>(
                "StormKit:ForwardPass",
                [&](auto &data, auto &builder) {
                    data.output = &builder.create("Backbuffer",
                                                  engine::ImageDescription {
                                                      .extent = { extent.width, extent.height },
                                                      .type   = gpu::ImageType::T2D,
                                                      .format = gpu::PixelFormat::RGBA8_UNorm });
                },
                [=](const auto &data, auto *renderpass, auto &cmb) {
                    const auto &vertex_shader   = shader_cache->get(FORWARD_PASS_VERTEX_SHADER);
                    const auto &fragment_shader = shader_cache->get(FORWARD_PASS_FRAGMENT_SHADER);

                    const auto state = gpu::GraphicsPipelineState {
                        .viewport_state = { .viewports = { gpu::Viewport {
                                                .position = { 0.f, 0.f },
                                                .extent   = extent,
                                                .depth    = { 0.f, 1.f } } },
                                            .scissors  = { gpu::Scissor { .offset = { 0, 0 },
                                                                          .extent = extent } } },

                        .color_blend_state   = { .attachments = { {} } },
                        .dynamic_state       = { { gpu::DynamicState::Viewport,
                                             gpu::DynamicState::Scissor } },
                        .shader_state        = { .shaders =
                                              core::makeConstObserverArray(vertex_shader,
                                                                           fragment_shader) },
                        .vertex_input_state  = { .binding_descriptions =
                                                    std::vector<gpu::VertexBindingDescription> {
                                                        std::begin(
                                                            VertexArray::bindingDescriptions()),
                                                        std::end(
                                                            VertexArray::bindingDescriptions()) },
                                                 .input_attribute_descriptions =
                                                    std::vector<
                                                        gpu::VertexInputAttributeDescription> {
                                                        std::begin(
                                                            VertexArray::attributesDescriptions()),
                                                        std::end(VertexArray::
                                                                     attributesDescriptions()) } },
                        .depth_stencil_state = { .depth_test_enable  = true,
                                                 .depth_write_enable = true,
                                                 .depth_compare_op =
                                                     gpu::CompareOperation::Greater },
                    };

                    auto &pipeline = pipeline_cache->getPipeline(state, *renderpass);

                    cmb.bindGraphicsPipeline(pipeline);
                },
                engine::GraphTaskBase::Type::Graphics,
                true);

            builder.setFinalResource(task.data().output->id());
        };
    }
} // namespace stormkit::engine
