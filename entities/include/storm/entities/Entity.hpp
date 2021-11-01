// Copryright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Types.hpp>

namespace storm::entities {
    using Entity                                 = core::UInt32;
    constexpr static const Entity INVALID_ENTITY = 0;

    struct EntityHashFunc {
        [[nodiscard]] core::Hash64 operator()(Entity k) const noexcept {
            return static_cast<core::Hash64>(k);
        }
    };
} // namespace storm::entities
