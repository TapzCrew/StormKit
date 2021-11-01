// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>

#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Fwd.hpp>
#include <storm/render/core/Vulkan.hpp>

namespace storm::render {
    class STORMKIT_PUBLIC Sampler: public core::NonCopyable {
      public:
        struct Settings {
            Filter mag_filter = Filter::Linear;
            Filter min_filter = Filter::Linear;

            SamplerAddressMode address_mode_u = SamplerAddressMode::Repeat;
            SamplerAddressMode address_mode_v = SamplerAddressMode::Repeat;
            SamplerAddressMode address_mode_w = SamplerAddressMode::Repeat;

            bool enable_anisotropy = false;
            float max_anisotropy   = 0.f;

            BorderColor border_color = BorderColor::Int_Opaque_Black;

            bool unnormalized_coordinates = false;

            bool compare_enable                = false;
            CompareOperation compare_operation = CompareOperation::Always;

            SamplerMipmapMode mipmap_mode = SamplerMipmapMode::Linear;
            float mip_lod_bias            = 0.f;

            float min_lod = 0.f;
            float max_lod = 0.f;
        };
        static constexpr auto DEBUG_TYPE = DebugObjectType::Sampler;

        explicit Sampler(Settings settings, const Device &device);
        ~Sampler();

        Sampler(Sampler &&);
        Sampler &operator=(Sampler &&);

        [[nodiscard]] inline const Settings &settings() const noexcept;

        inline vk::Sampler vkSampler() const noexcept;
        inline operator vk::Sampler() const noexcept;
        inline vk::Sampler vkHandle() const noexcept;
        inline core::UInt64 vkDebugHandle() const noexcept;

      private:
        DeviceConstPtr m_device;

        Settings m_settings;

        RAIIVkSampler m_vk_sampler;
    };
} // namespace storm::render

#include "Sampler.inl"
