// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/Types.mpp>

#include <stormkit/gpu/Fwd.hpp>
#include <stormkit/gpu/core/Surface.mpp>

#include <stormkit/engine/EngineObject.mpp>
#include <stormkit/engine/Fwd.hpp>

namespace stormkit::engine {
    class STORMKIT_PUBLIC BakedFrameGraph: public EngineObject {
        struct Data {
            struct Task {
                GraphID id;

                gpu::CommandBufferOwnedPtr commandbuffer;

                std::vector<gpu::ClearValue> clear_values = {};
                gpu::RenderPassOwnedPtr renderpass        = nullptr;
                gpu::FramebufferOwnedPtr framebuffer      = nullptr;
            };

            std::vector<Task> tasks;
            std::vector<gpu::Image> images          = {};
            std::vector<gpu::ImageView> image_views = {};
            std::vector<gpu::Buffer> buffers        = {};
        };

      public:
        BakedFrameGraph(Engine &engine,
                        const FrameGraphBuilder &builder,
                        Data &&data,
                        BakedFrameGraph *old = nullptr);
        ~BakedFrameGraph();

        BakedFrameGraph(const BakedFrameGraph &) = delete;
        auto operator=(const BakedFrameGraph &) -> BakedFrameGraph & = delete;

        BakedFrameGraph(BakedFrameGraph &&) noexcept;
        auto operator=(BakedFrameGraph &&) noexcept -> BakedFrameGraph &;

        auto execute(gpu::Surface::Frame &frame) -> void;

        auto setBackbuffer(gpu::Image &backbuffer) -> void;

      private:
        const FrameGraphBuilder *m_builder = nullptr;

        gpu::CommandBufferOwnedPtr m_main_cmb;
        gpu::CommandBufferOwnedPtr m_blit_cmb;

        gpu::SemaphoreOwnedPtr m_semaphore;
        gpu::Image *m_backbuffer = nullptr;
        gpu::ImageViewOwnedPtr m_backbuffer_view;

        Data m_data;

        friend class FrameGraphBuilder;
    };
} // namespace stormkit::engine

#include "BakedFrameGraph.inl"
