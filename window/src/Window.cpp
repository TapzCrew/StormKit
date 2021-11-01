/////////// - StormKit::window - ///////////
#include <storm/core/Assert.hpp>

/////////// - StormKit::window - ///////////
#include <storm/window/Window.hpp>

/////////// - StormKit::module - ///////////
#include <storm/module/Module.hpp>

#include "Log.hpp"

#if defined(STORMKIT_OS_LINUX)
    #if STORMKIT_ENABLE_XCB
        #include "Linux/X11/X11Keyboard.hpp"
        #include "Linux/X11/X11Mouse.hpp"
        #include "Linux/X11/X11Window.hpp"
    #endif

    #if STORMKIT_ENABLE_WAYLAND
        #include "Linux/Wayland/WaylandKeyboard.hpp"
        #include "Linux/Wayland/WaylandMouse.hpp"
        #include "Linux/Wayland/WaylandWindow.hpp"
    #endif
#elif defined(STORMKIT_OS_WINDOWS)
    #include "Win32/Win32Keyboard.hpp"
    #include "Win32/Win32Mouse.hpp"
    #include "Win32/Win32Window.hpp"
#elif defined(STORMKIT_OS_APPLE)
    #ifdef STORMKIT_OS_MACOS
        #include "Apple/macOS/macOSKeyboard.hpp"
        #include "Apple/macOS/macOSMouse.hpp"
        #include "Apple/macOS/macOSWindow.hpp"
    #endif
#endif

using namespace storm;
using namespace storm::window;
using namespace std::literals;

/////////////////////////////////////
/////////////////////////////////////
Window::Window() noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
Window::Window(std::string title, const VideoSettings &settings, WindowStyle style) noexcept {
    create(std::move(title), settings, style);
}

/////////////////////////////////////
/////////////////////////////////////
Window::~Window() {
}

/////////////////////////////////////
/////////////////////////////////////
Window::Window(Window &&) = default;

/////////////////////////////////////
/////////////////////////////////////
Window &Window::operator=(Window &&) = default;

/////////////////////////////////////
/////////////////////////////////////
auto Window::create(std::string title, const VideoSettings &settings, WindowStyle style) noexcept
    -> void {
    const auto wm = detectWM();
    switch (wm) {
        case WM::Win32: dlog("Using Win32 window backend");
#ifdef STORMKIT_OS_WINDOWS
            m_impl = details::Win32Window::allocateOwned(std::move(title), settings, style);
#else
            ASSERT(true, "Win32 backend not supported on this system");
#endif
            break;
        case WM::X11: dlog("Using XCB window backend");
#if defined(STORMKIT_OS_LINUX) && STORMKIT_ENABLE_XCB
            m_impl = details::X11Window::allocateOwned(std::move(title), settings, style);
#else
            ASSERT(true, "XCB backend not supported on this system");
#endif
            break;
        case WM::Wayland: dlog("Using Wayland window backend");
#if defined(STORMKIT_OS_LINUX) && defined(STORMKIT_ENABLE_WAYLAND)
            m_impl = details::WaylandWindow::allocateOwned(std::move(title), settings, style);
#else
            ASSERT(true, "Wayland backend not supported on this system");
#endif
            break;
        case WM::macOS: dlog("Using macOS cocoa window backend");
#ifdef STORMKIT_OS_MACOS
            m_impl = details::macOSWindow::allocateOwned(std::move(title), settings, style);
#else
            ASSERT(true, "macOS backend not supported on this system");
#endif
            break;
        case WM::iOS:
            dlog("Using iOS cocoa window backend");
            ASSERT(true, "Not yet implemented");
            break;
        case WM::Android:
            dlog("Using Android window backend");
            ASSERT(true, "Not yet implemented");
            break;
        case WM::Switch:
            dlog("Using Nintendo Switch window backend");
            ASSERT(true, "Not yet implemented");
            break;
        default: ASSERT(true, "Unhandled platform");
    }
}
/////////////////////////////////////
/////////////////////////////////////
auto Window::close() noexcept -> void {
    m_impl->close();
}

/////////////////////////////////////
/////////////////////////////////////
auto Window::pollEvent(Event &event) noexcept -> bool {
    return m_impl->pollEvent(event);
}

/////////////////////////////////////
/////////////////////////////////////
auto Window::waitEvent(Event &event) noexcept -> bool {
    return m_impl->waitEvent(event);
}

/////////////////////////////////////
/////////////////////////////////////
auto Window::setTitle(std::string title) noexcept -> void {
    m_impl->setTitle(std::move(title));
}

/////////////////////////////////////
/////////////////////////////////////
auto Window::setFullscreenEnabled(bool fullscreen) noexcept -> void {
    m_impl->setFullscreenEnabled(fullscreen);
}

/////////////////////////////////////
/////////////////////////////////////
auto Window::lockMouse() noexcept -> void {
    m_impl->lockMouse();
}

/////////////////////////////////////
/////////////////////////////////////
auto Window::unlockMouse() noexcept -> void {
    m_impl->unlockMouse();
}

/////////////////////////////////////
/////////////////////////////////////
auto Window::hideMouse() noexcept -> void {
    m_impl->hideMouse();
}

/////////////////////////////////////
/////////////////////////////////////
auto Window::size() const noexcept -> const core::Extentu & {
    return m_impl->size();
}

/////////////////////////////////////
/////////////////////////////////////
auto Window::title() const noexcept -> std::string_view {
    return m_impl->title();
}

/////////////////////////////////////
/////////////////////////////////////
auto Window::videoSettings() const noexcept -> const VideoSettings & {
    return m_impl->videoSettings();
}

/////////////////////////////////////
/////////////////////////////////////
auto Window::isOpen() const noexcept -> bool {
    return m_impl->isOpen();
}

/////////////////////////////////////
/////////////////////////////////////
auto Window::isVisible() const noexcept -> bool {
    return m_impl->isVisible();
}

/////////////////////////////////////
/////////////////////////////////////
auto Window::nativeHandle() const noexcept -> NativeHandle {
    return m_impl->nativeHandle();
}

/////////////////////////////////////
/////////////////////////////////////
auto Window::isMouseLocked() const noexcept -> bool {
    return m_impl->isMouseLocked();
}

/////////////////////////////////////
/////////////////////////////////////
auto Window::isInFullscreen() const noexcept -> bool {
    return m_impl->isInFullscreen();
}

/////////////////////////////////////
/////////////////////////////////////
KeyboardOwnedPtr Window::createKeyboardPtr() const {
    const auto wm = detectWM();
    switch (wm) {
        case WM::Win32:
#ifdef STORMKIT_OS_WINDOWS
            return details::Win32Keyboard::allocateOwned(*m_impl);
#else
            ASSERT(true, "Win32 backend not supported on this system");
#endif
        case WM::X11:
#if defined(STORMKIT_OS_LINUX) && STORMKIT_ENABLE_XCB
            return details::X11Keyboard::allocateOwned(*m_impl);
#else
            ASSERT(true, "XCB backend not supported on this system");
#endif
        case WM::Wayland:
#if defined(STORMKIT_OS_LINUX) && STORMKIT_ENABLE_WAYLAND
            return details::WaylandKeyboard::allocateOwned(*m_impl);
#else
            ASSERT(true, "Wayland backend not supported on this system");
#endif
        case WM::macOS:
#ifdef STORMKIT_OS_MACOS
            return details::macOSKeyboard::allocateOwned(*m_impl);
#else
            ASSERT(true, "macOS backend not supported on this system");
#endif
        case WM::iOS: ASSERT(true, "Not yet implemented"); break;
        case WM::Android: ASSERT(true, "Not yet implemented"); break;
        case WM::Switch: ASSERT(true, "Not yet implemented"); break;
        default: ASSERT(true, "Unhandled platform");
    }
    return {};
}

/////////////////////////////////////
/////////////////////////////////////
MouseOwnedPtr Window::createMousePtr() const {
    const auto wm = detectWM();
    switch (wm) {
        case WM::Win32:
#ifdef STORMKIT_OS_WINDOWS
            return details::Win32Mouse::allocateOwned(*m_impl);
#else
            ASSERT(true, "Win32 backend not supported on this system");
#endif
        case WM::X11:
#if defined(STORMKIT_OS_LINUX) && STORMKIT_ENABLE_XCB
            return details::X11Mouse::allocateOwned(*m_impl);
#else
            ASSERT(true, "XCB backend not supported on this system");
#endif
        case WM::Wayland:
#if defined(STORMKIT_OS_LINUX) && STORMKIT_ENABLE_WAYLAND
            return details::WaylandMouse::allocateOwned(*m_impl);
#else
            ASSERT(true, "Wayland backend not supported on this system");
#endif
        case WM::macOS:
#ifdef STORMKIT_OS_MACOS
            return details::macOSMouse::allocateOwned(*m_impl);
#else
            ASSERT(true, "macOS backend not supported on this system");
#endif
        case WM::iOS: ASSERT(true, "Not yet implemented"); break;
        case WM::Android: ASSERT(true, "Not yet implemented"); break;
        case WM::Switch: ASSERT(true, "Not yet implemented"); break;
        default: ASSERT(true, "Unhandled platform");
    }
    return {};
}

/////////////////////////////////////
/////////////////////////////////////
Window::WM Window::detectWM() noexcept {
#if defined(STORMKIT_OS_WINDOWS)
    return WM::Win32;
#elif defined(STORMKIT_OS_MACOS)
    return WM::macOS;
#elif defined(STORMKIT_OS_IOS)
    return WM::iOS;
#elif defined(STORMKIT_OS_ANDROID)
    return WM::Android;
#elif defined(STORMKIT_OS_SWITCH)
    return WM::Switch;
#elif defined(STORMKIT_OS_LINUX)
    #if STORMKIT_ENABLE_XCB && STORMKIT_ENABLE_WAYLAND
    auto is_wayland = std::getenv("WAYLAND_DISPLAY") != nullptr;

    if (is_wayland) return WM::Wayland;
    else
        return WM::X11;
    #elif STORMKIT_ENABLE_WAYLAND
    return WM::Wayland;
    #elif STORMKIT_ENABLE_XCB
    return WM::Xcb
    #endif
#endif
}

/////////////////////////////////////
/////////////////////////////////////
std::vector<VideoSettings> Window::getDesktopModes() {
    const auto wm = detectWM();
    switch (wm) {
        case WM::Win32:
#ifdef STORMKIT_OS_WINDOWS
            return details::Win32Window::getDesktopModes();
#else
            ASSERT(true, "Win32 backend not supported on this system");
#endif
        case WM::X11:
#if defined(STORMKIT_OS_LINUX) && STORMKIT_ENABLE_XCB
            return details::X11Window::getDesktopModes();
#else
            ASSERT(true, "XCB backend not supported on this system");
#endif
        case WM::Wayland:
#if defined(STORMKIT_OS_LINUX) && STORMKIT_ENABLE_WAYLAND
            return details::WaylandWindow::getDesktopModes();
#else
            ASSERT(true, "Wayland backend not supported on this system");
#endif
        case WM::macOS:
#ifdef STORMKIT_OS_MACOS
            return details::macOSWindow::getDesktopModes();
#else
            ASSERT(true, "macOS backend not supported on this system");
#endif
        case WM::iOS: ASSERT(true, "Not yet implemented"); break;
        case WM::Android: ASSERT(true, "Not yet implemented"); break;
        case WM::Switch: ASSERT(true, "Not yet implemented"); break;
        default: ASSERT(true, "Unhandled platform");
    }
    return {};
}

/////////////////////////////////////
/////////////////////////////////////
VideoSettings Window::getDesktopFullscreenSize() {
    const auto wm = detectWM();
    switch (wm) {
        case WM::Win32:
#ifdef STORMKIT_OS_WINDOWS
            return details::Win32Window::getDesktopFullscreenSize();
#else
            ASSERT(true, "Win32 backend not supported on this system");
#endif
        case WM::X11:
#if defined(STORMKIT_OS_LINUX) && STORMKIT_ENABLE_XCB
            return details::X11Window::getDesktopFullscreenSize();
#else
            ASSERT(true, "XCB backend not supported on this system");
#endif
            break;
        case WM::Wayland:
#if defined(STORMKIT_OS_LINUX) && STORMKIT_ENABLE_WAYLAND
            return details::WaylandWindow::getDesktopFullscreenSize();
#else
            ASSERT(true, "Wayland backend not supported on this system");
#endif
            break;
        case WM::macOS:
#ifdef STORMKIT_OS_MACOS
            return details::macOSWindow::getDesktopFullscreenSize();
#else
            ASSERT(true, "macOS backend not supported on this system");
#endif
        case WM::iOS: ASSERT(true, "Not yet implemented"); break;
        case WM::Android: ASSERT(true, "Not yet implemented"); break;
        case WM::Switch: ASSERT(true, "Not yet implemented"); break;
        default: ASSERT(true, "Unhandled platform");
    }
    return {};
}
