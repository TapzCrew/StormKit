// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Platform.hpp>
#include <storm/core/Span.hpp>
#include <storm/core/Strings.hpp>

#if STORMKIT_ENABLE_WSI
    #include <storm/window/Window.hpp>
#else
namespace storm::window {
    class Window;

    DECLARE_PTR_AND_REF(Window);
} // namespace storm::window

namespace vk {
    struct Win32SurfaceCreateInfoKHR;
    struct MacOSSurfaceCreateInfoMVK;
}
#endif

#include <storm/render/core/DynamicLoader.hpp>
#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>
#include <storm/render/core/Surface.hpp>
#include <storm/render/core/Vulkan.hpp>

namespace storm::render {
    class STORMKIT_PUBLIC Instance {
      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Instance;

        Instance(VkInstance instance);
        explicit Instance(std::string app_name = "");
        ~Instance();

        Instance(Instance &&);
        Instance &operator=(Instance &&);

        WindowSurface
            createWindowSurface(const window::Window &window,
                                Surface::Buffering buffering = Surface::Buffering::Swapchain) const;
        WindowSurfaceOwnedPtr createWindowSurfacePtr(
            const window::Window &window,
            Surface::Buffering buffering = Surface::Buffering::Swapchain) const;
        OffscreenSurface
            createOffscreenSurface(core::Extentu extent,
                                   Surface::Buffering buffering = Surface::Buffering::Triple) const;
        OffscreenSurfaceOwnedPtr createOffscreenSurfacePtr(
            core::Extentu,
            Surface::Buffering buffering = Surface::Buffering::Triple) const;

        const render::PhysicalDevice &pickPhysicalDevice() const noexcept;
        const render::PhysicalDevice &pickPhysicalDevice(const WindowSurface &surface) noexcept;

        inline core::span<const PhysicalDeviceOwnedPtr> physicalDevices();

        inline const DynamicLoader &loader() const noexcept;
        inline vk::Instance vkInstance() const noexcept;
        inline operator vk::Instance() const noexcept;
        inline vk::Instance vkHandle() const noexcept;
        inline core::UInt64 vkDebugHandle() const noexcept;

#if defined(STORMKIT_OS_WINDOWS)
        vk::UniqueSurfaceKHR
            createVkSurface(const vk::Win32SurfaceCreateInfoKHR &create_info) const noexcept;
#elif defined(STORMKIT_OS_MACOS)
        vk::UniqueSurfaceKHR
            createVkSurface(const vk::MacOSSurfaceCreateInfoMVK &create_info) const noexcept;
#elif defined(STORMKIT_OS_LINUX)
    #if STORMKIT_ENABLE_XCB
        vk::UniqueSurfaceKHR
            createVkSurface(const vk::XcbSurfaceCreateInfoKHR &create_info) const noexcept;
    #endif
    #if STORMKIT_ENABLE_WAYLAND
        vk::UniqueSurfaceKHR
            createVkSurface(const vk::WaylandSurfaceCreateInfoKHR &create_info) const noexcept;
    #endif
#elif defined(STORMKIT_OS_IOS)
        vk::UniqueSurfaceKHR
            createVkSurface(const vk::IOSSurfaceCreateInfoMVK &create_info) const noexcept;
#endif

      private:
        void createInstance() noexcept;
        void createDebugReportCallback();
        void retrievePhysicalDevices() noexcept;

        bool checkValidationLayerSupport(bool enable_validation) noexcept;
        bool checkExtensionSupport(const gsl::czstring<> extention) const noexcept;
        bool checkExtensionSupport(core::span<const gsl::czstring<>> extentions) const noexcept;

        std::vector<PhysicalDeviceOwnedPtr> m_physical_devices;

        DynamicLoader m_loader;
        std::variant<vk::UniqueInstance, vk::Instance> m_vk_instance;
        vk::UniqueDebugUtilsMessengerEXT m_vk_messenger;

        std::string m_app_name;

        std::vector<std::string> m_extensions;
    };
} // namespace storm::render

#include "Instance.inl"
