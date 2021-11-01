// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::window::details {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto AbstractWindow::title() const noexcept -> std::string_view { return m_title; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto AbstractWindow::videoSettings() const noexcept -> const VideoSettings & {
        return m_video_settings;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto AbstractWindow::isMouseLocked() const noexcept -> bool { return m_mouse_locked; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto AbstractWindow::isInFullscreen() const noexcept -> bool { return m_fullscreen; }
} // namespace storm::window::details
