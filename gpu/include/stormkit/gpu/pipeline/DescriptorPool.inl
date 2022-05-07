// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "DescriptorPool.mpp"

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto DescriptorPool::createDescriptorSet(const DescriptorSetLayout &layout) const
        -> DescriptorSet {
        auto descriptor = std::move(createDescriptorSets(1u, layout)[0]);

        return descriptor;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto DescriptorPool::allocateDescriptorSet(const DescriptorSetLayout &layout) const
        -> DescriptorSetOwnedPtr {
        auto descriptor = std::move(allocateDescriptorSets(1u, layout)[0]);

        return descriptor;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto
        DescriptorPool::allocateRefCountedDescriptorSet(const DescriptorSetLayout &layout) const
        -> DescriptorSetSharedPtr {
        auto descriptor = std::move(allocateRefCountedDescriptorSets(1u, layout)[0]);

        return descriptor;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline DescriptorPool::operator VkDescriptorPool() const noexcept { return vkHandle(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto DescriptorPool::vkHandle() const noexcept -> VkDescriptorPool {
        STORMKIT_EXPECTS(m_descriptor_pool);

        return m_descriptor_pool;
    }
} // namespace stormkit::gpu
