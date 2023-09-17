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
                                         bool m_vk_validation_layers_enabled) noexcept -> bool {
            if (!m_vk_validation_layers_enabled) return m_vk_validation_layers_enabled;

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

            return m_vk_validation_layers_enabled;
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
    } // namespace

    /////////////////////////////////////
    /////////////////////////////////////
    auto Renderer::doInitInstance(std::string_view application_name) noexcept
        -> VulkanExpected<void> {
        VULKAN_HPP_DEFAULT_DISPATCHER.init();

        m_vk_context = vk::raii::Context {};

        const auto exts       = m_vk_context->enumerateInstanceExtensionProperties();
        const auto extensions = exts | std::views::transform([](auto&& extension) noexcept {
                                    return std::string { extension.extensionName };
                                }) |
                                std::ranges::to<std::vector>();

        dlog("Instance extensions -----------");
        for (const auto& str : extensions) dlog("	{}", str);
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
        const auto physical_devices = m_vk_instance->enumeratePhysicalDevices();
        /*const auto extensions = physical_device.enumerateDeviceExtensionProperties() | std::views::transform([](auto &&extention) noexcept {

  }) | std::ranges::to<std::vector>();

        core::expects(std::ranges::find_first_not_of(extensions, BASE_EXTENSIONS));
        core::expects(std::ranges::find_first_not_of(extensions, SWAPCHAIN_EXTENSIONS));

        const auto support_raytracing = std::ranges::find_first_not_of(extensions, BASE_EXTENSIONS); */

        return {};
    }

} // namespace stormkit::engine
