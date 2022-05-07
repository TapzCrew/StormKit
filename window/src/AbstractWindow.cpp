// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "AbstractWindow.hpp"
#include "Log.hpp"

#include <cstddef>
#include <cstdlib>
#include <stdexcept>

#include <storm/window/VideoSettings.hpp>

using namespace storm;
using namespace storm::window;
using namespace storm::window::details;

/////////////////////////////////////
/////////////////////////////////////
AbstractWindow::AbstractWindow() noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
AbstractWindow::~AbstractWindow() noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
AbstractWindow::AbstractWindow(AbstractWindow &&) noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
auto AbstractWindow::operator=(AbstractWindow &&) noexcept -> AbstractWindow & = default;

/////////////////////////////////////
/////////////////////////////////////
auto AbstractWindow::pollEvent(Event &event) noexcept -> bool {
    auto has_event = !m_events.empty();

    if (has_event) {
        event = m_events.front();
        m_events.pop();
    }

    return has_event;
}

/////////////////////////////////////
/////////////////////////////////////
auto AbstractWindow::waitEvent(Event &event) noexcept -> bool {
    auto has_event = !m_events.empty();

    if (has_event) {
        event = m_events.front();
        m_events.pop();
    }

    return has_event;
}

/////////////////////////////////////
/////////////////////////////////////
auto AbstractWindow::mouseDownEvent(MouseButton button, core::Int16 x, core::Int16 y) noexcept
    -> void {
    Event event;
    event.type               = EventType::MouseButtonPushed;
    event.mouse_event.button = button;
    event.mouse_event.x      = x;
    event.mouse_event.y      = y;

    pushEvent(event);
}

/////////////////////////////////////
/////////////////////////////////////
auto AbstractWindow::mouseUpEvent(MouseButton button, core::Int16 x, core::Int16 y) noexcept
    -> void {
    Event event;
    event.type               = EventType::MouseButtonReleased;
    event.mouse_event.button = button;
    event.mouse_event.x      = x;
    event.mouse_event.y      = y;

    pushEvent(event);
}

/////////////////////////////////////
/////////////////////////////////////
auto AbstractWindow::mouseMoveEvent(core::Int16 x, core::Int16 y) noexcept -> void {
    Event event;
    event.type          = EventType::MouseMoved;
    event.mouse_event.x = x;
    event.mouse_event.y = y; // Up Left 0/0

    pushEvent(event);
}

/////////////////////////////////////
/////////////////////////////////////
auto AbstractWindow::mouseEnteredEvent() noexcept -> void {
    Event event;
    event.type = EventType::MouseEntered;

    pushEvent(event);
}

/////////////////////////////////////
/////////////////////////////////////
auto AbstractWindow::mouseExitedEvent() noexcept -> void {
    Event event;
    event.type = EventType::MouseExited;

    pushEvent(event);
}

/////////////////////////////////////
/////////////////////////////////////
auto AbstractWindow::pushEvent(Event event) noexcept -> void {
    m_events.emplace(event);
}

/////////////////////////////////////
/////////////////////////////////////
auto AbstractWindow::keyDownEvent(Key key, char character) noexcept -> void {
    Event event;
    event.type                = EventType::KeyPressed;
    event.key_event.key       = key;
    event.key_event.character = character;

    pushEvent(event);
}

/////////////////////////////////////
/////////////////////////////////////
auto AbstractWindow::keyUpEvent(Key key, char character) noexcept -> void {
    Event event;
    event.type                = EventType::KeyReleased;
    event.key_event.key       = key;
    event.key_event.character = character;

    pushEvent(event);
}

/////////////////////////////////////
/////////////////////////////////////
auto AbstractWindow::closeEvent() noexcept -> void {
    Event event;
    event.type = EventType::Closed;

    pushEvent(event);
}

/////////////////////////////////////
/////////////////////////////////////
auto AbstractWindow::resizeEvent(core::UInt16 width, core::UInt16 height) noexcept -> void {
    m_video_settings.size.width  = width;
    m_video_settings.size.height = height;

    Event event;
    event.type                 = EventType::Resized;
    event.resized_event.width  = width;
    event.resized_event.height = height;

    pushEvent(event);
}

/////////////////////////////////////
/////////////////////////////////////
auto AbstractWindow::minimizeEvent() noexcept -> void {
    Event event;
    event.type = EventType::Minimized;

    pushEvent(event);
}

/////////////////////////////////////
/////////////////////////////////////
auto AbstractWindow::maximizeEvent() noexcept -> void {
    Event event;
    event.type = EventType::Maximized;

    pushEvent(event);
}

/////////////////////////////////////
/////////////////////////////////////
auto AbstractWindow::lostFocusEvent() noexcept -> void {
    Event event;
    event.type = EventType::LostFocus;

    pushEvent(event);
}

/////////////////////////////////////
/////////////////////////////////////
auto AbstractWindow::gainedFocusEvent() noexcept -> void {
    Event event;
    event.type = EventType::GainedFocus;

    pushEvent(event);
}
