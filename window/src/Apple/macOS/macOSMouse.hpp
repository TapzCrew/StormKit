// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/Math.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

/////////// - StormKit::window - ///////////
#include <storm/window/Fwd.hpp>
#include <storm/window/Mouse.hpp>
#include <storm/window/MouseButton.hpp>

/////////// - Cocoa - ///////////
#include <Carbon/Carbon.h>

/////////// - IOKit - ///////////
#include <IOKit/hid/IOHIDDevice.h>
#include <IOKit/hid/IOHIDManager.h>

namespace storm::window::details {
    class macOSWindow;
    using macOSWindowConstPtr = const macOSWindow *;

    class STORMKIT_PRIVATE macOSMouse final: public Mouse {
      public:
        explicit macOSMouse(const AbstractWindow &window);
        ~macOSMouse() override;

        macOSMouse(macOSMouse &&) noexcept;
        macOSMouse &operator=(macOSMouse &&) noexcept;

        bool isButtonPressed(MouseButton button) const noexcept override;

        core::Position2u getPositionOnDesktop() const noexcept override;
        void setPositionOnDesktop(core::Position2u position) noexcept override;

        core::Position2i getPositionOnWindow() const noexcept override;
        void setPositionOnWindow(core::Position2i position) noexcept override;

        ALLOCATE_HELPERS(macOSMouse)
      private:
        macOSWindowConstPtr m_macos_window;
    };
} // namespace storm::window::details
