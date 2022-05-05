// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

namespace stormkit::wsi::details {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImplBase::pollEvent(Event &event) noexcept -> bool {
        auto has_event = !m_events.empty();

        if (has_event) {
            event = m_events.front();
            m_events.pop();
        }

        return has_event;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImplBase::waitEvent(Event &event) noexcept -> bool {
        auto has_event = !m_events.empty();

        if (has_event) {
            event = m_events.front();
            m_events.pop();
        }

        return has_event;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImplBase::mouseDownEvent(MouseButton button,
                                               core::Int16 x,
                                               core::Int16 y) noexcept -> void {
        auto event = Event {};
        event.type = EventType::MouseButtonPushed;
        event.data = MouseButtonPushedEventData { .position = { x, y }, .button = button };

        pushEvent(event);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImplBase::mouseUpEvent(MouseButton button,
                                             core::Int16 x,
                                             core::Int16 y) noexcept -> void {
        auto event = Event {};
        event.type = EventType::MouseButtonReleased;
        event.data = MouseButtonReleasedEventData { .position = { x, y }, .button = button };

        pushEvent(event);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImplBase::mouseMoveEvent(core::Int16 x, core::Int16 y) noexcept -> void {
        auto event = Event {};
        event.type = EventType::MouseMoved;
        event.data = MouseMovedEventData { .position = { x, y } };

        pushEvent(event);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImplBase::mouseEnteredEvent() noexcept -> void {
        auto event = Event {};
        event.type = EventType::MouseEntered;
        event.data = MouseEnteredEventData {};

        pushEvent(event);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImplBase::mouseExitedEvent() noexcept -> void {
        auto event = Event {};
        event.type = EventType::MouseExited;
        event.data = MouseExitedEventData {};

        pushEvent(event);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImplBase::keyDownEvent(Key key, char character) noexcept -> void {
        auto event = Event {};
        event.type = EventType::KeyPressed;
        event.data = KeyPressedEventData { .key = key, .character = character };

        pushEvent(event);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImplBase::keyUpEvent(Key key, char character) noexcept -> void {
        auto event = Event {};
        event.type = EventType::KeyReleased;
        event.data = KeyReleasedEventData { .key = key, .character = character };

        pushEvent(event);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImplBase::closeEvent() noexcept -> void {
        auto event = Event {};
        event.type = EventType::Closed;
        event.data = ClosedEventData {};

        pushEvent(event);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImplBase::resizeEvent(core::UInt16 width, core::UInt16 height) noexcept
        -> void {
        auto event = Event {};
        event.type = EventType::Resized;
        event.data = ResizedEventData { .extent { width, height } };

        pushEvent(event);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImplBase::minimizeEvent() noexcept -> void {
        auto event = Event {};
        event.type = EventType::Minimized;
        event.data = MinimizedEventData {};

        pushEvent(event);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImplBase::maximizeEvent() noexcept -> void {
        auto event = Event {};
        event.type = EventType::Maximized;
        event.data = MaximizedEventData {};

        pushEvent(event);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImplBase::lostFocusEvent() noexcept -> void {
        auto event = Event {};
        event.type = EventType::LostFocus;
        event.data = LostFocusEventData {};

        pushEvent(event);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImplBase::gainedFocusEvent() noexcept -> void {
        auto event = Event {};
        event.type = EventType::GainedFocus;
        event.data = GainedFocusEventData {};

        pushEvent(event);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImplBase::pushEvent(const Event &event) noexcept -> void {
        m_events.emplace(event);
    }
} // namespace stormkit::wsi::details
