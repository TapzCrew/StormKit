// Copryright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::core {
    template<class T, template<class> class CRTP_TYPE>
    struct CRTP {
        constexpr auto underlying() noexcept -> T &;
        constexpr auto underlying() const noexcept -> const T &;
    };
} // namespace stormkit::core

#include "CRTP.inl"
