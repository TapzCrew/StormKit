// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/Strings.hpp>

namespace storm::window {
    enum class MouseButton : core::UInt8 { Unknow, Left, Right, Middle, Button1, Button2 };

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto mouseButtonToString(storm::window::MouseButton value) -> std::string {
        using namespace storm::window;

        switch (value) {
            case MouseButton::Left: return "Left";
            case MouseButton::Right: return "Right";
            case MouseButton::Middle: return "Middle";
            case MouseButton::Button1: return "Button1";
            case MouseButton::Button2: return "Button2";
            case MouseButton::Unknow: return "Unknow";
        }

        return "Unknow";
    }
} // namespace storm::window

CUSTOM_FORMAT(storm::window::MouseButton, storm::window::mouseButtonToString(data));
