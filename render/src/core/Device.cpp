// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/////////// - STL - ///////////
#include <unordered_set>

/////////// - StormKit::core - ///////////
#include <storm/core/Strings.hpp>
/////////// - StormKit::core - ///////////
#include <storm/core/Format.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/core/Device.hpp>
#include <storm/render/core/Instance.hpp>
#include <storm/render/core/PhysicalDevice.hpp>
#include <storm/render/core/Queue.hpp>
#include <storm/render/core/Surface.hpp>

#include <storm/render/pipeline/ComputePipeline.hpp>
#include <storm/render/pipeline/DescriptorPool.hpp>
#include <storm/render/pipeline/DescriptorSetLayout.hpp>
#include <storm/render/pipeline/GraphicsPipeline.hpp>
#include <storm/render/pipeline/PipelineCache.hpp>
#include <storm/render/pipeline/RenderPass.hpp>

#include <storm/render/resource/HardwareBuffer.hpp>
#include <storm/render/resource/Sampler.hpp>
#include <storm/render/resource/Shader.hpp>
#include <storm/render/resource/Texture.hpp>

#include <storm/render/sync/Fence.hpp>
#include <storm/render/sync/Semaphore.hpp>

#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_IMPLEMENTATION
#include <storm/render/core/vk_mem_alloc.h>

using namespace storm;
using namespace storm::render;

#define DELETER \
    [](auto handle, [[maybe_unused]] const auto &device) { vmaDestroyAllocator(handle); }

/////////////////////////////////////
/////////////////////////////////////
Device::Device(const PhysicalDevice &physical_device, const Instance &instance)
    : m_instance { &instance }, m_physical_device { &physical_device }, m_vma_allocator { DELETER,
                                                                                          *this } {
    const auto &queue_families = m_physical_device->queueFamilies();

    struct Queue_ {
        std::optional<core::UInt32> id = std::nullopt;
        core::UInt32 count             = 0u;
        core::Byte padding[3];
        QueueFlag flags = QueueFlag {};
    };

    auto graphics_queue  = Queue_ {};
    auto compute_queue   = Queue_ {};
    auto transfert_queue = Queue_ {};

    auto i = 0;
    for (const auto &family : queue_families) {
        if ((family.flags & QueueFlag::Graphics) == QueueFlag::Graphics) {
            graphics_queue.id    = i;
            graphics_queue.count = family.count;
            graphics_queue.flags = family.flags;

            break;
        }
    }

    i = 0;
    for (const auto &family : queue_families) {
        if ((family.flags & QueueFlag::Transfert) == QueueFlag::Transfert &&
            i != graphics_queue.id) {
            transfert_queue.id    = i;
            transfert_queue.count = family.count;
            transfert_queue.flags = family.flags;

            break;
        }
    }

    i = 0;
    for (const auto &family : queue_families) {
        if ((family.flags & QueueFlag::Compute) == QueueFlag::Compute && i != graphics_queue.id &&
            i != transfert_queue.id) {
            compute_queue.id    = i;
            compute_queue.count = family.count;
            compute_queue.flags = family.flags;

            break;
        }
    }

    auto priorities         = std::vector<std::vector<float>> {};
    auto queue_create_infos = std::vector<vk::DeviceQueueCreateInfo> {};
    for (auto &queue : { graphics_queue, transfert_queue, compute_queue }) {
        if (queue.id.has_value()) {
            auto &priority    = priorities.emplace_back();
            auto &create_info = queue_create_infos.emplace_back();

            priority.resize(queue.count, 1.f);

            create_info.setQueueFamilyIndex(queue.id.value())
                .setQueueCount(queue.count)
                .setPQueuePriorities(std::data(priority));
        }
    }

    auto enabled_features = vk::PhysicalDeviceFeatures {};
    enabled_features.setFillModeNonSolid(
        m_physical_device->capabilities().features.fill_Mode_non_solid);
    enabled_features.setSampleRateShading(
        m_physical_device->capabilities().features.sampler_rate_shading);
    enabled_features.setSamplerAnisotropy(
        m_physical_device->capabilities().features.sampler_anisotropy);
    enabled_features.setMultiDrawIndirect(
        m_physical_device->capabilities().features.multi_draw_indirect);

    const auto create_info =
        vk::DeviceCreateInfo {}
            .setQueueCreateInfoCount(gsl::narrow_cast<core::UInt32>(std::size(queue_create_infos)))
            .setPQueueCreateInfos(std::data(queue_create_infos))
            .setEnabledExtensionCount(gsl::narrow_cast<core::UInt32>(std::size(DEVICE_EXTENSIONS)))
            .setPpEnabledExtensionNames(std::data(DEVICE_EXTENSIONS))
            .setPEnabledFeatures(&enabled_features);

    m_vk_device = physical_device.createVkDevice(create_info);

    auto vkGetInstanceProcAddr = m_instance->loader().vkGetInstanceProcAddr();
    m_vkGetDeviceProcAddr      = reinterpret_cast<PFN_vkGetDeviceProcAddr>(
        vkGetInstanceProcAddr(m_instance->vkInstance(), "vkGetDeviceProcAddr"));
    m_vk_dispatcher.init(m_instance->vkInstance(),
                         vkGetInstanceProcAddr,
                         vkDevice(),
                         m_vkGetDeviceProcAddr);

    auto graphics_vk_queue = vkDevice().getQueue(graphics_queue.id.value(), 0, m_vk_dispatcher);
    m_graphics_queue       = std::make_unique<Queue>(*this,
                                               graphics_queue.flags,
                                               graphics_queue.id.value(),
                                               graphics_vk_queue);

    setObjectName(*m_graphics_queue, "StormKit:GraphicsQueue");
    setObjectName(reinterpret_cast<core::UInt64>(
                      m_graphics_queue->vkCommandPool().operator VkCommandPool_T *()),
                  render::DebugObjectType::Command_Pool,
                  "StormKit:GraphicsQueueCommandPool");

    if (transfert_queue.id.has_value()) {
        auto transfert_vk_queue =
            vkDevice().getQueue(transfert_queue.id.value(), 0, m_vk_dispatcher);
        m_async_transfert_queue = std::make_unique<Queue>(*this,
                                                          transfert_queue.flags,
                                                          transfert_queue.id.value(),
                                                          transfert_vk_queue);

        setObjectName<Queue>(*m_async_transfert_queue, "StormKit:TransfertQueue");
        setObjectName(reinterpret_cast<core::UInt64>(
                          m_async_transfert_queue->vkCommandPool().operator VkCommandPool_T *()),
                      render::DebugObjectType::Command_Pool,
                      "StormKit:TransfertQueueCommandPool");
    }

    if (compute_queue.id.has_value()) {
        auto compute_vk_queue = vkDevice().getQueue(compute_queue.id.value(), 0, m_vk_dispatcher);
        m_async_compute_queue = std::make_unique<Queue>(*this,
                                                        compute_queue.flags,
                                                        compute_queue.id.value(),
                                                        compute_vk_queue);

        setObjectName<Queue>(*m_async_compute_queue, "StormKit:Comput Queue");
        setObjectName(reinterpret_cast<core::UInt64>(
                          m_async_compute_queue->vkCommandPool().operator VkCommandPool_T *()),
                      render::DebugObjectType::Command_Pool,
                      "StormKit:ComputeQueueCommandPool");
    }

    m_vma_device_table = VmaVulkanFunctions {
        .vkGetPhysicalDeviceProperties =
            VULKAN_HPP_DEFAULT_DISPATCHER.vkGetPhysicalDeviceProperties,
        .vkGetPhysicalDeviceMemoryProperties =
            VULKAN_HPP_DEFAULT_DISPATCHER.vkGetPhysicalDeviceMemoryProperties,
        .vkAllocateMemory                  = m_vk_dispatcher.vkAllocateMemory,
        .vkFreeMemory                      = m_vk_dispatcher.vkFreeMemory,
        .vkMapMemory                       = m_vk_dispatcher.vkMapMemory,
        .vkUnmapMemory                     = m_vk_dispatcher.vkUnmapMemory,
        .vkFlushMappedMemoryRanges         = m_vk_dispatcher.vkFlushMappedMemoryRanges,
        .vkInvalidateMappedMemoryRanges    = m_vk_dispatcher.vkInvalidateMappedMemoryRanges,
        .vkBindBufferMemory                = m_vk_dispatcher.vkBindBufferMemory,
        .vkBindImageMemory                 = m_vk_dispatcher.vkBindImageMemory,
        .vkGetBufferMemoryRequirements     = m_vk_dispatcher.vkGetBufferMemoryRequirements,
        .vkGetImageMemoryRequirements      = m_vk_dispatcher.vkGetImageMemoryRequirements,
        .vkCreateBuffer                    = m_vk_dispatcher.vkCreateBuffer,
        .vkDestroyBuffer                   = m_vk_dispatcher.vkDestroyBuffer,
        .vkCreateImage                     = m_vk_dispatcher.vkCreateImage,
        .vkDestroyImage                    = m_vk_dispatcher.vkDestroyImage,
        .vkCmdCopyBuffer                   = m_vk_dispatcher.vkCmdCopyBuffer,
        .vkGetBufferMemoryRequirements2KHR = m_vk_dispatcher.vkGetBufferMemoryRequirements2KHR,
        .vkGetImageMemoryRequirements2KHR  = m_vk_dispatcher.vkGetImageMemoryRequirements2KHR,
        .vkBindBufferMemory2KHR            = m_vk_dispatcher.vkBindBufferMemory2KHR,
        .vkBindImageMemory2KHR             = m_vk_dispatcher.vkBindImageMemory2KHR
    };

    const auto alloc_create_info =
        VmaAllocatorCreateInfo { .physicalDevice   = physical_device.vkPhysicalDevice(),
                                 .device           = vkDevice(),
                                 .pVulkanFunctions = &m_vma_device_table,
                                 .instance         = m_instance->vkInstance() };

    auto result = vmaCreateAllocator(&alloc_create_info, &m_vma_allocator.handle());
    STORMKIT_ENSURES(result == VK_SUCCESS);

    setObjectName(*this,
                  core::format("StormKit:Device ({})", m_physical_device->info().device_name));
}

Device::Device(VkDevice device, const PhysicalDevice &physical_device, const Instance &instance)
    : m_instance { &instance }, m_physical_device { &physical_device }, m_vk_device { device },
      m_vma_allocator { DELETER, *this } {
    const auto &queue_families = m_physical_device->queueFamilies();

    struct Queue_ {
        std::optional<core::UInt32> id = std::nullopt;
        core::UInt32 count             = 0u;
        core::Byte padding[3];
        QueueFlag flags = QueueFlag {};
    };

    auto graphics_queue  = Queue_ {};
    auto compute_queue   = Queue_ {};
    auto transfert_queue = Queue_ {};

    auto i = 0;
    for (const auto &family : queue_families) {
        if ((family.flags & QueueFlag::Graphics) == QueueFlag::Graphics) {
            graphics_queue.id    = i;
            graphics_queue.count = family.count;
            graphics_queue.flags = family.flags;

            break;
        }
    }

    i = 0;
    for (const auto &family : queue_families) {
        if ((family.flags & QueueFlag::Transfert) == QueueFlag::Transfert &&
            i != graphics_queue.id) {
            transfert_queue.id    = i;
            transfert_queue.count = family.count;
            transfert_queue.flags = family.flags;

            break;
        }
    }

    i = 0;
    for (const auto &family : queue_families) {
        if ((family.flags & QueueFlag::Compute) == QueueFlag::Compute && i != graphics_queue.id &&
            i != transfert_queue.id) {
            compute_queue.id    = i;
            compute_queue.count = family.count;
            compute_queue.flags = family.flags;

            break;
        }
    }

    auto priorities         = std::vector<std::vector<float>> {};
    auto queue_create_infos = std::vector<vk::DeviceQueueCreateInfo> {};
    for (auto &queue : { graphics_queue, transfert_queue, compute_queue }) {
        if (queue.id.has_value()) {
            auto &priority    = priorities.emplace_back();
            auto &create_info = queue_create_infos.emplace_back();

            priority.resize(queue.count, 1.f);

            create_info.setQueueFamilyIndex(queue.id.value())
                .setQueueCount(queue.count)
                .setPQueuePriorities(std::data(priority));
        }
    }

    auto vkGetInstanceProcAddr = m_instance->loader().vkGetInstanceProcAddr();
    m_vkGetDeviceProcAddr      = reinterpret_cast<PFN_vkGetDeviceProcAddr>(
        vkGetInstanceProcAddr(m_instance->vkInstance(), "vkGetDeviceProcAddr"));
    m_vk_dispatcher.init(m_instance->vkInstance(),
                         vkGetInstanceProcAddr,
                         vkDevice(),
                         m_vkGetDeviceProcAddr);

    auto graphics_vk_queue = vkDevice().getQueue(graphics_queue.id.value(), 0, m_vk_dispatcher);
    m_graphics_queue       = std::make_unique<Queue>(*this,
                                               graphics_queue.flags,
                                               graphics_queue.id.value(),
                                               graphics_vk_queue);

    setObjectName(*m_graphics_queue, "StormKit:GraphicsQueue");
    setObjectName(reinterpret_cast<core::UInt64>(
                      m_graphics_queue->vkCommandPool().operator VkCommandPool_T *()),
                  render::DebugObjectType::Command_Pool,
                  "StormKit:GraphicsQueueCommandPool");

    if (transfert_queue.id.has_value()) {
        auto transfert_vk_queue =
            vkDevice().getQueue(transfert_queue.id.value(), 0, m_vk_dispatcher);
        m_async_transfert_queue = std::make_unique<Queue>(*this,
                                                          transfert_queue.flags,
                                                          transfert_queue.id.value(),
                                                          transfert_vk_queue);

        setObjectName<Queue>(*m_async_transfert_queue, "StormKit:TransfertQueue");
        setObjectName(reinterpret_cast<core::UInt64>(
                          m_async_transfert_queue->vkCommandPool().operator VkCommandPool_T *()),
                      render::DebugObjectType::Command_Pool,
                      "StormKit:TransfertQueueCommandPool");
    }

    if (compute_queue.id.has_value()) {
        auto compute_vk_queue = vkDevice().getQueue(compute_queue.id.value(), 0, m_vk_dispatcher);
        m_async_compute_queue = std::make_unique<Queue>(*this,
                                                        compute_queue.flags,
                                                        compute_queue.id.value(),
                                                        compute_vk_queue);

        setObjectName<Queue>(*m_async_compute_queue, "StormKit:Comput Queue");
        setObjectName(reinterpret_cast<core::UInt64>(
                          m_async_compute_queue->vkCommandPool().operator VkCommandPool_T *()),
                      render::DebugObjectType::Command_Pool,
                      "StormKit:ComputeQueueCommandPool");
    }

    m_vma_device_table = VmaVulkanFunctions {
        .vkGetPhysicalDeviceProperties =
            VULKAN_HPP_DEFAULT_DISPATCHER.vkGetPhysicalDeviceProperties,
        .vkGetPhysicalDeviceMemoryProperties =
            VULKAN_HPP_DEFAULT_DISPATCHER.vkGetPhysicalDeviceMemoryProperties,
        .vkAllocateMemory                  = m_vk_dispatcher.vkAllocateMemory,
        .vkFreeMemory                      = m_vk_dispatcher.vkFreeMemory,
        .vkMapMemory                       = m_vk_dispatcher.vkMapMemory,
        .vkUnmapMemory                     = m_vk_dispatcher.vkUnmapMemory,
        .vkFlushMappedMemoryRanges         = m_vk_dispatcher.vkFlushMappedMemoryRanges,
        .vkInvalidateMappedMemoryRanges    = m_vk_dispatcher.vkInvalidateMappedMemoryRanges,
        .vkBindBufferMemory                = m_vk_dispatcher.vkBindBufferMemory,
        .vkBindImageMemory                 = m_vk_dispatcher.vkBindImageMemory,
        .vkGetBufferMemoryRequirements     = m_vk_dispatcher.vkGetBufferMemoryRequirements,
        .vkGetImageMemoryRequirements      = m_vk_dispatcher.vkGetImageMemoryRequirements,
        .vkCreateBuffer                    = m_vk_dispatcher.vkCreateBuffer,
        .vkDestroyBuffer                   = m_vk_dispatcher.vkDestroyBuffer,
        .vkCreateImage                     = m_vk_dispatcher.vkCreateImage,
        .vkDestroyImage                    = m_vk_dispatcher.vkDestroyImage,
        .vkCmdCopyBuffer                   = m_vk_dispatcher.vkCmdCopyBuffer,
        .vkGetBufferMemoryRequirements2KHR = m_vk_dispatcher.vkGetBufferMemoryRequirements2KHR,
        .vkGetImageMemoryRequirements2KHR  = m_vk_dispatcher.vkGetImageMemoryRequirements2KHR,
        .vkBindBufferMemory2KHR            = m_vk_dispatcher.vkBindBufferMemory2KHR,
        .vkBindImageMemory2KHR             = m_vk_dispatcher.vkBindImageMemory2KHR
    };

    const auto alloc_create_info =
        VmaAllocatorCreateInfo { .physicalDevice   = physical_device.vkPhysicalDevice(),
                                 .device           = vkDevice(),
                                 .pVulkanFunctions = &m_vma_device_table,
                                 .instance         = m_instance->vkInstance() };

    auto result = vmaCreateAllocator(&alloc_create_info, &m_vma_allocator.handle());
    STORMKIT_ENSURES(result == VK_SUCCESS);

    setObjectName(*this,
                  core::format("StormKit:Device ({})", m_physical_device->info().device_name));
}

/////////////////////////////////////
/////////////////////////////////////
Device::~Device() = default;

/////////////////////////////////////
/////////////////////////////////////
Device::Device(Device &&other) noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
Device &Device::operator=(Device &&other) noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
void Device::waitIdle() const noexcept {
    const auto result = vkDevice().waitIdle(m_vk_dispatcher);
    STORMKIT_ENSURES(result == vk::Result::eSuccess);
}

/////////////////////////////////////
/////////////////////////////////////
void Device::waitForFences(storm::core::span<const FenceConstRef> fences,
                           bool wait_all,
                           core::UInt64 timeout) const noexcept {
    auto vk_fences = std::vector<vk::Fence> {};
    vk_fences.reserve(gsl::narrow_cast<core::ArraySize>(std::size(fences)));

    for (const auto &fence : fences)
        vk_fences.emplace_back(static_cast<const Fence &>(fence.get()));

    const auto result = vkDevice().waitForFences(vk_fences, wait_all, timeout, m_vk_dispatcher);
    STORMKIT_ENSURES(result == vk::Result::eSuccess);
}

/////////////////////////////////////
/////////////////////////////////////
Shader Device::createShader(std::filesystem::path filepath, ShaderStage type) const {
    return { std::move(filepath), type, *this };
}

/////////////////////////////////////
/////////////////////////////////////
ShaderOwnedPtr Device::createShaderPtr(std::filesystem::path filepath, ShaderStage type) const {
    return std::make_unique<Shader>(std::move(filepath), type, *this);
}

/////////////////////////////////////
/////////////////////////////////////
Shader Device::createShader(core::span<const core::Byte> data, ShaderStage type) const {
    return Shader { data, type, *this };
}

/////////////////////////////////////
/////////////////////////////////////
ShaderOwnedPtr Device::createShaderPtr(core::span<const core::Byte> data, ShaderStage type) const {
    return std::make_unique<Shader>(data, type, *this);
}

/////////////////////////////////////
/////////////////////////////////////
Shader Device::createShader(core::span<const core::UInt32> data, ShaderStage type) const {
    return Shader { data, type, *this };
}

/////////////////////////////////////
/////////////////////////////////////
ShaderOwnedPtr Device::createShaderPtr(core::span<const core::UInt32> data,
                                       ShaderStage type) const {
    return std::make_unique<Shader>(data, type, *this);
}

/////////////////////////////////////
/////////////////////////////////////
GraphicsPipeline Device::createGraphicsPipeline(PipelineCacheConstPtr cache) const {
    return GraphicsPipeline { *this, cache };
}

/////////////////////////////////////
/////////////////////////////////////
GraphicsPipelineOwnedPtr Device::createGraphicsPipelinePtr(PipelineCacheConstPtr cache) const {
    return std::make_unique<GraphicsPipeline>(*this, cache);
}

/////////////////////////////////////
/////////////////////////////////////
ComputePipeline Device::createComputePipeline(PipelineCacheConstPtr cache) const {
    return ComputePipeline { *this, cache };
}

/////////////////////////////////////
/////////////////////////////////////
ComputePipelineOwnedPtr Device::createComputePipelinePtr(PipelineCacheConstPtr cache) const {
    return std::make_unique<ComputePipeline>(*this, cache);
}

/////////////////////////////////////
/////////////////////////////////////
RenderPass Device::createRenderPass(RenderPassDescription description) const {
    return RenderPass { *this, std::move(description) };
}

/////////////////////////////////////
/////////////////////////////////////
RenderPassOwnedPtr Device::createRenderPassPtr(RenderPassDescription description) const {
    return std::make_unique<RenderPass>(*this, std::move(description));
}

/////////////////////////////////////
/////////////////////////////////////
Fence Device::createFence() const {
    return Fence { *this };
}

/////////////////////////////////////
/////////////////////////////////////
FenceOwnedPtr Device::createFencePtr() const {
    return std::make_unique<Fence>(*this);
}

/////////////////////////////////////
/////////////////////////////////////
Semaphore Device::createSemaphore() const {
    return Semaphore { *this };
}

/////////////////////////////////////
/////////////////////////////////////
SemaphoreOwnedPtr Device::createSemaphorePtr() const {
    return std::make_unique<Semaphore>(*this);
}

/////////////////////////////////////
/////////////////////////////////////
HardwareBuffer Device::createHardwareBuffer(HardwareBufferUsage usage,
                                            core::ArraySize size,
                                            MemoryProperty property) const {
    return HardwareBuffer { *this, usage, size, property };
}

/////////////////////////////////////
/////////////////////////////////////
HardwareBufferArray Device::createHardwareBuffers(core::ArraySize count,
                                                  HardwareBufferUsage usage,
                                                  core::ArraySize size,
                                                  MemoryProperty property) const {
    auto buffers = HardwareBufferArray {};
    buffers.reserve(count);
    for (auto i = 0u; i < count; ++i)
        buffers.emplace_back(createHardwareBuffer(usage, size, property));

    return buffers;
}

/////////////////////////////////////
/////////////////////////////////////
HardwareBufferOwnedPtr Device::createHardwareBufferPtr(HardwareBufferUsage usage,
                                                       core::ArraySize size,
                                                       MemoryProperty property) const {
    return std::make_unique<HardwareBuffer>(*this, usage, size, property);
}

/////////////////////////////////////
/////////////////////////////////////
HardwareBufferOwnedPtrArray Device::createHardwareBuffersPtr(core::ArraySize count,
                                                             HardwareBufferUsage usage,
                                                             core::ArraySize size,
                                                             MemoryProperty property) const {
    auto buffers = HardwareBufferOwnedPtrArray {};
    buffers.reserve(count);
    for (auto i = 0u; i < count; ++i)
        buffers.emplace_back(createHardwareBufferPtr(usage, size, property));

    return buffers;
}

/////////////////////////////////////
/////////////////////////////////////
DescriptorSetLayout Device::createDescriptorSetLayout() const {
    return DescriptorSetLayout { *this };
}

/////////////////////////////////////
/////////////////////////////////////
DescriptorSetLayoutOwnedPtr Device::createDescriptorSetLayoutPtr() const {
    return std::make_unique<DescriptorSetLayout>(*this);
}

/////////////////////////////////////
/////////////////////////////////////
DescriptorPool Device::createDescriptorPool(std::vector<DescriptorPool::Size> sizes,
                                            core::ArraySize max_sets) const {
    return DescriptorPool { *this, std::move(sizes), max_sets };
}

/////////////////////////////////////
/////////////////////////////////////
DescriptorPoolOwnedPtr Device::createDescriptorPoolPtr(std::vector<DescriptorPool::Size> sizes,
                                                       core::ArraySize max_sets) const {
    return std::make_unique<DescriptorPool>(*this, std::move(sizes), max_sets);
}

/////////////////////////////////////
/////////////////////////////////////
Texture Device::createTexture(core::Extentu extent,
                              PixelFormat format,
                              core::UInt32 layers,
                              core::UInt32 mip_levels,
                              TextureType type,
                              TextureCreateFlag flags,
                              SampleCountFlag samples,
                              TextureUsage usage,
                              TextureTiling tiling,
                              MemoryProperty property) const {
    return Texture { *this,
                     std::move(extent),
                     format,
                     layers,
                     mip_levels,
                     type,
                     flags,
                     samples,
                     usage,
                     tiling,
                     property };
}

/////////////////////////////////////
/////////////////////////////////////
TextureOwnedPtr Device::createTexturePtr(core::Extentu extent,
                                         PixelFormat format,
                                         core::UInt32 layers,
                                         core::UInt32 mip_levels,
                                         TextureType type,
                                         TextureCreateFlag flags,
                                         SampleCountFlag samples,
                                         TextureUsage usage,
                                         TextureTiling tiling,
                                         MemoryProperty property) const {
    return std::make_unique<Texture>(*this,
                                     std::move(extent),
                                     format,
                                     layers,
                                     mip_levels,
                                     type,
                                     flags,
                                     samples,
                                     usage,
                                     tiling,
                                     property);
}

/////////////////////////////////////
/////////////////////////////////////
Sampler Device::createSampler(Sampler::Settings settings) const {
    return Sampler { std::move(settings), *this };
}

/////////////////////////////////////
/////////////////////////////////////
SamplerOwnedPtr Device::createSamplerPtr(Sampler::Settings settings) const {
    return std::make_unique<Sampler>(std::move(settings), *this);
}

/////////////////////////////////////
/////////////////////////////////////
PipelineCache Device::createPipelineCache(std::filesystem::path path) const {
    return PipelineCache { *this, path };
}

/////////////////////////////////////
/////////////////////////////////////
PipelineCacheOwnedPtr Device::createPipelineCachePtr(std::filesystem::path path) const {
    return std::make_unique<PipelineCache>(*this, path);
}

/////////////////////////////////////
/////////////////////////////////////
vk::Result Device::waitForVkFences(gsl::span<const vk::Fence> fences,
                                   core::UInt64 wait_for,
                                   bool wait_all) const noexcept {
    return vkDevice().waitForFences(gsl::narrow_cast<core::UInt32>(std::size(fences)),
                                    std::data(fences),
                                    wait_all,
                                    wait_for,
                                    m_vk_dispatcher);
}

/////////////////////////////////////
/////////////////////////////////////
void Device::resetVkFence(vk::Fence fence) const noexcept {
    const auto fences = std::array { fence };
    vkDevice().resetFences(fences, m_vk_dispatcher);
}

/////////////////////////////////////
/////////////////////////////////////
vk::Result Device::getVkFenceStatus(vk::Fence fence) const noexcept {
    return vkDevice().getFenceStatus(fence, m_vk_dispatcher);
}

/////////////////////////////////////
/////////////////////////////////////
vk::MemoryRequirements Device::getVkBufferMemoryRequirements(vk::Buffer buffer) const noexcept {
    return vkDevice().getBufferMemoryRequirements(buffer, m_vk_dispatcher);
}

/////////////////////////////////////
/////////////////////////////////////
vk::MemoryRequirements Device::getVkImageMemoryRequirements(vk::Image image) const noexcept {
    return vkDevice().getImageMemoryRequirements(image, m_vk_dispatcher);
}

/////////////////////////////////////
/////////////////////////////////////
std::pair<vk::Result, core::UInt32> Device::vkAcquireNextImage(vk::SwapchainKHR swapchain,
                                                               core::UInt64 timeout,
                                                               vk::Semaphore semaphore,
                                                               vk::Fence fence) const {
    auto texture_index = 0u;
    auto result        = vkDevice().acquireNextImageKHR(swapchain,
                                                 timeout,
                                                 semaphore,
                                                 fence,
                                                 &texture_index,
                                                 m_vk_dispatcher);

    return std::make_pair(result, texture_index);
}

/////////////////////////////////////
/////////////////////////////////////
std::vector<vk::Image> Device::getVkSwapchainImages(vk::SwapchainKHR swapchain) const noexcept {
    CHECK_VK_ERROR_VALUE(vkDevice().getSwapchainImagesKHR(swapchain, m_vk_dispatcher), images);
    return images;
}

/////////////////////////////////////
/////////////////////////////////////
void Device::updateVkDescriptorSets(gsl::span<const vk::WriteDescriptorSet> writes,
                                    gsl::span<const vk::CopyDescriptorSet> copies) const noexcept {
    vkDevice().updateDescriptorSets(gsl::narrow_cast<core::UInt32>(std::size(writes)),
                                    std::data(writes),
                                    gsl::narrow_cast<core::UInt32>(std::size(copies)),
                                    std::data(copies),
                                    m_vk_dispatcher);
}

/////////////////////////////////////
/////////////////////////////////////
RAIIVkSwapchain
    Device::createVkSwapchain(const vk::SwapchainCreateInfoKHR &create_info) const noexcept {
    CHECK_VK_ERROR_VALUE(vkDevice().createSwapchainKHRUnique(create_info, nullptr, m_vk_dispatcher),
                         swapchain);

    return swapchain;
}

/////////////////////////////////////
/////////////////////////////////////
RAIIVkShaderModule
    Device::createVkShaderModule(const vk::ShaderModuleCreateInfo &create_info) const noexcept {
    CHECK_VK_ERROR_VALUE(vkDevice().createShaderModuleUnique(create_info, nullptr, m_vk_dispatcher),
                         shader_module);

    return shader_module;
}

/////////////////////////////////////
/////////////////////////////////////
RAIIVkPipeline Device::createVkGraphicsPipeline(const vk::GraphicsPipelineCreateInfo &create_info,
                                                const vk::PipelineCache &cache) const noexcept {
    CHECK_VK_ERROR_VALUE(vkDevice().createGraphicsPipelineUnique(cache,
                                                                 create_info,
                                                                 nullptr,
                                                                 m_vk_dispatcher),
                         pipeline);

    return pipeline;
}

/////////////////////////////////////
/////////////////////////////////////
RAIIVkPipeline Device::createVkGraphicsPipelineUnsafe(const vk::GraphicsPipelineCreateInfo &create_info,
                                                const vk::PipelineCache &cache) const noexcept {
    auto result = vkDevice().createGraphicsPipelineUnique(cache,
                                                                 create_info,
                                                                 nullptr,
                                                                 m_vk_dispatcher);

    if(result.result != vk::Result::eSuccess) return {};

    return std::move(result.value);
}

/////////////////////////////////////
/////////////////////////////////////
RAIIVkPipeline Device::createVkComputePipeline(const vk::ComputePipelineCreateInfo &create_info,
                                               const vk::PipelineCache &cache) const noexcept {
    CHECK_VK_ERROR_VALUE(vkDevice().createComputePipelineUnique(cache,
                                                                create_info,
                                                                nullptr,
                                                                m_vk_dispatcher),
                         pipeline);

    return pipeline;
}

vk::UniquePipelineCache
    Device::createVkPipelineCache(const vk::PipelineCacheCreateInfo &create_info) const noexcept {
    CHECK_VK_ERROR_VALUE(vkDevice().createPipelineCacheUnique(create_info,
                                                              nullptr,
                                                              m_vk_dispatcher),
                         pipeline_cache);

    return pipeline_cache;
}

/////////////////////////////////////
/////////////////////////////////////
RAIIVkPipelineLayout
    Device::createVkPipelineLayout(const vk::PipelineLayoutCreateInfo &create_info) const noexcept {
    CHECK_VK_ERROR_VALUE(vkDevice().createPipelineLayoutUnique(create_info,
                                                               nullptr,
                                                               m_vk_dispatcher),
                         pipeline_layout);

    return pipeline_layout;
}

/////////////////////////////////////
/////////////////////////////////////
RAIIVkRenderPass
    Device::createVkRenderPass(const vk::RenderPassCreateInfo &create_info) const noexcept {
    CHECK_VK_ERROR_VALUE(vkDevice().createRenderPassUnique(create_info, nullptr, m_vk_dispatcher),
                         render_pass);

    return render_pass;
}

/////////////////////////////////////
/////////////////////////////////////
RAIIVkImage Device::createVkImage(const vk::ImageCreateInfo &create_info) const noexcept {
    CHECK_VK_ERROR_VALUE(vkDevice().createImageUnique(create_info, nullptr, m_vk_dispatcher),
                         image);

    return image;
}

/////////////////////////////////////
/////////////////////////////////////
RAIIVkImageView
    Device::createVkImageView(const vk::ImageViewCreateInfo &create_info) const noexcept {
    CHECK_VK_ERROR_VALUE(vkDevice().createImageViewUnique(create_info, nullptr, m_vk_dispatcher),
                         image_view);

    return image_view;
}

/////////////////////////////////////
/////////////////////////////////////
RAIIVkSampler Device::createVkSampler(const vk::SamplerCreateInfo &create_info) const noexcept {
    CHECK_VK_ERROR_VALUE(vkDevice().createSamplerUnique(create_info, nullptr, m_vk_dispatcher),
                         sampler);

    return sampler;
}

/////////////////////////////////////
/////////////////////////////////////
RAIIVkFramebuffer
    Device::createVkFramebuffer(const vk::FramebufferCreateInfo &create_info) const noexcept {
    CHECK_VK_ERROR_VALUE(vkDevice().createFramebufferUnique(create_info, nullptr, m_vk_dispatcher),
                         framebuffer);

    return framebuffer;
}

/////////////////////////////////////
/////////////////////////////////////
RAIIVkCommandPool
    Device::createVkCommandPool(const vk::CommandPoolCreateInfo &create_info) const noexcept {
    CHECK_VK_ERROR_VALUE(vkDevice().createCommandPoolUnique(create_info, nullptr, m_vk_dispatcher),
                         command_pool);

    return command_pool;
}

/////////////////////////////////////
/////////////////////////////////////
RAIIVkFence Device::createVkFence(const vk::FenceCreateInfo &create_info) const noexcept {
    CHECK_VK_ERROR_VALUE(vkDevice().createFenceUnique(create_info, nullptr, m_vk_dispatcher),
                         fence);

    return fence;
}

/////////////////////////////////////
/////////////////////////////////////
RAIIVkSemaphore
    Device::createVkSemaphore(const vk::SemaphoreCreateInfo &create_info) const noexcept {
    CHECK_VK_ERROR_VALUE(vkDevice().createSemaphoreUnique(create_info, nullptr, m_vk_dispatcher),
                         semaphore);

    return semaphore;
}

/////////////////////////////////////
/////////////////////////////////////
RAIIVkBuffer Device::createVkBuffer(const vk::BufferCreateInfo &create_info) const noexcept {
    CHECK_VK_ERROR_VALUE(vkDevice().createBufferUnique(create_info, nullptr, m_vk_dispatcher),
                         buffer);

    return buffer;
}

/////////////////////////////////////
/////////////////////////////////////
RAIIVkDescriptorSetLayout Device::createVkDescriptorSetLayout(
    const vk::DescriptorSetLayoutCreateInfo &create_info) const noexcept {
    CHECK_VK_ERROR_VALUE(vkDevice().createDescriptorSetLayoutUnique(create_info,
                                                                    nullptr,
                                                                    m_vk_dispatcher),
                         descriptor_layout);

    return descriptor_layout;
}

/////////////////////////////////////
/////////////////////////////////////
RAIIVkDescriptorPool
    Device::createVkDescriptorPool(const vk::DescriptorPoolCreateInfo &create_info) const noexcept {
    CHECK_VK_ERROR_VALUE(vkDevice().createDescriptorPoolUnique(create_info,
                                                               nullptr,
                                                               m_vk_dispatcher),
                         descriptor_pool);

    return descriptor_pool;
}

/////////////////////////////////////
/////////////////////////////////////
std::vector<RAIIVkCommandBuffer> Device::allocateVkCommandBuffers(
    const vk::CommandBufferAllocateInfo &allocate_info) const noexcept {
    CHECK_VK_ERROR_VALUE(vkDevice().allocateCommandBuffersUnique(allocate_info, m_vk_dispatcher),
                         buffers);

    return buffers;
}

/////////////////////////////////////
/////////////////////////////////////
std::vector<RAIIVkDescriptorSet> Device::allocateVkDescriptorSets(
    const vk::DescriptorSetAllocateInfo &allocate_info) const noexcept {
    CHECK_VK_ERROR_VALUE(vkDevice().allocateDescriptorSetsUnique(allocate_info, m_vk_dispatcher),
                         sets);

    return sets;
}

/////////////////////////////////////
/////////////////////////////////////
VmaAllocation
    Device::allocateVmaAllocation(const VmaAllocationCreateInfo &allocate_info,
                                  const vk::MemoryRequirements &requirements) const noexcept {
    auto vma_allocation = VmaAllocation {};

    auto result = vmaAllocateMemory(m_vma_allocator,
                                    &requirements.operator const VkMemoryRequirements &(),
                                    &allocate_info,
                                    &vma_allocation,
                                    nullptr);
    STORMKIT_ENSURES_MESSAGE(result == VK_SUCCESS, result);

    return vma_allocation;
}

/////////////////////////////////////
/////////////////////////////////////
void Device::deallocateVmaAllocation(VmaAllocation allocation) const noexcept {
    STORMKIT_EXPECTS(allocation != VK_NULL_HANDLE);

    vmaFreeMemory(m_vma_allocator, allocation);
}

/////////////////////////////////////
/////////////////////////////////////
void Device::bindVmaBufferMemory(VmaAllocation allocation, vk::Buffer buffer) const noexcept {
    STORMKIT_EXPECTS(allocation != VK_NULL_HANDLE);
    STORMKIT_EXPECTS(buffer);

    vmaBindBufferMemory(m_vma_allocator, allocation, buffer);
}

/////////////////////////////////////
/////////////////////////////////////
void Device::bindVmaImageMemory(VmaAllocation allocation, vk::Image image) const noexcept {
    STORMKIT_EXPECTS(allocation != VK_NULL_HANDLE);
    STORMKIT_EXPECTS(image);

    vmaBindImageMemory(m_vma_allocator, allocation, image);
}

/////////////////////////////////////
/////////////////////////////////////
core::Byte *Device::mapVmaMemory(VmaAllocation allocation) const noexcept {
    core::Byte *data = nullptr;

    auto result = vmaMapMemory(m_vma_allocator, allocation, reinterpret_cast<void **>(&data));
    STORMKIT_ENSURES(result == VK_SUCCESS);

    return data;
}

/////////////////////////////////////
/////////////////////////////////////
void Device::unmapVmaMemory(VmaAllocation allocation) const noexcept {
    vmaUnmapMemory(m_vma_allocator, allocation);
}

/////////////////////////////////////
/////////////////////////////////////
void Device::setObjectName(core::UInt64 object, DebugObjectType type, std::string_view name) const {
    if (!m_vk_dispatcher.vkSetDebugUtilsObjectNameEXT) return;

    const auto info = vk::DebugUtilsObjectNameInfoEXT {}
                          .setObjectType(toVK(type))
                          .setObjectHandle(object)
                          .setPObjectName(std::data(name));

    const auto result = vkDevice().setDebugUtilsObjectNameEXT(info, m_vk_dispatcher);
    STORMKIT_ENSURES(result == vk::Result::eSuccess);
}
