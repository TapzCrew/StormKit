module stormkit.Engine;

import std;

import stormkit.Core;
import stormkit.Log;
import stormkit.Wsi;
import stormkit.Gpu;

import :Renderer;

using namespace std::chrono_literals;

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
                .transform(emplaceTo(m_render_finisheds))
                .and_then(curry(gpu::Fence::createSignaled, std::cref(device)))
                .transform(emplaceTo(m_in_flight_fences))
                .transform_error(map(narrow<gpu::Result>(), throwError()));
        }

        const auto command_pool =
            gpu::CommandPool::create(device, raster_queue)
                .transform_error(core::expectsWithMessage("Failed to raster Queue command pool"))
                .value();

        auto transition_command_buffers =
            command_pool.createCommandBuffers(device, std::size(m_swapchain->images()));

        for (auto i : core::range(std::size(transition_command_buffers))) {
            auto&& image                     = m_swapchain->images()[i];
            auto&& transition_command_buffer = transition_command_buffers[i];

            transition_command_buffer.begin(true);
            transition_command_buffer.transitionImageLayout(image,
                                                            gpu::ImageLayout::Undefined,
                                                            gpu::ImageLayout::Present_Src);
            transition_command_buffer.end();
        }

        auto fence = gpu::Fence::create(device)
                         .transform_error(map(narrow<gpu::Result>(), throwError()))
                         .value();

        auto cmbs = core::toNakedRefs(transition_command_buffers);
        raster_queue.submit(cmbs, {}, {}, fence);

        fence.wait().transform_error(map(narrow<gpu::Result>(), throwError()));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto RenderSurface::beginFrame(const gpu::Device& device) -> gpu::Expected<Frame> {
        core::expects(m_surface.initialized());
        core::expects(m_swapchain.initialized());

        const auto& image_available = m_image_availables[m_current_frame];
        const auto& render_finished = m_render_finisheds[m_current_frame];
        auto&       in_flight       = m_in_flight_fences[m_current_frame];

        return in_flight.wait()
            .transform([&in_flight](auto&& _) noexcept { in_flight.reset(); })
            .and_then(core::curry(&gpu::Swapchain::acquireNextImage,
                                  &(m_swapchain.get()),
                                  100ms,
                                  std::cref(image_available)))
            .transform([&, this](auto&& _result) noexcept {
                auto&& [result, image_index] = _result; // TODO handle result
                return Frame { .current_frame   = core::narrow<core::UInt32>(m_current_frame),
                               .image_index     = image_index,
                               .image_available = image_available,
                               .render_finished = render_finished,
                               .in_flight       = in_flight };
            });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto RenderSurface::presentFrame(const gpu::Queue& queue,
                                     const Frame&      frame) -> gpu::Expected<void> {
        const auto image_indices   = std::array { frame.image_index };
        const auto wait_semaphores = core::makeNakedRefs<std::array>(*frame.render_finished);
        const auto swapchains      = core::makeNakedRefs<std::array>(*m_swapchain);

        return queue.present(swapchains, wait_semaphores, image_indices)
            .transform([this](auto&& _) noexcept {
                if (++m_current_frame >= bufferingCount()) m_current_frame = 0;
            });
    }
} // namespace stormkit::engine
