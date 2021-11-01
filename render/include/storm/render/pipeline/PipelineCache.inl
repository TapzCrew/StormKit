// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
    /////////////////////////////////////
    /////////////////////////////////////
    vk::PipelineCache PipelineCache::vkPipelineCache() const noexcept {
        STORMKIT_EXPECTS(m_vk_pipeline_cache);
        return *m_vk_pipeline_cache;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    PipelineCache::operator vk::PipelineCache() const noexcept { return vkPipelineCache(); }

    /////////////////////////////////////
    /////////////////////////////////////
    vk::PipelineCache PipelineCache::vkHandle() const noexcept { return vkPipelineCache(); }

    /////////////////////////////////////
    /////////////////////////////////////
    core::UInt64 PipelineCache::vkDebugHandle() const noexcept {
        return reinterpret_cast<core::UInt64>(vkHandle().operator VkPipelineCache_T *());
    }
} // namespace storm::render
