// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Buffer::usages() const noexcept -> BufferUsageFlag {
        STORMKIT_EXPECTS(m_buffer);
        STORMKIT_EXPECTS(m_buffer_memory);

        return m_usages;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Buffer::size() const noexcept -> core::USize {
        STORMKIT_EXPECTS(m_buffer);
        STORMKIT_EXPECTS(m_buffer_memory);

        return m_size;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Buffer::map(core::Int64 offset, core::USize size) noexcept -> core::ByteSpan {
        return core::ByteSpan { map(offset), size };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    inline auto Buffer::mapAs(core::Int64 offset) -> T * {
        return reinterpret_cast<T *>(map(offset));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Buffer::data() noexcept -> core::Byte * {
        STORMKIT_EXPECTS(m_buffer);
        STORMKIT_EXPECTS(m_buffer_memory);
        STORMKIT_EXPECTS(m_mapped_pointer);

        return m_mapped_pointer;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Buffer::data() const noexcept -> const core::Byte * {
        STORMKIT_EXPECTS(m_buffer);
        STORMKIT_EXPECTS(m_buffer_memory);
        STORMKIT_EXPECTS(m_mapped_pointer);

        return m_mapped_pointer;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Buffer::data(core::USize size) noexcept -> core::ByteSpan {
        STORMKIT_EXPECTS(m_buffer);
        STORMKIT_EXPECTS(m_buffer_memory);
        STORMKIT_EXPECTS(m_mapped_pointer);

        return core::ByteSpan { m_mapped_pointer, size };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Buffer::data(core::USize size) const noexcept -> core::ByteConstSpan {
        STORMKIT_EXPECTS(m_buffer);
        STORMKIT_EXPECTS(m_buffer_memory);
        STORMKIT_EXPECTS(m_mapped_pointer);

        return core::ByteConstSpan { m_mapped_pointer, size };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    inline auto Buffer::dataAs() noexcept -> T * {
        return reinterpret_cast<T *>(data());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    inline auto Buffer::dataAs() const noexcept -> const T * {
        return reinterpret_cast<const T *>(data());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    inline auto Buffer::upload(const T &data, core::Int64 offset) -> void {
        upload(core::toConstByteSpan(data), offset);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    inline auto Buffer::upload(std::span<const T> data, core::Int64 offset) -> void {
        upload(core::toConstByteSpan(data), offset);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline Buffer::operator VkBuffer() const noexcept { return vkHandle(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Buffer::vkHandle() const noexcept -> VkBuffer {
        STORMKIT_EXPECTS(m_buffer);
        STORMKIT_EXPECTS(m_buffer_memory);

        return m_buffer;
    }
} // namespace stormkit::gpu
