// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#ifdef STORMKIT_BUILD_MODULES
module stormkit.Gpu:Execution;

import :DescriptorPool;
import :DescriptorSet;
#else
    #include <stormkit/std.hpp>

    #include <stormkit/Core.hpp>
    #include <stormkit/Gpu.hpp>
#endif

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    DescriptorPool::DescriptorPool(const Device& device,
                                   std::span<const Size> sizes,
                                   core::UInt32 max_sets)
        : DeviceObject { device } {
        const auto& vk = this->device().table();

        const auto _sizes = core::transform(sizes, [&](const auto& s) {
            return VkDescriptorPoolSize { .type            = core::as<VkDescriptorType>(s.type),
                                          .descriptorCount = s.descriptor_count };
        });

        const auto create_info = VkDescriptorPoolCreateInfo {
            .sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .pNext         = nullptr,
            .flags         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
            .maxSets       = max_sets,
            .poolSizeCount = core::as<core::UInt32>(std::size(_sizes)),
            .pPoolSizes    = std::data(_sizes),
        };

        CHECK_VK_ERROR(
            vk.vkCreateDescriptorPool(this->device(), &create_info, nullptr, &m_descriptor_pool));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    DescriptorPool::~DescriptorPool() {
        if (m_descriptor_pool != VK_NULL_HANDLE) [[likely]] {
            const auto& vk = device().table();

            vk.vkDestroyDescriptorPool(device(), m_descriptor_pool, nullptr);
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    DescriptorPool::DescriptorPool(DescriptorPool&& other) noexcept
        : DeviceObject { std::move(other) },
          m_descriptor_pool { std::exchange(other.m_descriptor_pool, VK_NULL_HANDLE) } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto DescriptorPool::operator=(DescriptorPool&& other) noexcept -> DescriptorPool& {
        if (&other == this) [[unlikely]]
            return *this;

        DeviceObject::operator=(std::move(other));
        m_descriptor_pool = std::exchange(other.m_descriptor_pool, VK_NULL_HANDLE);

        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto DescriptorPool::createDescriptorSets(core::RangeExtent count,
                                              const DescriptorSetLayout& layout) const
        -> std::vector<gpu::DescriptorSet> {
        core::expects(m_descriptor_pool);
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
    auto DescriptorPool::allocateRefCountedDescriptorSets(core::RangeExtent count,
                                                          const DescriptorSetLayout& layout) const
        -> std::vector<std::shared_ptr<gpu::DescriptorSet>> {
        const auto [vk_sets, types] = createVkDescriptorSets(count, layout);

        return core::transform(vk_sets, [this, &types = types](const auto& v) {
            return std::make_shared<DescriptorSet>(
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
        const auto& vk = device().table();

        const auto layouts = std::vector { count, layout.vkHandle() };

        auto types = core::transform(layout.bindings(), [](const auto& b) { return b.type; });

        const auto allocate_info =
            VkDescriptorSetAllocateInfo { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
                                          .pNext = nullptr,
                                          .descriptorPool     = m_descriptor_pool,
                                          .descriptorSetCount = core::as<core::UInt32>(count),
                                          .pSetLayouts        = std::data(layouts) };

        auto vk_sets = [&] {
            auto s = std::vector<VkDescriptorSet> { count };
            CHECK_VK_ERROR(vk.vkAllocateDescriptorSets(device(), &allocate_info, std::data(s)))
            return s;
        }();

        return std::make_pair(std::move(vk_sets), std::move(types));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto DescriptorPool::deleteDescriptorSet(VkDescriptorSet set) const -> void {
        const auto& vk = device().table();
        vk.vkFreeDescriptorSets(device(), m_descriptor_pool, 1, &set);
    }
} // namespace stormkit::gpu
