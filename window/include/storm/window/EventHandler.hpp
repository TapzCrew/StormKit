// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <vector>

/////////// - StormKit::core - ///////////
#include <storm/core/HashMap.hpp>
#include <storm/core/Platform.hpp>

/////////// - StormKit::window - ///////////
#include <storm/window/Fwd.hpp>

namespace storm::window {
    class STORMKIT_PUBLIC EventHandler {
      public:
        using Callback = std::function<void(const Event &)>;

        explicit EventHandler(Window &window);
        ~EventHandler();

        EventHandler(const EventHandler &);
        EventHandler &operator=(const EventHandler &);

        EventHandler(EventHandler &&) noexcept;
        EventHandler &operator=(EventHandler &&) noexcept;

        void update();

        void addCallback(EventType event_type, Callback callback);
        void clearCallbacks(EventType event_type);

      private:
        WindowPtr m_window;

        storm::core::HashMap<EventType, std::vector<Callback>> m_callback;
    };
} // namespace storm::window

#include "EventHandler.inl"
