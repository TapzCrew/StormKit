// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "../Log.hpp"

#if STORMKIT_ENABLE_WSI
    #include <storm/window/Window.hpp>
#endif

/////////// - StormKit::core - ///////////
#include <storm/core/Format.hpp>

#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Instance.hpp>
#include <storm/render/core/PhysicalDevice.hpp>
#include <storm/render/core/Queue.hpp>
#include <storm/render/core/WindowSurface.hpp>

#include <storm/render/resource/Texture.hpp>

#include <storm/render/sync/Fence.hpp>
#include <storm/render/sync/Semaphore.hpp>

using namespace storm;
using namespace storm::render;

/////////////////////////////////////
/////////////////////////////////////
WindowSurface::WindowSurface(const window::Window &window,
                             const render::Instance &instance,
                             Surface::Buffering buffering)
    : Surface { instance, buffering }, m_window { &window } {
#if STORMKIT_ENABLE_WSI
    #if defined(STORMKIT_OS_WINDOWS)
    const auto create_info = vk::Win32SurfaceCreateInfoKHR {}
                                 .setHinstance(GetModuleHandleW(nullptr))
                                 .setHwnd(reinterpret_cast<HWND>(m_window->nativeHandle()));
    m_vk_surface = m_instance->createVkSurface(create_info);
    #elif defined(STORMKIT_OS_MACOS)
    const auto create_info = vk::MacOSSurfaceCreateInfoMVK {}.setPView(m_window->nativeHandle());
    m_vk_surface           = m_instance->createVkSurface(create_info);
    #elif defined(STORMKIT_OS_LINUX)
        #if STORMKIT_ENABLE_WAYLAND
    const auto make_wayland_surface = [this]() {
        struct Handles {
            wl_display *display;
            wl_surface *surface;
        } *handles = reinterpret_cast<Handles *>(m_window->nativeHandle());

        const auto create_info = vk::WaylandSurfaceCreateInfoKHR {}
                                     .setDisplay(handles->display)
                                     .setSurface(handles->surface);
        m_vk_surface = m_instance->createVkSurface(create_info);
    };
        #endif
        #if STORMKIT_ENABLE_XCB
    const auto make_xcb_surface = [this]() {
        struct Handles {
            xcb_connection_t *connection;
            xcb_window_t window;
        } *handles = reinterpret_cast<Handles *>(m_window->nativeHandle());

        const auto create_info = vk::XcbSurfaceCreateInfoKHR {}
                                     .setConnection(handles->connection)
                                     .setWindow(handles->window);
        m_vk_surface = m_instance->createVkSurface(create_info);
    };
        #endif

        #if STORMKIT_ENABLE_WAYLAND && STORMKIT_ENABLE_XCB
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
    const auto create_info = vk::IOSSurfaceCreateInfoMVK {}.setPView(m_window->nativeHandle());
    m_vk_surface           = m_instance->createVkSurface(create_info);
    #endif
#else
    elog("WSI disabled in this build");
#endif
};

/////////////////////////////////////
/////////////////////////////////////
WindowSurface::~WindowSurface() {
#if STORMKIT_ENABLE_WSI
    auto &in_flight = m_in_flight_fences[m_current_frame];
    onSwapchainFenceSignaled(in_flight);
#else
    elog("WSI disabled in this build");
#endif
}

/////////////////////////////////////
/////////////////////////////////////
WindowSurface::WindowSurface(WindowSurface &&) = default;

/////////////////////////////////////
/////////////////////////////////////
WindowSurface &WindowSurface::operator=(WindowSurface &&) = default;

/////////////////////////////////////
/////////////////////////////////////
void WindowSurface::initialize(const render::Device &device) {
#if STORMKIT_ENABLE_WSI
    m_device         = core::makeConstObserver(device);
    m_graphics_queue = core::makeConstObserver(device.graphicsQueue());

    createSwapchain();

    switch (m_buffering) {
        case Buffering::Simple: m_buffering_count = 1u; break;
        case Buffering::Double: m_buffering_count = std::min(2u, m_image_count); break;
        case Buffering::Triple: m_buffering_count = std::min(3u, m_image_count); break;
        case Buffering::Swapchain: m_buffering_count = m_image_count; break;
    }

    for (auto i = 0u; i < m_buffering_count; ++i) {
        m_texture_availables.emplace_back(device.createSemaphore());
        m_render_finisheds.emplace_back(device.createSemaphore());
        m_in_flight_fences.emplace_back(device.createFence());

        m_device->setObjectName(m_texture_availables.back(),
                                core::format("StormKit:TextureAvailableSemaphore ({})", i));
        m_device->setObjectName(m_render_finisheds.back(),
                                core::format("StormKit:RenderFinishedSemaphore ({})", i));
        m_device->setObjectName(m_in_flight_fences.back(),
                                core::format("StormKit:InFlightFence ({})", i));
    }

    m_device->setObjectName(*this, "StormKit:WindowWindowSurface");
#else
    elog("WSI disabled in this build");
#endif
}

/////////////////////////////////////
/////////////////////////////////////
void WindowSurface::recreate() {
#if STORMKIT_ENABLE_WSI
    STORMKIT_EXPECTS(m_device != nullptr);

    m_need_recreate = false;

    createSwapchain();
#else
    elog("WSI disabled in this build");
#endif
}

/////////////////////////////////////
/////////////////////////////////////
void WindowSurface::destroy() {
#if STORMKIT_ENABLE_WSI
    m_textures.clear();

    m_texture_availables.clear();
    m_render_finisheds.clear();
    m_in_flight_fences.clear();

    destroySwapchain();
#else
    elog("WSI disabled in this build");
#endif
}

/////////////////////////////////////
/////////////////////////////////////
render::WindowSurface::Frame WindowSurface::acquireNextFrame() {
#if STORMKIT_ENABLE_WSI
    const auto &texture_available = m_texture_availables[m_current_frame];
    const auto &render_finished   = m_render_finisheds[m_current_frame];
    auto &in_flight               = m_in_flight_fences[m_current_frame];

    const auto [result, texture_index] =
        m_device->vkAcquireNextImage(*m_vk_swapchain,
                                     std::numeric_limits<core::UInt64>::max(),
                                     texture_available,
                                     VK_NULL_HANDLE);

    in_flight.wait();
    onSwapchainFenceSignaled(in_flight);
    in_flight.reset();

    return Frame { .current_frame     = m_current_frame,
                   .texture_index     = texture_index,
                   .texture_available = core::makeConstObserver(texture_available),
                   .render_finished   = core::makeConstObserver(render_finished),
                   .in_flight         = core::makeObserver(in_flight) };
#else
    elog("WSI disabled in this build");
    return Frame { 0, 0, nullptr, nullptr, nullptr };
#endif
}

/////////////////////////////////////
/////////////////////////////////////
void WindowSurface::present(const render::WindowSurface::Frame &frame) {
#if STORMKIT_ENABLE_WSI
    auto vk_wait_semaphore = static_cast<const Semaphore &>(*frame.render_finished).vkSemaphore();

    const auto present_info = vk::PresentInfoKHR {}
                                  .setWaitSemaphoreCount(1)
                                  .setPWaitSemaphores(&vk_wait_semaphore)
                                  .setSwapchainCount(1)
                                  .setPSwapchains(&m_vk_swapchain.get())
                                  .setPImageIndices(&frame.texture_index)
                                  .setPResults(nullptr);

    auto result = m_device->graphicsQueue().vkPresent(present_info);

    if (result == vk::Result::eErrorOutOfDateKHR) {
        elog("Out of date Swapchain, need to recreate");
        m_need_recreate = true;
    } else if (result == vk::Result::eSuboptimalKHR) {
        dlog("Suboptimal Swapchain, need to recreate");
        m_need_recreate = true;
    }

    m_current_frame = (m_current_frame + 1);
    if (m_current_frame >= m_buffering_count) m_current_frame -= m_buffering_count;
#else
    elog("WSI disabled in this build");
#endif
}

/////////////////////////////////////
/////////////////////////////////////
void WindowSurface::createSwapchain() {
#if STORMKIT_ENABLE_WSI
    const auto &physical_device = m_device->physicalDevice();

    const auto capabilities  = physical_device.queryVkSurfaceCapabilities(*this);
    const auto formats       = physical_device.queryVkSurfaceFormats(*this);
    const auto present_modes = physical_device.queryVkPresentModes(*this);

    const auto format             = chooseSwapSurfaceFormat(formats);
    const auto present_mode       = chooseSwapPresentMode(present_modes);
    const auto extent             = chooseSwapExtent(capabilities);
    const auto image_count        = chooseImageCount(capabilities);
    const auto image_sharing_mode = vk::SharingMode::eExclusive;

    m_device->waitIdle();

    if (!m_vk_swapchain) {
        std::swap(m_vk_old_swapchain, m_vk_swapchain);
        m_textures.clear();
        m_vk_images.clear();
    }

    auto create_info = vk::SwapchainCreateInfoKHR {}
                           .setSurface(*m_vk_surface)
                           .setMinImageCount(image_count)
                           .setImageFormat(format.format)
                           .setImageColorSpace(format.colorSpace)
                           .setImageExtent(extent)
                           .setImageArrayLayers(1)
                           .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment |
                                          vk::ImageUsageFlagBits::eTransferDst)
                           .setImageSharingMode(image_sharing_mode)
                           .setPreTransform(capabilities.currentTransform)
                           .setPresentMode(present_mode)
                           .setClipped(true)
                           .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);

    if (m_vk_old_swapchain) create_info.setOldSwapchain(*m_vk_old_swapchain);

    m_vk_swapchain = m_device->createVkSwapchain(create_info);

    if (m_vk_old_swapchain) { m_vk_old_swapchain.reset(VK_NULL_HANDLE); }

    m_vk_images = m_device->getVkSwapchainImages(*m_vk_swapchain);

    m_pixel_format = fromVK(format.format);
    m_image_count  = gsl::narrow_cast<core::UInt32>(std::size(m_vk_images));
    m_extent       = { extent.width, extent.height };

    auto i = 0u;
    for (const auto &vk_image : m_vk_images) {
        auto &texture = m_textures.emplace_back(*m_device, m_extent, m_pixel_format, vk_image);

        m_device->setObjectName(texture, core::format("StormKit:SwapchainImage ({})", i++));
    }

    auto transition_command_buffer = m_graphics_queue->createCommandBuffer();

    transition_command_buffer.begin(true);
    for (auto &texture : m_textures)
        transition_command_buffer.transitionTextureLayout(texture,
                                                          TextureLayout::Undefined,
                                                          TextureLayout::Present_Src);
    transition_command_buffer.end();

    transition_command_buffer.build();

    auto fence = m_device->createFence();

    auto to_submit = core::makeConstObserverArray(transition_command_buffer);
    m_device->graphicsQueue().submit(to_submit, {}, {}, core::makeObserver(fence));

    auto fences = std::array { std::cref(fence) };
    m_device->waitForFences(fences);

    m_device->setObjectName(reinterpret_cast<core::UInt64>(
                                m_vk_swapchain.get().operator VkSwapchainKHR_T *()),
                            DebugObjectType::Swapchain,
                            "StormKit:Swapchain");
#else
    elog("WSI disabled in this build");
#endif
}

/////////////////////////////////////
/////////////////////////////////////
void WindowSurface::destroySwapchain() {
#if STORMKIT_ENABLE_WSI
    if (m_vk_swapchain) m_vk_swapchain.reset(VK_NULL_HANDLE);
#else
    elog("WSI disabled in this build");
#endif
}

/////////////////////////////////////
/////////////////////////////////////
vk::SurfaceFormatKHR WindowSurface::chooseSwapSurfaceFormat(
    storm::core::span<const vk::SurfaceFormatKHR> formats) noexcept {
#if STORMKIT_ENABLE_WSI
    for (const auto &format : formats) {
        if (format.format == vk::Format::eB8G8R8A8Unorm &&
            format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
            return format;
    }

    return formats[0];
#else
    elog("WSI disabled in this build");
    return vk::SurfaceFormatKHR {};
#endif
}

/////////////////////////////////////
/////////////////////////////////////
vk::PresentModeKHR WindowSurface::chooseSwapPresentMode(
    storm::core::span<const vk::PresentModeKHR> present_modes) noexcept {
#if STORMKIT_ENABLE_WSI
    auto present_mode_ = vk::PresentModeKHR::eFifo;

    for (const auto &present_mode : present_modes) {
        if (present_mode == vk::PresentModeKHR::eImmediate) return present_mode;
        if (present_mode == vk::PresentModeKHR::eMailbox) return present_mode;
    }

    return present_mode_;
#else
    elog("WSI disabled in this build");
    return vk::PresentModeKHR { 0 };
#endif
}

/////////////////////////////////////
/////////////////////////////////////
vk::Extent2D
    WindowSurface::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities) noexcept {
#if STORMKIT_ENABLE_WSI
    constexpr static auto int_max = std::numeric_limits<core::UInt32>::max();

    if (capabilities.currentExtent.width != int_max && capabilities.currentExtent.height != int_max)
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
core::UInt32
    WindowSurface::chooseImageCount(const vk::SurfaceCapabilitiesKHR &capabilities) noexcept {
#if STORMKIT_ENABLE_WSI
    auto image_count = capabilities.minImageCount + 1;

    if (capabilities.maxImageCount > 0 && image_count > capabilities.maxImageCount)
        image_count = capabilities.maxImageCount;

    return image_count;
#else
    elog("WSI disabled in this build");
    return 0;
#endif
}
