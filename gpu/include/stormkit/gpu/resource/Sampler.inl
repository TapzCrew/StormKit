// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "Sampler.mpp"

namespace stormkit::gpu {
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
} // namespace stormkit::gpu
