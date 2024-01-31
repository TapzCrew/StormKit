module;

#include <stormkit/Core/PlatformMacro.hpp>

#ifdef STORMKIT_OS_LINUX
    #include <wayland-client.h>
    #include <xcb/xcb.h>
#endif

module stormkit.Engine;

import std;
import vulkan_hpp;

import stormkit.Core;
import stormkit.Wsi;

import :Renderer.RenderSurface;
import :Vulkan.Utils;

namespace stormkit::engine {
    namespace {
        /////////////////////////////////////
        /////////////////////////////////////
        auto chooseSwapSurfaceFormat(std::span<const vk::SurfaceFormatKHR> formats) noexcept
            -> vk::SurfaceFormatKHR {
            for (const auto& format : formats) {
                if (format.format == vk::Format::eB8G8R8A8Unorm &&
                    format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
                    return format;
            }

            return formats[0];
        }

        /////////////////////////////////////
        /////////////////////////////////////
        auto chooseSwapPresentMode(std::span<const vk::PresentModeKHR> present_modes) noexcept
            -> vk::PresentModeKHR {
            for (const auto& present_mode : present_modes) {
                if (present_mode == vk::PresentModeKHR::eImmediate) return present_mode;
                else if (present_mode == vk::PresentModeKHR::eMailbox)
                    return present_mode;
            }

            return vk::PresentModeKHR::eFifo;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        auto chooseSwapExtent(const core::math::ExtentU       & extent,
                              const vk::SurfaceCapabilitiesKHR& capabilities) noexcept
            -> vk::Extent2D {
            constexpr static auto int_max = std::numeric_limits<core::UInt32>::max();

            if (capabilities.currentExtent.width != int_max &&
                capabilities.currentExtent.height != int_max)
                return capabilities.currentExtent;

            auto actual_extent   = vk::Extent2D { extent.width, extent.height };
            actual_extent.width  = core::math::clamp(actual_extent.width,
                                                    capabilities.minImageExtent.width,
                                                    capabilities.maxImageExtent.width);
            actual_extent.height = core::math::clamp(actual_extent.height,
                                                     capabilities.minImageExtent.height,
                                                     capabilities.maxImageExtent.height);

            return actual_extent;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        auto chooseImageCount(const vk::SurfaceCapabilitiesKHR& capabilities) noexcept
            -> core::UInt32 {
            auto image_count = capabilities.minImageCount + 1;
            return core::math::clamp(image_count,
                                     capabilities.minImageCount,
                                     capabilities.maxImageCount);
        }
    } // namespace

    /////////////////////////////////////
    /////////////////////////////////////
    auto RenderSurface::acquireNextFrame() noexcept -> VulkanExpected<Frame> {
        const auto& image_available = m_image_availables[m_current_frame];
        const auto& render_finished = m_render_finished[m_current_frame];
        auto      & in_flight       = m_in_flight_fences[m_current_frame];

        auto image_index = 0u;

        return Frame { core::as<core::UInt32>(m_current_frame),
                       image_index,
                       image_available,
                       render_finished,
                       in_flight };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto RenderSurface::present(const Frame& frame) const noexcept -> VulkanExpected<void> {
        return {};
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto RenderSurface::doInitSurface(
        const vk::raii::Instance                       & instance,
        std::optional<core::NakedRef<const wsi::Window>> window_opt) noexcept
        -> VulkanExpected<core::math::ExtentU> {
        core::expects(window_opt.has_value(), "Offscreen rendering not supported atm");

        const auto& window = *window_opt;

#if defined(STORMKIT_OS_LINUX)
        if (window->wm() == wsi::WM::X11) {
            struct Handles {
                xcb_connection_t *connection;
                xcb_window_t      window;
            };

            const auto handles     = std::bit_cast<const Handles *>(window->nativeHandle());
            const auto create_info = vk::XcbSurfaceCreateInfoKHR {}
                                         .setConnection(handles->connection)
                                         .setWindow(handles->window);

            try {
                m_surface = vk::raii::SurfaceKHR { instance, create_info };
            } catch (const vk::SystemError& err) {
                return std::unexpected { core::narrow<vk::Result>(err.code().value()) };
            }
        } else if (window->wm() == wsi::WM::Wayland) {
            struct Handles {
                wl_display *display;
                wl_surface *surface;
            };

            const auto handles     = std::bit_cast<const Handles *>(window->nativeHandle());
            const auto create_info = vk::WaylandSurfaceCreateInfoKHR {}
                                         .setDisplay(handles->display)
                                         .setSurface(handles->surface);

            try {
                m_surface = vk::raii::SurfaceKHR { instance, create_info };
            } catch (const vk::SystemError& err) {
                return std::unexpected { core::narrow<vk::Result>(err.code().value()) };
            }
        }
#else

        try {
            m_surface = vk::raii::SurfaceKHR { instance, create_info };
        } catch (const vk::SystemError& err) {
            return std::unexpected { core::narrow<vk::Result>(err.code().value()) };
        }
#endif

        return window->extent();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto RenderSurface::doInitSwapchain(const core::math::ExtentU     & extent,
                                        const vk::raii::PhysicalDevice& physical_device,
                                        const vk::raii::Device        & device) noexcept
        -> VulkanExpected<void> {
        constexpr auto buffering = Buffering::Swapchain;

        const auto capabilities  = physical_device.getSurfaceCapabilitiesKHR(*m_surface.get());
        const auto formats       = physical_device.getSurfaceFormatsKHR(*m_surface.get());
        const auto present_modes = physical_device.getSurfacePresentModesKHR(*m_surface.get());

        const auto format             = chooseSwapSurfaceFormat(formats);
        const auto present_mode       = chooseSwapPresentMode(present_modes);
        const auto swap_extent        = chooseSwapExtent(extent, capabilities);
        const auto image_count        = chooseImageCount(capabilities);
        const auto image_sharing_mode = vk::SharingMode::eExclusive;
        const auto pre_transform      = [&capabilities]() noexcept {
            if (checkFlag(capabilities.supportedTransforms,
                          vk::SurfaceTransformFlagBitsKHR::eIdentity))
                return vk::SurfaceTransformFlagBitsKHR::eIdentity;
            return capabilities.currentTransform;
        }();

        auto old_swapchain = vk::SwapchainKHR {};
        if (m_swapchain.initialized()) {
            auto old_swapchain = m_swapchain->release();
            m_images.clear();
        }

        const auto create_info = vk::SwapchainCreateInfoKHR {}
                                     .setSurface(*m_surface.get())
                                     .setMinImageCount(image_count)
                                     .setImageFormat(format.format)
                                     .setImageColorSpace(format.colorSpace)
                                     .setImageExtent(swap_extent)
                                     .setImageArrayLayers(1)
                                     .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment |
                                                    vk::ImageUsageFlagBits::eTransferDst)
                                     .setImageSharingMode(image_sharing_mode)
                                     .setPreTransform(pre_transform)
                                     .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
                                     .setPresentMode(present_mode)
                                     .setClipped(true)
                                     .setOldSwapchain(old_swapchain);

        try {
            m_swapchain = vk::raii::SwapchainKHR { device, create_info };

            m_images = m_swapchain->getImages();
        } catch (const vk::SystemError& err) {
            return std::unexpected { core::narrow<vk::Result>(err.code().value()) };
        }
        // switch (buffering) {
        //     case Buffering::Simple: m_buffering_count = 1u; break;
        //     case Buffering::Double: m_buffering_count = std::min(2u, m_image_count); break;
        //     case Buffering::Triple: m_buffering_count = std::min(3u, m_image_count); break;
        //     case Buffering::Swapchain: m_buffering_count = m_image_count; break;
        // }

        return {};
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto RenderSurface::doInitSynchronizationObjects(const vk::raii::Device& device) noexcept
        -> VulkanExpected<void> {
        for (const auto i : core::range(std::size(m_images))) {
            auto result =
                vkCreate<vk::raii::Semaphore>(device, vk::SemaphoreCreateInfo {})
                    .transform(core::emplaceTo(m_image_availables))
                    .and_then([&device] noexcept {
                        return vkCreate<vk::raii::Semaphore>(device, vk::SemaphoreCreateInfo {});
                    })
                    .transform(core::emplaceTo(m_render_finished))
                    .and_then([&device] noexcept {
                        return vkCreate<vk::raii::Fence>(device,
                                                         vk::FenceCreateInfo {}.setFlags(
                                                             vk::FenceCreateFlagBits::eSignaled));
                    })
                    .transform(core::emplaceTo(m_in_flight_fences));

            if (not result.has_value()) return std::unexpected { result.error() };
        }

        return {};
    }
} // namespace stormkit::engine
