// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::wsi {
    namespace details {
        constexpr static auto MOUSE_BUTTON_TO_STRING = [] {
            using namespace std::literals;

            return core::makeFrozenMap<MouseButton, std::string_view>({
                { MouseButton::Left, "Left"sv },
                { MouseButton::Right, "Right"sv },
                { MouseButton::Middle, "Middle"sv },
                { MouseButton::Button1, "Button1"sv },
                { MouseButton::Button2, "Button2"sv },
                { MouseButton::Unknow, "Unknown button"sv },
            });
        }();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    constexpr auto mouseButtonToString(MouseButton value) -> std::string_view {
        const auto it = details::MOUSE_BUTTON_TO_STRING.find(value);
        if (it == std::ranges::cend(details::MOUSE_BUTTON_TO_STRING)) [[unlikely]]
            return details::MOUSE_BUTTON_TO_STRING.at(MouseButton::Unknow);

        return it->second;
    }
} // namespace stormkit::wsi
