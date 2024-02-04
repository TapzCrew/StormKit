module;

#include <stormkit/Log/LogMacro.hpp>

module stormkit.Engine;

import std;

import stormkit.Core;
import stormkit.Log;
import stormkit.Wsi;
import stormkit.Gpu;

import :Renderer;

using namespace std::literals;

namespace stormkit::engine {
    LOGGER("stormkit.Renderer")

    /////////////////////////////////////
    /////////////////////////////////////
    auto Renderer::doInit(std::string_view application_name,
                          std::optional<core::NakedRef<const wsi::Window>>) noexcept
        -> gpu::Expected<void> {
        ilog("Initializing Renderer");
        return doInitInstance(application_name).and_then([this]() noexcept {
            return doInitDevice();
        });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Renderer::doInitInstance(std::string_view application_name) noexcept
        -> gpu::Expected<void> {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Renderer::doInitDevice() noexcept -> gpu::Expected<void> {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Renderer::threadLoop(std::stop_token token) noexcept -> void {
        //    const auto create_info = vk::CommandPoolCreateInfo {}.setFlags(
        //        vk::CommandPoolCreateFlagBits::eTransient |
        //        vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
        //    auto command_pool = vk::raii::CommandPool { m_device, create_info };
        //
        //    const auto& images     = m_render_surface->images();
        //    auto&& command_buffers = createCommandBuffers(m_device, command_pool,
        //    std::size(images));
        //
        //    for (const auto i : core::range(std::size(images))) {
        //        const auto& image          = images[i];
        //        auto      & command_buffer = command_buffers[i];
        //
        //        command_buffer.begin(vk::CommandBufferBeginInfo {}.setFlags(
        //            vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
        //
        //        transitionImageLayout(command_buffer,
        //                              image,
        //                              vk::ImageLayout::eUndefined,
        //                              vk::ImageLayout::ePresentSrcKHR);
        //
        //        command_buffer.end();
        //    }
        //
        //    auto fence = vk::raii::Fence { m_device,
        //                                   vk::FenceCreateInfo {}.setFlags(
        //                                       vk::FenceCreateFlagBits::eSignaled) };
        //
        //    submit(m_raster_queue, command_buffers, {}, {}, *fence);
        //
        // //        fence.wait();
        //
        //    for (;;) {
        //        if (token.stop_requested()) return;
        //
        //        m_render_surface->acquireNextFrame()
        //            .transform([this](auto&& frame) { m_render_surface->present(frame); })
        //            .transform_error([](auto&& error) {
        //                elog("Failed to acquire frame, reason: {}", error);
        //                return std::forward<decltype(error)>(error);
        //            });
        //    }
        //
        //    m_device->waitIdle();
    }
} // namespace stormkit::engine
