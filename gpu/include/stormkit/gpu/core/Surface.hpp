// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/PlatformMacro.hpp>

#include <functional>

#include <stormkit/core/Math.hpp>
#include <stormkit/core/Types.hpp>

#include <stormkit/gpu/core/Enums.hpp>
#include <stormkit/gpu/core/Fwd.hpp>
#include <stormkit/gpu/core/InstanceObject.hpp>
#include <stormkit/gpu/core/Types.hpp>
#include <stormkit/gpu/core/Vulkan.hpp>

#include <stormkit/gpu/sync/Fence.hpp>
#include <stormkit/gpu/sync/Fwd.hpp>
#include <stormkit/gpu/sync/Semaphore.hpp>

#include <stormkit/gpu/resource/Fwd.hpp>
#include <stormkit/gpu/resource/Image.hpp>

namespace stormkit::gpu {
    class STORMKIT_PUBLIC Surface: public InstanceObject {
      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Surface;

        enum class Buffering { Simple, Double, Triple, Swapchain };

        struct Frame {
            core::UInt32 current_frame;
            core::UInt32 image_index;

            const Semaphore *image_available;
            const Semaphore *render_finished;
            Fence *in_flight;
        };

        Surface(const Instance &instance, Buffering buffering = Buffering::Triple);
        virtual ~Surface() = 0;

        Surface(const Surface &) = delete;
        auto operator=(const Surface &) -> Surface & = delete;

        Surface(Surface &&) noexcept;
        auto operator=(Surface &&) noexcept -> Surface &;

        virtual auto initialize(const Device &device) -> void = 0;
        virtual auto recreate() -> void                       = 0;
        virtual auto destroy() -> void                        = 0;

        virtual auto acquireNextFrame() -> Expected<Frame> = 0;
        virtual auto present(const Frame &frame) -> void   = 0;

        [[nodiscard]] auto images() noexcept -> std::span<Image>;
        [[nodiscard]] auto images() const noexcept -> std::span<const Image>;

        [[nodiscard]] auto pixelFormat() const noexcept -> PixelFormat;
        [[nodiscard]] auto bufferingCount() const noexcept -> core::UInt32;
        [[nodiscard]] auto imageCount() const noexcept -> core::UInt32;

        [[nodiscard]] auto needRecreate() const noexcept -> bool;

        [[nodiscard]] auto extent() const noexcept -> const core::ExtentU &;

        std::function<void(const Fence &fence)> onSwapchainFenceSignaled;

        [[nodiscard]] auto device() const noexcept -> const Device &;

      protected:
        const Device *m_device        = nullptr;
        const Queue *m_graphics_queue = nullptr;

        std::vector<VkImage> m_vk_images = {};

        core::ExtentU m_extent     = {};
        PixelFormat m_pixel_format = PixelFormat::Undefined;
        core::UInt32 m_image_count = 1;

        Buffering m_buffering          = Buffering::Swapchain;
        core::UInt32 m_buffering_count = 0;

        std::vector<Image> m_images = {};

        std::vector<Semaphore> m_image_availables = {};
        std::vector<Semaphore> m_render_finisheds = {};
        std::vector<Fence> m_in_flight_fences     = {};

        bool m_need_recreate = false;

        core::UInt32 m_current_frame = 0;
    };
} // namespace stormkit::gpu

#include "Surface.inl"