// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <stormkit/render/core/Device.mpp>

#include <stormkit/render/pipeline/DescriptorPool.mpp>
#include <stormkit/render/pipeline/DescriptorSet.mpp>
#include <stormkit/render/pipeline/DescriptorSetLayout.mpp>

namespace stormkit::render {
    /////////////////////////////////////
    /////////////////////////////////////
    DescriptorPool::DescriptorPool(const Device &device,
                                   std::span<const Size> sizes,
                                   core::UInt32 max_sets)
        : m_device { &device } {
        const auto &vk = m_device->table();

        const auto vk_sizes = [&] {
            auto s = std::vector<VkDescriptorPoolSize> {};
            s.reserve(std::size(sizes));

            std::ranges::transform(sizes, std::back_inserter(s), [](const auto &_s) {
                return VkDescriptorPoolSize { .type = core::as<VkDescriptorType>(_s.type),
                                              .descriptorCount = _s.descriptor_count };
            });

            return s;
        }();

        const auto create_info = VkDescriptorPoolCreateInfo {
            .sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .flags         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
            .maxSets       = max_sets,
            .poolSizeCount = core::as<core::UInt32>(std::size(sizes)),
            .pPoolSizes    = std::data(vk_sizes),
        };

        CHECK_VK_ERROR(
            vk.vkCreateDescriptorPool(*m_device, &create_info, nullptr, &m_descriptor_pool));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    DescriptorPool::~DescriptorPool() {
        if (m_descriptor_pool != VK_NULL_HANDLE) [[likely]] {
            const auto &vk = device().table();

            vk.vkDestroyDescriptorPool(device(), m_descriptor_pool, nullptr);
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    DescriptorPool::DescriptorPool(DescriptorPool &&other) noexcept
        : m_device { std::exchange(other.m_device, nullptr) }, m_descriptor_pool {
              std::exchange(other.m_descriptor_pool, VK_NULL_HANDLE)
          } {}

    /////////////////////////////////////
    /////////////////////////////////////
    auto DescriptorPool::operator=(DescriptorPool &&other) noexcept -> DescriptorPool & {
        if (&other == this) [[unlikely]]
            return *this;

        m_device          = std::exchange(other.m_device, nullptr);
        m_descriptor_pool = std::exchange(other.m_descriptor_pool, VK_NULL_HANDLE);

        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto DescriptorPool::createDescriptorSets(core::USize count,
                                              const DescriptorSetLayout &layout) const
        -> std::vector<render::DescriptorSet> {
        STORMKIT_EXPECTS(m_descriptor_pool);
        const auto [vk_sets, types] = createVkDescriptorSets(count, layout);

        return core::transform(vk_sets,
                               [this, &vk_sets = vk_sets, &types = types](
                                   const auto &v) -> DescriptorSet {
                                   return DescriptorSet { *this,
                                                          std::move(types),
                                                          v,
                                                          [this](auto set) {
                                                              deleteDescriptorSet(set);
                                                          },
                                                          DescriptorSet::Tag {} };
                               });
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto DescriptorPool::allocateDescriptorSets(core::USize count,
                                                const DescriptorSetLayout &layout) const
        -> std::vector<render::DescriptorSetOwnedPtr> {
        const auto [vk_sets, types] = createVkDescriptorSets(count, layout);

        return core::transform(vk_sets, [this, &vk_sets = vk_sets, &types = types](const auto &v) {
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
    auto DescriptorPool::allocateRefCountedDescriptorSets(core::USize count,
                                                          const DescriptorSetLayout &layout) const
        -> std::vector<render::DescriptorSetSharedPtr> {
        const auto [vk_sets, types] = createVkDescriptorSets(count, layout);

        return core::transform(vk_sets, [this, &vk_sets = vk_sets, &types = types](const auto &v) {
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
    auto DescriptorPool::createVkDescriptorSets(core::USize count,
                                                const DescriptorSetLayout &layout) const
        -> std::pair<std::vector<VkDescriptorSet>, std::vector<DescriptorType>> {
        const auto &vk = device().table();

        const auto layouts = std::vector { count, layout.vkHandle() };

        auto types = core::transform(layout.bindings(), [](const auto &b) { return b.type; });

        const auto allocate_info =
            VkDescriptorSetAllocateInfo { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
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
        const auto &vk = device().table();
        vk.vkFreeDescriptorSets(device(), m_descriptor_pool, 1, &set);
    }
} // namespace stormkit::render
