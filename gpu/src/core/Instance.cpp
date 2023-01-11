// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#ifdef STORMKIT_BUILD_MODULES
module stormkit.Gpu:Core;
#else
    #include <stormkit/std.hpp>

    #include <stormkit/Core.hpp>
    #include <stormkit/Gpu.hpp>
    #include <stormkit/Log.hpp>
#endif

namespace stormkit::gpu {
    NAMED_LOGGER(instance_logger, "stormkit.Gpu:core.Instance")

    static constexpr auto VALIDATION_LAYERS = std::array {
        "VK_LAYER_KHRONOS_validation",
#ifdef STORMKIT_OS_LINUX
        "VK_LAYER_MESA_overlay",
#endif
    };
    static constexpr auto VALIDATION_FEATURES =
        std::array { VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT,
                     VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT };

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
                instance_logger.dlog(format_string, callback_data->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                instance_logger.elog(format_string, callback_data->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                instance_logger.wlog(format_string, callback_data->pMessage);
                break;
            default: break;
        }

        return VK_FALSE;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Instance::Instance(std::string app_name) : m_app_name { std::move(app_name) } {
        if (s_has_instance) {
            instance_logger.flog("Trying to create a second Instance, aborting...");
            std::exit(EXIT_FAILURE);
        }

        if (!m_loader.success()) {
            instance_logger.flog("Failed to initialize vulkan");
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
    Instance::Instance(Instance&& other) noexcept
        : m_loader { std::move(other.m_loader) },
          m_instance { std::exchange(other.m_instance, VK_NULL_HANDLE) },
          m_messenger { std::exchange(other.m_messenger, VK_NULL_HANDLE) },
          m_physical_devices { std::move(other.m_physical_devices) },
          m_app_name { std::move(other.m_app_name) },
          m_extensions { std::move(other.m_extensions) } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Instance::operator=(Instance&& other) noexcept -> Instance& {
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

        auto extension_count = core::UInt32 { 0 };
        CHECK_VK_ERROR(vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr));
        auto extensions = std::vector<VkExtensionProperties> { extension_count };
        CHECK_VK_ERROR(vkEnumerateInstanceExtensionProperties(nullptr,
                                                              &extension_count,
                                                              std::data(extensions)));

        m_extensions.reserve(extension_count);
        std::ranges::transform(extensions,
                               std::back_inserter(m_extensions),
                               [](const auto& extension) {
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

        instance_logger.dlog("Instance extensions -----------");
        for (const auto& str : m_extensions) instance_logger.dlog("	{}", str);
        instance_logger.dlog("-------------------------------");

        const auto enable_validation = checkValidationLayerSupport(ENABLE_VALIDATION);
        if (enable_validation) {
            instance_logger.dlog("Validation layers enabled");
            instance_logger.dlog("enabling layers: -----------");
            for (const auto& str : VALIDATION_LAYERS) instance_logger.dlog("	{}", str);
            instance_logger.dlog("-------------------------------");

            create_info.enabledLayerCount   = core::as<core::UInt32>(std::size(VALIDATION_LAYERS));
            create_info.ppEnabledLayerNames = std::data(VALIDATION_LAYERS);
        }

        const auto instance_extensions = [&] {
            auto e = core::concat(INSTANCE_BASE_EXTENSIONS, SURFACE_EXTENSIONS);

            if (enable_validation) core::merge(e, std::array { VK_EXT_DEBUG_UTILS_EXTENSION_NAME });

            return e;
        }();

        create_info.enabledExtensionCount = core::as<core::UInt32>(std::size(instance_extensions));
        create_info.ppEnabledExtensionNames = std::data(instance_extensions);

        core::ensures(not checkExtensionSupport(instance_extensions));

        CHECK_VK_ERROR(vkCreateInstance(&create_info, nullptr, &m_instance));

        volkLoadInstanceOnly(m_instance);

        if (enable_validation) createDebugReportCallback();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Instance::createDebugReportCallback() noexcept -> void {
        core::expects(m_instance != VK_NULL_HANDLE);

        const auto create_info = VkDebugUtilsMessengerCreateInfoEXT {
            .sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
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
        core::expects(m_instance != VK_NULL_HANDLE);

        auto physical_devices_count = core::UInt32 { 0 };
        CHECK_VK_ERROR(vkEnumeratePhysicalDevices(*this, &physical_devices_count, nullptr))
        auto physical_devices = std::vector<VkPhysicalDevice> { physical_devices_count };
        CHECK_VK_ERROR(vkEnumeratePhysicalDevices(*this,
                                                  &physical_devices_count,
                                                  std::data(physical_devices)));

        std::ranges::transform(physical_devices,
                               std::back_inserter(m_physical_devices),
                               [this](const auto& physical_device) {
                                   return PhysicalDevice { physical_device, *this };
                               });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Instance::checkValidationLayerSupport(bool enable_validation) noexcept -> bool {
        if (!enable_validation) return enable_validation;

        auto layers_count = core::UInt32 { 0 };
        CHECK_VK_ERROR(vkEnumerateInstanceLayerProperties(&layers_count, nullptr));
        auto layers = std::vector<VkLayerProperties> { layers_count };
        CHECK_VK_ERROR(vkEnumerateInstanceLayerProperties(&layers_count, std::data(layers)));

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
        const auto ext =
            core::transform(extensions, [](const auto& v) { return std::string_view { v }; });
        return checkExtensionSupport(ext);
    }
} // namespace stormkit::gpu
