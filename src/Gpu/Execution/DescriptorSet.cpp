// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module stormkit.Gpu;

import std;

import stormkit.Core;

import :Execution.Descriptors;

import vulkan;

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    DescriptorSet::DescriptorSet(const DescriptorPool& pool,
                                 std::vector<DescriptorType> types,
                                 vk::raii::DescriptorSet&& set,
                                 Deleter deleter,
                                 Tag)
        : DeviceObject { pool.device() }, m_pool { &pool }, m_types { std::move(types) },
          m_descriptor_set { std::move(set) }, m_deleter { std::move(deleter) } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    DescriptorSet::~DescriptorSet() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    DescriptorSet::DescriptorSet(DescriptorSet&& other) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto DescriptorSet::operator=(DescriptorSet&& other) noexcept -> DescriptorSet& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto DescriptorSet::update(std::span<const Descriptor> descriptors) -> void {
        core::expects(!std::empty(descriptors));

        const auto [buffers, images] = [&] {
            auto buffers = core::HashMap<
                core::UInt32,
                std::pair<std::vector<vk::DescriptorBufferInfo>, vk::DescriptorType>> {};
            auto images = core::HashMap<
                core::UInt32,
                std::pair<std::vector<vk::DescriptorImageInfo>, vk::DescriptorType>> {};

            for (const auto& proxy : descriptors) {
                if (core::is<BufferDescriptor>(proxy)) {
                    const auto& descriptor = core::as<BufferDescriptor>(proxy);

                    auto& [infos, type] = buffers[descriptor.binding];
                        infos.emplace_back(*descriptor.buffer, descriptor.offset), descriptor.range);
                        type = core::as<vk::DescriptorType>(descriptor.type);
                }

                if (core::is<ImageDescriptor>(proxy)) {
                    const auto& descriptor = core::as<ImageDescriptor>(proxy);

                    auto& [infos, type] = images[descriptor.binding];
                    infos.emplace_back((descriptor.sampler) ? descriptor.sampler->vkHandle()
                                                            : nullptr,
                                       *descriptor.image_view,
                                       core::as<vk::ImageLayout>(descriptor.layout));
                    type = core::as<vk::DescriptorType>(descriptor.type);
                }
            }

            return std::make_pair(std::move(buffers), std::move(images));
        }();

        const auto write_infos =
            [&buffers = buffers, &images = images, this] {
                auto infos = std::vector<vk::WriteDescriptorSet> {};
                infos.reserve(std::size(buffers) + std::size(images));

                for (const auto& [binding, data] : buffers) {
                    const auto& [descriptors, type] = data;

                    infos.emplace_back(m_descriptor_set,
                                       binding,
                                       0,
                                       core::as<core::UInt32>(std::size(descriptors)),
                                       type,
                                       nullptr,
                                       std::data(descriptors),
                                       nullptr);
                }
                for (const auto& [binding, data] : images) {
                    const auto& [descriptors, type] = data;
                    infos.emplace_back(m_descriptor_set,
                                       binding,
                                       0,
                                       core::as<core::UInt32>(std::size(descriptors)),
                                       type,
                                       std::data(descriptors),
                                       nullptr,
                                       nullptr);
                }
                return infos;
            }

        ();

        m_vk_descriptor_set->update(write_infos, {});
    }
} // namespace stormkit::gpu
