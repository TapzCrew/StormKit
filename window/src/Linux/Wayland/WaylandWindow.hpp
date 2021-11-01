// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "../../AbstractWindow.hpp"
#include "../Utils.hpp"

/////////// - STL - ///////////
#include <functional>
#include <memory>

/////////// - StormKit::core - ///////////
#include <storm/core/Platform.hpp>

/////////// - Wayland - ///////////
#include "pointer-constraints-unstable-v1.h"
#include "relative-pointer-unstable-v1.h"
#include "xdg-decoration-unstable-v1.h"
#include "xdg-shell.h"
#include <wayland-client.h>
#include <wayland-cursor.h>

/////////// - XKB - ///////////
#include <xkbcommon/xkbcommon.h>

namespace storm::window::details {
    // Base
    STORMKIT_RAII_CAPSULE(WLDisplay, wl_display, wl_display_disconnect)
    STORMKIT_RAII_CAPSULE(WLRegistry, wl_registry, wl_registry_destroy)
    STORMKIT_RAII_CAPSULE(WLCompositor, wl_compositor, wl_compositor_destroy)
    STORMKIT_RAII_CAPSULE(WLShm, wl_shm, wl_shm_destroy)
    STORMKIT_RAII_CAPSULE(WLShmPool, wl_shm_pool, wl_shm_pool_destroy)
    STORMKIT_RAII_CAPSULE(WLSeat, wl_seat, wl_seat_release)
    STORMKIT_RAII_CAPSULE(WLOutput, wl_output, wl_output_release)

    // Base Surface
    STORMKIT_RAII_CAPSULE(WLSurface, wl_surface, wl_surface_destroy)

    // Cursor
    STORMKIT_RAII_CAPSULE(WLCursorTheme, wl_cursor_theme, wl_cursor_theme_destroy)

    // WL_Shell
    STORMKIT_RAII_CAPSULE(WLShell, wl_shell, wl_shell_destroy)
    STORMKIT_RAII_CAPSULE(WLShellSurface, wl_shell_surface, wl_shell_surface_destroy)

    // XDG
    STORMKIT_RAII_CAPSULE(XDGShell, xdg_wm_base, xdg_wm_base_destroy)
    STORMKIT_RAII_CAPSULE(XDGSurface, xdg_surface, xdg_surface_destroy)
    STORMKIT_RAII_CAPSULE(XDGTopLevel, xdg_toplevel, xdg_toplevel_destroy)
    STORMKIT_RAII_CAPSULE(XDGDecorationManager,
                          zxdg_decoration_manager_v1,
                          zxdg_decoration_manager_v1_destroy)

    // Events
    STORMKIT_RAII_CAPSULE(WLPointer, wl_pointer, wl_pointer_release)
    STORMKIT_RAII_CAPSULE(WLKeyboard, wl_keyboard, wl_keyboard_release)
    STORMKIT_RAII_CAPSULE(WLTouch, wl_touch, wl_touch_release)

    // WP
    STORMKIT_RAII_CAPSULE(WPRelativePointerManager,
                          zwp_relative_pointer_manager_v1,
                          zwp_relative_pointer_manager_v1_destroy)
    STORMKIT_RAII_CAPSULE(WPPointerConstraints,
                          zwp_pointer_constraints_v1,
                          zwp_pointer_constraints_v1_destroy);
    STORMKIT_RAII_CAPSULE(WPLockedPointer, zwp_locked_pointer_v1, zwp_locked_pointer_v1_destroy)
    STORMKIT_RAII_CAPSULE(WPRelativePointer,
                          zwp_relative_pointer_v1,
                          zwp_relative_pointer_v1_destroy)

    // Fake Buffer
    STORMKIT_RAII_CAPSULE(WLBuffer, wl_buffer, wl_buffer_destroy)

    struct Globals {
        WLDisplayScoped display;
        WLRegistryScoped registry;
        WLCompositorScoped compositor;
        std::vector<WLOutputScoped> outputs;

        XDGShellScoped xdg_shell;
        XDGDecorationManagerScoped xdg_decoration_manager;

        WLShellScoped wayland_shell;

        WLShmScoped shm;
        WLSeatScoped seat;
        std::vector<WLPointerScoped> pointers;
        std::vector<WLKeyboardScoped> keyboards;
        std::vector<WLTouchScoped> touchscreens;
        WPPointerConstraintsScoped pointer_constraints;
        WPRelativePointerManagerScoped relative_pointer_manager;

        std::vector<VideoSettings> video_settings;
    };

    class STORMKIT_PRIVATE WaylandWindow final: public AbstractWindow {
      public:
        struct Handles {
            wl_display *display;
            wl_surface *surface;
        };

        struct KeyState {
            xkb_keysym_t key;
            bool down;
        };

        struct MouseState {
            struct MouseButtonState {
                std::uint32_t button;
                bool down;
            };

            std::array<MouseButtonState, 5> button_state;

            core::Vector2i position_in_window;
        };

        WaylandWindow();
        WaylandWindow(std::string title,
                      const VideoSettings &settings,
                      storm::window::WindowStyle style);
        ~WaylandWindow() override;

        WaylandWindow(WaylandWindow &&) noexcept;
        WaylandWindow &operator=(WaylandWindow &&) noexcept;

        void create(std::string title, const VideoSettings &settings, WindowStyle style) override;
        void close() noexcept override;

        [[nodiscard]] bool pollEvent(Event &event) noexcept override;
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

        [[nodiscard]] NativeHandle nativeHandle() const noexcept override;
        [[nodiscard]] const Handles &waylandHandles() const noexcept { return m_handles; }

        void surfaceOutputEnter(wl_surface *surface, wl_output *output) noexcept;

        void surfaceConfigure(xdg_surface *surface, std::uint32_t serial) noexcept;

        void toplevelConfigure(xdg_toplevel *xdg_tl,
                               std::int32_t width,
                               std::int32_t height,
                               wl_array *state) noexcept;
        void toplevelClose(xdg_toplevel *xdg_tl) noexcept;

        void shellSurfaceConfigure(wl_shell_surface *xdg_tl,
                                   std::uint32_t edges,
                                   std::int32_t width,
                                   std::int32_t height) noexcept;

        void seatCapabilities(wl_seat *seat, std::uint32_t capabilities) noexcept;

        void pointerEnter(wl_pointer *pointer,
                          std::uint32_t serial,
                          wl_surface *surface,
                          wl_fixed_t surface_x,
                          wl_fixed_t surface_y) noexcept;
        void pointerLeave(wl_pointer *pointer, std::uint32_t serial, wl_surface *surface) noexcept;
        void pointerMotion(wl_pointer *pointer,
                           std::uint32_t time,
                           wl_fixed_t surface_x,
                           wl_fixed_t surface_y) noexcept;
        void pointerButton(wl_pointer *pointer,
                           std::uint32_t serial,
                           std::uint32_t time,
                           std::uint32_t button,
                           std::uint32_t state) noexcept;

        void keyboardKeymap(wl_keyboard *keyboard,
                            std::uint32_t format,
                            std::int32_t fd,
                            std::uint32_t size) noexcept;
        void keyboardEnter(wl_keyboard *keyboard,
                           std::uint32_t serial,
                           wl_surface *surface,
                           wl_array *keys) noexcept;
        void keyboardLeave(wl_keyboard *keyboard,
                           std::uint32_t serial,
                           wl_surface *surface) noexcept;
        void keyboardKey(wl_keyboard *keyboard,
                         std::uint32_t serial,
                         std::uint32_t time,
                         std::uint32_t key,
                         std::uint32_t state) noexcept;
        void keyboardModifiers(wl_keyboard *keyboard,
                               std::uint32_t serial,
                               std::uint32_t mods_depressed,
                               std::uint32_t mods_latched,
                               std::uint32_t mods_locked,
                               std::uint32_t group) noexcept;
        void keyboardRepeatInfo(wl_keyboard *keyboard,
                                std::int32_t rate,
                                std::int32_t delay) noexcept;

        void relativePointerRelativeMotion(zwp_relative_pointer_v1 *pointer,
                                           std::uint32_t time_hi,
                                           std::uint32_t time_lw,
                                           wl_fixed_t dx,
                                           wl_fixed_t dy,
                                           wl_fixed_t dx_unaccel,
                                           wl_fixed_t dy_unaccel) noexcept;

        [[nodiscard]] core::span<const KeyState> keyStates() const noexcept {
            return m_keyboard_state;
        }
        [[nodiscard]] const MouseState &mouseState() const noexcept { return m_mouse_state; }

        void setMousePosition(core::Vector2i position) const noexcept;

        [[nodiscard]] static std::vector<VideoSettings> getDesktopModes();
        [[nodiscard]] static VideoSettings getDesktopFullscreenSize();

        ALLOCATE_HELPERS(WaylandWindow)
      private:
        void createXDGShell() noexcept;
        void createWaylandShell() noexcept;
        void createPixelBuffer() noexcept;
        void updateKeymap(std::string_view keymap_string) noexcept;
        void updateXKBMods() noexcept;

        // Base Surface
        wl_output *m_current_output;
        WLSurfaceScoped m_surface;
        WLShmPoolScoped m_shm_pool;

        // Cursor
        WLCursorThemeScoped m_cursor_theme;
        WLSurfaceScoped m_cursor_surface;
        WLBufferScoped m_cursor_buffer;

        // WL_Shell
        WLShellSurfaceScoped m_wlshell_surface;

        // XDG
        XDGSurfaceScoped m_xdg_surface;
        XDGTopLevelScoped m_xdg_toplevel;

        // Events
        wl_pointer *m_pointer          = nullptr;
        std::uint32_t m_pointer_serial = 0u;
        wl_keyboard *m_keyboard        = nullptr;
        wl_touch *m_touchscreen        = nullptr;

        // WP
        WPLockedPointerScoped m_locked_pointer;
        WPRelativePointerScoped m_relative_pointer;

        // Fake Buffer
        WLBufferScoped m_buffer;

        // XKB
        XKBContextScoped m_xkb_context;
        XKBKeymapScoped m_xkb_keymap;
        XKBStateScoped m_xkb_state;

        struct XKBMods {
            xkb_mod_index_t shift;
            xkb_mod_index_t lock;
            xkb_mod_index_t control;
            xkb_mod_index_t mod1;
            xkb_mod_index_t mod2;
            xkb_mod_index_t mod3;
            xkb_mod_index_t mod4;
            xkb_mod_index_t mod5;
        } m_xkb_mods;

        Handles m_handles;

        bool m_opened                    = false;
        bool m_visible                   = false;
        core::Extentu m_fullscren_extent = {};
        core::Extentu m_extent           = {};

        std::array<KeyState, 102> m_keyboard_state;

        MouseState m_mouse_state;

        bool m_configured = false;

        friend class WaylandInputHandler;
    };
    DECLARE_PTR_AND_REF(WaylandWindow)
} // namespace storm::window::details
