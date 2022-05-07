// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
    /////////////////////////////////////
    /////////////////////////////////////
    const PhysicalDeviceInfo &PhysicalDevice::info() const noexcept { return m_device_info; }

    /////////////////////////////////////
    /////////////////////////////////////
    const RenderCapabilities &PhysicalDevice::capabilities() const noexcept {
        return m_capabilities;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    PhysicalDevice::MemoryPropertiesSpan PhysicalDevice::memoryProperties() const noexcept {
        return m_memory_properties;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    storm::core::span<const QueueFamily> PhysicalDevice::queueFamilies() const noexcept {
        return m_queue_families;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    const vk::PhysicalDeviceMemoryProperties &PhysicalDevice::vkMemoryProperties() const noexcept {
        return m_vk_memory_properties;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    vk::PhysicalDevice PhysicalDevice::vkPhysicalDevice() const noexcept {
        return m_vk_physical_device;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    PhysicalDevice::operator vk::PhysicalDevice() const noexcept { return m_vk_physical_device; }

    /////////////////////////////////////
    /////////////////////////////////////
    vk::PhysicalDevice PhysicalDevice::vkHandle() const noexcept { return vkPhysicalDevice(); }

    /////////////////////////////////////
    /////////////////////////////////////
    core::UInt64 PhysicalDevice::vkDebugHandle() const noexcept {
        return reinterpret_cast<core::UInt64>(vkHandle().operator VkPhysicalDevice());
    }
} // namespace storm::render
