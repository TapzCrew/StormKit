// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <pointer-constraints-unstable-v1.h>
#include <relative-pointer-unstable-v1.h>
#include <wayland-client.h>
#include <xdg-decoration-unstable-v1.h>
#include <xdg-shell.h>

/////////// - StormKit::core - ///////////
#include <stormkit/core/Types.hpp>

namespace stormkit::wsi::details::wayland {
    /////////////////////////////////////
    /////////////////////////////////////
    auto registryHandler(void *data,
                            wl_registry *registry,
                            core::UInt32 id,
                            const char *interface,
                            core::UInt32 version) noexcept -> void;

    /////////////////////////////////////
    /////////////////////////////////////
    auto registryRemoverHandler(void *data, wl_registry *registry, core::UInt32 id) noexcept
        -> void;

    /////////////////////////////////////
    /////////////////////////////////////
    auto surfaceEnterHandler(void *data, wl_surface *surface, wl_output *output) noexcept
        -> void;

    /////////////////////////////////////
    /////////////////////////////////////
    auto surfaceLeaveHandler(void *data, wl_surface *surface, wl_output *output) noexcept
        -> void;

    /////////////////////////////////////
    /////////////////////////////////////
    auto toplevelConfigureHandler(void *data,
                                    xdg_toplevel *xdg_tl,
                                    core::Int32 width,
                                    core::Int32 height,
                                    wl_array *states) noexcept -> void;

    /////////////////////////////////////
    /////////////////////////////////////
    auto toplevelCloseHandler(void *data, xdg_toplevel *xdg_tl) noexcept -> void;

    /////////////////////////////////////
    /////////////////////////////////////
    auto surfaceConfigureHandler(void *data, xdg_surface *surface, core::UInt32 serial) noexcept
        -> void;

    /////////////////////////////////////
    /////////////////////////////////////
    auto shellPingHandler(void *data, xdg_wm_base *xdg_shell, core::UInt32 serial) noexcept
        -> void;

    auto shellSurfaceConfigureHandler(void *data,
                                        wl_shell_surface *shell_surface,
                                        core::UInt32 edges,
                                        core::Int32 width,
                                        core::Int32 height) noexcept -> void;

    /////////////////////////////////////
    /////////////////////////////////////
    auto shellPingHandler(void *data,
                            wl_shell_surface *shell_surface,
                            core::UInt32 serial) noexcept -> void;

    /////////////////////////////////////
    /////////////////////////////////////
    auto relativePointerRelativeMotionHandler(void *data,
                                                zwp_relative_pointer_v1 *pointer,
                                                core::UInt32 time_hi,
                                                core::UInt32 time_lw,
                                                wl_fixed_t dx,
                                                wl_fixed_t dy,
                                                wl_fixed_t dx_unaccel,
                                                wl_fixed_t dy_unaccel) noexcept -> void;

    /////////////////////////////////////
    /////////////////////////////////////
    auto lockedPointerLockedHandler(void *data, zwp_locked_pointer_v1 *locked_pointer) noexcept
        -> void;

    /////////////////////////////////////
    /////////////////////////////////////
    auto lockedPointerUnlockedHandler(void *data,
                                        zwp_locked_pointer_v1 *locked_pointer) noexcept -> void;
} // namespace stormkit::wsi::details::wayland
