// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <array>
#include <cstdint>
#include <type_traits>

#include <gsl/string_span>

#include <storm/core/Configure.hpp>
#include <storm/core/Platform.hpp>

#if STORMKIT_ENABLE_WSI
    #if defined(STORMKIT_OS_LINUX)
        #if STORMKIT_ENABLE_XCB
            #define VK_USE_PLATFORM_XCB_KHR 1
        #endif
        #if STORMKIT_ENABLE_WAYLAND
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

#define VK_NO_PROTOTYPES
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#define VULKAN_HPP_ENABLE_DYNAMIC_LOADER_TOOL 0
#define VULKAN_HPP_NO_EXCEPTIONS
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 0
#define VMA_STATIC_VULKAN_FUNCTIONS 0

#include <vulkan/vulkan.h>

#if STORMKIT_ENABLE_WSI
    #if defined(STORMKIT_OS_LINUX)
        #if STORMKIT_ENABLE_WAYLAND
            #include <vulkan/vulkan_wayland.h>
        #endif
        #if STORMKIT_ENABLE_XCB
            #include <xcb/xcb.h>

            #include <vulkan/vulkan_xcb.h>
        #endif
    #elif defined(STORMKIT_OS_MACOS)
        #include <vulkan/vulkan_macos.h>
    #elif defined(STORMKIT_OS_WIN32)
        #include <vulkan/vulkan_win32.h>
    #endif
#endif

#include <vulkan/vulkan.hpp>

#include <storm/render/core/vk_mem_alloc.h>

#include <storm/core/Configure.hpp>
#include <storm/core/Math.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/Ranges.hpp>

#include <storm/log/LogHandler.hpp>

#include <storm/render/core/Fwd.hpp>

#undef VK_NULL_HANDLE
#define VK_NULL_HANDLE nullptr

namespace storm::render {
    template<typename T, typename Deleter>
    class VkRAII: core::NonCopyable {
      public:
        explicit VkRAII(Deleter &&deleter, const Device &device) noexcept
            : m_device { &device }, m_deleter { std::forward<Deleter>(deleter) } {}
        explicit VkRAII(T handle, Deleter &&deleter, const Device &device) noexcept
            : m_handle { handle }, m_device { &device }, m_deleter { std::forward<Deleter>(
                                                             deleter) } {}
        ~VkRAII() noexcept {
            if (m_handle != VK_NULL_HANDLE) m_deleter(m_handle, m_device);
        }

        VkRAII(VkRAII &&rhs) noexcept
            : m_handle { rhs.m_handle }, m_device { std::move(rhs.m_device) }, m_deleter {
                  std::move(rhs.m_deleter)
              } {
            rhs.m_handle = VK_NULL_HANDLE;
        }
        inline VkRAII &operator=(VkRAII &&rhs) noexcept {
            if (&rhs == this) return *this;

            m_handle  = rhs.m_handle;
            m_device  = std::move(rhs.m_device);
            m_deleter = std::move(rhs.m_deleter);

            rhs.m_handle = VK_NULL_HANDLE;

            return *this;
        }

        inline void reset(T handle) {
            if (m_handle) m_deleter(m_handle, m_device);

            m_handle = handle;
        }

        inline T &handle() noexcept { return m_handle; }

        inline const T &handle() const noexcept { return m_handle; }

        inline operator T() noexcept { return handle(); }

        inline operator const T() const noexcept { return handle(); }

        inline operator bool() const noexcept { return m_handle != VK_NULL_HANDLE; }

      private:
        T m_handle = VK_NULL_HANDLE;
        DeviceConstPtr m_device;
        Deleter m_deleter;
    };

#define DECLARE_VK_RAII(X)                                                   \
    using X##Deleter = std::function<void(X, const DeviceConstPtr &device)>; \
    using RAII##X    = VkRAII<X, X##Deleter>;

    DECLARE_VK_RAII(VmaAllocator)
    DECLARE_VK_RAII(VmaAllocation)

#undef DECLARE_VK_RAII

    using RAIIVkAccelerationStructureNV =
        vk::UniqueHandle<vk::AccelerationStructureNV, vk::DispatchLoaderDynamic>;
    using RAIIVkBuffer         = vk::UniqueHandle<vk::Buffer, vk::DispatchLoaderDynamic>;
    using RAIIVkBufferView     = vk::UniqueHandle<vk::BufferView, vk::DispatchLoaderDynamic>;
    using RAIIVkCommandBuffer  = vk::UniqueHandle<vk::CommandBuffer, vk::DispatchLoaderDynamic>;
    using RAIIVkCommandPool    = vk::UniqueHandle<vk::CommandPool, vk::DispatchLoaderDynamic>;
    using RAIIVkDescriptorPool = vk::UniqueHandle<vk::DescriptorPool, vk::DispatchLoaderDynamic>;
    using RAIIVkDescriptorSet  = vk::UniqueHandle<vk::DescriptorSet, vk::DispatchLoaderDynamic>;
    using RAIIVkDescriptorSetLayout =
        vk::UniqueHandle<vk::DescriptorSetLayout, vk::DispatchLoaderDynamic>;
    using RAIIVkDescriptorUpdateTemplate =
        vk::UniqueHandle<vk::DescriptorUpdateTemplate, vk::DispatchLoaderDynamic>;
    using RAIIVkDeviceMemory   = vk::UniqueHandle<vk::DeviceMemory, vk::DispatchLoaderDynamic>;
    using RAIIVkEvent          = vk::UniqueHandle<vk::Event, vk::DispatchLoaderDynamic>;
    using RAIIVkFence          = vk::UniqueHandle<vk::Fence, vk::DispatchLoaderDynamic>;
    using RAIIVkFramebuffer    = vk::UniqueHandle<vk::Framebuffer, vk::DispatchLoaderDynamic>;
    using RAIIVkImage          = vk::UniqueHandle<vk::Image, vk::DispatchLoaderDynamic>;
    using RAIIVkImageView      = vk::UniqueHandle<vk::ImageView, vk::DispatchLoaderDynamic>;
    using RAIIVkPipeline       = vk::UniqueHandle<vk::Pipeline, vk::DispatchLoaderDynamic>;
    using RAIIVkPipelineCache  = vk::UniqueHandle<vk::PipelineCache, vk::DispatchLoaderDynamic>;
    using RAIIVkPipelineLayout = vk::UniqueHandle<vk::PipelineLayout, vk::DispatchLoaderDynamic>;
    using RAIIVkQueryPool      = vk::UniqueHandle<vk::QueryPool, vk::DispatchLoaderDynamic>;
    using RAIIVkRenderPass     = vk::UniqueHandle<vk::RenderPass, vk::DispatchLoaderDynamic>;
    using RAIIVkSampler        = vk::UniqueHandle<vk::Sampler, vk::DispatchLoaderDynamic>;
    using RAIIVkSamplerYcbcrConversion =
        vk::UniqueHandle<vk::SamplerYcbcrConversion, vk::DispatchLoaderDynamic>;
    using RAIIVkSemaphore    = vk::UniqueHandle<vk::Semaphore, vk::DispatchLoaderDynamic>;
    using RAIIVkShaderModule = vk::UniqueHandle<vk::ShaderModule, vk::DispatchLoaderDynamic>;
    using RAIIVkSwapchain    = vk::UniqueHandle<vk::SwapchainKHR, vk::DispatchLoaderDynamic>;
    using RAIIVkValidationCache =
        vk::UniqueHandle<vk::ValidationCacheEXT, vk::DispatchLoaderDynamic>;

    template<typename T>
    inline constexpr auto vkMakeVersion(T &&major, T &&minor, T &&patch) {
        return (((major) << 22u) | ((minor) << 12u) | (patch));
    }

    template<typename T>
    inline constexpr auto vkVersionMajor(T &&version) {
        return static_cast<std::remove_reference_t<T>>(static_cast<core::UInt32>(version >> 22u));
    }

    template<typename T>
    inline constexpr auto vkVersionMinor(T &&version) {
        return static_cast<std::remove_reference_t<T>>(
            static_cast<core::UInt32>((version >> 12u) & 0x3ffu));
    }

    template<typename T>
    inline constexpr auto vkVersionPatch(T &&version) {
        return static_cast<std::remove_reference_t<T>>(static_cast<core::UInt32>(version & 0xfffu));
    }

    static constexpr auto DEVICE_EXTENSIONS = std::array {
#if STORMKIT_ENABLE_WSI
        gsl::czstring<> { VK_KHR_SWAPCHAIN_EXTENSION_NAME },
#endif
            gsl::czstring<> { VK_KHR_MAINTENANCE1_EXTENSION_NAME },
    };

#ifdef STORMKIT_BUILD_DEBUG
    static constexpr auto VALIDATION_LAYERS = std::array {
        gsl::czstring<> { "VK_LAYER_KHRONOS_validation" },
    // "VK_LAYER_LUNARG_api_dump",
    #ifdef STORMKIT_OS_LINUX
            "VK_LAYER_MESA_overlay",
    #endif
    #if !defined(STORMKIT_OS_MACOS) && !STORMKIT_ENABLE_WAYLAND
            "VK_LAYER_LUNARG_monitor",
    #endif
    };
#else
    static constexpr auto VALIDATION_LAYERS   = std::array<gsl::czstring<>, 0> {};
#endif

    static constexpr auto VALIDATION_FEATURES =
        std::array { VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT,
                     VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT };

#if STORMKIT_ENABLE_WSI
    static constexpr auto INSTANCE_EXTENSIONS = std::array {
        gsl::czstring<> { VK_KHR_SURFACE_EXTENSION_NAME },
    #if defined(STORMKIT_OS_LINUX)
        #if STORMKIT_ENABLE_WAYLAND
            VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME,
        #endif
        #if STORMKIT_ENABLE_XCB
            VK_KHR_XCB_SURFACE_EXTENSION_NAME,
        #endif
    #elif defined(STORMKIT_OS_WINDOWS)
            VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
    #elif defined(STORMKIT_OS_MACOS)
            VK_MVK_MACOS_SURFACE_EXTENSION_NAME,
    #elif defined(STORMKIT_OS_IOS)
            VK_MVK_IOS_SURFACE_EXTENSION_NAME,
    #endif
    };
#else
    static constexpr auto INSTANCE_EXTENSIONS = std::array<gsl::czstring<>, 0> {};
#endif

    static constexpr auto STORMKIT_VERSION =
        vkMakeVersion(STORMKIT_MAJOR_VERSION, STORMKIT_MINOR_VERSION, STORMKIT_PATCH_VERSION);

#if defined(STORMKIT_BUILD_DEBUG) || defined(STORMKIT_ENABLE_VALIDATION_LAYERS)
    static constexpr auto ENABLE_VALIDATION = true;
#else
    static constexpr auto ENABLE_VALIDATION   = false;
#endif

    inline void checkVkError(vk::Result result,
                             std::string file,
                             std::string function,
                             core::UInt32 line) noexcept {
        if (result == vk::Result::eSuccess) return;
        using log::operator"" _module;

        const auto module = "Vulkan"_module;

        log::LogHandler::flog(
            module,
            "Vulkan function failed (return code: {})\nat {} in function {} at line {}",
            result,
            file,
            function,
            line);

        std::exit(EXIT_FAILURE);
    }

    inline core::UInt32 computeMipLevel(const core::Extentu extent) noexcept {
        return static_cast<core::UInt32>(
                   std::floor(std::log2(std::max(extent.width, extent.height)))) +
               1;
    }

#define CHECK_VK_ERROR_VALUE(line, v)                                            \
    auto _result = line;                                                         \
    checkVkError(_result.result, __FILE__, STORMKIT_CURRENT_FUNCTION, __LINE__); \
    auto v = std::move(_result.value);
#define CHECK_VK_ERROR(line)   \
    const auto _result = line; \
    checkVkError(_result, __FILE__, STORMKIT_CURRENT_FUNCTION, __LINE__);
} // namespace storm::render
