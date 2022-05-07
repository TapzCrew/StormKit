// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
    /////////////////////////////////////
    /////////////////////////////////////
    DescriptorSet DescriptorPool::allocateDescriptorSet(const DescriptorSetLayout &layout) const {
        auto descriptor = std::move(allocateDescriptorSets(1u, layout)[0]);

        return descriptor;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    DescriptorSetOwnedPtr
        DescriptorPool::allocateDescriptorSetPtr(const DescriptorSetLayout &layout) const {
        auto descriptor = std::move(allocateDescriptorSetsPtr(1u, layout)[0]);

        return descriptor;
    }
    /////////////////////////////////////
    /////////////////////////////////////
    const Device &DescriptorPool::device() const noexcept { return *m_device; }
    /////////////////////////////////////
    /////////////////////////////////////
    vk::DescriptorPool DescriptorPool::vkDescriptorPool() const noexcept {
        STORMKIT_EXPECTS(m_vk_descriptor_pool);
        return *m_vk_descriptor_pool;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    DescriptorPool::operator vk::DescriptorPool() const noexcept { return vkDescriptorPool(); }

    /////////////////////////////////////
    /////////////////////////////////////
    vk::DescriptorPool DescriptorPool::vkHandle() const noexcept { return vkDescriptorPool(); }
    /////////////////////////////////////
    /////////////////////////////////////
    core::UInt64 DescriptorPool::vkDebugHandle() const noexcept {
        return reinterpret_cast<core::UInt64>(vkHandle().operator VkDescriptorPool_T *());
    }
} // namespace storm::render
