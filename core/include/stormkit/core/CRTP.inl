// Copryright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::core {
    /////////////////////////////////////
    /////////////////////////////////////
    template<class T, template<class> class CRTP_TYPE>
    constexpr auto CRTP<T, CRTP_TYPE>::underlying() noexcept -> T & {
        return static_cast<T &>(*this);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<class T, template<class> class CRTP_TYPE>
    constexpr auto CRTP<T, CRTP_TYPE>::underlying() const noexcept -> const T & {
        return static_cast<const T &>(*this);
    }
} // namespace stormkit::core
