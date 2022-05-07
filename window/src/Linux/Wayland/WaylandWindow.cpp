// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "../Utils.hpp"

#include "Callback.hpp"
#include "Log.hpp"
#include "WaylandKeyboard.hpp"
#include "WaylandMouse.hpp"
#include "WaylandWindow.hpp"

/////////// - Posix - ///////////
#include <sys/mman.h>
#include <syscall.h>
#include <unistd.h>

#include <linux/input-event-codes.h>

using namespace storm;
using namespace storm::window;
using namespace storm::window::details;

static auto globals = Globals {};

static const auto stormkit_registry_listener =
    wl_registry_listener { .global = registry_handler, .global_remove = registry_remover_handler };

static const auto stormkit_surface_listener =
    wl_surface_listener { .enter = surface_enter_handler, .leave = surface_leave_handler };

static const auto stormkit_xdg_surface_listener =
    xdg_surface_listener { .configure = surface_configure_handler };

static const auto stormkit_xdg_toplevel_listener =
    xdg_toplevel_listener { .configure = toplevel_configure_handler,
                            .close     = toplevel_close_handler };

static const auto stormkit_shell_listener = xdg_wm_base_listener { .ping = shell_ping_handler };

static const auto stormkit_shell_surface_listener =
    wl_shell_surface_listener { .ping      = shell_ping_handler,
                                .configure = shell_surface_configure_handler };

static const auto stormkit_relative_pointer_listener =
    zwp_relative_pointer_v1_listener { .relative_motion =
                                           relative_pointer_relative_motion_handler };

static const auto stormkit_locked_pointer_listener =
    zwp_locked_pointer_v1_listener { .locked   = locked_pointer_locked_handler,
                                     .unlocked = locked_pointer_unlocked_handler };

void init() {
    globals.display.reset(wl_display_connect(nullptr));

    if (globals.display) dlog("Wayland context initialized");
    else {
        flog("Failed to initialize wayland");
        std::exit(EXIT_FAILURE);
    }

    globals.registry.reset(wl_display_get_registry(globals.display.get()));

    wl_registry_add_listener(globals.registry.get(), &stormkit_registry_listener, &globals);

    wl_display_dispatch(globals.display.get());
    wl_display_roundtrip(globals.display.get());

    if (globals.compositor) dlog("Wayland compositor found !");
    else {
        flog("Failed to find a Wayland compositor");
        std::exit(EXIT_FAILURE);
    }
}

/////////////////////////////////////
/////////////////////////////////////
WaylandWindow::WaylandWindow() {
    if (!globals.display) init();

    m_keyboard_state = std::array {
        KeyState { XKB_KEY_a, false },           KeyState { XKB_KEY_b, false },
        KeyState { XKB_KEY_c, false },           KeyState { XKB_KEY_d, false },
        KeyState { XKB_KEY_e, false },           KeyState { XKB_KEY_f, false },
        KeyState { XKB_KEY_g, false },           KeyState { XKB_KEY_h, false },
        KeyState { XKB_KEY_i, false },           KeyState { XKB_KEY_j, false },
        KeyState { XKB_KEY_k, false },           KeyState { XKB_KEY_l, false },
        KeyState { XKB_KEY_m, false },           KeyState { XKB_KEY_n, false },
        KeyState { XKB_KEY_o, false },           KeyState { XKB_KEY_p, false },
        KeyState { XKB_KEY_q, false },           KeyState { XKB_KEY_r, false },
        KeyState { XKB_KEY_s, false },           KeyState { XKB_KEY_t, false },
        KeyState { XKB_KEY_u, false },           KeyState { XKB_KEY_v, false },
        KeyState { XKB_KEY_w, false },           KeyState { XKB_KEY_x, false },
        KeyState { XKB_KEY_y, false },           KeyState { XKB_KEY_z, false },
        KeyState { XKB_KEY_0, false },           KeyState { XKB_KEY_1, false },
        KeyState { XKB_KEY_2, false },           KeyState { XKB_KEY_3, false },
        KeyState { XKB_KEY_4, false },           KeyState { XKB_KEY_5, false },
        KeyState { XKB_KEY_6, false },           KeyState { XKB_KEY_7, false },
        KeyState { XKB_KEY_8, false },           KeyState { XKB_KEY_9, false },
        KeyState { XKB_KEY_Escape, false },      KeyState { XKB_KEY_Control_L, false },
        KeyState { XKB_KEY_Shift_L, false },     KeyState { XKB_KEY_Alt_L, false },
        KeyState { XKB_KEY_Super_L, false },     KeyState { XKB_KEY_Control_R, false },
        KeyState { XKB_KEY_Shift_R, false },     KeyState { XKB_KEY_Alt_R, false },
        KeyState { XKB_KEY_Super_R, false },     KeyState { XKB_KEY_Menu, false },
        KeyState { XKB_KEY_bracketleft, false }, KeyState { XKB_KEY_bracketright, false },
        KeyState { XKB_KEY_semicolon, false },   KeyState { XKB_KEY_comma, false },
        KeyState { XKB_KEY_period, false },      KeyState { XKB_KEY_quoteleft, false },
        KeyState { XKB_KEY_slash, false },       KeyState { XKB_KEY_backslash, false },
        KeyState { XKB_KEY_dead_grave, false },  KeyState { XKB_KEY_equal, false },
        KeyState { XKB_KEY_hyphen, false },      KeyState { XKB_KEY_space, false },
        KeyState { XKB_KEY_Return, false },      KeyState { XKB_KEY_BackSpace, false },
        KeyState { XKB_KEY_Tab, false },         KeyState { XKB_KEY_Page_Up, false },
        KeyState { XKB_KEY_Page_Down, false },   KeyState { XKB_KEY_Begin, false },
        KeyState { XKB_KEY_End, false },         KeyState { XKB_KEY_Home, false },
        KeyState { XKB_KEY_Insert, false },      KeyState { XKB_KEY_Delete, false },
        KeyState { XKB_KEY_KP_Add, false },      KeyState { XKB_KEY_KP_Subtract, false },
        KeyState { XKB_KEY_KP_Multiply, false }, KeyState { XKB_KEY_KP_Divide, false },
        KeyState { XKB_KEY_Left, false },        KeyState { XKB_KEY_Right, false },
        KeyState { XKB_KEY_Up, false },          KeyState { XKB_KEY_Down, false },
        KeyState { XKB_KEY_KP_0, false },        KeyState { XKB_KEY_KP_1, false },
        KeyState { XKB_KEY_KP_2, false },        KeyState { XKB_KEY_KP_3, false },
        KeyState { XKB_KEY_KP_4, false },        KeyState { XKB_KEY_KP_5, false },
        KeyState { XKB_KEY_KP_6, false },        KeyState { XKB_KEY_KP_7, false },
        KeyState { XKB_KEY_KP_8, false },        KeyState { XKB_KEY_KP_9, false },
        KeyState { XKB_KEY_F1, false },          KeyState { XKB_KEY_F2, false },
        KeyState { XKB_KEY_F3, false },          KeyState { XKB_KEY_F4, false },
        KeyState { XKB_KEY_F5, false },          KeyState { XKB_KEY_F6, false },
        KeyState { XKB_KEY_F7, false },          KeyState { XKB_KEY_F8, false },
        KeyState { XKB_KEY_F9, false },          KeyState { XKB_KEY_F10, false },
        KeyState { XKB_KEY_F11, false },         KeyState { XKB_KEY_F12, false },
        KeyState { XKB_KEY_F13, false },         KeyState { XKB_KEY_F14, false },
        KeyState { XKB_KEY_F15, false },         KeyState { XKB_KEY_Pause, false },
    };

    m_xkb_context.reset(xkb_context_new(XKB_CONTEXT_NO_FLAGS));

    m_cursor_theme.reset(wl_cursor_theme_load(nullptr, 24, globals.shm.get()));
    auto cursor = wl_cursor_theme_get_cursor(m_cursor_theme.get(), "default");

    if (!cursor) cursor = wl_cursor_theme_get_cursor(m_cursor_theme.get(), "left_ptr");

    auto cursor_image = cursor->images[0];

    m_cursor_buffer.reset(wl_cursor_image_get_buffer(cursor_image));

    m_cursor_surface.reset(wl_compositor_create_surface(globals.compositor.get()));
    wl_surface_attach(m_cursor_surface.get(), m_cursor_buffer.get(), 0, 0);
    wl_surface_commit(m_cursor_surface.get());

    m_handles.display = globals.display.get();
}

/////////////////////////////////////
/////////////////////////////////////
WaylandWindow::~WaylandWindow() {
    wl_display_flush(globals.display.get());
}

/////////////////////////////////////
/////////////////////////////////////
WaylandWindow::WaylandWindow(std::string title, const VideoSettings &settings, WindowStyle style)
    : WaylandWindow {} {
    create(std::move(title), settings, style);
}

/////////////////////////////////////
/////////////////////////////////////
WaylandWindow::WaylandWindow(WaylandWindow &&) noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::operator=(WaylandWindow &&) noexcept -> WaylandWindow & = default;

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::create(std::string title, const VideoSettings &settings, WindowStyle style)
    -> void {
    m_title          = title;
    m_extent         = settings.size;
    m_video_settings = settings;
    m_style          = style;

    m_locked_mouse_position.x = m_extent.width / 2;
    m_locked_mouse_position.y = m_extent.height / 2;

    m_surface.reset(wl_compositor_create_surface(globals.compositor.get()));

    if (!std::empty(globals.pointers)) {
        m_pointer = globals.pointers.front().get();
        wl_pointer_set_user_data(m_pointer, this);
    } else
        wlog("No pointer found");
    if (!std::empty(globals.keyboards)) {
        m_keyboard = globals.keyboards.front().get();
        wl_keyboard_set_user_data(m_keyboard, this);
    } else
        wlog("No keyboard found");
    if (!std::empty(globals.touchscreens)) {
        m_touchscreen = globals.touchscreens.front().get();
        wl_touch_set_user_data(m_touchscreen, this);
    } else
        wlog("No touchscreen found");

    wl_surface_add_listener(m_surface.get(), &stormkit_surface_listener, this);

    if (globals.xdg_shell) {
        dlog("XDG shell found !");
        createXDGShell();
    } else {
        dlog("XDGShell not found, falling back to WLShell");

        if (globals.wayland_shell) createWaylandShell();
        else {
            flog("WLShell not found, abording...");
            std::exit(EXIT_FAILURE);
        }
    }

    m_handles.surface = m_surface.get();
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::close() noexcept -> void {
    wl_display_flush(globals.display.get());

    // Fake Buffer
    m_buffer.release();

    // WP
    m_locked_pointer.reset();
    m_relative_pointer.reset();

    // Events
    if (m_pointer) wl_pointer_set_user_data(m_pointer, nullptr);

    if (m_keyboard) wl_keyboard_set_user_data(m_keyboard, nullptr);

    if (m_touchscreen) wl_touch_set_user_data(m_touchscreen, nullptr);

    m_pointer        = nullptr;
    m_pointer_serial = 0u;
    m_keyboard       = nullptr;
    m_touchscreen    = nullptr;

    // XDG
    globals.xdg_decoration_manager.reset();
    m_xdg_toplevel.reset();
    m_xdg_surface.reset();

    // WL_Shell
    m_wlshell_surface.reset();

    // Base_Surface
    m_surface.reset();

    m_opened     = false;
    m_configured = false;
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::pollEvent(Event &event) noexcept -> bool {
    while (wl_display_prepare_read(globals.display.get()) != 0)
        wl_display_dispatch_pending(globals.display.get());
    wl_display_flush(globals.display.get());
    wl_display_read_events(globals.display.get());
    wl_display_dispatch_pending(globals.display.get());

    return AbstractWindow::pollEvent(event);
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::waitEvent(Event &event) noexcept -> bool {
    while (!m_configured) wl_display_dispatch(globals.display.get());
    while (wl_display_prepare_read(globals.display.get()) != 0)
        wl_display_dispatch(globals.display.get());
    wl_display_flush(globals.display.get());

    return AbstractWindow::waitEvent(event);
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::setTitle(std::string title) noexcept -> void {
    if (!m_opened) return;
    m_title = std::move(title);

    if (m_xdg_toplevel) {
        xdg_toplevel_set_title(m_xdg_toplevel.get(), m_title.c_str());
        xdg_toplevel_set_app_id(m_xdg_toplevel.get(), m_title.c_str());
    } else {
        wl_shell_surface_set_title(m_wlshell_surface.get(), m_title.c_str());
    }
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::lockMouse() noexcept -> void {
    if (!m_opened) return;
    if (!globals.relative_pointer_manager) {
        elog("Can't lock mouse, {} protocol is not present",
             zwp_relative_pointer_manager_v1_interface.name);
        return;
    }
    if (!globals.pointer_constraints) {
        elog("Can't lock mouse, {} protocol is not present",
             zwp_pointer_constraints_v1_interface.name);
        return;
    }

    m_mouse_state.position_in_window = m_locked_mouse_position;

    m_relative_pointer.reset(
        zwp_relative_pointer_manager_v1_get_relative_pointer(globals.relative_pointer_manager.get(),
                                                             m_pointer));
    zwp_relative_pointer_v1_add_listener(m_relative_pointer.get(),
                                         &stormkit_relative_pointer_listener,
                                         this);

    m_locked_pointer.reset(
        zwp_pointer_constraints_v1_lock_pointer(globals.pointer_constraints.get(),
                                                m_surface.get(),
                                                m_pointer,
                                                nullptr,
                                                ZWP_POINTER_CONSTRAINTS_V1_LIFETIME_PERSISTENT));

    zwp_locked_pointer_v1_add_listener(m_locked_pointer.get(),
                                       &stormkit_locked_pointer_listener,
                                       this);

    m_mouse_locked = true;

    hideMouse();
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::unlockMouse() noexcept -> void {
    if (!m_opened) return;

    m_locked_mouse_position = m_mouse_state.position_in_window;

    m_locked_pointer.reset();
    m_relative_pointer.reset();

    m_mouse_locked = false;

    unhideMouse();
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::hideMouse() noexcept -> void {
    if (!m_opened) return;
    wl_pointer_set_cursor(m_pointer, m_pointer_serial, nullptr, 0, 0);
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::unhideMouse() noexcept -> void {
    if (!m_opened) return;
    wl_pointer_set_cursor(m_pointer, m_pointer_serial, m_cursor_surface.get(), 0, 0);
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::setFullscreenEnabled(bool enabled) noexcept -> void {
    if (!m_opened) return;
    if (m_xdg_toplevel) {
        if (enabled) xdg_toplevel_set_fullscreen(m_xdg_toplevel.get(), m_current_output);
        else
            xdg_toplevel_unset_fullscreen(m_xdg_toplevel.get());
    } else {
        // TODO implement for wl_shell_surface
    }

    m_fullscreen = enabled;
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::size() const noexcept -> const storm::core::Extentu & {
    while (!m_configured) wl_display_dispatch(globals.display.get());

    return m_extent;
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::isOpen() const noexcept -> bool {
    while (!m_configured) wl_display_dispatch(globals.display.get());

    return m_opened;
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::isVisible() const noexcept -> bool {
    while (!m_configured) wl_display_dispatch(globals.display.get());

    return m_visible;
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::nativeHandle() const noexcept -> storm::window::NativeHandle {
    return reinterpret_cast<storm::window::NativeHandle>(const_cast<Handles *>(&m_handles));
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::getDesktopModes() -> std::vector<VideoSettings> {
    if (!globals.display) init();

    return globals.video_settings;
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::surfaceOutputEnter([[maybe_unused]] wl_surface *surface,
                                       wl_output *output) noexcept -> void {
    m_current_output = output;
}
/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::surfaceConfigure(xdg_surface *surface, std::uint32_t serial) noexcept -> void {
    dlog("XDG surface configure, serial: {}", serial);

    xdg_surface_ack_configure(surface, serial);
    m_configured = true;
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::toplevelConfigure([[maybe_unused]] xdg_toplevel *xdg_tl,
                                      std::int32_t width,
                                      std::int32_t height,
                                      wl_array *state) noexcept -> void {
    dlog("XDG Shell configure: {}:{}", width, height);

    m_opened  = true;
    m_visible = width > 0 && height > 0;

    auto data = static_cast<xdg_toplevel_state *>(state->data);
    for (auto i = 0u; i < state->size; ++i) {
        const auto state = data[i];

        switch (state) {
            case XDG_TOPLEVEL_STATE_MAXIMIZED: AbstractWindow::maximizeEvent(); break;
            case XDG_TOPLEVEL_STATE_RESIZING: {
                AbstractWindow::resizeEvent(width, height);
                break;
            }
            default: break;
        }
    }

    if (width <= 0 || height <= 0) return;

    m_extent.width  = width;
    m_extent.height = height;
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::toplevelClose([[maybe_unused]] xdg_toplevel *xdg_tl) noexcept -> void {
    m_opened        = false;
    m_visible       = false;
    m_extent.width  = 0;
    m_extent.height = 0;

    AbstractWindow::closeEvent();
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::shellSurfaceConfigure([[maybe_unused]] wl_shell_surface *xdg_tl,
                                          [[maybe_unused]] std::uint32_t edges,
                                          std::int32_t width,
                                          std::int32_t height) noexcept -> void {
    dlog("WL Shell configure: {}:{}", width, height);

    m_opened  = true;
    m_visible = width > 0 && height > 0;

    if (width <= 0 || height <= 0) return;

    m_extent.width  = width;
    m_extent.height = height;
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::seatCapabilities(wl_seat *seat, std::uint32_t capabilities) noexcept -> void {
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::pointerEnter([[maybe_unused]] wl_pointer *pointer,
                                 std::uint32_t serial,
                                 [[maybe_unused]] wl_surface *surface,
                                 [[maybe_unused]] wl_fixed_t surface_x,
                                 [[maybe_unused]] wl_fixed_t surface_y) noexcept -> void {
    m_pointer_serial = serial;

    if (!m_mouse_locked) unhideMouse();
    else
        hideMouse();

    AbstractWindow::mouseEnteredEvent();
}
/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::pointerLeave([[maybe_unused]] wl_pointer *pointer,
                                 [[maybe_unused]] std::uint32_t serial,
                                 [[maybe_unused]] wl_surface *surface) noexcept -> void {
    m_pointer_serial = serial;

    AbstractWindow::mouseExitedEvent();
}
/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::pointerMotion([[maybe_unused]] wl_pointer *pointer,
                                  [[maybe_unused]] std::uint32_t time,
                                  wl_fixed_t surface_x,
                                  wl_fixed_t surface_y) noexcept -> void {
    if (m_mouse_locked) return;

    m_mouse_state.position_in_window.x = wl_fixed_to_int(surface_x);
    m_mouse_state.position_in_window.y = wl_fixed_to_int(surface_y);

    AbstractWindow::mouseMoveEvent(m_mouse_state.position_in_window.x,
                                   m_mouse_state.position_in_window.y);
}
/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::pointerButton([[maybe_unused]] wl_pointer *pointer,
                                  [[maybe_unused]] std::uint32_t serial,
                                  [[maybe_unused]] std::uint32_t time,
                                  std::uint32_t button,
                                  std::uint32_t state) noexcept -> void {
#define BUTTON_HANDLER(a, b)                                                           \
    case a: {                                                                          \
        auto it  = core::ranges::find_if(m_mouse_state.button_state,                   \
                                        [](const auto &s) { return s.button == a; }); \
        it->down = down;                                                               \
        if (down)                                                                      \
            AbstractWindow::mouseDownEvent(b,                                          \
                                           m_mouse_state.position_in_window.x,         \
                                           m_mouse_state.position_in_window.y);        \
        else                                                                           \
            AbstractWindow::mouseUpEvent(b,                                            \
                                         m_mouse_state.position_in_window.x,           \
                                         m_mouse_state.position_in_window.y);          \
        break;                                                                         \
    }

    m_pointer_serial = serial;

    const auto down = !!state;

    switch (button) {
        BUTTON_HANDLER(BTN_LEFT, MouseButton::Left)
        BUTTON_HANDLER(BTN_RIGHT, MouseButton::Right)
        BUTTON_HANDLER(BTN_MIDDLE, MouseButton::Middle)
        BUTTON_HANDLER(BTN_FORWARD, MouseButton::Button1)
        BUTTON_HANDLER(BTN_BACK, MouseButton::Button2)
        default:
            if (down)
                AbstractWindow::mouseDownEvent(MouseButton::Unknow,
                                               m_mouse_state.position_in_window.x,
                                               m_mouse_state.position_in_window.y);
            else
                AbstractWindow::mouseUpEvent(MouseButton::Unknow,
                                             m_mouse_state.position_in_window.x,
                                             m_mouse_state.position_in_window.y);
    }

#undef BUTTON_HANDLER
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::keyboardKeymap([[maybe_unused]] wl_keyboard *keyboard,
                                   std::uint32_t format,
                                   std::int32_t fd,
                                   std::uint32_t size) noexcept -> void {
    if (format == WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1) {
        auto map_shm = reinterpret_cast<char *>(mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0));

        updateKeymap(std::string_view { map_shm, size });

        munmap(map_shm, size);
        ::close(fd);
    }
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::keyboardEnter([[maybe_unused]] wl_keyboard *keyboard,
                                  [[maybe_unused]] std::uint32_t serial,
                                  [[maybe_unused]] wl_surface *surface,
                                  wl_array *keys) noexcept -> void {
    AbstractWindow::gainedFocusEvent();

    auto data = static_cast<std::uint32_t *>(keys->data);
    for (auto i = 0u; i < keys->size; ++i) {
        const auto keycode = data[i] + 8;

        auto character = char {};

        const auto symbol = xkb_state_key_get_one_sym(m_xkb_state.get(), keycode);
        xkb_state_key_get_utf8(m_xkb_state.get(), keycode, &character, sizeof(char));

        const auto skey = xkbKeyToStormkitKey(symbol);

        AbstractWindow::keyDownEvent(skey, character);
    }
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::keyboardLeave(wl_keyboard *keyboard,
                                  std::uint32_t serial,
                                  wl_surface *surface) noexcept -> void {
    AbstractWindow::lostFocusEvent();
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::keyboardKey([[maybe_unused]] wl_keyboard *keyboard,
                                [[maybe_unused]] std::uint32_t serial,
                                [[maybe_unused]] std::uint32_t time,
                                std::uint32_t key,
                                std::uint32_t state) noexcept -> void {
    auto character = char {};

    const auto keycode = key + 8;

    const auto symbol = xkb_state_key_get_one_sym(m_xkb_state.get(), keycode);
    xkb_state_key_get_utf8(m_xkb_state.get(), keycode, &character, sizeof(char));

    const auto skey = xkbKeyToStormkitKey(symbol);

    const auto down = state == WL_KEYBOARD_KEY_STATE_PRESSED;

    auto it = core::ranges::find_if(m_keyboard_state,
                                    [symbol](const auto &s) { return s.key == symbol; });

    it->down = down;
    if (down) AbstractWindow::keyDownEvent(skey, character);
    else
        AbstractWindow::keyUpEvent(skey, character);
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::keyboardModifiers([[maybe_unused]] wl_keyboard *keyboard,
                                      [[maybe_unused]] std::uint32_t serial,
                                      std::uint32_t mods_depressed,
                                      std::uint32_t mods_latched,
                                      std::uint32_t mods_locked,
                                      std::uint32_t group) noexcept -> void {
    xkb_state_update_mask(m_xkb_state.get(),
                          mods_depressed,
                          mods_latched,
                          mods_locked,
                          0,
                          0,
                          group);
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::keyboardRepeatInfo([[maybe_unused]] wl_keyboard *keyboard,
                                       [[maybe_unused]] std::int32_t rate,
                                       [[maybe_unused]] std::int32_t delay) noexcept -> void {
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::relativePointerRelativeMotion(zwp_relative_pointer_v1 *pointer,
                                                  std::uint32_t time_hi,
                                                  std::uint32_t time_lw,
                                                  wl_fixed_t dx,
                                                  wl_fixed_t dy,
                                                  wl_fixed_t dx_unaccel,
                                                  wl_fixed_t dy_unaccel) noexcept -> void {
    m_mouse_state.position_in_window.x += wl_fixed_to_int(dx_unaccel);
    m_mouse_state.position_in_window.y += wl_fixed_to_int(dy_unaccel);

    AbstractWindow::mouseMoveEvent(m_mouse_state.position_in_window.x,
                                   m_mouse_state.position_in_window.y);
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::setMousePosition(core::Vector2i position) const noexcept -> void {
    if (!m_opened) return;
    if (m_mouse_locked) {
        zwp_locked_pointer_v1_set_cursor_position_hint(m_locked_pointer.get(),
                                                       wl_fixed_from_int(position.x),
                                                       wl_fixed_from_int(position.y));
        wl_surface_commit(m_surface.get());
    }
}
/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::createXDGShell() noexcept -> void {
    xdg_wm_base_add_listener(globals.xdg_shell.get(), &stormkit_shell_listener, nullptr);

    m_xdg_surface.reset(xdg_wm_base_get_xdg_surface(globals.xdg_shell.get(), m_surface.get()));

    xdg_surface_add_listener(m_xdg_surface.get(), &stormkit_xdg_surface_listener, this);

    m_xdg_toplevel.reset(xdg_surface_get_toplevel(m_xdg_surface.get()));

    xdg_toplevel_add_listener(m_xdg_toplevel.get(), &stormkit_xdg_toplevel_listener, this);

    xdg_toplevel_set_title(m_xdg_toplevel.get(), m_title.c_str());
    xdg_toplevel_set_app_id(m_xdg_toplevel.get(), m_title.c_str());

    if (!core::checkFlag(m_style, WindowStyle::Resizable)) {
        xdg_toplevel_set_min_size(m_xdg_toplevel.get(), m_extent.width, m_extent.height);
        xdg_toplevel_set_max_size(m_xdg_toplevel.get(), m_extent.width, m_extent.height);
    } else {
        auto fullscreen_size = getDesktopFullscreenSize();

        xdg_toplevel_set_min_size(m_xdg_toplevel.get(), 1, 1);
        xdg_toplevel_set_max_size(m_xdg_toplevel.get(),
                                  fullscreen_size.size.width,
                                  fullscreen_size.size.height);
    }

    if (globals.xdg_decoration_manager) {
        zxdg_decoration_manager_v1_get_toplevel_decoration(globals.xdg_decoration_manager.get(),
                                                           m_xdg_toplevel.get());
    }

    wl_surface_damage(m_surface.get(), 0, 0, m_extent.width, m_extent.height);
    createPixelBuffer();
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::createWaylandShell() noexcept -> void {
    m_wlshell_surface.reset(
        wl_shell_get_shell_surface(globals.wayland_shell.get(), m_surface.get()));
    wl_shell_surface_add_listener(m_wlshell_surface.get(), &stormkit_shell_surface_listener, this);
    wl_shell_surface_set_toplevel(m_wlshell_surface.get());

    wl_surface_damage(m_surface.get(), 0, 0, m_extent.width, m_extent.height);
    createPixelBuffer();
}

auto WaylandWindow::createPixelBuffer() noexcept -> void {
    const auto byte_per_pixel = m_video_settings.bpp / sizeof(core::Byte);
    const auto buffer_size =
        m_video_settings.size.width * m_video_settings.size.height * byte_per_pixel;
    const auto buffer_stride = m_video_settings.size.width * byte_per_pixel;

    auto fd = syscall(SYS_memfd_create, "buffer", 0);
    ftruncate(fd, buffer_size);

    [[maybe_unused]] auto *data =
        mmap(nullptr, buffer_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    m_shm_pool.reset(wl_shm_create_pool(globals.shm.get(), fd, buffer_size));

    m_buffer.reset(wl_shm_pool_create_buffer(m_shm_pool.get(),
                                             0,
                                             m_video_settings.size.width,
                                             m_video_settings.size.height,
                                             buffer_stride,
                                             WL_SHM_FORMAT_XRGB8888));

    wl_surface_attach(m_surface.get(), m_buffer.get(), 0, 0);
    wl_surface_commit(m_surface.get());
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::updateKeymap(std::string_view keymap_string) noexcept -> void {
    m_xkb_keymap.reset(xkb_keymap_new_from_string(m_xkb_context.get(),
                                                  std::data(keymap_string),
                                                  XKB_KEYMAP_FORMAT_TEXT_V1,
                                                  XKB_KEYMAP_COMPILE_NO_FLAGS));

    if (!m_xkb_keymap) {
        elog("Failed to compile a keymap");
        return;
    }

    m_xkb_state.reset(xkb_state_new(m_xkb_keymap.get()));

    if (!m_xkb_state) {
        elog("Failed to create XKB state");
        return;
    }

    updateXKBMods();
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::updateXKBMods() noexcept -> void {
    m_xkb_mods =
        XKBMods { .shift   = xkb_keymap_mod_get_index(m_xkb_keymap.get(), XKB_MOD_NAME_SHIFT),
                  .lock    = xkb_keymap_mod_get_index(m_xkb_keymap.get(), XKB_MOD_NAME_CAPS),
                  .control = xkb_keymap_mod_get_index(m_xkb_keymap.get(), XKB_MOD_NAME_CTRL),
                  .mod1    = xkb_keymap_mod_get_index(m_xkb_keymap.get(), "Mod1"),
                  .mod2    = xkb_keymap_mod_get_index(m_xkb_keymap.get(), "Mod2"),
                  .mod3    = xkb_keymap_mod_get_index(m_xkb_keymap.get(), "Mod3"),
                  .mod4    = xkb_keymap_mod_get_index(m_xkb_keymap.get(), "Mod4"),
                  .mod5    = xkb_keymap_mod_get_index(m_xkb_keymap.get(), "Mod5") };
}

/////////////////////////////////////
/////////////////////////////////////
auto WaylandWindow::getDesktopFullscreenSize() -> VideoSettings {
    static auto video_setting = storm::window::VideoSettings {};
    static auto init          = false;

    if (!init) {
        const auto modes = getDesktopModes();

        for (const auto &mode : modes) {
            video_setting.size.width  = std::max(video_setting.size.width, mode.size.width);
            video_setting.size.height = std::max(video_setting.size.height, mode.size.height);
            video_setting.size.depth  = std::max(video_setting.size.depth, mode.size.depth);
        }

        init = true;
    }

    return video_setting;
}
