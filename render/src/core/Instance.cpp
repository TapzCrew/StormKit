// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "../Log.hpp"

#include <map>
#include <unordered_set>

#include <storm/core/Math.hpp>

#include <storm/log/LogHandler.hpp>

#include <storm/render/core/Instance.hpp>
#include <storm/render/core/OffscreenSurface.hpp>
#include <storm/render/core/PhysicalDevice.hpp>
#include <storm/render/core/PhysicalDeviceInfo.hpp>
#include <storm/render/core/WindowSurface.hpp>

using namespace storm;
using namespace storm::log;
using namespace storm::render;

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE;

static constexpr inline auto VULKAN_LOG_MODULE = storm::log::makeModule("StormKit.Render.Vulkan");

core::UInt64 scorePhysicalDevice(const render::PhysicalDevice &physical_device) {
    if (!physical_device.checkExtensionSupport(DEVICE_EXTENSIONS)) return 0u;

    auto score = core::UInt64 { 0u };

    const auto &info = physical_device.info();

    if (info.type == render::PhysicalDeviceType::Discrete_GPU) score += 10000000u;
    else if (info.type == render::PhysicalDeviceType::Virtual_GPU)
        score += 5000000u;
    else if (info.type == render::PhysicalDeviceType::Integrated_GPU)
        score += 250000u;

    const auto &capabilities = physical_device.capabilities();

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

extern "C" {
/////////////////////////////////////
/////////////////////////////////////
VKAPI_ATTR VkBool32 vkDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                    VkDebugUtilsMessageTypeFlagsEXT message_type,
                                    const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
                                    [[maybe_unused]] void *user_data) {
    auto format_string = "";

    if (message_type == VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
        format_string = "[Validation] {}";
    else if (message_type == VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
        return VK_FALSE; // format_string = "[General] {}";
    else if (message_type == VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
        format_string = "[Performance] {}";

    switch (message_severity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            LogHandler::dlog(VULKAN_LOG_MODULE, format_string, callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            LogHandler::elog(VULKAN_LOG_MODULE, format_string, callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            LogHandler::wlog(VULKAN_LOG_MODULE, format_string, callback_data->pMessage);
            break;
        default: break;
    }

    return VK_FALSE;
}
}

Instance::Instance(VkInstance instance) : m_vk_instance { vk::Instance { instance } } {
    if (!m_loader.success()) {
        flog("Failed to initialize vulkan");
        std::exit(EXIT_FAILURE);
    }

    VULKAN_HPP_DEFAULT_DISPATCHER.init(m_loader.vkGetInstanceProcAddr());
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkInstance());

    retrievePhysicalDevices();
}

/////////////////////////////////////
/////////////////////////////////////
Instance::Instance(std::string app_name) : m_app_name { app_name } {
    if (!m_loader.success()) {
        flog("Failed to initialize vulkan");
        std::exit(EXIT_FAILURE);
    }

    VULKAN_HPP_DEFAULT_DISPATCHER.init(m_loader.vkGetInstanceProcAddr());

    createInstance();

    retrievePhysicalDevices();
}

/////////////////////////////////////
/////////////////////////////////////
Instance::~Instance() = default;

/////////////////////////////////////
/////////////////////////////////////
Instance::Instance(Instance &&) = default;

/////////////////////////////////////
/////////////////////////////////////
Instance &Instance::operator=(Instance &&) = default;

/////////////////////////////////////
/////////////////////////////////////
void Instance::createInstance() noexcept {
    auto target_api_version = core::UInt32 { VK_API_VERSION_1_0 };

    if (VULKAN_HPP_DEFAULT_DISPATCHER.vkEnumerateInstanceVersion) {
        auto supported_api_version = 0u;
        VULKAN_HPP_DEFAULT_DISPATCHER.vkEnumerateInstanceVersion(&supported_api_version);

        target_api_version = std::max(target_api_version, supported_api_version);
    }

    auto count = 0u;
    VULKAN_HPP_DEFAULT_DISPATCHER.vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);

    auto extensions = std::vector<VkExtensionProperties> {};
    extensions.resize(count);
    VULKAN_HPP_DEFAULT_DISPATCHER.vkEnumerateInstanceExtensionProperties(nullptr,
                                                                         &count,
                                                                         std::data(extensions));

    m_extensions.reserve(std::size(extensions));
    std::transform(std::cbegin(extensions),
                   std::cend(extensions),
                   std::back_inserter(m_extensions),
                   [](const auto &extension) { return std::string { extension.extensionName }; });

    const auto app_info = vk::ApplicationInfo {}
                              .setPEngineName("StormKit C++")
                              .setEngineVersion(STORMKIT_VERSION)
                              .setApiVersion(target_api_version)
                              .setPApplicationName(std::data(m_app_name));

    auto create_info =
        vk::InstanceCreateInfo {}.setPApplicationInfo(&app_info).setEnabledLayerCount(0);

    auto instance_extensions = std::vector<gsl::czstring<>> {};
    for (auto ext : INSTANCE_EXTENSIONS) {
        if (checkExtensionSupport(ext)) instance_extensions.emplace_back(ext);
    }

    // auto has_base_exts = checkExtensionSupport(instance_extensions);
    dlog("Instance extensions -----------");
    for (const auto &str : m_extensions) dlog("	{}", str);
    dlog("-------------------------------");

    const auto enable_validation = checkValidationLayerSupport(ENABLE_VALIDATION);
    if (enable_validation) {
        dlog("Validation layers enabled");
        dlog("enabling layers: -----------");
        for (const auto &str : VALIDATION_LAYERS) dlog("	{}", str);
        dlog("-------------------------------");
        create_info.setEnabledLayerCount(std::size(VALIDATION_LAYERS));
        create_info.setPpEnabledLayerNames(std::data(VALIDATION_LAYERS));

        instance_extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    create_info.setEnabledExtensionCount(std::size(instance_extensions));
    create_info.setPpEnabledExtensionNames(std::data(instance_extensions));

    CHECK_VK_ERROR_VALUE(vk::createInstanceUnique(create_info), instance);
    m_vk_instance = std::move(instance);

    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkInstance());
    // volkLoadInstance(*m_vk_instance);

    if (enable_validation) createDebugReportCallback();
}

/////////////////////////////////////
/////////////////////////////////////
void Instance::createDebugReportCallback() {
    const auto create_info =
        vk::DebugUtilsMessengerCreateInfoEXT {}
            .setMessageSeverity(vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                                vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                                vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
            .setMessageType(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                            vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance)
            .setPfnUserCallback(vkDebugCallback);

    auto instance = vkInstance();

    CHECK_VK_ERROR_VALUE(instance.createDebugUtilsMessengerEXTUnique(create_info), vk_messenger);
    m_vk_messenger = std::move(vk_messenger);
}

void Instance::retrievePhysicalDevices() noexcept {
    auto instance = vkInstance();

    CHECK_VK_ERROR_VALUE(instance.enumeratePhysicalDevices(), physical_devices);

    for (auto physical_device : physical_devices) {
        auto &p = m_physical_devices.emplace_back(
            std::make_unique<PhysicalDevice>(physical_device, *this));
        dlog("{}", p->info());
    }
}

/////////////////////////////////////
/////////////////////////////////////
render::WindowSurface Instance::createWindowSurface(const window::Window &window,
                                                    Surface::Buffering buffering) const {
    return WindowSurface { window, *this, buffering };
}

/////////////////////////////////////
/////////////////////////////////////
render::WindowSurfaceOwnedPtr Instance::createWindowSurfacePtr(const window::Window &window,
                                                               Surface::Buffering buffering) const {
    return std::make_unique<WindowSurface>(window, *this, buffering);
}

/////////////////////////////////////
/////////////////////////////////////
render::OffscreenSurface Instance::createOffscreenSurface(core::Extentu extent,
                                                          Surface::Buffering buffering) const {
    return OffscreenSurface { extent, *this, buffering };
}

/////////////////////////////////////
/////////////////////////////////////
render::OffscreenSurfaceOwnedPtr
    Instance::createOffscreenSurfacePtr(core::Extentu extent, Surface::Buffering buffering) const {
    return std::make_unique<OffscreenSurface>(extent, *this, buffering);
}

/////////////////////////////////////
/////////////////////////////////////
const render::PhysicalDevice &Instance::pickPhysicalDevice() const noexcept {
    auto ranked_devices = std::multimap<core::UInt64, render::PhysicalDeviceConstRef> {};

    for (const auto &physical_device : m_physical_devices) {
        const auto score = scorePhysicalDevice(*physical_device);

        ranked_devices.emplace(score, *physical_device);
    }
    // ASSERT(ranked_devices.rbegin()->first >= 0u);

    return ranked_devices.rbegin()->second.get();
}

/////////////////////////////////////
/////////////////////////////////////
const render::PhysicalDevice &
    Instance::pickPhysicalDevice(const render::WindowSurface &surface) noexcept {
    auto ranked_devices = std::multimap<core::UInt64, render::PhysicalDeviceConstRef> {};

    for (auto &physical_device : m_physical_devices) {
        physical_device->checkIfPresentSupportIsEnabled(surface);

        const auto score = scorePhysicalDevice(*physical_device);

        ranked_devices.emplace(score, *physical_device);
    }

    // ASSERT(ranked_devices.rbegin()->first >= 0u);

    return ranked_devices.rbegin()->second.get();
}

#if defined(STORMKIT_OS_WINDOWS)
/////////////////////////////////////
/////////////////////////////////////
vk::UniqueSurfaceKHR
    Instance::createVkSurface(const vk::Win32SurfaceCreateInfoKHR &create_info) const noexcept {
    #if STORMKIT_ENABLE_WSI
    auto instance = vkInstance();

    CHECK_VK_ERROR_VALUE(instance.createWin32SurfaceKHRUnique(create_info), surface);

    return surface;
    #else
    elog("WSI disabled in this build");
    return vk::UniqueSurfaceKHR{VK_NULL_HANDLE};
    #endif
}
#elif defined(STORMKIT_OS_MACOS)
/////////////////////////////////////
/////////////////////////////////////
vk::UniqueSurfaceKHR
    Instance::createVkSurface(const vk::MacOSSurfaceCreateInfoMVK &create_info) const noexcept {
    #if STORMKIT_ENABLE_WSI
    auto instance = vkInstance();

    CHECK_VK_ERROR_VALUE(instance.createMacOSSurfaceMVKUnique(create_info), surface);

    return surface;
    #else
    elog("WSI disabled in this build");
    return vk::UniqueSurfaceKHR{VK_NULL_HANDLE};
    #endif
}
#elif defined(STORMKIT_OS_LINUX)
    #if STORMKIT_ENABLE_XCB
/////////////////////////////////////
/////////////////////////////////////
vk::UniqueSurfaceKHR
    Instance::createVkSurface(const vk::XcbSurfaceCreateInfoKHR &create_info) const noexcept {
        #if STORMKIT_ENABLE_WSI
    auto instance = vkInstance();

    CHECK_VK_ERROR_VALUE(instance.createXcbSurfaceKHRUnique(create_info), surface);

    return surface;
        #else
    elog("WSI disabled in this build");
    return vk::UniqueSurfaceKHR{VK_NULL_HANDLE};
        #endif
}
    #endif
    #if STORMKIT_ENABLE_WAYLAND
/////////////////////////////////////
/////////////////////////////////////
vk::UniqueSurfaceKHR
    Instance::createVkSurface(const vk::WaylandSurfaceCreateInfoKHR &create_info) const noexcept {
        #if STORMKIT_ENABLE_WSI
    auto instance = vkInstance();

    CHECK_VK_ERROR_VALUE(instance.createWaylandSurfaceKHRUnique(create_info,
                                                                nullptr,
                                                                VULKAN_HPP_DEFAULT_DISPATCHER),
                         surface);

    return surface;
        #else
    elog("WSI disabled in this build");
    return vk::UniqueSurfaceKHR{VK_NULL_HANDLE};
        #endif
}
    #endif
#elif defined(STORMKIT_OS_IOS)
/////////////////////////////////////
/////////////////////////////////////
vk::UniqueSurfaceKHR
    Instance::createVkSurface(const vk::IOSSurfaceCreateInfoMVK &create_info) const noexcept {
    #if STORMKIT_ENABLE_WSI
    auto instance = vkInstance();

    CHECK_VK_ERROR_VALUE(instance.createIOSSurfaceMVKUnique(create_info), surface);

    return surface;
    #else
    elog("WSI disabled in this build");
    return vk::UniqueSurfaceKHR{VK_NULL_HANDLE};
    #endif
}
#endif

/////////////////////////////////////
/////////////////////////////////////
bool Instance::checkValidationLayerSupport(bool enable_validation) noexcept {
    if (!enable_validation) return enable_validation;

    CHECK_VK_ERROR_VALUE(vk::enumerateInstanceLayerProperties(), available_layers);

    for (auto layer_name : std::as_const(VALIDATION_LAYERS)) {
        auto layer_found = false;

        for (const auto &layer_properties : available_layers) {
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

    return enable_validation;
}

bool Instance::checkExtensionSupport(const gsl::czstring<> extension) const noexcept {
    return std::find(std::cbegin(m_extensions), std::cend(m_extensions), extension) !=
           std::cend(m_extensions);
}

bool Instance::checkExtensionSupport(core::span<const gsl::czstring<>> extensions) const noexcept {
    auto required_extensions =
        storm::core::HashSet<std::string_view> { core::ranges::begin(extensions),
                                                 core::ranges::end(extensions) };

    for (const auto &extension : m_extensions) required_extensions.erase(extension);
    auto support = required_extensions.empty();

    return support;
}
