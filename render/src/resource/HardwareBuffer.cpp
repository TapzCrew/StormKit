// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/////////// - STL - ///////////
#include <fstream>

/////////// - StormKit::render - ///////////
#include <storm/render/core/Device.hpp>
#include <storm/render/core/Enums.hpp>
#include <storm/render/resource/HardwareBuffer.hpp>

using namespace storm;
using namespace storm::render;

#define DELETER [](auto handle, const auto &device) { device->deallocateVmaAllocation(handle); }

/////////////////////////////////////
/////////////////////////////////////
HardwareBuffer::HardwareBuffer(const render::Device &device,
                               HardwareBufferUsage usage,
                               core::ArraySize size,
                               MemoryProperty property,
                               bool persistently_mapped)
    : m_device { &device }, m_usage { usage }, m_byte_count { size },
      m_is_persistently_mapped { persistently_mapped }, m_vma_buffer_memory { DELETER, *m_device } {
    const auto create_info = vk::BufferCreateInfo {}
                                 .setSize(m_byte_count)
                                 .setUsage(toVK(usage))
                                 .setSharingMode(vk::SharingMode::eExclusive);

    m_vk_buffer = m_device->createVkBuffer(create_info);

    const auto requirements = m_device->getVkBufferMemoryRequirements(*m_vk_buffer);

    auto allocate_info =
        VmaAllocationCreateInfo { .requiredFlags = toVK(property).operator unsigned int() };

    m_vma_buffer_memory.reset(m_device->allocateVmaAllocation(allocate_info, requirements));

    m_device->bindVmaBufferMemory(m_vma_buffer_memory, *m_vk_buffer);

    if (m_is_persistently_mapped) STORMKIT_UNUSED(map(0u));
}

/////////////////////////////////////
/////////////////////////////////////
HardwareBuffer::~HardwareBuffer() = default;

/////////////////////////////////////
/////////////////////////////////////
HardwareBuffer::HardwareBuffer(HardwareBuffer &&) = default;

/////////////////////////////////////
/////////////////////////////////////
HardwareBuffer &HardwareBuffer::operator=(HardwareBuffer &&) = default;

/////////////////////////////////////
/////////////////////////////////////
core::Byte *HardwareBuffer::map(core::UInt32 offset) {
    STORMKIT_EXPECTS(offset < m_byte_count);

    if (!m_mapped_pointer) {
        const auto &device = static_cast<const Device &>(*m_device);
        m_mapped_pointer   = device.mapVmaMemory(m_vma_buffer_memory);
    }

    return m_mapped_pointer + offset;
}

/////////////////////////////////////
/////////////////////////////////////
void HardwareBuffer::flush(core::Int32 offset, core::ArraySize count) {
    STORMKIT_EXPECTS(offset < gsl::narrow_cast<core::Int32>(m_byte_count));
    STORMKIT_EXPECTS(count <= m_byte_count);

    vmaFlushAllocation(m_device->vmaAllocator(),
                       m_vma_buffer_memory,
                       gsl::narrow_cast<VkDeviceSize>(offset),
                       count);
}

/////////////////////////////////////
/////////////////////////////////////
void HardwareBuffer::unmap() {
    if (!m_is_persistently_mapped) {
        m_device->unmapVmaMemory(m_vma_buffer_memory);
        m_mapped_pointer = nullptr;
    }
}

/////////////////////////////////////
/////////////////////////////////////
core::UInt32
    HardwareBuffer::findMemoryType(core::UInt32 type_filter,
                                   VkMemoryPropertyFlags properties,
                                   const VkPhysicalDeviceMemoryProperties &mem_properties,
                                   [[maybe_unused]] const VkMemoryRequirements &mem_requirements) {
    for (auto i = 0u; i < mem_properties.memoryTypeCount; ++i) {
        if ((type_filter & (1 << i)) &&
            (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
            return i;
    }

    return 0;
}
