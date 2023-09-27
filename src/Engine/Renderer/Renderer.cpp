module;

#include <vulkan/vulkan_hpp_macros.hpp>

module stormkit.Engine;

import std;
import vulkan;

import stormkit.Core;
import stormkit.Log;

import <stormkit/Log/LogMacro.hpp>;

import :Renderer;
import :Renderer.Vulkan.Utils;

using namespace std::literals;

namespace stormkit::engine {
    LOGGER("stormkit.Renderer")

    namespace {
        constexpr auto VALIDATION_LAYERS = std::array {
            "VK_LAYER_KHRONOS_validation",
#ifdef STORMKIT_OS_LINUX
            "VK_LAYER_MESA_overlay",
#endif
        };
        constexpr auto VALIDATION_FEATURES =
            std::array { vk::ValidationFeatureEnableEXT::eBestPractices,
                         vk::ValidationFeatureEnableEXT::eGpuAssisted };

        constexpr auto STORMKIT_VK_VERSION =
            vkMakeVersion<core::Int32>(core::STORMKIT_MAJOR_VERSION,
                                       core::STORMKIT_MINOR_VERSION,
                                       core::STORMKIT_PATCH_VERSION);

        constexpr auto RAYTRACING_EXTENSIONS =
            std::array { "VK_KHR_ray_tracing_pipeline"sv,  "VK_KHR_acceleration_structure"sv,
                         "VK_KHR_buffer_device_address"sv, "VK_KHR_deferred_host_operations"sv,
                         "VK_EXT_descriptor_indexing"sv,   "VK_KHR_spirv_1_4"sv,
                         "VK_KHR_shader_float_controls"sv };

        constexpr auto BASE_EXTENSIONS = std::array { "VK_KHR_maintenance3"sv };

        constexpr auto SWAPCHAIN_EXTENSIONS = std::array { "VK_KHR_swapchain"sv };

        /////////////////////////////////////
        /////////////////////////////////////
        auto debugCallback(vk::DebugUtilsMessageSeverityFlagsEXT severity,
                           vk::DebugUtilsMessageTypeFlagsEXT type,
                           const vk::DebugUtilsMessengerCallbackDataEXT& callback_data,
                           [[maybe_unused]] void *user_data) -> bool {
            auto message = std::format("[{}] {}", vk::to_string(severity), callback_data.pMessage);

            if (checkFlag(severity, vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo)) ilog(message);
            else if (checkFlag(severity, vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose))
                dlog(message);
            else if (checkFlag(severity, vk::DebugUtilsMessageSeverityFlagBitsEXT::eError))
                elog(message);
            else if (checkFlag(severity, vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning))
                wlog(message);

            return false;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        auto vendorNameByID(core::UInt64 ID) -> std::string_view {
            switch (ID) {
                case 0x1002: return "AMD";
                case 0x1010: return "ImgTex";
                case 0x10DE: return "NVidia";
                case 0x8086: return "core::Intel";
                case 0x5143: return "Qualcomm";
                case 0x13B5: return "ARM";
            }

            return "Unknown";
        }

        /////////////////////////////////////
        /////////////////////////////////////
        auto checkValidationLayerSupport(const vk::raii::Context& vk_context,
                                         bool vk_validation_layers_enabled) noexcept -> bool {
            if (!vk_validation_layers_enabled) return vk_validation_layers_enabled;

            const auto layers = vk_context.enumerateInstanceLayerProperties();
            for (const auto& layer_name : std::as_const(VALIDATION_LAYERS)) {
                auto layer_found = false;

                for (const auto& layer_properties : layers) {
                    if (std::strcmp(layer_name, layer_properties.layerName) == 0) {
                        layer_found = true;
                        break;
                    }
                }

                if (!layer_found) {
                    dlog("Failed to find validation layers, disabling...");
                    return false;
                }
            }

            return vk_validation_layers_enabled;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        auto checkExtensionSupport(std::span<const std::string> supported_extensions,
                                   std::span<const std::string_view> extensions) noexcept -> bool {
            auto required_extensions =
                core::HashSet<std::string_view> { std::ranges::begin(extensions),
                                                  std::ranges::end(extensions) };

            for (const auto& extension : supported_extensions) required_extensions.erase(extension);

            return required_extensions.empty();
        }

        /////////////////////////////////////
        /////////////////////////////////////
        auto checkExtensionSupport(std::span<const std::string> supported_extensions,
                                   std::span<const core::CZString> extensions) noexcept -> bool {
            const auto ext = extensions | std::views::transform([](const auto& extension) noexcept {
                                 return std::string_view { extension };
                             }) |
                             std::ranges::to<std::vector>();
            return checkExtensionSupport(supported_extensions, ext);
        }

        /////////////////////////////////////
        /////////////////////////////////////
        auto scorePhysicalDevice(const vk::raii::PhysicalDevice& physical_device) -> core::UInt64 {
            const auto extensions = physical_device.enumerateDeviceExtensionProperties() |
                                    std::views::transform([](auto&& extention) noexcept {
                                        return std::string_view { extention.extensionName };
                                    }) |
                                    std::ranges::to<std::vector>();

            core::expects(core::findFirstNotOf(extensions, BASE_EXTENSIONS) ==
                              std::ranges::cend(BASE_EXTENSIONS),
                          "Base required extensions not supported GPU");
            core::expects(core::findFirstNotOf(extensions, SWAPCHAIN_EXTENSIONS) ==
                              std::ranges::cend(SWAPCHAIN_EXTENSIONS),
                          "Swapchain required extensions not supported by this GPU");

            const auto support_raytracing =
                core::findFirstNotOf(extensions, RAYTRACING_EXTENSIONS) ==
                std::ranges::cend(RAYTRACING_EXTENSIONS);

            auto score = core::UInt64 { 0u };

            const auto& properties = physical_device.getProperties();

            if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) score += 10000000u;
            else if (properties.deviceType == vk::PhysicalDeviceType::eVirtualGpu)
                score += 5000000u;
            else if (properties.deviceType == vk::PhysicalDeviceType::eIntegratedGpu)
                score += 250000u;

            score += properties.limits.maxImageDimension1D;
            score += properties.limits.maxImageDimension2D;
            score += properties.limits.maxImageDimension3D;
            score += properties.limits.maxImageDimensionCube;
            score += properties.limits.maxUniformBufferRange;
            score += vkVersionMajor(properties.apiVersion) * 10000000u;
            score += vkVersionMinor(properties.apiVersion) * 10000u;
            score += vkVersionPatch(properties.apiVersion) * 100u;

            if (support_raytracing) score += 10000000u;

            return score;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        auto pickPhysicalDevice(std::span<const vk::raii::PhysicalDevice> physical_devices) noexcept
            -> std::optional<vk::raii::PhysicalDevice> {
            auto ranked_devices =
                std::multimap<core::UInt64, core::NakedRef<const vk::raii::PhysicalDevice>> {};

            for (const auto& physical_device : physical_devices) {
                const auto& properties = physical_device.getProperties();

                dlog("Scoring for {}\n"
                     "    device id:      {:#06x}\n"
                     "    vendor id:      {}\n"
                     "    vendor name:    {}\n"
                     "    api version:    {}.{}.{}\n"
                     "    driver version: {}.{}.{}\n"
                     "    type:           {}",
                     std::string_view { properties.deviceName },
                     properties.deviceID,
                     properties.vendorID,
                     vendorNameByID(properties.vendorID),
                     vkVersionMajor(properties.apiVersion),
                     vkVersionMinor(properties.apiVersion),
                     vkVersionPatch(properties.apiVersion),
                     vkVersionMajor(properties.driverVersion),
                     vkVersionMinor(properties.driverVersion),
                     vkVersionPatch(properties.driverVersion),
                     vk::to_string(properties.deviceType));

                const auto score = scorePhysicalDevice(physical_device);

                dlog("Score is {}", score);

                ranked_devices.emplace(score, physical_device);
            }

            if (std::ranges::empty(ranked_devices)) return std::nullopt;

            return ranked_devices.rbegin()->second.get();
        }

        /////////////////////////////////////
        /////////////////////////////////////
        template<vk::QueueFlagBits flag, vk::QueueFlagBits... no_flag>
        constexpr auto findQueue() noexcept {
            return [](auto&& family) noexcept {
                return checkFlag(family.queueFlags, flag) and
                       (not checkFlag(family.queueFlags, no_flag) and ...);
            };
        }
    } // namespace

    /////////////////////////////////////
    /////////////////////////////////////
    auto Renderer::doInitInstance(std::string_view application_name) noexcept
        -> VulkanExpected<void> {
        ilog("Initializing Renderer");

        VULKAN_HPP_DEFAULT_DISPATCHER.init();

        m_vk_context = vk::raii::Context {};

        const auto exts       = m_vk_context->enumerateInstanceExtensionProperties();
        const auto extensions = exts | std::views::transform([](auto&& extension) noexcept {
                                    return std::string { extension.extensionName };
                                }) |
                                std::ranges::to<std::vector>();

        dlog("Instance extensions -----------");
        for (auto&& str : extensions) dlog("	{}", str);
        dlog("-------------------------------");

        const auto validation_layers = [this]() noexcept {
            auto output = std::vector<core::CZString> {};
            m_vk_validation_layers_enabled =
                checkValidationLayerSupport(m_vk_context, m_vk_validation_layers_enabled);
            if (m_vk_validation_layers_enabled) {
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

            if (m_vk_validation_layers_enabled) core::merge(e, std::array { "VK_EXT_debug_utils" });

            return e;
        }();

        core::ensures(checkExtensionSupport(extensions, instance_extensions));

        constexpr auto ENGINE_NAME = "StormKit";

        const auto app_info = vk::ApplicationInfo {}
                                  .setPApplicationName(std::data(application_name))
                                  .setPEngineName(ENGINE_NAME)
                                  .setEngineVersion(STORMKIT_VK_VERSION)
                                  .setApiVersion(vkMakeVersion<core::Int32>(1, 0, 0));

        const auto create_info = vk::InstanceCreateInfo {}
                                     .setPApplicationInfo(&app_info)
                                     .setPEnabledExtensionNames(instance_extensions)
                                     .setPEnabledLayerNames(validation_layers);

        try {
            m_vk_instance = vk::raii::Instance { m_vk_context, create_info };
        } catch (const vk::SystemError& err) {
            return std::unexpected { core::as<vk::Result>(err.code().value()) };
        }

        VULKAN_HPP_DEFAULT_DISPATCHER.init(*m_vk_instance.get());

        return {};
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Renderer::doInitDebugReportCallback() noexcept -> VulkanExpected<void> {
        if (!m_vk_validation_layers_enabled) return {};

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

        try {
            m_vk_messenger = vk::raii::DebugUtilsMessengerEXT { m_vk_instance, create_info };
        } catch (const vk::SystemError& err) {
            return std::unexpected { core::as<vk::Result>(err.code().value()) };
        }

        return {};
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Renderer::doInitDevice() noexcept -> VulkanExpected<void> {
        const auto vk_physical_devices = m_vk_instance->enumeratePhysicalDevices();

        auto vk_physical_device = pickPhysicalDevice(vk_physical_devices)
                                      .or_else(core::expectsWithMessage<vk::raii::PhysicalDevice>(
                                          "No compatible physical device found !"))
                                      .value();

        const auto& properties = vk_physical_device.getProperties();

        ilog("Using PhysicalDevice {} ({:#06x})",
             std::string_view { properties.deviceName },
             properties.deviceID);

        const auto extensions = vk_physical_device.enumerateDeviceExtensionProperties() |
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

        const auto& queue_families = vk_physical_device.getQueueFamilyProperties();

        struct QueueFamily {
            core::UInt32 id      = 0u;
            core::UInt32 count   = 0u;
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

        const auto& features        = vk_physical_device.getFeatures();
        const auto enabled_features = vk::PhysicalDeviceFeatures {}
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
            m_vk_device = vk::raii::Device { vk_physical_device, create_info };
        } catch (const vk::SystemError& err) {
            return std::unexpected { core::as<vk::Result>(err.code().value()) };
        }

        VULKAN_HPP_DEFAULT_DISPATCHER.init(*m_vk_device.get());
        dlog("enabled device extensions: {}", enabled_extensions);

        return {};
    }
} // namespace stormkit::engine
