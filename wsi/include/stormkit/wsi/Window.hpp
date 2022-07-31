// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/FlagsMacro.hpp>
#include <stormkit/core/PlatformMacro.hpp>

#include <array>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <stormkit/core/Flags.hpp>
#include <stormkit/core/Math.hpp>
#include <stormkit/core/Pimpl.hpp>
#include <stormkit/core/Types.hpp>

#include <stormkit/wsi/Event.hpp>
#include <stormkit/wsi/Monitor.hpp>
#include <stormkit/wsi/WindowStyle.hpp>

namespace stormkit::wsi {
    namespace details {
        class WindowImpl;
        using WindowImplPimpl = core::Pimpl<WindowImpl>;
    } // namespace details

    class STORMKIT_PUBLIC Window {
      public:
        using NativeHandle = void *;

        enum class WM {
            Win32,
            X11,
            Wayland,
            macOS,
            iOS,
            Android,
            Switch
        };

        Window() noexcept;
        Window(std::string title, const core::ExtentU& size, WindowStyle style) noexcept;
        ~Window();

        Window(Window&&);
        auto operator=(Window&&) -> Window&;

        auto create(std::string title, const core::ExtentU& size, WindowStyle style) noexcept
            -> void;
        auto close() noexcept -> void;

        [[nodiscard]] auto pollEvent(Event& event) noexcept -> bool;
        [[nodiscard]] auto waitEvent(Event& event) noexcept -> bool;

        auto setTitle(std::string title) noexcept -> void;

        auto setFullscreenEnabled(bool fullscreen) noexcept -> void;

        auto setExtent(const core::ExtentU& extent) noexcept -> void;

        auto lockMouse() noexcept -> void;
        auto unlockMouse() noexcept -> void;
        auto hideMouse() noexcept -> void;

        [[nodiscard]] auto extent() const noexcept -> const core::ExtentU&;
        [[nodiscard]] auto title() const noexcept -> const std::string&;

        [[nodiscard]] auto isOpen() const noexcept -> bool;
        [[nodiscard]] auto visible() const noexcept -> bool;

        [[nodiscard]] auto nativeHandle() const noexcept -> NativeHandle;

        [[nodiscard]] auto mouseLocked() const noexcept -> bool;
        [[nodiscard]] auto fullscreen() const noexcept -> bool;

        auto setKeyRepeatEnabled(bool enabled) noexcept -> void;
        [[nodiscard]] auto keyRepeatEnabled() const noexcept -> bool;

        auto setVirtualKeyboardVisible(bool visible) noexcept -> void;

        auto setMousePosition(const core::Position2i& position) noexcept -> void;
        static auto setMousePositionOnDesktop(const core::Position2u& position) noexcept -> void;

        [[nodiscard]] static auto getMonitorSettings() -> std::vector<Monitor>;
        [[nodiscard]] static auto getPrimaryMonitorSettings() -> Monitor;

        [[nodiscard]] static auto wm() noexcept -> WM;

      private:
        details::WindowImplPimpl m_impl;
    };
    DECLARE_PTR_AND_REF(Window)
} // namespace stormkit::wsi

#include "Window.inl"
