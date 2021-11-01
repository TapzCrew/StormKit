// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/////////// - STL - ///////////
#include <fstream>

/////////// - StormKit::render - ///////////
#include <storm/render/core/Device.hpp>
#include <storm/render/core/Enums.hpp>
#include <storm/render/resource/Sampler.hpp>

using namespace storm;
using namespace storm::render;

/////////////////////////////////////
/////////////////////////////////////
Sampler::Sampler(Settings settings, const render::Device &device)
    : m_device { &device }, m_settings { std::move(settings) } {
    const auto create_info = vk::SamplerCreateInfo {}
                                 .setMagFilter(toVK(m_settings.mag_filter))
                                 .setMinFilter(toVK(m_settings.min_filter))
                                 .setMipmapMode(toVK(m_settings.mipmap_mode))
                                 .setAddressModeU(toVK(m_settings.address_mode_u))
                                 .setAddressModeV(toVK(m_settings.address_mode_v))
                                 .setAddressModeW(toVK(m_settings.address_mode_w))
                                 .setMipLodBias(m_settings.mip_lod_bias)
                                 .setAnisotropyEnable(m_settings.enable_anisotropy)
                                 .setMaxAnisotropy(m_settings.max_anisotropy)
                                 .setCompareEnable(m_settings.compare_enable)
                                 .setCompareOp(toVK(m_settings.compare_operation))
                                 .setMinLod(m_settings.min_lod)
                                 .setMaxLod(m_settings.max_lod)
                                 .setBorderColor(toVK(m_settings.border_color))
                                 .setUnnormalizedCoordinates(m_settings.unnormalized_coordinates);

    m_vk_sampler = m_device->createVkSampler(create_info);
}

/////////////////////////////////////
/////////////////////////////////////
Sampler::~Sampler() = default;

/////////////////////////////////////
/////////////////////////////////////
Sampler::Sampler(Sampler &&) = default;

/////////////////////////////////////
/////////////////////////////////////
Sampler &Sampler::operator=(Sampler &&) = default;
