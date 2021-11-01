// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/Platform.hpp>

/////////// - StormKit::window - ///////////
#include <storm/window/Fwd.hpp>
#include <storm/window/Keyboard.hpp>

namespace storm::window::details {
    class Win32Window;
    using Win32WindowConstPtr = const Win32Window *;
    class STORMKIT_PRIVATE Win32Keyboard final: public Keyboard {
      public:
        explicit Win32Keyboard(const AbstractWindow &window) noexcept;
        ~Win32Keyboard() noexcept override;

        Win32Keyboard(Win32Keyboard &&) noexcept;
        Win32Keyboard &operator=(Win32Keyboard &&) noexcept;

        bool isKeyPressed(Key key) const noexcept override;

        void setVirtualKeyboardVisible(bool visible) noexcept override;

        ALLOCATE_HELPERS(Win32Keyboard)
    };
} // namespace storm::window::details
