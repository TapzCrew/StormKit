// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "WaylandMouse.hpp"
#include "Log.hpp"
#include "WaylandWindow.hpp"

#include <linux/input-event-codes.h>

using namespace storm;
using namespace storm::window;
using namespace storm::window::details;

STORMKIT_PRIVATE auto toWButton(MouseButton button) noexcept -> std::uint32_t {
    switch (button) {
        case MouseButton::Left: return BTN_LEFT;
        case MouseButton::Right: return BTN_RIGHT;
        case MouseButton::Middle: return BTN_MIDDLE;
        case MouseButton::Button1: return BTN_FORWARD;
        case MouseButton::Button2: return BTN_BACK;
        default:;
    }

    return BTN_EXTRA;
}

/////////////////////////////////////
/////////////////////////////////////
WaylandMouse::WaylandMouse(const AbstractWindow &window) noexcept : Mouse { window } {
    m_wayland_window = static_cast<WaylandWindowConstPtr>(m_window);
}

/////////////////////////////////////
/////////////////////////////////////
WaylandMouse::~WaylandMouse() = default;

/////////////////////////////////////
/////////////////////////////////////
WaylandMouse::WaylandMouse(WaylandMouse &&) noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
auto WaylandMouse::operator=(WaylandMouse &&) noexcept -> WaylandMouse & = default;

/////////////////////////////////////
/////////////////////////////////////
auto WaylandMouse::isButtonPressed(MouseButton button) const noexcept -> bool {
    const auto wbutton = toWButton(button);

    auto it = core::ranges::find_if(m_wayland_window->mouseState().button_state,
                                    [wbutton](const auto &s) { return s.button == wbutton; });

    return it->down;
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandMouse::getPositionOnDesktop() const noexcept -> core::Position2u {
    elog("Getting on desktop mouse position is not supported on Wayland");
    return { 0, 0 };
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandMouse::setPositionOnDesktop(core::Position2u position) noexcept -> void {
    elog("Setting on desktop mouse position is not supported on Wayland");
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandMouse::getPositionOnWindow() const noexcept -> core::Position2i {
    return core::makeNamed<core::Position2i>(m_wayland_window->mouseState().position_in_window);
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandMouse::setPositionOnWindow(core::Position2i position) noexcept -> void {
    if (!m_wayland_window->isMouseLocked())
        elog("Setting on window mouse position is not supported on Wayland when mouse is not "
             "locked");
    else
        m_wayland_window->setMousePosition(position);
}
