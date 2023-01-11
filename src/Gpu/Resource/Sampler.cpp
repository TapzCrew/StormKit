// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module stormkit.Gpu;

import std;

import stormkit.Core;

import :Core;

import :Resource.Image;

import vulkan;

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    Sampler::Sampler(Settings settings, const Device& device)
        : DeviceObject { device }, m_settings { std::move(settings) } {
        const auto create_info =
            vk::SamplerCreateInfo {}
                .setMagFilter(core::as<vk::Filter>(m_settings.mag_filter))
                .setMinFilter(core::as<vk::Filter>(m_settings.min_filter))
                .setMipmapMode(core::as<vk::SamplerMipmapMode>(m_settings.mipmap_mode))
                .setAddressModeU(core::as<vk::SamplerAddressMode>(m_settings.address_mode_u))
                .setAddressModeV(core::as<vk::SamplerAddressMode>(m_settings.address_mode_v))
                .setAddressModeW(core::as<vk::SamplerAddressMode>(m_settings.address_mode_w))
                .setMipLodBias(m_settings.mip_lod_bias)
                .setAnisotropyEnable(m_settings.enable_anisotropy)
                .setMaxAnisotropy(m_settings.max_anisotropy)
                .setCompareEnable(m_settings.compare_enable)
                .setCompareOp(core::as<vk::CompareOp>(m_settings.compare_operation))
                .setMinLod(m_settings.min_lod)
                .setMaxLod(m_settings.max_lod)
                .setBorderColor(core::as<vk::BorderColor>(m_settings.border_color))
                .setUnnormalizedCoordinates(m_settings.unnormalized_coordinates);

        try {
            m_vk_sampler = vk::raii::Sampler { this->device().vkHandle(), create_info };
        } catch (const vk::SystemError& err) {
            throw std::unexpected { core::as<Result>(err.code().value()) };
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Sampler::~Sampler() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    Sampler::Sampler(Sampler&& other) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto Sampler::operator=(Sampler&& other) noexcept -> Sampler& = default;
} // namespace stormkit::gpu
