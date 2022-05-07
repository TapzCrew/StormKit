// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::code - ///////////
#include <storm/core/Platform.hpp>

/////////// - StormKit::window - ///////////
#include <storm/window/Fwd.hpp>
#include <storm/window/Keyboard.hpp>

namespace storm::window::details {
    class X11Window;
    using X11WindowConstPtr = const X11Window *;

    class STORMKIT_PRIVATE X11Keyboard final: public Keyboard {
      public:
        explicit X11Keyboard(const details::AbstractWindow &window) noexcept;
        ~X11Keyboard() override;

        X11Keyboard(X11Keyboard &&) noexcept;
        X11Keyboard &operator=(X11Keyboard &&) noexcept;

        bool isKeyPressed(Key key) const noexcept override;

        void setVirtualKeyboardVisible(bool visible) noexcept override;

        ALLOCATE_HELPERS(X11Keyboard)
      private:
        X11WindowConstPtr m_xcb_window;
    };
} // namespace storm::window::details
