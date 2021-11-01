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
#include <storm/window/MouseButton.hpp>

namespace storm::window {
    class STORMKIT_PUBLIC Mouse: public core::NonCopyable {
      public:
        virtual ~Mouse() = 0;

        Mouse(Mouse &&) noexcept = default;
        Mouse &operator=(Mouse &&) noexcept = default;

        virtual bool isButtonPressed(MouseButton button) const noexcept = 0;

        virtual core::Position2u getPositionOnDesktop() const noexcept        = 0;
        virtual void setPositionOnDesktop(core::Position2u position) noexcept = 0;

        virtual core::Position2i getPositionOnWindow() const noexcept        = 0;
        virtual void setPositionOnWindow(core::Position2i position) noexcept = 0;

      protected:
        explicit Mouse(const details::AbstractWindow &window) noexcept : m_window { &window } {}

        details::AbstractWindowConstPtr m_window;
    };

    inline Mouse::~Mouse() = default;
} // namespace storm::window

#include "Mouse.inl"
