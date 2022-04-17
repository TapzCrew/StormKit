// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/////////// - StormKit::core - ///////////
#include <stormkit/core/Format.mpp>

#include <stormkit/render/core/CommandBuffer.mpp>
#include <stormkit/render/core/Device.mpp>
#include <stormkit/render/core/Enums.mpp>
#include <stormkit/render/core/Instance.mpp>
#include <stormkit/render/core/OffscreenSurface.mpp>
#include <stormkit/render/core/PhysicalDevice.mpp>
#include <stormkit/render/core/Queue.mpp>

#include <stormkit/render/sync/Fence.mpp>
#include <stormkit/render/sync/Semaphore.mpp>

namespace stormkit::render {
    /////////////////////////////////////
    /////////////////////////////////////
    OffscreenSurface::OffscreenSurface(const core::ExtentU &extent,
                                       const render::Instance &instance,
                                       Buffering buffering)
        : Surface { instance, buffering } {
        m_extent = extent;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    OffscreenSurface::~OffscreenSurface() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    OffscreenSurface::OffscreenSurface(OffscreenSurface &&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto OffscreenSurface::operator=(OffscreenSurface &&) noexcept -> OffscreenSurface & = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto OffscreenSurface::initialize(const Device &device) -> void {
        m_device         = &device;
        m_graphics_queue = &(device.graphicsQueue());

        m_pixel_format = PixelFormat::RGBA8_UNorm;

        STORMKIT_EXPECTS_MESSAGE(m_buffering != Buffering::Swapchain,
                                 "Offscreen surface don't support Swapchain image count");

        switch (m_buffering) {
            case Buffering::Simple: m_buffering_count = m_image_count = 1u; break;
            case Buffering::Double: m_buffering_count = m_image_count = 2u; break;
            case Buffering::Triple: m_buffering_count = m_image_count = 3u; break;
            default: break;
        }

        auto command_buffer = m_device->graphicsQueue().createCommandBuffer();
        auto fence          = m_device->createFence();

        command_buffer.begin(true);
        for (auto i = 0u; i < m_buffering_count; ++i) {
            auto &image = m_images.emplace_back(device.createImage(
                { m_extent,
                  m_pixel_format,
                  1u,
                  1u,
                  ImageType::T2D,
                  ImageCreateFlag::None,
                  SampleCountFlag::C1,
                  ImageUsageFlag::Color_Attachment | ImageUsageFlag::Transfert_Src |
                      ImageUsageFlag::Sampled }));
            command_buffer.transitionImageLayout(image,
                                                 ImageLayout::Undefined,
                                                 ImageLayout::Color_Attachment_Optimal);

            m_vk_images.emplace_back(image);

            m_image_availables.emplace_back(device.createSemaphore());
            m_render_finisheds.emplace_back(device.createSemaphore());
            m_in_flight_fences.emplace_back(device.createFence());

            m_device->setObjectName(image, core::format("StormKit:OffscreenImage ({})", i));
            m_device->setObjectName(m_image_availables.back(),
                                    core::format("StormKit:ImageAvailableSemaphore ({})", i));
            m_device->setObjectName(m_render_finisheds.back(),
                                    core::format("StormKit:RenderFinishedSemaphore ({})", i));
            m_device->setObjectName(m_in_flight_fences.back(),
                                    core::format("StormKit:InFlightFence ({})", i));
        }

        command_buffer.end();

        command_buffer.submit({}, {}, &fence);

        fence.wait();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto OffscreenSurface::recreate() -> void {
        STORMKIT_EXPECTS(m_device != nullptr);

        m_need_recreate = false;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto OffscreenSurface::destroy() -> void {
        m_images.clear();

        m_image_availables.clear();
        m_render_finisheds.clear();
        m_in_flight_fences.clear();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto OffscreenSurface::acquireNextFrame() -> Frame {
        const auto &image_available = m_image_availables[m_current_frame];
        const auto &render_finished = m_render_finisheds[m_current_frame];
        auto &in_flight             = m_in_flight_fences[m_current_frame];

        in_flight.wait();
        in_flight.reset();

        return Frame { .current_frame   = m_current_frame,
                       .image_index     = m_current_frame,
                       .image_available = &image_available,
                       .render_finished = &render_finished,
                       .in_flight       = &in_flight };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto OffscreenSurface::present(const Frame &frame) -> void {
        m_current_frame = (m_current_frame + 1);
        if (m_current_frame >= m_buffering_count) m_current_frame -= m_buffering_count;
    }
} // namespace stormkit::render
