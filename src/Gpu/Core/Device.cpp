// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <vulkan/vulkan_hpp_macros.hpp>

module stormkit.Gpu;

import std;

import stormkit.Core;
import stormkit.Log;

#include <stormkit/Log/LogMacro.hpp>
#include <stormkit/Gpu/Core/VulkanMacro.hpp>

import :Core.Device;
import :Core.Sync;
import :Vulkan;

using namespace std::literals;

namespace {
    constexpr auto RAYTRACING_EXTENSIONS =
        std::array { "VK_KHR_ray_tracing_pipeline"sv,  "VK_KHR_acceleration_structure"sv,
                     "VK_KHR_buffer_device_address"sv, "VK_KHR_deferred_host_operations"sv,
                     "VK_EXT_descriptor_indexing"sv,   "VK_KHR_spirv_1_4"sv,
                     "VK_KHR_shader_float_controls"sv };

    constexpr auto BASE_EXTENSIONS = std::array { "VK_KHR_maintenance3"sv };

    constexpr auto SWAPCHAIN_EXTENSIONS = std::array { "VK_KHR_swapchain"sv };

} // namespace

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
    Device::Device(const PhysicalDevice& physical_device,
                   const Instance      & instance,
                   const Info          & info,
                   Tag)
        : InstanceObject { instance }, m_physical_device { &physical_device } {
        const auto& queue_families = m_physical_device->queueFamilies();

        struct Queue_ {
            std::optional<core::UInt32> id    = std::nullopt;
            core::UInt32                count = 0u;
            core::Byte                  padding[3];
            QueueFlag                   flags = QueueFlag {};
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

            return vk::DeviceQueueCreateInfo {}
                .setQueueFamilyIndex(queue->id.value())
                .setQueuePriorities(priority);
        });

        const auto& capabilities = m_physical_device->capabilities();
        const auto  enabled_features =
            vk::PhysicalDeviceFeatures {}
                .setSampleRateShading(capabilities.features.sampler_rate_shading)
                .setMultiDrawIndirect(capabilities.features.multi_draw_indirect)
                .setFillModeNonSolid(capabilities.features.fill_Mode_non_solid)
                .setSamplerAnisotropy(capabilities.features.sampler_anisotropy);

        const auto device_extensions =
            m_physical_device->vkHandle().enumerateDeviceExtensionProperties();

        device_logger.dlog("Device extensions -----------");
        for (const auto& ext : device_extensions)
            device_logger.dlog("{}", std::string_view { ext.extensionName });
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

        const auto acceleration_feature = vk::PhysicalDeviceAccelerationStructureFeaturesKHR {};
        const auto rt_pipeline_feature =
            vk::PhysicalDeviceRayTracingPipelineFeaturesKHR {}.setPNext(
                std::bit_cast<void *>(&acceleration_feature));

        const auto next = [&]() -> void * {
            if (raytracing_available and info.enable_raytracing)
                return std::bit_cast<void *>(&rt_pipeline_feature);
            return nullptr;
        }();

        const auto create_info = vk::DeviceCreateInfo {}
                                     .setPNext(next)
                                     .setQueueCreateInfos(queue_create_infos)
                                     .setPEnabledExtensionNames(extensions)
                                     .setPEnabledFeatures(&enabled_features);

        vkCreate<vk::raii::Device>(m_physical_device->vkHandle(), create_info)
            .transform(core::monadic::set(m_vk_device))
            .transform([this]() noexcept -> VulkanExpected<void> {
                VULKAN_HPP_DEFAULT_DISPATCHER.init(*vkHandle());

                try {
                    m_vma_function_table =
                        vma::functionsFromDispatcher(this->instance().vkHandle().getDispatcher(),
                                                     vkHandle().getDispatcher());
                } catch (const vk::SystemError& err) {
                    return std::unexpected { core::narrow<vk::Result>(err.code().value()) };
                }

                const auto alloc_create_info =
                    vma::AllocatorCreateInfo {}
                        .setInstance(*(this->instance().vkHandle()))
                        .setPhysicalDevice(*m_physical_device->vkHandle())
                        .setDevice(*vkHandle())
                        .setPVulkanFunctions(&vmaFunctionTable());

                try {
                    m_vma_allocator = vma::createAllocatorUnique(alloc_create_info);
                } catch (const vk::SystemError& err) {
                    return std::unexpected { core::narrow<vk::Result>(err.code().value()) };
                }

                return {};
            })
            .transform_error(core::monadic::map(core::monadic::as<Result>(), core::throwError()));

        setObjectName(*this,
                      std::format("StormKit:Device ({})", m_physical_device->info().device_name));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::waitForFences(std::span<const core::NakedRef<const Fence>> fences,
                               bool                                         wait_all,
                               const std::chrono::milliseconds            & timeout) const noexcept
        -> Expected<Result> {
        const auto vk_fences =
            fences | std::views::transform(toVkHandle()) | std::ranges::to<std::vector>();

        return vkCall(*m_vk_device,
                      &vk::raii::Device::waitForFences,
                      vk_fences,
                      wait_all,
                      std::chrono::duration_cast<std::chrono::nanoseconds>(timeout).count())
            .transform(core::monadic::as<Result>())
            .transform_error(core::monadic::as<Result>());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::resetFences(std::span<const core::NakedRef<const Fence>> fences) const noexcept
        -> Expected<void> {
        const auto vk_fences =
            fences | std::views::transform(toVkHandle()) | std::ranges::to<std::vector>();

        return vkCall(*m_vk_device, &vk::raii::Device::resetFences, vk_fences)
            .transform_error(core::monadic::as<Result>());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Device::setObjectName(core::UInt64     object,
                               DebugObjectType  type,
                               std::string_view name) const -> Expected<void> {
        if (!vkHandle().getDispatcher()->vkSetDebugUtilsObjectNameEXT) return {};

        const auto info = vk::DebugUtilsObjectNameInfoEXT {}
                              .setObjectType(core::narrow<vk::ObjectType>(type))
                              .setObjectHandle(object)
                              .setPObjectName(std::data(name));

        return vkCall(*m_vk_device, &vk::raii::Device::setDebugUtilsObjectNameEXT, info)
            .transform_error(core::monadic::as<Result>());
    }
} // namespace stormkit::gpu
