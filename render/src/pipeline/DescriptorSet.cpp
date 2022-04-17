// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <stormkit/render/core/Device.mpp>

#include <stormkit/render/pipeline/DescriptorPool.mpp>
#include <stormkit/render/pipeline/DescriptorSet.mpp>

namespace stormkit::render {
    /////////////////////////////////////
    /////////////////////////////////////
    DescriptorSet::DescriptorSet(const DescriptorPool &pool,
                                 std::vector<DescriptorType> types,
                                 VkDescriptorSet set,
                                 Deleter deleter,
                                 Tag)
        : m_device { &pool.device() }, m_pool { &pool }, m_types { std::move(types) },
          m_descriptor_set { set }, m_deleter { std::move(deleter) } {}

    /////////////////////////////////////
    /////////////////////////////////////
    DescriptorSet::~DescriptorSet() {
        if (m_descriptor_set != VK_NULL_HANDLE) [[likely]]
            m_deleter(m_descriptor_set);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    DescriptorSet::DescriptorSet(DescriptorSet &&other) noexcept
        : m_device { std::exchange(other.m_device, nullptr) },
          m_pool { std::exchange(other.m_pool, nullptr) }, m_types { std::move(other.m_types) },
          m_descriptor_set { std::exchange(other.m_descriptor_set, VK_NULL_HANDLE) }, m_deleter {
              std::move(other.m_deleter)
          } {}

    /////////////////////////////////////
    /////////////////////////////////////
    auto DescriptorSet::operator=(DescriptorSet &&other) noexcept -> DescriptorSet & {
        if (&other == this) [[unlikely]]
            return *this;

        m_device         = std::exchange(other.m_device, nullptr);
        m_pool           = std::exchange(other.m_pool, nullptr);
        m_types          = std::move(other.m_types);
        m_descriptor_set = std::exchange(other.m_descriptor_set, VK_NULL_HANDLE);
        m_deleter        = std::move(other.m_deleter);

        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto DescriptorSet::update(std::span<const Descriptor> descriptors) -> void {
        STORMKIT_EXPECTS(m_descriptor_set);
        STORMKIT_EXPECTS(!std::empty(descriptors));

        const auto &vk = device().table();

        const auto [buffers, images] = [&] {
            auto buffers =
                core::HashMap<core::UInt32,
                              std::pair<std::vector<VkDescriptorBufferInfo>, VkDescriptorType>> {};
            auto images =
                core::HashMap<core::UInt32,
                              std::pair<std::vector<VkDescriptorImageInfo>, VkDescriptorType>> {};

            for (const auto &proxy : descriptors) {
                if (core::is<BufferDescriptor>(proxy)) {
                    const auto &descriptor = core::as<BufferDescriptor>(proxy);

                    auto &[infos, type] = buffers[descriptor.binding];
                    infos.emplace_back(VkDescriptorBufferInfo { .buffer = *descriptor.buffer,
                                                                .offset = descriptor.offset,
                                                                .range  = descriptor.range });
                    type = core::as<VkDescriptorType>(descriptor.type);
                }
                if (core::is<ImageDescriptor>(proxy)) {
                    const auto &descriptor = core::as<ImageDescriptor>(proxy);

                    auto &[infos, type] = images[descriptor.binding];
                    infos.emplace_back(VkDescriptorImageInfo {
                        .sampler =
                            (descriptor.sampler) ? descriptor.sampler->vkHandle() : VK_NULL_HANDLE,
                        .imageView   = *descriptor.image_view,
                        .imageLayout = core::as<VkImageLayout>(descriptor.layout),
                    });
                    type = core::as<VkDescriptorType>(descriptor.type);
                }
            }

            return std::make_pair(std::move(buffers), std::move(images));
        }();

        const auto write_infos = [&buffers = buffers, &images = images, this] {
            auto i = std::vector<VkWriteDescriptorSet> {};
            i.reserve(std::size(buffers) + std::size(images));

            for (const auto &[binding, data] : buffers) {
                const auto &[descriptors, type] = data;

                i.emplace_back(VkWriteDescriptorSet {
                    .dstSet          = m_descriptor_set,
                    .dstBinding      = binding,
                    .dstArrayElement = 0,
                    .descriptorCount = core::as<core::UInt32>(std::size(descriptors)),
                    .descriptorType  = type,
                    .pBufferInfo     = std::data(descriptors) });
            }
            for (const auto &[binding, data] : images) {
                const auto &[descriptors, type] = data;
                i.emplace_back(VkWriteDescriptorSet {
                    .dstSet          = m_descriptor_set,
                    .dstBinding      = binding,
                    .dstArrayElement = 0,
                    .descriptorCount = core::as<core::UInt32>(std::size(descriptors)),
                    .descriptorType  = type,
                    .pImageInfo      = std::data(descriptors) });
            }
            return i;
        }();

        vk.vkUpdateDescriptorSets(device(),
                                  std::size(write_infos),
                                  std::data(write_infos),
                                  0,
                                  nullptr);
    }
} // namespace stormkit::render
