// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/ThreadPool.hpp>
#include <stormkit/core/Types.hpp>

#include <stormkit/wsi/EventHandler.hpp>
#include <stormkit/wsi/Fwd.hpp>

#include <stormkit/entities/EntityManager.hpp>

#include <stormkit/engine/Fwd.hpp>

#include <stormkit/gpu/core/Surface.hpp>

namespace stormkit::engine {
    class STORMKIT_PUBLIC Engine {
      public:
        explicit Engine(wsi::Window& window);
        explicit Engine(const core::ExtentU& extent,
                        gpu::Surface::Buffering buffering = gpu::Surface::Buffering::Triple);

        ~Engine();

        Engine(const Engine&)                    = delete;
        auto operator=(const Engine&) -> Engine& = delete;

        Engine(Engine&&) noexcept;
        auto operator=(Engine&&) noexcept -> Engine&;

        auto world() noexcept -> entities::EntityManager&;
        auto world() const noexcept -> const entities::EntityManager&;

        auto window() const noexcept -> wsi::Window&;

        auto threadPool() noexcept -> core::ThreadPool&;
        auto threadPool() const noexcept -> const core::ThreadPool&;

        auto renderer() noexcept -> Renderer&;

        auto run(const int argc, const char **argv) -> stormkit::core::Int32;

      private:
        wsi::Window *m_window = nullptr;
        wsi::EventHandlerOwnedPtr m_event_handler;

        core::ThreadPool m_thread_pool;

        RendererOwnedPtr m_renderer;

        entities::EntityManager m_world;

        log::LoggerOwnedPtr m_logger;

        bool m_fullscreen_enabled = false;
    };

} // namespace stormkit::engine

#include "Engine.inl"
