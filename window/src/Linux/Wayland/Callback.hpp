// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "WaylandWindow.hpp"

namespace storm::window::details {
    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_PRIVATE void registry_handler(void *data,
                                           wl_registry *registry,
                                           std::uint32_t id,
                                           const char *interface,
                                           std::uint32_t version) noexcept;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_PRIVATE void
        registry_remover_handler(void *data, wl_registry *registry, std::uint32_t id) noexcept;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_PRIVATE void
        surface_enter_handler(void *data, wl_surface *surface, wl_output *output) noexcept;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_PRIVATE void
        surface_leave_handler(void *data, wl_surface *surface, wl_output *output) noexcept;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_PRIVATE void toplevel_configure_handler(void *data,
                                                     xdg_toplevel *xdg_tl,
                                                     int32_t width,
                                                     int32_t height,
                                                     wl_array *states) noexcept;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_PRIVATE void toplevel_close_handler(void *data, xdg_toplevel *xdg_tl) noexcept;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_PRIVATE void
        surface_configure_handler(void *data, xdg_surface *surface, std::uint32_t serial) noexcept;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_PRIVATE void
        shell_ping_handler(void *data, xdg_wm_base *xdg_shell, std::uint32_t serial) noexcept;

    STORMKIT_PRIVATE void shell_surface_configure_handler(void *data,
                                                          wl_shell_surface *shell_surface,
                                                          std::uint32_t edges,
                                                          std::int32_t width,
                                                          std::int32_t height) noexcept;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_PRIVATE void shell_ping_handler(void *data,
                                             wl_shell_surface *shell_surface,
                                             std::uint32_t serial) noexcept;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_PRIVATE void relative_pointer_relative_motion_handler(void *data,
                                                                   zwp_relative_pointer_v1 *pointer,
                                                                   std::uint32_t time_hi,
                                                                   std::uint32_t time_lw,
                                                                   wl_fixed_t dx,
                                                                   wl_fixed_t dy,
                                                                   wl_fixed_t dx_unaccel,
                                                                   wl_fixed_t dy_unaccel) noexcept;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_PRIVATE void
        locked_pointer_locked_handler(void *data, zwp_locked_pointer_v1 *locked_pointer) noexcept;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_PRIVATE void
        locked_pointer_unlocked_handler(void *data, zwp_locked_pointer_v1 *locked_pointer) noexcept;
} // namespace storm::window::details
