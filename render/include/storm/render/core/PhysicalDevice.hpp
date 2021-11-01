// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <vector>

#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/Span.hpp>

#include <storm/render/core/Fwd.hpp>
#include <storm/render/core/PhysicalDeviceInfo.hpp>
#include <storm/render/core/QueueFamily.hpp>
#include <storm/render/core/RenderCapabilities.hpp>
#include <storm/render/core/Surface.hpp>
#include <storm/render/core/Vulkan.hpp>

namespace storm::render {
    class STORMKIT_PUBLIC PhysicalDevice: public core::NonCopyable {
      public:
        using MemoryProperties           = std::vector<MemoryProperty>;
        using MemoryPropertiesSpan       = core::span<const MemoryProperty>;
        static constexpr auto DEBUG_TYPE = DebugObjectType::Physical_Device;

        explicit PhysicalDevice(vk::PhysicalDevice physical_device, const Instance &context);
        ~PhysicalDevice();

        PhysicalDevice(PhysicalDevice &&);
        PhysicalDevice &operator=(PhysicalDevice &&);

        bool checkExtensionSupport(gsl::czstring<> extension) const noexcept;
        bool checkExtensionSupport(core::span<const gsl::czstring<>> extensions) const noexcept;

        Device createLogicalDevice() const;
        DeviceOwnedPtr createLogicalDevicePtr() const;

        void checkIfPresentSupportIsEnabled(const WindowSurface &surface) noexcept;

        vk::SurfaceCapabilitiesKHR
            queryVkSurfaceCapabilities(const WindowSurface &surface) const noexcept;
        std::vector<vk::SurfaceFormatKHR>
            queryVkSurfaceFormats(const WindowSurface &surface) const noexcept;
        std::vector<vk::PresentModeKHR>
            queryVkPresentModes(const WindowSurface &surface) const noexcept;

        inline const PhysicalDeviceInfo &info() const noexcept;
        inline const RenderCapabilities &capabilities() const noexcept;
        inline MemoryPropertiesSpan memoryProperties() const noexcept;

        inline storm::core::span<const QueueFamily> queueFamilies() const noexcept;

        vk::UniqueDevice createVkDevice(const vk::DeviceCreateInfo &create_info) const;

        inline const vk::PhysicalDeviceMemoryProperties &vkMemoryProperties() const noexcept;

        inline vk::PhysicalDevice vkPhysicalDevice() const noexcept;
        inline operator vk::PhysicalDevice() const noexcept;
        inline vk::PhysicalDevice vkHandle() const noexcept;
        inline core::UInt64 vkDebugHandle() const noexcept;

        vk::FormatProperties vkGetFormatProperties(vk::Format format) const noexcept;

      private:
        InstanceConstPtr m_instance;

        PhysicalDeviceInfo m_device_info;
        RenderCapabilities m_capabilities;
        MemoryProperties m_memory_properties;

        std::vector<QueueFamily> m_queue_families;

        vk::PhysicalDevice m_vk_physical_device = VK_NULL_HANDLE;
        vk::PhysicalDeviceMemoryProperties m_vk_memory_properties;

        std::vector<std::string> m_extensions;
    };

    inline core::ArraySize
        computeUniformBufferInt32Alignement(core::ArraySize size,
                                            const RenderCapabilities &capabilities) {
        if (size < capabilities.limits.min_uniform_buffer_offset_alignment)
            size = capabilities.limits.min_uniform_buffer_offset_alignment;
        else if (size > capabilities.limits.min_uniform_buffer_offset_alignment)
            size = capabilities.limits.min_uniform_buffer_offset_alignment *
                   ((size / capabilities.limits.min_uniform_buffer_offset_alignment) + 1);

        return size;
    }
} // namespace storm::render

#include "PhysicalDevice.inl"
