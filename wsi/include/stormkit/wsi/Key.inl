// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::wsi {
    namespace details {
        constexpr static auto KEY_TO_STRING = [] {
            using namespace std::literals;

            return core::makeFrozenMap<Key, std::string_view>({
                { Key::Unknow, "Unknow key"sv },
                { Key::A, "A"sv },
                { Key::B, "B"sv },
                { Key::C, "C"sv },
                { Key::D, "D"sv },
                { Key::E, "E"sv },
                { Key::F, "F"sv },
                { Key::G, "G"sv },
                { Key::H, "H"sv },
                { Key::I, "I"sv },
                { Key::J, "J"sv },
                { Key::K, "K"sv },
                { Key::L, "L"sv },
                { Key::M, "M"sv },
                { Key::N, "N"sv },
                { Key::O, "O"sv },
                { Key::P, "P"sv },
                { Key::Q, "Q"sv },
                { Key::R, "R"sv },
                { Key::S, "S"sv },
                { Key::T, "T"sv },
                { Key::U, "U"sv },
                { Key::V, "V"sv },
                { Key::W, "W"sv },
                { Key::X, "X"sv },
                { Key::Y, "Y"sv },
                { Key::Z, "Z"sv },
                { Key::Num0, "Num0"sv },
                { Key::Num1, "Num1"sv },
                { Key::Num2, "Num2"sv },
                { Key::Num3, "Num3"sv },
                { Key::Num4, "Num4"sv },
                { Key::Num5, "Num5"sv },
                { Key::Num6, "Num6"sv },
                { Key::Num7, "Num7"sv },
                { Key::Num8, "Num8"sv },
                { Key::Num9, "Num9"sv },
                { Key::Escape, "Escape"sv },
                { Key::L_Control, "L_Control"sv },
                { Key::L_Shift, "L_Shift"sv },
                { Key::L_Alt, "L_Alt"sv },
                { Key::L_System, "L_System"sv },
                { Key::R_Control, "R_Control"sv },
                { Key::R_Shift, "R_Shift"sv },
                { Key::R_Alt, "R_Alt"sv },
                { Key::R_System, "R_System"sv },
                { Key::Menu, "Menu"sv },
                { Key::L_Bracket, "L_Bracket"sv },
                { Key::R_Bracket, "R_Bracket"sv },
                { Key::Semi_Colon, "Semi_Colon"sv },
                { Key::Comma, "Comma"sv },
                { Key::Period, "Period"sv },
                { Key::Quote, "Quote"sv },
                { Key::Slash, "Slash"sv },
                { Key::Back_Slash, "Back_Slash"sv },
                { Key::Tilde, "Tilde"sv },
                { Key::Equal, "Equal"sv },
                { Key::Hyphen, "Hyphen"sv },
                { Key::Space, "Space"sv },
                { Key::Enter, "Enter"sv },
                { Key::Back_Space, "Back_Space"sv },
                { Key::Tab, "Tab"sv },
                { Key::Page_Up, "Page_Up"sv },
                { Key::Page_Down, "Page_Down"sv },
                { Key::Begin, "Begin"sv },
                { Key::End, "End"sv },
                { Key::Home, "Home"sv },
                { Key::Insert, "Insert"sv },
                { Key::Delete, "Delete"sv },
                { Key::Add, "Add"sv },
                { Key::Substract, "Substract"sv },
                { Key::Multiply, "Multiply"sv },
                { Key::Divide, "Divide"sv },
                { Key::Left, "Left"sv },
                { Key::Right, "Right"sv },
                { Key::Up, "Up"sv },
                { Key::Down, "Down"sv },
                { Key::Numpad0, "Numpad0"sv },
                { Key::Numpad1, "Numpad1"sv },
                { Key::Numpad2, "Numpad2"sv },
                { Key::Numpad3, "Numpad3"sv },
                { Key::Numpad4, "Numpad4"sv },
                { Key::Numpad5, "Numpad5"sv },
                { Key::Numpad6, "Numpad6"sv },
                { Key::Numpad7, "Numpad7"sv },
                { Key::Numpad8, "Numpad8"sv },
                { Key::Numpad9, "Numpad9"sv },
                { Key::F1, "F1"sv },
                { Key::F2, "F2"sv },
                { Key::F3, "F3"sv },
                { Key::F4, "F4"sv },
                { Key::F5, "F5"sv },
                { Key::F6, "F6"sv },
                { Key::F7, "F7"sv },
                { Key::F8, "F8"sv },
                { Key::F9, "F9"sv },
                { Key::F10, "F10"sv },
                { Key::F11, "F11"sv },
                { Key::F12, "F12"sv },
                { Key::F13, "F13"sv },
                { Key::F14, "F14"sv },
                { Key::F15, "F15"sv },
                { Key::Pause, "Pause"sv },
                { Key::Keycount, "KEYCOUNT"sv },
            });
        }();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    constexpr auto keyToString(Key value) -> std::string_view {
        const auto it = details::KEY_TO_STRING.find(value);
        if (it == std::ranges::cend(details::KEY_TO_STRING)) [[unlikely]]
            return details::KEY_TO_STRING.at(Key::Unknow);

        return it->second;
    }

} // namespace stormkit::wsi
