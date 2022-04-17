// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::render {
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

    /////////////////////////////////////
    /////////////////////////////////////
    constexpr auto RenderPass::vkObjectType() const noexcept -> VkObjectType {
        return VK_OBJECT_TYPE_RENDER_PASS;
    }
} // namespace stormkit::render
