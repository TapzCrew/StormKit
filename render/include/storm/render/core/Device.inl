// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
    /////////////////////////////////////
    /////////////////////////////////////
    void Device::waitForFence(const Fence &fence, core::UInt64 timeout) const noexcept {
        auto fences = std::array { std::cref(fence) };
        waitForFences(fences, true, timeout);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename Container,
             std::enable_if_t<!std::is_same_v<Container, std::filesystem::path>>>
    Shader Device::createShader(const Container &container, ShaderStage type) const {
        return createShader({ std::data(container), std::size(container) }, type);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename Container,
             std::enable_if_t<!std::is_same_v<Container, std::filesystem::path>>>
    ShaderOwnedPtr Device::createShaderPtr(const Container &container, ShaderStage type) const {
        return createShaderPtr({ std::data(container), std::size(container) }, type);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    HardwareBuffer Device::createVertexBuffer(core::ArraySize size,
                                              MemoryProperty property,
                                              bool use_staging) const {
        constexpr auto with_staging_buffer_usage =
            HardwareBufferUsage::Vertex | HardwareBufferUsage::Transfert_Dst;
        return createHardwareBuffer((use_staging) ? with_staging_buffer_usage
                                                  : HardwareBufferUsage::Vertex,
                                    size,
                                    property);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    HardwareBufferArray Device::createVertexBuffers(core::ArraySize count,
                                                    core::ArraySize size,
                                                    MemoryProperty property,
                                                    bool use_staging) const {
        auto buffers = HardwareBufferArray {};
        buffers.reserve(count);
        for (auto i = 0u; i < count; ++i)
            buffers.emplace_back(createVertexBuffer(size, property, use_staging));

        return buffers;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    HardwareBufferOwnedPtr Device::createVertexBufferPtr(core::ArraySize size,
                                                         MemoryProperty property,
                                                         bool use_staging) const {
        constexpr auto with_staging_buffer_usage =
            HardwareBufferUsage::Vertex | HardwareBufferUsage::Transfert_Dst;
        return createHardwareBufferPtr((use_staging) ? with_staging_buffer_usage
                                                     : HardwareBufferUsage::Vertex,
                                       size,
                                       property);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    HardwareBufferOwnedPtrArray Device::createVertexBuffersPtr(core::ArraySize count,
                                                               core::ArraySize size,
                                                               MemoryProperty property,
                                                               bool use_staging) const {
        auto buffers = HardwareBufferOwnedPtrArray {};
        buffers.reserve(count);
        for (auto i = 0u; i < count; ++i)
            buffers.emplace_back(createVertexBufferPtr(size, property, use_staging));

        return buffers;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    HardwareBuffer Device::createIndexBuffer(core::ArraySize size,
                                             MemoryProperty property,
                                             bool use_staging) const {
        constexpr auto with_staging_buffer_usage =
            HardwareBufferUsage::Index | HardwareBufferUsage::Transfert_Dst;
        return createHardwareBuffer((use_staging) ? with_staging_buffer_usage
                                                  : HardwareBufferUsage::Index,
                                    size,
                                    property);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    HardwareBufferArray Device::createIndexBuffers(core::ArraySize count,
                                                   core::ArraySize size,
                                                   MemoryProperty property,
                                                   bool use_staging) const {
        auto buffers = HardwareBufferArray {};
        buffers.reserve(count);
        for (auto i = 0u; i < count; ++i)
            buffers.emplace_back(createIndexBuffer(size, property, use_staging));

        return buffers;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    HardwareBufferOwnedPtr Device::createIndexBufferPtr(core::ArraySize size,
                                                        MemoryProperty property,
                                                        bool use_staging) const {
        constexpr auto with_staging_buffer_usage =
            HardwareBufferUsage::Index | HardwareBufferUsage::Transfert_Dst;
        return createHardwareBufferPtr((use_staging) ? with_staging_buffer_usage
                                                     : HardwareBufferUsage::Index,
                                       size,
                                       property);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    HardwareBufferOwnedPtrArray Device::createIndexBuffersPtr(core::ArraySize count,
                                                              core::ArraySize size,
                                                              MemoryProperty property,
                                                              bool use_staging) const {
        auto buffers = HardwareBufferOwnedPtrArray {};
        buffers.reserve(count);
        for (auto i = 0u; i < count; ++i)
            buffers.emplace_back(createIndexBufferPtr(size, property, use_staging));

        return buffers;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    HardwareBuffer Device::createUniformBuffer(core::ArraySize size,
                                               MemoryProperty property,
                                               bool use_staging) const {
        constexpr auto with_staging_buffer_usage =
            HardwareBufferUsage::Uniform | HardwareBufferUsage::Transfert_Dst;
        return createHardwareBuffer((use_staging) ? with_staging_buffer_usage
                                                  : HardwareBufferUsage::Uniform,
                                    size,
                                    property);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    HardwareBufferArray Device::createUniformBuffers(core::ArraySize count,
                                                     core::ArraySize size,
                                                     MemoryProperty property,
                                                     bool use_staging) const {
        auto buffers = HardwareBufferArray {};
        buffers.reserve(count);
        for (auto i = 0u; i < count; ++i)
            buffers.emplace_back(createUniformBuffer(size, property, use_staging));

        return buffers;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    HardwareBufferOwnedPtr Device::createUniformBufferPtr(core::ArraySize size,
                                                          MemoryProperty property,
                                                          bool use_staging) const {
        constexpr auto with_staging_buffer_usage =
            HardwareBufferUsage::Uniform | HardwareBufferUsage::Transfert_Dst;
        return createHardwareBufferPtr((use_staging) ? with_staging_buffer_usage
                                                     : HardwareBufferUsage::Uniform,
                                       size,
                                       property);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    HardwareBufferOwnedPtrArray Device::createUniformBuffersPtr(core::ArraySize count,
                                                                core::ArraySize size,
                                                                MemoryProperty property,
                                                                bool use_staging) const {
        auto buffers = HardwareBufferOwnedPtrArray {};
        buffers.reserve(count);
        for (auto i = 0u; i < count; ++i)
            buffers.emplace_back(createUniformBufferPtr(size, property, use_staging));

        return buffers;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    HardwareBuffer Device::createStagingBuffer(core::ArraySize size,
                                               MemoryProperty property) const {
        return createHardwareBuffer(HardwareBufferUsage::Transfert_Src, size, property);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    HardwareBufferArray Device::createStagingBuffers(core::ArraySize count,
                                                     core::ArraySize size,
                                                     MemoryProperty property) const {
        auto buffers = HardwareBufferArray {};
        buffers.reserve(count);
        for (auto i = 0u; i < count; ++i) buffers.emplace_back(createStagingBuffer(size, property));

        return buffers;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    HardwareBufferOwnedPtr Device::createStagingBufferPtr(core::ArraySize size,
                                                          MemoryProperty property) const {
        return createHardwareBufferPtr(HardwareBufferUsage::Transfert_Src, size, property);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    HardwareBufferOwnedPtrArray Device::createStagingBuffersPtr(core::ArraySize count,
                                                                core::ArraySize size,
                                                                MemoryProperty property) const {
        auto buffers = HardwareBufferOwnedPtrArray {};
        buffers.reserve(count);
        for (auto i = 0u; i < count; ++i)
            buffers.emplace_back(createStagingBufferPtr(size, property));

        return buffers;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    const Queue &Device::graphicsQueue() const noexcept { return *m_graphics_queue; }

    /////////////////////////////////////
    /////////////////////////////////////
    const Queue &Device::asyncTransfertQueue() const noexcept {
        STORMKIT_EXPECTS(m_async_transfert_queue != nullptr);

        return *m_async_transfert_queue;
    }
    /////////////////////////////////////
    /////////////////////////////////////
    const Queue &Device::asyncComputeQueue() const noexcept {
        STORMKIT_EXPECTS(m_async_compute_queue != nullptr);

        return *m_async_compute_queue;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    bool Device::hasAsyncTransfertQueue() const noexcept {
        return m_async_transfert_queue != nullptr;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    bool Device::hasAsyncComputeQueue() const noexcept { return m_async_compute_queue != nullptr; }

    /////////////////////////////////////
    /////////////////////////////////////
    const PhysicalDevice &Device::physicalDevice() const noexcept { return *m_physical_device; }

    /////////////////////////////////////
    /////////////////////////////////////
    vk::Device Device::vkDevice() const noexcept {
        auto device = vk::Device { nullptr };

        if (std::holds_alternative<vk::UniqueDevice>(m_vk_device))
            device = *std::get<vk::UniqueDevice>(m_vk_device);
        else
            device = std::get<vk::Device>(m_vk_device);

        STORMKIT_EXPECTS(device);

        return device;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Device::operator vk::Device() const noexcept { return vkDevice(); }

    /////////////////////////////////////
    /////////////////////////////////////
    vk::Device Device::vkHandle() const noexcept { return vkDevice(); }

    /////////////////////////////////////
    /////////////////////////////////////
    core::UInt64 Device::vkDebugHandle() const noexcept {
        return reinterpret_cast<core::UInt64>(vkHandle().operator VkDevice_T *());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    VmaAllocator Device::vmaAllocator() const noexcept { return m_vma_allocator; }

    /////////////////////////////////////
    /////////////////////////////////////
    const vk::DispatchLoaderDynamic &Device::vkDispatcher() const noexcept {
        return m_vk_dispatcher;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    void Device::setObjectName(const T &object, std::string_view name) const {
        setObjectName(object.vkDebugHandle(), T::DEBUG_TYPE, name);
    }
} // namespace storm::render
