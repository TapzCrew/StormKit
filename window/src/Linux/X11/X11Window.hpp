// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "../../AbstractWindow.hpp"
#include "../Utils.hpp"

/////////// - STL - ///////////
#include <functional>
#include <memory>

/////////// - StormKit::core - ///////////
#include <storm/core/Platform.hpp>

/////////// - XCB - ///////////
#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>

/////////// - XKB - ///////////
#include <xkbcommon/xkbcommon.h>

namespace storm::window::details {
    STORMKIT_RAII_CAPSULE(XCBConnection, xcb_connection_t, xcb_disconnect)
    STORMKIT_RAII_CAPSULE(XCBKeySymbols, xcb_key_symbols_t, xcb_key_symbols_free)

    class STORMKIT_PRIVATE X11Window final: public AbstractWindow {
      public:
        struct Handles {
            xcb_connection_t *connection;
            xcb_window_t window;
            xcb_key_symbols_t *key_symbols;
            xkb_state *state;
        };

        X11Window();
        X11Window(std::string title,
                  const VideoSettings &settings,
                  storm::window::WindowStyle style);
        ~X11Window() override;

        X11Window(X11Window &&) noexcept;
        X11Window &operator=(X11Window &&) noexcept;

        void create(std::string title, const VideoSettings &settings, WindowStyle style) override;
        void close() noexcept override;

        [[nodiscard]] bool pollEvent(Event &event) noexcept override;
        [[nodiscard]] bool waitEvent(Event &event) noexcept override;

        void setTitle(std::string title) noexcept override;
        void setFullscreenEnabled(bool enabled) noexcept override;

        void lockMouse() noexcept override;
        void unlockMouse() noexcept override;

        void hideMouse() noexcept override;
        void unhideMouse() noexcept override;

        [[nodiscard]] const core::Extentu &size() const noexcept override;

        [[nodiscard]] bool isOpen() const noexcept override;
        [[nodiscard]] bool isVisible() const noexcept override;

        [[nodiscard]] NativeHandle nativeHandle() const noexcept override;
        [[nodiscard]] const Handles &xcbHandles() const noexcept { return m_handles; }

        [[nodiscard]] static std::vector<VideoSettings> getDesktopModes();
        [[nodiscard]] static VideoSettings getDesktopFullscreenSize();

        ALLOCATE_HELPERS(X11Window)
      private:
        void processEvents(xcb_generic_event_t xevent);
        void updateKeymap();
        void updateXKBMods();

        XCBConnectionScoped m_connection;
        xcb_window_t m_window;
        XCBKeySymbolsScoped m_key_symbols;

        bool m_has_xkb = false;
        XKBContextScoped m_xkb_context;
        XKBKeymapScoped m_xkb_keymap;
        XKBStateScoped m_xkb_state;

        struct XKBMods {
            xkb_mod_index_t shift;
            xkb_mod_index_t lock;
            xkb_mod_index_t control;
            xkb_mod_index_t mod1;
            xkb_mod_index_t mod2;
            xkb_mod_index_t mod3;
            xkb_mod_index_t mod4;
            xkb_mod_index_t mod5;
        } m_xkb_mods;

        xcb_intern_atom_cookie_t m_protocol_cookie;
        xcb_intern_atom_reply_t *m_protocol_reply = nullptr;

        xcb_intern_atom_cookie_t m_close_cookie;
        xcb_intern_atom_reply_t *m_close_reply = nullptr;

        xcb_intern_atom_cookie_t m_state_cookie;
        xcb_intern_atom_reply_t *m_state_reply = nullptr;

        xcb_intern_atom_cookie_t m_state_fullscreen_cookie;
        xcb_intern_atom_reply_t *m_state_fullscreen_reply = nullptr;

        struct WindowHints {
            uint32_t flags       = 0;
            uint32_t functions   = 0;
            uint32_t decorations = 0;
            int32_t input_mode   = 0;
            uint32_t state       = 0;
        } m_window_hints;

        core::Extentu m_extent;

        bool m_is_open;
        bool m_is_visible;

        Handles m_handles;

        mutable core::Int16 m_mouse_x;
        mutable core::Int16 m_mouse_y;
    };

    DECLARE_PTR_AND_REF(X11Window)
} // namespace storm::window::details
