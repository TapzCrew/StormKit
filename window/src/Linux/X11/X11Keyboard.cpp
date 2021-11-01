// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "X11Keyboard.hpp"
#include "Log.hpp"
#include "Utils.hpp"
#include "X11Window.hpp"

/////////// - XKB - ///////////
#define explicit _explicit
#include <xcb/xkb.h>
#undef explicit

using namespace storm;
using namespace storm::window;
using namespace storm::window::details;

/////////////////////////////////////
/////////////////////////////////////
X11Keyboard::X11Keyboard(const AbstractWindow &window) noexcept : Keyboard { window } {
    m_xcb_window = static_cast<X11WindowConstPtr>(m_window);
}

/////////////////////////////////////
/////////////////////////////////////
X11Keyboard::~X11Keyboard() = default;

/////////////////////////////////////
/////////////////////////////////////
X11Keyboard::X11Keyboard(X11Keyboard &&) noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
auto X11Keyboard::operator=(X11Keyboard &&) noexcept -> X11Keyboard & = default;

/////////////////////////////////////
/////////////////////////////////////
auto X11Keyboard::isKeyPressed(Key key) const noexcept -> bool {
    auto handles = m_xcb_window->xcbHandles();

    const auto keysym   = stormkitKeyToXKBKey(key);
    const auto keycodes = xcb_key_symbols_get_keycode(handles.key_symbols, keysym);

    auto keycode    = xcb_keycode_t {};
    auto keycode_it = xcb_keycode_t { keycodes[0] };
    auto i          = 1;
    while (keycode_it != XCB_NO_SYMBOL) {
        auto symbol = xkb_state_key_get_one_sym(handles.state, keycode_it);

        if (symbol == keysym) {
            keycode = keycode_it;
            break;
        }

        keycode_it = keycodes[i];
        ++i;
    }

    auto is_pressed = false;

    auto cookie = xcb_query_keymap(handles.connection);
    auto reply  = xcb_query_keymap_reply(handles.connection, cookie, nullptr);

    is_pressed = ((reply->keys[keycode >> 3] & (1 << (keycode & 7))) != 0);

    free(reply);

    return is_pressed;
}

/////////////////////////////////////
/////////////////////////////////////
auto X11Keyboard::setVirtualKeyboardVisible(bool visible) noexcept -> void {
    dlog("X11Keyboard::setVirtualKeyboardVisible isn't yet implemented");
}
