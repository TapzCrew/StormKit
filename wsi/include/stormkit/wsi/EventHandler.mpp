// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/MemoryMacro.hpp>
#include <stormkit/core/PlatformMacro.hpp>

#include <array>
#include <functional>
#include <memory>
#include <optional>
#include <span>
#include <vector>

#include <stormkit/core/HashMap.mpp>
#include <stormkit/core/Memory.mpp>

#include <stormkit/wsi/Event.mpp>
#include <stormkit/wsi/Fwd.hpp>
#include <stormkit/wsi/Key.mpp>
#include <stormkit/wsi/MouseButton.mpp>

namespace stormkit::wsi {
    class STORMKIT_PUBLIC EventHandler {
      public:
        using Callback = std::function<void(const Event &)>;

        explicit EventHandler(Window &window);

        auto update() -> void;

        auto addCallback(EventType event_type, Callback callback) -> void;
        auto clearCallbacks(EventType event_type) -> void;

      private:
        WindowRef m_window;

        core::HashMap<EventType, std::vector<Callback>> m_callback;
    };
    DECLARE_PTR_AND_REF(EventHandler)
} // namespace stormkit::wsi

#include "EventHandler.inl"
