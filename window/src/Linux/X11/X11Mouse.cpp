// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "X11Mouse.hpp"
#include "Utils.hpp"
#include "X11Window.hpp"

using namespace storm;
using namespace storm::window;
using namespace storm::window::details;

/////////////////////////////////////
/////////////////////////////////////
X11Mouse::X11Mouse(const AbstractWindow &window) noexcept : Mouse { window } {
    m_xcb_window = static_cast<X11WindowConstPtr>(m_window);
}

/////////////////////////////////////
/////////////////////////////////////
X11Mouse::~X11Mouse() = default;

/////////////////////////////////////
/////////////////////////////////////
X11Mouse::X11Mouse(X11Mouse &&) noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
auto X11Mouse::operator=(X11Mouse &&) noexcept -> X11Mouse & = default;

/////////////////////////////////////
/////////////////////////////////////
auto X11Mouse::isButtonPressed(MouseButton button) const noexcept -> bool {
    auto handles = m_xcb_window->xcbHandles();

    const auto default_screen_id = 0;
    auto root_window             = defaultRootWindow(handles.connection, default_screen_id);

    auto cookie = xcb_query_pointer(handles.connection, root_window);
    auto reply  = xcb_query_pointer_reply(handles.connection, cookie, nullptr);

    auto is_pressed = false;
    switch (button) {
        case MouseButton::Left: is_pressed = (reply->mask & XCB_BUTTON_MASK_1); break;
        case MouseButton::Middle: is_pressed = (reply->mask & XCB_BUTTON_MASK_2); break;
        case MouseButton::Right: is_pressed = (reply->mask & XCB_BUTTON_MASK_3); break;
        case MouseButton::Button1: is_pressed = (reply->mask & XCB_BUTTON_MASK_4); break;
        case MouseButton::Button2: is_pressed = (reply->mask & XCB_BUTTON_MASK_5); break;
        case MouseButton::Unknow: is_pressed = false; break;
    }

    free(reply);

    return is_pressed;
}

/////////////////////////////////////
/////////////////////////////////////
auto X11Mouse::getPositionOnDesktop() const noexcept -> core::Position2u {
    auto handles = m_xcb_window->xcbHandles();

    const auto default_screen_id = 0;
    auto root_window             = defaultRootWindow(handles.connection, default_screen_id);

    auto cookie = xcb_query_pointer(handles.connection, root_window);
    auto reply  = xcb_query_pointer_reply(handles.connection, cookie, nullptr);

    const auto position = core::Vector2u { reply->win_x, reply->win_y };

    free(reply);

    return core::makeNamed<core::Position2u>(position);
}

/////////////////////////////////////
/////////////////////////////////////
auto X11Mouse::setPositionOnDesktop(core::Position2u position) noexcept -> void {
    auto handles = m_xcb_window->xcbHandles();

    const auto default_screen_id = 0;
    auto root_window             = defaultRootWindow(handles.connection, default_screen_id);

    xcb_warp_pointer(handles.connection,
                     XCB_NONE,
                     root_window,
                     0,
                     0,
                     0,
                     0,
                     position->x,
                     position->y);

    xcb_flush(handles.connection);
}

/////////////////////////////////////
/////////////////////////////////////
auto X11Mouse::getPositionOnWindow() const noexcept -> core::Position2i {
    auto handles = m_xcb_window->xcbHandles();

    auto cookie = xcb_query_pointer(handles.connection, handles.window);
    auto reply  = xcb_query_pointer_reply(handles.connection, cookie, nullptr);

    const auto position = core::Vector2i { reply->win_x, reply->win_y };

    free(reply);

    return core::makeNamed<core::Position2i>(position);
}

/////////////////////////////////////
/////////////////////////////////////
auto X11Mouse::setPositionOnWindow(core::Position2i position) noexcept -> void {
    auto handles = m_xcb_window->xcbHandles();

    xcb_warp_pointer(handles.connection,
                     XCB_NONE,
                     handles.window,
                     0,
                     0,
                     0,
                     0,
                     position->x,
                     position->y);

    xcb_flush(handles.connection);
}
