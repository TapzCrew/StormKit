// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "CommandBuffer.mpp"

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto CommandBuffer::state() const noexcept -> State {
        STORMKIT_EXPECTS(m_command_buffer);

        return m_state;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto CommandBuffer::level() const noexcept -> CommandBufferLevel {
        STORMKIT_EXPECTS(m_command_buffer);

        return m_level;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline CommandBuffer::operator VkCommandBuffer() const noexcept { return vkHandle(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto CommandBuffer::vkHandle() const noexcept -> VkCommandBuffer {
        STORMKIT_EXPECTS(m_command_buffer);

        return m_command_buffer;
    }
} // namespace stormkit::gpu
