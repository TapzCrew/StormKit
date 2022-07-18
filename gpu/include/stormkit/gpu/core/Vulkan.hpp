// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/MemoryMacro.hpp>
#include <stormkit/core/PlatformMacro.hpp>
#include <stormkit/core/StringsMacro.hpp>

#if STORMKIT_WSI_ENABLED
    #if defined(STORMKIT_OS_LINUX)
        #if STORMKIT_WSI_BUILD_X11
            #define VK_USE_PLATFORM_XCB_KHR 1
        #endif
        #if STORMKIT_WSI_BUILD_WAYLAND
            #define VK_USE_PLATFORM_WAYLAND_KHR 1
        #endif
    #elif defined(STORMKIT_OS_WINDOWS)
        #define VK_USE_PLATFORM_WIN32_KHR 1
    #elif defined(STORMKIT_OS_MACOS)
        #define VK_USE_PLATFORM_MACOS_MVK 1
    #elif defined(STORMKIT_OS_IOS)
        #define VK_USE_PLATFORM_IOS_MVK 1
    #endif
#endif

#if defined(STORMKIT_BUILD_DEBUG)
    #define STORMKIT_ENABLE_VALIDATION_LAYERS
#endif

#define VK_NO_PROTOTYPES
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 0
#define VMA_STATIC_VULKAN_FUNCTIONS 0

#ifdef module
    #undef module
    #define module_back
#endif

#include <volk.h>

#undef VK_NULL_HANDLE
#define VK_NULL_HANDLE nullptr

#include <vk_mem_alloc.h>

#include <stormkit/gpu/core/VulkanMacro.hpp>

#ifdef module_back
    #undef module_back
    #define module
#endif

#include <array>
#include <ranges>

#include <stormkit/core/Concepts.hpp>
#include <stormkit/core/Configure.hpp>
#include <stormkit/core/Math.hpp>
#include <stormkit/core/Types.hpp>

#include <stormkit/log/LogHandler.hpp>

namespace stormkit::gpu {
    template<std::integral T>
    constexpr auto vkMakeVersion(T major, T minor, T patch) noexcept -> core::UInt32;

    constexpr auto vkVersionMajor(std::integral auto version) noexcept -> core::UInt32;

    constexpr auto vkVersionMinor(std::integral auto version) noexcept -> core::UInt32;

    constexpr auto vkVersionPatch(std::integral auto version) noexcept -> core::UInt32;

    static constexpr auto DEVICE_EXTENSIONS = std::array {
        core::CZString { VK_KHR_MAINTENANCE3_EXTENSION_NAME },
#if STORMKIT_WSI_ENABLED
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
#endif
    };

#ifdef STORMKIT_ENABLE_VALIDATION_LAYERS
    static constexpr auto VALIDATION_LAYERS = std::array {
        core::CZString { "VK_LAYER_KHRONOS_validation" },
    #ifdef STORMKIT_OS_LINUX
            "VK_LAYER_MESA_overlay",
    #endif
    #if !defined(STORMKIT_OS_MACOS) && !STORMKIT_ENABLE_WAYLAND
            "VK_LAYER_LUNARG_monitor",
    #endif
    };
#else
    static constexpr auto VALIDATION_LAYERS   = std::array<core::CZString, 0> {};
#endif

    static constexpr auto VALIDATION_FEATURES =
        std::array { VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT,
                     VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT };

#if STORMKIT_WSI_ENABLED
    static constexpr auto INSTANCE_EXTENSIONS = std::array {
        std::string_view { VK_KHR_SURFACE_EXTENSION_NAME },
            std::string_view { VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME },
    #if defined(STORMKIT_OS_LINUX)
        #if STORMKIT_ENABLE_WAYLAND
            std::string_view { VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME },
        #endif
        #if STORMKIT_ENABLE_XCB
            std::string_view { VK_KHR_XCB_SURFACE_EXTENSION_NAME },
        #endif
    #elif defined(STORMKIT_OS_WINDOWS)
            std::string_view { VK_KHR_WIN32_SURFACE_EXTENSION_NAME },
    #elif defined(STORMKIT_OS_MACOS)
            std::string_view { VK_MVK_MACOS_SURFACE_EXTENSION_NAME },
    #elif defined(STORMKIT_OS_IOS)
            std::string_view { VK_MVK_IOS_SURFACE_EXTENSION_NAME },
    #endif
    };
#else
    static constexpr auto INSTANCE_EXTENSIONS = std::array<std::string_view, 0> {};
#endif

#if defined(STORMKIT_RENDER_SAFE_VULKAN) || defined(STORMKIT_ENABLE_VALIDATION_LAYERS)
    static constexpr auto ENABLE_VALIDATION = true;
#else
    static constexpr auto ENABLE_VALIDATION   = false;
#endif

    [[noreturn]] auto checkVkError(VkResult result,
                                   std::string file,
                                   std::string function,
                                   core::UInt32 line) noexcept -> void;

    constexpr auto vkResultToString(VkResult error) -> std::string_view;

    inline auto computeMipLevel(const core::ExtentU &extent) noexcept -> core::UInt32;

    template<typename T>
    concept VulkanObject = requires(const T &t) {
        t.vkHandle();
    };

    template<core::NonPointerType T>
    requires VulkanObject<T>
    constexpr auto getHandle(const T &t) noexcept;

    template<core::RawPointerType T>
    requires VulkanObject<std::remove_pointer_t<T>>
    constexpr auto getHandle(T t) noexcept;

    template<core::SmartPointerType T>
    requires VulkanObject<typename T::element_type>
    constexpr auto getHandle(const T &t) noexcept;

} // namespace stormkit::gpu

#include "Vulkan.inl"
