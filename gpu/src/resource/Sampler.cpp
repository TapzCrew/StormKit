// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/////////// - StormKit::render - ///////////
#include <stormkit/gpu/core/Device.mpp>
#include <stormkit/gpu/resource/Sampler.mpp>

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    Sampler::Sampler(Settings settings, const Device &device)
        : m_device { &device }, m_settings { std::move(settings) } {
        const auto &vk = m_device->table();

        const auto create_info = VkSamplerCreateInfo {
            .sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .magFilter               = core::as<VkFilter>(m_settings.mag_filter),
            .minFilter               = core::as<VkFilter>(m_settings.min_filter),
            .mipmapMode              = core::as<VkSamplerMipmapMode>(m_settings.mipmap_mode),
            .addressModeU            = core::as<VkSamplerAddressMode>(m_settings.address_mode_u),
            .addressModeV            = core::as<VkSamplerAddressMode>(m_settings.address_mode_v),
            .addressModeW            = core::as<VkSamplerAddressMode>(m_settings.address_mode_w),
            .mipLodBias              = m_settings.mip_lod_bias,
            .anisotropyEnable        = m_settings.enable_anisotropy,
            .maxAnisotropy           = m_settings.max_anisotropy,
            .compareEnable           = m_settings.compare_enable,
            .compareOp               = core::as<VkCompareOp>(m_settings.compare_operation),
            .minLod                  = m_settings.min_lod,
            .maxLod                  = m_settings.max_lod,
            .borderColor             = core::as<VkBorderColor>(m_settings.border_color),
            .unnormalizedCoordinates = m_settings.unnormalized_coordinates,
        };

        CHECK_VK_ERROR(vk.vkCreateSampler(*m_device, &create_info, nullptr, &m_sampler));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Sampler::~Sampler() {
        if (m_sampler != VK_NULL_HANDLE) [[likely]] {
            const auto &vk = m_device->table();

            vk.vkDestroySampler(*m_device, m_sampler, nullptr);
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    Sampler::Sampler(Sampler &&other) noexcept
        : m_device { std::exchange(other.m_device, nullptr) },
          m_settings { std::exchange(other.m_settings, {}) }, m_sampler {
              std::exchange(other.m_sampler, VK_NULL_HANDLE)
          } {}

    /////////////////////////////////////
    /////////////////////////////////////
    auto Sampler::operator=(Sampler &&other) noexcept -> Sampler & {
        if (&other == this) [[unlikely]]
            return *this;

        m_device   = std::exchange(other.m_device, nullptr);
        m_settings = std::exchange(other.m_settings, {});
        m_sampler  = std::exchange(other.m_sampler, VK_NULL_HANDLE);

        return *this;
    }
} // namespace stormkit::gpu
