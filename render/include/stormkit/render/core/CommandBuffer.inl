// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::render {
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

    /////////////////////////////////////
    /////////////////////////////////////
    constexpr auto CommandBuffer::vkObjectType() const noexcept -> VkObjectType {
        return VK_OBJECT_TYPE_COMMAND_BUFFER;
    }
} // namespace stormkit::render
