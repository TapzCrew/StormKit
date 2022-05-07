// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/Math.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/Span.hpp>

#if STORMKIT_ENABLE_WSI
    #include <storm/window/Fwd.hpp>
#else
namespace storm::window {
    class Window;

    DECLARE_PTR_AND_REF(Window);
} // namespace storm::window
#endif

#include <storm/render/Fwd.hpp>
#include <storm/render/core/Surface.hpp>

namespace storm::render {
    class STORMKIT_PUBLIC WindowSurface final: public Surface {
      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Surface;

        WindowSurface(const window::Window &window,
                      const Instance &instance,
                      Buffering buffering = Buffering::Swapchain);
        ~WindowSurface() override;

        WindowSurface(WindowSurface &&);
        WindowSurface &operator=(WindowSurface &&);

        void initialize(const Device &device) override;
        void recreate() override;
        void destroy() override;

        Frame acquireNextFrame() override;
        void present(const Frame &frame) override;

        [[nodiscard]] vk::SurfaceKHR vkSurface() const noexcept;
        [[nodiscard]] operator vk::SurfaceKHR() const noexcept;
        [[nodiscard]] vk::SurfaceKHR vkHandle() const noexcept;
        [[nodiscard]] core::UInt64 vkDebugHandle() const noexcept;

      private:
        void createSwapchain();
        void destroySwapchain();

        vk::SurfaceFormatKHR
            chooseSwapSurfaceFormat(storm::core::span<const vk::SurfaceFormatKHR> formats) noexcept;
        vk::PresentModeKHR chooseSwapPresentMode(
            storm::core::span<const vk::PresentModeKHR> present_modes) noexcept;
        vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities) noexcept;
        core::UInt32 chooseImageCount(const vk::SurfaceCapabilitiesKHR &capabilities) noexcept;

        window::WindowConstPtr m_window;

        vk::UniqueSurfaceKHR m_vk_surface;
        RAIIVkSwapchain m_vk_swapchain;
        RAIIVkSwapchain m_vk_old_swapchain;
    };
} // namespace storm::render

#include "WindowSurface.inl"
