// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <variant>

/////////// - StormKit::core - ///////////
#include <stormkit/core/Math.hpp>
#include <stormkit/core/Types.hpp>

/////////// - StormKit::wsi - ///////////
#include <stormkit/wsi/Event.hpp>
#include <stormkit/wsi/Fwd.hpp>
#include <stormkit/wsi/Monitor.hpp>
#include <stormkit/wsi/Window.hpp>
#include <stormkit/wsi/WindowStyle.hpp>

#include "../common/WindowImplBase.hpp"
#include "wayland/WindowImpl.hpp"
#include "x11/WindowImpl.hpp"

namespace stormkit::wsi::details {
    class WindowImpl: public WindowImplBase {
      public:
        explicit WindowImpl(Window::WM wm);
        WindowImpl(Window::WM wm, std::string title, const core::ExtentU& size, WindowStyle style);
        ~WindowImpl();

        WindowImpl(const WindowImpl&) noexcept                    = delete;
        auto operator=(const WindowImpl&) noexcept -> WindowImpl& = delete;

        WindowImpl(WindowImpl&&) noexcept;
        auto operator=(WindowImpl&&) noexcept -> WindowImpl&;

        auto create(std::string title, const core::ExtentU& size, WindowStyle style) -> void;
        auto close() noexcept -> void;

        [[nodiscard]] auto pollEvent(Event& event) noexcept -> bool;
        [[nodiscard]] auto waitEvent(Event& event) noexcept -> bool;

        auto setTitle(std::string title) noexcept -> void;
        auto setExtent(const core::ExtentU& size) noexcept -> void;
        auto setFullscreenEnabled(bool enabled) noexcept -> void;

        auto lockMouse() noexcept -> void;
        auto unlockMouse() noexcept -> void;

        auto hideMouse() noexcept -> void;
        auto unhideMouse() noexcept -> void;

        [[nodiscard]] auto extent() const noexcept -> const core::ExtentU&;
        [[nodiscard]] auto title() const noexcept -> const std::string&;

        [[nodiscard]] auto isOpen() const noexcept -> bool;
        [[nodiscard]] auto visible() const noexcept -> bool;

        [[nodiscard]] auto nativeHandle() const noexcept -> Window::NativeHandle;

        [[nodiscard]] auto mouseLocked() const noexcept -> bool;
        [[nodiscard]] auto mouseHidden() const noexcept -> bool;
        [[nodiscard]] auto fullscreen() const noexcept -> bool;

        auto setKeyRepeatEnabled(bool enabled) noexcept -> void;
        [[nodiscard]] auto keyRepeatEnabled() const noexcept -> bool;

        auto setVirtualKeyboardVisible(bool visible) noexcept -> void;

        auto setMousePosition(const core::Position2i& position) noexcept -> void;
        static auto setMousePositionOnDesktop(Window::WM wm,
                                              const core::Position2u& position) noexcept -> void;

        [[nodiscard]] static auto getMonitorSettings(Window::WM wm) -> std::vector<Monitor>;

      private:
        struct Sentinelle {};

        using BackendWindowImpl = std::variant<x11::WindowImpl, wayland::WindowImpl, Sentinelle>;

        Window::WM m_wm;

        BackendWindowImpl m_impl;
    };
} // namespace stormkit::wsi::details

#include "WindowImpl.inl"
