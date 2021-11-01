// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::window - ///////////
#include <storm/core/Math.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/Types.hpp>

/////////// - StormKit::window - ///////////
#include <storm/window/Fwd.hpp>
#include <storm/window/Mouse.hpp>

namespace storm::window::details {
    class X11Window;
    using X11WindowConstPtr = const X11Window *;

    class STORMKIT_PRIVATE X11Mouse final: public Mouse {
      public:
        explicit X11Mouse(const details::AbstractWindow &window) noexcept;
        ~X11Mouse() override;

        X11Mouse(X11Mouse &&) noexcept;
        X11Mouse &operator=(X11Mouse &&) noexcept;

        bool isButtonPressed(MouseButton button) const noexcept override;

        core::Position2u getPositionOnDesktop() const noexcept override;
        void setPositionOnDesktop(core::Position2u position) noexcept override;

        core::Position2i getPositionOnWindow() const noexcept override;
        void setPositionOnWindow(core::Position2i position) noexcept override;

        ALLOCATE_HELPERS(X11Mouse)
      private:
        X11WindowConstPtr m_xcb_window;
    };
} // namespace storm::window::details
