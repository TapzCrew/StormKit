// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <stormkit/core/ThreadUtils.mpp>

#include <stormkit/log/LogHandler.mpp>
#include <stormkit/log/LogMacro.hpp>

#include <stormkit/entities/EntityManager.mpp>
#include <stormkit/entities/System.mpp>

#include <stormkit/engine/Engine.mpp>
#include <stormkit/engine/render/Renderer.mpp>
#include <stormkit/engine/render/core/RenderQueue.mpp>
#include <stormkit/engine/render/framegraphs/BakedFrameGraph.mpp>
#include <stormkit/engine/render/framegraphs/FrameGraphBuilder.mpp>

#include <stormkit/gpu/core/Device.mpp>
#include <stormkit/gpu/core/Instance.mpp>
#include <stormkit/gpu/core/PhysicalDevice.mpp>
#include <stormkit/gpu/core/PhysicalDeviceInfo.mpp>
#include <stormkit/gpu/core/Queue.mpp>
#include <stormkit/gpu/core/WindowSurface.mpp>

namespace stormkit::engine {
    LOGGER("StormKit.Renderer.Renderer");

    class RendererSyncSystem: public entities::System {
      public:
        RendererSyncSystem(RenderQueue &queue, entities::EntityManager &manager)
            : System { manager, 0, {} } {}

        RendererSyncSystem(const RendererSyncSystem &) = delete;
        auto operator=(const RendererSyncSystem &) -> RendererSyncSystem & = delete;

        RendererSyncSystem(RendererSyncSystem &&) noexcept = default;
        auto operator=(RendererSyncSystem &&) noexcept -> RendererSyncSystem & = default;

        auto update(core::Secondf delta) -> void override {};

      private:
        auto onMessageReceived(const entities::Message &message) -> void override {}

        RenderQueue *m_render_queue = nullptr;
    };

    /////////////////////////////////////
    /////////////////////////////////////
    Renderer::Renderer(Engine &engine) : m_engine { &engine } {
        m_instance = std::make_unique<gpu::Instance>();
        ilog("Render backend successfully initialized");
        ilog("Using StormKit {}.{}.{} {} {}, built with {}",
             core::STORMKIT_MAJOR_VERSION,
             core::STORMKIT_MINOR_VERSION,
             core::STORMKIT_PATCH_VERSION,
             core::STORMKIT_GIT_BRANCH,
             core::STORMKIT_GIT_COMMIT_HASH,
             STORMKIT_COMPILER);

        ilog("--------- Physical Devices ----------");
        for (const auto &device : m_instance->physicalDevices()) ilog("{}", device.info());

        auto &window = m_engine->window();

        auto surface = m_instance->allocateWindowSurface(window);

        const auto &physical_device = m_instance->pickPhysicalDevice(*surface);

        m_surface = std::move(surface);

        const auto &physical_device_info = physical_device.info();

        ilog("Using physical device {} ({:#06x})",
             physical_device_info.device_name,
             physical_device_info.device_id);

        m_device = physical_device.allocateLogicalDevice();
        m_surface->initialize(*m_device);

        m_render_queue = std::make_unique<RenderQueue>();

        reallocateViewDependentResources();

        m_builder = std::make_unique<FrameGraphBuilder>();

        auto &world = m_engine->world();

        world.addSystem<RendererSyncSystem>(*m_render_queue);

        m_render_thread = std::thread { [this] { threadLoop(); } };
        core::setThreadName(m_render_thread, "Render Thread");
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Renderer::~Renderer() {
        m_stop_thread = true;
        if (m_render_thread.joinable()) m_render_thread.join();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Renderer::Renderer(Renderer &&other) noexcept {
        other.m_stop_thread = true;
        if (other.m_render_thread.joinable()) other.m_render_thread.join();

        m_instance      = std::move(other.m_instance);
        m_device        = std::move(other.m_device);
        m_surface       = std::move(other.m_surface);
        m_surface_views = std::move(other.m_surface_views);
        m_render_thread = std::thread { [this] { threadLoop(); } };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Renderer::operator=(Renderer &&other) noexcept -> Renderer & {
        if (&other == this) [[unlikely]]
            return *this;

        other.m_stop_thread = true;
        if (other.m_render_thread.joinable()) other.m_render_thread.join();

        m_instance      = std::move(other.m_instance);
        m_device        = std::move(other.m_device);
        m_surface       = std::move(other.m_surface);
        m_surface_views = std::move(other.m_surface_views);
        m_render_thread = std::thread { [this] { threadLoop(); } };

        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Renderer::threadLoop() -> void {
        while (!m_stop_thread) {
            if (m_surface->needRecreate()) {
                m_surface->recreate();
                reallocateViewDependentResources();
            }

            if (m_updated) {
                m_updated = false;

                m_render_queue->update();
                m_framegraph = m_builder->bake();
            }

            auto frame = std::move(m_surface->acquireNextFrame().value());
            m_surface->present(frame);
        }

        m_device->waitIdle();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Renderer::reallocateViewDependentResources() -> void {
        const auto &images = m_surface->images();

        m_surface_views.clear();
        m_surface_views.reserve(std::size(images));
        for (const auto &image : images) m_surface_views.emplace_back(image.createView());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Renderer::rebuildFrameGraph() -> void {}
} // namespace stormkit::engine
