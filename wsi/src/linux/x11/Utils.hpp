// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#pragma once

/////////// - XCB - ///////////
#include <xcb/xcb.h>

#if defined(STORMKIT_CXX20_MODULES)
// clang-format off
export module stormkit.wsi.details.x11.utils;

/////////// - StormKit::core - ///////////
import stormkit.core.types;

/////////// - StormKit::wsi - ///////////
import stormkit.wsi.mousebutton;
// clang-format on
export {
#else
    /////////// - StormKit::core - ///////////
    #include <stormkit/core/Types.mpp>

    /////////// - StormKit::wsi - ///////////
    #include <stormkit/wsi/MouseButton.mpp>
#endif

    namespace stormkit::wsi::details::x11 {
        auto x11KeyToChar(xcb_keysym_t key) noexcept -> char;
        auto x11MouseButtonToStormMouseButton(xcb_button_t button) noexcept -> MouseButton;
        auto defaultRootWindow(xcb_connection_t *connection, core::Int32 screen_id) noexcept
            -> xcb_window_t;

    } // namespace stormkit::wsi::details::x11
