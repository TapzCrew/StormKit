// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
    /////////////////////////////////////
    /////////////////////////////////////
    vk::Semaphore Semaphore::vkSemaphore() const noexcept {
        STORMKIT_EXPECTS(m_vk_semaphore);
        return *m_vk_semaphore;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Semaphore::operator vk::Semaphore() const noexcept {
        STORMKIT_EXPECTS(m_vk_semaphore);
        return *m_vk_semaphore;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    vk::Semaphore Semaphore::vkHandle() const noexcept { return vkSemaphore(); }

    /////////////////////////////////////
    /////////////////////////////////////
    core::UInt64 Semaphore::vkDebugHandle() const noexcept {
        return reinterpret_cast<core::UInt64>(vkHandle().operator VkSemaphore_T *());
    }
} // namespace storm::render
