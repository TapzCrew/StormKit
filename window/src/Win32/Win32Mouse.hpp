// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::window - ///////////
#include <storm/window/Fwd.hpp>
#include <storm/window/Mouse.hpp>

namespace storm::window::details {
    class Win32Window;
    using Win32WindowConstPtr = const Win32Window *;
    class STORMKIT_PRIVATE Win32Mouse final: public Mouse {
      public:
        explicit Win32Mouse(const AbstractWindow &window) noexcept;
        ~Win32Mouse() noexcept override;

        Win32Mouse(Win32Mouse &&) noexcept;
        Win32Mouse &operator=(Win32Mouse &&) noexcept;

        bool isButtonPressed(MouseButton button) const noexcept override;

        core::Position2u getPositionOnDesktop() const noexcept override;
        void setPositionOnDesktop(core::Position2u position) noexcept override;

        core::Position2i getPositionOnWindow() const noexcept override;
        void setPositionOnWindow(core::Position2i position) noexcept override;

        ALLOCATE_HELPERS(Win32Mouse)
      private:
        Win32WindowConstPtr m_win32_window;
    };
} // namespace storm::window::details
