// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <optional>

#include <storm/core/NonCopyable.hpp>
#include <storm/core/Span.hpp>

#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>
#include <storm/render/core/Vulkan.hpp>

#include <storm/render/resource/Fwd.hpp>

namespace storm::render {
    class STORMKIT_PUBLIC HardwareBuffer: public core::NonCopyable {
      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Buffer;

        HardwareBuffer(const Device &device,
                       HardwareBufferUsage usage,
                       core::ArraySize byte_count,
                       MemoryProperty property = MemoryProperty::Host_Visible |
                                                 MemoryProperty::Host_Coherent,
                       bool persistently_mapping = false);
        ~HardwareBuffer();

        HardwareBuffer(HardwareBuffer &&);
        HardwareBuffer &operator=(HardwareBuffer &&);

        const Device &device() const noexcept;
        HardwareBufferUsage usage() const noexcept;
        core::ArraySize size() const noexcept;

        [[nodiscard]] core::Byte *map(core::UInt32 offset);
        template<typename T>
        [[nodiscard]] T *mapAs(core::UInt32 offset);

        [[nodiscard]] core::Byte *mappedPointer() noexcept;
        [[nodiscard]] const core::Byte *mappedPointer() const noexcept;

        template<typename T>
        [[nodiscard]] T *mappedPointerAs() noexcept;
        template<typename T>
        [[nodiscard]] const T *mappedPointerAs() const noexcept;

        void flush(core::Int32 offset, core::ArraySize byte_count);
        void unmap();

        [[nodiscard]] bool isPersistentlyMapped() const noexcept;

        template<typename T>
        void upload(core::span<const T> data, core::Int32 offset = 0);

        vk::Buffer vkBuffer() const noexcept;
        operator vk::Buffer() const noexcept;
        vk::Buffer vkHandle() const noexcept;
        core::UInt64 vkDebugHandle() const noexcept;

      private:
        static core::UInt32 findMemoryType(core::UInt32 type_filter,
                                           VkMemoryPropertyFlags properties,
                                           const VkPhysicalDeviceMemoryProperties &mem_properties,
                                           const VkMemoryRequirements &mem_requirements);

        DeviceConstPtr m_device;
        HardwareBufferUsage m_usage;
        core::ArraySize m_byte_count;

        bool m_is_persistently_mapped;
        core::Byte *m_mapped_pointer = nullptr;

        RAIIVkBuffer m_vk_buffer;
        RAIIVmaAllocation m_vma_buffer_memory;
    };
} // namespace storm::render

#include "HardwareBuffer.inl"
