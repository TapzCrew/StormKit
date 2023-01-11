// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#ifdef STORMKIT_BUILD_MODULES
module stormkit.Gpu:Resource;

import :Buffer;
#else
    #include <stormkit/std.hpp>

    #include <stormkit/Core.hpp>
    #include <stormkit/Gpu.hpp>
#endif

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    Buffer::Buffer(const Device& device, const CreateInfo& info, bool persistently_mapped)
        : DeviceObject { device }, m_usages { info.usages }, m_size { info.size },
          m_is_persistently_mapped { persistently_mapped } {
        const auto& vk = this->device().table();

        const auto create_info =
            VkBufferCreateInfo { .sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                                 .pNext                 = nullptr,
                                 .flags                 = {},
                                 .size                  = m_size,
                                 .usage                 = core::as<VkBufferUsageFlags>(m_usages),
                                 .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
                                 .queueFamilyIndexCount = 0,
                                 .pQueueFamilyIndices   = nullptr };

        CHECK_VK_ERROR(vk.vkCreateBuffer(this->device(), &create_info, nullptr, &m_buffer));

        const auto requirements = [&] {
            auto r = VkMemoryRequirements {};

            vk.vkGetBufferMemoryRequirements(this->device(), m_buffer, &r);

            return r;
        }();

        const auto allocate_info =
            VmaAllocationCreateInfo { .requiredFlags = core::as<core::UInt32>(info.property) };

        CHECK_VK_ERROR(vmaAllocateMemory(this->device().vmaAllocator(),
                                         &requirements,
                                         &allocate_info,
                                         &m_buffer_memory,
                                         nullptr));

        CHECK_VK_ERROR(
            vmaBindBufferMemory(this->device().vmaAllocator(), m_buffer_memory, m_buffer));

        if (m_is_persistently_mapped) STORMKIT_UNUSED(map(0u));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Buffer::~Buffer() {
        if (m_buffer_memory != VK_NULL_HANDLE) [[likely]]
            vmaFreeMemory(device().vmaAllocator(), m_buffer_memory);

        if (m_buffer != VK_NULL_HANDLE) [[likely]] {
            const auto& vk = device().table();

            vk.vkDestroyBuffer(device(), m_buffer, nullptr);
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Buffer::Buffer(Buffer&& other) noexcept
        : DeviceObject { std::move(other) }, m_usages { std::exchange(other.m_usages, {}) },
          m_size { std::exchange(other.m_size, 0) },
          m_is_persistently_mapped { std::exchange(other.m_is_persistently_mapped, false) },
          m_mapped_pointer { std::exchange(other.m_mapped_pointer, nullptr) },
          m_buffer { std::exchange(other.m_buffer, VK_NULL_HANDLE) },
          m_buffer_memory { std::exchange(other.m_buffer_memory, VK_NULL_HANDLE) } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Buffer::operator=(Buffer&& other) noexcept -> Buffer& {
        if (&other == this) [[unlikely]]
            return *this;

        DeviceObject::operator=(std::move(other));
        m_usages                 = std::exchange(other.m_usages, {});
        m_size                   = std::exchange(other.m_size, 0);
        m_is_persistently_mapped = std::exchange(other.m_is_persistently_mapped, false);
        m_mapped_pointer         = std::exchange(other.m_mapped_pointer, nullptr);
        m_buffer                 = std::exchange(other.m_buffer, VK_NULL_HANDLE);
        m_buffer_memory          = std::exchange(other.m_buffer_memory, VK_NULL_HANDLE);

        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Buffer::map(core::Int64 offset) noexcept -> core::Byte * {
        core::expects(m_buffer);
        core::expects(m_buffer_memory);
        core::expects(offset < core::as<core::Size>(m_size));

        if (!m_mapped_pointer) {
            vmaMapMemory(device().vmaAllocator(),
                         m_buffer_memory,
                         reinterpret_cast<void **>(&m_mapped_pointer));
            m_mapped_pointer += offset;
        }

        return m_mapped_pointer;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Buffer::flush(core::Int64 offset, core::RangeExtent size) -> void {
        core::expects(m_buffer);
        core::expects(m_buffer_memory);
        core::expects(offset < core::as<core::Size>(m_size));
        core::expects(size <= m_size);

        vmaFlushAllocation(device().vmaAllocator(),
                           m_buffer_memory,
                           core::as<VkDeviceSize>(offset),
                           size);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Buffer::unmap() -> void {
        core::expects(m_buffer);
        core::expects(m_buffer_memory);

        if (!m_is_persistently_mapped) {
            vmaUnmapMemory(device().vmaAllocator(), m_buffer_memory);

            m_mapped_pointer = nullptr;
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Buffer::upload(std::span<const core::Byte> data, core::Int64 offset) -> void {
        auto ptr = map(offset, std::size(data));

        std::ranges::copy(data, std::begin(ptr));

        unmap();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Buffer::findMemoryType(core::UInt32 type_filter,
                                VkMemoryPropertyFlags properties,
                                const VkPhysicalDeviceMemoryProperties& mem_properties,
                                [[maybe_unused]] const VkMemoryRequirements& mem_requirements)
        -> core::UInt32 {
        for (auto i : core::range(mem_properties.memoryTypeCount)) {
            if ((type_filter & (1 << i)) and
                (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
                return i;
        }

        return 0;
    }
} // namespace stormkit::gpu
