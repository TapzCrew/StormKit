// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::render {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto PhysicalDevice::info() const noexcept -> const PhysicalDeviceInfo & {
        return m_device_info;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto PhysicalDevice::capabilities() const noexcept -> const RenderCapabilities & {
        return m_capabilities;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto PhysicalDevice::memoryProperties() const noexcept
        -> std::span<const MemoryPropertyFlag> {
        return m_memory_properties;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto PhysicalDevice::queueFamilies() const noexcept -> std::span<const QueueFamily> {
        return m_queue_families;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline PhysicalDevice::operator VkPhysicalDevice() const noexcept { return vkHandle(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto PhysicalDevice::vkHandle() const noexcept -> VkPhysicalDevice {
        return m_physical_device;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    constexpr auto PhysicalDevice::vkObjectType() const noexcept -> VkObjectType {
        return VK_OBJECT_TYPE_PHYSICAL_DEVICE;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    constexpr auto computeUniformBufferOffsetAlignement(core::USize size,
                                                        const RenderCapabilities &capabilities)
        -> core::USize {
        if (size < capabilities.limits.min_uniform_buffer_offset_alignment)
            size = capabilities.limits.min_uniform_buffer_offset_alignment;
        else if (size > capabilities.limits.min_uniform_buffer_offset_alignment)
            size = capabilities.limits.min_uniform_buffer_offset_alignment *
                   ((size / capabilities.limits.min_uniform_buffer_offset_alignment) + 1);

        return size;
    }
} // namespace stormkit::render
