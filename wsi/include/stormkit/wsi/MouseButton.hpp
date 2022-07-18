// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/FormatMacro.hpp>

#include <string_view>

#include <stormkit/core/FrozenMap.hpp>
#include <stormkit/core/Format.hpp>
#include <stormkit/core/Types.hpp>

namespace stormkit::wsi {
    enum class MouseButton : core::UInt8 {
        Unknow,
        Left,
        Right,
        Middle,
        Button1,
        Button2,
        ButtonCount
    };

    static constexpr auto MOUSE_BUTTON_COUNT = core::as<core::USize>(MouseButton::ButtonCount);

    ////////////////////////////////////////
    ////////////////////////////////////////
    [[nodiscard]] constexpr auto mouseButtonToString(MouseButton value) -> std::string_view;
} // namespace stormkit::wsi

CUSTOM_FORMAT(stormkit::wsi::MouseButton, stormkit::wsi::mouseButtonToString(data));

#include "MouseButton.inl"
