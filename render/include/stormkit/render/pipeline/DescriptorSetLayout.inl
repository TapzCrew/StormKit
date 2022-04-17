// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::render {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto DescriptorSetLayout::addBinding(const DescriptorSetLayoutBinding &binding) -> void {
        m_bindings.emplace_back(binding);

        updateHash();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto DescriptorSetLayout::hash() const noexcept -> core::Hash64 { return m_hash; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto DescriptorSetLayout::bindings() const noexcept
        -> std::span<const DescriptorSetLayoutBinding> {
        return m_bindings;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline DescriptorSetLayout::operator VkDescriptorSetLayout() const noexcept {
        return vkHandle();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto DescriptorSetLayout::vkHandle() const noexcept -> VkDescriptorSetLayout {
        STORMKIT_EXPECTS(m_descriptor_set_layout);

        return m_descriptor_set_layout;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto DescriptorSetLayout::operator==(const DescriptorSetLayout &second) const noexcept
        -> bool {
        return m_hash == second.hash();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    constexpr auto DescriptorSetLayout::vkObjectType() const noexcept -> VkObjectType {
        return VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT;
    }
} // namespace stormkit::render
