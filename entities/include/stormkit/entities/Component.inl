// Copryright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "Component.mpp"

namespace stormkit::entities {
    /////////////////////////////////////
    /////////////////////////////////////
    template<typename Result>
    constexpr auto componentHash(std::string_view str) noexcept -> Result {
        return std::empty(str) ? 0xcbf29ce484222325UL
                               : (core::as<core::Hash64>(str[0]) ^
                                  componentHash<Result>(str.substr(1, std::size(str) - 1))) *
                                     0x100000001b3UL;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    constexpr auto componentHash(const char *str, core::USize size) noexcept -> Component::Type {
        return size == 0 ? 0xcbf29ce484222325UL
                         : (core::as<core::USize>(str[0]) ^
                            componentHash(std::string_view { str + 1, size - 1 })) *
                               0x100000001b3UL;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    constexpr auto componentHash(std::string_view str) noexcept -> Component::Type {
        return componentHash(std::data(str), std::size(str));
    }

    namespace literals {
        /////////////////////////////////////
        /////////////////////////////////////
        constexpr auto operator"" _component_type(const char *str, core::USize size)
            -> Component::Type {
            return stormkit::entities::componentHash(str, size);
        }
    } // namespace literals
} // namespace stormkit::entities
