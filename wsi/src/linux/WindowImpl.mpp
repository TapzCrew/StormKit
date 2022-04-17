// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#pragma once

#if defined(STORMKIT_CXX20_MODULES)
// clang-format off
export module stormkit.wsi.details.windowimpl;

/////////// - STL - ///////////
import <variant>;

/////////// - StormKit::core - ///////////
import stormkit.core.math;
import stormkit.core.types;

/////////// - StormKit::wsi - ///////////
import stormkit.wsi.event;
import stormkit.wsi.videosettings;
import stormkit.wsi.windowstyle;
import stormkit.wsi.window;

import stormkit.wsi.details.x11.windowimpl;
import stormkit.wsi.details.wayland.windowimpl;
// clang-format on
export {
#else
    /////////// - STL - ///////////
    #include <variant>

    /////////// - StormKit::core - ///////////
    #include <stormkit/core/Math.mpp>
    #include <stormkit/core/Types.mpp>

    /////////// - StormKit::wsi - ///////////
    #include <stormkit/wsi/Event.mpp>
    #include <stormkit/wsi/Fwd.hpp>
    #include <stormkit/wsi/VideoSettings.mpp>
    #include <stormkit/wsi/Window.mpp>
    #include <stormkit/wsi/WindowStyle.mpp>

    #include "wayland/WindowImpl.mpp"
    #include "x11/WindowImpl.mpp"
#endif

    namespace stormkit::wsi::details {
        class WindowImpl {
          public:
            explicit WindowImpl(Window::WM wm);
            WindowImpl(Window::WM wm,
                       std::string title,
                       const VideoSettings &settings,
                       WindowStyle style);
            ~WindowImpl();

            WindowImpl(const WindowImpl &) noexcept = delete;
            auto operator=(const WindowImpl &) noexcept -> WindowImpl & = delete;

            WindowImpl(WindowImpl &&) noexcept;
            auto operator=(WindowImpl &&) noexcept -> WindowImpl &;

            auto create(std::string title, const VideoSettings &settings, WindowStyle style)
                -> void;
            auto close() noexcept -> void;

            [[nodiscard]] auto pollEvent(Event &event) noexcept -> bool;
            [[nodiscard]] auto waitEvent(Event &event) noexcept -> bool;

            auto setTitle(std::string title) noexcept -> void;

            auto setFullscreenEnabled(bool enabled) noexcept -> void;

            auto lockMouse() noexcept -> void;
            auto unlockMouse() noexcept -> void;

            auto hideMouse() noexcept -> void;
            auto unhideMouse() noexcept -> void;

            [[nodiscard]] auto size() const noexcept -> const core::ExtentU &;
            [[nodiscard]] auto title() const noexcept -> std::string_view;
            [[nodiscard]] auto videoSettings() const noexcept -> const VideoSettings &;

            [[nodiscard]] auto isOpen() const noexcept -> bool;
            [[nodiscard]] auto visible() const noexcept -> bool;

            [[nodiscard]] auto nativeHandle() const noexcept -> Window::NativeHandle;

            [[nodiscard]] auto mouseLocked() const noexcept -> bool;
            [[nodiscard]] auto fullscreen() const noexcept -> bool;

            auto setKeyRepeatEnabled(bool enabled) noexcept -> void;
            [[nodiscard]] auto keyRepeatEnabled() const noexcept -> bool;

            auto setVirtualKeyboardVisible(bool visible) noexcept -> void;

            auto setMousePosition(core::Position2i position) noexcept -> void;
            static auto setMousePositionOnDesktop(Window::WM wm, core::Position2u position) noexcept
                -> void;

            [[nodiscard]] static auto getDesktopModes(Window::WM wm) -> std::vector<VideoSettings>;
            [[nodiscard]] static auto getDesktopFullscreenSize(Window::WM wm) -> VideoSettings;

            struct Sentinelle {};

          private:
            using BackendWindowImpl =
                std::variant<x11::WindowImpl, wayland::WindowImpl, Sentinelle>;

            Window::WM m_wm;

            BackendWindowImpl m_impl;
        };
    } // namespace stormkit::wsi::details

#if defined(STORMKIT_CXX20_MODULES)
}
#endif

#include "WindowImpl.inl"
