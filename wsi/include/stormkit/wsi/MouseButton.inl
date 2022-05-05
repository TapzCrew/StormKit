// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "MouseButton.mpp"

namespace stormkit::wsi {
    ////////////////////////////////////////
    ////////////////////////////////////////
    constexpr auto mouseButtonToString(MouseButton value) -> std::string_view {
        switch (value) {
            case MouseButton::Left: return "Left";
            case MouseButton::Right: return "Right";
            case MouseButton::Middle: return "Middle";
            case MouseButton::Button1: return "Button1";
            case MouseButton::Button2: return "Button2";
            case MouseButton::Unknow: return "Unknown key";
        }

        return "Unknown key";
    }
} // namespace stormkit::wsi
