// Copryright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#ifndef STORMKIT_FLAGS_MACRO_HPP
#define STORMKIT_FLAGS_MACRO_HPP

#define FLAG_ENUM(e)                                            \
    template<>                                                  \
    struct stormkit::core::details::EnableBitmaskOperators<e> { \
        constexpr EnableBitmaskOperators() = default;           \
        static constexpr bool enable       = true;              \
    };

#endif