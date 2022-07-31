// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - XCB - ///////////
#include <xcb/xcb.h>

/////////// - StormKit::core - ///////////
#include <stormkit/core/Types.hpp>

/////////// - StormKit::wsi - ///////////
#include <stormkit/wsi/MouseButton.hpp>

namespace stormkit::wsi::details::x11 {
    auto x11KeyToChar(xcb_keysym_t key) noexcept -> char;
    auto x11MouseButtonToStormMouseButton(xcb_button_t button) noexcept -> MouseButton;
    auto defaultRootWindow(xcb_connection_t *connection, core::Int32 screen_id) noexcept
        -> xcb_window_t;
} // namespace stormkit::wsi::details::x11
