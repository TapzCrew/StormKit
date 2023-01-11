// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#ifdef STORMKIT_BUILD_MODULES
module;
#endif

#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

#ifdef STORMKIT_BUILD_MODULES
module stormkit.Gpu:Core;
#else
    #include <stormkit/std.hpp>

    #include <stormkit/Core.hpp>
    #include <stormkit/Gpu.hpp>
#endif

#define DELETER \
    [](auto handle, [[maybe_unused]] const auto& device) { vmaDestroyAllocator(handle); }

static constexpr auto RAYTRACING_EXTENSIONS = stormkit::core::makeStaticArray<std::string_view>(
    VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
    VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
    VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
    VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
    VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
    VK_KHR_SPIRV_1_4_EXTENSION_NAME,
    VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME);

static constexpr auto BASE_EXTENSIONS =
    stormkit::core::makeStaticArray<std::string_view>(VK_KHR_MAINTENANCE3_EXTENSION_NAME);

static constexpr auto SWAPCHAIN_EXTENSIONS =
    stormkit::core::makeStaticArray<std::string_view>(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

namespace stormkit::gpu {
    NAMED_LOGGER(device_logger, "stormkit.Gpu:core.Device")

    template<QueueFlag flag, QueueFlag... no_flag>
    constexpr auto findQueue() {
        return [](const auto& family) {
            return core::checkFlag(family.flags, flag) and
                   (not core::checkFlag(family.flags, no_flag) and ...);
        };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Device::Device(const PhysicalDevice& physical_device, const Instance& instance, Info info)
        : InstanceObject { instance }, m_physical_device { &physical_device } {
        const auto& queue_families = m_physical_device->queueFamilies();

        struct Queue_ {
            std::optional<core::UInt32> id = std::nullopt;
            core::UInt32 count             = 0u;
            core::Byte padding[3];
            QueueFlag flags = QueueFlag {};
        };

        const auto raster_queue = [&queue_families]() -> Queue_ {
            const auto it = std::ranges::find_if(queue_families, findQueue<QueueFlag::Graphics>());
            if (it == std::ranges::cend(queue_families)) return {};

            return { .id = core::as<core::UInt32>(
                         std::distance(std::ranges::cbegin(queue_families), it)),
                     .count = it->count,
                     .flags = it->flags };
        }();

        const auto compute_queue = [&queue_families]() -> Queue_ {
            const auto it =
                std::ranges::find_if(queue_families,
                                     findQueue<QueueFlag::Transfert, QueueFlag::Graphics>());
            if (it == std::ranges::cend(queue_families)) return {};

            return { .id = core::as<core::UInt32>(
                         std::distance(std::ranges::cbegin(queue_families), it)),
                     .count = it->count,
                     .flags = it->flags };
        }();

        const auto transfert_queue = [&queue_families]() -> Queue_ {
            const auto it = std::ranges::find_if(
                queue_families,
                findQueue<QueueFlag::Compute, QueueFlag::Graphics, QueueFlag::Transfert>());
            if (it == std::ranges::cend(queue_families)) return {};

            return { .id = core::as<core::UInt32>(
                         std::distance(std::ranges::cbegin(queue_families), it)),
                     .count = it->count,
                     .flags = it->flags };
        }();

        const auto queues = [&] {
            auto q = std::vector<const Queue_ *> {};
            q.reserve(3);

            if (raster_queue.id) q.push_back(&raster_queue);
            if (compute_queue.id) q.push_back(&compute_queue);
            if (transfert_queue.id) q.push_back(&transfert_queue);

            return q;
        }();

        auto priorities = std::vector<std::vector<float>> {};
        priorities.reserve(std::size(queues));

        const auto queue_create_infos = core::transform(queues, [&priorities](auto queue) {
            auto& priority = priorities.emplace_back();

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
        for (const auto& ext : device_extensions) device_logger.dlog("{}", ext.extensionName);
        device_logger.dlog("-------------------------------");

        const auto swapchain_available = [&] {
            for (const auto& ext : SWAPCHAIN_EXTENSIONS) {
                if (std::ranges::find_if(device_extensions, [&](const auto& e) {
                        return ext == e.extensionName;
                    }) == std::ranges::cend(device_extensions)) {
                    return false;
                }
            }

            device_logger.ilog("Swapchain supported !");

            return true;
        }();

        const auto raytracing_available = [&] {
            for (const auto& ext : RAYTRACING_EXTENSIONS) {
                if (std::ranges::find_if(device_extensions, [&](const auto& e) {
                        return ext == e.extensionName;
                    }) == std::ranges::cend(device_extensions)) {
                    return false;
                }
            }

            device_logger.ilog("Raytracing supported !");

            return true;
        }();

        const auto extensions = [&] {
            constexpr auto toCZString = [](const auto& v) { return std::data(v); };

            auto e = core::transform(BASE_EXTENSIONS, toCZString);
            if (swapchain_available and info.enable_swapchain)
                core::merge(e, core::transform(SWAPCHAIN_EXTENSIONS, toCZString));
            if (raytracing_available and info.enable_raytracing)
                core::merge(e, core::transform(RAYTRACING_EXTENSIONS, toCZString));

            return e;
        }();

        using Next = void *;
        auto next  = Next { nullptr };

        auto rt_pipeline_feature = VkPhysicalDeviceRayTracingPipelineFeaturesKHR {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR
        };

        auto acceleration_feature = VkPhysicalDeviceAccelerationStructureFeaturesKHR {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR
        };

        if (raytracing_available and info.enable_raytracing) {
            rt_pipeline_feature.pNext = reinterpret_cast<void *>(&acceleration_feature);

            next = reinterpret_cast<void *>(&rt_pipeline_feature);
        }

        const auto create_info = VkDeviceCreateInfo {
            .sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext                   = next,
            .queueCreateInfoCount    = core::as<core::UInt32>(std::size(queue_create_infos)),
            .pQueueCreateInfos       = std::data(queue_create_infos),
            .enabledExtensionCount   = core::as<core::UInt32>(std::size(extensions)),
            .ppEnabledExtensionNames = std::data(extensions),
            .pEnabledFeatures        = &enabled_features
        };

        CHECK_VK_ERROR(vkCreateDevice(*m_physical_device, &create_info, nullptr, &m_device));

        volkLoadDeviceTable(&m_device_table, m_device);

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
                                     .instance         = this->instance() };

        CHECK_VK_ERROR(vmaCreateAllocator(&alloc_create_info, &m_vma_allocator));

        setObjectName(*this,
                      std::format("StormKit:Device ({})", m_physical_device->info().device_name));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Device::~Device() {
        waitIdle();

        if (m_vma_allocator != VK_NULL_HANDLE) [[likely]] {
            vmaDestroyAllocator(m_vma_allocator);
            m_vma_allocator = VK_NULL_HANDLE;
        }

        if (m_device != VK_NULL_HANDLE) [[likely]] {
            m_device_table.vkDestroyDevice(m_device, nullptr);
            m_device = VK_NULL_HANDLE;
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Device::Device(Device&& other) noexcept
        : InstanceObject { std::move(other) },
          m_physical_device { std::exchange(other.m_physical_device, nullptr) },
          m_device { std::exchange(other.m_device, VK_NULL_HANDLE) },
          m_device_table { std::exchange(other.m_device_table, {}) },
          m_raster_queue { std::move(other.m_raster_queue) },
          m_async_transfert_queue { std::move(other.m_async_transfert_queue) },
          m_async_compute_queue { std::move(other.m_async_compute_queue) },
          m_vma_allocator { std::exchange(other.m_vma_allocator, VK_NULL_HANDLE) },
          m_vma_device_table { std::exchange(other.m_vma_device_table, {}) } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::operator=(Device&& other) noexcept -> Device& {
        if (&other == this) [[unlikely]]
            return *this;

        InstanceObject::operator=(std::move(other));
        m_physical_device       = std::exchange(other.m_physical_device, nullptr);
        m_device                = std::exchange(other.m_device, VK_NULL_HANDLE);
        m_device_table          = std::exchange(other.m_device_table, {});
        m_raster_queue          = std::move(other.m_raster_queue);
        m_async_transfert_queue = std::move(other.m_async_transfert_queue);
        m_async_compute_queue   = std::move(other.m_async_compute_queue;
        m_vma_allocator         = std::exchange(other.m_vma_allocator, VK_NULL_HANDLE);
        m_vma_device_table      = std::exchange(other.m_vma_device_table, {});

        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::waitIdle() const noexcept -> void {
        core::expects(m_device != VK_NULL_HANDLE);

        CHECK_VK_ERROR(m_device_table.vkDeviceWaitIdle(m_device));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::waitForFences(std::span<const FenceConstRef> fences,
                               bool wait_all,
                               std::chrono::milliseconds timeout) const noexcept -> void {
        core::expects(m_device != VK_NULL_HANDLE);

        auto vk_fences = std::vector<VkFence> {};
        vk_fences.reserve(core::as<core::RangeExtent>(std::size(fences)));

        std::ranges::transform(fences,
                               std::back_inserter(vk_fences),
                               [](const auto& fence) -> VkFence { return fence.get(); });

        CHECK_VK_ERROR(m_device_table.vkWaitForFences(
            m_device,
            core::as<core::UInt32>(std::size(vk_fences)),
            std::data(vk_fences),
            wait_all,
            std::chrono::duration_cast<std::chrono::nanoseconds>(timeout).count()));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::setObjectName(core::UInt64 object,
                               DebugObjectType type,
                               std::string_view name) const -> void {
        core::expects(m_device != VK_NULL_HANDLE);

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
