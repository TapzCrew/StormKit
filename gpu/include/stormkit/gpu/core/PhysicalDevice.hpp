// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <span>
#include <string>
#include <vector>

#include <stormkit/core/Types.hpp>

#include <stormkit/gpu/core/Enums.hpp>
#include <stormkit/gpu/core/Fwd.hpp>
#include <stormkit/gpu/core/InstanceObject.hpp>
#include <stormkit/gpu/core/PhysicalDeviceInfo.hpp>
#include <stormkit/gpu/core/QueueFamily.hpp>
#include <stormkit/gpu/core/RenderCapabilities.hpp>
#include <stormkit/gpu/core/Vulkan.hpp>

namespace stormkit::gpu {
    class STORMKIT_PUBLIC PhysicalDevice: public InstanceObject {
      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Physical_Device;

        ~PhysicalDevice();

        PhysicalDevice(const PhysicalDevice&)                    = delete;
        auto operator=(const PhysicalDevice&) -> PhysicalDevice& = delete;

        PhysicalDevice(PhysicalDevice&&) noexcept;
        auto operator=(PhysicalDevice&&) noexcept -> PhysicalDevice&;

        [[nodiscard]] auto checkExtensionSupport(std::string_view extension) const noexcept -> bool;
        [[nodiscard]] auto
            checkExtensionSupport(std::span<const std::string_view> extensions) const noexcept
            -> bool;
        [[nodiscard]] auto
            checkExtensionSupport(std::span<const core::CZString> extensions) const noexcept
            -> bool;

        auto checkIfPresentSupportIsEnabled(const WindowSurface& surface) noexcept -> void;

        [[nodiscard]] auto info() const noexcept -> const PhysicalDeviceInfo&;
        [[nodiscard]] auto capabilities() const noexcept -> const RenderCapabilities&;
        [[nodiscard]] auto memoryProperties() const noexcept -> std::span<const MemoryPropertyFlag>;

        [[nodiscard]] auto queueFamilies() const noexcept -> std::span<const QueueFamily>;

        [[nodiscard]] auto createLogicalDevice() const -> Device;
        [[nodiscard]] auto allocateLogicalDevice() const -> DeviceOwnedPtr;
        [[nodiscard]] auto allocateRefCountedLogicalDevice() const -> DeviceSharedPtr;

        [[nodiscard]] operator VkPhysicalDevice() const noexcept;
        [[nodiscard]] VkPhysicalDevice vkHandle() const noexcept;

      private:
        PhysicalDevice(VkPhysicalDevice physical_device, const Instance& instance);

        VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;

        PhysicalDeviceInfo m_device_info;
        RenderCapabilities m_capabilities;
        std::vector<MemoryPropertyFlag> m_memory_properties;

        std::vector<QueueFamily> m_queue_families;
        VkPhysicalDeviceMemoryProperties m_vk_memory_properties;

        std::vector<std::string> m_extensions;

        friend class Instance;
    };

    constexpr auto
        computeUniformBufferOffsetAlignement(core::USize size,
                                             const RenderCapabilities& capabilities) noexcept
        -> core::USize;
} // namespace stormkit::gpu

#include "PhysicalDevice.inl"