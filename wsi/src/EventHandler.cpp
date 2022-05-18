// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <stormkit/wsi/EventHandler.mpp>
#include <stormkit/wsi/Window.mpp>

namespace stormkit::wsi {
    /////////////////////////////////////
    /////////////////////////////////////
    EventHandler::EventHandler(Window &window) : m_window { window } {}

    /////////////////////////////////////
    /////////////////////////////////////
    auto EventHandler::update() -> void {
        auto event = Event {};

        while (m_window.get().pollEvent(event)) {
            auto event_type = event.type;

            for (auto &callback : m_callback[event_type]) callback(event);
        }
    }
} // namespace stormkit::wsi
