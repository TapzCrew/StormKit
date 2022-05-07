// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "Win32Window.hpp"
#include "Utils.hpp"

/////////// - GSL - ///////////
#include <gsl/gsl_util>

/////////// - STL - ///////////
#include <locale>
#include <string>

/////////// - Win32API - ///////////
#include <WinUser.h>

using namespace storm;
using namespace storm::window;
using namespace storm::window::details;

auto fromStdString(std::string_view str) -> std::wstring {
    const auto size = gsl::narrow_cast<std::size_t>(
        MultiByteToWideChar(CP_UTF8, 0, std::data(str), -1, nullptr, 0));
    auto wstr = std::wstring {};
    wstr.resize(size);

    MultiByteToWideChar(CP_UTF8,
                        0,
                        std::data(str),
                        -1,
                        std::data(wstr),
                        gsl::narrow_cast<int>(size));

    return wstr;
}

/////////////////////////////////////
/////////////////////////////////////
Win32Window::Win32Window() = default;

/////////////////////////////////////
/////////////////////////////////////
Win32Window::~Win32Window() {
    if (m_window_handle) DestroyWindow(m_window_handle);

    UnregisterClassW(CLASS_NAME, GetModuleHandleW(nullptr));
}

/////////////////////////////////////
/////////////////////////////////////
Win32Window::Win32Window(std::string title, const VideoSettings &settings, WindowStyle style)
    : m_is_open { false }, m_is_visible { false } {
    create(std::move(title), settings, style);
}

/////////////////////////////////////
/////////////////////////////////////
Win32Window::Win32Window(Win32Window &&) noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
auto Win32Window::operator=(Win32Window &&) noexcept -> Win32Window & = default;

/////////////////////////////////////
/////////////////////////////////////
auto Win32Window::create(std::string title, const VideoSettings &settings, WindowStyle style)
    -> void {
    registerWindowClass();

    auto _style     = DWORD { WS_BORDER };
    auto h_instance = GetModuleHandleA(nullptr);

    if ((style & WindowStyle::TitleBar) == WindowStyle::TitleBar) _style |= WS_CAPTION;
    if ((style & WindowStyle::Close) == WindowStyle::Close) _style |= WS_SYSMENU;
    if ((style & WindowStyle::Minimizable) == WindowStyle::Minimizable) _style |= WS_MINIMIZEBOX;
    if ((style & WindowStyle::Resizable) == WindowStyle::Resizable)
        _style |= WS_MAXIMIZEBOX | WS_THICKFRAME;

    m_window_handle = CreateWindowExW(0l,
                                      CLASS_NAME,
                                      fromStdString(title).c_str(),
                                      _style,
                                      CW_USEDEFAULT,
                                      CW_USEDEFAULT,
                                      settings.size.width,
                                      settings.size.height,
                                      nullptr,
                                      nullptr,
                                      h_instance,
                                      this);

    ShowWindow(m_window_handle, SW_SHOWNORMAL);

    m_is_open       = true;
    m_is_visible    = true;
    m_is_fullscreen = false;
    m_resizing      = false;
    m_mouse_inside  = false;

    m_title = std::move(title);

    m_last_size = settings.size;
}

/////////////////////////////////////
/////////////////////////////////////
auto Win32Window::close() noexcept -> void {
    m_is_open    = false;
    m_is_visible = false;
}

/////////////////////////////////////
/////////////////////////////////////
auto Win32Window::pollEvent(Event &event) noexcept -> bool {
    auto message = MSG {};
    ZeroMemory(&message, sizeof(MSG));

    while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    return AbstractWindow::pollEvent(event);
}

/////////////////////////////////////
/////////////////////////////////////
auto Win32Window::waitEvent(Event &event) noexcept -> bool {
    auto message = MSG {};
    ZeroMemory(&message, sizeof(MSG));

    auto has_message = WaitMessage();
    while (has_message && PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    return AbstractWindow::waitEvent(event);
}

/////////////////////////////////////
/////////////////////////////////////
auto Win32Window::setTitle(std::string title) noexcept -> void {
    SetWindowTextW(m_window_handle, fromStdString(title).c_str());

    m_title = std::move(title);
}

/////////////////////////////////////
/////////////////////////////////////
void Win32Window::setFullscreenEnabled(bool enabled) noexcept {
    auto mode         = DEVMODE {};
    ZeroMemory(&mode, sizeof(DEVMODE));

    mode.dmSize       = sizeof(DEVMODE);
    mode.dmBitsPerPel = m_video_settings.bpp;
    mode.dmPelsWidth  = m_current_size.width;
    mode.dmPelsHeight = m_current_size.height;
    mode.dmFields     = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;

    ChangeDisplaySettings(&mode, CDS_FULLSCREEN);

    SetWindowLongW(m_window_handle, GWL_STYLE, WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
    SetWindowLongW(m_window_handle, GWL_EXSTYLE, WS_EX_APPWINDOW);

    SetWindowPos(m_window_handle,
                 HWND_TOP,
                 0,
                 0,
                 m_current_size.width,
                 m_current_size.height,
                 SWP_FRAMECHANGED);

    m_fullscreen = true;
}

/////////////////////////////////////
/////////////////////////////////////
void Win32Window::lockMouse() noexcept {
    auto rect = RECT{};
    ZeroMemory(&rect, sizeof(RECT));

    GetClientRect(m_window_handle, &rect);

    ClipCursor(&rect);
    m_mouse_locked = true;
}

/////////////////////////////////////
/////////////////////////////////////
void Win32Window::unlockMouse() noexcept {
    ClipCursor(nullptr);
    m_mouse_locked = false;
}

/////////////////////////////////////
/////////////////////////////////////
void Win32Window::hideMouse() noexcept {
    ShowCursor(FALSE);
}

/////////////////////////////////////
/////////////////////////////////////
void Win32Window::unhideMouse() noexcept {
    ShowCursor(TRUE);
}

/////////////////////////////////////
/////////////////////////////////////
auto Win32Window::size() const noexcept -> const core::Extentu & {
    auto rect = RECT {};
    ZeroMemory(&rect, sizeof(RECT));

    GetClientRect(m_window_handle, &rect);

    m_current_size.width  = rect.right - rect.left;
    m_current_size.height = rect.bottom - rect.top;

    return m_current_size;
}

/////////////////////////////////////
/////////////////////////////////////
auto Win32Window::isOpen() const noexcept -> bool {
    return m_is_open;
}

/////////////////////////////////////
/////////////////////////////////////
auto Win32Window::isVisible() const noexcept -> bool {
    return m_is_visible;
}

/////////////////////////////////////
/////////////////////////////////////
auto Win32Window::nativeHandle() const noexcept -> storm::window::NativeHandle {
    return static_cast<storm::window::NativeHandle>(m_window_handle);
}

/////////////////////////////////////
/////////////////////////////////////
auto Win32Window::getDesktopModes() -> std::vector<VideoSettings> {
    static auto video_settings = std::vector<VideoSettings> {};
    static auto init           = false;

    if (!init) {
        auto dm = DEVMODE {};
        ZeroMemory(&dm, sizeof(DEVMODE));

        dm.dmSize = sizeof(dm);

        for (auto i = 0; EnumDisplaySettings(nullptr, i, &dm) != 0; ++i) {
            auto video_setting =
                VideoSettings { .size = { gsl::narrow_cast<core::UInt16>(dm.dmPelsWidth),
                                          gsl::narrow_cast<core::UInt16>(dm.dmPelsHeight) } };

            video_settings.emplace_back(video_setting);
        }

        init = true;
    }

    return video_settings;
}

/////////////////////////////////////
/////////////////////////////////////
auto Win32Window::getDesktopFullscreenSize() -> VideoSettings {
    static auto video_setting = VideoSettings {};
    static auto init          = false;

    if (!init) {
        video_setting.size.width  = static_cast<core::UInt16>(GetSystemMetrics(SM_CXSCREEN));
        video_setting.size.height = static_cast<core::UInt16>(GetSystemMetrics(SM_CYSCREEN));

        init = true;
    }

    return video_setting;
}

/////////////////////////////////////
/////////////////////////////////////
auto Win32Window::globalOnEvent(HWND handle, UINT message, WPARAM w_param, LPARAM l_param)
    -> LRESULT {
    if (message == WM_CREATE) {
        auto lp_create_params = reinterpret_cast<CREATESTRUCT *>(l_param)->lpCreateParams;

        SetWindowLongPtrW(handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(lp_create_params));
    }

    auto window = handle ? reinterpret_cast<Win32Window *>(GetWindowLongPtrW(handle, GWLP_USERDATA))
                         : nullptr;
    if (window) { window->processEvents(message, w_param, l_param); }

    if (message == WM_CLOSE) return 0;

    if ((message == WM_SYSCOMMAND) && (w_param == SC_KEYMENU)) return 0;

    return DefWindowProcW(handle, message, w_param, l_param);
}

/////////////////////////////////////
/////////////////////////////////////
auto Win32Window::registerWindowClass() -> void {
    auto window_class = WNDCLASSW {};
    std::memset(&window_class, 0, sizeof(window_class));

    window_class.lpfnWndProc   = &Win32Window::globalOnEvent;
    window_class.hInstance     = GetModuleHandleW(nullptr);
    window_class.lpszClassName = CLASS_NAME;

    RegisterClassW(&window_class);
}

/////////////////////////////////////
/////////////////////////////////////
auto Win32Window::processEvents(UINT message, WPARAM w_param, LPARAM l_param) -> void {
    if (!m_window_handle) return;

    switch (message) {
        case WM_CLOSE: closeEvent(); break;
        case WM_SIZE:
            if ((w_param != SIZE_MINIMIZED) && !m_resizing && (m_last_size != size())) {
                m_last_size = size();

                resizeEvent(m_last_size.width, m_last_size.height);
            }
            break;
        case WM_ENTERSIZEMOVE: m_resizing = true; break;
        case WM_EXITSIZEMOVE:
            m_resizing = false;

            if (m_last_size != size()) {
                m_last_size = size();

                resizeEvent(m_last_size.width, m_last_size.height);
            }
            break;
        case WM_LBUTTONDOWN: {
            auto x = static_cast<core::Int16>(LOWORD(l_param));
            auto y = static_cast<core::Int16>(HIWORD(l_param));
            mouseDownEvent(MouseButton::Left, x, y);
            break;
        }
        case WM_LBUTTONUP: {
            auto x = static_cast<core::Int16>(LOWORD(l_param));
            auto y = static_cast<core::Int16>(HIWORD(l_param));
            mouseUpEvent(MouseButton::Left, x, y);
            break;
        }
        case WM_RBUTTONDOWN: {
            auto x = static_cast<core::Int16>(LOWORD(l_param));
            auto y = static_cast<core::Int16>(HIWORD(l_param));
            mouseDownEvent(MouseButton::Right, x, y);
            break;
        }
        case WM_RBUTTONUP: {
            auto x = static_cast<core::Int16>(LOWORD(l_param));
            auto y = static_cast<core::Int16>(HIWORD(l_param));
            mouseUpEvent(MouseButton::Right, x, y);
            break;
        }
        case WM_MBUTTONDOWN: {
            auto x = static_cast<core::Int16>(LOWORD(l_param));
            auto y = static_cast<core::Int16>(HIWORD(l_param));
            mouseDownEvent(MouseButton::Middle, x, y);
            break;
        }
        case WM_MBUTTONUP: {
            auto x = static_cast<core::Int16>(LOWORD(l_param));
            auto y = static_cast<core::Int16>(HIWORD(l_param));
            mouseUpEvent(MouseButton::Middle, x, y);
            break;
        }
        case WM_XBUTTONDOWN: {
            auto x      = static_cast<core::Int16>(LOWORD(l_param));
            auto y      = static_cast<core::Int16>(HIWORD(l_param));
            auto button = HIWORD(w_param);
            if (button == XBUTTON1) mouseDownEvent(MouseButton::Button1, x, y);
            else if (button == XBUTTON2)
                mouseDownEvent(MouseButton::Button1, x, y);
            break;
        }
        case WM_XBUTTONUP: {
            auto x      = static_cast<core::Int16>(LOWORD(l_param));
            auto y      = static_cast<core::Int16>(HIWORD(l_param));
            auto button = HIWORD(w_param);
            if (button == XBUTTON1) mouseUpEvent(MouseButton::Button2, x, y);
            else if (button == XBUTTON2)
                mouseUpEvent(MouseButton::Button2, x, y);
            break;
        }
        case WM_MOUSEMOVE: {
            auto x = static_cast<core::Int16>(LOWORD(l_param));
            auto y = static_cast<core::Int16>(HIWORD(l_param));

            auto area = RECT {};
            GetClientRect(m_window_handle, &area);

            if ((w_param & (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON | MK_XBUTTON1 | MK_XBUTTON2)) ==
                0) {
                if (GetCapture() == m_window_handle) ReleaseCapture();
            } else if (GetCapture() != m_window_handle)
                SetCapture(m_window_handle);

            if ((x < area.left) || (x > area.right) || (y < area.top) || (y > area.bottom)) {
                if (m_mouse_inside) {
                    m_mouse_inside = false;

                    auto mouseEvent        = TRACKMOUSEEVENT {};
                    mouseEvent.cbSize      = sizeof(TRACKMOUSEEVENT);
                    mouseEvent.dwFlags     = TME_CANCEL;
                    mouseEvent.hwndTrack   = m_window_handle;
                    mouseEvent.dwHoverTime = HOVER_DEFAULT;
                    TrackMouseEvent(&mouseEvent);

                    mouseExitedEvent();
                }
            } else {
                if (!m_mouse_inside) {
                    m_mouse_inside = true;

                    auto mouseEvent        = TRACKMOUSEEVENT {};
                    mouseEvent.cbSize      = sizeof(TRACKMOUSEEVENT);
                    mouseEvent.dwFlags     = TME_LEAVE;
                    mouseEvent.hwndTrack   = m_window_handle;
                    mouseEvent.dwHoverTime = HOVER_DEFAULT;
                    TrackMouseEvent(&mouseEvent);

                    mouseEnteredEvent();
                }
            }

            mouseMoveEvent(x, y);
            break;
        }
        case WM_KEYDOWN: [[fallthrough]];
        case WM_SYSKEYDOWN: {
            auto key        = win32KeyToStormKitKey(w_param, l_param);
            auto charactere = win32KeyToChar(w_param, l_param);

            keyDownEvent(key, charactere);
            break;
        }
        case WM_KEYUP: [[fallthrough]];
        case WM_SYSKEYUP: {
            auto key        = win32KeyToStormKitKey(w_param, l_param);
            auto charactere = win32KeyToChar(w_param, l_param);

            keyUpEvent(key, charactere);
            break;
        }
    }
}
