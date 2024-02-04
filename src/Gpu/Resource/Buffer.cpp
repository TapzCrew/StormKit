// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module stormkit.Gpu;

import std;

import stormkit.Core;

import :Core;
import stormkit.Gpu.Vulkan;

import :Resource.Buffer;

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    Buffer::Buffer(const Device& device, const CreateInfo& info, bool persistently_mapped, Tag)
        : DeviceObject { device }, m_usages { info.usages }, m_size { info.size },
          m_is_persistently_mapped { persistently_mapped } {
        this->device()
            .vkHandle()
            .createBuffer({ .size        = m_size,
                            .usage       = core::narrow<vk::BufferUsageFlagBits>(m_usages),
                            .sharingMode = vk::SharingMode::eExclusive })
            .transform(core::monadic::set(m_vk_buffer))
            .transform([this, &info]() noexcept -> VulkanExpected<void> {
                const auto requirements = m_vk_buffer->getMemoryRequirements();

                auto allocate_info = vma::AllocationCreateInfo {}.setRequiredFlags(
                    core::narrow<vk::MemoryPropertyFlagBits>(info.property));

                auto&& allocator = this->device().vmaAllocator();

                auto&& [error, m_vma_allocation] =
                    allocator.allocateMemoryUnique(requirements, allocate_info);
                if (error != vk::Result::eSuccess)
                    return std::unexpected { core::narrow<vk::Result>(error) };

                error = allocator.bindBufferMemory(*m_vma_allocation, *m_vk_buffer.get());
                if (error != vk::Result::eSuccess)
                    return std::unexpected { core::narrow<vk::Result>(error) };

                if (m_is_persistently_mapped) [[maybe_unused]]
                    auto _ = map(0u);

                return {};
            })
            .transform_error(core::monadic::map(core::monadic::as<Result>(), core::throwError()));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Buffer::findMemoryType(core::UInt                                     type_filter,
                                vk::MemoryPropertyFlags                        properties,
                                const vk::PhysicalDeviceMemoryProperties&      mem_properties,
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
