// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::render {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto DescriptorSet::types() const noexcept -> std::span<const DescriptorType> {
        return m_types;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline DescriptorSet::operator VkDescriptorSet() const noexcept { return vkHandle(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto DescriptorSet::vkHandle() const noexcept -> VkDescriptorSet {
        STORMKIT_EXPECTS(m_descriptor_set);

        return m_descriptor_set;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    constexpr auto DescriptorSet::vkObjectType() const noexcept -> VkObjectType {
        return VK_OBJECT_TYPE_DESCRIPTOR_SET;
    }
} // namespace stormkit::render
