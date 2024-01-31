module;

#include <vulkan/vulkan_hpp_macros.hpp>
#include <stormkit/Log/LogMacro.hpp>

module stormkit.Engine;

import std;

import stormkit.Core;
import stormkit.Log;
import stormkit.Wsi;

import :Renderer;

using namespace std::literals;

namespace stormkit::engine {
    LOGGER("stormkit.Renderer")

    namespace {
    } // namespace

    /////////////////////////////////////
    /////////////////////////////////////
    auto Renderer::doInitInstance(std::string_view application_name) noexcept
        -> VulkanExpected<void> {
        ilog("Initializing Renderer");

        VULKAN_HPP_DEFAULT_DISPATCHER.init();

        m_context = vk::raii::Context {};

        const auto exts       = m_context->enumerateInstanceExtensionProperties();
        const auto extensions = exts | std::views::transform([](auto&& extension) noexcept {
                                    return std::string { extension.extensionName };
                                }) |
                                std::ranges::to<std::vector>();

        dlog("Instance extensions -----------");
        for (auto&& str : extensions) dlog("	{}", str);
        dlog("-------------------------------");

        const auto validation_layers = [this]() noexcept {
            auto output = std::vector<core::CZString> {};
            m_validation_layers_enabled =
                checkValidationLayerSupport(m_context, m_validation_layers_enabled);
            if (m_validation_layers_enabled) {
                dlog("Validation layers enabled");
                dlog("enabling layers: -----------");
                for (const auto& str : VALIDATION_LAYERS) dlog("	{}", str);
                dlog("-------------------------------");

                output = VALIDATION_LAYERS | std::ranges::to<std::vector>();
            }

            return output;
        }();

        const auto instance_extensions = [this]() noexcept {
            auto e = core::concat(INSTANCE_BASE_EXTENSIONS, SURFACE_EXTENSIONS);

            if (m_validation_layers_enabled) core::merge(e, std::array { "EXT_debug_utils" });

            return e;
        }();

        core::ensures(checkExtensionSupport(extensions, instance_extensions));

        constexpr auto ENGINE_NAME = "StormKit";

        const auto app_info = vk::ApplicationInfo {}
                                  .setPApplicationName(std::data(application_name))
                                  .setPEngineName(ENGINE_NAME)
                                  .setEngineVersion(STORMKIT_VERSION)
                                  .setApiVersion(vkMakeVersion<core::Int32>(1, 0, 0));

        const auto create_info = vk::InstanceCreateInfo {}
                                     .setPApplicationInfo(&app_info)
                                     .setPEnabledExtensionNames(instance_extensions)
                                     .setPEnabledLayerNames(validation_layers);

        try {
            m_instance = vk::raii::Instance { m_context, create_info };
        } catch (const vk::SystemError& err) {
            return std::unexpected { core::narrow<vk::Result>(err.code().value()) };
        }

        VULKAN_HPP_DEFAULT_DISPATCHER.init(*m_instance.get());

        return {};
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Renderer::doInitDebugReportCallback() noexcept -> VulkanExpected<void> {
        if (!m_validation_layers_enabled) return {};

        constexpr auto severity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                                  vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                                  vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;

        constexpr auto type = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                              vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                              vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;

        const auto create_info =
            vk::DebugUtilsMessengerCreateInfoEXT {}
                .setMessageSeverity(severity)
                .setMessageType(type)
                .setPfnUserCallback(
                    std::bit_cast<decltype(vk::DebugUtilsMessengerCreateInfoEXT::pfnUserCallback)>(
                        &debugCallback));

        return vkCreate<vk::raii::DebugUtilsMessengerEXT>(m_instance, create_info)
            .transform(core::monadic::set(m_messenger));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Renderer::doInitDevice() noexcept -> VulkanExpected<void> {
        const auto physical_devices = m_instance->enumeratePhysicalDevices();

        m_physical_device = pickPhysicalDevice(physical_devices)
                                .or_else(core::expectsWithMessage<vk::raii::PhysicalDevice>(
                                    "No compatible physical device found !"))
                                .value();

        const auto& properties = m_physical_device->getProperties();

        ilog("Using PhysicalDevice {} ({:#06x})",
             std::string_view { properties.deviceName },
             properties.deviceID);

        const auto extensions = m_physical_device->enumerateDeviceExtensionProperties() |
                                std::views::transform([](auto&& extention) noexcept {
                                    return std::string_view { extention.extensionName };
                                }) |
                                std::ranges::to<std::vector>();
        dlog("Device extensions -----------");
        for (auto&& str : extensions) dlog("	{}", str);
        dlog("-------------------------------");

        const auto support_raytracing = core::findFirstNotOf(extensions, RAYTRACING_EXTENSIONS) ==
                                        std::ranges::cend(RAYTRACING_EXTENSIONS);

        ilog("Raytracing {}", support_raytracing ? "supported !" : "not supported !");

        const auto& queue_families = m_physical_device->getQueueFamilyProperties();

        struct QueueFamily {
            core::UInt32   id    = 0u;
            core::UInt32   count = 0u;
            vk::QueueFlags flags = {};
        };

        const auto raster_queue = [&queue_families]() noexcept
            -> std::optional<QueueFamily> {
            const auto it =
                std::ranges::find_if(queue_families, findQueue<vk::QueueFlagBits::eGraphics>());
            if (it == std::ranges::cend(queue_families)) return std::nullopt;

            return QueueFamily { .id = core::as<core::UInt32>(
                                     std::distance(std::ranges::cbegin(queue_families), it)),
                                 .count = it->queueCount,
                                 .flags = it->queueFlags };
        }()
                   .or_else(core::expectsWithMessage<QueueFamily>("No raster queue found !"))
                   .value();

        const auto compute_queue = [&queue_families]() noexcept -> std::optional<QueueFamily> {
            const auto it = std::ranges::find_if(
                queue_families,
                findQueue<vk::QueueFlagBits::eCompute, vk::QueueFlagBits::eGraphics>());
            if (it == std::ranges::cend(queue_families)) return {};

            return QueueFamily { .id = core::as<core::UInt32>(
                                     std::distance(std::ranges::cbegin(queue_families), it)),
                                 .count = it->queueCount,
                                 .flags = it->queueFlags };
        }();

        const auto transfert_queue = [&queue_families]() noexcept -> std::optional<QueueFamily> {
            const auto it = std::ranges::find_if(queue_families,
                                                 findQueue<vk::QueueFlagBits::eTransfer,
                                                           vk::QueueFlagBits::eCompute,
                                                           vk::QueueFlagBits::eGraphics>());
            if (it == std::ranges::cend(queue_families)) return {};

            return QueueFamily { .id = core::as<core::UInt32>(
                                     std::distance(std::ranges::cbegin(queue_families), it)),
                                 .count = it->queueCount,
                                 .flags = it->queueFlags };
        }();

        const auto queues = [&raster_queue, &compute_queue, &transfert_queue]() noexcept {
            auto queues = std::vector<core::NakedRef<const QueueFamily>> {};
            queues.reserve(3);

            queues.emplace_back(raster_queue);
            if (compute_queue) queues.emplace_back(*compute_queue);
            if (transfert_queue) queues.emplace_back(*transfert_queue);

            return queues;
        }();

        auto priorities = std::vector<std::vector<float>> {};
        priorities.reserve(std::size(queues));

        const auto queue_create_infos = core::transform(queues, [&priorities](auto queue) {
            auto& priority = priorities.emplace_back();

            priority.resize(queue->count, 1.f);

            return vk::DeviceQueueCreateInfo {}.setQueueFamilyIndex(queue->id).setQueuePriorities(
                priority);
        });

        const auto& features         = m_physical_device->getFeatures();
        const auto  enabled_features = vk::PhysicalDeviceFeatures {}
                                          .setSampleRateShading(features.sampleRateShading)
                                          .setMultiDrawIndirect(features.multiDrawIndirect)
                                          .setFillModeNonSolid(features.fillModeNonSolid)
                                          .setSamplerAnisotropy(features.samplerAnisotropy);

        const auto enabled_extensions = [support_raytracing]() noexcept {
            auto extensions = core::concat(BASE_EXTENSIONS, SWAPCHAIN_EXTENSIONS);
            if (support_raytracing) core::merge(extensions, RAYTRACING_EXTENSIONS);
            return extensions | std::views::transform(core::toCZString) |
                   std::ranges::to<std::vector>();
        }();

        const auto acceleration_feature = vk::PhysicalDeviceAccelerationStructureFeaturesKHR {};
        const auto rt_pipeline_feature =
            vk::PhysicalDeviceRayTracingPipelineFeaturesKHR {}.setPNext(
                std::bit_cast<void *>(&acceleration_feature));

        const auto next = [&rt_pipeline_feature, support_raytracing]() noexcept -> void * {
            if (support_raytracing) return std::bit_cast<void *>(&rt_pipeline_feature);

            return nullptr;
        }();

        const auto create_info = vk::DeviceCreateInfo {}
                                     .setPNext(next)
                                     .setQueueCreateInfos(queue_create_infos)
                                     .setPEnabledExtensionNames(enabled_extensions)
                                     .setPEnabledFeatures(&enabled_features);

        try {
            m_device = vk::raii::Device { m_physical_device, create_info };
        } catch (const vk::SystemError& err) {
            return std::unexpected { core::narrow<vk::Result>(err.code().value()) };
        }

        VULKAN_HPP_DEFAULT_DISPATCHER.init(*m_device.get());
        dlog("enabled device extensions: {}", enabled_extensions);

        m_raster_queue = m_device->getQueue(raster_queue.id, 0);

        if (transfert_queue) m_transfert_queue = m_device->getQueue(transfert_queue->id, 0);

        if (compute_queue) m_compute_queue = m_device->getQueue(compute_queue->id, 0);

        return {};
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Renderer::threadLoop(std::stop_token token) noexcept -> void {
        const auto create_info = vk::CommandPoolCreateInfo {}.setFlags(
            vk::CommandPoolCreateFlagBits::eTransient |
            vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
        auto command_pool = vk::raii::CommandPool { m_device, create_info };

        const auto& images     = m_render_surface->images();
        auto&& command_buffers = createCommandBuffers(m_device, command_pool, std::size(images));

        for (const auto i : core::range(std::size(images))) {
            const auto& image          = images[i];
            auto      & command_buffer = command_buffers[i];

            command_buffer.begin(vk::CommandBufferBeginInfo {}.setFlags(
                vk::CommandBufferUsageFlagBits::eOneTimeSubmit));

            transitionImageLayout(command_buffer,
                                  image,
                                  vk::ImageLayout::eUndefined,
                                  vk::ImageLayout::ePresentSrcKHR);

            command_buffer.end();
        }

        auto fence = vk::raii::Fence { m_device,
                                       vk::FenceCreateInfo {}.setFlags(
                                           vk::FenceCreateFlagBits::eSignaled) };

        submit(m_raster_queue, command_buffers, {}, {}, *fence);

//        fence.wait();

        for (;;) {
            if (token.stop_requested()) return;

            m_render_surface->acquireNextFrame()
                .transform([this](auto&& frame) { m_render_surface->present(frame); })
                .transform_error([](auto&& error) {
                    elog("Failed to acquire frame, reason: {}", error);
                    return std::forward<decltype(error)>(error);
                });
        }

        m_device->waitIdle();
    }
} // namespace stormkit::engine
