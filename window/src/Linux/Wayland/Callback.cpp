#include "Callback.hpp"
#include "Log.hpp"

namespace storm::window::details {
    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_PRIVATE void output_geometry_handler(void *data,
                                                  wl_output *output,
                                                  std::int32_t x,
                                                  std::int32_t y,
                                                  std::int32_t pwidth,
                                                  std::int32_t pheight,
                                                  std::int32_t subpixels,
                                                  const char *make,
                                                  const char *model,
                                                  std::int32_t transform) noexcept;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_PRIVATE void output_mode_handler(void *data,
                                              wl_output *wl_output,
                                              std::uint32_t flags,
                                              std::int32_t width,
                                              std::int32_t height,
                                              std::int32_t refresh) noexcept;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_PRIVATE void output_done_handler(void *data, wl_output *wl_output) noexcept;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_PRIVATE void
        output_scale_handler(void *data, wl_output *wl_output, std::int32_t factor) noexcept;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_PRIVATE void
        seat_capabilities_handler(void *data, wl_seat *seat, std::uint32_t capabilities) noexcept;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_PRIVATE void seat_name_handler(void *data, wl_seat *seat, const char *name) noexcept;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_PRIVATE void pointer_enter_handler(void *data,
                                                wl_pointer *pointer,
                                                std::uint32_t serial,
                                                wl_surface *surface,
                                                wl_fixed_t surface_x,
                                                wl_fixed_t surface_y) noexcept;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_PRIVATE void pointer_leave_handler(void *data,
                                                wl_pointer *pointer,
                                                std::uint32_t serial,
                                                wl_surface *surface) noexcept;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_PRIVATE void pointer_motion_handler(void *data,
                                                 wl_pointer *pointer,
                                                 std::uint32_t time,
                                                 wl_fixed_t surface_x,
                                                 wl_fixed_t surface_y) noexcept;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_PRIVATE void pointer_button_handler(void *data,
                                                 wl_pointer *pointer,
                                                 std::uint32_t serial,
                                                 std::uint32_t time,
                                                 std::uint32_t button,
                                                 std::uint32_t state) noexcept;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_PRIVATE void pointer_axis_handler(void *data,
                                               wl_pointer *pointer,
                                               std::uint32_t time,
                                               std::uint32_t axis,
                                               wl_fixed_t value) noexcept;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_PRIVATE void pointer_frame_handler(void *data, wl_pointer *pointer) noexcept;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_PRIVATE void pointer_axis_source_handler(void *data,
                                                      wl_pointer *pointer,
                                                      std::uint32_t axis_source) noexcept;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_PRIVATE void pointer_axis_stop_handler(void *data,
                                                    wl_pointer *pointer,
                                                    std::uint32_t time,
                                                    std::uint32_t axis) noexcept;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_PRIVATE void pointer_axis_discrete_handler(void *data,
                                                        wl_pointer *pointer,
                                                        std::uint32_t axis,
                                                        std::int32_t discrete) noexcept;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_PRIVATE void keyboard_keymap_handler(void *data,
                                                  wl_keyboard *keyboard,
                                                  std::uint32_t format,
                                                  std::int32_t fd,
                                                  std::uint32_t size) noexcept;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_PRIVATE void keyboard_enter_handler(void *data,
                                                 wl_keyboard *keyboard,
                                                 std::uint32_t serial,
                                                 wl_surface *surface,
                                                 wl_array *keys) noexcept;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_PRIVATE void keyboard_leave_handler(void *data,
                                                 wl_keyboard *keyboard,
                                                 std::uint32_t serial,
                                                 wl_surface *surface) noexcept;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_PRIVATE void keyboard_key_handler(void *data,
                                               wl_keyboard *keyboard,
                                               std::uint32_t serial,
                                               std::uint32_t time,
                                               std::uint32_t key,
                                               std::uint32_t state) noexcept;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_PRIVATE void keyboard_modifiers_handler(void *data,
                                                     wl_keyboard *keyboard,
                                                     std::uint32_t serial,
                                                     std::uint32_t mods_depressed,
                                                     std::uint32_t mods_latcher,
                                                     std::uint32_t mods_locked,
                                                     std::uint32_t group) noexcept;

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_PRIVATE void keyboard_repeat_info_handler(void *data,
                                                       wl_keyboard *keyboard,
                                                       std::int32_t rate,
                                                       std::int32_t delay) noexcept;

    static const auto stormkit_output_listener =
        wl_output_listener { .geometry = output_geometry_handler,
                             .mode     = output_mode_handler,
                             .done     = output_done_handler,
                             .scale    = output_scale_handler };

    static const auto stormkit_seat_listener =
        wl_seat_listener { .capabilities = seat_capabilities_handler, .name = seat_name_handler };

    static const auto stormkit_pointer_listener =
        wl_pointer_listener { .enter         = pointer_enter_handler,
                              .leave         = pointer_leave_handler,
                              .motion        = pointer_motion_handler,
                              .button        = pointer_button_handler,
                              .axis          = pointer_axis_handler,
                              .frame         = pointer_frame_handler,
                              .axis_source   = pointer_axis_source_handler,
                              .axis_stop     = pointer_axis_stop_handler,
                              .axis_discrete = pointer_axis_discrete_handler };

    static const auto stormkit_keyboard_listener =
        wl_keyboard_listener { .keymap      = keyboard_keymap_handler,
                               .enter       = keyboard_enter_handler,
                               .leave       = keyboard_leave_handler,
                               .key         = keyboard_key_handler,
                               .modifiers   = keyboard_modifiers_handler,
                               .repeat_info = keyboard_repeat_info_handler };

    static const auto stormkit_touchscreen_listener = wl_touch_listener {};

    /////////////////////////////////////
    /////////////////////////////////////
    auto registry_handler(void *data,
                          wl_registry *registry,
                          std::uint32_t id,
                          const char *interface,
                          std::uint32_t version) noexcept -> void {
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
    auto registry_remover_handler([[maybe_unused]] void *data,
                                  [[maybe_unused]] wl_registry *registry,
                                  std::uint32_t id) noexcept -> void {
        dlog("Wayland registry lost {}", id);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto output_geometry_handler([[maybe_unused]] void *data,
                                 [[maybe_unused]] wl_output *output,
                                 std::int32_t x,
                                 std::int32_t y,
                                 std::int32_t pwidth,
                                 std::int32_t pheight,
                                 [[maybe_unused]] std::int32_t subpixels,
                                 const char *make,
                                 const char *model,
                                 [[maybe_unused]] std::int32_t transform) noexcept -> void {
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
    auto output_mode_handler(void *data,
                             [[maybe_unused]] wl_output *wl_output,
                             std::uint32_t flags,
                             std::int32_t width,
                             std::int32_t height,
                             std::int32_t refresh) noexcept -> void {
        auto &globals = *static_cast<Globals *>(data);
        globals.video_settings.emplace_back(
            VideoSettings { .size = { gsl::narrow_cast<core::UInt32>(width),
                                      gsl::narrow_cast<core::UInt32>(height) } });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto output_done_handler([[maybe_unused]] void *data,
                             [[maybe_unused]] wl_output *wl_output) noexcept -> void {}

    /////////////////////////////////////
    /////////////////////////////////////
    auto output_scale_handler(void *data,
                              [[maybe_unused]] wl_output *wl_output,
                              [[maybe_unused]] std::int32_t factor) noexcept -> void {
        if (data == nullptr) return;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto surface_enter_handler(void *data, wl_surface *surface, wl_output *output) noexcept
        -> void {
        auto *window = static_cast<WaylandWindow *>(data);
        window->surfaceOutputEnter(surface, output);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto surface_leave_handler([[maybe_unused]] void *data,
                               [[maybe_unused]] wl_surface *surface,
                               [[maybe_unused]] wl_output *output) noexcept -> void {}

    /////////////////////////////////////
    /////////////////////////////////////
    auto toplevel_configure_handler(void *data,
                                    xdg_toplevel *xdg_tl,
                                    int32_t width,
                                    int32_t height,
                                    wl_array *states) noexcept -> void {
        auto *window = static_cast<WaylandWindow *>(data);
        window->toplevelConfigure(xdg_tl, width, height, states);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto toplevel_close_handler(void *data, xdg_toplevel *xdg_tl) noexcept -> void {
        auto *window = static_cast<WaylandWindow *>(data);
        window->toplevelClose(xdg_tl);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto surface_configure_handler(void *data, xdg_surface *surface, std::uint32_t serial) noexcept
        -> void {
        auto *window = static_cast<WaylandWindow *>(data);
        window->surfaceConfigure(surface, serial);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto shell_ping_handler([[maybe_unused]] void *data,
                            xdg_wm_base *xdg_shell,
                            std::uint32_t serial) noexcept -> void {
        ilog("Ping received from shell");

        xdg_wm_base_pong(xdg_shell, serial);
    }

    auto shell_surface_configure_handler(void *data,
                                         wl_shell_surface *shell_surface,
                                         std::uint32_t edges,
                                         std::int32_t width,
                                         std::int32_t height) noexcept -> void {
        auto *window = static_cast<WaylandWindow *>(data);
        window->shellSurfaceConfigure(shell_surface, edges, width, height);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto shell_ping_handler([[maybe_unused]] void *data,
                            wl_shell_surface *shell_surface,
                            std::uint32_t serial) noexcept -> void {
        ilog("Ping received from shell");

        wl_shell_surface_pong(shell_surface, serial);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto seat_capabilities_handler(void *data, wl_seat *seat, std::uint32_t capabilities) noexcept
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
    auto seat_name_handler([[maybe_unused]] void *data, wl_seat *seat, const char *name) noexcept
        -> void {
        dlog("WL Seat found! {}", name);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto pointer_enter_handler(void *data,
                               wl_pointer *pointer,
                               std::uint32_t serial,
                               wl_surface *surface,
                               wl_fixed_t surface_x,
                               wl_fixed_t surface_y) noexcept -> void {
        if (data == nullptr) return;

        auto *window = static_cast<WaylandWindow *>(data);
        window->pointerEnter(pointer, serial, surface, surface_x, surface_y);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto pointer_leave_handler(void *data,
                               wl_pointer *pointer,
                               std::uint32_t serial,
                               wl_surface *surface) noexcept -> void {
        if (data == nullptr) return;

        auto *window = static_cast<WaylandWindow *>(data);
        window->pointerLeave(pointer, serial, surface);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto pointer_motion_handler(void *data,
                                wl_pointer *pointer,
                                std::uint32_t time,
                                wl_fixed_t surface_x,
                                wl_fixed_t surface_y) noexcept -> void {
        if (data == nullptr) return;

        auto *window = static_cast<WaylandWindow *>(data);
        window->pointerMotion(pointer, time, surface_x, surface_y);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto pointer_button_handler(void *data,
                                wl_pointer *pointer,
                                std::uint32_t serial,
                                std::uint32_t time,
                                std::uint32_t button,
                                std::uint32_t state) noexcept -> void {
        if (data == nullptr) return;

        auto *window = static_cast<WaylandWindow *>(data);
        window->pointerButton(pointer, serial, time, button, state);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto pointer_axis_handler([[maybe_unused]] void *data,
                              [[maybe_unused]] wl_pointer *pointer,
                              [[maybe_unused]] std::uint32_t time,
                              [[maybe_unused]] std::uint32_t axis,
                              [[maybe_unused]] wl_fixed_t value) noexcept -> void {}

    /////////////////////////////////////
    /////////////////////////////////////
    auto pointer_frame_handler([[maybe_unused]] void *data,
                               [[maybe_unused]] wl_pointer *pointer) noexcept -> void {}

    /////////////////////////////////////
    /////////////////////////////////////
    auto pointer_axis_source_handler([[maybe_unused]] void *data,
                                     [[maybe_unused]] wl_pointer *pointer,
                                     [[maybe_unused]] std::uint32_t axis_source) noexcept -> void {}

    /////////////////////////////////////
    /////////////////////////////////////
    auto pointer_axis_stop_handler([[maybe_unused]] void *data,
                                   [[maybe_unused]] wl_pointer *pointer,
                                   [[maybe_unused]] std::uint32_t time,
                                   [[maybe_unused]] std::uint32_t axis) noexcept -> void {}

    /////////////////////////////////////
    /////////////////////////////////////
    auto pointer_axis_discrete_handler([[maybe_unused]] void *data,
                                       [[maybe_unused]] wl_pointer *pointer,
                                       [[maybe_unused]] std::uint32_t axis,
                                       [[maybe_unused]] std::int32_t discrete) noexcept -> void {}

    /////////////////////////////////////
    /////////////////////////////////////
    auto keyboard_keymap_handler(void *data,
                                 wl_keyboard *keyboard,
                                 std::uint32_t format,
                                 std::int32_t fd,
                                 std::uint32_t size) noexcept -> void {
        if (data == nullptr) return;

        auto *window = static_cast<WaylandWindow *>(data);
        window->keyboardKeymap(keyboard, format, fd, size);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto keyboard_enter_handler(void *data,
                                wl_keyboard *keyboard,
                                std::uint32_t serial,
                                wl_surface *surface,
                                wl_array *keys) noexcept -> void {
        if (data == nullptr) return;

        auto *window = static_cast<WaylandWindow *>(data);
        window->keyboardEnter(keyboard, serial, surface, keys);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto keyboard_leave_handler(void *data,
                                wl_keyboard *keyboard,
                                std::uint32_t serial,
                                wl_surface *surface) noexcept -> void {
        if (data == nullptr) return;

        auto *window = static_cast<WaylandWindow *>(data);
        window->keyboardLeave(keyboard, serial, surface);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto keyboard_key_handler(void *data,
                              wl_keyboard *keyboard,
                              std::uint32_t serial,
                              std::uint32_t time,
                              std::uint32_t key,
                              std::uint32_t state) noexcept -> void {
        if (data == nullptr) return;

        auto *window = static_cast<WaylandWindow *>(data);
        window->keyboardKey(keyboard, serial, time, key, state);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto keyboard_modifiers_handler(void *data,
                                    wl_keyboard *keyboard,
                                    std::uint32_t serial,
                                    std::uint32_t mods_depressed,
                                    std::uint32_t mods_latcher,
                                    std::uint32_t mods_locked,
                                    std::uint32_t group) noexcept -> void {
        if (data == nullptr) return;

        auto *window = static_cast<WaylandWindow *>(data);
        window
            ->keyboardModifiers(keyboard, serial, mods_depressed, mods_latcher, mods_locked, group);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto keyboard_repeat_info_handler(void *data,
                                      wl_keyboard *keyboard,
                                      std::int32_t rate,
                                      std::int32_t delay) noexcept -> void {
        if (data == nullptr) return;

        auto *window = static_cast<WaylandWindow *>(data);
        window->keyboardRepeatInfo(keyboard, rate, delay);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto relative_pointer_relative_motion_handler(void *data,
                                                  zwp_relative_pointer_v1 *pointer,
                                                  std::uint32_t time_hi,
                                                  std::uint32_t time_lw,
                                                  wl_fixed_t dx,
                                                  wl_fixed_t dy,
                                                  wl_fixed_t dx_unaccel,
                                                  wl_fixed_t dy_unaccel) noexcept -> void {
        auto *window = static_cast<WaylandWindow *>(data);
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
    auto locked_pointer_locked_handler(
        [[maybe_unused]] void *data,
        [[maybe_unused]] zwp_locked_pointer_v1 *locked_pointer) noexcept -> void {}

    /////////////////////////////////////
    /////////////////////////////////////
    auto locked_pointer_unlocked_handler(
        [[maybe_unused]] void *data,
        [[maybe_unused]] zwp_locked_pointer_v1 *locked_pointer) noexcept -> void {}
} // namespace storm::window::details
