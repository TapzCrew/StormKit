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
    DescriptorPool::DescriptorPool(const Device& device,
                                   std::span<const Size> sizes,
                                   core::UInt32 max_sets)
        : DeviceObject { device } {
        const auto pool_sizes = core::transform(sizes, [&](const auto& s) {
            return vk::DescriptorPoolSize {}
                .setType(core::as<VkDescriptorType>(s.type))
                .setDescriptorCount(s.descriptor_count)
        });

        const auto create_info = vk::DescriptorPoolCreateInfo {}
                                     .setFlags(vk::DescriptorPoolFlagBits::eCreateFreeDescriptorSet)
                                     .setMaxSets(max_sets)
                                     .setPoolSizes(_sizes);

        try {
            m_vk_descriptor_pool =
                vk::raii::DescriptorSetPool { this->device().vkHandle(), create_info };
        } catch (const vk::SystemError& err) {
            throw std::unexpected { core::as<Result>(err.code().value()) };
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    DescriptorPool::~DescriptorPool() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    DescriptorPool::DescriptorPool(DescriptorPool&& other) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto DescriptorPool::operator=(DescriptorPool&& other) noexcept -> DescriptorPool& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto DescriptorPool::createDescriptorSets(core::RangeExtent count,
                                              const DescriptorSetLayout& layout) const
        -> std::vector<gpu::DescriptorSet> {
        const auto [vk_sets, types] = createVkDescriptorSets(count, layout);

        return core::transform(vk_sets, [this, &types = types](const auto& v) -> DescriptorSet {
            return DescriptorSet { *this,
                                   std::move(types),
                                   v,
                                   [this](auto set) { deleteDescriptorSet(set); },
                                   DescriptorSet::Tag {} };
        });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto DescriptorPool::allocateDescriptorSets(core::RangeExtent count,
                                                const DescriptorSetLayout& layout) const
        -> std::vector<std::unique_ptr<gpu::DescriptorSet>> {
        const auto [vk_sets, types] = createVkDescriptorSets(count, layout);

        return core::transform(vk_sets, [this, &types = types](const auto& v) {
            return std::make_unique<DescriptorSet>(
                *this,
                types,
                v,
                [this](auto set) { deleteDescriptorSet(set); },
                DescriptorSet::Tag {});
        });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto DescriptorPool::createVkDescriptorSets(core::RangeExtent count,
                                                const DescriptorSetLayout& layout) const
        -> std::pair<std::vector<VkDescriptorSet>, std::vector<DescriptorType>> {
        const auto layouts = std::vector { count, layout.vkHandle() };

        auto types = core::transform(layout.bindings(), [](const auto& b) { return b.type; });

        const auto allocate_info = vk::DescriptorSetAllocateInfo {}
                                       .setDescriptorPool(m_descriptor_pool)
                                       .setLayouts(layouts);

        auto vk_sets = [&] {
            auto s = std::vector<vk::DescriptorSet> { count };

            try { // TODO return expected
                device().vkHandle().allocateDescriptorSet(allocate_info, s);
            } catch (const vk::SystemError& err) {
                throw std::unexpected { core::as<Result>(err.code().value()) };
            }

            return s;
        }();

        return std::make_pair(std::move(vk_sets), std::move(types));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto DescriptorPool::deleteDescriptorSet(vk::DescriptorSet set) const -> void {
        try { // TODO return expected
            device().vkHandle().freeDescriptorSets(*m_descriptor_pool, std::array { set });
        } catch (const vk::SystemError& err) {
            throw std::unexpected { core::as<Result>(err.code().value()) };
        }
    }
} // namespace stormkit::gpu
