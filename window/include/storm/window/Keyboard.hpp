// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/Types.hpp>

/////////// - StormKit::window - ///////////
#include <storm/window/Fwd.hpp>
#include <storm/window/Key.hpp>

namespace storm::window {
    class STORMKIT_PUBLIC Keyboard: public core::NonCopyable {
      public:
        virtual ~Keyboard() = 0;

        Keyboard(Keyboard &&) noexcept = default;
        Keyboard &operator=(Keyboard &&) noexcept = default;

        virtual bool isKeyPressed(Key key) const noexcept = 0;

        virtual void setVirtualKeyboardVisible(bool visible) noexcept = 0;

      protected:
        explicit Keyboard(const details::AbstractWindow &window) noexcept : m_window { &window } {}

        details::AbstractWindowConstPtr m_window;
    };

    inline Keyboard::~Keyboard() = default;
} // namespace storm::window

#include "Keyboard.inl"
