// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::render {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto AbstractPipeline::baked() const noexcept -> bool { return m_is_baked; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline AbstractPipeline::operator VkPipeline() const noexcept { return vkHandle(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto AbstractPipeline::vkHandle() const noexcept -> VkPipeline {
        STORMKIT_EXPECTS(m_pipeline);

        return m_pipeline;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto AbstractPipeline::vkLayout() const noexcept -> VkPipelineLayout {
        STORMKIT_EXPECTS(m_pipeline_layout);

        return m_pipeline_layout;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    constexpr auto AbstractPipeline::vkObjectType() const noexcept -> VkObjectType {
        return VK_OBJECT_TYPE_PIPELINE;
    }
} // namespace stormkit::render
