// Copryright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include <gsl/string_span>

#include <storm/core/Hash.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/Types.hpp>

#include <storm/entities/Fwd.hpp>

namespace storm::entities {
    struct STORMKIT_PUBLIC Component {
        Component();

        Component(const Component &);
        Component &operator=(const Component &);

        Component(Component &&);
        Component &operator=(Component &&);

        using Type = core::UInt64;

        static constexpr Type INVALID_TYPE = 0;
        static constexpr Type TYPE         = INVALID_TYPE;
    };

    template<typename Result, core::ArraySize size>
    constexpr inline Result componentHash(gsl::czstring_span<size> str) noexcept {
        return size == 0 ? 0xcbf29ce484222325UL
                         : (static_cast<core::Hash64>(str[0]) ^
                            componentHash<Result, std::size(str) - 1>(str[1])) *
                               0x100000001b3UL;
    }

    constexpr Component::Type componentHash(const char *str, core::ArraySize size) noexcept {
        return size == 0
                   ? 0xcbf29ce484222325UL
                   : (static_cast<core::ArraySize>(str[0]) ^ componentHash(str + 1, size - 1)) *
                         0x100000001b3UL;
    }

    constexpr Component::Type componentHash(std::string_view str) noexcept {
        return componentHash(std::data(str), std::size(str));
    }

    namespace literals {
        constexpr Component::Type operator"" _component_type(const char *str,
                                                             core::ArraySize size) {
            return storm::entities::componentHash(str, size);
        }
    } // namespace literals
} // namespace storm::entities
