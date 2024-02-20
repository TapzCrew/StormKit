module stormkit.Engine;

import std;

import stormkit.Core;
import stormkit.Log;
import stormkit.Wsi;
import stormkit.Gpu;

import :Renderer;

namespace stormkit::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    RenderSurface::RenderSurface(const gpu::Instance& instance,
                                 const gpu::Device&   device,
                                 const gpu::Queue&    raster_queue,
                                 const wsi::Window&   window,
                                 Tag) {
        gpu::Surface::createFromWindow(instance, window)
            .transform(core::monadic::set(m_surface))
            .and_then(core::curry(gpu::Swapchain::create,
                                  std::cref(device),
                                  std::cref(*m_surface),
                                  std::cref(window.extent()),
                                  std::nullopt))
            .transform(core::monadic::set(m_swapchain))
            .transform_error(core::throwError());

        using core::curry;
        using core::throwError;
        using core::monadic::emplaceTo;
        using core::monadic::map;
        using core::monadic::narrow;
        for (auto _ : core::range(std::size(m_swapchain->images()))) {
            gpu::Semaphore::create(device)
                .transform(emplaceTo(m_image_availables))
                .and_then(curry(gpu::Semaphore::create, std::cref(device)))
                .transform(emplaceTo(m_render_finished))
                .and_then(curry(gpu::Fence::createSignaled, std::cref(device)))
                .transform(emplaceTo(m_in_flight_fences))
                .transform_error(map(narrow<gpu::Result>(), throwError()));
        }

        const auto command_pool =
            gpu::CommandPool::create(device, raster_queue)
                .transform_error(core::expectsWithMessage("Failed to raster Queue command pool"))
                .value();

        auto transition_command_buffers =
            command_pool.createCommandBuffers(std::size(m_swapchain->images()));

        for(auto i : core::range(std::size(transition_command_buffers))) {
            auto &&image = m_swapchain->images()[i];
            auto &&transition_command_buffer = transition_command_buffers[i];

            transition_command_buffer.begin(true);
            transition_command_buffer.transitionImageLayout(image, gpu::ImageLayout::Undefined, gpu::ImageLayout::Present_Src);
            transition_command_buffer.end();
        }


        auto fence = gpu::Fence::create(device)
                         .transform_error(map(narrow<gpu::Result>(), throwError()))
                         .value();

        auto _foo = core::toNakedRefs(transition_command_buffers);
        raster_queue.submit(_foo, {}, {}, fence);

        fence.wait();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto RenderSurface::acquireNextFrame() -> gpu::Expected<Frame> {
        return {};
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto RenderSurface::present(const Frame& frame) -> void {
    }
} // namespace stormkit::engine
