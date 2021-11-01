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
    class WaylandWindow;
    using WaylandWindowConstPtr = const WaylandWindow *;

    class STORMKIT_PRIVATE WaylandMouse final: public Mouse {
      public:
        explicit WaylandMouse(const details::AbstractWindow &window) noexcept;
        ~WaylandMouse() override;

        WaylandMouse(WaylandMouse &&) noexcept;
        WaylandMouse &operator=(WaylandMouse &&) noexcept;

        bool isButtonPressed(MouseButton button) const noexcept override;

        core::Position2u getPositionOnDesktop() const noexcept override;
        void setPositionOnDesktop(core::Position2u position) noexcept override;

        core::Position2i getPositionOnWindow() const noexcept override;
        void setPositionOnWindow(core::Position2i position) noexcept override;

        ALLOCATE_HELPERS(WaylandMouse)
      private:
        WaylandWindowConstPtr m_wayland_window;
    };
} // namespace storm::window::details
