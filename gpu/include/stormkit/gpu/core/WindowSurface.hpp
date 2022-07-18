// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/PlatformMacro.hpp>

#include <stormkit/core/Expected.hpp>
#include <stormkit/core/Math.hpp>
#include <stormkit/core/Types.hpp>

#if STORMKIT_WSI_ENABLED
    #include <stormkit/wsi/Fwd.hpp>
#else
namespace stormkit::wsi {
    class Window;

    DECLARE_PTR_AND_REF(Window);
} // namespace stormkit::wsi
#endif

#include <stormkit/gpu/core/Enums.hpp>
#include <stormkit/gpu/core/Fwd.hpp>
#include <stormkit/gpu/core/Surface.hpp>

namespace stormkit::gpu {
    class STORMKIT_PUBLIC WindowSurface final: public Surface {
      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Surface;

        WindowSurface(const wsi::Window &window,
                      const Instance &instance,
                      Buffering buffering = Buffering::Swapchain);
        ~WindowSurface() override;

        WindowSurface(const WindowSurface &) = delete;
        auto operator=(const WindowSurface &) -> WindowSurface & = delete;

        WindowSurface(WindowSurface &&) noexcept;
        auto operator=(WindowSurface &&) noexcept -> WindowSurface &;

        auto initialize(const Device &device) -> void override;
        auto recreate() -> void override;
        auto destroy() -> void override;

        auto setExclusiveFullscreenEnabled(bool enabled) noexcept -> void;

        [[nodiscard]] auto acquireNextFrame() -> Expected<Frame> override;
        auto present(const Frame &frame) -> void override;

        [[nodiscard]] operator VkSurfaceKHR() const noexcept;
        [[nodiscard]] auto vkHandle() const noexcept -> VkSurfaceKHR;

      private:
        auto createSwapchain() -> void;
        auto destroySwapchain() -> void;

        auto chooseSwapSurfaceFormat(std::span<const VkSurfaceFormatKHR> formats) noexcept
            -> VkSurfaceFormatKHR;
        auto chooseSwapPresentMode(std::span<const VkPresentModeKHR> present_modes) noexcept
            -> VkPresentModeKHR;
        auto chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) noexcept -> VkExtent2D;
        auto chooseImageCount(const VkSurfaceCapabilitiesKHR &capabilities) noexcept
            -> core::UInt32;

        const wsi::Window *m_window = nullptr;

        bool m_initialized                  = false;
        bool m_exclusive_fullscreen_enabled = false;

        VkSurfaceKHR m_surface         = VK_NULL_HANDLE;
        VkSwapchainKHR m_swapchain     = VK_NULL_HANDLE;
        VkSwapchainKHR m_old_swapchain = VK_NULL_HANDLE;
    };

    DECLARE_PTR_AND_REF(WindowSurface);
} // namespace stormkit::gpu

#include "WindowSurface.inl"
