// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/gpu/core/CommandBuffer.hpp>
#include <stormkit/gpu/core/Device.hpp>
#include <stormkit/gpu/core/Queue.hpp>
#include <stormkit/gpu/sync/Semaphore.hpp>

module stormkit.engine.render.framegraph.BakedFrameGraph;

import stormkit.Engine;
import stormkit.engine.render.Renderer;

namespace stormkit::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    BakedFrameGraph::BakedFrameGraph(Engine& engine,
                                     const FrameGraphBuilder& builder,
                                     Data&& data,
                                     [[maybe_unused]] BakedFrameGraph *old)
        : EngineObject { engine }, m_builder { &builder }, m_data { std::move(data) } {
        const auto& device = this->engine().renderer().device();

        m_semaphore = device.allocateSemaphore();

        const auto& queue = device.graphicsQueue();

        m_main_cmb = queue.allocateCommandBuffer();
        m_blit_cmb = queue.allocateCommandBuffer();

        m_main_cmb->begin();

        for (const auto& task : m_data.tasks) {
            if (task.renderpass)
                m_main_cmb->beginRenderPass(*task.renderpass,
                                            *task.framebuffer,
                                            task.clear_values,
                                            true);

            auto command_buffers = core::makeConstObserverStaticArray(task.commandbuffer);
            m_main_cmb->executeSubCommandBuffers(command_buffers);

            if (task.renderpass) m_main_cmb->endRenderPass();
        }

        m_main_cmb->end();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    BakedFrameGraph::~BakedFrameGraph() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    BakedFrameGraph::BakedFrameGraph(BakedFrameGraph&& other) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto BakedFrameGraph::operator=(BakedFrameGraph&& other) noexcept -> BakedFrameGraph& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto BakedFrameGraph::execute(gpu::Surface::Frame& frame) -> void {
        core::expects(m_backbuffer);
        core::expects(m_backbuffer_view);

        {
            auto signal = core::makeConstObserverStaticArray(m_semaphore);

            m_main_cmb->submit({}, signal);
        }

        {
            auto wait   = core::makeConstObserverStaticArray(m_semaphore, frame.image_available);
            auto signal = core::makeConstObserverStaticArray(frame.render_finished);

            m_blit_cmb->submit(wait, signal, frame.in_flight);
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto BakedFrameGraph::setBackbuffer(gpu::Image& backbuffer) -> void {
        if (&backbuffer != m_backbuffer) {
            m_backbuffer = &backbuffer;

            m_backbuffer_view = m_backbuffer->allocateView();

            m_blit_cmb->reset();
            m_blit_cmb->begin();
            // m_blit_cmb->blitImage( , backbuffer);
            m_blit_cmb->end();
        }
    }
} // namespace stormkit::engine
