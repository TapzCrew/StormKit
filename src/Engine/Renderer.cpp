module;

#include <stormkit/Log/LogMacro.hpp>

module stormkit.Engine;

import std;

import stormkit.Core;
import stormkit.Log;
import stormkit.Wsi;
import stormkit.Gpu;

import :Renderer;
import :Renderer.FrameGraph;

using namespace std::literals;

namespace stormkit::engine {
    LOGGER("stormkit.Renderer")

    namespace {
        constexpr auto RAYTRACING_EXTENSIONS =
            std::array { "VK_KHR_ray_tracing_pipeline"sv,  "VK_KHR_acceleration_structure"sv,
                         "VK_KHR_buffer_device_address"sv, "VK_KHR_deferred_host_operations"sv,
                         "VK_EXT_descriptor_indexing"sv,   "VK_KHR_spirv_1_4"sv,
                         "VK_KHR_shader_float_controls"sv };

        constexpr auto BASE_EXTENSIONS = std::array { "VK_KHR_maintenance3"sv };

        constexpr auto SWAPCHAIN_EXTENSIONS = std::array { "VK_KHR_swapchain"sv };

        /////////////////////////////////////
        /////////////////////////////////////
        auto scorePhysicalDevice(const gpu::PhysicalDevice& physical_device) -> core::UInt64 {
            const auto support_raytracing =
                physical_device.checkExtensionSupport(RAYTRACING_EXTENSIONS);

            auto score = core::UInt64 { 0u };

            const auto& info         = physical_device.info();
            const auto& capabilities = physical_device.capabilities();

            if (info.type == gpu::PhysicalDeviceType::Discrete_GPU) score += 10000000u;
            else if (info.type == gpu::PhysicalDeviceType::Virtual_GPU)
                score += 5000000u;
            else if (info.type == gpu::PhysicalDeviceType::Integrated_GPU)
                score += 250000u;

            score += capabilities.limits.max_image_dimension_1D;
            score += capabilities.limits.max_image_dimension_2D;
            score += capabilities.limits.max_image_dimension_3D;
            score += capabilities.limits.max_image_dimension_cube;
            score += capabilities.limits.max_uniform_buffer_range;
            score += info.api_major_version * 10000000u;
            score += info.api_minor_version * 10000u;
            score += info.api_patch_version * 100u;

            if (support_raytracing) score += 10000000u;

            return score;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        auto pickPhysicalDevice(std::span<const gpu::PhysicalDevice> physical_devices) noexcept
            -> std::optional<core::NakedRef<const gpu::PhysicalDevice>> {
            auto ranked_devices =
                std::multimap<core::UInt64, core::NakedRef<const gpu::PhysicalDevice>> {};

            for (const auto& physical_device : physical_devices) {
                if (not physical_device.checkExtensionSupport(BASE_EXTENSIONS)) {
                    dlog("Base required extensions not supported for GPU {}", physical_device);
                    continue;
                }
                if (not physical_device.checkExtensionSupport(SWAPCHAIN_EXTENSIONS)) {
                    dlog("Swapchain required extensions not supported for GPU {}", physical_device);
                    continue;
                }

                const auto& info = physical_device.info();

                dlog("Scoring for {}\n"
                     "    device id:      {:#06x}\n"
                     "    vendor name:    {}\n"
                     "    vendor id:      {}\n"
                     "    api version:    {}.{}.{}\n"
                     "    driver version: {}.{}.{}\n"
                     "    type:           {}",
                     info.device_name,
                     info.device_id,
                     info.vendor_name,
                     info.vendor_id,
                     info.api_major_version,
                     info.api_minor_version,
                     info.api_patch_version,
                     info.driver_major_version,
                     info.driver_minor_version,
                     info.driver_patch_version,
                     info.type);

                const auto score = scorePhysicalDevice(physical_device);

                dlog("Score is {}", score);

                ranked_devices.emplace(score, physical_device);
            }

            if (std::ranges::empty(ranked_devices)) return std::nullopt;

            return ranked_devices.rbegin()->second;
        }

        /////////////////////////////////////
        /////////////////////////////////////
        // auto
        //     chooseSwapExtent(const core::math::ExtentU&      extent,
        //                      const gpu::SurfaceCapabilities& capabilities) noexcept ->
        //                      gpu::ExtentU {
        //     constexpr static auto int_max = std::numeric_limits<core::UInt32>::max();
        //
        //     if (capabilities.currentExtent.width != int_max &&
        //         capabilities.currentExtent.height != int_max)
        //         return capabilities.currentExtent;
        //
        //     auto actual_extent   = extent;
        //     actual_extent.width  = core::math::clamp(actual_extent.width,
        //                                             capabilities.minImageExtent.width,
        //                                             capabilities.maxImageExtent.width);
        //     actual_extent.height = core::math::clamp(actual_extent.height,
        //                                              capabilities.minImageExtent.height,
        //                                              capabilities.maxImageExtent.height);
        //
        //     return actual_extent;
        // }
        //
        // /////////////////////////////////////
        // /////////////////////////////////////
        // auto chooseImageCount(const gpu::SurfaceCapabilities& capabilities) noexcept
        //     -> core::UInt32 {
        //     auto image_count = capabilities.minImageCount + 1;
        //     return core::math::clamp(image_count,
        //                              capabilities.minImageCount,
        //                              capabilities.maxImageCount);
        // }
    } // namespace

    /////////////////////////////////////
    /////////////////////////////////////
    auto Renderer::doInit(std::string_view                                 application_name,
                          std::optional<core::NakedRef<const wsi::Window>> window) noexcept
        -> gpu::Expected<void> {
        ilog("Initializing Renderer");
        return doInitInstance(application_name)
            .and_then(core::curry(&Renderer::doInitDevice, this))
            .and_then(
                core::curry(gpu::Queue::create, std::cref(*m_device), m_device->rasterQueueEntry()))
            .transform(core::monadic::set(m_raster_queue))
            .and_then(core::curry(gpu::CommandPool::create,
                                  std::cref(*m_device),
                                  std::cref(*m_raster_queue)))
            .transform(core::monadic::set(m_main_command_pool))
            .and_then(core::curry(&Renderer::doInitRenderSurface, this, std::move(window)));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Renderer::doInitInstance(std::string_view application_name) noexcept
        -> gpu::Expected<void> {
        return gpu::Instance::create(std::string { application_name })
            .transform(core::monadic::set(m_instance));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Renderer::doInitDevice() noexcept -> gpu::Expected<void> {
        const auto& physical_devices = m_instance->physicalDevices();
        auto        physical_device =
            pickPhysicalDevice(physical_devices)
                .or_else(core::expectsWithMessage<core::NakedRef<const gpu::PhysicalDevice>>(
                    "No suitable GPU found !"))
                .value();

        ilog("Using physical device {}", *physical_device);

        return gpu::Device::create(*physical_device, *m_instance)
            .transform(core::monadic::set(m_device));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Renderer::doInitRenderSurface(
        std::optional<core::NakedRef<const wsi::Window>> window) noexcept -> gpu::Expected<void> {
        if (window)
            return RenderSurface::createFromWindow(*m_instance,
                                                   *m_device,
                                                   *m_raster_queue,
                                                   *(window.value()))
                .transform(core::monadic::set(m_surface));

        core::ensures(not window, "Offscreen rendering not yet implemented");
        return {};
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Renderer::threadLoop(std::stop_token token) noexcept -> void {
        // auto transition_command_buffers =
        //           raster_queue
        //
        // transition_command_buffer.begin(true);
        // for (auto& image : m_images)
        //     transition_command_buffer.transitionImageLayout(image,
        //                                                     ImageLayout::Undefined,
        //                                                     ImageLayout::Present_Src);
        // transition_command_buffer.end();
        //
        // auto fence = m_device->createFence();
        //
        // transition_command_buffer.submit({}, {}, &fence);
        //
        // const auto fences = core::makeConstRefStaticArray(fence);
        // m_device->waitForFences(fences);
        //    const auto create_info = gpu::CommandPoolCreateInfo {}.setFlags(
        //        gpu::CommandPoolCreateFlagBits::eTransient |
        //        gpu::CommandPoolCreateFlagBits::eResetCommandBuffer);
        //    auto command_pool = gpu::CommandPool { m_device, create_info };
        //
        //    const auto& images     = m_render_surface->images();
        //    auto&& command_buffers = createCommandBuffers(m_device, command_pool,
        //    std::size(images));
        //
        //    for (const auto i : core::range(std::size(images))) {
        //        const auto& image          = images[i];
        //        auto      & command_buffer = command_buffers[i];
        //
        //        command_buffer.begin(gpu::CommandBufferBeginInfo {}.setFlags(
        //            gpu::CommandBufferUsageFlagBits::eOneTimeSubmit));
        //
        //        transitionImageLayout(command_buffer,
        //                              image,
        //                              gpu::ImageLayout::eUndefined,
        //                              gpu::ImageLayout::ePresentSrc);
        //
        //        command_buffer.end();
        //    }
        //
        //    auto fence = gpu::Fence { m_device,
        //                                   gpu::FenceCreateInfo {}.setFlags(
        //                                       gpu::FenceCreateFlagBits::eSignaled) };
        //
        //    submit(m_raster_queue, command_buffers, {}, {}, *fence);
        //
        // //        fence.wait();
        //
        m_command_buffers =
            m_main_command_pool->createCommandBuffers(m_device, m_surface->bufferingCount());
        for (auto i : core::range(m_surface->bufferingCount()))
            gpu::Fence::createSignaled(m_device)
                .transform(core::monadic::emplaceTo(m_fences))
                .transform_error(core::expectsWithMessage("Failed to create swapchain fences"));

        for (;;) {
            if (token.stop_requested()) return;

            m_surface->beginFrame(m_device)
                .and_then(core::curry(&Renderer::doRender, this))
                .and_then(core::curry(&RenderSurface::presentFrame,
                                      &m_surface.get(),
                                      std::cref(*m_raster_queue)))
                .transform_error(core::expectsWithMessage("Failed to render frame"));
        }

        m_device->waitIdle();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Renderer::doRender(RenderSurface::Frame&& frame) noexcept
        -> gpu::Expected<RenderSurface::Frame> {
        auto&& command_buffer = m_command_buffers[frame.current_frame];
        auto&& fence          = m_fences[frame.current_frame];

        return fence.wait()
            .transform([&fence](auto&& _) noexcept { fence.reset(); })
            .transform([frame = std::move(frame), &command_buffer, &fence, this]() {
                command_buffer.reset();
                command_buffer.begin(true);
                command_buffer.end();
                command_buffer.submit(m_raster_queue,
                                      {},
                                      core::makeNakedRefs<std::array>(*frame.render_finished),
                                      fence);

                return frame;
            });
    }
} // namespace stormkit::engine
