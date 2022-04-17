// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::render {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Sampler::settings() const noexcept -> const Settings & {
        STORMKIT_EXPECTS(m_device);
        STORMKIT_EXPECTS(m_sampler);

        return m_settings;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline Sampler::operator VkSampler() const noexcept { return vkHandle(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Sampler::vkHandle() const noexcept -> VkSampler {
        STORMKIT_EXPECTS(m_device);
        STORMKIT_EXPECTS(m_sampler);

        return m_sampler;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    constexpr auto Sampler::vkObjectType() const noexcept -> VkObjectType {
        return VK_OBJECT_TYPE_SAMPLER;
    }
} // namespace stormkit::render
