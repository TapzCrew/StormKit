// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#ifdef STORMKIT_BUILD_MODULES
module stormkit.Gpu:Graphics;

import :Raytracing;
#else
    #include <stormkit/std.hpp>

    #include <stormkit/Core.hpp>
    #include <stormkit/Gpu.hpp>
#endif

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    AccelerationStructure::AccelerationStructure(
        const Device& device,
        [[maybe_unused]] const AccelerationStructureDescription& description)
        : DeviceObject { device } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    AccelerationStructure::~AccelerationStructure() {
        if (m_acceleration_structure != VK_NULL_HANDLE) [[likely]] {
            const auto& vk = device().table();

            vk.vkDestroyAccelerationStructureKHR(device(), m_acceleration_structure, nullptr);

            m_acceleration_structure = VK_NULL_HANDLE;
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    AccelerationStructure::AccelerationStructure(AccelerationStructure&& other) noexcept
        : DeviceObject { std::move(other) },
          m_acceleration_structure { std::exchange(other.m_acceleration_structure,
                                                   VK_NULL_HANDLE) } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto AccelerationStructure::operator=(AccelerationStructure&& other) noexcept
        -> AccelerationStructure& {
        if (&other == this) [[unlikely]]
            return *this;

        m_acceleration_structure = std::exchange(other.m_acceleration_structure, VK_NULL_HANDLE);

        return *this;
    }
} // namespace stormkit::gpu
