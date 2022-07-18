// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/PlatformMacro.hpp>

#include <filesystem>
#include <span>
#include <vector>

#include <stormkit/core/Memory.hpp>
#include <stormkit/core/Types.hpp>

#include <stormkit/gpu/core/Enums.hpp>
#include <stormkit/gpu/core/Fwd.hpp>
#include <stormkit/gpu/core/InstanceObject.hpp>
#include <stormkit/gpu/core/Queue.hpp>
#include <stormkit/gpu/core/Vulkan.hpp>

#include <stormkit/gpu/sync/Fence.hpp>
#include <stormkit/gpu/sync/Fwd.hpp>
#include <stormkit/gpu/sync/Semaphore.hpp>

#include <stormkit/gpu/resource/Buffer.hpp>
#include <stormkit/gpu/resource/Fwd.hpp>
#include <stormkit/gpu/resource/Image.hpp>
#include <stormkit/gpu/resource/Sampler.hpp>
#include <stormkit/gpu/resource/Shader.hpp>

#include <stormkit/gpu/pipeline/ComputePipeline.hpp>
#include <stormkit/gpu/pipeline/DescriptorPool.hpp>
#include <stormkit/gpu/pipeline/DescriptorSet.hpp>
#include <stormkit/gpu/pipeline/DescriptorSetLayout.hpp>
#include <stormkit/gpu/pipeline/Fwd.hpp>
#include <stormkit/gpu/pipeline/GraphicsPipeline.hpp>
#include <stormkit/gpu/pipeline/PipelineCache.hpp>
#include <stormkit/gpu/pipeline/RenderPass.hpp>

namespace stormkit::gpu {
    class STORMKIT_PUBLIC Device: public InstanceObject {
      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Device;

        Device(const PhysicalDevice &physical_device, const Instance &instance);
        ~Device();

        Device(const Device &) = delete;
        auto operator=(const Device &) -> Device & = delete;

        Device(Device &&) noexcept;
        auto operator=(Device &&) noexcept -> Device &;

        auto waitIdle() const noexcept -> void;
        auto waitForFences(std::span<const FenceConstRef> fences,
                           bool wait_all = true,
                           std::chrono::milliseconds timeout =
                               std::chrono::milliseconds::max()) const noexcept -> void;
        auto waitForFence(const Fence &fence,
                          std::chrono::milliseconds timeout =
                              std::chrono::milliseconds::max()) const noexcept -> void;

        [[nodiscard]] auto createShader(std::filesystem::path filepath, ShaderStageFlag type) const
            -> Shader;
        [[nodiscard]] auto allocateShader(std::filesystem::path filepath,
                                          ShaderStageFlag type) const -> ShaderOwnedPtr;
        [[nodiscard]] auto allocateRefCountedShader(std::filesystem::path filepath,
                                                    ShaderStageFlag type) const -> ShaderSharedPtr;

        [[nodiscard]] auto createShader(core::ByteConstSpan data, ShaderStageFlag type) const
            -> Shader;
        [[nodiscard]] auto allocateShader(core::ByteConstSpan data, ShaderStageFlag type) const
            -> ShaderOwnedPtr;
        [[nodiscard]] auto allocateRefCountedShader(core::ByteConstSpan data,
                                                    ShaderStageFlag type) const -> ShaderSharedPtr;

        [[nodiscard]] auto createShader(std::span<const SpirvID> data, ShaderStageFlag type) const
            -> Shader;
        [[nodiscard]] auto allocateShader(std::span<const SpirvID> data, ShaderStageFlag type) const
            -> ShaderOwnedPtr;
        [[nodiscard]] auto allocateRefCountedShader(std::span<const SpirvID> data,
                                                    ShaderStageFlag type) const -> ShaderSharedPtr;

        [[nodiscard]] auto createGraphicsPipeline(const PipelineCache *cache = nullptr) const
            -> GraphicsPipeline;
        [[nodiscard]] auto allocateGraphicsPipeline(const PipelineCache *cache = nullptr) const
            -> GraphicsPipelineOwnedPtr;
        [[nodiscard]] auto
            allocateRefCountedGraphicsPipeline(const PipelineCache *cache = nullptr) const
            -> GraphicsPipelineSharedPtr;

        [[nodiscard]] auto createComputePipeline(const PipelineCache *cache = nullptr) const
            -> ComputePipeline;
        [[nodiscard]] auto allocateComputePipeline(const PipelineCache *cache = nullptr) const
            -> ComputePipelineOwnedPtr;
        [[nodiscard]] auto
            allocateRefCountedComputePipeline(const PipelineCache *cache = nullptr) const
            -> ComputePipelineSharedPtr;

        [[nodiscard]] auto createPipelineCache(std::filesystem::path path = "") const
            -> PipelineCache;
        [[nodiscard]] auto allocatePipelineCache(std::filesystem::path path = "") const
            -> PipelineCacheOwnedPtr;
        [[nodiscard]] auto allocateRefCountedPipelineCache(std::filesystem::path path = "") const
            -> PipelineCacheSharedPtr;

        [[nodiscard]] auto createRenderPass(const RenderPassDescription &description) const
            -> RenderPass;
        [[nodiscard]] auto allocateRenderPass(const RenderPassDescription &description) const
            -> RenderPassOwnedPtr;
        [[nodiscard]] auto
            allocateRefCountedRenderPass(const RenderPassDescription &description) const
            -> RenderPassSharedPtr;

        [[nodiscard]] auto createFence(bool signaled = false) const -> Fence;
        [[nodiscard]] auto allocateFence(bool signaled = false) const -> FenceOwnedPtr;
        [[nodiscard]] auto allocateRefCountedFence(bool signaled = false) const -> FenceSharedPtr;

        [[nodiscard]] auto createSemaphore() const -> Semaphore;
        [[nodiscard]] auto allocateSemaphore() const -> SemaphoreOwnedPtr;
        [[nodiscard]] auto allocateRefCountedSemaphore() const -> SemaphoreSharedPtr;

        [[nodiscard]] auto createBuffer(const Buffer::CreateInfo &info) const -> Buffer;
        [[nodiscard]] auto allocateBuffer(const Buffer::CreateInfo &info) const -> BufferOwnedPtr;
        [[nodiscard]] auto allocateRefCountedBuffer(const Buffer::CreateInfo &info) const
            -> BufferSharedPtr;

        [[nodiscard]] auto createBuffers(core::USize count, const Buffer::CreateInfo &info) const
            -> std::vector<Buffer>;
        [[nodiscard]] auto allocateBuffers(core::USize count, const Buffer::CreateInfo &info) const
            -> std::vector<BufferOwnedPtr>;
        [[nodiscard]] auto allocateRefCountedBuffers(core::USize count,
                                                     const Buffer::CreateInfo &info) const
            -> std::vector<BufferSharedPtr>;

        [[nodiscard]] auto createDescriptorSetLayout() const -> DescriptorSetLayout;
        [[nodiscard]] auto allocateDescriptorSetLayout() const -> DescriptorSetLayoutOwnedPtr;
        [[nodiscard]] auto allocateRefCountedDescriptorSetLayout() const
            -> DescriptorSetLayoutSharedPtr;

        [[nodiscard]] auto createDescriptorPool(std::span<const DescriptorPool::Size> sizes,
                                                core::UInt32 max_sets) const -> DescriptorPool;
        [[nodiscard]] auto allocateDescriptorPool(std::span<const DescriptorPool::Size> sizes,
                                                  core::UInt32 max_sets) const
            -> DescriptorPoolOwnedPtr;
        [[nodiscard]] auto
            allocateRefCountedDescriptorPool(std::span<const DescriptorPool::Size> sizes,
                                             core::UInt32 max_sets) const
            -> DescriptorPoolSharedPtr;

        [[nodiscard]] auto createImage(const Image::CreateInfo &info) const -> Image;
        [[nodiscard]] auto allocateImage(const Image::CreateInfo &info) const -> ImageOwnedPtr;
        [[nodiscard]] auto allocateRefCountedImage(const Image::CreateInfo &info) const
            -> ImageSharedPtr;

        [[nodiscard]] auto createSampler(Sampler::Settings settings = Sampler::Settings {}) const
            -> Sampler;
        [[nodiscard]] auto allocateSampler(Sampler::Settings settings = Sampler::Settings {}) const
            -> SamplerOwnedPtr;
        [[nodiscard]] auto
            allocateRefCountedSampler(Sampler::Settings settings = Sampler::Settings {}) const
            -> SamplerSharedPtr;

        [[nodiscard]] auto
            createVertexBuffer(core::USize size,
                               MemoryPropertyFlag property = MemoryPropertyFlag::Device_Local) const
            -> Buffer;
        [[nodiscard]] auto allocateVertexBuffer(
            core::USize size,
            MemoryPropertyFlag property = MemoryPropertyFlag::Device_Local) const -> BufferOwnedPtr;
        [[nodiscard]] auto allocateRefCountedVertexBuffer(
            core::USize size,
            MemoryPropertyFlag property = MemoryPropertyFlag::Device_Local) const
            -> BufferSharedPtr;

        [[nodiscard]] auto createVertexBuffers(
            core::USize count,
            core::USize size,
            MemoryPropertyFlag property = MemoryPropertyFlag::Device_Local) const
            -> std::vector<Buffer>;
        [[nodiscard]] auto allocateVertexBuffers(
            core::USize count,
            core::USize size,
            MemoryPropertyFlag property = MemoryPropertyFlag::Device_Local) const
            -> std::vector<BufferOwnedPtr>;
        [[nodiscard]] auto allocateRefCountedVertexBuffers(
            core::USize count,
            core::USize size,
            MemoryPropertyFlag property = MemoryPropertyFlag::Device_Local) const
            -> std::vector<BufferSharedPtr>;

        [[nodiscard]] auto
            createIndexBuffer(core::USize size,
                              MemoryPropertyFlag property = MemoryPropertyFlag::Device_Local) const
            -> Buffer;
        [[nodiscard]] auto allocateIndexBuffer(
            core::USize size,
            MemoryPropertyFlag property = MemoryPropertyFlag::Device_Local) const -> BufferOwnedPtr;
        [[nodiscard]] auto allocateRefCountedIndexBuffer(
            core::USize size,
            MemoryPropertyFlag property = MemoryPropertyFlag::Device_Local) const
            -> BufferSharedPtr;

        [[nodiscard]] auto
            createIndexBuffers(core::USize count,
                               core::USize size,
                               MemoryPropertyFlag property = MemoryPropertyFlag::Device_Local) const
            -> std::vector<Buffer>;
        [[nodiscard]] auto allocateIndexBuffers(
            core::USize count,
            core::USize size,
            MemoryPropertyFlag property = MemoryPropertyFlag::Device_Local) const
            -> std::vector<BufferOwnedPtr>;
        [[nodiscard]] auto allocateRefCountedIndexBuffers(
            core::USize count,
            core::USize size,
            MemoryPropertyFlag property = MemoryPropertyFlag::Device_Local) const
            -> std::vector<BufferSharedPtr>;

        [[nodiscard]] auto createUniformBuffer(
            core::USize size,
            MemoryPropertyFlag property = MemoryPropertyFlag::Host_Visible |
                                          MemoryPropertyFlag::Host_Coherent) const -> Buffer;
        [[nodiscard]] auto allocateUniformBuffer(
            core::USize size,
            MemoryPropertyFlag property = MemoryPropertyFlag::Host_Visible |
                                          MemoryPropertyFlag::Host_Coherent) const
            -> BufferOwnedPtr;
        [[nodiscard]] auto allocateRefCountedUniformBuffer(
            core::USize size,
            MemoryPropertyFlag property = MemoryPropertyFlag::Host_Visible |
                                          MemoryPropertyFlag::Host_Coherent) const
            -> BufferSharedPtr;

        [[nodiscard]] auto createUniformBuffers(
            core::USize count,
            core::USize size,
            MemoryPropertyFlag property = MemoryPropertyFlag::Host_Visible |
                                          MemoryPropertyFlag::Host_Coherent) const
            -> std::vector<Buffer>;
        [[nodiscard]] auto allocateUniformBuffers(
            core::USize count,
            core::USize size,
            MemoryPropertyFlag property = MemoryPropertyFlag::Host_Visible |
                                          MemoryPropertyFlag::Host_Coherent) const
            -> std::vector<BufferOwnedPtr>;
        [[nodiscard]] auto allocateRefCountedUniformBuffers(
            core::USize count,
            core::USize size,
            MemoryPropertyFlag property = MemoryPropertyFlag::Host_Visible |
                                          MemoryPropertyFlag::Host_Coherent) const
            -> std::vector<BufferSharedPtr>;

        [[nodiscard]] auto createStagingBuffer(
            core::USize size,
            MemoryPropertyFlag property = MemoryPropertyFlag::Host_Visible |
                                          MemoryPropertyFlag::Host_Coherent) const -> Buffer;
        [[nodiscard]] auto allocateStagingBuffer(
            core::USize size,
            MemoryPropertyFlag property = MemoryPropertyFlag::Host_Visible |
                                          MemoryPropertyFlag::Host_Coherent) const
            -> BufferOwnedPtr;
        [[nodiscard]] auto allocateRefCountedStagingBuffer(
            core::USize size,
            MemoryPropertyFlag property = MemoryPropertyFlag::Host_Visible |
                                          MemoryPropertyFlag::Host_Coherent) const
            -> BufferSharedPtr;

        [[nodiscard]] auto createStagingBuffers(
            core::USize count,
            core::USize size,
            MemoryPropertyFlag property = MemoryPropertyFlag::Host_Visible |
                                          MemoryPropertyFlag::Host_Coherent) const
            -> std::vector<Buffer>;
        [[nodiscard]] auto allocateStagingBuffers(
            core::USize count,
            core::USize size,
            MemoryPropertyFlag property = MemoryPropertyFlag::Host_Visible |
                                          MemoryPropertyFlag::Host_Coherent) const
            -> std::vector<BufferOwnedPtr>;
        [[nodiscard]] auto allocateRefCountedStagingBuffers(
            core::USize count,
            core::USize size,
            MemoryPropertyFlag property = MemoryPropertyFlag::Host_Visible |
                                          MemoryPropertyFlag::Host_Coherent) const
            -> std::vector<BufferSharedPtr>;

        [[nodiscard]] auto graphicsQueue() const noexcept -> const Queue &;
        [[nodiscard]] auto asyncTransfertQueue() const noexcept -> const Queue &;
        [[nodiscard]] auto asyncComputeQueue() const noexcept -> const Queue &;

        [[nodiscard]] auto hasAsyncTransfertQueue() const noexcept -> bool;
        [[nodiscard]] auto hasAsyncComputeQueue() const noexcept -> bool;

        [[nodiscard]] auto physicalDevice() const noexcept -> const PhysicalDevice &;

        [[nodiscard]] operator VkDevice() const noexcept;
        [[nodiscard]] auto vkHandle() const noexcept -> VkDevice;

        [[nodiscard]] auto table() const noexcept -> const VolkDeviceTable &;

        [[nodiscard]] auto vmaAllocator() const noexcept -> VmaAllocator;

        template<typename T>
        auto setObjectName(const T &object, std::string_view name) const -> void;

        auto setObjectName(core::UInt64 object, DebugObjectType type, std::string_view name) const
            -> void;

      private:
        const PhysicalDevice *m_physical_device = nullptr;

        VkDevice m_device              = VK_NULL_HANDLE;
        VolkDeviceTable m_device_table = {};

        core::DeferredAlloc<Queue> m_graphics_queue        = std::nullopt;
        core::DeferredAlloc<Queue> m_async_transfert_queue = std::nullopt;
        core::DeferredAlloc<Queue> m_async_compute_queue   = std::nullopt;

        VmaAllocator m_vma_allocator = VK_NULL_HANDLE;

        VmaVulkanFunctions m_vma_device_table;
    };
} // namespace stormkit::gpu

#include "Device.inl"
