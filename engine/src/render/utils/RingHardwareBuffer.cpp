#include <storm/engine/render/utils/RingHardwareBuffer.hpp>

#include <storm/render/core/Device.hpp>
#include <storm/render/core/PhysicalDevice.hpp>

#include <storm/render/resource/HardwareBuffer.hpp>

using namespace storm;
using namespace storm::engine;

/////////////////////////////////////
/////////////////////////////////////
RingHardwareBuffer::RingHardwareBuffer(core::ArraySize count,
                                       const render::Device &device,
                                       render::HardwareBufferUsage usage,
                                       core::ArraySize byte_count,
                                       render::MemoryProperty property)
    : m_device { device }, m_count { count } {
    const auto &physical_device  = device.physicalDevice();
    const auto &capabilities     = physical_device.capabilities();
    const auto memory_properties = physical_device.memoryProperties();

    const auto it =
        std::find_if(std::cbegin(memory_properties),
                     std::cend(memory_properties),
                     [property](const auto &p) { return core::checkFlag(p, property); });

    if (property == (render::MemoryProperty::Host_Visible | render::MemoryProperty::Device_Local)) {
        if (it == std::cend(memory_properties))
            property = render::MemoryProperty::Host_Visible |
                       render::MemoryProperty::Host_Coherent | render::MemoryProperty::Host_Cached;
    } else
        STORMKIT_ENSURES(it != std::cend(memory_properties));

    m_element_size = computeUniformBufferInt32Alignement(byte_count, capabilities);

    m_buffer = device.createHardwareBufferPtr(usage, m_count * m_element_size, property);
}

/////////////////////////////////////
/////////////////////////////////////
RingHardwareBuffer::~RingHardwareBuffer() = default;

/////////////////////////////////////
/////////////////////////////////////
RingHardwareBuffer::RingHardwareBuffer(RingHardwareBuffer &&) noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
auto RingHardwareBuffer::operator=(RingHardwareBuffer &&) noexcept
    -> RingHardwareBuffer &      = default;

/////////////////////////////////////
/////////////////////////////////////
auto RingHardwareBuffer::next() -> void {
    m_offset += m_element_size;
    m_offset %= (m_element_size * m_count);
}

/////////////////////////////////////
/////////////////////////////////////
auto RingHardwareBuffer::map(core::UInt32 offset) -> core::Byte * {
    return m_buffer->map(m_offset + offset);
}

/////////////////////////////////////
/////////////////////////////////////
auto RingHardwareBuffer::unmap() -> void {
    m_buffer->unmap();
}
