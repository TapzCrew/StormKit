// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::render {
    /////////////////////////////////////
    /////////////////////////////////////
    inline Semaphore::operator VkSemaphore() const noexcept { return vkHandle(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Semaphore::vkHandle() const noexcept -> VkSemaphore {
        STORMKIT_EXPECTS(m_semaphore);

        return m_semaphore;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    constexpr auto Semaphore::vkObjectType() const noexcept -> VkObjectType {
        return VK_OBJECT_TYPE_SEMAPHORE;
    }
} // namespace stormkit::render
