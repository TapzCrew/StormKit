// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "Fence.mpp"

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    inline Fence::operator VkFence() const noexcept { return vkHandle(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Fence::vkHandle() const noexcept -> VkFence {
        STORMKIT_EXPECTS(m_fence);
        return m_fence;
    }
} // namespace stormkit::gpu
