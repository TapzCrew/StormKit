// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <optional>

#include <stormkit/gpu/core/Device.mpp>
#include <stormkit/gpu/core/Instance.mpp>
#include <stormkit/gpu/core/PhysicalDevice.mpp>
#include <stormkit/gpu/core/PhysicalDeviceInfo.mpp>
#include <stormkit/gpu/core/Queue.mpp>
#include <stormkit/gpu/core/RenderCapabilities.mpp>


#include <stormkit/log/LogMacro.hpp>

#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

#define DELETER \
    [](auto handle, [[maybe_unused]] const auto &device) { vmaDestroyAllocator(handle); }

static constexpr auto RAYTRACING_EXTENSIONS =
    std::array { std::string_view { VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME },
                 std::string_view { VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME },
                 std::string_view { VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME },
                 std::string_view { VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME },
                 std::string_view { VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME },
                 std::string_view { VK_KHR_SPIRV_1_4_EXTENSION_NAME },
                 std::string_view { VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME } };

namespace stormkit::gpu {
    NAMED_LOGGER(device_logger, "StormKit.GPU.core.Device")

    template<QueueFlag flag, QueueFlag... no_flag>
    constexpr auto findQueue() {
        return [](const auto &family) {
            /*constexpr auto has_not_flags = [&family]() {
                if constexpr (sizeof...(no_flag) == 0) return true;
                else
                    return (core::checkFlag(family.flags, no_flag) && ...);
            }();*/

            return core::checkFlag(family.flags, flag) &&
                   (!core::checkFlag(family.flags, no_flag) && ...);
        };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Device::Device(const PhysicalDevice &physical_device, const Instance &instance)
        : m_instance { &instance }, m_physical_device { &physical_device } {
        const auto &queue_families = m_physical_device->queueFamilies();

        struct Queue_ {
            std::optional<core::UInt32> id = std::nullopt;
            core::UInt32 count             = 0u;
            core::Byte padding[3];
            QueueFlag flags = QueueFlag {};
        };

        const auto graphics_queue = [&queue_families]() -> Queue_ {
            const auto it = std::ranges::find_if(queue_families, findQueue<QueueFlag::Graphics>());
            if (it == std::ranges::cend(queue_families)) return {};

            return { .id    = std::distance(std::ranges::cbegin(queue_families), it),
                     .count = it->count,
                     .flags = it->flags };
        }();

        const auto compute_queue = [&queue_families]() -> Queue_ {
            const auto it =
                std::ranges::find_if(queue_families,
                                     findQueue<QueueFlag::Transfert, QueueFlag::Graphics>());
            if (it == std::ranges::cend(queue_families)) return {};

            return { .id    = std::distance(std::ranges::cbegin(queue_families), it),
                     .count = it->count,
                     .flags = it->flags };
        }();

        const auto transfert_queue = [&queue_families]() -> Queue_ {
            const auto it = std::ranges::find_if(
                queue_families,
                findQueue<QueueFlag::Compute, QueueFlag::Graphics, QueueFlag::Transfert>());
            if (it == std::ranges::cend(queue_families)) return {};

            return { .id    = std::distance(std::ranges::cbegin(queue_families), it),
                     .count = it->count,
                     .flags = it->flags };
        }();

        const auto queues = [&] {
            auto q = std::vector<const Queue_ *> {};
            q.reserve(3);

            if (graphics_queue.id) q.push_back(&graphics_queue);
            if (compute_queue.id) q.push_back(&compute_queue);
            if (transfert_queue.id) q.push_back(&transfert_queue);

            return q;
        }();

        auto priorities = std::vector<std::vector<float>> {};
        priorities.reserve(std::size(queues));

        const auto queue_create_infos = core::transform(queues, [&priorities](auto queue) {
            auto &priority = priorities.emplace_back();

            priority.resize(queue->count, 1.f);

            return VkDeviceQueueCreateInfo { .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                                             .queueFamilyIndex = queue->id.value(),
                                             .queueCount       = queue->count,
                                             .pQueuePriorities = std::data(priority) };
        });

        const auto enabled_features = VkPhysicalDeviceFeatures {
            .sampleRateShading = m_physical_device->capabilities().features.sampler_rate_shading,
            .multiDrawIndirect = m_physical_device->capabilities().features.multi_draw_indirect,
            .fillModeNonSolid  = m_physical_device->capabilities().features.fill_Mode_non_solid,
            .samplerAnisotropy = m_physical_device->capabilities().features.sampler_anisotropy,
        };

        const auto device_extensions = [&] {
            auto e = std::vector<VkExtensionProperties> {};

            auto count = core::UInt32 {};

            CHECK_VK_ERROR(
                vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &count, nullptr));
            e.resize(count);
            CHECK_VK_ERROR(vkEnumerateDeviceExtensionProperties(physical_device,
                                                                nullptr,
                                                                &count,
                                                                std::data(e)));

            return e;
        }();

        device_logger.dlog("Device extensions -----------");
        for (const auto &ext : device_extensions) device_logger.dlog("{}", ext.extensionName);
        device_logger.dlog("-------------------------------");

        auto raytracing_available = true;
        for (const auto &ext : RAYTRACING_EXTENSIONS) {
            if (std::ranges::find_if(device_extensions, [&](const auto &e) {
                    return ext == e.extensionName;
                }) == std::ranges::cend(device_extensions)) {
                raytracing_available = false;
                break;
            }
        }

        const auto extensions = [&] {
            auto e = std::vector<std::string_view> { std::begin(DEVICE_EXTENSIONS),
                                                     std::end(DEVICE_EXTENSIONS) };

            if (raytracing_available) {
                e.reserve(std::size(RAYTRACING_EXTENSIONS));
                std::ranges::copy(RAYTRACING_EXTENSIONS, std::back_inserter(e));

                device_logger.ilog("Raytracing supported !");
            }

            return e;
        }();

        using Next = void *;

        auto next = Next { nullptr };

        auto rt_pipeline_feature = VkPhysicalDeviceRayTracingPipelineFeaturesKHR {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR
        };

        auto acceleration_feature = VkPhysicalDeviceAccelerationStructureFeaturesKHR {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR
        };

        if (raytracing_available) {
            rt_pipeline_feature.pNext = reinterpret_cast<void *>(&acceleration_feature);

            next = reinterpret_cast<void *>(&rt_pipeline_feature);
        }

        const auto create_info = VkDeviceCreateInfo {
            .sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext                   = next,
            .queueCreateInfoCount    = core::as<core::UInt32>(std::size(queue_create_infos)),
            .pQueueCreateInfos       = std::data(queue_create_infos),
            .enabledExtensionCount   = core::as<core::UInt32>(std::size(DEVICE_EXTENSIONS)),
            .ppEnabledExtensionNames = std::data(DEVICE_EXTENSIONS),
            .pEnabledFeatures        = &enabled_features
        };

        CHECK_VK_ERROR(vkCreateDevice(*m_physical_device, &create_info, nullptr, &m_device));

        volkLoadDeviceTable(&m_device_table, m_device);

        auto graphics_vk_queue = VkQueue {};
        m_device_table.vkGetDeviceQueue(*this, graphics_queue.id.value(), 0, &graphics_vk_queue);
        m_graphics_queue =
            Queue { *this, graphics_queue.flags, graphics_queue.id.value(), graphics_vk_queue };

        setObjectName(*m_graphics_queue, "StormKit:GraphicsQueue");
        setObjectName(reinterpret_cast<core::UInt64>(m_graphics_queue->vkCommandPool()),
                      gpu::DebugObjectType::Command_Pool,
                      "StormKit:GraphicsQueueCommandPool");

        if (transfert_queue.id) {
            auto transfert_vk_queue = VkQueue {};
            m_device_table.vkGetDeviceQueue(*this,
                                            transfert_queue.id.value(),
                                            0,
                                            &transfert_vk_queue);
            m_async_transfert_queue = Queue { *this,
                                              transfert_queue.flags,
                                              transfert_queue.id.value(),
                                              transfert_vk_queue };

            setObjectName(*m_async_transfert_queue, "StormKit:TransfertQueue");
            setObjectName(reinterpret_cast<core::UInt64>(m_async_transfert_queue->vkCommandPool()),
                          gpu::DebugObjectType::Command_Pool,
                          "StormKit:TransfertQueueCommandPool");
        }

        if (compute_queue.id) {
            auto compute_vk_queue = VkQueue {};
            m_device_table.vkGetDeviceQueue(*this, compute_queue.id.value(), 0, &compute_vk_queue);
            m_async_compute_queue =
                Queue { *this, compute_queue.flags, compute_queue.id.value(), compute_vk_queue };

            setObjectName(*m_async_compute_queue, "StormKit:ComputeQueue");
            setObjectName(reinterpret_cast<core::UInt64>(m_async_compute_queue->vkCommandPool()),
                          gpu::DebugObjectType::Command_Pool,
                          "StormKit:ComputeQueueCommandPool");
        }

        m_vma_device_table = VmaVulkanFunctions {
            .vkGetPhysicalDeviceProperties       = vkGetPhysicalDeviceProperties,
            .vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties,
            .vkAllocateMemory                    = m_device_table.vkAllocateMemory,
            .vkFreeMemory                        = m_device_table.vkFreeMemory,
            .vkMapMemory                         = m_device_table.vkMapMemory,
            .vkUnmapMemory                       = m_device_table.vkUnmapMemory,
            .vkFlushMappedMemoryRanges           = m_device_table.vkFlushMappedMemoryRanges,
            .vkInvalidateMappedMemoryRanges      = m_device_table.vkInvalidateMappedMemoryRanges,
            .vkBindBufferMemory                  = m_device_table.vkBindBufferMemory,
            .vkBindImageMemory                   = m_device_table.vkBindImageMemory,
            .vkGetBufferMemoryRequirements       = m_device_table.vkGetBufferMemoryRequirements,
            .vkGetImageMemoryRequirements        = m_device_table.vkGetImageMemoryRequirements,
            .vkCreateBuffer                      = m_device_table.vkCreateBuffer,
            .vkDestroyBuffer                     = m_device_table.vkDestroyBuffer,
            .vkCreateImage                       = m_device_table.vkCreateImage,
            .vkDestroyImage                      = m_device_table.vkDestroyImage,
            .vkCmdCopyBuffer                     = m_device_table.vkCmdCopyBuffer,
            .vkGetBufferMemoryRequirements2KHR   = m_device_table.vkGetBufferMemoryRequirements2KHR,
            .vkGetImageMemoryRequirements2KHR    = m_device_table.vkGetImageMemoryRequirements2KHR,
            .vkBindBufferMemory2KHR              = m_device_table.vkBindBufferMemory2KHR,
            .vkBindImageMemory2KHR               = m_device_table.vkBindImageMemory2KHR,
            .vkGetPhysicalDeviceMemoryProperties2KHR = vkGetPhysicalDeviceMemoryProperties2KHR
        };

        const auto alloc_create_info =
            VmaAllocatorCreateInfo { .physicalDevice   = physical_device,
                                     .device           = *this,
                                     .pVulkanFunctions = &m_vma_device_table,
                                     .instance         = *m_instance };

        CHECK_VK_ERROR(vmaCreateAllocator(&alloc_create_info, &m_vma_allocator));

        setObjectName(*this,
                      core::format("StormKit:Device ({})", m_physical_device->info().device_name));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Device::~Device() {
        waitIdle();

        if (m_vma_allocator != VK_NULL_HANDLE) [[likely]]
            vmaDestroyAllocator(m_vma_allocator);

        m_graphics_queue        = std::nullopt;
        m_async_transfert_queue = std::nullopt;
        m_async_compute_queue   = std::nullopt;

        if (m_device != VK_NULL_HANDLE) [[likely]]
            m_device_table.vkDestroyDevice(m_device, nullptr);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Device::Device(Device &&other) noexcept
        : m_instance { std::exchange(other.m_instance, nullptr) },
          m_physical_device { std::exchange(other.m_physical_device, nullptr) },
          m_device { std::exchange(other.m_device, VK_NULL_HANDLE) },
          m_device_table { std::exchange(other.m_device_table, {}) },
          m_graphics_queue { std::exchange(other.m_graphics_queue, std::nullopt) },
          m_async_transfert_queue { std::exchange(other.m_async_transfert_queue, std::nullopt) },
          m_async_compute_queue { std::exchange(other.m_async_compute_queue, std::nullopt) },
          m_vma_allocator { std::exchange(other.m_vma_allocator, VK_NULL_HANDLE) },
          m_vma_device_table { std::exchange(other.m_vma_device_table, {}) } {}

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::operator=(Device &&other) noexcept -> Device & {
        if (&other == this) [[unlikely]]
            return *this;

        m_instance              = std::exchange(other.m_instance, nullptr);
        m_physical_device       = std::exchange(other.m_physical_device, nullptr);
        m_device                = std::exchange(other.m_device, VK_NULL_HANDLE);
        m_device_table          = std::exchange(other.m_device_table, {});
        m_graphics_queue        = std::exchange(other.m_graphics_queue, std::nullopt);
        m_async_transfert_queue = std::exchange(other.m_async_transfert_queue, std::nullopt);
        m_async_compute_queue   = std::exchange(other.m_async_compute_queue, std::nullopt);
        m_vma_allocator         = std::exchange(other.m_vma_allocator, VK_NULL_HANDLE);
        m_vma_device_table      = std::exchange(other.m_vma_device_table, {});

        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::waitIdle() const noexcept -> void {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        CHECK_VK_ERROR(m_device_table.vkDeviceWaitIdle(m_device));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::waitForFences(std::span<const FenceConstRef> fences,
                               bool wait_all,
                               std::chrono::milliseconds timeout) const noexcept -> void {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        auto vk_fences = std::vector<VkFence> {};
        vk_fences.reserve(core::as<core::USize>(std::size(fences)));

        std::ranges::transform(fences,
                               std::back_inserter(vk_fences),
                               [](const auto &fence) -> VkFence { return fence.get(); });

        CHECK_VK_ERROR(m_device_table.vkWaitForFences(
            m_device,
            std::size(vk_fences),
            std::data(vk_fences),
            wait_all,
            std::chrono::duration_cast<std::chrono::nanoseconds>(timeout).count()));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::createShader(std::filesystem::path filepath, ShaderStageFlag type) const
        -> Shader {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return { std::move(filepath), type, *this };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::allocateShader(std::filesystem::path filepath, ShaderStageFlag type) const
        -> ShaderOwnedPtr {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return std::make_unique<Shader>(std::move(filepath), type, *this);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::allocateRefCountedShader(std::filesystem::path filepath,
                                          ShaderStageFlag type) const -> ShaderSharedPtr {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return std::make_shared<Shader>(std::move(filepath), type, *this);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::createShader(core::ByteConstSpan data, ShaderStageFlag type) const -> Shader {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return Shader { data, type, *this };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::allocateShader(core::ByteConstSpan data, ShaderStageFlag type) const
        -> ShaderOwnedPtr {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return std::make_unique<Shader>(data, type, *this);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::allocateRefCountedShader(core::ByteConstSpan data, ShaderStageFlag type) const
        -> ShaderSharedPtr {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return std::make_shared<Shader>(data, type, *this);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::createShader(std::span<const SpirvID> data, ShaderStageFlag type) const -> Shader {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return Shader { data, type, *this };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::allocateShader(std::span<const SpirvID> data, ShaderStageFlag type) const
        -> ShaderOwnedPtr {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return std::make_unique<Shader>(data, type, *this);
    }
    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::allocateRefCountedShader(std::span<const SpirvID> data, ShaderStageFlag type) const
        -> ShaderSharedPtr {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return std::make_shared<Shader>(data, type, *this);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::createGraphicsPipeline(const PipelineCache *cache) const -> GraphicsPipeline {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return GraphicsPipeline { *this, cache };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::allocateGraphicsPipeline(const PipelineCache *cache) const
        -> GraphicsPipelineOwnedPtr {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return std::make_unique<GraphicsPipeline>(*this, cache);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::allocateRefCountedGraphicsPipeline(const PipelineCache *cache) const
        -> GraphicsPipelineSharedPtr {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return std::make_shared<GraphicsPipeline>(*this, cache);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::createComputePipeline(const PipelineCache *cache) const -> ComputePipeline {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return ComputePipeline { *this, cache };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::allocateComputePipeline(const PipelineCache *cache) const
        -> ComputePipelineOwnedPtr {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return std::make_unique<ComputePipeline>(*this, cache);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::allocateRefCountedComputePipeline(const PipelineCache *cache) const
        -> ComputePipelineSharedPtr {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return std::make_shared<ComputePipeline>(*this, cache);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::createPipelineCache(std::filesystem::path path) const -> PipelineCache {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return PipelineCache { *this, std::move(path) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::allocatePipelineCache(std::filesystem::path path) const -> PipelineCacheOwnedPtr {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return std::make_unique<PipelineCache>(*this, std::move(path));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::allocateRefCountedPipelineCache(std::filesystem::path path) const
        -> PipelineCacheSharedPtr {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return std::make_shared<PipelineCache>(*this, std::move(path));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::createRenderPass(const RenderPassDescription &description) const -> RenderPass {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return RenderPass { *this, description };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::allocateRenderPass(const RenderPassDescription &description) const
        -> RenderPassOwnedPtr {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return std::make_unique<RenderPass>(*this, description);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::allocateRefCountedRenderPass(const RenderPassDescription &description) const
        -> RenderPassSharedPtr {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return std::make_shared<RenderPass>(*this, description);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::createFence(bool signaled) const -> Fence {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return Fence { *this, signaled };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::allocateFence(bool signaled) const -> FenceOwnedPtr {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return std::make_unique<Fence>(*this, signaled);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::allocateRefCountedFence(bool signaled) const -> FenceSharedPtr {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return std::make_shared<Fence>(*this, signaled);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::createSemaphore() const -> Semaphore {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return Semaphore { *this };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::allocateSemaphore() const -> SemaphoreOwnedPtr {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return std::make_unique<Semaphore>(*this);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::allocateRefCountedSemaphore() const -> SemaphoreSharedPtr {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return std::make_shared<Semaphore>(*this);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::createBuffer(const Buffer::CreateInfo &info) const -> Buffer {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return Buffer { *this, info };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::allocateBuffer(const Buffer::CreateInfo &info) const -> BufferOwnedPtr {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return std::make_unique<Buffer>(*this, info);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::allocateRefCountedBuffer(const Buffer::CreateInfo &info) const -> BufferSharedPtr {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return std::make_shared<Buffer>(*this, info);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::createBuffers(core::USize count, const Buffer::CreateInfo &info) const
        -> std::vector<Buffer> {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        auto buffers = std::vector<Buffer> {};
        buffers.reserve(count);
        for (auto i : core::range(count)) buffers.emplace_back(createBuffer(info));

        return buffers;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::allocateBuffers(core::USize count, const Buffer::CreateInfo &info) const
        -> std::vector<BufferOwnedPtr> {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        auto buffers = std::vector<BufferOwnedPtr> {};
        buffers.reserve(count);

        for (auto i : core::range(count)) buffers.emplace_back(allocateBuffer(info));

        return buffers;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::allocateRefCountedBuffers(core::USize count, const Buffer::CreateInfo &info) const
        -> std::vector<BufferSharedPtr> {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        auto buffers = std::vector<BufferSharedPtr> {};
        buffers.reserve(count);

        for (auto i : core::range(count)) buffers.emplace_back(allocateRefCountedBuffer(info));

        return buffers;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::createDescriptorSetLayout() const -> DescriptorSetLayout {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return DescriptorSetLayout { *this };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::allocateDescriptorSetLayout() const -> DescriptorSetLayoutOwnedPtr {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return std::make_unique<DescriptorSetLayout>(*this);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::allocateRefCountedDescriptorSetLayout() const -> DescriptorSetLayoutSharedPtr {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return std::make_shared<DescriptorSetLayout>(*this);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::createDescriptorPool(std::span<const DescriptorPool::Size> sizes,
                                      core::UInt32 max_sets) const -> DescriptorPool {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return DescriptorPool { *this, sizes, max_sets };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::allocateDescriptorPool(std::span<const DescriptorPool::Size> sizes,
                                        core::UInt32 max_sets) const -> DescriptorPoolOwnedPtr {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return std::make_unique<DescriptorPool>(*this, sizes, max_sets);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::allocateRefCountedDescriptorPool(std::span<const DescriptorPool::Size> sizes,
                                                  core::UInt32 max_sets) const
        -> DescriptorPoolSharedPtr {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return std::make_shared<DescriptorPool>(*this, sizes, max_sets);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::createImage(const Image::CreateInfo &info) const -> Image {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return Image { *this, info };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::allocateImage(const Image::CreateInfo &info) const -> ImageOwnedPtr {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return std::make_unique<Image>(*this, info);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::allocateRefCountedImage(const Image::CreateInfo &info) const -> ImageSharedPtr {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return std::make_shared<Image>(*this, info);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::createSampler(Sampler::Settings settings) const -> Sampler {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return Sampler { std::move(settings), *this };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::allocateSampler(Sampler::Settings settings) const -> SamplerOwnedPtr {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return std::make_unique<Sampler>(std::move(settings), *this);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::allocateRefCountedSampler(Sampler::Settings settings) const -> SamplerSharedPtr {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        return std::make_shared<Sampler>(std::move(settings), *this);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::setObjectName(core::UInt64 object,
                               DebugObjectType type,
                               std::string_view name) const -> void {
        STORMKIT_EXPECTS(m_device != VK_NULL_HANDLE);

        if (!vkSetDebugUtilsObjectNameEXT) return;

        const auto info =
            VkDebugUtilsObjectNameInfoEXT { .sType =
                                                VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
                                            .objectType   = core::as<VkObjectType>(type),
                                            .objectHandle = object,
                                            .pObjectName  = std::data(name) };

        CHECK_VK_ERROR(vkSetDebugUtilsObjectNameEXT(m_device, &info));
    }
} // namespace stormkit::gpu
