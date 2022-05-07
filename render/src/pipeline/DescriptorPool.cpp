// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <storm/render/core/Device.hpp>
#include <storm/render/core/Enums.hpp>

#include <storm/render/pipeline/DescriptorPool.hpp>
#include <storm/render/pipeline/DescriptorSet.hpp>
#include <storm/render/pipeline/DescriptorSetLayout.hpp>

using namespace storm;
using namespace storm::render;

/////////////////////////////////////
/////////////////////////////////////
DescriptorPool::DescriptorPool(const Device &device,
                               std::vector<Size> sizes,
                               core::ArraySize max_sets)
    : m_device { &device } {
    auto vk_sizes = std::vector<vk::DescriptorPoolSize> {};
    vk_sizes.reserve(std::size(sizes));

    for (const auto &size : sizes) {
        const auto pool_size =
            vk::DescriptorPoolSize {}
                .setType(toVK(size.type))
                .setDescriptorCount(gsl::narrow_cast<core::UInt32>(size.descriptor_count));
        vk_sizes.emplace_back(std::move(pool_size));
    }

    const auto create_info = vk::DescriptorPoolCreateInfo {}
                                 .setMaxSets(gsl::narrow_cast<core::UInt32>(max_sets))
                                 .setPoolSizeCount(gsl::narrow_cast<core::UInt32>(std::size(sizes)))
                                 .setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
                                 .setPPoolSizes(std::data(vk_sizes));
    m_vk_descriptor_pool = m_device->createVkDescriptorPool(create_info);
}

/////////////////////////////////////
/////////////////////////////////////
DescriptorPool::~DescriptorPool() = default;

/////////////////////////////////////
/////////////////////////////////////
DescriptorPool::DescriptorPool(DescriptorPool &&) = default;

/////////////////////////////////////
/////////////////////////////////////
DescriptorPool &DescriptorPool::operator=(DescriptorPool &&) = default;

/////////////////////////////////////
/////////////////////////////////////
std::vector<render::DescriptorSet>
    DescriptorPool::allocateDescriptorSets(core::ArraySize count,
                                           const DescriptorSetLayout &layout) const {
    auto layouts = std::vector { count, layout.vkDescriptorSetLayout() };

    auto types = std::vector<DescriptorType> {};

    for (const auto &binding : layout.bindings()) {
        if (std::find(core::ranges::begin(types), core::ranges::end(types), binding.type) ==
            core::ranges::end(types))
            types.emplace_back(binding.type);
    }

    const auto allocate_info = vk::DescriptorSetAllocateInfo {}
                                   .setDescriptorPool(*m_vk_descriptor_pool)
                                   .setDescriptorSetCount(gsl::narrow_cast<core::UInt32>(count))
                                   .setPSetLayouts(std::data(layouts));

    auto vk_sets = m_device->allocateVkDescriptorSets(allocate_info);

    auto sets = std::vector<render::DescriptorSet> {};
    sets.reserve(count);
    for (auto &vk_set : vk_sets) sets.emplace_back(*this, types, std::move(vk_set));

    return sets;
}

/////////////////////////////////////
/////////////////////////////////////
std::vector<render::DescriptorSetOwnedPtr>
    DescriptorPool::allocateDescriptorSetsPtr(core::ArraySize count,
                                              const DescriptorSetLayout &layout) const {
    auto layouts = std::vector { count, layout.vkDescriptorSetLayout() };

    auto types = std::vector<DescriptorType> {};

    for (const auto &binding : layout.bindings()) {
        if (std::find(core::ranges::begin(types), core::ranges::end(types), binding.type) ==
            core::ranges::end(types))
            types.emplace_back(binding.type);
    }

    const auto allocate_info = vk::DescriptorSetAllocateInfo {}
                                   .setDescriptorPool(*m_vk_descriptor_pool)
                                   .setDescriptorSetCount(gsl::narrow_cast<core::UInt32>(count))
                                   .setPSetLayouts(std::data(layouts));

    auto vk_sets = m_device->allocateVkDescriptorSets(allocate_info);

    auto sets = std::vector<render::DescriptorSetOwnedPtr> {};
    sets.reserve(count);
    for (auto &vk_set : vk_sets)
        sets.emplace_back(std::make_unique<DescriptorSet>(*this, types, std::move(vk_set)));

    return sets;
}
