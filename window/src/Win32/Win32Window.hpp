// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "../AbstractWindow.hpp"

/////////// - StormKit::core - ///////////
#include <vector>

/////////// - StormKit::core - ///////////
#include <storm/core/Platform.hpp>

/////////// - Win32API - ///////////
#include <Windows.h>

namespace storm::window::details {
    class STORMKIT_PRIVATE Win32Window final: public AbstractWindow {
      public:
        Win32Window();
        Win32Window(std::string title, const VideoSettings &settings, WindowStyle style);
        ~Win32Window() override;

        Win32Window(Win32Window &&) noexcept;
        Win32Window &operator=(Win32Window &&) noexcept;

        void create(std::string title, const VideoSettings &settings, WindowStyle style) override;
        void close() noexcept override;

        [[nodiscard]] bool pollEvent(storm::window::Event &event) noexcept override;
        [[nodiscard]] bool waitEvent(Event &event) noexcept override;

        void setTitle(std::string title) noexcept override;
        void setFullscreenEnabled(bool enabled) noexcept override;

        void lockMouse() noexcept override;
        void unlockMouse() noexcept override;

        void hideMouse() noexcept override;
        void unhideMouse() noexcept override;

        [[nodiscard]] const core::Extentu &size() const noexcept override;

        [[nodiscard]] bool isOpen() const noexcept override;
        [[nodiscard]] bool isVisible() const noexcept override;

        [[nodiscard]] storm::window::NativeHandle nativeHandle() const noexcept override;
        [[nodiscard]] HWND win32Handles() const noexcept { return m_window_handle; }

        [[nodiscard]] static std::vector<VideoSettings> getDesktopModes();
        [[nodiscard]] static VideoSettings getDesktopFullscreenSize();

        ALLOCATE_HELPERS(Win32Window)
      private:
        static inline constexpr const auto CLASS_NAME = L"Stormkit_Window";
        static LRESULT CALLBACK globalOnEvent(HWND handle,
                                              UINT message,
                                              WPARAM w_param,
                                              LPARAM l_param);

        void registerWindowClass();
        void processEvents(UINT message, WPARAM w_param, LPARAM l_param);

        bool m_is_open       = false;
        bool m_is_visible    = false;
        bool m_is_fullscreen = false;
        bool m_resizing      = false;
        bool m_mouse_inside  = false;

        mutable core::Extentu m_current_size = { 0u, 0u };
        core::Extentu m_last_size            = { 0u, 0u };

        HWND m_window_handle = nullptr;
    };
} // namespace storm::window::details
