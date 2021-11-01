// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/Math.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/Span.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/Fwd.hpp>
#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Vulkan.hpp>

#include <storm/render/resource/Texture.hpp>

namespace storm::render {
    class STORMKIT_PUBLIC Surface: public core::NonCopyable {
      public:
        static constexpr auto DEBUG_TYPE = DebugObjectType::Surface;

        enum class Buffering { Simple, Double, Triple, Swapchain };

        struct Frame {
            core::UInt32 current_frame;
            core::UInt32 texture_index;

            SemaphoreConstPtr texture_available;
            SemaphoreConstPtr render_finished;
            FencePtr in_flight;
        };

        Surface(const Instance &instance, Buffering buffering = Buffering::Triple);
        virtual ~Surface() = 0;

        Surface(Surface &&);
        Surface &operator=(Surface &&);

        virtual void initialize(const Device &device) = 0;
        virtual void recreate()                       = 0;
        virtual void destroy()                        = 0;

        virtual Frame acquireNextFrame()         = 0;
        virtual void present(const Frame &frame) = 0;

        [[nodiscard]] storm::core::span<Texture> textures() noexcept;
        [[nodiscard]] storm::core::span<const Texture> textures() const noexcept;

        [[nodiscard]] PixelFormat pixelFormat() const noexcept;
        [[nodiscard]] core::UInt32 bufferingCount() const noexcept;
        [[nodiscard]] core::UInt32 textureCount() const noexcept;

        [[nodiscard]] bool needRecreate() const noexcept;

        [[nodiscard]] const core::Extentu &extent() const noexcept;

        std::function<void(const Fence &fence)> onSwapchainFenceSignaled;

      protected:
        InstanceConstPtr m_instance;
        DeviceConstPtr m_device        = nullptr;
        QueueConstPtr m_graphics_queue = nullptr;

        std::vector<vk::Image> m_vk_images;

        core::Extentu m_extent;
        PixelFormat m_pixel_format = PixelFormat::Undefined;
        core::UInt32 m_image_count = 1;

        Buffering m_buffering;
        core::UInt32 m_buffering_count;

        TextureArray m_textures;

        SemaphoreArray m_texture_availables;
        SemaphoreArray m_render_finisheds;
        FenceArray m_in_flight_fences;

        bool m_need_recreate = false;

        core::UInt32 m_current_frame = 0;
    };
} // namespace storm::render

#include "Surface.inl"
