// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
    /////////////////////////////////////
    /////////////////////////////////////
    const Sampler::Settings &Sampler::settings() const noexcept { return m_settings; }

    /////////////////////////////////////
    /////////////////////////////////////
    vk::Sampler Sampler::vkSampler() const noexcept {
        STORMKIT_EXPECTS(m_vk_sampler);
        return *m_vk_sampler;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Sampler::operator vk::Sampler() const noexcept {
        STORMKIT_EXPECTS(m_vk_sampler);
        return *m_vk_sampler;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    vk::Sampler Sampler::vkHandle() const noexcept { return vkSampler(); }

    /////////////////////////////////////
    /////////////////////////////////////
    core::UInt64 Sampler::vkDebugHandle() const noexcept {
        return reinterpret_cast<core::UInt64>(vkHandle().operator VkSampler_T *());
    }
} // namespace storm::render
