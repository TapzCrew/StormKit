// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <stormkit/core/ThreadUtils.mpp>

#include <stormkit/engine/Engine.mpp>
#include <stormkit/engine/render/Renderer.mpp>

#include <stormkit/wsi/Event.mpp>
#include <stormkit/wsi/EventHandler.mpp>
#include <stormkit/wsi/Monitor.mpp>
#include <stormkit/wsi/Window.mpp>

#include "Log.mpp"

namespace stormkit::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    Engine::Engine(wsi::Window &window) : m_window { &window }, m_event_handler { *m_window } {
        core::setCurrentThreadName("Main Thread");

        ilog("Initialization of Renderer");
        m_renderer = std::make_unique<Renderer>(*this);
        ilog("---------------- success");

        m_event_handler.addCallback(wsi::EventType::Closed,
                                    [this]([[maybe_unused]] const wsi::Event &event) {
                                        m_window->close();
                                    });
        m_event_handler.addCallback(wsi::EventType::KeyReleased, [&](const wsi::Event &event) {
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
        });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Engine::~Engine() {}

    /////////////////////////////////////
    /////////////////////////////////////
    Engine::Engine(Engine &&other) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto Engine::operator=(Engine &&other) noexcept -> Engine & = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto Engine::run(const int argc, const char **argv) -> core::Int32 {
        while (m_window->isOpen()) { m_event_handler.update(); }

        return EXIT_SUCCESS;
    }
} // namespace stormkit::engine
