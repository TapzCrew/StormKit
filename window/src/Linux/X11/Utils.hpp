// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "../Utils.hpp"

/////////// - StormKit::core - ///////////
#include <storm/core/Platform.hpp>

/////////// - StormKit::window - ///////////
#include <storm/window/Key.hpp>
#include <storm/window/MouseButton.hpp>

/////////// - XCB - ///////////
#include <xcb/xcb.h>

namespace storm::window::details {
    STORMKIT_PRIVATE char x11KeyToChar(xcb_keysym_t key) noexcept;
    STORMKIT_PRIVATE storm::window::MouseButton
        x11MouseButtonToStormMouseButton(xcb_button_t button) noexcept;
    STORMKIT_PRIVATE xcb_window_t defaultRootWindow(xcb_connection_t *connection,
                                                    storm::core::Int32 screen_id) noexcept;

} // namespace storm::window::details
