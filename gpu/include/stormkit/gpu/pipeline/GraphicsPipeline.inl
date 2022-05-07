// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "GraphicsPipeline.mpp"

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto GraphicsPipeline::setRenderPass(const RenderPass &render_pass) noexcept -> void {
        STORMKIT_EXPECTS(m_is_baked == false);

        m_render_pass = &render_pass;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto GraphicsPipeline::renderPass() const noexcept -> const RenderPass & {
        STORMKIT_EXPECTS(m_render_pass);

        return *m_render_pass;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto GraphicsPipeline::setState(const GraphicsPipelineState &state) noexcept -> void {
        STORMKIT_EXPECTS(m_is_baked == false);

        m_state = state;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto GraphicsPipeline::state() noexcept -> GraphicsPipelineState & {
        STORMKIT_EXPECTS(m_is_baked == false);

        return m_state;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto GraphicsPipeline::state() const noexcept -> const GraphicsPipelineState & {
        return m_state;
    }
} // namespace stormkit::gpu
