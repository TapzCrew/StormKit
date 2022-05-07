// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
    /////////////////////////////////////
    /////////////////////////////////////
    void ComputePipeline::setState(ComputePipelineState state) noexcept {
        m_state = std::move(state);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    ComputePipelineState &ComputePipeline::state() noexcept { return m_state; }

    /////////////////////////////////////
    /////////////////////////////////////
    const ComputePipelineState &ComputePipeline::state() const noexcept { return m_state; }
} // namespace storm::render
