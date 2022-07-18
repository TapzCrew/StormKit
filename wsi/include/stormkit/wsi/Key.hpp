// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/FormatMacro.hpp>

#undef DELETE

#include <string_view>

#include <stormkit/core/AsCast.hpp>
#include <stormkit/core/Format.hpp>
#include <stormkit/core/FrozenMap.hpp>
#include <stormkit/core/Types.hpp>

namespace stormkit::wsi {
    enum class Key : core::Int8 {
        A = 0,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,
        Num0,
        Num1,
        Num2,
        Num3,
        Num4,
        Num5,
        Num6,
        Num7,
        Num8,
        Num9,
        Escape,
        L_Control,
        L_Shift,
        L_Alt,
        L_System,
        R_Control,
        R_Shift,
        R_Alt,
        R_System,
        Menu,
        L_Bracket,
        R_Bracket,
        Semi_Colon,
        Comma,
        Period,
        Quote,
        Slash,
        Back_Slash,
        Tilde,
        Equal,
        Hyphen,
        Space,
        Enter,
        Back_Space,
        Tab,
        Page_Up,
        Page_Down,
        Begin,
        End,
        Home,
        Insert,
        Delete,
        Add,
        Substract,
        Multiply,
        Divide,
        Left,
        Right,
        Up,
        Down,
        Numpad0,
        Numpad1,
        Numpad2,
        Numpad3,
        Numpad4,
        Numpad5,
        Numpad6,
        Numpad7,
        Numpad8,
        Numpad9,
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,
        F13,
        F14,
        F15,
        Pause,
        Keycount,
        Unknow = std::numeric_limits<core::Int8>::max(),
    };

    static constexpr auto KEY_COUNT = core::as<core::USize>(Key::Keycount);

    ////////////////////////////////////////
    ////////////////////////////////////////
    constexpr auto keyToString(Key value) -> std::string_view; // namespace stormkit::wsi

} // namespace stormkit::wsi

CUSTOM_FORMAT(stormkit::wsi::Key, stormkit::wsi::keyToString(data));

#include "Key.inl"
