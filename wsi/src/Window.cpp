// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <stormkit/wsi/Window.hpp>

#include "Log.hpp"

#if defined(STORMKIT_OS_WINDOWS)
    #include "win32/WindowImpl.hpp"
#elif defined(STORMKIT_OS_LINUX)
    #include "linux/wayland/WindowImpl.hpp"
    #include "linux/x11/WindowImpl.hpp"
#endif

#include <stormkit/core/AssertMacro.hpp>

using namespace std::literals;

namespace stormkit::wsi {
    /////////////////////////////////////
    /////////////////////////////////////
    Window::Window() noexcept : m_impl { wm() } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Window::Window(std::string title, const core::ExtentU& size, WindowStyle style) noexcept
        : m_impl { wm() } {
        create(std::move(title), size, style);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Window::~Window() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    Window::Window(Window&&) = default;

    /////////////////////////////////////
    /////////////////////////////////////
    Window& Window::operator=(Window&&) = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::create(std::string title, const core::ExtentU& size, WindowStyle style) noexcept
        -> void {
        m_impl->create(std::move(title), size, style);
    }
    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::close() noexcept -> void {
        m_impl->close();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::pollEvent(Event& event) noexcept -> bool {
        return m_impl->pollEvent(event);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::waitEvent(Event& event) noexcept -> bool {
        return m_impl->waitEvent(event);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::setTitle(std::string title) noexcept -> void {
        m_impl->setTitle(std::move(title));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::setExtent(const core::ExtentU& extent) noexcept -> void {
        m_impl->setExtent(extent);
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
    auto Window::extent() const noexcept -> const core::ExtentU& {
        return m_impl->extent();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::isOpen() const noexcept -> bool {
        return m_impl->isOpen();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::visible() const noexcept -> bool {
        return m_impl->visible();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::nativeHandle() const noexcept -> NativeHandle {
        return m_impl->nativeHandle();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Window::WM Window::wm() noexcept {
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
        auto is_wayland = std::getenv("WAYLAND_DISPLAY") != nullptr;

        if (is_wayland) return WM::Wayland;
        else
            return WM::X11;
#endif
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::title() const noexcept -> const std::string& {
        return m_impl->title();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::mouseLocked() const noexcept -> bool {
        return m_impl->mouseLocked();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::fullscreen() const noexcept -> bool {
        return m_impl->fullscreen();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::setKeyRepeatEnabled(bool enabled) noexcept -> void {
        return m_impl->setKeyRepeatEnabled(enabled);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::keyRepeatEnabled() const noexcept -> bool {
        return m_impl->keyRepeatEnabled();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::setVirtualKeyboardVisible(bool visible) noexcept -> void {
        m_impl->setVirtualKeyboardVisible(visible);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::setMousePosition(const core::Position2i& position) noexcept -> void {
        m_impl->setMousePosition(position);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::setMousePositionOnDesktop(const core::Position2u& position) noexcept -> void {
        details::WindowImpl::setMousePositionOnDesktop(wm(), position);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::getMonitorSettings() -> std::vector<Monitor> {
        return details::WindowImpl::getMonitorSettings(wm());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Window::getPrimaryMonitorSettings() -> Monitor {
        const auto settings = getMonitorSettings();

        const auto it = std::ranges::find_if(settings, [](const auto& monitor) {
            return core::checkFlag(monitor.flags, Monitor::Flags::Primary);
        });

        STORMKIT_ENSURES(it != std::ranges::cend(settings));

        return *it;
    }

} // namespace stormkit::wsi
