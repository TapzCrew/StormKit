// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <vulkan/vulkan_hpp_macros.hpp>

module stormkit.Gpu;

import std;

import stormkit.Core;
import stormkit.Log;

import <stormkit/Log/LogMacro.hpp>;

import vulkan;

namespace stormkit::gpu {
    NAMED_LOGGER(instance_logger, "stormkit.Gpu:core.Instance")

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
    } // namespace

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto scorePhysicalDevice(std::span<const core::CZString> extensions,
                                    const PhysicalDevice& physical_device) -> core::UInt64 {
        if (!physical_device.checkExtensionSupport(extensions)) return 0u;

        auto score = core::UInt64 { 0u };

        const auto& info = physical_device.info();

        if (info.type == PhysicalDeviceType::Discrete_GPU) score += 10000000u;
        else if (info.type == PhysicalDeviceType::Virtual_GPU)
            score += 5000000u;
        else if (info.type == PhysicalDeviceType::Integrated_GPU)
            score += 250000u;

        const auto& capabilities = physical_device.capabilities();

        score += capabilities.limits.max_image_dimension_1D;
        score += capabilities.limits.max_image_dimension_2D;
        score += capabilities.limits.max_image_dimension_3D;
        score += capabilities.limits.max_image_dimension_cube;
        score += capabilities.limits.max_uniform_buffer_range;
        score += info.api_major_version * 10000000u;
        score += info.api_minor_version * 1000000u;
        score += info.api_patch_version * 100u;

        return score;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto debugCallback(vk::DebugUtilsMessageSeverityFlagsEXT severity,
                       vk::DebugUtilsMessageTypeFlagsEXT type,
                       const vk::DebugUtilsMessengerCallbackDataEXT& callback_data,
                       [[maybe_unused]] void *user_data) -> bool {
        auto message = std::format("[{}] {}", vk::to_string(severity), callback_data.pMessage);

        if (checkFlag(severity, vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo))
            instance_logger.ilog(message);
        else if (checkFlag(severity, vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose))
            instance_logger.dlog(message);
        else if (checkFlag(severity, vk::DebugUtilsMessageSeverityFlagBitsEXT::eError))
            instance_logger.elog(message);
        else if (checkFlag(severity, vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning))
            instance_logger.wlog(message);

        return false;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Instance::Instance(std::string app_name) : m_app_name { std::move(app_name) } {
        core::expects(!s_has_instance, "Trying to create a second Instance, aborting...");

        m_vk_context = std::make_unique<vk::raii::Context>();
        VULKAN_HPP_DEFAULT_DISPATCHER.init();

        createInstance();

        retrievePhysicalDevices();

        s_has_instance = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Instance::~Instance() {
        s_has_instance = false;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Instance::Instance(Instance&& other) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto Instance::operator=(Instance&& other) noexcept -> Instance& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto Instance::pickPhysicalDevice() const noexcept
        -> ErrorExpected<core::NakedRef<const PhysicalDevice>> {
        auto ranked_devices = std::multimap<core::UInt64, core::NakedRef<const PhysicalDevice>> {};

        for (const auto& physical_device : physicalDevices()) {
            const auto score = scorePhysicalDevice(INSTANCE_BASE_EXTENSIONS, physical_device);

            ranked_devices.emplace(score, physical_device);
        }

        if (std::ranges::empty(ranked_devices))
            return std::unexpected { Error::No_Physical_Device_Found };

        return ranked_devices.rbegin()->second.get();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    // auto InstanceObjectBuilder::pickPhysicalDevice(const WindowSurface& surface) noexcept
    // -> Expected<PhysicalDeviceConstRef> {
    // auto ranked_devices = std::multimap<core::UInt64, PhysicalDeviceConstRef> {};

    // const auto has_present_support = [&] {
    // auto support = false;

    // for (const auto& physical_device : instance().physicalDevices()) {
    // auto i         = 0;
    // auto supported = false;
    // for (const auto queue_family : physical_device.queueFamilies()) {
    // auto supported = surface.isPhysicalDeviceSupported(physical_device, i++);
    // if (supported) break;
    // }

    // if (!supported) continue;

    // support = true;

    // const auto extensions = core::concat(INSTANCE_BASE_EXTENSIONS, SURFACE_EXTENSIONS);
    // const auto score      = details::scorePhysicalDevice(extensions, physical_device);

    // ranked_devices.emplace(score, physical_device);
    // }

    // return support;
    // }();

    // if (std::ranges::empty(ranked_devices))
    // return std::unexpected(Error::No_Physical_Device_Found);

    // if (!has_present_support)
    // return std::unexpected(Error::No_Present_Compatible_Physical_Device_Found);

    // return std::cref(ranked_devices.rbegin()->second.get());
    // }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Instance::createInstance() noexcept -> void {
        m_extensions = m_vk_context->enumerateInstanceExtensionProperties() |
                       std::views::transform([](const auto& extension) {
                           return std::string { extension.extensionName };
                       }) |
                       std::ranges::to<std::vector>();

        instance_logger.dlog("Instance extensions -----------");
        for (const auto& str : m_extensions) instance_logger.dlog("	{}", str);
        instance_logger.dlog("-------------------------------");

        const auto [validation_layers, enable_validation] = [this] {
            const auto enable_validation = checkValidationLayerSupport(ENABLE_VALIDATION);
            if (enable_validation) {
                instance_logger.dlog("Validation layers enabled");
                instance_logger.dlog("enabling layers: -----------");
                for (const auto& str : VALIDATION_LAYERS) instance_logger.dlog("	{}", str);
                instance_logger.dlog("-------------------------------");

                return std::pair { VALIDATION_LAYERS | std::ranges::to<std::vector>(), true };
            }

            return std::pair { std::vector<core::CZString> {}, false };
        }();

        const auto instance_extensions = [enable_validation = enable_validation] {
            auto e = core::concat(INSTANCE_BASE_EXTENSIONS, SURFACE_EXTENSIONS);

            if (enable_validation) core::merge(e, std::array { "VK_EXT_debug_utils" });

            return e;
        }();

        core::ensures(checkExtensionSupport(instance_extensions));

        const auto app_info = vk::ApplicationInfo {}
                                  .setPApplicationName(std::data(m_app_name))
                                  .setPEngineName("StormKit")
                                  .setEngineVersion(STORMKIT_VK_VERSION)
                                  .setApiVersion(vkMakeVersion<core::Int32>(1, 0, 0));

        const auto create_info = vk::InstanceCreateInfo {}
                                     .setPApplicationInfo(&app_info)
                                     .setPEnabledExtensionNames(instance_extensions)
                                     .setPEnabledLayerNames(validation_layers);

        m_vk_instance = std::make_unique<vk::raii::Instance>(*m_vk_context, create_info);
        vk::defaultDispatchLoaderDynamic.init(*vkHandle());

        if (enable_validation) createDebugReportCallback();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Instance::createDebugReportCallback() noexcept -> void {
        core::expects(m_vk_instance != nullptr);

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

        m_vk_messenger =
            std::make_unique<vk::raii::DebugUtilsMessengerEXT>(*m_vk_instance, create_info);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Instance::retrievePhysicalDevices() noexcept -> void {
        core::expects(m_vk_instance != nullptr);

        m_physical_devices = m_vk_instance->enumeratePhysicalDevices() |
                             std::views::transform([this](auto&& physical_device) {
                                 return PhysicalDevice { std::move(physical_device), *this };
                             }) |
                             std::ranges::to<std::vector>();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Instance::checkValidationLayerSupport(bool enable_validation) noexcept -> bool {
        core::expects(m_vk_context != nullptr);

        if (!enable_validation) return enable_validation;

        const auto layers = m_vk_context->enumerateInstanceLayerProperties();
        for (const auto& layer_name : std::as_const(VALIDATION_LAYERS)) {
            auto layer_found = false;

            for (const auto& layer_properties : layers) {
                if (std::strcmp(layer_name, layer_properties.layerName) == 0) {
                    layer_found = true;
                    break;
                }
            }

            if (!layer_found) {
                instance_logger.dlog("Failed to find validation layers, disabling...");
                return false;
            }
        }

        return enable_validation;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Instance::checkExtensionSupport(std::string_view extension) const noexcept -> bool {
        return std::ranges::find(m_extensions, extension) != std::ranges::cend(m_extensions);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto
        Instance::checkExtensionSupport(std::span<const std::string_view> extensions) const noexcept
        -> bool {
        auto required_extensions = core::HashSet<std::string_view> { std::ranges::begin(extensions),
                                                                     std::ranges::end(extensions) };

        for (const auto& extension : m_extensions) required_extensions.erase(extension);

        return required_extensions.empty();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Instance::checkExtensionSupport(std::span<const core::CZString> extensions) const noexcept
        -> bool {
        const auto ext = extensions | std::views::transform([](const auto& extension) {
                             return std::string_view { extension };
                         }) |
                         std::ranges::to<std::vector>();
        return checkExtensionSupport(ext);
    }
} // namespace stormkit::gpu
