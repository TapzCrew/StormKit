// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once


#include <array>
#include <memory>
#include <optional>
#include <span>
#include <vector>


#include <stormkit/core/MemoryMacro.hpp>
#include <stormkit/core/Types.mpp>

namespace stormkit::wsi {
    enum class EventType : core::UInt8;
    enum class Key : core::Int8;
    enum class MouseButton : core::UInt8;
    enum class WindowStyle : core::UInt8;

    struct Monitor;

    class Window;
    DECLARE_PTR_AND_REF(Window)

    class EventHandler;
    DECLARE_PTR_AND_REF(EventHandler)

    class Keyboard;
    DECLARE_PTR_AND_REF(Keyboard)

    class Mouse;
    DECLARE_PTR_AND_REF(Mouse)
} // namespace stormkit::wsi
