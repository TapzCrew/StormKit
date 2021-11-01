// Copryright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution
#pragma once

namespace storm::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto RenderQueue::vertexBufferID() const noexcept -> FrameGraphResourceID {
        return m_vertex_buffer_id;
    }
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto RenderQueue::indexBufferID() const noexcept -> FrameGraphResourceID {
        return m_index_buffer_id;
    }
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto RenderQueue::drawDataBufferID() const noexcept -> FrameGraphResourceID {
        return m_draw_data_buffer_id;
    }
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto RenderQueue::drawCommandBufferID() const noexcept -> FrameGraphResourceID {
        return m_draw_command_buffer_id;
    }
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto RenderQueue::transformBufferID() const noexcept -> FrameGraphResourceID {
        return m_transform_buffer_id;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto RenderQueue::drawCommandCount() const noexcept -> core::ArraySize {
        return std::size(m_draw_commands);
    }
} // namespace storm::engine
