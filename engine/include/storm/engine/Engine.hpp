// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <chrono>

/////////// - StormKit::core - ///////////
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/StateManager.hpp>

/////////// - StormKit::window - ///////////
#include <storm/window/Fwd.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/Fwd.hpp>

#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/Instance.hpp>
#include <storm/render/core/PhysicalDevice.hpp>
#include <storm/render/core/Surface.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>
#include <storm/engine/Profiler.hpp>

#include <storm/engine/render/utils/DeletionQueue.hpp>
#include <storm/engine/render/utils/ResourceCache.hpp>

#include <storm/engine/render/framegraph/FrameGraph.hpp>

namespace storm::engine {
    class STORMKIT_PUBLIC Engine {
      public:
        // using Callback = std::function<void(FramePassTextureID &, FrameGraph &)>;
        using ImmediateSubmitCallback = std::function<void(render::CommandBuffer &)>;

        Engine(const window::Window &window, std::string app_name);
        Engine(core::Extentu extent, render::Surface::Buffering buffering, std::string app_name);
        Engine(VkInstance instance,
               VkPhysicalDevice physical_device,
               VkDevice device,
               const window::Window &window);
        Engine(VkInstance instance,
               VkPhysicalDevice physical_device,
               VkDevice device,
               core::Extentu extent,
               render::Surface::Buffering buffering);
        ~Engine();

        Engine(Engine &&) noexcept;
        Engine &operator=(Engine &&) noexcept;

        template<class T, typename... Args>
        void pushState(Args &&...args);
        template<class T, typename... Args>
        void setState(Args &&...args);
        void popState();

        core::State &currentState() noexcept;
        const core::State &currentState() const noexcept;

        template<CHILD_OF_STATE_CONCEPT(State)>
        State &currentState();
        template<CHILD_OF_STATE_CONCEPT(State)>
        const State &currentState() const;

        void update();

        void recreateSwapchain();

        render::Surface::Frame &beginFrame();
        void endFrame();

        [[nodiscard]] render::Instance &instance() noexcept;
        [[nodiscard]] const render::Instance &instance() const noexcept;

        [[nodiscard]] render::Device &device() noexcept;
        [[nodiscard]] const render::Device &device() const noexcept;

        [[nodiscard]] render::Surface &surface() noexcept;
        [[nodiscard]] const render::Surface &surface() const noexcept;

        [[nodiscard]] float getCPUTime() const noexcept;
        [[nodiscard]] float getGPUTime() const noexcept;

        [[nodiscard]] Profiler &profiler() noexcept;
        [[nodiscard]] const Profiler &profiler() const noexcept;

        [[nodiscard]] float maxAnisotropy() const noexcept;
        [[nodiscard]] render::SampleCountFlag maxSampleCount() const noexcept;

        [[nodiscard]] render::Surface::Frame &currentFrame() noexcept;
        [[nodiscard]] const render::Surface::Frame &currentFrame() const noexcept;

        [[nodiscard]] ResourceCache &resourceCache() noexcept;
        [[nodiscard]] const ResourceCache &resourceCache() const noexcept;

        [[nodiscard]] FrameGraph &frameGraph() noexcept;
        [[nodiscard]] const FrameGraph &frameGraph() const noexcept;

        [[nodiscard]] DeletionQueue &deletionQueue() noexcept;
        [[nodiscard]] const DeletionQueue &deletionQueue() const noexcept;
        // FramePassTextureID doInitDebugGUIPasses(FramePassTextureID output, FrameGraph
        // &frame_graph);

        struct ImmediateSubmitData {
            render::CommandBuffer cmb;
            render::Fence fence;
        };

        ImmediateSubmitData immediateSubmit(const ImmediateSubmitCallback &callback,
                                            bool on_transfert_queue = false) const;

      private:
        void initialize();

        using Clock = std::chrono::high_resolution_clock;

        render::InstanceOwnedPtr m_instance;
        render::PhysicalDeviceOwnedPtr m_physical_device;
        render::DeviceOwnedPtr m_device;
        render::SurfaceOwnedPtr m_surface;

        core::DeferredAlloc<render::Surface::Frame> m_frame;

        bool m_is_frame_began = false;

        bool m_is_msaa_enabled = true;

        Clock::time_point m_last_tp;
        core::Secondf m_delta = core::Secondf { 0 };
        float m_cpu_time      = 0.f;
        float m_gpu_time      = 0.f;

        float m_max_anisotropy                     = 1.f;
        render::SampleCountFlag m_max_sample_count = render::SampleCountFlag::C1_BIT;

        core::StateManager m_state_manager;

        core::DeferredAlloc<FrameGraph> m_frame_graph;

        core::DeferredAlloc<ResourceCache> m_resource_cache;

        core::DeferredAlloc<Profiler> m_profiler;

        core::DeferredAlloc<DeletionQueue> m_deletion_queue;
    };
} // namespace storm::engine

#include "Engine.inl"
