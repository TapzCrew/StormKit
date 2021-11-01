// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <filesystem>

#include "X11Window.hpp"

#include <libinput.h>
#include <libudev.h>

/////////// - StormKit::window - ///////////
#include <storm/window/InputHandler.hpp>
#include <storm/window/Key.hpp>

namespace storm::window::details {
    class X11InputHandler final: public InputHandler {
      public:
        explicit X11InputHandler(const AbstractWindow &window);
        ~X11InputHandler() override;

        X11InputHandler(X11InputHandler &&) noexcept;
        X11InputHandler &operator=(X11InputHandler &&) noexcept;

        bool isKeyPressed(window::Key key) const noexcept override;
        bool isMouseButtonPressed(MouseButton button) const noexcept override;

        core::Position2u getMousePositionOnDesktop() const noexcept override;
        void setMousePositionOnDesktop(core::Position2u position) noexcept override;

        core::Position2i getMousePositionOnWindow() const noexcept override;
        void setMousePositionOnWindow(core::Position2i position) noexcept override;

        void setVirtualKeyboardVisible(bool visible) noexcept override;

        void handleEvents();

        ALLOCATE_HELPERS(X11InputHandler)
      private:
        struct Key {
            window::Key key;
            bool down = false;
        };

        X11WindowConstPtr m_x11_window;

        std::array<Key, window::KEY_COUNT> m_keyboard;
    };
} // namespace storm::window::details
