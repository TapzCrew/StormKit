// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/RingBuffer.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/Fwd.hpp>

#include <storm/render/core/Enums.hpp>

#include <storm/render/resource/HardwareBuffer.hpp>

namespace storm::engine {
    class STORMKIT_PUBLIC RingHardwareBuffer {
      public:
        RingHardwareBuffer(core::ArraySize count,
                           const render::Device &device,
                           render::HardwareBufferUsage usage,
                           core::ArraySize byte_count,
                           render::MemoryProperty property = render::MemoryProperty::Host_Visible |
                                                             render::MemoryProperty::Device_Local);
        ~RingHardwareBuffer();

        RingHardwareBuffer(RingHardwareBuffer &&) noexcept;
        RingHardwareBuffer &operator=(RingHardwareBuffer &&) noexcept;

        void next();
        core::Byte *map(core::UInt32 offset);
        void unmap();
        template<typename T>
        void upload(core::span<const T> data, core::Int32 offset = 0);

        render::HardwareBuffer &buffer();
        const render::HardwareBuffer &buffer() const noexcept;

        core::ArraySize count() const noexcept;
        core::ArraySize elementSize() const noexcept;
        core::ArraySize size() const noexcept;
        core::Int32 currentOffset() const noexcept;

        DEVICE_CONST_GETTER
      private:
        render::DeviceConstRef m_device;

        core::ArraySize m_count        = 0u;
        core::ArraySize m_element_size = 0u;
        core::Int32 m_offset           = 0u;

        render::HardwareBufferOwnedPtr m_buffer;
    };
} // namespace storm::engine

#include "RingHardwareBuffer.inl"
