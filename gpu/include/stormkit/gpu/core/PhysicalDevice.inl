// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "PhysicalDevice.mpp"

namespace stormkit::gpu {
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
    constexpr auto
        computeUniformBufferOffsetAlignement(core::USize size,
                                             const RenderCapabilities &capabilities) noexcept
        -> core::USize {
        const auto min_ubo_align = capabilities.limits.min_uniform_buffer_offset_alignment;

        if (min_ubo_align > 0) size = (size + min_ubo_align - 1) & ~(min_ubo_align - 1);

        return size;
    }
} // namespace stormkit::gpu
