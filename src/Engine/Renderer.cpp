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
        auto chooseSwapSurfaceFormat(std::span<const gpu::SurfaceFormat> formats) noexcept
            -> gpu::SurfaceFormat {
            for (const auto& format : formats) {
                if (format.format == gpu::PixelFormat::BGRA8_UNorm &&
                    format.color_space == gpu::ColorSpace::SRGB_NonLinear)
                    return format;
            }

            return formats[0];
        }

        /////////////////////////////////////
        /////////////////////////////////////
        auto chooseSwapPresentMode(std::span<const gpu::PresentMode> present_modes) noexcept
            -> gpu::PresentMode {
            for (const auto& present_mode : present_modes) {
                if (present_mode == gpu::PresentMode::Immediate) return present_mode;
                else if (present_mode == gpu::PresentMode::Mailbox)
                    return present_mode;
            }

            return gpu::PresentMode::Fifo;
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
            .and_then(std::bind_front(&Renderer::doInitDevice, this))
            .and_then([this, window = std::move(window)] {
                return doInitRenderSurface(std::move(window));
            });
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
    auto Renderer::doInitRenderSurface(std::optional<core::NakedRef<const wsi::Window>>) noexcept
        -> gpu::Expected<void> {
        return {};
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto Renderer::threadLoop(std::stop_token token) noexcept -> void {
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
