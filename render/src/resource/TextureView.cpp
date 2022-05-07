// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/////////// - StormKit::render - ///////////
#include <storm/render/core/Device.hpp>

#include <storm/render/resource/Texture.hpp>
#include <storm/render/resource/TextureView.hpp>

using namespace storm;
using namespace storm::render;

/////////////////////////////////////
/////////////////////////////////////
TextureView::TextureView(const Device &device,
                         const Texture &texture,
                         TextureViewType type,
                         TextureSubresourceRange subresource_range)
    : m_device { &device }, m_texture { &texture }, m_type { type }, m_subresource_range {
          std::move(subresource_range)
      } {
    const auto vk_subresource_range = vk::ImageSubresourceRange {}
                                          .setAspectMask(toVK(m_subresource_range.aspect_mask))
                                          .setBaseMipLevel(m_subresource_range.base_mip_level)
                                          .setLevelCount(m_subresource_range.level_count)
                                          .setBaseArrayLayer(m_subresource_range.base_array_layer)
                                          .setLayerCount(m_subresource_range.layer_count);

    const auto components = vk::ComponentMapping {}
                                .setR(vk::ComponentSwizzle::eR)
                                .setG(vk::ComponentSwizzle::eG)
                                .setB(vk::ComponentSwizzle::eB)
                                .setA(vk::ComponentSwizzle::eA);

    const auto view_create_info = vk::ImageViewCreateInfo {}
                                      .setImage(m_texture->vkImage())
                                      .setViewType(toVK(m_type))
                                      .setFormat(toVK(m_texture->format()))
                                      .setComponents(std::move(components))
                                      .setSubresourceRange(std::move(vk_subresource_range));

    m_vk_image_view = m_device->createVkImageView(view_create_info);
}

/////////////////////////////////////
/////////////////////////////////////
TextureView::~TextureView() = default;

/////////////////////////////////////
/////////////////////////////////////
TextureView::TextureView(TextureView &&) = default;

/////////////////////////////////////
/////////////////////////////////////
TextureView &TextureView::operator=(TextureView &&) = default;
