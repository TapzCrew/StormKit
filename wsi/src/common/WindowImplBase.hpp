// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <queue>

#include <stormkit/core/Math.hpp>
#include <stormkit/core/Types.hpp>

#include <stormkit/wsi/Event.hpp>
#include <stormkit/wsi/Fwd.hpp>
#include <stormkit/wsi/Key.hpp>
#include <stormkit/wsi/Monitor.hpp>
#include <stormkit/wsi/MouseButton.hpp>

namespace stormkit::wsi::details {
    class WindowImplBase {
      public:
        [[nodiscard]] auto pollEvent(Event &event) noexcept -> bool;
        [[nodiscard]] auto waitEvent(Event &event) noexcept -> bool;

        auto mouseDownEvent(MouseButton button, core::Int16 x, core::Int16 y) noexcept -> void;
        auto mouseUpEvent(MouseButton button, core::Int16 x, core::Int16 y) noexcept -> void;

        auto mouseMoveEvent(core::Int16 x, core::Int16 y) noexcept -> void;

        auto mouseEnteredEvent() noexcept -> void;
        auto mouseExitedEvent() noexcept -> void;

        auto keyDownEvent(Key key, char character) noexcept -> void;
        auto keyUpEvent(Key key, char character) noexcept -> void;

        auto closeEvent() noexcept -> void;

        auto resizeEvent(core::UInt16 width, core::UInt16 height) noexcept -> void;
        auto minimizeEvent() noexcept -> void;
        auto maximizeEvent() noexcept -> void;

        auto lostFocusEvent() noexcept -> void;
        auto gainedFocusEvent() noexcept -> void;

      protected:
        auto pushEvent(const Event &event) noexcept -> void;

        Monitor m_video_settings;

        std::queue<Event> m_events;
    };
} // namespace stormkit::wsi::details

#include "WindowImplBase.inl"
