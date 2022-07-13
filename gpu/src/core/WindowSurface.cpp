// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "../Log.mpp"

#if STORMKIT_WSI_ENABLED
    #include <stormkit/wsi/Window.mpp>
#endif

#include <stormkit/core/Format.mpp>

#include <stormkit/gpu/core/CommandBuffer.mpp>
#include <stormkit/gpu/core/Device.mpp>
#include <stormkit/gpu/core/Enums.mpp>
#include <stormkit/gpu/core/Instance.mpp>
#include <stormkit/gpu/core/PhysicalDevice.mpp>
#include <stormkit/gpu/core/Queue.mpp>
#include <stormkit/gpu/core/WindowSurface.mpp>

#include <stormkit/gpu/sync/Fence.mpp>
#include <stormkit/gpu/sync/Semaphore.mpp>

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    WindowSurface::WindowSurface(const wsi::Window& window,
                                 const gpu::Instance& instance,
                                 Surface::Buffering buffering)
        : Surface { instance, buffering }, m_window { &window } {
#if STORMKIT_WSI_ENABLED
    #if defined(STORMKIT_OS_WINDOWS)
        const auto create_info =
            VkWin32SurfaceCreateInfoKHR { .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
                                          .flags = {},
                                          .hinstance = GetModuleHandleW(nullptr),
                                          .hwnd =
                                              reinterpret_cast<HWND>(m_window->nativeHandle()) };

        CHECK_VK_ERROR(
            vkCreateWin32SurfaceKHR(this->instance(), &create_info, nullptr, &m_surface));
    #elif defined(STORMKIT_OS_MACOS)
        const auto create_info = VkMacOSSurfaceCreateInfoMVK {
            .sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK, .pView =
                                                                          m_window->nativeHandle();
        };
        CHECK_VK_ERROR(
            vkCreateMacOSSurfaceMVK(this->instance(), &create_info, nullptr, &m_surface));
    #elif defined(STORMKIT_OS_LINUX)
        #if STORMKIT_WSI_BUILD_WAYLAND
        const auto make_wayland_surface = [&instance = this->instance(),
                                           &surface  = m_surface,
                                           &window   = *m_window]() {
            struct Handles {
                wl_display *display;
                wl_surface *surface;
            } *handles = reinterpret_cast<Handles *>(window.nativeHandle());

            const auto create_info = VkWaylandSurfaceCreateInfoKHR {
                .sType   = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,
                .display = handles->display,
                .surface = handles->surface
            };

            auto create =
                instance.loader().get<PFN_vkCreateWaylandSurfaceKHR>("vkCreateWaylandSurfaceKHR");
            CHECK_VK_ERROR(create(instance, &create_info, nullptr, &surface));
        };
        #endif
        #if STORMKIT_WSI_BUILD_X11
        const auto make_xcb_surface = [&instance = this->instance(),
                                       &surface  = m_surface,
                                       &window   = *m_window]() {
            struct Handles {
                xcb_connection_t *connection;
                xcb_window_t window;
            } *handles = reinterpret_cast<Handles *>(window.nativeHandle());

            const auto create_info =
                VkXcbSurfaceCreateInfoKHR { .sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,
                                            .connection = handles->connection,
                                            .window     = handles->window };

            auto create = instance.loader().get<PFN_vkCreateXcbSurfaceKHR>("vkCreateXcbSurfaceKHR");
            CHECK_VK_ERROR(create(instance, &create_info, nullptr, &surface));
        };
        #endif

        #if STORMKIT_WSI_BUILD_WAYLAND && STORMKIT_WSI_BUILD_X11
        auto is_wayland = std::getenv("WAYLAND_DISPLAY") != nullptr;

        if (is_wayland) {
            make_wayland_surface();
        } else {
            make_xcb_surface();
        }
        #elif STORMKIT_ENABLE_WAYLAND
        make_wayland_surface();
        #elif STORMKIT_ENABLE_XCB
        make_xcb_surface();
        #endif

    #elif defined(STORMKIT_OS_IOS)
        const auto create_info =
            VkIOSSurfaceCreateInfoMVK { .sType = VK_STRUCTURE_TYPE_IOS_SURFACE_CREATE_INFO_MVK,
                                        .pView = m_window->nativeHandle() };
        CHECK_VK_ERROR(vkCreateIOSSurfaceMVK(this->instance(), &create_info, &m_surface));
    #endif
#else
        elog("WSI disabled in this build");
#endif
    };

    /////////////////////////////////////
    /////////////////////////////////////
    WindowSurface::~WindowSurface() {
#if STORMKIT_WSI_ENABLED
        if (!m_initialized) [[unlikely]]
            return;

        auto& in_flight = m_in_flight_fences[m_current_frame];
        onSwapchainFenceSignaled(in_flight);

        destroy();
#else
        elog("WSI disabled in this build");
#endif
    }

    /////////////////////////////////////
    /////////////////////////////////////
    WindowSurface::WindowSurface(WindowSurface&& other) noexcept
        : Surface { std::move(other) }, m_window { std::exchange(other.m_window, nullptr) },
          m_initialized { std::exchange(other.m_initialized, false) },
          m_exclusive_fullscreen_enabled { std::exchange(other.m_exclusive_fullscreen_enabled,
                                                         false) },
          m_surface { std::exchange(other.m_surface, VK_NULL_HANDLE) },
          m_swapchain { std::exchange(other.m_swapchain, VK_NULL_HANDLE) }, m_old_swapchain {
              std::exchange(other.m_old_swapchain, VK_NULL_HANDLE)
          } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowSurface::operator=(WindowSurface&& other) noexcept -> WindowSurface& {
        if (&other == this) [[unlikely]]
            return *this;

        Surface::operator=(std::move(other));

        m_device                       = std::exchange(other.m_device, nullptr);
        m_window                       = std::exchange(other.m_window, nullptr);
        m_initialized                  = std::exchange(other.m_initialized, false);
        m_surface                      = std::exchange(other.m_surface, VK_NULL_HANDLE);
        m_swapchain                    = std::exchange(other.m_swapchain, VK_NULL_HANDLE);
        m_old_swapchain                = std::exchange(other.m_old_swapchain, VK_NULL_HANDLE);
        m_exclusive_fullscreen_enabled = std::exchange(other.m_exclusive_fullscreen_enabled, false);

        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowSurface::initialize(const Device& device) -> void {
#if STORMKIT_WSI_ENABLED
        m_device         = &device;
        m_graphics_queue = &device.graphicsQueue();

        createSwapchain();

        switch (m_buffering) {
            case Buffering::Simple: m_buffering_count = 1u; break;
            case Buffering::Double: m_buffering_count = std::min(2u, m_image_count); break;
            case Buffering::Triple: m_buffering_count = std::min(3u, m_image_count); break;
            case Buffering::Swapchain: m_buffering_count = m_image_count; break;
        }

        for (auto i : core::range(m_buffering_count)) {
            m_image_availables.emplace_back(device.createSemaphore());
            m_render_finisheds.emplace_back(device.createSemaphore());
            m_in_flight_fences.emplace_back(device.createFence(true));

            m_device->setObjectName(m_image_availables.back(),
                                    core::format("StormKit:ImageAvailableSemaphore ({})", i));
            m_device->setObjectName(m_render_finisheds.back(),
                                    core::format("StormKit:RenderFinishedSemaphore ({})", i));
            m_device->setObjectName(m_in_flight_fences.back(),
                                    core::format("StormKit:InFlightFence ({})", i));
        }

        m_device->setObjectName(*this, "StormKit:WindowWindowSurface");

        m_initialized = true;
#else
        elog("WSI disabled in this build");
#endif
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowSurface::recreate() -> void {
#if STORMKIT_WSI_ENABLED
        STORMKIT_EXPECTS(m_device);
        STORMKIT_EXPECTS(m_surface);
        STORMKIT_EXPECTS(m_swapchain);

        m_need_recreate = false;

        m_device->waitIdle();
        createSwapchain();
#else
        elog("WSI disabled in this build");
#endif
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowSurface::destroy() -> void {
#if STORMKIT_WSI_ENABLED
        m_images.clear();

        m_image_availables.clear();
        m_render_finisheds.clear();
        m_in_flight_fences.clear();

        destroySwapchain();

        if (m_surface != VK_NULL_HANDLE) [[likely]] {
            vkDestroySurfaceKHR(instance(), m_surface, nullptr);
            m_surface = VK_NULL_HANDLE;
        }

        m_initialized = false;
#else
        elog("WSI disabled in this build");
#endif
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowSurface::setExclusiveFullscreenEnabled([[maybe_unused]] bool enabled) noexcept
        -> void {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowSurface::acquireNextFrame() -> Expected<Frame> {
#if STORMKIT_WSI_ENABLED
        STORMKIT_EXPECTS(m_device);
        STORMKIT_EXPECTS(m_surface);
        STORMKIT_EXPECTS(m_swapchain);
        const auto& vk = m_device->table();

        const auto& image_available = m_image_availables[m_current_frame];
        const auto& render_finished = m_render_finisheds[m_current_frame];
        auto& in_flight             = m_in_flight_fences[m_current_frame];

        if (const auto result = in_flight.wait(std::chrono::milliseconds { 1000 });
            result != Result::Success) {
            elog("Failed to acquire frame {} fence, reason: {}",
                 m_current_frame,
                 to_string(result));

            return core::makeUnexpected(result);
        }

        onSwapchainFenceSignaled(in_flight);
        in_flight.reset();

        const auto image_index = [&] {
            auto index  = core::UInt32 { 0 };
            auto result = vk.vkAcquireNextImageKHR(*m_device,
                                                   m_swapchain,
                                                   std::chrono::nanoseconds::max().count(),
                                                   image_available,
                                                   VK_NULL_HANDLE,
                                                   &index);

            if (result == VK_ERROR_OUT_OF_DATE_KHR) [[unlikely]] {
                wlog("Out of date Swapchain, need to recreate");
                m_need_recreate = true;

                result = VK_SUCCESS;
            } else if (result == VK_SUBOPTIMAL_KHR) [[unlikely]] {
                wlog("Suboptimal Swapchain, need to recreate");
                m_need_recreate = true;

                result = VK_SUCCESS;
            }

            CHECK_VK_ERROR(result);

            return index;
        }();

        return Frame { .current_frame   = m_current_frame,
                       .image_index     = image_index,
                       .image_available = &image_available,
                       .render_finished = &render_finished,
                       .in_flight       = &in_flight };
#else
        elog("WSI disabled in this build");
        return Frame { 0, 0, nullptr, nullptr, nullptr };
#endif
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowSurface::present(const gpu::WindowSurface::Frame& frame) -> void {
#if STORMKIT_WSI_ENABLED
        STORMKIT_EXPECTS(m_device);
        STORMKIT_EXPECTS(m_surface);
        STORMKIT_EXPECTS(m_swapchain);
        const auto& vk = device().table();

        const auto vk_wait_semaphore = frame.render_finished->vkHandle();

        const auto present_info = VkPresentInfoKHR { .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                                                     .waitSemaphoreCount = 1,
                                                     .pWaitSemaphores    = &vk_wait_semaphore,
                                                     .swapchainCount     = 1,
                                                     .pSwapchains        = &m_swapchain,
                                                     .pImageIndices      = &frame.image_index,
                                                     .pResults           = nullptr };

        auto result = vk.vkQueuePresentKHR(m_device->graphicsQueue(), &present_info);
        if (result == VK_ERROR_OUT_OF_DATE_KHR) [[unlikely]] {
            wlog("Out of date Swapchain, need to recreate");
            m_need_recreate = true;

            result = VK_SUCCESS;
        } else if (result == VK_SUBOPTIMAL_KHR) [[unlikely]] {
            wlog("Suboptimal Swapchain, need to recreate");
            m_need_recreate = true;

            result = VK_SUCCESS;
        }

        CHECK_VK_ERROR(result);

        m_current_frame = (m_current_frame + 1);
        if (m_current_frame >= m_buffering_count) m_current_frame -= m_buffering_count;
#else
        elog("WSI disabled in this build");
#endif
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowSurface::createSwapchain() -> void {
#if STORMKIT_WSI_ENABLED
        STORMKIT_EXPECTS(m_device);
        STORMKIT_EXPECTS(m_surface);

        const auto& vk = m_device->table();

        const auto& physical_device = m_device->physicalDevice();

        const auto capabilities = [&] {
            auto c = VkSurfaceCapabilitiesKHR {};

            CHECK_VK_ERROR(
                vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, m_surface, &c));

            return c;
        }();

        const auto formats = [&] {
            auto count = core::UInt32 { 0 };
            CHECK_VK_ERROR(
                vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, m_surface, &count, nullptr));
            auto f = std::vector<VkSurfaceFormatKHR> { count };
            CHECK_VK_ERROR(vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device,
                                                                m_surface,
                                                                &count,
                                                                std::data(f)));

            return f;
        }();

        const auto present_modes = [&] {
            auto count = core::UInt32 { 0 };
            CHECK_VK_ERROR(vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device,
                                                                     m_surface,
                                                                     &count,
                                                                     nullptr));
            auto p = std::vector<VkPresentModeKHR> { count };
            CHECK_VK_ERROR(vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device,
                                                                     m_surface,
                                                                     &count,
                                                                     std::data(p)));

            return p;
        }();

        const auto format             = chooseSwapSurfaceFormat(formats);
        const auto present_mode       = chooseSwapPresentMode(present_modes);
        const auto extent             = chooseSwapExtent(capabilities);
        const auto image_count        = chooseImageCount(capabilities);
        const auto image_sharing_mode = VkSharingMode { VK_SHARING_MODE_EXCLUSIVE };

        if (m_swapchain) {
            std::swap(m_old_swapchain, m_swapchain);
            m_images.clear();
        }

        const auto create_info = VkSwapchainCreateInfoKHR {
            .sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .surface          = m_surface,
            .minImageCount    = image_count,
            .imageFormat      = format.format,
            .imageColorSpace  = format.colorSpace,
            .imageExtent      = extent,
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
            .imageSharingMode = image_sharing_mode,
            .preTransform     = capabilities.currentTransform,
            .compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode      = present_mode,
            .clipped          = true,
            .oldSwapchain     = m_old_swapchain,
        };

        CHECK_VK_ERROR(vk.vkCreateSwapchainKHR(*m_device, &create_info, nullptr, &m_swapchain));
        m_old_swapchain = VK_NULL_HANDLE;

        m_vk_images = [&] {
            auto count = core::UInt32 { 0 };
            CHECK_VK_ERROR(vk.vkGetSwapchainImagesKHR(*m_device, m_swapchain, &count, nullptr));

            auto i = std::vector<VkImage> { count };
            CHECK_VK_ERROR(
                vk.vkGetSwapchainImagesKHR(*m_device, m_swapchain, &count, std::data(i)));

            return i;
        }();

        m_pixel_format = core::as<PixelFormat>(format.format);
        m_extent       = { extent.width, extent.height };

        m_images.reserve(std::size(m_vk_images));
        auto i = 0u;
        for (const auto& vk_image : m_vk_images) {
            auto& image = m_images.emplace_back(*m_device, m_extent, m_pixel_format, vk_image);

            m_device->setObjectName(image, core::format("StormKit:SwapchainImage ({})", i++));
        }

        m_image_count = core::as<core::UInt32>(std::size(m_images));

        auto transition_command_buffer = m_graphics_queue->createCommandBuffer();

        transition_command_buffer.begin(true);
        for (auto& image : m_images)
            transition_command_buffer.transitionImageLayout(image,
                                                            ImageLayout::Undefined,
                                                            ImageLayout::Present_Src);
        transition_command_buffer.end();

        auto fence = m_device->createFence();

        transition_command_buffer.submit({}, {}, &fence);

        const auto fences = core::makeConstRefStaticArray(fence);
        m_device->waitForFences(fences);

        m_device->setObjectName(reinterpret_cast<core::UInt64>(m_swapchain),
                                DebugObjectType::Swapchain,
                                "StormKit:Swapchain");
#else
        elog("WSI disabled in this build");
#endif
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowSurface::destroySwapchain() -> void {
#if STORMKIT_WSI_ENABLED
        auto& vk = m_device->table();

        if (m_old_swapchain != VK_NULL_HANDLE) [[unlikely]] {
            vk.vkDestroySwapchainKHR(*m_device, m_old_swapchain, nullptr);
            m_old_swapchain = VK_NULL_HANDLE;
        }

        if (m_swapchain != VK_NULL_HANDLE) [[likely]] {
            vk.vkDestroySwapchainKHR(*m_device, m_swapchain, nullptr);
            m_swapchain = VK_NULL_HANDLE;
        }
#else
        elog("WSI disabled in this build");
#endif
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto
        WindowSurface::chooseSwapSurfaceFormat(std::span<const VkSurfaceFormatKHR> formats) noexcept
        -> VkSurfaceFormatKHR {
#if STORMKIT_WSI_ENABLED
        for (const auto& format : formats) {
            if (format.format == VK_FORMAT_B8G8R8A8_UNORM &&
                format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
                return format;
        }

        return formats[0];
#else
        elog("WSI disabled in this build");
        return VkSurfaceFormatKHR {};
#endif
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowSurface::chooseSwapPresentMode(
        std::span<const VkPresentModeKHR> present_modes) noexcept -> VkPresentModeKHR {
#if STORMKIT_WSI_ENABLED
        auto present_mode_ = VkPresentModeKHR { VK_PRESENT_MODE_FIFO_KHR };

        for (const auto& present_mode : present_modes) {
            if (present_mode == VK_PRESENT_MODE_IMMEDIATE_KHR) return present_mode;
            else if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
                return present_mode;
        }

        return present_mode_;
#else
        elog("WSI disabled in this build");
        return VkPresentModeKHR { 0 };
#endif
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowSurface::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) noexcept
        -> VkExtent2D {
#if STORMKIT_WSI_ENABLED
        constexpr static auto int_max = std::numeric_limits<core::UInt32>::max();

        if (capabilities.currentExtent.width != int_max &&
            capabilities.currentExtent.height != int_max)
            return capabilities.currentExtent;

        auto actual_extent = VkExtent2D { m_window->size().width, m_window->size().height };
        actual_extent.width =
            std::max(capabilities.minImageExtent.width,
                     std::min(capabilities.maxImageExtent.width, actual_extent.width));
        actual_extent.height =
            std::max(capabilities.minImageExtent.height,
                     std::min(capabilities.maxImageExtent.height, actual_extent.height));

        return actual_extent;
#else
        elog("WSI disabled in this build");
        return { 0, 0 };
#endif
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto WindowSurface::chooseImageCount(const VkSurfaceCapabilitiesKHR& capabilities) noexcept
        -> core::UInt32 {
#if STORMKIT_WSI_ENABLED
        auto image_count = capabilities.minImageCount + 1;

        if (capabilities.maxImageCount > 0 && image_count > capabilities.maxImageCount)
            image_count = capabilities.maxImageCount;

        return image_count;
#else
        elog("WSI disabled in this build");
        return 0;
#endif
    }
} // namespace stormkit::gpu
