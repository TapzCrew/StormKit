// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "ComputePipeline.mpp"

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto ComputePipeline::setState(const ComputePipelineState &state) noexcept -> void {
        STORMKIT_EXPECTS(m_is_baked == false);

        m_state = state;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto ComputePipeline::state() noexcept -> ComputePipelineState & {
        STORMKIT_EXPECTS(m_is_baked == false);

        return m_state;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto ComputePipeline::state() const noexcept -> const ComputePipelineState & {
        return m_state;
    }
} // namespace stormkit::gpu
