// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - GSL - ///////////
#include <gsl/string_span>

/////////// - StormKit::core - ///////////
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>
#include <storm/render/core/Vulkan.hpp>

#include <storm/render/pipeline/ComputePipeline.hpp>
#include <storm/render/pipeline/DescriptorPool.hpp>
#include <storm/render/pipeline/Fwd.hpp>
#include <storm/render/pipeline/GraphicsPipeline.hpp>
#include <storm/render/pipeline/RenderPassDescription.hpp>

#include <storm/render/resource/Fwd.hpp>
#include <storm/render/resource/HardwareBuffer.hpp>
#include <storm/render/resource/Sampler.hpp>
#include <storm/render/resource/Shader.hpp>

#include <storm/render/sync/Fence.hpp>
#include <storm/render/sync/Fwd.hpp>
#include <storm/render/sync/Semaphore.hpp>

namespace storm::render {
    class STORMKIT_PUBLIC Device: public core::NonCopyable {
      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Device;

        Device(const PhysicalDevice &physical_device, const Instance &instance);
        Device(VkDevice device, const PhysicalDevice &physical_device, const Instance &instance);
        ~Device();

        Device(Device &&) noexcept;
        Device &operator=(Device &&) noexcept;

        void waitIdle() const noexcept;
        void waitForFences(
            storm::core::span<const FenceConstRef> fences,
            bool wait_all        = true,
            core::UInt64 timeout = std::numeric_limits<core::UInt64>::max()) const noexcept;
        inline void waitForFence(
            const Fence &fence,
            core::UInt64 timeout = std::numeric_limits<core::UInt64>::max()) const noexcept;

        Shader createShader(std::filesystem::path filepath, ShaderStage type) const;
        ShaderOwnedPtr createShaderPtr(std::filesystem::path filepath, ShaderStage type) const;
        Shader createShader(core::span<const core::Byte> data, ShaderStage type) const;
        ShaderOwnedPtr createShaderPtr(core::span<const core::Byte> data, ShaderStage type) const;
        Shader createShader(core::span<const core::UInt32> data, ShaderStage type) const;
        ShaderOwnedPtr createShaderPtr(core::span<const core::UInt32> data, ShaderStage type) const;
        template<typename Container,
                 std::enable_if_t<!std::is_same_v<Container, std::filesystem::path>>>
        inline Shader createShader(const Container &container, ShaderStage type) const;
        template<typename Container,
                 std::enable_if_t<!std::is_same_v<Container, std::filesystem::path>>>
        inline ShaderOwnedPtr createShaderPtr(const Container &container, ShaderStage type) const;

        GraphicsPipeline createGraphicsPipeline(PipelineCacheConstPtr cache = nullptr) const;
        GraphicsPipelineOwnedPtr
            createGraphicsPipelinePtr(PipelineCacheConstPtr cache = nullptr) const;
        ComputePipeline createComputePipeline(PipelineCacheConstPtr cache = nullptr) const;
        ComputePipelineOwnedPtr
            createComputePipelinePtr(PipelineCacheConstPtr cache = nullptr) const;

        RenderPass createRenderPass(RenderPassDescription description) const;
        RenderPassOwnedPtr createRenderPassPtr(RenderPassDescription description) const;

        Fence createFence() const;
        FenceOwnedPtr createFencePtr() const;

        Semaphore createSemaphore() const;
        SemaphoreOwnedPtr createSemaphorePtr() const;

        HardwareBuffer createHardwareBuffer(HardwareBufferUsage usage,
                                            core::ArraySize size,
                                            MemoryProperty property) const;
        HardwareBufferArray createHardwareBuffers(core::ArraySize count,
                                                  HardwareBufferUsage usage,
                                                  core::ArraySize size,
                                                  MemoryProperty property) const;
        HardwareBufferOwnedPtr createHardwareBufferPtr(HardwareBufferUsage usage,
                                                       core::ArraySize size,
                                                       MemoryProperty property) const;
        HardwareBufferOwnedPtrArray createHardwareBuffersPtr(core::ArraySize count,
                                                             HardwareBufferUsage usage,
                                                             core::ArraySize size,
                                                             MemoryProperty property) const;

        DescriptorSetLayout createDescriptorSetLayout() const;
        DescriptorSetLayoutOwnedPtr createDescriptorSetLayoutPtr() const;

        DescriptorPool createDescriptorPool(std::vector<DescriptorPool::Size> sizes,
                                            core::ArraySize max_sets) const;
        DescriptorPoolOwnedPtr createDescriptorPoolPtr(std::vector<DescriptorPool::Size> sizes,
                                                       core::ArraySize max_sets) const;

        Texture createTexture(core::Extentu extent,
                              PixelFormat format      = PixelFormat::RGBA8_UNorm,
                              core::UInt32 layers     = 1u,
                              core::UInt32 mip_levels = 1u,
                              TextureType type        = TextureType::T2D,
                              TextureCreateFlag flags = TextureCreateFlag::None,
                              SampleCountFlag samples = SampleCountFlag::C1_BIT,
                              TextureUsage usage      = TextureUsage::Sampled |
                                                        TextureUsage::Transfert_Dst |
                                                        TextureUsage::Transfert_Src,
                              TextureTiling tiling    = TextureTiling::Optimal,
                              MemoryProperty property = MemoryProperty::Device_Local) const;
        TextureOwnedPtr
            createTexturePtr(core::Extentu extent,
                             PixelFormat format      = render::PixelFormat::RGBA8_UNorm,
                             core::UInt32 layers     = 1u,
                             core::UInt32 mip_levels = 1u,
                             TextureType type        = TextureType::T2D,
                             TextureCreateFlag flags = TextureCreateFlag::None,
                             SampleCountFlag samples = SampleCountFlag::C1_BIT,
                             TextureUsage usage      = TextureUsage::Sampled |
                                                       TextureUsage::Transfert_Dst |
                                                       TextureUsage::Transfert_Src,
                              TextureTiling tiling    = TextureTiling::Optimal,
                              MemoryProperty property = MemoryProperty::Device_Local) const;

        Sampler createSampler(Sampler::Settings settings = Sampler::Settings {}) const;
        SamplerOwnedPtr createSamplerPtr(Sampler::Settings settings = Sampler::Settings {}) const;

        PipelineCache createPipelineCache(std::filesystem::path path = "") const;
        PipelineCacheOwnedPtr createPipelineCachePtr(std::filesystem::path path = "") const;

        inline HardwareBuffer createVertexBuffer(
            core::ArraySize size,
            MemoryProperty property = MemoryProperty::Host_Visible | MemoryProperty::Host_Coherent,
            bool use_staging        = false) const;
        inline HardwareBufferArray createVertexBuffers(
            core::ArraySize count,
            core::ArraySize size,
            MemoryProperty property = MemoryProperty::Host_Visible | MemoryProperty::Host_Coherent,
            bool use_staging        = false) const;
        inline HardwareBufferOwnedPtr createVertexBufferPtr(
            core::ArraySize size,
            MemoryProperty property = MemoryProperty::Host_Visible | MemoryProperty::Host_Coherent,
            bool use_staging        = false) const;
        inline HardwareBufferOwnedPtrArray createVertexBuffersPtr(
            core::ArraySize count,
            core::ArraySize size,
            MemoryProperty property = MemoryProperty::Host_Visible | MemoryProperty::Host_Coherent,
            bool use_staging        = false) const;

        inline HardwareBuffer createIndexBuffer(
            core::ArraySize size,
            MemoryProperty property = MemoryProperty::Host_Visible | MemoryProperty::Host_Coherent,
            bool use_staging        = false) const;
        inline HardwareBufferArray createIndexBuffers(
            core::ArraySize count,
            core::ArraySize size,
            MemoryProperty property = MemoryProperty::Host_Visible | MemoryProperty::Host_Coherent,
            bool use_staging        = false) const;
        inline HardwareBufferOwnedPtr createIndexBufferPtr(
            core::ArraySize size,
            MemoryProperty property = MemoryProperty::Host_Visible | MemoryProperty::Host_Coherent,
            bool use_staging        = false) const;
        inline HardwareBufferOwnedPtrArray createIndexBuffersPtr(
            core::ArraySize count,
            core::ArraySize size,
            MemoryProperty property = MemoryProperty::Host_Visible | MemoryProperty::Host_Coherent,
            bool use_staging        = false) const;

        inline HardwareBuffer createUniformBuffer(
            core::ArraySize size,
            MemoryProperty property = MemoryProperty::Host_Visible | MemoryProperty::Host_Coherent,
            bool use_staging        = false) const;

        inline HardwareBufferArray createUniformBuffers(
            core::ArraySize count,
            core::ArraySize size,
            MemoryProperty property = MemoryProperty::Host_Visible | MemoryProperty::Host_Coherent,
            bool use_staging        = false) const;
        inline HardwareBufferOwnedPtr createUniformBufferPtr(
            core::ArraySize size,
            MemoryProperty property = MemoryProperty::Host_Visible | MemoryProperty::Host_Coherent,
            bool use_staging        = false) const;
        inline HardwareBufferOwnedPtrArray createUniformBuffersPtr(
            core::ArraySize count,
            core::ArraySize size,
            MemoryProperty property = MemoryProperty::Host_Visible | MemoryProperty::Host_Coherent,
            bool use_staging        = false) const;

        inline HardwareBuffer
            createStagingBuffer(core::ArraySize size,
                                MemoryProperty property = MemoryProperty::Host_Visible |
                                                          MemoryProperty::Host_Coherent) const;
        inline HardwareBufferArray
            createStagingBuffers(core::ArraySize count,
                                 core::ArraySize size,
                                 MemoryProperty property = MemoryProperty::Host_Visible |
                                                           MemoryProperty::Host_Coherent) const;
        inline HardwareBufferOwnedPtr
            createStagingBufferPtr(core::ArraySize size,
                                   MemoryProperty property = MemoryProperty::Host_Visible |
                                                             MemoryProperty::Host_Coherent) const;
        inline HardwareBufferOwnedPtrArray
            createStagingBuffersPtr(core::ArraySize count,
                                    core::ArraySize size,
                                    MemoryProperty property = MemoryProperty::Host_Visible |
                                                              MemoryProperty::Host_Coherent) const;

        inline const Queue &graphicsQueue() const noexcept;
        inline const Queue &asyncTransfertQueue() const noexcept;
        inline const Queue &asyncComputeQueue() const noexcept;

        inline bool hasAsyncTransfertQueue() const noexcept;
        inline bool hasAsyncComputeQueue() const noexcept;

        inline const PhysicalDevice &physicalDevice() const noexcept;

        inline vk::Device vkDevice() const noexcept;
        inline operator vk::Device() const noexcept;
        inline vk::Device vkHandle() const noexcept;
        inline core::UInt64 vkDebugHandle() const noexcept;

        inline const vk::DispatchLoaderDynamic &vkDispatcher() const noexcept;

        inline VmaAllocator vmaAllocator() const noexcept;

        inline vk::Result waitForVkFence(
            vk::Fence fence,
            core::UInt64 wait_for = std::numeric_limits<core::UInt64>::max()) const noexcept {
            auto fences = std::array { std::move(fence) };

            return waitForVkFences(fences, wait_for);
        }
        vk::Result waitForVkFences(gsl::span<const vk::Fence> fences,
                                   core::UInt64 wait_for = std::numeric_limits<core::UInt64>::max(),
                                   bool wait_all         = true) const noexcept;
        void resetVkFence(vk::Fence fence) const noexcept;
        vk::Result getVkFenceStatus(vk::Fence fence) const noexcept;

        vk::MemoryRequirements getVkBufferMemoryRequirements(vk::Buffer buffer) const noexcept;
        vk::MemoryRequirements getVkImageMemoryRequirements(vk::Image image) const noexcept;

        std::pair<vk::Result, core::UInt32> vkAcquireNextImage(vk::SwapchainKHR swapchain,
                                                               core::UInt64 timeout,
                                                               vk::Semaphore semaphore,
                                                               vk::Fence fence) const;
        std::vector<vk::Image> getVkSwapchainImages(vk::SwapchainKHR swapchain) const noexcept;

        void updateVkDescriptorSets(gsl::span<const vk::WriteDescriptorSet> writes,
                                    gsl::span<const vk::CopyDescriptorSet> copies) const noexcept;
        RAIIVkSwapchain
            createVkSwapchain(const vk::SwapchainCreateInfoKHR &create_info) const noexcept;

        RAIIVkShaderModule
            createVkShaderModule(const vk::ShaderModuleCreateInfo &create_info) const noexcept;

        RAIIVkPipeline createVkGraphicsPipeline(
            const vk::GraphicsPipelineCreateInfo &create_info,
            const vk::PipelineCache &cache = VK_NULL_HANDLE) const noexcept;
        RAIIVkPipeline createVkGraphicsPipelineUnsafe(
            const vk::GraphicsPipelineCreateInfo &create_info,
            const vk::PipelineCache &cache = VK_NULL_HANDLE) const noexcept;
        RAIIVkPipeline
            createVkComputePipeline(const vk::ComputePipelineCreateInfo &create_info,
                                    const vk::PipelineCache &cache = VK_NULL_HANDLE) const noexcept;

        vk::UniquePipelineCache
            createVkPipelineCache(const vk::PipelineCacheCreateInfo &create_info) const noexcept;

        RAIIVkPipelineLayout
            createVkPipelineLayout(const vk::PipelineLayoutCreateInfo &create_info) const noexcept;

        RAIIVkRenderPass
            createVkRenderPass(const vk::RenderPassCreateInfo &create_info) const noexcept;

        RAIIVkImage createVkImage(const vk::ImageCreateInfo &create_info) const noexcept;

        RAIIVkImageView
            createVkImageView(const vk::ImageViewCreateInfo &create_info) const noexcept;

        RAIIVkSampler createVkSampler(const vk::SamplerCreateInfo &create_info) const noexcept;

        RAIIVkFramebuffer
            createVkFramebuffer(const vk::FramebufferCreateInfo &create_info) const noexcept;

        RAIIVkCommandPool
            createVkCommandPool(const vk::CommandPoolCreateInfo &create_info) const noexcept;

        RAIIVkFence createVkFence(const vk::FenceCreateInfo &create_info) const noexcept;

        RAIIVkSemaphore
            createVkSemaphore(const vk::SemaphoreCreateInfo &create_info) const noexcept;

        RAIIVkBuffer createVkBuffer(const vk::BufferCreateInfo &create_info) const noexcept;

        RAIIVkDescriptorSetLayout createVkDescriptorSetLayout(
            const vk::DescriptorSetLayoutCreateInfo &create_info) const noexcept;

        RAIIVkDescriptorPool
            createVkDescriptorPool(const vk::DescriptorPoolCreateInfo &create_info) const noexcept;

        std::vector<RAIIVkCommandBuffer> allocateVkCommandBuffers(
            const vk::CommandBufferAllocateInfo &allocate_info) const noexcept;

        std::vector<RAIIVkDescriptorSet> allocateVkDescriptorSets(
            const vk::DescriptorSetAllocateInfo &allocate_info) const noexcept;

        VmaAllocation
            allocateVmaAllocation(const VmaAllocationCreateInfo &allocate_info,
                                  const vk::MemoryRequirements &requirements) const noexcept;
        void deallocateVmaAllocation(VmaAllocation allocation) const noexcept;

        void bindVmaBufferMemory(VmaAllocation allocation, vk::Buffer buffer) const noexcept;
        void bindVmaImageMemory(VmaAllocation allocation, vk::Image image) const noexcept;

        core::Byte *mapVmaMemory(VmaAllocation allocation) const noexcept;
        void unmapVmaMemory(VmaAllocation allocation) const noexcept;

        template<typename T>
        inline void setObjectName(const T &object, std::string_view name) const;

        void setObjectName(core::UInt64 object, DebugObjectType type, std::string_view name) const;

      private:
        InstanceConstPtr m_instance;

        PhysicalDeviceConstPtr m_physical_device;

        PFN_vkGetDeviceProcAddr m_vkGetDeviceProcAddr;

        QueueOwnedPtr m_graphics_queue;
        QueueOwnedPtr m_async_transfert_queue;
        QueueOwnedPtr m_async_compute_queue;

        std::variant<vk::UniqueDevice, vk::Device> m_vk_device;
        vk::DispatchLoaderDynamic m_vk_dispatcher;

        RAIIVmaAllocator m_vma_allocator;

        VmaVulkanFunctions m_vma_device_table;
    };
} // namespace storm::render

#include "Device.inl"
