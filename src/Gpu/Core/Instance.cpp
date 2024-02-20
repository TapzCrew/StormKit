// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <vulkan/vulkan_hpp_macros.hpp>

module stormkit.Gpu;

import std;

import stormkit.Core;
import stormkit.Log;

#include <stormkit/Core/PlatformMacro.hpp>
#include <stormkit/Log/LogMacro.hpp>

import stormkit.Gpu.Vulkan;

namespace stormkit::gpu {
    LOGGER("stormkit.Gpu")

    namespace {
        constexpr auto VALIDATION_LAYERS = std::array {
            "VK_LAYER_KHRONOS_validation",
#ifdef STORMKIT_OS_LINUX
            "VK_LAYER_MESA_overlay",
#endif
        };

        [[maybe_unused]] constexpr auto VALIDATION_FEATURES =
            std::array { vk::ValidationFeatureEnableEXT::eBestPractices,
                         vk::ValidationFeatureEnableEXT::eGpuAssisted };

        constexpr auto STORMKIT_VK_VERSION =
            vkMakeVersion<core::Int32>(core::STORMKIT_MAJOR_VERSION,
                                       core::STORMKIT_MINOR_VERSION,
                                       core::STORMKIT_PATCH_VERSION);

        constexpr auto BASE_EXTENSIONS = std::array { "VK_KHR_get_physical_device_properties2" };

        constexpr auto SURFACE_EXTENSIONS = std::array {
            "VK_KHR_surface",
        };

        constexpr auto WSI_SURFACE_EXTENSIONS = std::array {
#ifdef STORMKIT_OS_WINDOWS
            "VK_KHR_win32_surface"
#elif defined(STORMKIT_OS_LINUX)
            "VK_KHR_xcb_surface",
            "VK_KHR_wayland_surface"
#elif defined(STORMKIT_OS_MACOS)
            "VK_MVK_MACOS_surface"
#elif defined(STORMKIT_OS_IOS)
            "VK_MVK_IOS_surface"
#endif
        };

        /////////////////////////////////////
        /////////////////////////////////////
        auto debugCallback(vk::DebugUtilsMessageSeverityFlagsEXT         severity,
                           vk::DebugUtilsMessageTypeFlagsEXT             type,
                           const vk::DebugUtilsMessengerCallbackDataEXT& callback_data,
                           [[maybe_unused]] void*                        user_data) -> bool {
            auto message = std::format("{}", callback_data.pMessage);

            if (checkFlag(severity, vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo))
                ilog("{}", message);
            else if (checkFlag(severity, vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose))
                dlog("{}", message);
            else if (checkFlag(severity, vk::DebugUtilsMessageSeverityFlagBitsEXT::eError))
                elog("{}", message);
            else if (checkFlag(severity, vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning))
                wlog("{}", message);

            return false;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        auto checkValidationLayerSupport(const vk::raii::Context& context,
                                         bool validation_layers_enabled) noexcept -> bool {
            if (!validation_layers_enabled) return validation_layers_enabled;

            const auto layers = context.enumerateInstanceLayerProperties();
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

            return validation_layers_enabled;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        auto checkExtensionSupport(std::span<const std::string>      supported_extensions,
                                   std::span<const std::string_view> extensions) noexcept -> bool {
            auto required_extensions =
                core::HashSet<std::string_view> { std::ranges::begin(extensions),
                                                  std::ranges::end(extensions) };

            for (const auto& extension : supported_extensions) required_extensions.erase(extension);

            return required_extensions.empty();
        }

        /////////////////////////////////////
        /////////////////////////////////////
        auto checkExtensionSupport(std::span<const std::string>    supported_extensions,
                                   std::span<const core::CZString> extensions) noexcept -> bool {
            const auto ext = extensions | std::views::transform([](const auto& extension) noexcept {
                                 return std::string_view { extension };
                             }) |
                             std::ranges::to<std::vector>();
            return checkExtensionSupport(supported_extensions, ext);
        }
    } // namespace

    /////////////////////////////////////
    /////////////////////////////////////
    auto Instance::doInitInstance() noexcept -> VulkanExpected<void> {
        VULKAN_HPP_DEFAULT_DISPATCHER.init();

        m_vk_context = vk::raii::Context();

        const auto exts = m_vk_context->enumerateInstanceExtensionProperties();
        m_extensions    = exts | std::views::transform([](auto&& extension) noexcept {
                           return std::string { extension.extensionName };
                       }) |
                       std::ranges::to<std::vector>();

        dlog("Instance extensions: {}", m_extensions);

        const auto validation_layers = [this]() noexcept {
            auto output = std::vector<core::CZString> {};
            m_validation_layers_enabled =
                checkValidationLayerSupport(m_vk_context, m_validation_layers_enabled);
            if (m_validation_layers_enabled) {
                dlog("Enabling layers: {}", VALIDATION_LAYERS);

                output = VALIDATION_LAYERS | std::ranges::to<std::vector>();
            }

            return output;
        }();

        const auto instance_extensions = [this]() noexcept {
            auto e = core::concat(BASE_EXTENSIONS, SURFACE_EXTENSIONS, WSI_SURFACE_EXTENSIONS);

            if (m_validation_layers_enabled) core::merge(e, std::array { "VK_EXT_debug_utils" });

            return e;
        }();

        core::ensures(checkExtensionSupport(m_extensions, instance_extensions));
        core::ensures(checkExtensionSupport(m_extensions, WSI_SURFACE_EXTENSIONS));

        constexpr auto ENGINE_NAME = "StormKit";

        const auto app_info = vk::ApplicationInfo {}
                                  .setPApplicationName(std::data(m_app_name))
                                  .setPEngineName(ENGINE_NAME)
                                  .setEngineVersion(STORMKIT_VK_VERSION)
                                  .setApiVersion(vkMakeVersion<core::Int32>(1, 0, 0));

        const auto create_info = vk::InstanceCreateInfo {}
                                     .setPApplicationInfo(&app_info)
                                     .setPEnabledExtensionNames(instance_extensions)
                                     .setPEnabledLayerNames(validation_layers);

        return m_vk_context->createInstance(create_info)
            .transform(core::monadic::set(m_vk_instance))
            .and_then(core::curry(&Instance::doInitDebugReportCallback, this))
            .transform(
                [this]() noexcept { VULKAN_HPP_DEFAULT_DISPATCHER.init(*m_vk_instance.get()); });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Instance::doInitDebugReportCallback() noexcept -> VulkanExpected<void> {
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

        return m_vk_instance->createDebugUtilsMessengerEXT(create_info)
            .transform(core::monadic::set(m_vk_messenger))
            .transform([] noexcept { ilog("Validation layers enabled !"); });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Instance::doRetrievePhysicalDevices() noexcept -> VulkanExpected<void> {
        return m_vk_instance->enumeratePhysicalDevices().transform([this](auto&& physical_devices) {
            m_physical_devices = std::forward<decltype(physical_devices)>(physical_devices) |
                                 std::views::transform([this](auto&& physical_device) {
                                     return PhysicalDevice { std::move(physical_device), *this };
                                 }) |
                                 std::ranges::to<std::vector>();
        });
    }
} // namespace stormkit::gpu
