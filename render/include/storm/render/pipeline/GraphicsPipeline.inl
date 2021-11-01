// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
    /////////////////////////////////////
    /////////////////////////////////////
    void GraphicsPipeline::setRenderPass(const RenderPass &render_pass) noexcept {
        m_render_pass = core::makeConstObserver(render_pass);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    const RenderPass &GraphicsPipeline::renderPass() const noexcept { return *m_render_pass; }

    /////////////////////////////////////
    /////////////////////////////////////
    void GraphicsPipeline::setState(GraphicsPipelineState state) noexcept {
        m_state = std::move(state);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    GraphicsPipelineState &GraphicsPipeline::state() noexcept { return m_state; }

    /////////////////////////////////////
    /////////////////////////////////////
    const GraphicsPipelineState &GraphicsPipeline::state() const noexcept { return m_state; }
} // namespace storm::render
