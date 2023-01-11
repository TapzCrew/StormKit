// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#ifdef STORMKIT_BUILD_MODULES
module stormkit.Gpu:Execution;

import :DescriptorSet;
import :DescriptorPool;
#else
    #include <stormkit/std.hpp>

    #include <stormkit/Core.hpp>
    #include <stormkit/Gpu.hpp>
#endif

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    DescriptorSet::DescriptorSet(const DescriptorPool& pool,
                                 std::vector<DescriptorType> types,
                                 VkDescriptorSet set,
                                 Deleter deleter,
                                 Tag)
        : DeviceObject { pool.device() }, m_pool { &pool }, m_types { std::move(types) },
          m_descriptor_set { set }, m_deleter { std::move(deleter) } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    DescriptorSet::~DescriptorSet() {
        if (m_descriptor_set != VK_NULL_HANDLE) [[likely]]
            m_deleter(m_descriptor_set);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    DescriptorSet::DescriptorSet(DescriptorSet&& other) noexcept
        : DeviceObject { std::move(other) }, m_pool { std::exchange(other.m_pool, nullptr) },
          m_types { std::move(other.m_types) },
          m_descriptor_set { std::exchange(other.m_descriptor_set, VK_NULL_HANDLE) },
          m_deleter { std::move(other.m_deleter) } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto DescriptorSet::operator=(DescriptorSet&& other) noexcept -> DescriptorSet& {
        if (&other == this) [[unlikely]]
            return *this;

        DeviceObject::operator=(std::move(other));
        m_pool           = std::exchange(other.m_pool, nullptr);
        m_types          = std::move(other.m_types);
        m_descriptor_set = std::exchange(other.m_descriptor_set, VK_NULL_HANDLE);
        m_deleter        = std::move(other.m_deleter);

        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto DescriptorSet::update(std::span<const Descriptor> descriptors) -> void {
        core::expects(m_descriptor_set);
        core::expects(!std::empty(descriptors));

        const auto& vk = device().table();

        const auto [buffers, images] = [&] {
            auto buffers =
                core::HashMap<core::UInt32,
                              std::pair<std::vector<VkDescriptorBufferInfo>, VkDescriptorType>> {};
            auto images =
                core::HashMap<core::UInt32,
                              std::pair<std::vector<VkDescriptorImageInfo>, VkDescriptorType>> {};

            for (const auto& proxy : descriptors) {
                if (core::is<BufferDescriptor>(proxy)) {
                    const auto& descriptor = core::as<BufferDescriptor>(proxy);

                    auto& [infos, type] = buffers[descriptor.binding];
                    infos.emplace_back(VkDescriptorBufferInfo { .buffer = *descriptor.buffer,
                                                                .offset = descriptor.offset,
                                                                .range  = descriptor.range });
                    type = core::as<VkDescriptorType>(descriptor.type);
                }
                if (core::is<ImageDescriptor>(proxy)) {
                    const auto& descriptor = core::as<ImageDescriptor>(proxy);

                    auto& [infos, type] = images[descriptor.binding];
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

            for (const auto& [binding, data] : buffers) {
                const auto& [descriptors, type] = data;

                i.emplace_back(VkWriteDescriptorSet {
                    .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .pNext            = nullptr,
                    .dstSet           = m_descriptor_set,
                    .dstBinding       = binding,
                    .dstArrayElement  = 0,
                    .descriptorCount  = core::as<core::UInt32>(std::size(descriptors)),
                    .descriptorType   = type,
                    .pImageInfo       = nullptr,
                    .pBufferInfo      = std::data(descriptors),
                    .pTexelBufferView = nullptr });
            }
            for (const auto& [binding, data] : images) {
                const auto& [descriptors, type] = data;
                i.emplace_back(VkWriteDescriptorSet {
                    .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .pNext            = nullptr,
                    .dstSet           = m_descriptor_set,
                    .dstBinding       = binding,
                    .dstArrayElement  = 0,
                    .descriptorCount  = core::as<core::UInt32>(std::size(descriptors)),
                    .descriptorType   = type,
                    .pImageInfo       = std::data(descriptors),
                    .pBufferInfo      = nullptr,
                    .pTexelBufferView = nullptr });
            }
            return i;
        }();

        vk.vkUpdateDescriptorSets(device(),
                                  core::as<core::UInt32>(std::size(write_infos)),
                                  std::data(write_infos),
                                  0,
                                  nullptr);
    }
} // namespace stormkit::gpu
