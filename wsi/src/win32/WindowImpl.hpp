// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <windows.h>

#include <vector>

#include <stormkit/core/Math.mpp>
#include <stormkit/core/Types.mpp>

#include <stormkit/wsi/Event.mpp>
#include <stormkit/wsi/Fwd.hpp>
#include <stormkit/wsi/Monitor.mpp>
#include <stormkit/wsi/Window.mpp>
#include <stormkit/wsi/WindowStyle.mpp>

#include "../common/WindowImplBase.mpp"

namespace stormkit::wsi::details::win32 {
    class WindowImpl: public WindowImplBase {
      public:
        WindowImpl(Window::WM wm);
        WindowImpl(Window::WM wm, std::string title, const core::ExtentU &size, WindowStyle style);
        ~WindowImpl();

        WindowImpl(const WindowImpl &) noexcept;
        auto operator=(const WindowImpl &) noexcept -> WindowImpl &;

        WindowImpl(WindowImpl &&) noexcept;
        auto operator=(WindowImpl &&) noexcept -> WindowImpl &;

        auto create(std::string title, const core::ExtentU &size, WindowStyle style) -> void;
        auto close() noexcept -> void;

        [[nodiscard]] auto pollEvent(Event &event) noexcept -> bool;
        [[nodiscard]] auto waitEvent(Event &event) noexcept -> bool;

        auto setTitle(std::string title) noexcept -> void;

        auto setSize(const core::ExtentU &size) noexcept -> void;

        auto setFullscreenEnabled(bool enabled) noexcept -> void;

        auto lockMouse() noexcept -> void;
        auto unlockMouse() noexcept -> void;

        auto hideMouse() noexcept -> void;
        auto unhideMouse() noexcept -> void;

        [[nodiscard]] auto size() const noexcept -> const core::ExtentU16 &;
        [[nodiscard]] auto title() const noexcept -> const std::string &;
        [[nodiscard]] auto videoSettings() const noexcept -> const Monitor &;

        [[nodiscard]] auto isOpen() const noexcept -> bool;
        [[nodiscard]] auto visible() const noexcept -> bool;

        [[nodiscard]] auto mouseLocked() const noexcept -> bool;
        [[nodiscard]] auto mouseHided() const noexcept -> bool;
        [[nodiscard]] auto fullscreen() const noexcept -> bool;

        auto setKeyRepeatEnabled(bool enabled) noexcept -> void;
        [[nodiscard]] auto keyRepeatEnabled() const noexcept -> bool;

        auto setVirtualKeyboardVisible(bool visible) noexcept -> void;

        auto setMousePosition(core::Position2i position) noexcept -> void;
        static auto setMousePositionOnDesktop(Window::WM wm, core::Position2u position) noexcept
            -> void;

        [[nodiscard]] static auto getMonitorSettings(Window::WM wm) -> std::vector<Monitor>;
        [[nodiscard]] static auto getPrimaryMonitorSettings(Window::WM wm) -> Monitor;

        [[nodiscard]] auto nativeHandle() const noexcept -> Window::NativeHandle;
        [[nodiscard]] auto win32Handles() const noexcept -> HWND;

      private:
        static auto CALLBACK globalOnEvent(HWND handle,
                                           UINT message,
                                           WPARAM w_param,
                                           LPARAM l_param) -> LRESULT;

        auto registerWindowClass() -> void;
        auto processEvents(UINT message, WPARAM w_param, LPARAM l_param) -> void;

        bool m_is_open       = false;
        bool m_is_visible    = false;
        bool m_is_fullscreen = false;
        bool m_resizing      = false;
        bool m_mouse_inside  = false;

        WindowStyle m_style;

        LONG m_last_style;
        LONG m_last_style_ex;
        mutable core::ExtentU16 m_current_size = { 0u, 0u };
        core::ExtentU16 m_last_size            = { 0u, 0u };
        core::Vector2u m_last_position         = { 0u, 0u };

        HWND m_window_handle = nullptr;

        std::string m_title;
        bool m_mouse_locked       = false;
        bool m_mouse_hided        = false;
        bool m_key_repeat_enabled = false;
    };
} // namespace stormkit::wsi::details::win32

namespace stormkit::wsi::details {
    class WindowImpl: public win32::WindowImpl {};
} // namespace stormkit::wsi::details

#if defined(STORMKIT_CXX20_MODULES)
}
#endif

#include "WindowImpl.inl"
