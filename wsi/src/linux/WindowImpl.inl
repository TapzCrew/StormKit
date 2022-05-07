// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#define CASE_DO(case_, namespace_, work)               \
    case case_:                                        \
        core::as<namespace_::WindowImpl>(m_impl).work; \
        break
#define CASE_DO_RETURN(case_, namespace_, work) \
    case case_: return core::as<namespace_::WindowImpl>(m_impl).work

namespace stormkit::wsi::details {
    /////////////////////////////////////
    /////////////////////////////////////
    inline WindowImpl::WindowImpl(Window::WM wm) : m_wm { wm }, m_impl { Sentinelle {} } {
        if (m_wm == Window::WM::X11) m_impl = x11::WindowImpl {};
        else if (m_wm == Window::WM::Wayland)
            m_impl = wayland::WindowImpl {};
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline WindowImpl::~WindowImpl() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    inline WindowImpl::WindowImpl(Window::WM wm,
                                  std::string title,
                                  const VideoSettings &settings,
                                  WindowStyle style)
        : m_wm { wm }, m_impl { Sentinelle {} } {
        if (m_wm == Window::WM::X11) m_impl = x11::WindowImpl { std::move(title), settings, style };
        else if (m_wm == Window::WM::Wayland)
            m_impl = wayland::WindowImpl { title, settings, style };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline WindowImpl::WindowImpl(WindowImpl &&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::operator=(WindowImpl &&) noexcept -> WindowImpl & = default;

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::create(std::string title,
                                   const VideoSettings &settings,
                                   WindowStyle style) -> void {
        switch (m_wm) {
            CASE_DO(Window::WM::X11, x11, create(std::move(title), settings, style));
            CASE_DO(Window::WM::Wayland, wayland, create(std::move(title), settings, style));

            default: break;
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::close() noexcept -> void {
        switch (m_wm) {
            CASE_DO(Window::WM::X11, x11, close());
            CASE_DO(Window::WM::Wayland, wayland, close());

            default: break;
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::pollEvent(Event &event) noexcept -> bool {
        switch (m_wm) {
            CASE_DO_RETURN(Window::WM::X11, x11, pollEvent(event));
            CASE_DO_RETURN(Window::WM::Wayland, wayland, pollEvent(event));

            default: break;
        }

        return false;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::waitEvent(Event &event) noexcept -> bool {
        switch (m_wm) {
            CASE_DO_RETURN(Window::WM::X11, x11, waitEvent(event));
            CASE_DO_RETURN(Window::WM::Wayland, wayland, waitEvent(event));

            default: break;
        }

        return false;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::setTitle(std::string title) noexcept -> void {
        switch (m_wm) {
            CASE_DO(Window::WM::X11, x11, setTitle(std::move(title)));
            CASE_DO(Window::WM::Wayland, wayland, setTitle(std::move(title)));

            default: break;
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::setFullscreenEnabled(bool enabled) noexcept -> void {
        switch (m_wm) {
            CASE_DO(Window::WM::X11, x11, setFullscreenEnabled(enabled));
            CASE_DO(Window::WM::Wayland, wayland, setFullscreenEnabled(enabled));

            default: break;
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::lockMouse() noexcept -> void {
        switch (m_wm) {
            CASE_DO(Window::WM::X11, x11, lockMouse());
            CASE_DO(Window::WM::Wayland, wayland, lockMouse());

            default: break;
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::unlockMouse() noexcept -> void {
        switch (m_wm) {
            CASE_DO(Window::WM::X11, x11, unlockMouse());
            CASE_DO(Window::WM::Wayland, wayland, unlockMouse());

            default: break;
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::hideMouse() noexcept -> void {
        switch (m_wm) {
            CASE_DO(Window::WM::X11, x11, hideMouse());
            CASE_DO(Window::WM::Wayland, wayland, hideMouse());

            default: break;
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::unhideMouse() noexcept -> void {
        switch (m_wm) {
            CASE_DO(Window::WM::X11, x11, unhideMouse());
            CASE_DO(Window::WM::Wayland, wayland, unhideMouse());

            default: break;
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::size() const noexcept -> const stormkit::core::ExtentU & {
        switch (m_wm) {
            CASE_DO_RETURN(Window::WM::X11, x11, size());
            CASE_DO(Window::WM::Wayland, wayland, size());

            default: break;
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::isOpen() const noexcept -> bool {
        switch (m_wm) {
            CASE_DO_RETURN(Window::WM::X11, x11, isOpen());
            CASE_DO_RETURN(Window::WM::Wayland, wayland, isOpen());

            default: break;
        }

        return false;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::visible() const noexcept -> bool {
        switch (m_wm) {
            CASE_DO_RETURN(Window::WM::X11, x11, visible());
            CASE_DO_RETURN(Window::WM::Wayland, wayland, visible());

            default: break;
        }

        return false;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::nativeHandle() const noexcept -> Window::NativeHandle {
        switch (m_wm) {
            CASE_DO_RETURN(Window::WM::X11, x11, nativeHandle());
            CASE_DO_RETURN(Window::WM::Wayland, wayland, nativeHandle());

            default: break;
        }

        return nullptr;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::mouseLocked() const noexcept -> bool {
        switch (m_wm) {
            CASE_DO_RETURN(Window::WM::X11, x11, mouseLocked());
            CASE_DO_RETURN(Window::WM::Wayland, wayland, mouseLocked());

            default: break;
        }

        return false;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::fullscreen() const noexcept -> bool {
        switch (m_wm) {
            CASE_DO_RETURN(Window::WM::X11, x11, fullscreen());
            CASE_DO_RETURN(Window::WM::Wayland, wayland, fullscreen());

            default: break;
        }

        return false;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::title() const noexcept -> std::string_view {
        switch (m_wm) {
            CASE_DO_RETURN(Window::WM::X11, x11, title());
            CASE_DO_RETURN(Window::WM::Wayland, wayland, title());

            default: break;
        }

        return "";
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::videoSettings() const noexcept -> const VideoSettings & {
        switch (m_wm) {
            CASE_DO_RETURN(Window::WM::X11, x11, videoSettings());
            CASE_DO_RETURN(Window::WM::Wayland, wayland, videoSettings());

            default: break;
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::setKeyRepeatEnabled(bool enabled) noexcept -> void {
        switch (m_wm) {
            CASE_DO_RETURN(Window::WM::X11, x11, setKeyRepeatEnabled(enabled));
            CASE_DO_RETURN(Window::WM::Wayland, wayland, setKeyRepeatEnabled(enabled));

            default: break;
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::keyRepeatEnabled() const noexcept -> bool {
        switch (m_wm) {
            CASE_DO_RETURN(Window::WM::X11, x11, keyRepeatEnabled());
            CASE_DO_RETURN(Window::WM::Wayland, wayland, keyRepeatEnabled());

            default: break;
        }

        return false;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::setVirtualKeyboardVisible(bool visible) noexcept -> void {
        switch (m_wm) {
            CASE_DO(Window::WM::X11, x11, setVirtualKeyboardVisible(visible));
            CASE_DO(Window::WM::Wayland, wayland, setVirtualKeyboardVisible(visible));

            default: break;
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::setMousePosition(core::Position2i position) noexcept -> void {
        switch (m_wm) {
            CASE_DO(Window::WM::X11, x11, setMousePosition(position));
            CASE_DO(Window::WM::Wayland, wayland, setMousePosition(position));

            default: break;
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::setMousePositionOnDesktop(Window::WM wm,
                                                      core::Position2u position) noexcept -> void {
        switch (wm) {
            case Window::WM::X11:
                details::x11::WindowImpl::setMousePositionOnDesktop(position);
                break;
            case Window::WM::Wayland:
                details::wayland::WindowImpl::setMousePositionOnDesktop(position);
                break;

            default: break;
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::getDesktopModes(Window::WM wm) -> std::vector<VideoSettings> {
        switch (wm) {
            case Window::WM::X11: return details::x11::WindowImpl::getDesktopModes(); break;
            case Window::WM::Wayland: return details::wayland::WindowImpl::getDesktopModes(); break;
            default: break;
        }

        return {};
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto WindowImpl::getDesktopFullscreenSize(Window::WM wm) -> VideoSettings {
        switch (wm) {
            case Window::WM::X11: return details::x11::WindowImpl::getDesktopFullscreenSize();
            case Window::WM::Wayland:
                return details::wayland::WindowImpl::getDesktopFullscreenSize();

            default: break;
        }

        return {};
    }
} // namespace stormkit::wsi::details

#undef CASE_DO
#undef CASE_DO_RETURN
