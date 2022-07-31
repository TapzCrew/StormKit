// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/PlatformMacro.hpp>

#include <stormkit/gpu/core/VulkanMacro.hpp>

#include <span>
#include <string>
#include <vector>

#include <stormkit/core/Strings.hpp>

#include <stormkit/gpu/core/DynamicLoader.hpp>
#include <stormkit/gpu/core/Enums.hpp>
#include <stormkit/gpu/core/Fwd.hpp>
#include <stormkit/gpu/core/Surface.hpp>
#include <stormkit/gpu/core/Vulkan.hpp>

#if STORMKIT_WSI_ENABLED
    #include <stormkit/wsi/Window.hpp>
#endif

namespace stormkit::gpu {
    class STORMKIT_PUBLIC Instance {
      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Instance;

        explicit Instance(std::string app_name = "");
        ~Instance();

        Instance(const Instance&)                    = delete;
        auto operator=(const Instance&) -> Instance& = delete;

        Instance(Instance&&) noexcept;
        auto operator=(Instance&&) noexcept -> Instance&;

#ifdef STORMKIT_WSI_ENABLED
        [[nodiscard]] auto
            createWindowSurface(const wsi::Window& window,
                                Surface::Buffering buffering = Surface::Buffering::Swapchain) const
            -> WindowSurface;
        [[nodiscard]] auto allocateWindowSurface(
            const wsi::Window& window,
            Surface::Buffering buffering = Surface::Buffering::Swapchain) const
            -> WindowSurfaceOwnedPtr;
        [[nodiscard]] auto allocateRefCountedWindowSurface(
            const wsi::Window& window,
            Surface::Buffering buffering = Surface::Buffering::Swapchain) const
            -> WindowSurfaceSharedPtr;
#endif

        [[nodiscard]] auto
            createOffscreenSurface(const core::ExtentU& extent,
                                   Surface::Buffering buffering = Surface::Buffering::Triple) const
            -> OffscreenSurface;
        [[nodiscard]] auto allocateOffscreenSurface(
            const core::ExtentU& extent,
            Surface::Buffering buffering = Surface::Buffering::Triple) const
            -> OffscreenSurfaceOwnedPtr;
        [[nodiscard]] auto allocateRefCountedOffscreenSurface(
            const core::ExtentU& extent,
            Surface::Buffering buffering = Surface::Buffering::Triple) const
            -> OffscreenSurfaceSharedPtr;

        [[nodiscard]] auto pickPhysicalDevice() const noexcept -> const PhysicalDevice&;
        [[nodiscard]] auto pickPhysicalDevice(const WindowSurface& surface) noexcept
            -> const PhysicalDevice&;

        [[nodiscard]] auto physicalDevices() const noexcept -> std::span<const PhysicalDevice>;

        [[nodiscard]] auto loader() const noexcept -> const DynamicLoader&;

        [[nodiscard]] operator VkInstance() const noexcept;
        [[nodiscard]] auto vkHandle() const noexcept -> VkInstance;

      private:
        auto createInstance() noexcept -> void;
        auto createDebugReportCallback() noexcept -> void;
        auto retrievePhysicalDevices() noexcept -> void;

        auto checkValidationLayerSupport(bool enable_validation) noexcept -> bool;
        auto checkExtensionSupport(std::string_view extention) const noexcept -> bool;
        auto checkExtensionSupport(std::span<const std::string_view> extentions) const noexcept
            -> bool;

        static inline bool s_has_instance = 0;

        DynamicLoader m_loader;

        VkInstance m_instance                = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT m_messenger = VK_NULL_HANDLE;

        std::vector<PhysicalDevice> m_physical_devices;

        std::string m_app_name;

        std::vector<std::string> m_extensions;
    };
} // namespace stormkit::gpu

#include "Instance.inl"