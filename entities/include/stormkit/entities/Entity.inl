// Copryright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "Entity.mpp"

namespace stormkit::entities {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto EntityHashFunc::operator()(Entity k) const noexcept -> core::Hash64 {
        return core::as<core::Hash64>(k);
    }
} // namespace stormkit::entities
