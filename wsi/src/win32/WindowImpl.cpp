// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <locale>

#include "Log.mpp"
#include "Utils.mpp"
#include "WindowImpl.mpp"

#include <shellscalingapi.h>
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
                           const core::ExtentU &size,
                           WindowStyle style)
        : WindowImpl { wm } {
        create(std::move(title), size, style);
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
    auto WindowImpl::create(std::string title, const core::ExtentU &size, WindowStyle style)
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
                                          size.width,
                                          size.height,
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
        m_style         = style;

        m_title = std::move(title);

        m_last_size    = size;
        m_current_size = size;
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
    auto WindowImpl::setSize(const core::ExtentU &size) noexcept -> void {
        SetWindowPos(m_window_handle,
                     HWND_TOP,
                     0,
                     0,
                     size.width,
                     size.height,
                     SWP_NOMOVE | SWP_FRAMECHANGED);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowImpl::setFullscreenEnabled(bool enabled) noexcept -> void {
        if (enabled) {
            if (!m_is_fullscreen) {
                m_last_style    = GetWindowLongW(m_window_handle, GWL_STYLE);
                m_last_style_ex = GetWindowLongW(m_window_handle, GWL_EXSTYLE);

                auto rect = RECT {};
                ZeroMemory(&rect, sizeof(RECT));

                GetWindowRect(m_window_handle, &rect);

                m_last_position = { rect.left, rect.top };
            }

            SetWindowLongW(m_window_handle,
                           GWL_STYLE,
                           m_last_style & ~(WS_CAPTION | WS_THICKFRAME));
            SetWindowLongW(m_window_handle,
                           GWL_EXSTYLE,
                           m_last_style_ex & ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE |
                                               WS_EX_CLIENTEDGE | WS_EX_STATICEDGE));

            SetWindowPos(m_window_handle,
                         nullptr,
                         0,
                         0,
                         m_current_size.width,
                         m_current_size.height,
                         SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

            m_is_fullscreen = true;
        } else {
            SetWindowLongW(m_window_handle, GWL_STYLE, m_last_style);
            SetWindowLongW(m_window_handle, GWL_EXSTYLE, m_last_style_ex);

            SetWindowPos(m_window_handle,
                         nullptr,
                         m_last_position.x,
                         m_last_position.y,
                         m_last_size.width,
                         m_last_size.height,
                         SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

            m_is_fullscreen = false;
        }
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
    auto WindowImpl::size() const noexcept -> const core::ExtentU16 & {
        auto rect = RECT {};
        ZeroMemory(&rect, sizeof(RECT));

        GetClientRect(m_window_handle, &rect);

        m_current_size.width  = core::as<core::UInt16>(rect.right - rect.left);
        m_current_size.height = core::as<core::UInt16>(rect.bottom - rect.top);

        return m_current_size;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowImpl::setKeyRepeatEnabled([[maybe_unused]] bool enabled) noexcept -> void {
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
        auto point = POINT { core::as<long>(position->x), core::as<long>(position->y) };
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
    auto loadMonitor(HMONITOR _monitor,
                     [[maybe_unused]] HDC hdc,
                     [[maybe_unused]] LPRECT rect,
                     LPARAM data) -> BOOL {
        if (_monitor == nullptr) return TRUE;

        auto &monitors = *reinterpret_cast<std::vector<Monitor> *>(data);

        auto monitor_info = MONITORINFOEX {};
        ZeroMemory(&monitor_info, sizeof(MONITORINFOEX));
        monitor_info.cbSize = sizeof(MONITORINFOEX);

        GetMonitorInfo(_monitor, &monitor_info);

        auto &monitor  = monitors.emplace_back();
        monitor.handle = _monitor;
        if ((monitor_info.dwFlags & MONITORINFOF_PRIMARY) == MONITORINFOF_PRIMARY)
            monitor.flags = Monitor::Flags::Primary;

        monitor.name = std::string { monitor_info.szDevice };

        auto dm = DEVMODE {};
        ZeroMemory(&dm, sizeof(DEVMODE));

        for (auto i = 0; EnumDisplaySettings(monitor_info.szDevice, i, &dm) != 0; ++i) {
            monitor.sizes.emplace_back(core::as<core::UInt32>(dm.dmPelsWidth),
                                       core::as<core::UInt32>(dm.dmPelsHeight));
        }

        monitor.sizes.erase(std::unique(std::begin(monitor.sizes), std::end(monitor.sizes)),
                            std::end(monitor.sizes));
        std::ranges::sort(monitor.sizes);

        return TRUE;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowImpl::getMonitorSettings([[maybe_unused]] Window::WM wm) -> std::vector<Monitor> {
        static auto monitors = std::vector<Monitor> {};
        static auto init     = false;

        if (!init) {
            EnumDisplayMonitors(nullptr, nullptr, loadMonitor, reinterpret_cast<LPARAM>(&monitors));

            init = true;
        }

        return monitors;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowImpl::getPrimaryMonitorSettings(Window::WM wm) -> Monitor {
        const auto settings = getMonitorSettings(wm);

        const auto it = std::ranges::find_if(settings, [](const auto &monitor) {
            return core::checkFlag(monitor.flags, Monitor::Flags::Primary);
        });

        return *it;
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
                auto x = core::as<core::Int16>(LOWORD(l_param));
                auto y = core::as<core::Int16>(HIWORD(l_param));
                mouseDownEvent(MouseButton::Left, x, y);
                break;
            }
            case WM_LBUTTONUP: {
                auto x = core::as<core::Int16>(LOWORD(l_param));
                auto y = core::as<core::Int16>(HIWORD(l_param));
                mouseUpEvent(MouseButton::Left, x, y);
                break;
            }
            case WM_RBUTTONDOWN: {
                auto x = core::as<core::Int16>(LOWORD(l_param));
                auto y = core::as<core::Int16>(HIWORD(l_param));
                mouseDownEvent(MouseButton::Right, x, y);
                break;
            }
            case WM_RBUTTONUP: {
                auto x = core::as<core::Int16>(LOWORD(l_param));
                auto y = core::as<core::Int16>(HIWORD(l_param));
                mouseUpEvent(MouseButton::Right, x, y);
                break;
            }
            case WM_MBUTTONDOWN: {
                auto x = core::as<core::Int16>(LOWORD(l_param));
                auto y = core::as<core::Int16>(HIWORD(l_param));
                mouseDownEvent(MouseButton::Middle, x, y);
                break;
            }
            case WM_MBUTTONUP: {
                auto x = core::as<core::Int16>(LOWORD(l_param));
                auto y = core::as<core::Int16>(HIWORD(l_param));
                mouseUpEvent(MouseButton::Middle, x, y);
                break;
            }
            case WM_XBUTTONDOWN: {
                auto x      = core::as<core::Int16>(LOWORD(l_param));
                auto y      = core::as<core::Int16>(HIWORD(l_param));
                auto button = HIWORD(w_param);
                if (button == XBUTTON1) mouseDownEvent(MouseButton::Button1, x, y);
                else if (button == XBUTTON2)
                    mouseDownEvent(MouseButton::Button1, x, y);
                break;
            }
            case WM_XBUTTONUP: {
                auto x      = core::as<core::Int16>(LOWORD(l_param));
                auto y      = core::as<core::Int16>(HIWORD(l_param));
                auto button = HIWORD(w_param);
                if (button == XBUTTON1) mouseUpEvent(MouseButton::Button2, x, y);
                else if (button == XBUTTON2)
                    mouseUpEvent(MouseButton::Button2, x, y);
                break;
            }
            case WM_MOUSEMOVE: {
                auto x = core::as<core::Int16>(LOWORD(l_param));
                auto y = core::as<core::Int16>(HIWORD(l_param));

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
