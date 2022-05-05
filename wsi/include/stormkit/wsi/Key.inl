// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "Key.mpp"

namespace stormkit::wsi {
    ////////////////////////////////////////
    ////////////////////////////////////////
    constexpr auto keyToString(Key value) -> std::string_view {
        switch (value) {
            case Key::Unknow: return "Unknow";
            case Key::A: return "A";
            case Key::B: return "B";
            case Key::C: return "C";
            case Key::D: return "D";
            case Key::E: return "E";
            case Key::F: return "F";
            case Key::G: return "G";
            case Key::H: return "H";
            case Key::I: return "I";
            case Key::J: return "J";
            case Key::K: return "K";
            case Key::L: return "L";
            case Key::M: return "M";
            case Key::N: return "N";
            case Key::O: return "O";
            case Key::P: return "P";
            case Key::Q: return "Q";
            case Key::R: return "R";
            case Key::S: return "S";
            case Key::T: return "T";
            case Key::U: return "U";
            case Key::V: return "V";
            case Key::W: return "W";
            case Key::X: return "X";
            case Key::Y: return "Y";
            case Key::Z: return "Z";
            case Key::Num0: return "Num0";
            case Key::Num1: return "Num1";
            case Key::Num2: return "Num2";
            case Key::Num3: return "Num3";
            case Key::Num4: return "Num4";
            case Key::Num5: return "Num5";
            case Key::Num6: return "Num6";
            case Key::Num7: return "Num7";
            case Key::Num8: return "Num8";
            case Key::Num9: return "Num9";
            case Key::Escape: return "Escape";
            case Key::L_Control: return "L_Control";
            case Key::L_Shift: return "L_Shift";
            case Key::L_Alt: return "L_Alt";
            case Key::L_System: return "L_System";
            case Key::R_Control: return "R_Control";
            case Key::R_Shift: return "R_Shift";
            case Key::R_Alt: return "R_Alt";
            case Key::R_System: return "R_System";
            case Key::Menu: return "Menu";
            case Key::L_Bracket: return "L_Bracket";
            case Key::R_Bracket: return "R_Bracket";
            case Key::Semi_Colon: return "Semi_Colon";
            case Key::Comma: return "Comma";
            case Key::Period: return "Period";
            case Key::Quote: return "Quote";
            case Key::Slash: return "Slash";
            case Key::Back_Slash: return "Back_Slash";
            case Key::Tilde: return "Tilde";
            case Key::Equal: return "Equal";
            case Key::Hyphen: return "Hyphen";
            case Key::Space: return "Space";
            case Key::Enter: return "Enter";
            case Key::Back_Space: return "Back_Space";
            case Key::Tab: return "Tab";
            case Key::Page_Up: return "Page_Up";
            case Key::Page_Down: return "Page_Down";
            case Key::Begin: return "Begin";
            case Key::End: return "End";
            case Key::Home: return "Home";
            case Key::Insert: return "Insert";
            case Key::Delete: return "Delete";
            case Key::Add: return "Add";
            case Key::Substract: return "Substract";
            case Key::Multiply: return "Multiply";
            case Key::Divide: return "Divide";
            case Key::Left: return "Left";
            case Key::Right: return "Right";
            case Key::Up: return "Up";
            case Key::Down: return "Down";
            case Key::Numpad0: return "Numpad0";
            case Key::Numpad1: return "Numpad1";
            case Key::Numpad2: return "Numpad2";
            case Key::Numpad3: return "Numpad3";
            case Key::Numpad4: return "Numpad4";
            case Key::Numpad5: return "Numpad5";
            case Key::Numpad6: return "Numpad6";
            case Key::Numpad7: return "Numpad7";
            case Key::Numpad8: return "Numpad8";
            case Key::Numpad9: return "Numpad9";
            case Key::F1: return "F1";
            case Key::F2: return "F2";
            case Key::F3: return "F3";
            case Key::F4: return "F4";
            case Key::F5: return "F5";
            case Key::F6: return "F6";
            case Key::F7: return "F7";
            case Key::F8: return "F8";
            case Key::F9: return "F9";
            case Key::F10: return "F10";
            case Key::F11: return "F11";
            case Key::F12: return "F12";
            case Key::F13: return "F13";
            case Key::F14: return "F14";
            case Key::F15: return "F15";
            case Key::Pause: return "PAUSE";
            case Key::Keycount: return "KEYCOUNT";
        }

        return "UNKNOWN KEY";
    }

} // namespace stormkit::wsi
