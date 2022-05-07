// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#if defined(STORMKIT_CXX20_MODULES)
module stormkit.wsi.window;
#else
    /////////// - StormKit::wsi - ///////////
    #include <stormkit/wsi/EventHandler.mpp>
    #include <stormkit/wsi/Window.mpp>
#endif

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
