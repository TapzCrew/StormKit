// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "Win32Mouse.hpp"
#include "Utils.hpp"
#include "Win32Window.hpp"

using namespace storm;
using namespace storm::window;
using namespace storm::window::details;

/////////////////////////////////////
/////////////////////////////////////
Win32Mouse::Win32Mouse(const AbstractWindow &window) noexcept : Mouse { window } {
    m_win32_window = static_cast<Win32WindowConstPtr>(m_window);
}

/////////////////////////////////////
/////////////////////////////////////
Win32Mouse::~Win32Mouse() noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
Win32Mouse::Win32Mouse(Win32Mouse &&) noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
auto Win32Mouse::operator=(Win32Mouse &&) noexcept -> Win32Mouse & = default;

/////////////////////////////////////
/////////////////////////////////////
auto Win32Mouse::isButtonPressed(MouseButton button) const noexcept -> bool {
    auto vkey = 0;
    switch (button) {
        case MouseButton::Left:
            vkey = GetSystemMetrics(SM_SWAPBUTTON) ? VK_RBUTTON : VK_LBUTTON;
            break;
        case MouseButton::Right:
            vkey = GetSystemMetrics(SM_SWAPBUTTON) ? VK_LBUTTON : VK_RBUTTON;
            break;
        case MouseButton::Middle: vkey = VK_MBUTTON; break;
        case MouseButton::Button1: vkey = VK_XBUTTON1; break;
        case MouseButton::Button2: vkey = VK_XBUTTON2; break;
        default: vkey = 0; break;
    }

    return (GetAsyncKeyState(vkey) & 0x8000) != 0;
}

/////////////////////////////////////
/////////////////////////////////////
auto Win32Mouse::getPositionOnDesktop() const noexcept -> core::Position2u {
    auto point = POINT {};
    GetCursorPos(&point);

    const auto position = core::Vector2u { point.x, point.y };

    return core::makeNamed<core::Position2u>(std::move(position));
}

/////////////////////////////////////
/////////////////////////////////////
void Win32Mouse::setPositionOnDesktop(core::Position2u position) noexcept {
    SetCursorPos(position->x, position->y);
}

/////////////////////////////////////
/////////////////////////////////////
auto Win32Mouse::getPositionOnWindow() const noexcept -> core::Position2i {
    auto handle = m_win32_window->win32Handles();

    auto point = POINT {};
    GetCursorPos(&point);
    ScreenToClient(handle, &point);

    const auto position = core::Vector2i { point.x, point.y };

    return core::makeNamed<core::Position2i>(std::move(position));
}

/////////////////////////////////////
/////////////////////////////////////
auto Win32Mouse::setPositionOnWindow(core::Position2i position) noexcept -> void {
    auto handle = m_win32_window->win32Handles();
    if (handle) {
        auto point =
            POINT { gsl::narrow_cast<long>(position->x), gsl::narrow_cast<long>(position->y) };
        ClientToScreen(handle, &point);
        SetCursorPos(point.x, point.y);
    }
}
