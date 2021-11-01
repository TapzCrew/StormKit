#include "X11InputHandler.hpp"
#include "Log.hpp"
#include "Utils.hpp"
#include "X11Window.hpp"

/////////// - StormKit::window - ///////////
#include <storm/window/Window.hpp>

/////////// - XCB - ///////////
#include <xcb/xcb_atom.h>
#include <xcb/xcb_icccm.h>
#include <xcb/xcb_keysyms.h>
#include <xcb/xinput.h>

#include <libinput.h>
#include <libi

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace storm;
using namespace storm::window;
using namespace storm::window::details;

static int open_restricted(const char *path, int flags, [[maybe_unused]] void *user_data) {
    int fd = open(path, flags);
    return fd < 0 ? -errno : fd;
}

static void close_restricted(int fd, void *user_data) {
    close(fd);
}

const static auto interface = libinput_interface {
    .open_restricted  = open_restricted,
    .close_restricted = close_restricted,
};

/////////////////////////////////////
/////////////////////////////////////
X11InputHandler::X11InputHandler(const AbstractWindow &window)
    : InputHandler { window }, m_x11_window { &static_cast<const X11Window &>(window) } {
    const_cast<X11Window *>(m_x11_window)->registerInputHandler(*this);

    m_udev.reset(udev_new());
    m_context.reset(libinput_udev_create_context(&interface, nullptr, m_udev.get()));
    libinput_udev_assign_seat(m_context.get(), "seat0");
    libinput_dispatch(m_context.get());
}

/////////////////////////////////////
/////////////////////////////////////
X11InputHandler::~X11InputHandler() {
    const_cast<X11Window *>(m_x11_window)->unregisterInputHandler(*this);
}

/////////////////////////////////////
/////////////////////////////////////
X11InputHandler::X11InputHandler(X11InputHandler &&) noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
auto X11InputHandler::operator=(X11InputHandler &&) noexcept -> X11InputHandler & = default;

/////////////////////////////////////
/////////////////////////////////////
auto X11InputHandler::isKeyPressed(window::Key key) const noexcept -> bool {
    auto it = core::ranges::find_if(m_keyboard, [key](const auto &k) { return k.key == key; });

    return it->down;
}

/////////////////////////////////////
/////////////////////////////////////
auto X11InputHandler::isMouseButtonPressed(MouseButton button) const noexcept -> bool {
    auto default_screen_id    = 0;
    const auto &native_handle = m_x11_window->xcbHandles();
    auto root_window          = defaultRootWindow(native_handle.connection, default_screen_id);

    auto cookie = xcb_query_pointer(native_handle.connection, root_window);
    auto reply  = xcb_query_pointer_reply(native_handle.connection, cookie, nullptr);

    auto is_pressed = false;
    switch (button) {
        case MouseButton::Left: is_pressed = (reply->mask & XCB_BUTTON_MASK_1); break;
        case MouseButton::Middle: is_pressed = (reply->mask & XCB_BUTTON_MASK_2); break;
        case MouseButton::Right: is_pressed = (reply->mask & XCB_BUTTON_MASK_3); break;
        case MouseButton::Button1: is_pressed = (reply->mask & XCB_BUTTON_MASK_4); break;
        case MouseButton::Button2: is_pressed = (reply->mask & XCB_BUTTON_MASK_5); break;
        case MouseButton::Unknow: is_pressed = false; break;
    }

    return is_pressed;
}

/////////////////////////////////////
/////////////////////////////////////
auto X11InputHandler::setMousePositionOnDesktop(core::Position2u position) noexcept -> void {
    const auto &native_handle = m_x11_window->xcbHandles();
    auto root_window          = defaultRootWindow(native_handle.connection, 0);

    xcb_warp_pointer(native_handle.connection,
                     XCB_NONE,
                     root_window,
                     0,
                     0,
                     0,
                     0,
                     position->x,
                     position->y);

    xcb_flush(native_handle.connection);
}

/////////////////////////////////////
/////////////////////////////////////
auto X11InputHandler::getMousePositionOnDesktop() const noexcept -> core::Position2u {
    auto default_screen_id    = 0;
    const auto &native_handle = m_x11_window->xcbHandles();
    auto root_window          = defaultRootWindow(native_handle.connection, default_screen_id);

    auto cookie = xcb_query_pointer(native_handle.connection, root_window);
    auto reply  = xcb_query_pointer_reply(native_handle.connection, cookie, nullptr);

    const auto position = core::Vector2u { reply->root_x, reply->root_y };

    return core::makeNamed<core::Position2u>(std::move(position));
}

/////////////////////////////////////
/////////////////////////////////////
auto X11InputHandler::setMousePositionOnWindow(core::Position2i position) noexcept -> void {
    const auto &native_handle = m_x11_window->xcbHandles();

    xcb_warp_pointer_checked(native_handle.connection,
                             XCB_NONE,
                             native_handle.window,
                             0,
                             0,
                             0,
                             0,
                             position->x,
                             position->y);
    xcb_flush(native_handle.connection);
}

/////////////////////////////////////
/////////////////////////////////////
auto X11InputHandler::getMousePositionOnWindow() const noexcept -> core::Position2i {
    const auto &native_handle = m_x11_window->xcbHandles();

    auto cookie = xcb_query_pointer(native_handle.connection, native_handle.window);
    auto reply  = xcb_query_pointer_reply(native_handle.connection, cookie, nullptr);

    const auto position = core::Vector2i { reply->win_x, reply->win_y };

    return core::makeNamed<core::Position2i>(std::move(position));
}

/////////////////////////////////////
/////////////////////////////////////
auto X11InputHandler::setVirtualKeyboardVisible([[maybe_unused]] bool visible) noexcept -> void {
    // not supported
}

auto X11InputHandler::handleEvents() -> void {
    using LibInputEvent = libinput_event *;
    auto event          = LibInputEvent { nullptr };

    while ((event = libinput_get_event(m_context.get())) != nullptr) {
        if(event->

        libinput_event_destroy(event);
        libinput_dispatch(m_context.get());
    }
}
