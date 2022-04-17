#pragma once

namespace stormkit::render {
    /////////////////////////////////////
    /////////////////////////////////////
    template<std::integral T>
    constexpr auto vkMakeVersion(T major, T minor, T patch) -> core::UInt32 {
        return vkVersionMajor(major) | vkVersionMinor(minor) | vkVersionPatch(patch);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    constexpr auto vkVersionMajor(std::integral auto version) -> core::UInt32 {
        return core::as<core::UInt32>(version >> 22u);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    constexpr auto vkVersionMinor(std::integral auto version) -> core::UInt32 {
        return core::as<core::UInt32>((version >> 12u) & 0x3ffu);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    constexpr auto vkVersionPatch(std::integral auto version) -> core::UInt32 {
        return core::as<core::UInt32>(version & 0xfffu);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto checkVkError(VkResult result,
                             std::string file,
                             std::string function,
                             core::UInt32 line) noexcept -> void {
        if (result == VK_SUCCESS) return;
        using log::operator"" _module;

        const auto m = "Vulkan"_module;

        log::LogHandler::flog(
            m,
            "Vulkan function failed (return code: {})\nat {} in function {} at line {}",
            vkResultToString(result),
            file,
            function,
            line);

        std::exit(EXIT_FAILURE);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    constexpr auto vkResultToString(VkResult error) -> std::string_view {
        switch (error) {
            CASE(VK_NOT_READY);
            CASE(VK_TIMEOUT);
            CASE(VK_EVENT_SET);
            CASE(VK_EVENT_RESET);
            CASE(VK_INCOMPLETE);
            CASE(VK_ERROR_OUT_OF_HOST_MEMORY);
            CASE(VK_ERROR_OUT_OF_DEVICE_MEMORY);
            CASE(VK_ERROR_INITIALIZATION_FAILED);
            CASE(VK_ERROR_DEVICE_LOST);
            CASE(VK_ERROR_MEMORY_MAP_FAILED);
            CASE(VK_ERROR_LAYER_NOT_PRESENT);
            CASE(VK_ERROR_EXTENSION_NOT_PRESENT);
            CASE(VK_ERROR_FEATURE_NOT_PRESENT);
            CASE(VK_ERROR_INCOMPATIBLE_DRIVER);
            CASE(VK_ERROR_TOO_MANY_OBJECTS);
            CASE(VK_ERROR_FORMAT_NOT_SUPPORTED);
            CASE(VK_ERROR_SURFACE_LOST_KHR);
            CASE(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR);
            CASE(VK_SUBOPTIMAL_KHR);
            CASE(VK_ERROR_OUT_OF_DATE_KHR);
            CASE(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR);
            CASE(VK_ERROR_VALIDATION_FAILED_EXT);
            CASE(VK_ERROR_INVALID_SHADER_NV);
            default: return "Unknow error";
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto computeMipLevel(const core::ExtentU &extent) noexcept -> core::UInt32 {
        const auto as_float = core::ExtentF { extent };

        return core::as<core::UInt32>(
                   glm::floor(glm::log2(glm::max(as_float.width, as_float.height)))) +
               1;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<core::NonPointerType T>
    requires VulkanObject<T>
    constexpr auto getHandle(const T &t) noexcept { return t.vkHandle(); }

    /////////////////////////////////////
    /////////////////////////////////////
    template<core::RawPointerType T>
    requires VulkanObject<std::remove_pointer_t<T>>
    constexpr auto getHandle(T t) noexcept {
        STORMKIT_CONSTEXPR_EXPECTS(t);

        return t->vkHandle();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<core::SmartPointerType T>
    requires VulkanObject<typename T::element_type>
    constexpr auto getHandle(const T &t) noexcept {
        STORMKIT_CONSTEXPR_EXPECTS(t);

        return t->vkHandle();
    }
    static constexpr auto STORMKIT_VERSION = vkMakeVersion(core::STORMKIT_MAJOR_VERSION,
                                                           core::STORMKIT_MINOR_VERSION,
                                                           core::STORMKIT_PATCH_VERSION);
} // namespace stormkit::render
