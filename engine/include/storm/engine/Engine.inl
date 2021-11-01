// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class T, typename... Args>
    auto Engine::pushState(Args &&...args) -> void {
        m_state_manager.requestPush<T>(std::forward<Args>(args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class T, typename... Args>
    auto Engine::setState(Args &&...args) -> void {
        m_state_manager.requestSet(std::forward<Args>(args)...);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto Engine::popState() -> void { m_state_manager.requestPop(); }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto Engine::currentState() noexcept -> core::State & { return m_state_manager.top(); }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto Engine::currentState() const noexcept -> const core::State & {
        return m_state_manager.top();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<CHILD_OF_STATE_CONCEPT(State)>
    auto Engine::currentState() -> State & {
        return static_cast<State &>(m_state_manager.top());
    }

    template<CHILD_OF_STATE_CONCEPT(State)>
    auto Engine::currentState() const -> const State & {
        return static_cast<State &>(m_state_manager.top());
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto Engine::instance() noexcept -> render::Instance & { return *m_instance; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto Engine::instance() const noexcept -> const render::Instance & {
        return *m_instance;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto Engine::device() noexcept -> render::Device & { return *m_device; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto Engine::device() const noexcept -> const render::Device & { return *m_device; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto Engine::surface() noexcept -> render::Surface & { return *m_surface; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto Engine::surface() const noexcept -> const render::Surface & { return *m_surface; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto Engine::getCPUTime() const noexcept -> float { return m_cpu_time; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto Engine::profiler() noexcept -> Profiler & { return *m_profiler; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto Engine::profiler() const noexcept -> const Profiler & { return *m_profiler; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto Engine::maxAnisotropy() const noexcept -> float { return m_max_anisotropy; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto Engine::maxSampleCount() const noexcept -> render::SampleCountFlag {
        return m_max_sample_count;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto Engine::currentFrame() noexcept -> render::Surface::Frame & { return *m_frame; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto Engine::currentFrame() const noexcept -> const render::Surface::Frame & {
        return *m_frame;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto Engine::resourceCache() noexcept -> ResourceCache & { return *m_resource_cache; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto Engine::resourceCache() const noexcept -> const ResourceCache & {
        return *m_resource_cache;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto Engine::frameGraph() noexcept -> FrameGraph & { return *m_frame_graph; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto Engine::frameGraph() const noexcept -> const FrameGraph & { return *m_frame_graph; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto Engine::deletionQueue() noexcept -> DeletionQueue & { return *m_deletion_queue; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto Engine::deletionQueue() const noexcept -> const DeletionQueue & {
        return *m_deletion_queue;
    }
} // namespace storm::engine
