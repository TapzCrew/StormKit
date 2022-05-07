// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
    /////////////////////////////////////
    /////////////////////////////////////
    vk::Fence Fence::vkFence() const noexcept {
        STORMKIT_EXPECTS(m_vk_fence);
        return *m_vk_fence;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Fence::operator vk::Fence() const noexcept {
        STORMKIT_EXPECTS(m_vk_fence);
        return *m_vk_fence;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    vk::Fence Fence::vkHandle() const noexcept { return vkFence(); }

    /////////////////////////////////////
    /////////////////////////////////////
    core::UInt64 Fence::vkDebugHandle() const noexcept {
        return reinterpret_cast<core::UInt64>(vkHandle().operator VkFence_T *());
    }
} // namespace storm::render
