// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#if defined(STORMKIT_CXX20_MODULES)
module stormkit.render.core.instance;

/////////// - STL - ///////////
import<map>;
import<unordered_set>;

#else
    /////////// - STL - ///////////
    #include <map>
    #include <unordered_set>

    /////////// - StormKit::core - ///////////
    #include <stormkit/core/Math.mpp>

    /////////// - StormKit::log - ///////////
    #include <stormkit/log/LogHandler.mpp>

    /////////// - StormKit::render - ///////////
    #include <stormkit/render/core/Instance.mpp>
    #include <stormkit/render/core/OffscreenSurface.mpp>
    #include <stormkit/render/core/PhysicalDevice.mpp>
    #include <stormkit/render/core/PhysicalDeviceInfo.mpp>
    #include <stormkit/render/core/WindowSurface.mpp>
#endif

/////////// - StormKit::log - ///////////
#include <stormkit/log/LogMacro.hpp>

namespace stormkit::render {
    LOGGER("Stormkit.Render.core.Instance")

    auto scorePhysicalDevice(const PhysicalDevice &physical_device) -> core::UInt64 {
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

    /////////////////////////////////////
    /////////////////////////////////////
    extern "C" VKAPI_ATTR auto
        vkDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                        VkDebugUtilsMessageTypeFlagsEXT message_type,
                        const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
                        [[maybe_unused]] void *user_data) -> VkBool32 {
        auto format_string = "";

        if (message_type == VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
            format_string = "[Validation] {}";
        else if (message_type == VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
            return VK_FALSE; // format_string = "[General] {}";
        else if (message_type == VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
            format_string = "[Performance] {}";

        switch (message_severity) {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                dlog(format_string, callback_data->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                elog(format_string, callback_data->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                wlog(format_string, callback_data->pMessage);
                break;
            default: break;
        }

        return VK_FALSE;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Instance::Instance(std::string app_name) : m_app_name { std::move(app_name) } {
        if (s_has_instance) {
            flog("Trying to create a second Instance, aborting...");
            std::exit(EXIT_FAILURE);
        }

        if (!m_loader.success()) {
            flog("Failed to initialize vulkan");
            std::exit(EXIT_FAILURE);
        }

        static auto volk_initialized = false;

        if (!volk_initialized) {
            // volkInitializeCustom(m_loader.get<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr"));
            volkInitialize();
            volk_initialized = true;
        }

        createInstance();

        retrievePhysicalDevices();

        s_has_instance = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Instance::~Instance() {
        if (m_messenger != VK_NULL_HANDLE) [[likely]]
            vkDestroyDebugUtilsMessengerEXT(m_instance, m_messenger, nullptr);
        if (m_instance != VK_NULL_HANDLE) [[likely]]
            vkDestroyInstance(m_instance, nullptr);

        s_has_instance = false;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Instance::Instance(Instance &&other) noexcept
        : m_loader { std::move(other.m_loader) }, m_instance { std::exchange(other.m_instance,
                                                                             VK_NULL_HANDLE) },
          m_messenger { std::exchange(other.m_messenger, VK_NULL_HANDLE) },
          m_physical_devices { std::move(other.m_physical_devices) },
          m_app_name { std::move(other.m_app_name) }, m_extensions { std::move(
                                                          other.m_extensions) } {}

    /////////////////////////////////////
    /////////////////////////////////////
    auto Instance::operator=(Instance &&other) noexcept -> Instance & {
        if (&other == this) [[unlikely]]
            return *this;

        m_loader           = std::move(other.m_loader);
        m_instance         = std::exchange(other.m_instance, VK_NULL_HANDLE);
        m_messenger        = std::exchange(other.m_messenger, VK_NULL_HANDLE);
        m_physical_devices = std::move(other.m_physical_devices);
        m_app_name         = std::move(other.m_app_name);
        m_extensions       = std::move(other.m_extensions);

        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Instance::createInstance() noexcept -> void {
        auto target_api_version = core::UInt32 { VK_API_VERSION_1_0 };

        auto supported_api_version = volkGetInstanceVersion();
        target_api_version         = std::max(target_api_version, supported_api_version);

        auto count = 0u;

        auto extension_count = core::UInt32 { 0 };
        CHECK_VK_ERROR(vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr));
        auto extensions = std::vector<VkExtensionProperties> { extension_count };
        CHECK_VK_ERROR(vkEnumerateInstanceExtensionProperties(nullptr,
                                                              &extension_count,
                                                              std::data(extensions)));

        m_extensions.reserve(extension_count);
        std::ranges::transform(extensions,
                               std::back_inserter(m_extensions),
                               [](const auto &extension) {
                                   return std::string { extension.extensionName };
                               });

        const auto app_info = VkApplicationInfo {
            .sType            = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pApplicationName = std::data(m_app_name),
            .pEngineName      = "StormKit C++",
            .engineVersion    = STORMKIT_VERSION,
            .apiVersion       = target_api_version,
        };

        auto create_info = VkInstanceCreateInfo { .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                                                  .pApplicationInfo      = &app_info,
                                                  .enabledExtensionCount = 0 };

        auto instance_extensions = std::vector<core::CZString> {};
        for (auto ext : INSTANCE_EXTENSIONS)
            if (checkExtensionSupport(ext)) instance_extensions.emplace_back(std::data(ext));

        dlog("Instance extensions -----------");
        for (const auto &str : m_extensions) dlog("	{}", str);
        dlog("-------------------------------");

        const auto enable_validation = checkValidationLayerSupport(ENABLE_VALIDATION);
        if (enable_validation) {
            dlog("Validation layers enabled");
            dlog("enabling layers: -----------");
            for (const auto &str : VALIDATION_LAYERS) dlog("	{}", str);
            dlog("-------------------------------");

            create_info.enabledLayerCount   = std::size(VALIDATION_LAYERS);
            create_info.ppEnabledLayerNames = std::data(VALIDATION_LAYERS);

            instance_extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        create_info.enabledExtensionCount   = std::size(instance_extensions);
        create_info.ppEnabledExtensionNames = std::data(instance_extensions);

        CHECK_VK_ERROR(vkCreateInstance(&create_info, nullptr, &m_instance));

        volkLoadInstanceOnly(m_instance);

        if (enable_validation) createDebugReportCallback();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Instance::createDebugReportCallback() noexcept -> void {
        STORMKIT_EXPECTS(m_instance != VK_NULL_HANDLE);

        const auto create_info = VkDebugUtilsMessengerCreateInfoEXT {
            .sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT,
            .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = vkDebugCallback
        };

        CHECK_VK_ERROR(vkCreateDebugUtilsMessengerEXT(*this, &create_info, nullptr, &m_messenger));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Instance::retrievePhysicalDevices() noexcept -> void {
        STORMKIT_EXPECTS(m_instance != VK_NULL_HANDLE);

        auto physical_devices_count = core::UInt32 { 0 };
        CHECK_VK_ERROR(vkEnumeratePhysicalDevices(*this, &physical_devices_count, nullptr))
        auto physical_devices = std::vector<VkPhysicalDevice> { physical_devices_count };
        CHECK_VK_ERROR(vkEnumeratePhysicalDevices(*this,
                                                  &physical_devices_count,
                                                  std::data(physical_devices)));

        std::ranges::transform(physical_devices,
                               std::back_inserter(m_physical_devices),
                               [this](const auto &physical_device) {
                                   return PhysicalDevice { physical_device, *this };
                               });
    }

#ifdef STORMKIT_WSI_ENABLED
    /////////////////////////////////////
    /////////////////////////////////////
    auto Instance::createWindowSurface(const wsi::Window &window,
                                       Surface::Buffering buffering) const -> WindowSurface {
        STORMKIT_EXPECTS(m_instance != VK_NULL_HANDLE);

        return WindowSurface { window, *this, buffering };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Instance::allocateWindowSurface(const wsi::Window &window,
                                         Surface::Buffering buffering) const
        -> WindowSurfaceOwnedPtr {
        STORMKIT_EXPECTS(m_instance != VK_NULL_HANDLE);

        return std::make_unique<WindowSurface>(window, *this, buffering);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Instance::allocateRefCountedWindowSurface(const wsi::Window &window,
                                                   Surface::Buffering buffering) const
        -> WindowSurfaceSharedPtr {
        STORMKIT_EXPECTS(m_instance != VK_NULL_HANDLE);

        return std::make_shared<WindowSurface>(window, *this, buffering);
    }
#endif

    /////////////////////////////////////
    /////////////////////////////////////
    auto Instance::createOffscreenSurface(const core::ExtentU &extent,
                                          Surface::Buffering buffering) const
        -> render::OffscreenSurface {
        STORMKIT_EXPECTS(m_instance != VK_NULL_HANDLE);

        return OffscreenSurface { extent, *this, buffering };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Instance::allocateOffscreenSurface(const core::ExtentU &extent,
                                            Surface::Buffering buffering) const
        -> OffscreenSurfaceOwnedPtr {
        STORMKIT_EXPECTS(m_instance != VK_NULL_HANDLE);

        return std::make_unique<OffscreenSurface>(extent, *this, buffering);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Instance::allocateRefCountedOffscreenSurface(const core::ExtentU &extent,
                                                      Surface::Buffering buffering) const
        -> OffscreenSurfaceSharedPtr {
        STORMKIT_EXPECTS(m_instance != VK_NULL_HANDLE);

        return std::make_shared<OffscreenSurface>(extent, *this, buffering);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Instance::pickPhysicalDevice() const noexcept -> const render::PhysicalDevice & {
        STORMKIT_EXPECTS(m_instance != VK_NULL_HANDLE);

        auto ranked_devices = std::multimap<core::UInt64, render::PhysicalDeviceConstRef> {};

        for (const auto &physical_device : m_physical_devices) {
            const auto score = scorePhysicalDevice(physical_device);

            ranked_devices.emplace(score, physical_device);
        }
        // ASSERT(ranked_devices.rbegin()->first >= 0u);

        return ranked_devices.rbegin()->second.get();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Instance::pickPhysicalDevice(const render::WindowSurface &surface) noexcept
        -> const render::PhysicalDevice & {
        STORMKIT_EXPECTS(m_instance != VK_NULL_HANDLE);
        auto ranked_devices = std::multimap<core::UInt64, render::PhysicalDeviceConstRef> {};

        for (auto &physical_device : m_physical_devices) {
            physical_device.checkIfPresentSupportIsEnabled(surface);

            const auto score = scorePhysicalDevice(physical_device);

            ranked_devices.emplace(score, physical_device);
        }

        // ASSERT(ranked_devices.rbegin()->first >= 0u);

        return ranked_devices.rbegin()->second.get();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Instance::checkValidationLayerSupport(bool enable_validation) noexcept -> bool {
        if (!enable_validation) return enable_validation;

        auto layers_count = core::UInt32 { 0 };
        CHECK_VK_ERROR(vkEnumerateInstanceLayerProperties(&layers_count, nullptr));
        auto layers = std::vector<VkLayerProperties> { layers_count };
        CHECK_VK_ERROR(vkEnumerateInstanceLayerProperties(&layers_count, std::data(layers)));

        for (const auto &layer_name : std::as_const(VALIDATION_LAYERS)) {
            auto layer_found = false;

            for (const auto &layer_properties : layers) {
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

        for (const auto &extension : m_extensions) required_extensions.erase(extension);

        return required_extensions.empty();
    }
} // namespace stormkit::render
