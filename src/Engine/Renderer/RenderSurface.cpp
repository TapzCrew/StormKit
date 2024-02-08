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
                                 const wsi::Window&   window,
                                 Tag) {
        gpu::Surface::createFromWindow(instance, window)
            .transform(core::monadic::set(m_surface))
            .and_then(std::bind_front(&RenderSurface::doInitSwapchain, this))
            .transform_error(core::throwError());
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto RenderSurface::doInitSwapchain() noexcept -> gpu::Expected<gpu::Swapchain> {
        return {};
    }
} // namespace stormkit::engine