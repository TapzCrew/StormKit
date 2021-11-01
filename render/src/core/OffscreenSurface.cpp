// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/////////// - StormKit::core - ///////////
#include <storm/core/Format.hpp>

#include <storm/render/core/CommandBuffer.hpp>
#include <storm/render/core/Device.hpp>
#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Instance.hpp>
#include <storm/render/core/OffscreenSurface.hpp>
#include <storm/render/core/PhysicalDevice.hpp>
#include <storm/render/core/Queue.hpp>

#include <storm/render/resource/Texture.hpp>

#include <storm/render/sync/Fence.hpp>
#include <storm/render/sync/Semaphore.hpp>

using namespace storm;
using namespace storm::render;

/////////////////////////////////////
/////////////////////////////////////
OffscreenSurface::OffscreenSurface(core::Extentu extent,
                                   const render::Instance &instance,
                                   Buffering buffering)
    : Surface { instance, buffering } {
    m_extent = std::move(extent);
}

/////////////////////////////////////
/////////////////////////////////////
OffscreenSurface::~OffscreenSurface() = default;

/////////////////////////////////////
/////////////////////////////////////
OffscreenSurface::OffscreenSurface(OffscreenSurface &&) = default;

/////////////////////////////////////
/////////////////////////////////////
OffscreenSurface &OffscreenSurface::operator=(OffscreenSurface &&) = default;

/////////////////////////////////////
/////////////////////////////////////
void OffscreenSurface::initialize(const render::Device &device) {
    m_device         = core::makeConstObserver(&device);
    m_graphics_queue = core::makeConstObserver(&device.graphicsQueue());

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
    auto fence = m_device->createFence();

    command_buffer.begin(true);
    for (auto i = 0u; i < m_buffering_count; ++i) {
        auto &texture = m_textures.emplace_back(
            device.createTexture(m_extent,
                                 m_pixel_format,
                                 1u,
                                 1u,
                                 TextureType::T2D,
                                 TextureCreateFlag::None,
                                 SampleCountFlag::C1_BIT,
                                 TextureUsage::Color_Attachment | TextureUsage::Transfert_Src |
                                     TextureUsage::Sampled));
        command_buffer.transitionTextureLayout(texture, TextureLayout::Undefined, TextureLayout::Color_Attachment_Optimal);

        m_vk_images.emplace_back(texture);

        m_texture_availables.emplace_back(device.createSemaphore());
        m_render_finisheds.emplace_back(device.createSemaphore());
        m_in_flight_fences.emplace_back(device.createFence());

        m_device->setObjectName(texture, core::format("StormKit:OffscreenImage ({})", i));
        m_device->setObjectName(m_texture_availables.back(),
                                core::format("StormKit:TextureAvailableSemaphore ({})", i));
        m_device->setObjectName(m_render_finisheds.back(),
                                core::format("StormKit:RenderFinishedSemaphore ({})", i));
        m_device->setObjectName(m_in_flight_fences.back(),
                                core::format("StormKit:InFlightFence ({})", i));
    }
    command_buffer.end();
    command_buffer.build();

    command_buffer.submit({}, {}, &fence);

    fence.wait();
}

/////////////////////////////////////
/////////////////////////////////////
void OffscreenSurface::recreate() {
    STORMKIT_EXPECTS(m_device != nullptr);

    m_need_recreate = false;
}

/////////////////////////////////////
/////////////////////////////////////
void OffscreenSurface::destroy() {
    m_textures.clear();

    m_texture_availables.clear();
    m_render_finisheds.clear();
    m_in_flight_fences.clear();
}

/////////////////////////////////////
/////////////////////////////////////
render::Surface::Frame OffscreenSurface::acquireNextFrame() {
    const auto &texture_available = m_texture_availables[m_current_frame];
    const auto &render_finished   = m_render_finisheds[m_current_frame];
    auto &in_flight               = m_in_flight_fences[m_current_frame];

    in_flight.wait();
    in_flight.reset();

    return Frame { .current_frame     = m_current_frame,
                   .texture_index     = m_current_frame,
                   .texture_available = core::makeObserver(texture_available),
                   .render_finished   = core::makeObserver(render_finished),
                   .in_flight         = core::makeObserver(in_flight) };
}

/////////////////////////////////////
/////////////////////////////////////
void OffscreenSurface::present(const render::OffscreenSurface::Frame &frame) {
    m_current_frame = (m_current_frame + 1);
    if (m_current_frame >= m_buffering_count) m_current_frame -= m_buffering_count;
}
