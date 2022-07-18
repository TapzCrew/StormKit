// Copryright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/AssertMacro.hpp>
#include <stormkit/core/HashMacro.hpp>
#include <stormkit/core/PlatformMacro.hpp>


#include <array>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>


#include <stormkit/core/Types.hpp>


#include <stormkit/entities/Fwd.hpp>

namespace stormkit::entities {
    struct STORMKIT_PUBLIC Component {
        using Type = core::UInt64;

        static constexpr Type INVALID_TYPE = 0;
        static constexpr Type TYPE         = INVALID_TYPE;
    };
    DECLARE_PTR_AND_REF(Component)

    template<typename Result>
    constexpr auto componentHash(std::string_view str) noexcept -> Result;

    constexpr auto componentHash(const char *str, core::USize size) noexcept -> Component::Type;

    constexpr auto componentHash(std::string_view str) noexcept -> Component::Type;

    namespace literals {
        constexpr auto operator"" _component_type(const char *str, core::USize size)
            -> Component::Type;
    } // namespace literals
} // namespace stormkit::entities

#include "Component.inl"
