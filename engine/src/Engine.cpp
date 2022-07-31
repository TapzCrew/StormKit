// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <stormkit/core/ThreadUtils.hpp>

#include <stormkit/engine/Engine.hpp>
#include <stormkit/engine/render/Renderer.hpp>

#include <stormkit/wsi/Event.hpp>
#include <stormkit/wsi/EventHandler.hpp>
#include <stormkit/wsi/Monitor.hpp>
#include <stormkit/wsi/Window.hpp>

#include <stormkit/log/ConsoleLogger.hpp>

#include "Log.hpp"

namespace stormkit::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    Engine::Engine(wsi::Window& window) : m_window { &window } {
        m_logger = log::Logger::allocateLoggerInstance<log::ConsoleLogger>();

        m_event_handler = std::make_unique<wsi::EventHandler>(*m_window);

        core::setCurrentThreadName("StormKit:MainThread");

        ilog("Initialization of Renderer");
        m_renderer = std::make_unique<Renderer>(*this);
        ilog("---------------- success");

        m_event_handler->addCallback(wsi::EventType::Closed,
                                     [this]([[maybe_unused]] const wsi::Event& event) {
                                         m_window->close();
                                     });
        m_event_handler->addCallback(wsi::EventType::KeyReleased, [&](const wsi::Event& event) {
            const auto extent = wsi::Window::getPrimaryMonitorSettings().extents.back();

            const auto& event_data = core::as<wsi::KeyReleasedEventData>(event.data);
            if (event_data.key == wsi::Key::Escape) [[unlikely]] { m_window->close(); }
            if (event_data.key == wsi::Key::F11) {
                if (m_fullscreen_enabled) {
                    m_fullscreen_enabled = false;
                    m_window->setFullscreenEnabled(false);
                } else {
                    m_fullscreen_enabled = true;
                    m_window->setExtent(extent);
                    m_window->setFullscreenEnabled(true);
                }
            }
        });
    }

    Engine::Engine(const core::ExtentU& extent, gpu::Surface::Buffering buffering) {
        m_logger = log::Logger::allocateLoggerInstance<log::ConsoleLogger>();

        core::setCurrentThreadName("StormKit:MainThread");

        ilog("Initialization of Renderer");
        m_renderer = std::make_unique<Renderer>(*this, extent, buffering);
        ilog("---------------- success");

        /*
        m_event_handler->addCallback(wsi::EventType::Closed,
                                     [this]([[maybe_unused]] const wsi::Event &event) {
                                         m_window->close();
                                     });
        m_event_handler->addCallback(wsi::EventType::KeyReleased, [&](const wsi::Event &event) {
            const auto size = wsi::Window::getPrimaryMonitorSettings().sizes.back();

            const auto &event_data = core::as<wsi::KeyReleasedEventData>(event.data);
            if (event_data.key == wsi::Key::Escape) [[unlikely]] { m_window->close(); }
            if (event_data.key == wsi::Key::F11) {
                if (m_fullscreen_enabled) {
                    m_fullscreen_enabled = false;
                    m_window->setFullscreenEnabled(false);
                } else {
                    m_fullscreen_enabled = true;
                    m_window->setSize(size);
                    m_window->setFullscreenEnabled(true);
                }
            }
        });*/
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Engine::~Engine() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    Engine::Engine(Engine&& other) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto Engine::operator=(Engine&& other) noexcept -> Engine& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto Engine::run([[maybe_unused]] const int argc, [[maybe_unused]] const char **argv)
        -> core::Int32 {
        if (m_event_handler)
            while (m_window->isOpen()) { m_event_handler->update(); }
        else
            while (m_window->isOpen()) {}

        return EXIT_SUCCESS;
    }
} // namespace stormkit::engine
