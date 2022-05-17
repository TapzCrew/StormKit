// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "Engine.mpp"

namespace stormkit::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Engine::world() noexcept -> entities::EntityManager & { return m_world; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Engine::world() const noexcept -> const entities::EntityManager & {
        return m_world;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Engine::window() const noexcept -> wsi::Window & { return *m_window; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Engine::threadPool() noexcept -> core::ThreadPool & { return m_thread_pool; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Engine::threadPool() const noexcept -> const core::ThreadPool & {
        return m_thread_pool;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Engine::renderer() noexcept -> Renderer & { return *m_renderer; }
} // namespace stormkit::engine
