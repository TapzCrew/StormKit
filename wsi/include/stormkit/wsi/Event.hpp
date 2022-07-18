// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <variant>

#include <stormkit/core/Math.hpp>
#include <stormkit/core/Types.hpp>

#include <stormkit/wsi/Key.hpp>
#include <stormkit/wsi/MouseButton.hpp>

#undef None

namespace stormkit::wsi {
    struct NoneEventData;
    struct ClosedEventData;
    struct ResizedEventData;
    struct MinimizedEventData;
    struct MaximizedEventData;
    struct KeyPressedEventData;
    struct KeyReleasedEventData;
    struct MouseButtonPushedEventData;
    struct MouseButtonReleasedEventData;
    struct MouseEnteredEventData;
    struct MouseExitedEventData;
    struct MouseMovedEventData;
    struct LostFocusEventData;
    struct GainedFocusEventData;

    using EventData = std::variant<NoneEventData,
                                   ClosedEventData,
                                   ResizedEventData,
                                   MinimizedEventData,
                                   MaximizedEventData,
                                   KeyPressedEventData,
                                   KeyReleasedEventData,
                                   MouseButtonPushedEventData,
                                   MouseButtonReleasedEventData,
                                   MouseEnteredEventData,
                                   MouseExitedEventData,
                                   MouseMovedEventData,
                                   LostFocusEventData,
                                   GainedFocusEventData>;

    enum class EventType : core::UInt8 {
        None,
        Closed,
        Resized,
        Minimized,
        Maximized,
        KeyPressed,
        KeyReleased,
        MouseButtonPushed,
        MouseButtonReleased,
        MouseEntered,
        MouseExited,
        MouseMoved,
        LostFocus,
        GainedFocus,
    };

    struct NoneEventData {
        static constexpr auto TYPE = EventType::None;
    };

    struct ClosedEventData {
        static constexpr auto TYPE = EventType::Closed;
    };

    struct ResizedEventData {
        static constexpr auto TYPE = EventType::Resized;
        core::ExtentU16 extent;
    };

    struct MinimizedEventData {
        static constexpr auto TYPE = EventType::Minimized;
    };

    struct MaximizedEventData {
        static constexpr auto TYPE = EventType::Maximized;
    };

    struct KeyPressedEventData {
        static constexpr auto TYPE = EventType::KeyPressed;
        Key key;
        char character;
    };

    struct KeyReleasedEventData {
        static constexpr auto TYPE = EventType::KeyReleased;
        Key key;
        char character;
    };

    struct MouseButtonPushedEventData {
        static constexpr auto TYPE = EventType::MouseButtonPushed;
        core::Vector2i position;
        MouseButton button;
    };

    struct MouseButtonReleasedEventData {
        static constexpr auto TYPE = EventType::MouseButtonReleased;
        core::Vector2i position;
        MouseButton button;
    };

    struct MouseEnteredEventData {
        static constexpr auto TYPE = EventType::MouseEntered;
    };

    struct MouseExitedEventData {
        static constexpr auto TYPE = EventType::MouseExited;
    };

    struct MouseMovedEventData {
        static constexpr auto TYPE = EventType::MouseMoved;
        core::Vector2i position;
    };

    struct LostFocusEventData {
        static constexpr auto TYPE = EventType::LostFocus;
    };

    struct GainedFocusEventData {
        static constexpr auto TYPE = EventType::GainedFocus;
    };

    struct Event {
        EventType type = EventType::None;
        EventData data = NoneEventData {};
    };
} // namespace stormkit::wsi
