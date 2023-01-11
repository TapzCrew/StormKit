// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#ifdef STORMKIT_BUILD_MODULES
module stormkit.Gpu:Surface;
#else
    #include <stormkit/std.hpp>

    #include <stormkit/Core.hpp>
    #include <stormkit/Gpu.hpp>
#endif

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    OffscreenSurface::OffscreenSurface(const core::math::ExtentU& extent,
                                       const gpu::Instance& instance,
                                       Buffering buffering)
        : Surface { instance, buffering } {
        m_extent = extent;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    OffscreenSurface::~OffscreenSurface() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    OffscreenSurface::OffscreenSurface(OffscreenSurface&&) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto OffscreenSurface::operator=(OffscreenSurface&&) noexcept -> OffscreenSurface& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto OffscreenSurface::initialize(const Device& device) -> void {
        m_device = &device;

        auto builder = DeviceObjectBuilder { *m_device };

        m_raster_queue = builder.createQueue(m_device->rasterQueueEntry());

        m_pixel_format = PixelFormat::RGBA8_UNorm;

        core::expects(m_buffering != Buffering::Swapchain,
                      "Offscreen surface don't support Swapchain image count");

        switch (m_buffering) {
            case Buffering::Simple: m_buffering_count = m_image_count = 1u; break;
            case Buffering::Double: m_buffering_count = m_image_count = 2u; break;
            case Buffering::Triple: m_buffering_count = m_image_count = 3u; break;
            default: break;
        }

        auto pool          = CommandBufferPool { *m_device, *m_raster_queue };
        auto commandbuffer = pool.createCommandBuffer();
        auto fence         = builder.createFence();

        commandbuffer.begin(true);
        for (auto i : core::range(m_buffering_count)) {
            auto& image = m_images.emplace_back(builder.createImage(
                { m_extent,
                  m_pixel_format,
                  1u,
                  1u,
                  ImageType::T2D,
                  ImageCreateFlag::None,
                  SampleCountFlag::C1,
                  ImageUsageFlag::Color_Attachment | ImageUsageFlag::Transfert_Src |
                      ImageUsageFlag::Sampled }));
            commandbuffer.transitionImageLayout(image,
                                                ImageLayout::Undefined,
                                                ImageLayout::Color_Attachment_Optimal);

            m_vk_images.emplace_back(image);

            m_image_availables.emplace_back(builder.createSemaphore());
            m_render_finisheds.emplace_back(builder.createSemaphore());
            m_in_flight_fences.emplace_back(builder.createFence());

            m_device->setObjectName(image, std::format("StormKit:OffscreenImage ({})", i));
            m_device->setObjectName(m_image_availables.back(),
                                    std::format("StormKit:ImageAvailableSemaphore ({})", i));
            m_device->setObjectName(m_render_finisheds.back(),
                                    std::format("StormKit:RenderFinishedSemaphore ({})", i));
            m_device->setObjectName(m_in_flight_fences.back(),
                                    std::format("StormKit:InFlightFence ({})", i));
        }

        commandbuffer.end();

        commandbuffer.submit({}, {}, &fence);

        fence.wait();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto OffscreenSurface::recreate() -> void {
        core::expects(m_device != nullptr);

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
    auto OffscreenSurface::acquireNextFrame() -> Expected<Frame> {
        const auto& image_available = m_image_availables[m_current_frame];
        const auto& render_finished = m_render_finisheds[m_current_frame];
        auto& in_flight             = m_in_flight_fences[m_current_frame];

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
    auto OffscreenSurface::present([[maybe_unused]] const Frame& frame) -> void {
        m_current_frame = (m_current_frame + 1);
        if (m_current_frame >= m_buffering_count) m_current_frame -= m_buffering_count;
    }
} // namespace stormkit::gpu
