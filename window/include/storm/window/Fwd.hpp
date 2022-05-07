// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <cstdint>

/////////// - StormKit::core - ///////////
#include <storm/core/Memory.hpp>

namespace storm::window {
    namespace details {
        class AbstractWindow;
        DECLARE_PTR_AND_REF(AbstractWindow)
    } // namespace details

    enum class EventType : core::UInt8;
    enum class Key : core::Int8;
    enum class MouseButton : core::UInt8;
    enum class WindowStyle : core::UInt8;

    struct Event;
    struct VideoSettings;

    class Window;
    DECLARE_PTR_AND_REF(Window)

    class EventHandler;
    DECLARE_PTR_AND_REF(EventHandler)

    class Keyboard;
    DECLARE_PTR_AND_REF(Keyboard)

    class Mouse;
    DECLARE_PTR_AND_REF(Mouse)

    using NativeHandle = void *;
} // namespace storm::window
