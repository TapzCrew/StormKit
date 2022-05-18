// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto DynamicLoader::success() const noexcept -> bool { return m_success; }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    inline auto DynamicLoader::get(std::string_view name) const noexcept -> T {
        STORMKIT_EXPECTS(m_success);

        return m_loader.cFunc<T>(name);
    }
} // namespace stormkit::gpu
