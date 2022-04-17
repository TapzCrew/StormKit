// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#if defined(STORMKIT_CXX20_MODULES)
module stormkit.wsi.details.win32.windowimpl;

/////////// - STL - ///////////
import<locale>;

#else
/////////// - STL - ///////////
    #include <locale>

    /////////// - StormKit::wsi - ///////////
    #include "Log.mpp"
    #include "Utils.mpp"
    #include "WindowImpl.mpp"
#endif

/////////// - Win32API - ///////////
#include <winuser.h>

namespace stormkit::wsi::details::win32 {
    constinit auto CLASS_NAME = L"Stormkit_Window";

    auto fromStdString(std::string_view str) -> std::wstring {
        const auto size =
            core::as<std::size_t>(MultiByteToWideChar(CP_UTF8, 0, std::data(str), -1, nullptr, 0));
        auto wstr = std::wstring {};
        wstr.resize(size);

        MultiByteToWideChar(CP_UTF8, 0, std::data(str), -1, std::data(wstr), core::as<int>(size));

        return wstr;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    WindowImpl::WindowImpl([[maybe_unused]] Window::WM wm) {}

    /////////////////////////////////////
    /////////////////////////////////////
    WindowImpl::WindowImpl(Window::WM wm,
                           std::string title,
                           const VideoSettings &settings,
                           WindowStyle style)
        : WindowImpl { wm } {
        create(std::move(title), settings, style);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    WindowImpl::~WindowImpl() {
        if (m_window_handle) DestroyWindow(m_window_handle);

        UnregisterClassW(CLASS_NAME, GetModuleHandleW(nullptr));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    WindowImpl::WindowImpl(WindowImpl &&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowImpl::operator=(WindowImpl &&) noexcept -> WindowImpl & = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowImpl::create(std::string title, const VideoSettings &settings, WindowStyle style)
        -> void {
        registerWindowClass();

        auto _style     = DWORD { WS_BORDER };
        auto h_instance = GetModuleHandleW(nullptr);

        if ((style & WindowStyle::TitleBar) == WindowStyle::TitleBar) _style |= WS_CAPTION;
        if ((style & WindowStyle::Close) == WindowStyle::Close) _style |= WS_SYSMENU;
        if ((style & WindowStyle::Minimizable) == WindowStyle::Minimizable)
            _style |= WS_MINIMIZEBOX;
        if ((style & WindowStyle::Resizable) == WindowStyle::Resizable)
            _style |= WS_MAXIMIZEBOX | WS_THICKFRAME;

        const auto w_title = fromStdString(title);

        m_window_handle = CreateWindowExW(0l,
                                          CLASS_NAME,
                                          std::data(w_title),
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
    auto WindowImpl::pollEvent(Event &event) noexcept -> bool {
        auto message = MSG {};
        ZeroMemory(&message, sizeof(MSG));

        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }

        return WindowImplBase::pollEvent(event);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowImpl::waitEvent(Event &event) noexcept -> bool {
        auto message = MSG {};
        ZeroMemory(&message, sizeof(MSG));

        auto has_message = WaitMessage();
        while (has_message && PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }

        return WindowImplBase::waitEvent(event);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowImpl::setTitle(std::string title) noexcept -> void {
        const auto w_title = fromStdString(title);
        SetWindowTextW(m_window_handle, std::data(w_title));

        m_title = std::move(title);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowImpl::setFullscreenEnabled(bool enabled) noexcept -> void {
        auto mode = DEVMODE {};
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

        m_is_fullscreen = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowImpl::lockMouse() noexcept -> void {
        auto rect = RECT {};
        ZeroMemory(&rect, sizeof(RECT));

        GetClientRect(m_window_handle, &rect);

        ClipCursor(&rect);
        m_mouse_locked = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowImpl::unlockMouse() noexcept -> void {
        ClipCursor(nullptr);
        m_mouse_locked = false;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowImpl::hideMouse() noexcept -> void { ShowCursor(TRUE); }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowImpl::unhideMouse() noexcept -> void { ShowCursor(FALSE); }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowImpl::size() const noexcept -> const core::ExtentU & {
        auto rect = RECT {};
        ZeroMemory(&rect, sizeof(RECT));

        GetClientRect(m_window_handle, &rect);

        m_current_size.width  = rect.right - rect.left;
        m_current_size.height = rect.bottom - rect.top;

        return m_current_size;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowImpl::setKeyRepeatEnabled(bool enabled) noexcept -> void {
        elog("win32::WindowImpl::setKeyRepeatEnabled isn't yet implemented");
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowImpl::setVirtualKeyboardVisible([[maybe_unused]] bool visible) noexcept -> void {
        elog("win32::WindowImpl::setVirtualKeyboardVisible isn't yet implemented");
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowImpl::setMousePosition(core::Position2i position) noexcept -> void {
        auto point = POINT { static_cast<long>(position->x), static_cast<long>(position->y) };
        ClientToScreen(m_window_handle, &point);
        SetCursorPos(point.x, point.y);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowImpl::setMousePositionOnDesktop([[maybe_unused]] Window::WM wm,
                                               core::Position2u position) noexcept -> void {
        SetCursorPos(position->x, position->y);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowImpl::getDesktopModes([[maybe_unused]] Window::WM wm) -> std::vector<VideoSettings> {
        static auto video_settings = std::vector<VideoSettings> {};
        static auto init           = false;

        if (!init) {
            auto dm = DEVMODE {};
            ZeroMemory(&dm, sizeof(DEVMODE));

            dm.dmSize = sizeof(dm);

            for (auto i = 0; EnumDisplaySettings(nullptr, i, &dm) != 0; ++i) {
                auto video_setting =
                    VideoSettings { .size = { core::as<core::UInt16>(dm.dmPelsWidth),
                                              core::as<core::UInt16>(dm.dmPelsHeight) } };

                video_settings.emplace_back(video_setting);
            }

            init = true;
        }

        return video_settings;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowImpl::getDesktopFullscreenSize([[maybe_unused]] Window::WM wm) -> VideoSettings {
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
    auto WindowImpl::globalOnEvent(HWND handle, UINT message, WPARAM w_param, LPARAM l_param)
        -> LRESULT {
        if (message == WM_CREATE) {
            auto lp_create_params = reinterpret_cast<CREATESTRUCT *>(l_param)->lpCreateParams;

            SetWindowLongPtrW(handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(lp_create_params));
        }

        auto window = handle
                          ? reinterpret_cast<WindowImpl *>(GetWindowLongPtrW(handle, GWLP_USERDATA))
                          : nullptr;
        if (window) window->processEvents(message, w_param, l_param);

        if (message == WM_CLOSE) return 0;

        if ((message == WM_SYSCOMMAND) && (w_param == SC_KEYMENU)) return 0;

        return DefWindowProcW(handle, message, w_param, l_param);
    } // namespace stormkit::wsi::details::win32

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowImpl::registerWindowClass() -> void {
        auto window_class = WNDCLASSW {};
        ZeroMemory(&window_class, sizeof(WNDCLASSW));

        window_class.lpfnWndProc   = &WindowImpl::globalOnEvent;
        window_class.hInstance     = GetModuleHandleW(nullptr);
        window_class.lpszClassName = CLASS_NAME;

        RegisterClassW(&window_class);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowImpl::processEvents(UINT message, WPARAM w_param, LPARAM l_param) -> void {
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

                if ((w_param &
                     (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON | MK_XBUTTON1 | MK_XBUTTON2)) == 0) {
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
} // namespace stormkit::wsi::details::win32
