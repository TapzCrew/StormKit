// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto ShaderCache::has(std::string_view name) const noexcept -> bool {
        return std::ranges::any_of(m_shaders,
                                   [name](const auto &pair) { return pair.first == name; });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto ShaderCache::get(std::string_view name) const noexcept -> const gpu::Shader & {
        STORMKIT_EXPECTS(has(name));

        return m_shaders.find(name)->second;
    }
} // namespace stormkit::engine
