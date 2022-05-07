// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <string>
#include <string_view>

/////////// - StormKit::core - ///////////
#include <storm/core/Flags.hpp>
#include <storm/core/Math.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

/////////// - StormKit::window - ///////////
#include <storm/window/Event.hpp>
#include <storm/window/Fwd.hpp>
#include <storm/window/VideoSettings.hpp>
#include <storm/window/WindowStyle.hpp>

/////////// - StormKit::module - ///////////
#include <storm/module/Fwd.hpp>

namespace storm::window {
    class STORMKIT_PUBLIC Window: public core::NonCopyable {
      public:
        enum class WM { Win32, X11, Wayland, macOS, iOS, Android, Switch };

        Window() noexcept;
        Window(std::string title, const VideoSettings &settings, WindowStyle style) noexcept;
        ~Window();

        Window(Window &&);
        Window &operator=(Window &&);

        void create(std::string title, const VideoSettings &settings, WindowStyle style) noexcept;
        void close() noexcept;

        [[nodiscard]] bool pollEvent(Event &event) noexcept;
        [[nodiscard]] bool waitEvent(Event &event) noexcept;

        void setTitle(std::string title) noexcept;

        void setFullscreenEnabled(bool fullscreen) noexcept;

        void lockMouse() noexcept;
        void unlockMouse() noexcept;
        void hideMouse() noexcept;

        [[nodiscard]] const core::Extentu &size() const noexcept;
        [[nodiscard]] std::string_view title() const noexcept;
        [[nodiscard]] const VideoSettings &videoSettings() const noexcept;

        [[nodiscard]] bool isOpen() const noexcept;
        [[nodiscard]] bool isVisible() const noexcept;

        [[nodiscard]] NativeHandle nativeHandle() const noexcept;

        [[nodiscard]] bool isMouseLocked() const noexcept;
        [[nodiscard]] bool isInFullscreen() const noexcept;

        [[nodiscard]] static std::vector<VideoSettings> getDesktopModes();
        [[nodiscard]] static VideoSettings getDesktopFullscreenSize();

        [[nodiscard]] KeyboardOwnedPtr createKeyboardPtr() const;
        [[nodiscard]] MouseOwnedPtr createMousePtr() const;

      private:
        static WM detectWM() noexcept;

        details::AbstractWindowOwnedPtr m_impl;
    };
} // namespace storm::window

#include "Window.inl"
