// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module stormkit.Gpu;

import std;

import stormkit.Core;

import :Core;
import :Vulkan;

import :Resource.Buffer;

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    Buffer::Buffer(const Device& device, const CreateInfo& info, bool persistently_mapped, Tag)
        : DeviceObject { device }, m_usages { info.usages }, m_size { info.size },
          m_is_persistently_mapped { persistently_mapped } {
        const auto create_info = vk::BufferCreateInfo {}
                                     .setSize(m_size)
                                     .setUsage(core::as<vk::BufferUsageFlagBits>(m_usages))
                                     .setSharingMode(vk::SharingMode::eExclusive);

        vkCreate<vk::raii::Buffer>(this->device().vkHandle(), create_info)
            .transform(core::set(m_vk_buffer))
            .transform([this, &info]() noexcept -> VulkanExpected<void> {
                const auto requirements = m_vk_buffer->getMemoryRequirements();

                auto allocate_info = vma::AllocationCreateInfo {}.setRequiredFlags(
                    core::as<vk::MemoryPropertyFlagBits>(info.property));

                auto&& allocator = this->device().vmaAllocator();

                try {
                    m_vma_allocation = allocator.allocateMemoryUnique(requirements, allocate_info);
                    allocator.bindBufferMemory(*m_vma_allocation, *m_vk_buffer.get());
                } catch (const vk::SystemError& err) {
                    return std::unexpected { core::as<vk::Result>(err.code().value()) };
                }

                if (m_is_persistently_mapped) [[maybe_unused]]
                    auto _ = map(0u);

                return {};
            })
            .transform_error(core::map(core::as<Result>(), core::throwError()));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Buffer::findMemoryType(core::UInt                                     type_filter,
                                vk::MemoryPropertyFlags                        properties,
                                const vk::PhysicalDeviceMemoryProperties     & mem_properties,
                                [[maybe_unused]] const vk::MemoryRequirements& mem_requirements)
        -> core::UInt {
        for (auto i : core::range(mem_properties.memoryTypeCount)) {
            if ((type_filter & (1 << i)) and
                (checkFlag(mem_properties.memoryTypes[i].propertyFlags, properties)))
                return i;
        }

        return 0;
    }
} // namespace stormkit::gpu
