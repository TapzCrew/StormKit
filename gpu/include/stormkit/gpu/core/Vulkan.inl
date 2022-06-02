// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "Vulkan.mpp"

namespace stormkit::gpu {
    namespace details {
        constexpr static auto VK_RESULT_TO_STRING = [] {
            using namespace std::literals;

            return core::makeFrozenMap<VkResult, std::string_view>({
                PAIR(VK_NOT_READY),
                PAIR(VK_TIMEOUT),
                PAIR(VK_EVENT_SET),
                PAIR(VK_EVENT_RESET),
                PAIR(VK_INCOMPLETE),
                PAIR(VK_ERROR_OUT_OF_HOST_MEMORY),
                PAIR(VK_ERROR_OUT_OF_DEVICE_MEMORY),
                PAIR(VK_ERROR_OUT_OF_POOL_MEMORY),
                PAIR(VK_ERROR_INITIALIZATION_FAILED),
                PAIR(VK_ERROR_DEVICE_LOST),
                PAIR(VK_ERROR_MEMORY_MAP_FAILED),
                PAIR(VK_ERROR_LAYER_NOT_PRESENT),
                PAIR(VK_ERROR_EXTENSION_NOT_PRESENT),
                PAIR(VK_ERROR_FEATURE_NOT_PRESENT),
                PAIR(VK_ERROR_INCOMPATIBLE_DRIVER),
                PAIR(VK_ERROR_TOO_MANY_OBJECTS),
                PAIR(VK_ERROR_FORMAT_NOT_SUPPORTED),
                PAIR(VK_ERROR_SURFACE_LOST_KHR),
                PAIR(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR),
                PAIR(VK_SUBOPTIMAL_KHR),
                PAIR(VK_ERROR_OUT_OF_DATE_KHR),
                PAIR(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR),
                PAIR(VK_ERROR_VALIDATION_FAILED_EXT),
                PAIR(VK_ERROR_INVALID_SHADER_NV),
            });
        }();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<std::integral T>
    constexpr auto vkMakeVersion(T major, T minor, T patch) noexcept -> core::UInt32 {
        return vkVersionMajor(major) | vkVersionMinor(minor) | vkVersionPatch(patch);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    constexpr auto vkVersionMajor(std::integral auto version) noexcept -> core::UInt32 {
        return core::as<core::UInt32>(version >> 22u);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    constexpr auto vkVersionMinor(std::integral auto version) noexcept -> core::UInt32 {
        return core::as<core::UInt32>((version >> 12u) & 0x3ffu);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    constexpr auto vkVersionPatch(std::integral auto version) noexcept -> core::UInt32 {
        return core::as<core::UInt32>(version & 0xfffu);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto checkVkError(VkResult result,
                             std::string_view str_line,
                             std::string file,
                             std::string function,
                             core::UInt32 line) noexcept -> void {
        if (result == VK_SUCCESS) return;
        using log::operator"" _module;

        const auto m = "Vulkan"_module;

        log::LogHandler::flog(m,
                              "\"{}\" failed, return code: {}\n\tat {} in function {} at line {}",
                              str_line,
                              vkResultToString(result),
                              file,
                              function,
                              line);

        core::printStacktrace();

        std::quick_exit(EXIT_FAILURE);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    constexpr auto vkResultToString(VkResult error) -> std::string_view {
        using namespace std::literals;

        const auto it = details::VK_RESULT_TO_STRING.find(error);
        if (it == std::ranges::cend(details::VK_RESULT_TO_STRING)) [[unlikely]]
            return "Unknow error"sv;

        return it->second;
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
} // namespace stormkit::gpu
