// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#if defined(STORMKIT_CXX20_MODULES)
module stormkitwindow - details.wayland.callbacks;

/////////// - StormKit::wsi - ///////////
import stormkit.wsi.details.wayland.log;
import stormkit.wsi.details.wayland.windowimpl;
#else
    /////////// - StormKit::wsi - ///////////
    #include "Callbacks.mpp"
    #include "Log.mpp"
    #include "WindowImpl.mpp"
#endif

namespace stormkit::wsi::details::wayland {
    /////////////////////////////////////
    /////////////////////////////////////
    auto outputGeometryHandler(void *data,
                               wl_output *output,
                               core::Int32 x,
                               core::Int32 y,
                               core::Int32 pwidth,
                               core::Int32 pheight,
                               core::Int32 subpixels,
                               const char *make,
                               const char *model,
                               core::Int32 transform) noexcept -> void;

    /////////////////////////////////////
    /////////////////////////////////////
    auto outputModeHandler(void *data,
                           wl_output *wl_output,
                           core::UInt32 flags,
                           core::Int32 width,
                           core::Int32 height,
                           core::Int32 refresh) noexcept -> void;

    /////////////////////////////////////
    /////////////////////////////////////
    auto outputDoneHandler(void *data, wl_output *wl_output) noexcept -> void;

    /////////////////////////////////////
    /////////////////////////////////////
    auto outputScaleHandler(void *data, wl_output *wl_output, core::Int32 factor) noexcept -> void;

    /////////////////////////////////////
    /////////////////////////////////////
    auto seatCapabilitiesHandler(void *data, wl_seat *seat, core::UInt32 capabilities) noexcept
        -> void;

    /////////////////////////////////////
    /////////////////////////////////////
    auto seatNameHandler(void *data, wl_seat *seat, const char *name) noexcept -> void;

    /////////////////////////////////////
    /////////////////////////////////////
    auto pointerEnterHandler(void *data,
                             wl_pointer *pointer,
                             core::UInt32 serial,
                             wl_surface *surface,
                             wl_fixed_t surface_x,
                             wl_fixed_t surface_y) noexcept -> void;

    /////////////////////////////////////
    /////////////////////////////////////
    auto pointerLeaveHandler(void *data,
                             wl_pointer *pointer,
                             core::UInt32 serial,
                             wl_surface *surface) noexcept -> void;

    /////////////////////////////////////
    /////////////////////////////////////
    auto pointerMotionHandler(void *data,
                              wl_pointer *pointer,
                              core::UInt32 time,
                              wl_fixed_t surface_x,
                              wl_fixed_t surface_y) noexcept -> void;

    /////////////////////////////////////
    /////////////////////////////////////
    auto pointerButtonHandler(void *data,
                              wl_pointer *pointer,
                              core::UInt32 serial,
                              core::UInt32 time,
                              core::UInt32 button,
                              core::UInt32 state) noexcept -> void;

    /////////////////////////////////////
    /////////////////////////////////////
    auto pointerAxisHandler(void *data,
                            wl_pointer *pointer,
                            core::UInt32 time,
                            core::UInt32 axis,
                            wl_fixed_t value) noexcept -> void;

    /////////////////////////////////////
    /////////////////////////////////////
    auto pointerFrameHandler(void *data, wl_pointer *pointer) noexcept -> void;

    /////////////////////////////////////
    /////////////////////////////////////
    auto pointerAxisSourceHandler(void *data,
                                  wl_pointer *pointer,
                                  core::UInt32 axis_source) noexcept -> void;

    /////////////////////////////////////
    /////////////////////////////////////
    auto pointerAxisStopHandler(void *data,
                                wl_pointer *pointer,
                                core::UInt32 time,
                                core::UInt32 axis) noexcept -> void;

    /////////////////////////////////////
    /////////////////////////////////////
    auto pointerAxisDiscreteHandler(void *data,
                                    wl_pointer *pointer,
                                    core::UInt32 axis,
                                    core::Int32 discrete) noexcept -> void;

    /////////////////////////////////////
    /////////////////////////////////////
    auto keyboardKeymapHandler(void *data,
                               wl_keyboard *keyboard,
                               core::UInt32 format,
                               core::Int32 fd,
                               core::UInt32 size) noexcept -> void;

    /////////////////////////////////////
    /////////////////////////////////////
    auto keyboardEnterHandler(void *data,
                              wl_keyboard *keyboard,
                              core::UInt32 serial,
                              wl_surface *surface,
                              wl_array *keys) noexcept -> void;

    /////////////////////////////////////
    /////////////////////////////////////
    auto keyboardLeaveHandler(void *data,
                              wl_keyboard *keyboard,
                              core::UInt32 serial,
                              wl_surface *surface) noexcept -> void;

    /////////////////////////////////////
    /////////////////////////////////////
    auto keyboardKeyHandler(void *data,
                            wl_keyboard *keyboard,
                            core::UInt32 serial,
                            core::UInt32 time,
                            core::UInt32 key,
                            core::UInt32 state) noexcept -> void;

    /////////////////////////////////////
    /////////////////////////////////////
    auto keyboardModifiersHandler(void *data,
                                  wl_keyboard *keyboard,
                                  core::UInt32 serial,
                                  core::UInt32 mods_depressed,
                                  core::UInt32 mods_latcher,
                                  core::UInt32 mods_locked,
                                  core::UInt32 group) noexcept -> void;

    /////////////////////////////////////
    /////////////////////////////////////
    auto keyboardRepeatInfoHandler(void *data,
                                   wl_keyboard *keyboard,
                                   core::Int32 rate,
                                   core::Int32 delay) noexcept -> void;

    static const auto stormkit_output_listener =
        wl_output_listener { .geometry = outputGeometryHandler,
                             .mode     = outputModeHandler,
                             .done     = outputDoneHandler,
                             .scale    = outputScaleHandler };

    static const auto stormkit_seat_listener =
        wl_seat_listener { .capabilities = seatCapabilitiesHandler, .name = seatNameHandler };

    static const auto stormkit_pointer_listener =
        wl_pointer_listener { .enter         = pointerEnterHandler,
                              .leave         = pointerLeaveHandler,
                              .motion        = pointerMotionHandler,
                              .button        = pointerButtonHandler,
                              .axis          = pointerAxisHandler,
                              .frame         = pointerFrameHandler,
                              .axis_source   = pointerAxisSourceHandler,
                              .axis_stop     = pointerAxisStopHandler,
                              .axis_discrete = pointerAxisDiscreteHandler };

    static const auto stormkit_keyboard_listener =
        wl_keyboard_listener { .keymap      = keyboardKeymapHandler,
                               .enter       = keyboardEnterHandler,
                               .leave       = keyboardLeaveHandler,
                               .key         = keyboardKeyHandler,
                               .modifiers   = keyboardModifiersHandler,
                               .repeat_info = keyboardRepeatInfoHandler };

    static const auto stormkit_touchscreen_listener = wl_touch_listener {};

    /////////////////////////////////////
    /////////////////////////////////////
    auto registryHandler(void *data,
                         wl_registry *registry,
                         core::UInt32 id,
                         const char *interface,
                         core::UInt32 version) noexcept -> void {
        auto &globals = *static_cast<Globals *>(data);

#define BIND(n, t, v) n.reset(static_cast<t *>(wl_registry_bind(registry, id, &t##_interface, v)));

        dlog("Wayland registry acquired {} (id: {}, version: {})", interface, id, version);

        const auto size = std::char_traits<char>::length(interface);

        const auto interface_name = std::string_view { interface, size };

        if (interface_name == wl_compositor_interface.name)
            BIND(globals.compositor, wl_compositor, 3)
        else if (interface_name == wl_output_interface.name) {
            auto &output = globals.outputs.emplace_back(
                static_cast<wl_output *>(wl_registry_bind(registry, id, &wl_output_interface, 2)));
            wl_output_add_listener(output.get(), &stormkit_output_listener, &globals);
        } else if (interface_name == xdg_wm_base_interface.name)
            BIND(globals.xdg_shell, xdg_wm_base, 1)
        else if (interface_name == zxdg_decoration_manager_v1_interface.name)
            BIND(globals.xdg_decoration_manager, zxdg_decoration_manager_v1, 1)
        else if (interface_name == wl_shell_interface.name)
            BIND(globals.wayland_shell, wl_shell, 1)
        else if (interface_name == wl_shm_interface.name)
            BIND(globals.shm, wl_shm, 1)
        else if (interface_name == wl_seat_interface.name) {
            BIND(globals.seat, wl_seat, 5)
            wl_seat_add_listener(globals.seat.get(), &stormkit_seat_listener, &globals);
        } else if (interface_name == zwp_pointer_constraints_v1_interface.name)
            BIND(globals.pointer_constraints, zwp_pointer_constraints_v1, 1)
        else if (interface_name == zwp_relative_pointer_manager_v1_interface.name)
            BIND(globals.relative_pointer_manager, zwp_relative_pointer_manager_v1, 1)
#undef BIND
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto registryRemoverHandler([[maybe_unused]] void *data,
                                [[maybe_unused]] wl_registry *registry,
                                core::UInt32 id) noexcept -> void {
        dlog("Wayland registry lost {}", id);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto outputGeometryHandler([[maybe_unused]] void *data,
                               [[maybe_unused]] wl_output *output,
                               core::Int32 x,
                               core::Int32 y,
                               core::Int32 pwidth,
                               core::Int32 pheight,
                               [[maybe_unused]] core::Int32 subpixels,
                               const char *make,
                               const char *model,
                               [[maybe_unused]] core::Int32 transform) noexcept -> void {
        dlog("Screen found! {} {} with size of {}x{}mm at {} {}",
             make,
             model,
             pwidth,
             pheight,
             x,
             y);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto outputModeHandler(void *data,
                           [[maybe_unused]] wl_output *wl_output,
                           core::UInt32 flags,
                           core::Int32 width,
                           core::Int32 height,
                           core::Int32 refresh) noexcept -> void {
        auto &globals = *static_cast<Globals *>(data);
        globals.video_settings.emplace_back(VideoSettings {
            .size = { core::as<core::UInt32>(width), core::as<core::UInt32>(height) } });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto outputDoneHandler([[maybe_unused]] void *data,
                           [[maybe_unused]] wl_output *wl_output) noexcept -> void {}

    /////////////////////////////////////
    /////////////////////////////////////
    auto outputScaleHandler(void *data,
                            [[maybe_unused]] wl_output *wl_output,
                            [[maybe_unused]] core::Int32 factor) noexcept -> void {
        if (data == nullptr) return;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto surfaceEnterHandler(void *data, wl_surface *surface, wl_output *output) noexcept -> void {
        auto *window = static_cast<WindowImpl *>(data);
        window->surfaceOutputEnter(surface, output);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto surfaceLeaveHandler([[maybe_unused]] void *data,
                             [[maybe_unused]] wl_surface *surface,
                             [[maybe_unused]] wl_output *output) noexcept -> void {}

    /////////////////////////////////////
    /////////////////////////////////////
    auto toplevelConfigureHandler(void *data,
                                  xdg_toplevel *xdg_tl,
                                  core::Int32 width,
                                  core::Int32 height,
                                  wl_array *states) noexcept -> void {
        auto *window = static_cast<WindowImpl *>(data);
        window->toplevelConfigure(xdg_tl, width, height, states);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto toplevelCloseHandler(void *data, xdg_toplevel *xdg_tl) noexcept -> void {
        auto *window = static_cast<WindowImpl *>(data);
        window->toplevelClose(xdg_tl);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto surfaceConfigureHandler(void *data, xdg_surface *surface, core::UInt32 serial) noexcept
        -> void {
        auto window = static_cast<WindowImpl *>(data);
        window->surfaceConfigure(surface, serial);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto shellPingHandler([[maybe_unused]] void *data,
                          xdg_wm_base *xdg_shell,
                          core::UInt32 serial) noexcept -> void {
        ilog("Ping received from shell");

        xdg_wm_base_pong(xdg_shell, serial);
    }

    auto shellSurfaceConfigureHandler(void *data,
                                      wl_shell_surface *shell_surface,
                                      core::UInt32 edges,
                                      core::Int32 width,
                                      core::Int32 height) noexcept -> void {
        auto *window = static_cast<WindowImpl *>(data);
        window->shellSurfaceConfigure(shell_surface, edges, width, height);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto shellPingHandler([[maybe_unused]] void *data,
                          wl_shell_surface *shell_surface,
                          core::UInt32 serial) noexcept -> void {
        ilog("Ping received from shell");

        wl_shell_surface_pong(shell_surface, serial);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto seatCapabilitiesHandler(void *data, wl_seat *seat, core::UInt32 capabilities) noexcept
        -> void {
        auto &globals = *static_cast<Globals *>(data);

        if ((capabilities & WL_SEAT_CAPABILITY_KEYBOARD) > 0) {
            auto &keyboard =
                globals.keyboards.emplace_back(wl_seat_get_keyboard(globals.seat.get()));
            wl_keyboard_add_listener(keyboard.get(), &stormkit_keyboard_listener, nullptr);
        }

        if ((capabilities & WL_SEAT_CAPABILITY_POINTER) > 0) {
            auto &pointer = globals.pointers.emplace_back(wl_seat_get_pointer(globals.seat.get()));
            wl_pointer_add_listener(pointer.get(), &stormkit_pointer_listener, nullptr);
        }

        if ((capabilities & WL_SEAT_CAPABILITY_TOUCH) > 0) {
            globals.touchscreens.emplace_back(wl_seat_get_touch(globals.seat.get()));
            // wl_touch_add_listener(touchscreen, &stormkit_touchscreen_listener, this);
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto seatNameHandler([[maybe_unused]] void *data, wl_seat *seat, const char *name) noexcept
        -> void {
        dlog("WL Seat found! {}", name);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto pointerEnterHandler(void *data,
                             wl_pointer *pointer,
                             core::UInt32 serial,
                             wl_surface *surface,
                             wl_fixed_t surface_x,
                             wl_fixed_t surface_y) noexcept -> void {
        if (data == nullptr) return;

        auto *window = static_cast<WindowImpl *>(data);
        window->pointerEnter(pointer, serial, surface, surface_x, surface_y);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto pointerLeaveHandler(void *data,
                             wl_pointer *pointer,
                             core::UInt32 serial,
                             wl_surface *surface) noexcept -> void {
        if (data == nullptr) return;

        auto *window = static_cast<WindowImpl *>(data);
        window->pointerLeave(pointer, serial, surface);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto pointerMotionHandler(void *data,
                              wl_pointer *pointer,
                              core::UInt32 time,
                              wl_fixed_t surface_x,
                              wl_fixed_t surface_y) noexcept -> void {
        if (data == nullptr) return;

        auto *window = static_cast<WindowImpl *>(data);
        window->pointerMotion(pointer, time, surface_x, surface_y);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto pointerButtonHandler(void *data,
                              wl_pointer *pointer,
                              core::UInt32 serial,
                              core::UInt32 time,
                              core::UInt32 button,
                              core::UInt32 state) noexcept -> void {
        if (data == nullptr) return;

        auto *window = static_cast<WindowImpl *>(data);
        window->pointerButton(pointer, serial, time, button, state);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto pointerAxisHandler([[maybe_unused]] void *data,
                            [[maybe_unused]] wl_pointer *pointer,
                            [[maybe_unused]] core::UInt32 time,
                            [[maybe_unused]] core::UInt32 axis,
                            [[maybe_unused]] wl_fixed_t value) noexcept -> void {}

    /////////////////////////////////////
    /////////////////////////////////////
    auto pointerFrameHandler([[maybe_unused]] void *data,
                             [[maybe_unused]] wl_pointer *pointer) noexcept -> void {}

    /////////////////////////////////////
    /////////////////////////////////////
    auto pointerAxisSourceHandler([[maybe_unused]] void *data,
                                  [[maybe_unused]] wl_pointer *pointer,
                                  [[maybe_unused]] core::UInt32 axis_source) noexcept -> void {}

    /////////////////////////////////////
    /////////////////////////////////////
    auto pointerAxisStopHandler([[maybe_unused]] void *data,
                                [[maybe_unused]] wl_pointer *pointer,
                                [[maybe_unused]] core::UInt32 time,
                                [[maybe_unused]] core::UInt32 axis) noexcept -> void {}

    /////////////////////////////////////
    /////////////////////////////////////
    auto pointerAxisDiscreteHandler([[maybe_unused]] void *data,
                                    [[maybe_unused]] wl_pointer *pointer,
                                    [[maybe_unused]] core::UInt32 axis,
                                    [[maybe_unused]] core::Int32 discrete) noexcept -> void {}

    /////////////////////////////////////
    /////////////////////////////////////
    auto keyboardKeymapHandler(void *data,
                               wl_keyboard *keyboard,
                               core::UInt32 format,
                               core::Int32 fd,
                               core::UInt32 size) noexcept -> void {
        if (data == nullptr) return;

        auto *window = static_cast<WindowImpl *>(data);
        window->keyboardKeymap(keyboard, format, fd, size);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto keyboardEnterHandler(void *data,
                              wl_keyboard *keyboard,
                              core::UInt32 serial,
                              wl_surface *surface,
                              wl_array *keys) noexcept -> void {
        if (data == nullptr) return;

        auto *window = static_cast<WindowImpl *>(data);
        window->keyboardEnter(keyboard, serial, surface, keys);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto keyboardLeaveHandler(void *data,
                              wl_keyboard *keyboard,
                              core::UInt32 serial,
                              wl_surface *surface) noexcept -> void {
        if (data == nullptr) return;

        auto *window = static_cast<WindowImpl *>(data);
        window->keyboardLeave(keyboard, serial, surface);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto keyboardKeyHandler(void *data,
                            wl_keyboard *keyboard,
                            core::UInt32 serial,
                            core::UInt32 time,
                            core::UInt32 key,
                            core::UInt32 state) noexcept -> void {
        if (data == nullptr) return;

        auto *window = static_cast<WindowImpl *>(data);
        window->keyboardKey(keyboard, serial, time, key, state);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto keyboardModifiersHandler(void *data,
                                  wl_keyboard *keyboard,
                                  core::UInt32 serial,
                                  core::UInt32 mods_depressed,
                                  core::UInt32 mods_latcher,
                                  core::UInt32 mods_locked,
                                  core::UInt32 group) noexcept -> void {
        if (data == nullptr) return;

        auto *window = static_cast<WindowImpl *>(data);
        window
            ->keyboardModifiers(keyboard, serial, mods_depressed, mods_latcher, mods_locked, group);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto keyboardRepeatInfoHandler(void *data,
                                   wl_keyboard *keyboard,
                                   core::Int32 rate,
                                   core::Int32 delay) noexcept -> void {
        if (data == nullptr) return;

        auto *window = static_cast<WindowImpl *>(data);
        window->keyboardRepeatInfo(keyboard, rate, delay);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto relativePointerRelativeMotionHandler(void *data,
                                              zwp_relative_pointer_v1 *pointer,
                                              core::UInt32 time_hi,
                                              core::UInt32 time_lw,
                                              wl_fixed_t dx,
                                              wl_fixed_t dy,
                                              wl_fixed_t dx_unaccel,
                                              wl_fixed_t dy_unaccel) noexcept -> void {
        auto *window = static_cast<WindowImpl *>(data);
        window->relativePointerRelativeMotion(pointer,
                                              time_hi,
                                              time_lw,
                                              dx,
                                              dy,
                                              dx_unaccel,
                                              dy_unaccel);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto lockedPointerLockedHandler([[maybe_unused]] void *data,
                                    [[maybe_unused]] zwp_locked_pointer_v1 *locked_pointer) noexcept
        -> void {}

    /////////////////////////////////////
    /////////////////////////////////////
    auto lockedPointerUnlockedHandler(
        [[maybe_unused]] void *data,
        [[maybe_unused]] zwp_locked_pointer_v1 *locked_pointer) noexcept -> void {}
} // namespace stormkit::wsi::details::wayland
