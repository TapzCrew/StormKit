// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/PlatformMacro.hpp>

#include <stormkit/core/Memory.hpp>
#include <stormkit/core/Types.hpp>

#include <stormkit/gpu/core/DeviceObject.hpp>
#include <stormkit/gpu/core/Enums.hpp>
#include <stormkit/gpu/core/Fwd.hpp>
#include <stormkit/gpu/core/Vulkan.hpp>

namespace stormkit::gpu {
    class STORMKIT_PUBLIC Sampler: public DeviceObject {
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

        Sampler(Settings settings, const Device &device);
        ~Sampler();

        Sampler(const Sampler &) = delete;
        auto operator=(const Sampler &) -> Sampler & = delete;

        Sampler(Sampler &&) noexcept;
        auto operator=(Sampler &&) noexcept -> Sampler &;

        [[nodiscard]] auto settings() const noexcept -> const Settings &;

        [[nodiscard]] operator VkSampler() const noexcept;
        [[nodiscard]] auto vkHandle() const noexcept -> VkSampler;

      private:
        Settings m_settings = {};

        VkSampler m_sampler = VK_NULL_HANDLE;
    };
} // namespace stormkit::gpu

#include "Sampler.inl"