// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
    /////////////////////////////////////
    /////////////////////////////////////
    QueueFlag Queue::type() const noexcept { return m_queue_flag; }

    /////////////////////////////////////
    /////////////////////////////////////
    const Device &Queue::device() const noexcept { return *m_device; }

    /////////////////////////////////////
    /////////////////////////////////////
    core::UInt32 Queue::familyIndex() const noexcept { return m_family_index; }

    /////////////////////////////////////
    /////////////////////////////////////
    vk::Queue Queue::vkQueue() const noexcept {
        STORMKIT_EXPECTS(m_vk_queue);
        return m_vk_queue;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Queue::operator vk::Queue() const noexcept {
        STORMKIT_EXPECTS(m_vk_queue);
        return m_vk_queue;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    vk::Queue Queue::vkHandle() const noexcept { return vkQueue(); }

    /////////////////////////////////////
    /////////////////////////////////////
    vk::CommandPool Queue::vkCommandPool() const noexcept {
        STORMKIT_EXPECTS(m_vk_command_pool);
        return *m_vk_command_pool;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    core::UInt64 Queue::vkDebugHandle() const noexcept {
        return reinterpret_cast<core::UInt64>(vkHandle().operator VkQueue_T *());
    }
} // namespace storm::render
