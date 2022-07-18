// Copryright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once


#include <stormkit/core/AsCast.hpp>
#include <stormkit/core/Types.hpp>

namespace stormkit::entities {
    using Entity                                 = core::UInt32;
    constexpr static const Entity INVALID_ENTITY = 0;

    struct EntityHashFunc {
        [[nodiscard]] auto operator()(Entity k) const noexcept -> core::Hash64;
    };
} // namespace stormkit::entities

#include "Entity.inl"
