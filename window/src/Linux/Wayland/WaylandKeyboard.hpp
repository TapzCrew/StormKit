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
    class WaylandWindow;
    using WaylandWindowConstPtr = const WaylandWindow *;

    class STORMKIT_PRIVATE WaylandKeyboard final: public Keyboard {
      public:
        explicit WaylandKeyboard(const details::AbstractWindow &window) noexcept;
        ~WaylandKeyboard() override;

        WaylandKeyboard(WaylandKeyboard &&) noexcept;
        WaylandKeyboard &operator=(WaylandKeyboard &&) noexcept;

        bool isKeyPressed(Key key) const noexcept override;

        void setVirtualKeyboardVisible(bool visible) noexcept override;

        ALLOCATE_HELPERS(WaylandKeyboard)
      private:
        WaylandWindowConstPtr m_wayland_window;
    };
} // namespace storm::window::details
