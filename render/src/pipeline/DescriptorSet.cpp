// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <storm/render/core/Device.hpp>
#include <storm/render/core/Enums.hpp>
#include <storm/render/pipeline/DescriptorPool.hpp>
#include <storm/render/pipeline/DescriptorSet.hpp>
#include <storm/render/resource/HardwareBuffer.hpp>
#include <storm/render/resource/Sampler.hpp>
#include <storm/render/resource/Texture.hpp>

using namespace storm;
using namespace storm::render;

/////////////////////////////////////
/////////////////////////////////////
DescriptorSet::DescriptorSet(const DescriptorPool &pool,
                             std::vector<DescriptorType> types,
                             RAIIVkDescriptorSet set)
    : m_device { &pool.device() }, m_pool { &pool }, m_types { std::move(types) },
      m_vk_descriptor_set { std::move(set) } {
}

/////////////////////////////////////
/////////////////////////////////////
DescriptorSet::~DescriptorSet() = default;

/////////////////////////////////////
/////////////////////////////////////
DescriptorSet::DescriptorSet(DescriptorSet &&) = default;

/////////////////////////////////////
/////////////////////////////////////
DescriptorSet &DescriptorSet::operator=(DescriptorSet &&) = default;

/////////////////////////////////////
/////////////////////////////////////
void DescriptorSet::update(core::span<const render::Descriptor> descriptors) {
    STORMKIT_EXPECTS(!std::empty(descriptors));

    auto vk_buffer_infos = core::HashMap<core::UInt32, std::pair<std::vector<vk::DescriptorBufferInfo>, vk::DescriptorType>>{};
    auto vk_image_infos = core::HashMap<core::UInt32, std::pair<std::vector<vk::DescriptorImageInfo>, vk::DescriptorType>>{};

    for (const auto &descriptor_proxy : descriptors) {
        if (std::holds_alternative<BufferDescriptor>(descriptor_proxy)) {
            const auto &descriptor = std::get<BufferDescriptor>(descriptor_proxy);

            const auto buffer_info = vk::DescriptorBufferInfo {}
                                         .setBuffer(*descriptor.buffer)
                                         .setOffset(descriptor.offset)
                                         .setRange(descriptor.range);

            auto &buffer_infos = vk_buffer_infos[descriptor.binding];
            buffer_infos.first.emplace_back(buffer_info);
            buffer_infos.second = toVK(descriptor.type);
        } else if (std::holds_alternative<TextureDescriptor>(descriptor_proxy)) {
            const auto &descriptor = std::get<TextureDescriptor>(descriptor_proxy);

            auto image_info = vk::DescriptorImageInfo {}
                                  .setImageView(*descriptor.texture_view)
                                  .setImageLayout(toVK(descriptor.layout));

            if (descriptor.sampler != nullptr) image_info.setSampler(*descriptor.sampler);

            auto &image_infos = vk_image_infos[descriptor.binding];
            image_infos.first.emplace_back(image_info);
            image_infos.second = toVK(descriptor.type);
        }
    }

    auto vk_write_infos = std::vector<vk::WriteDescriptorSet> {};
    vk_write_infos.reserve(std::size(vk_buffer_infos) + std::size(vk_image_infos));

    for (const auto &[binding, data] : vk_buffer_infos) {
        auto &[descriptors, type] = data;
        const auto write_info = vk::WriteDescriptorSet {}
                                    .setDstSet(*m_vk_descriptor_set)
                                    .setDstBinding(binding)
                                    .setDstArrayElement(0)
                                    .setDescriptorCount(std::size(descriptors))
                                    .setDescriptorType(type)
                                    .setPBufferInfo(std::data(descriptors));

        vk_write_infos.emplace_back(std::move(write_info));
    }

    for (const auto &[binding, data] : vk_image_infos) {
        auto &[descriptors, type] = data;
        const auto write_info = vk::WriteDescriptorSet {}
                                    .setDstSet(*m_vk_descriptor_set)
                                    .setDstBinding(binding)
                                    .setDstArrayElement(0)
                                    .setDescriptorCount(std::size(descriptors))
                                    .setDescriptorType(type)
                                    .setPImageInfo(std::data(descriptors));

        vk_write_infos.emplace_back(std::move(write_info));
    }

    m_device->updateVkDescriptorSets(vk_write_infos, {});
}

namespace std {
    core::Hash64
        hash<BufferDescriptor>::operator()(const BufferDescriptor &descriptor) const noexcept {
        auto hash = core::Hash64 { 0 };

        core::hashCombine(hash, descriptor.type);
        core::hashCombine(hash, descriptor.binding);
        core::hashCombine(hash, descriptor.buffer);
        core::hashCombine(hash, descriptor.range);
        core::hashCombine(hash, descriptor.offset);

        return hash;
    }

    core::Hash64
        hash<TextureDescriptor>::operator()(const TextureDescriptor &descriptor) const noexcept {
        auto hash = core::Hash64 { 0 };

        core::hashCombine(hash, descriptor.type);
        core::hashCombine(hash, descriptor.binding);
        core::hashCombine(hash, descriptor.layout);
        core::hashCombine(hash, descriptor.texture_view);
        core::hashCombine(hash, descriptor.sampler);

        return hash;
    }

    core::Hash64 hash<Descriptor>::operator()(const Descriptor &descriptor_proxy) const noexcept {
        auto hash = core::Hash64 { 0 };

        std::visit(core::overload { [&hash](auto &descriptor) {
                       core::hashCombine(hash, descriptor);
                   } },
                   descriptor_proxy);

        return hash;
    }

    core::Hash64
        hash<DescriptorArray>::operator()(const DescriptorArray &descriptors) const noexcept {
        auto hash = core::Hash64 { 0 };

        for (const auto &descriptor : descriptors) core::hashCombine(hash, descriptor);

        return hash;
    }

    core::Hash64
        hash<DescriptorSpan>::operator()(const DescriptorSpan &descriptors) const noexcept {
        auto hash = core::Hash64 { 0 };

        for (const auto &descriptor : descriptors) core::hashCombine(hash, descriptor);

        return hash;
    }

    core::Hash64 hash<DescriptorConstSpan>::operator()(
        const DescriptorConstSpan &descriptors) const noexcept {
        auto hash = core::Hash64 { 0 };

        for (const auto &descriptor : descriptors) core::hashCombine(hash, descriptor);

        return hash;
    }
} // namespace std
