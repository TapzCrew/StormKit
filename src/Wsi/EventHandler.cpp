// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module stormkit.Wsi;

import std;

import stormkit.Core;

import :EventHandler;

namespace stormkit::wsi {
    /////////////////////////////////////
    /////////////////////////////////////
    EventHandler::EventHandler(Window& window) : m_window { window } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto EventHandler::update() -> void {
        auto event = Event {};

        while (m_window->pollEvent(event)) {
            const auto event_type = event.type;

            for (auto&& callback : m_callback.at(event_type)) callback(event);
        }
    }
} // namespace stormkit::wsi
