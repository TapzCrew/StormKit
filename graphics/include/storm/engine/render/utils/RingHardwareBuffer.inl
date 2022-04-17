// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename T>
    auto RingHardwareBuffer::upload(core::span<const T> data, core::Int32 offset) -> void {
        m_buffer->upload(data, m_offset + offset);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto RingHardwareBuffer::buffer() -> render::HardwareBuffer & { return *m_buffer; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto RingHardwareBuffer::buffer() const noexcept -> const render::HardwareBuffer & {
        return *m_buffer;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto RingHardwareBuffer::count() const noexcept -> core::ArraySize { return m_count; }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto RingHardwareBuffer::elementSize() const noexcept -> core::ArraySize {
        return m_element_size;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto RingHardwareBuffer::size() const noexcept -> core::ArraySize {
        return m_buffer->size();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    inline auto RingHardwareBuffer::currentOffset() const noexcept -> core::Int32 {
        return m_offset;
    }
} // namespace storm::engine
