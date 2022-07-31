// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <stormkit/core/MemoryMacro.hpp>

/////////// - XCB - ///////////
#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>

/////////// - XKB - ///////////
#include <xkbcommon/xkbcommon.h>

/////////// - STL - ///////////
#include <functional>

/////////// - StormKit::core - ///////////
#include <stormkit/core/Math.hpp>
#include <stormkit/core/Types.hpp>

/////////// - StormKit::wsi - ///////////
#include <stormkit/wsi/Event.hpp>
#include <stormkit/wsi/Fwd.hpp>
#include <stormkit/wsi/Monitor.hpp>
#include <stormkit/wsi/Window.hpp>
#include <stormkit/wsi/WindowStyle.hpp>

#include "XCB.hpp"

#include "../XKB.hpp"

#include "../../common/WindowImplBase.hpp"

namespace stormkit::wsi::details::x11 {
    STORMKIT_RAII_CAPSULE_OPAQUE(
        XCBWindow,
        xcb_window_t,
        []() noexcept { return XCB_WINDOW_NONE; },
        [](auto window) noexcept { xcb_destroy_window(getXCBConnection(), window); },
        XCB_WINDOW_NONE)
    STORMKIT_RAII_CAPSULE(XCBKeySymbols, xcb_key_symbols_t, xcb_key_symbols_free)

    class WindowImpl: public WindowImplBase {
      public:
        struct Handles {
            xcb_connection_t *connection;
            xcb_window_t window;
            xcb_key_symbols_t *key_symbols;
            xkb_state *state;
        };

        WindowImpl();
        WindowImpl(std::string title, const core::ExtentU& size, WindowStyle style);
        ~WindowImpl();

        WindowImpl(const WindowImpl&) noexcept                    = delete;
        auto operator=(const WindowImpl&) noexcept -> WindowImpl& = delete;

        WindowImpl(WindowImpl&&) noexcept;
        auto operator=(WindowImpl&&) noexcept -> WindowImpl&;

        auto create(std::string title, const core::ExtentU& size, WindowStyle style) -> void;
        auto close() noexcept -> void;

        [[nodiscard]] auto pollEvent(Event& event) noexcept -> bool;
        [[nodiscard]] auto waitEvent(Event& event) noexcept -> bool;

        auto setTitle(std::string title) noexcept -> void;
        auto setExtent(const core::ExtentU& size) noexcept -> void;
        auto setFullscreenEnabled(bool enabled) noexcept -> void;

        auto lockMouse() noexcept -> void;
        auto unlockMouse() noexcept -> void;

        auto hideMouse() noexcept -> void;
        auto unhideMouse() noexcept -> void;

        [[nodiscard]] auto extent() const noexcept -> const core::ExtentU&;
        [[nodiscard]] auto title() const noexcept -> const std::string&;

        [[nodiscard]] auto isOpen() const noexcept -> bool;
        [[nodiscard]] auto visible() const noexcept -> bool;

        [[nodiscard]] auto nativeHandle() const noexcept -> Window::NativeHandle;
        [[nodiscard]] auto xcbHandles() const noexcept -> const Handles&;

        [[nodiscard]] auto mouseLocked() const noexcept -> bool;
        [[nodiscard]] auto mouseHidden() const noexcept -> bool;
        [[nodiscard]] auto fullscreen() const noexcept -> bool;

        auto setKeyRepeatEnabled(bool enabled) noexcept -> void;
        [[nodiscard]] auto keyRepeatEnabled() const noexcept -> bool;

        auto setVirtualKeyboardVisible(bool visible) noexcept -> void;

        auto setMousePosition(const core::Position2i& position) noexcept -> void;
        static auto setMousePositionOnDesktop(const core::Position2u& position) noexcept -> void;

        [[nodiscard]] static auto getMonitorSettings() -> std::vector<Monitor>;

      private:
        auto processEvents(xcb_generic_event_t *xevent) -> void;
        auto updateKeymap() -> void;

        auto handleKeyEvent(xcb_keycode_t keycode, bool up) noexcept -> void;

        XCBConnectionScoped m_connection = nullptr;
        XKBContextScoped m_xkb_context   = nullptr;
        XCBWindowScoped m_window;
        XCBKeySymbolsScoped m_key_symbols;

        XKBKeymapScoped m_xkb_keymap;
        XKBStateScoped m_xkb_state;

        XKBMods m_xkb_mods;

        xcb_intern_atom_cookie_t m_protocol_cookie;
        xcb_intern_atom_cookie_t m_close_cookie;
        xcb_intern_atom_cookie_t m_state_cookie;
        xcb_intern_atom_cookie_t m_state_fullscreen_cookie;

        struct WindowHints {
            uint32_t flags       = 0;
            uint32_t functions   = 0;
            uint32_t decorations = 0;
            int32_t input_mode   = 0;
            uint32_t state       = 0;
        } m_window_hints;

        std::string m_title;
        bool m_mouse_locked = false;
        bool m_mouse_hided  = false;
        bool m_fullscreen   = false;

        core::ExtentU m_extent;

        int m_xi_opcode;

        bool m_open               = false;
        bool m_visible            = false;
        bool m_key_repeat_enabled = true;

        Handles m_handles;

        core::Vector2i m_locked_mouse_position;
        mutable core::Vector2i m_mouse_position;
    };
} // namespace stormkit::wsi::details::x11

#include "WindowImpl.inl"
