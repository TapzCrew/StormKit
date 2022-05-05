// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "RenderPass.mpp"

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto RenderPass::description() const noexcept -> const RenderPassDescription & {
        return m_description;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline RenderPass::operator VkRenderPass() const noexcept { return vkHandle(); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto RenderPass::vkHandle() const noexcept -> VkRenderPass {
        STORMKIT_EXPECTS(m_render_pass);

        return m_render_pass;
    }
} // namespace stormkit::gpu
