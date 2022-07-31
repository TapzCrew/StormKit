// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::wsi::details::x11 {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::title() const noexcept -> const std::string& {
        return m_title;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::mouseLocked() const noexcept -> bool {
        return m_mouse_locked;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::mouseHidden() const noexcept -> bool {
        return m_mouse_hided;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::fullscreen() const noexcept -> bool {
        return m_fullscreen;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::isOpen() const noexcept -> bool {
        return m_open;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::visible() const noexcept -> bool {
        return m_visible;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::xcbHandles() const noexcept -> const Handles& {
        return m_handles;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::extent() const noexcept -> const core::ExtentU& {
        return m_extent;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::keyRepeatEnabled() const noexcept -> bool {
        return m_key_repeat_enabled;
    }
} // namespace stormkit::wsi::details::x11
