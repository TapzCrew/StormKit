// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/Types.hpp>

#include <stormkit/entities/Fwd.hpp>

#include <stormkit/wsi/Fwd.hpp>

#include <stormkit/gpu/Fwd.hpp>
#include <stormkit/gpu/core/CommandBuffer.hpp>
#include <stormkit/gpu/core/Surface.hpp>

#include <stormkit/gpu/resource/ImageView.hpp>

#include <stormkit/engine/EngineObject.hpp>
#include <stormkit/engine/Fwd.hpp>
#include <stormkit/engine/render/framegraph/GraphResource.hpp>

namespace stormkit::engine {
    class STORMKIT_PUBLIC Renderer: public EngineObject {
      public:
        explicit Renderer(Engine &engine);
        Renderer(Engine &engine, const core::ExtentU &extent, gpu::Surface::Buffering buffering);
        ~Renderer();

        Renderer(const Renderer &) = delete;
        auto operator=(const Renderer &) -> Renderer & = delete;

        Renderer(Renderer &&) noexcept;
        auto operator=(Renderer &&) noexcept -> Renderer &;

        auto setBuildFrameGraphCallback(BuildFrameGraphCallback callback) -> void;

        auto instance() const noexcept -> const gpu::Instance &;
        auto device() const noexcept -> const gpu::Device &;
        auto surface() const noexcept -> const gpu::Surface &;

        auto pipelineCache() noexcept -> gpu::PipelineCache &;
        auto pipelineCache() const noexcept -> const gpu::PipelineCache &;

        auto shaderCache() noexcept -> ShaderCache &;
        auto shaderCache() const noexcept -> const ShaderCache &;

      private:
        enum class FrameGraphState { Updated, Old };

        auto threadLoop() -> void;
        auto rebuildFrameGraph() -> void;

        BuildFrameGraphCallback m_build_framegraph;

        gpu::InstanceOwnedPtr m_instance;
        gpu::DeviceOwnedPtr m_device;
        gpu::SurfaceOwnedPtr m_surface;

        std::atomic_bool m_stop_thread = false;
        std::thread m_render_thread;

        std::atomic_bool m_updated = true;
        RenderQueueOwnedPtr m_render_queue;
        FrameGraphBuilderOwnedPtr m_builder;

        gpu::PipelineCacheOwnedPtr m_pipeline_cache;
        ShaderCacheOwnedPtr m_shader_cache;

        std::vector<BakedFrameGraphOwnedPtr> m_framegraphs;
        std::vector<FrameGraphState> m_framegraph_states;
    };
} // namespace stormkit::engine

#include "Renderer.inl"
