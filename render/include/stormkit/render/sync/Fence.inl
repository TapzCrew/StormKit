// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::render {
    /////////////////////////////////////
    /////////////////////////////////////
    inline Fence::operator VkFence() const noexcept { return vkHandle(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Fence::vkHandle() const noexcept -> VkFence {
        STORMKIT_EXPECTS(m_fence);
        return m_fence;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    constexpr auto Fence::vkObjectType() const noexcept -> VkObjectType {
        return VK_OBJECT_TYPE_FENCE;
    }
} // namespace stormkit::render
