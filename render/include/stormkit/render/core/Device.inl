// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::render {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Device::waitForFence(const Fence &fence,
                                     std::chrono::milliseconds timeout) const noexcept -> void {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        waitForFences(core::makeConstRefStaticArray(fence), true, timeout);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Device::createVertexBuffer(core::USize size,
                                           MemoryPropertyFlag property,
                                           bool use_staging) const -> Buffer {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        constexpr auto with_staging_buffer_usage =
            BufferUsageFlag::Vertex | BufferUsageFlag::Transfert_Dst;
        return createBuffer({ (use_staging) ? with_staging_buffer_usage : BufferUsageFlag::Vertex,
                              size,
                              property });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Device::allocateVertexBuffer(core::USize size,
                                             MemoryPropertyFlag property,
                                             bool use_staging) const -> BufferOwnedPtr {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        constexpr auto with_staging_buffer_usage =
            BufferUsageFlag::Vertex | BufferUsageFlag::Transfert_Dst;
        return allocateBuffer({ (use_staging) ? with_staging_buffer_usage : BufferUsageFlag::Vertex,
                                size,
                                property });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Device::allocateRefCountedVertexBuffer(core::USize size,
                                                       MemoryPropertyFlag property,
                                                       bool use_staging) const -> BufferSharedPtr {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        constexpr auto with_staging_buffer_usage =
            BufferUsageFlag::Vertex | BufferUsageFlag::Transfert_Dst;
        return allocateRefCountedBuffer(
            { (use_staging) ? with_staging_buffer_usage : BufferUsageFlag::Vertex,
              size,
              property });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Device::createVertexBuffers(core::USize count,
                                            core::USize size,
                                            MemoryPropertyFlag property,
                                            bool use_staging) const -> std::vector<Buffer> {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        auto buffers = std::vector<Buffer> {};
        buffers.reserve(count);
        for (auto i = 0u; i < count; ++i)
            buffers.emplace_back(createVertexBuffer(size, property, use_staging));

        return buffers;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Device::allocateVertexBuffers(core::USize count,
                                              core::USize size,
                                              MemoryPropertyFlag property,
                                              bool use_staging) const
        -> std::vector<BufferOwnedPtr> {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        auto buffers = std::vector<BufferOwnedPtr> {};
        buffers.reserve(count);
        for (auto i = 0u; i < count; ++i)
            buffers.emplace_back(allocateVertexBuffer(size, property, use_staging));

        return buffers;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Device::allocateRefCountedVertexBuffers(core::USize count,
                                                        core::USize size,
                                                        MemoryPropertyFlag property,
                                                        bool use_staging) const
        -> std::vector<BufferSharedPtr> {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        auto buffers = std::vector<BufferSharedPtr> {};
        buffers.reserve(count);
        for (auto i = 0u; i < count; ++i)
            buffers.emplace_back(allocateRefCountedVertexBuffer(size, property, use_staging));

        return buffers;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Device::createIndexBuffer(core::USize size,
                                          MemoryPropertyFlag property,
                                          bool use_staging) const -> Buffer {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        constexpr auto with_staging_buffer_usage =
            BufferUsageFlag::Index | BufferUsageFlag::Transfert_Dst;
        return createBuffer(
            { (use_staging) ? with_staging_buffer_usage : BufferUsageFlag::Index, size, property });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Device::allocateIndexBuffer(core::USize size,
                                            MemoryPropertyFlag property,
                                            bool use_staging) const -> BufferOwnedPtr {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        constexpr auto with_staging_buffer_usage =
            BufferUsageFlag::Index | BufferUsageFlag::Transfert_Dst;
        return allocateBuffer(
            { (use_staging) ? with_staging_buffer_usage : BufferUsageFlag::Index, size, property });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Device::allocateRefCountedIndexBuffer(core::USize size,
                                                      MemoryPropertyFlag property,
                                                      bool use_staging) const -> BufferSharedPtr {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        constexpr auto with_staging_buffer_usage =
            BufferUsageFlag::Index | BufferUsageFlag::Transfert_Dst;
        return allocateRefCountedBuffer(
            { (use_staging) ? with_staging_buffer_usage : BufferUsageFlag::Index, size, property });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Device::createIndexBuffers(core::USize count,
                                           core::USize size,
                                           MemoryPropertyFlag property,
                                           bool use_staging) const -> std::vector<Buffer> {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        auto buffers = std::vector<Buffer> {};
        buffers.reserve(count);
        for (auto i = 0u; i < count; ++i)
            buffers.emplace_back(createIndexBuffer(size, property, use_staging));

        return buffers;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Device::allocateIndexBuffers(core::USize count,
                                             core::USize size,
                                             MemoryPropertyFlag property,
                                             bool use_staging) const
        -> std::vector<BufferOwnedPtr> {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        auto buffers = std::vector<BufferOwnedPtr> {};
        buffers.reserve(count);

        for (auto i = 0u; i < count; ++i)
            buffers.emplace_back(allocateIndexBuffer(size, property, use_staging));

        return buffers;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Device::allocateRefCountedIndexBuffers(core::USize count,
                                                       core::USize size,
                                                       MemoryPropertyFlag property,
                                                       bool use_staging) const
        -> std::vector<BufferSharedPtr> {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        auto buffers = std::vector<BufferSharedPtr> {};
        buffers.reserve(count);

        for (auto i = 0u; i < count; ++i)
            buffers.emplace_back(allocateRefCountedIndexBuffer(size, property, use_staging));

        return buffers;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Device::createUniformBuffer(core::USize size,
                                            MemoryPropertyFlag property,
                                            bool use_staging) const -> Buffer {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        constexpr auto with_staging_buffer_usage =
            BufferUsageFlag::Uniform | BufferUsageFlag::Transfert_Dst;
        return createBuffer({ (use_staging) ? with_staging_buffer_usage : BufferUsageFlag::Uniform,
                              size,
                              property });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Device::allocateUniformBuffer(core::USize size,
                                              MemoryPropertyFlag property,
                                              bool use_staging) const -> BufferOwnedPtr {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        constexpr auto with_staging_buffer_usage =
            BufferUsageFlag::Uniform | BufferUsageFlag::Transfert_Dst;
        return allocateBuffer(
            { (use_staging) ? with_staging_buffer_usage : BufferUsageFlag::Uniform,
              size,
              property });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Device::allocateRefCountedUniformBuffer(core::USize size,
                                                        MemoryPropertyFlag property,
                                                        bool use_staging) const -> BufferSharedPtr {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        constexpr auto with_staging_buffer_usage =
            BufferUsageFlag::Uniform | BufferUsageFlag::Transfert_Dst;
        return allocateRefCountedBuffer(
            { (use_staging) ? with_staging_buffer_usage : BufferUsageFlag::Uniform,
              size,
              property });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Device::createUniformBuffers(core::USize count,
                                             core::USize size,
                                             MemoryPropertyFlag property,
                                             bool use_staging) const -> std::vector<Buffer> {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        auto buffers = std::vector<Buffer> {};
        buffers.reserve(count);

        for (auto i = 0u; i < count; ++i)
            buffers.emplace_back(createUniformBuffer(size, property, use_staging));

        return buffers;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Device::allocateUniformBuffers(core::USize count,
                                               core::USize size,
                                               MemoryPropertyFlag property,
                                               bool use_staging) const
        -> std::vector<BufferOwnedPtr> {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        auto buffers = std::vector<BufferOwnedPtr> {};
        buffers.reserve(count);

        for (auto i = 0u; i < count; ++i)
            buffers.emplace_back(allocateUniformBuffer(size, property, use_staging));

        return buffers;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Device::allocateRefCountedUniformBuffers(core::USize count,
                                                         core::USize size,
                                                         MemoryPropertyFlag property,
                                                         bool use_staging) const
        -> std::vector<BufferSharedPtr> {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        auto buffers = std::vector<BufferSharedPtr> {};
        buffers.reserve(count);

        for (auto i = 0u; i < count; ++i)
            buffers.emplace_back(allocateRefCountedUniformBuffer(size, property, use_staging));

        return buffers;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Device::createStagingBuffer(core::USize size, MemoryPropertyFlag property) const
        -> Buffer {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return createBuffer({ BufferUsageFlag::Transfert_Src, size, property });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Device::allocateStagingBuffer(core::USize size, MemoryPropertyFlag property) const
        -> BufferOwnedPtr {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return allocateBuffer({ BufferUsageFlag::Transfert_Src, size, property });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Device::allocateRefCountedStagingBuffer(core::USize size,
                                                        MemoryPropertyFlag property) const
        -> BufferSharedPtr {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return allocateRefCountedBuffer({ BufferUsageFlag::Transfert_Src, size, property });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Device::createStagingBuffers(core::USize count,
                                             core::USize size,
                                             MemoryPropertyFlag property) const
        -> std::vector<Buffer> {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        auto buffers = std::vector<Buffer> {};
        buffers.reserve(count);

        for (auto i = 0u; i < count; ++i) buffers.emplace_back(createStagingBuffer(size, property));

        return buffers;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Device::allocateStagingBuffers(core::USize count,
                                               core::USize size,
                                               MemoryPropertyFlag property) const
        -> std::vector<BufferOwnedPtr> {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        auto buffers = std::vector<BufferOwnedPtr> {};
        buffers.reserve(count);

        for (auto i = 0u; i < count; ++i)
            buffers.emplace_back(allocateStagingBuffer(size, property));

        return buffers;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Device::allocateRefCountedStagingBuffers(core::USize count,
                                                         core::USize size,
                                                         MemoryPropertyFlag property) const
        -> std::vector<BufferSharedPtr> {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        auto buffers = std::vector<BufferSharedPtr> {};
        buffers.reserve(count);

        for (auto i = 0u; i < count; ++i)
            buffers.emplace_back(allocateRefCountedStagingBuffer(size, property));

        return buffers;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Device::graphicsQueue() const noexcept -> const Queue & {
        STORMKIT_EXPECTS(m_graphics_queue != std::nullopt);

        return *m_graphics_queue;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Device::asyncTransfertQueue() const noexcept -> const Queue & {
        STORMKIT_EXPECTS(m_async_transfert_queue != std::nullopt);

        return *m_async_transfert_queue;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Device::asyncComputeQueue() const noexcept -> const Queue & {
        STORMKIT_EXPECTS(m_async_compute_queue != std::nullopt);

        return *m_async_compute_queue;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Device::hasAsyncTransfertQueue() const noexcept -> bool {
        return m_async_transfert_queue != std::nullopt;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Device::hasAsyncComputeQueue() const noexcept -> bool {
        return m_async_compute_queue != std::nullopt;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Device::physicalDevice() const noexcept -> const PhysicalDevice & {
        return *m_physical_device;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline Device::operator VkDevice() const noexcept { return vkHandle(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Device::vkHandle() const noexcept -> VkDevice {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return m_device;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    constexpr auto Device::vkObjectType() const noexcept -> VkObjectType {
        return VK_OBJECT_TYPE_DEVICE;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Device::vmaAllocator() const noexcept -> VmaAllocator {
        STORMKIT_EXPECTS(m_vma_allocator != VK_NULL_HANDLE);

        return m_vma_allocator;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto Device::table() const noexcept -> const VolkDeviceTable & { return m_device_table; }

    /////////////////////////////////////
    /////////////////////////////////////
    template<typename T>
    auto Device::setObjectName(const T &object, std::string_view name) const -> void {
        setObjectName(reinterpret_cast<core::UInt64>(object.vkHandle()), T::DEBUG_TYPE, name);
    }
} // namespace stormkit::render
