// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module stormkit.Engine;

import std;

import stormkit.Core;
import stormkit.Gpu;

import :Renderer.FrameGraph;
import :Renderer.RenderSurface;

namespace stormkit::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    BakedFrameGraph::BakedFrameGraph(const gpu::Image&                 backbuffer,
                                     Data&&                            data,
                                     [[maybe_unused]] BakedFrameGraph* old)
        : m_backbuffer { backbuffer }, m_data { std::move(data) } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto BakedFrameGraph::execute(const gpu::Queue& queue, const RenderSurface::Frame& frame) -> const gpu::Semaphore& {
        auto signal = core::makeNakedRefs<std::array>(*m_data.semaphore);
        m_data.cmb->submit(queue, {}, signal);

        return *m_data.semaphore;
    }
} // namespace stormkit::engine
