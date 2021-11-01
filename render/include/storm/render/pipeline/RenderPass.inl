// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::render {
    /////////////////////////////////////
    /////////////////////////////////////
    const Device &RenderPass::device() const noexcept { return *m_device; }

    /////////////////////////////////////
    /////////////////////////////////////
    const RenderPassDescription &RenderPass::description() const noexcept { return m_description; }

    /////////////////////////////////////
    /////////////////////////////////////
    vk::RenderPass RenderPass::vkRenderPass() const noexcept {
        auto render_pass = vk::RenderPass{nullptr};

        if(std::holds_alternative<RAIIVkRenderPass>(m_vk_render_pass))
            render_pass = *std::get<RAIIVkRenderPass>(m_vk_render_pass);
        else render_pass = std::get<vk::RenderPass>(m_vk_render_pass);

        STORMKIT_EXPECTS(render_pass);

        return render_pass;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    RenderPass::operator vk::RenderPass() const noexcept { return vkRenderPass(); }

    /////////////////////////////////////
    /////////////////////////////////////
    vk::RenderPass RenderPass::vkHandle() const noexcept { return vkRenderPass(); }

    /////////////////////////////////////
    /////////////////////////////////////
    core::UInt64 RenderPass::vkDebugHandle() const noexcept {
        return reinterpret_cast<core::UInt64>(vkHandle().operator VkRenderPass_T *());
    }
} // namespace storm::render
