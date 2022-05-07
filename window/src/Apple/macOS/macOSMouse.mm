// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/////////// - StormKit::window - ///////////
#import "macOSMouse.hpp"
#import "StormView.hpp"
#import "Utils.hpp"
#import "macOSWindow.hpp"

#include <storm/window/Event.hpp>
#include <storm/window/VideoSettings.hpp>
#include <storm/window/Window.hpp>

/////////// - SL - ///////////
#include <cmath>

/////////// - AppKit - ///////////
#import <AppKit/AppKit.h>

using namespace storm;
using namespace storm::window;
using namespace storm::window::details;

/////////////////////////////////////
/////////////////////////////////////
void setMousePositionInternal(CGPoint point) {
    auto move_event =
        CGEventCreateMouseEvent(nullptr, kCGEventMouseMoved, std::move(point), kCGMouseButtonLeft);

    CGEventPost(kCGSessionEventTap, move_event);
    CFRelease(move_event);
}

/////////////////////////////////////
/////////////////////////////////////
macOSMouse::macOSMouse(const AbstractWindow &window)
    : Mouse { window }, m_macos_window { &static_cast<const macOSWindow &>(window) } {
}

/////////////////////////////////////
/////////////////////////////////////
macOSMouse::~macOSMouse() = default;

/////////////////////////////////////
/////////////////////////////////////
macOSMouse::macOSMouse(macOSMouse &&) noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
auto macOSMouse::operator=(macOSMouse &&) noexcept -> macOSMouse & = default;

/////////////////////////////////////
/////////////////////////////////////
auto macOSMouse::isButtonPressed(MouseButton button) const noexcept -> bool {
    auto state = [NSEvent pressedMouseButtons];

    switch (button) {
        case MouseButton::Left: return (state & (1 << 0)) != 0;
        case MouseButton::Right: return (state & (1 << 1)) != 0;
        case MouseButton::Middle: return (state & (1 << 2)) != 0;
        case MouseButton::Button1: return (state & (1 << 3)) != 0;
        case MouseButton::Button2: return (state & (1 << 4)) != 0;
        default: return false;
    }

    return false;
}

extern "C" {
extern const storm::window::VideoSettings *getDesktopModes(storm::core::ArraySize &size);
}
/////////////////////////////////////
/////////////////////////////////////
auto macOSMouse::getPositionOnDesktop() const noexcept -> core::Position2u {
    auto size                 = core::ArraySize { 0u };
    const auto scale          = [[NSScreen mainScreen] backingScaleFactor];
    const auto desktop_height = macOSWindow::getDesktopFullscreenSize().size.height;
    const auto pos            = [NSEvent mouseLocation];

    const auto position = core::Vector2u { scale * pos.x, desktop_height - (scale * pos.y) };

    return core::makeNamed<core::Position2u>(std::move(position));
}

/////////////////////////////////////
/////////////////////////////////////
auto macOSMouse::setPositionOnDesktop(core::Position2u position) noexcept -> void {
    const auto scale = [[NSScreen mainScreen] backingScaleFactor];
    const auto point = CGPoint { gsl::narrow_cast<double>(position->x / scale),
                                 gsl::narrow_cast<double>(position->y / scale) };

    setMousePositionInternal(std::move(point));
}

/////////////////////////////////////
/////////////////////////////////////
auto macOSMouse::getPositionOnWindow() const noexcept -> core::Position2i {
    auto native_handle = m_macos_window->macOSHandles().view;
    auto window        = [native_handle myWindow];
    const auto scale   = [[NSScreen mainScreen] backingScaleFactor];

    const auto window_height = m_window->size().height;

    const auto screen_pos = [NSEvent mouseLocation];

    const auto rect        = NSMakeRect(screen_pos.x, screen_pos.y, 0, 0);
    const auto window_rect = [window convertRectFromScreen:rect];

    const NSPoint pos =
        [native_handle convertPoint:NSMakePoint(window_rect.origin.x, window_rect.origin.y)
                           fromView:native_handle];
    const auto position = core::Vector2i { scale * pos.x, scale * (window_height - pos.y) };

    return core::makeNamed<core::Position2i>(std::move(position));
}

/////////////////////////////////////
/////////////////////////////////////
auto macOSMouse::setPositionOnWindow(core::Position2i position) noexcept -> void {
    auto native_handle = m_macos_window->macOSHandles().view;
    const auto scale   = [[NSScreen mainScreen] backingScaleFactor];

    const auto point     = CGPoint { gsl::narrow_cast<double>(position->x / scale),
                                 gsl::narrow_cast<double>(position->y / scale) };
    const auto on_screen = [native_handle relativeToGlobal:point];

    setMousePositionInternal(std::move(on_screen));
}
