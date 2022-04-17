// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::render {
    /////////////////////////////////////
    /////////////////////////////////////
    inline PipelineCache::operator VkPipelineCache() const noexcept { return vkHandle(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto PipelineCache::vkHandle() const noexcept -> VkPipelineCache {
        STORMKIT_EXPECTS(m_pipeline_cache);

        return m_pipeline_cache;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    constexpr auto PipelineCache::vkObjectType() const noexcept -> VkObjectType {
        return VK_OBJECT_TYPE_PIPELINE_CACHE;
    }
} // namespace stormkit::render
