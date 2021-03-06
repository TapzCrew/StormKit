// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/////////// - StormKit::render - ///////////
#include <stormkit/gpu/core/Device.mpp>
#include <stormkit/gpu/core/Enums.mpp>

#include <stormkit/gpu/resource/Buffer.mpp>

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    Buffer::Buffer(const Device &device, const CreateInfo &info, bool persistently_mapped)
        : m_device { &device }, m_usage { info.usage }, m_size { info.size },
          m_is_persistently_mapped { persistently_mapped } {
        const auto &vk = m_device->table();

        const auto create_info =
            VkBufferCreateInfo { .sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                                 .size        = m_size,
                                 .usage       = core::as<VkBufferUsageFlags>(m_usage),
                                 .sharingMode = VK_SHARING_MODE_EXCLUSIVE };

        CHECK_VK_ERROR(vk.vkCreateBuffer(*m_device, &create_info, nullptr, &m_buffer));

        const auto requirements = [&] {
            auto r = VkMemoryRequirements {};

            vk.vkGetBufferMemoryRequirements(*m_device, m_buffer, &r);

            return r;
        }();

        const auto allocate_info =
            VmaAllocationCreateInfo { .requiredFlags = core::as<core::UInt32>(info.property) };

        CHECK_VK_ERROR(vmaAllocateMemory(m_device->vmaAllocator(),
                                         &requirements,
                                         &allocate_info,
                                         &m_buffer_memory,
                                         nullptr));

        CHECK_VK_ERROR(vmaBindBufferMemory(m_device->vmaAllocator(), m_buffer_memory, m_buffer));

        if (m_is_persistently_mapped) STORMKIT_UNUSED(map(0u));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Buffer::~Buffer() {
        if (m_buffer_memory != VK_NULL_HANDLE) [[likely]]
            vmaFreeMemory(device().vmaAllocator(), m_buffer_memory);

        if (m_buffer != VK_NULL_HANDLE) [[likely]] {
            const auto &vk = device().table();

            vk.vkDestroyBuffer(device(), m_buffer, nullptr);
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Buffer::Buffer(Buffer &&other) noexcept
        : m_device { std::exchange(other.m_device, nullptr) },
          m_usage { std::exchange(other.m_usage, {}) }, m_size { std::exchange(other.m_size, 0) },
          m_is_persistently_mapped { std::exchange(other.m_is_persistently_mapped, false) },
          m_mapped_pointer { std::exchange(other.m_mapped_pointer, nullptr) },
          m_buffer { std::exchange(other.m_buffer, VK_NULL_HANDLE) }, m_buffer_memory {
              std::exchange(other.m_buffer_memory, VK_NULL_HANDLE)
          } {}

    /////////////////////////////////////
    /////////////////////////////////////
    auto Buffer::operator=(Buffer &&other) noexcept -> Buffer & {
        if (&other == this) [[unlikely]]
            return *this;

        m_device                 = std::exchange(other.m_device, nullptr);
        m_usage                  = std::exchange(other.m_usage, {});
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
        STORMKIT_EXPECTS(m_buffer);
        STORMKIT_EXPECTS(m_buffer_memory);
        STORMKIT_EXPECTS(offset < core::as<core::Size>(m_size));

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
    auto Buffer::flush(core::Int64 offset, core::USize size) -> void {
        STORMKIT_EXPECTS(m_buffer);
        STORMKIT_EXPECTS(m_buffer_memory);
        STORMKIT_EXPECTS(offset < core::as<core::Size>(m_size));
        STORMKIT_EXPECTS(size <= m_size);

        vmaFlushAllocation(device().vmaAllocator(),
                           m_buffer_memory,
                           core::as<VkDeviceSize>(offset),
                           size);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Buffer::unmap() -> void {
        STORMKIT_EXPECTS(m_buffer);
        STORMKIT_EXPECTS(m_buffer_memory);

        if (!m_is_persistently_mapped) {
            vmaUnmapMemory(device().vmaAllocator(), m_buffer_memory);

            m_mapped_pointer = nullptr;
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Buffer::upload(core::ByteConstSpan data, core::Int64 offset) -> void {
        auto ptr = map(offset, std::size(data));

        std::ranges::copy(data, std::begin(ptr));

        unmap();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Buffer::findMemoryType(core::UInt32 type_filter,
                                VkMemoryPropertyFlags properties,
                                const VkPhysicalDeviceMemoryProperties &mem_properties,
                                [[maybe_unused]] const VkMemoryRequirements &mem_requirements)
        -> core::UInt32 {
        for (auto i : core::range(mem_properties.memoryTypeCount)) {
            if ((type_filter & (1 << i)) &&
                (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
                return i;
        }

        return 0;
    }
} // namespace stormkit::gpu
