// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "Queue.mpp"

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Queue::type() const noexcept -> QueueFlag { return m_queue_flag; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Queue::familyIndex() const noexcept -> core::UInt32 { return m_family_index; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline Queue::operator VkQueue() const noexcept { return vkHandle(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Queue::vkHandle() const noexcept -> VkQueue {
        STORMKIT_EXPECTS(m_queue);

        return m_queue;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Queue::vkCommandPool() const noexcept -> VkCommandPool {
        STORMKIT_EXPECTS(m_command_pool);

        return m_command_pool;
    }
} // namespace stormkit::gpu
