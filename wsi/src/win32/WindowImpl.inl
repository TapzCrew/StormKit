// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::wsi::details::win32 {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::close() noexcept -> void {
        m_is_open    = false;
        m_is_visible = false;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::title() const noexcept -> const std::string& {
        return m_title;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::isOpen() const noexcept -> bool {
        return m_is_open;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::visible() const noexcept -> bool {
        return m_is_visible;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::mouseLocked() const noexcept -> bool {
        return m_mouse_locked;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::mouseHided() const noexcept -> bool {
        return m_mouse_hided;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::fullscreen() const noexcept -> bool {
        return m_is_fullscreen;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::keyRepeatEnabled() const noexcept -> bool {
        return m_key_repeat_enabled;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::nativeHandle() const noexcept -> Window::NativeHandle {
        return static_cast<Window::NativeHandle>(win32Handles());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::win32Handles() const noexcept -> HWND {
        return m_window_handle;
    }
} // namespace stormkit::wsi::details::win32
