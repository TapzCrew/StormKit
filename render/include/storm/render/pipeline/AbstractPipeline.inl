// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
    /////////////////////////////////////
    /////////////////////////////////////
    bool AbstractPipeline::isBuilded() const noexcept { return m_is_builded; }

    /////////////////////////////////////
    /////////////////////////////////////
    vk::Pipeline AbstractPipeline::vkPipeline() const noexcept {
        STORMKIT_EXPECTS(m_vk_pipeline);
        return *m_vk_pipeline;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    vk::PipelineLayout AbstractPipeline::vkPipelineLayout() const noexcept {
        STORMKIT_EXPECTS(m_vk_pipeline_layout);

        return *m_vk_pipeline_layout;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    AbstractPipeline::operator vk::Pipeline() const noexcept { return vkPipeline(); }

    /////////////////////////////////////
    /////////////////////////////////////
    vk::Pipeline AbstractPipeline::vkHandle() const noexcept { return vkPipeline(); }

    /////////////////////////////////////
    /////////////////////////////////////
    core::UInt64 AbstractPipeline::vkDebugHandle() const noexcept {
        return reinterpret_cast<core::UInt64>(vkHandle().operator VkPipeline_T *());
    }

} // namespace storm::render
