// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
    /////////////////////////////////////
    /////////////////////////////////////
    inline const Device &HardwareBuffer::device() const noexcept { return *m_device; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline HardwareBufferUsage HardwareBuffer::usage() const noexcept { return m_usage; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline core::ArraySize HardwareBuffer::size() const noexcept { return m_byte_count; }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    inline T *HardwareBuffer::mapAs(core::UInt32 offset) {
        return reinterpret_cast<T *>(map(offset));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline core::Byte *HardwareBuffer::mappedPointer() noexcept { return m_mapped_pointer; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline const core::Byte *HardwareBuffer::mappedPointer() const noexcept {
        return m_mapped_pointer;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    inline T *HardwareBuffer::mappedPointerAs() noexcept {
        return reinterpret_cast<T *>(mappedPointer());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    inline const T *HardwareBuffer::mappedPointerAs() const noexcept {
        return reinterpret_cast<const T *>(mappedPointer());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    inline void HardwareBuffer::upload(core::span<const T> data, core::Int32 offset) {
        const auto size = std::size(data) * sizeof(T);

        auto data_ptr = map(gsl::narrow_cast<core::UInt32>(offset));
        std::memcpy(data_ptr, std::data(data), size);
        flush(offset, size);
        unmap();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline vk::Buffer HardwareBuffer::vkBuffer() const noexcept {
        STORMKIT_EXPECTS(m_vk_buffer);
        return *m_vk_buffer;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline HardwareBuffer::operator vk::Buffer() const noexcept { return vkBuffer(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline vk::Buffer HardwareBuffer::vkHandle() const noexcept { return vkBuffer(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline core::UInt64 HardwareBuffer::vkDebugHandle() const noexcept {
        return reinterpret_cast<core::UInt64>(vkHandle().operator VkBuffer_T *());
    }
} // namespace storm::render
