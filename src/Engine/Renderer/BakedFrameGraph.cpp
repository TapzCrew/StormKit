// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module stormkit.Engine;

import std;

import stormkit.Core;
import stormkit.Gpu;

import :Renderer.FrameGraphBuilder;
import :Renderer.BakedFrameGraph;

namespace stormkit::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    BakedFrameGraph::BakedFrameGraph(const FrameGraphBuilder&          builder,
                                     Data&&                            data,
                                     [[maybe_unused]] BakedFrameGraph *old)
        : m_builder { &builder }, m_data { std::move(data) } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto BakedFrameGraph::execute(Frame& frame) -> void {
        core::expects(m_backbuffer);

        // {
        //     auto signal = core::makeConstObserverStaticArray(m_semaphore);
        //
        //     m_main_cmb->submit({}, signal);
        // }
        //
        // {
        //     auto wait   = core::makeConstObserverStaticArray(m_semaphore, frame.image_available);
        //     auto signal = core::makeConstObserverStaticArray(frame.render_finished);
        //
        //     m_blit_cmb->submit(wait, signal, frame.in_flight);
        // }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto BakedFrameGraph::setBackbuffer(gpu::ImageView& backbuffer) -> void {
        // if (&backbuffer != m_backbuffer) {
        //     m_backbuffer = &backbuffer;
        //
        //     m_blit_cmb->reset();
        //     m_blit_cmb->begin();
        //     // m_blit_cmb->blitImage( , backbuffer);
        //     m_blit_cmb->end();
        // }
    }
} // namespace stormkit::engine
