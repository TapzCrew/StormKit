// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/////////// - StormKit::core - ///////////
#include <storm/core/Numerics.hpp>
#include <storm/core/Ranges.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/core/Device.hpp>
#include <storm/render/core/Enums.hpp>

#include <storm/render/pipeline/Framebuffer.hpp>
#include <storm/render/pipeline/RenderPass.hpp>

using namespace storm;
using namespace storm::render;

/////////////////////////////////////
/////////////////////////////////////
RenderPass::RenderPass(const render::Device &device, vk::RenderPass render_pass)
    : m_device{&device} {
    m_vk_render_pass = render_pass;
}

/////////////////////////////////////
/////////////////////////////////////
RenderPass::RenderPass(const render::Device &device, RenderPassDescription description)
    : m_device { &device }, m_description { std::move(description) } {
    build();
}

/////////////////////////////////////
/////////////////////////////////////
RenderPass::~RenderPass() = default;

/////////////////////////////////////
/////////////////////////////////////
RenderPass::RenderPass(RenderPass &&) = default;

/////////////////////////////////////
/////////////////////////////////////
RenderPass &RenderPass::operator=(RenderPass &&) = default;

/////////////////////////////////////
/////////////////////////////////////
void RenderPass::build() {
    auto attachments = std::vector<vk::AttachmentDescription> {};
    attachments.reserve(std::size(m_description.attachments));
    for (const auto &attachment : m_description.attachments) {
        const auto vk_attachment = vk::AttachmentDescription {}
                                       .setFormat(toVK(attachment.format))
                                       .setSamples(toVKBits(attachment.samples))
                                       .setLoadOp(toVK(attachment.load_op))
                                       .setStencilLoadOp(toVK(attachment.stencil_load_op))
                                       .setStencilStoreOp(toVK(attachment.stencil_store_op))
                                       .setInitialLayout(toVK(attachment.source_layout))
                                       .setFinalLayout(toVK(attachment.destination_layout));

        attachments.emplace_back(std::move(vk_attachment));
    }

    auto color_attachment_refs   = std::vector<std::vector<vk::AttachmentReference>> {};
    auto depth_attachment_ref    = std::optional<vk::AttachmentReference> {};
    auto resolve_attachment_refs = std::vector<std::vector<vk::AttachmentReference>> {};
    auto subpasses               = std::vector<vk::SubpassDescription> {};
    auto subpasses_deps          = std::vector<vk::SubpassDependency> {};

    for (const auto &subpass : m_description.subpasses) {
        auto &color_attachment_ref = color_attachment_refs.emplace_back();
        color_attachment_ref.reserve(std::size(subpass.attachment_refs));
        auto &resolve_attachment_ref = resolve_attachment_refs.emplace_back();

        for (const auto &attachment_ref : subpass.attachment_refs) {
            if (isDepthFormat(m_description.attachments[attachment_ref.attachment_id].format)) {
                depth_attachment_ref = vk::AttachmentReference {}
                                           .setAttachment(attachment_ref.attachment_id)
                                           .setLayout(toVK(attachment_ref.layout));
                continue;
            }

            const auto vk_attachment_ref = vk::AttachmentReference {}
                                               .setAttachment(attachment_ref.attachment_id)
                                               .setLayout(toVK(attachment_ref.layout));

            if (m_description.attachments[attachment_ref.attachment_id].resolve)
                resolve_attachment_ref.emplace_back(std::move(vk_attachment_ref));
            else
                color_attachment_ref.emplace_back(std::move(vk_attachment_ref));
        }

        const auto vk_subpass =
            vk::SubpassDescription {}
                .setPipelineBindPoint(toVK(subpass.bind_point))
                .setColorAttachmentCount(
                    gsl::narrow_cast<core::UInt32>(std::size(color_attachment_ref)))
                .setPColorAttachments(std::data(color_attachment_ref))
                .setPResolveAttachments(std::data(resolve_attachment_ref))
                .setPDepthStencilAttachment(
                    (depth_attachment_ref.has_value() ? &depth_attachment_ref.value() : nullptr));

        subpasses.emplace_back(std::move(vk_subpass));

        const auto vk_subpass_dependency =
            vk::SubpassDependency {}
                .setSrcSubpass(VK_SUBPASS_EXTERNAL)
                .setDstSubpass(0)
                .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
                .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
                .setSrcAccessMask(vk::AccessFlags {})
                .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentRead |
                                  vk::AccessFlagBits::eColorAttachmentWrite);

        subpasses_deps.emplace_back(std::move(vk_subpass_dependency));
    }

    const auto create_info =
        vk::RenderPassCreateInfo {}
            .setAttachmentCount(gsl::narrow_cast<core::UInt32>(std::size(attachments)))
            .setPAttachments(std::data(attachments))
            .setSubpassCount(gsl::narrow_cast<core::UInt32>(std::size(subpasses)))
            .setPSubpasses(std::data(subpasses))
            .setDependencyCount(gsl::narrow_cast<core::UInt32>(std::size(subpasses_deps)))
            .setPDependencies(std::data(subpasses_deps));

    m_vk_render_pass = m_device->createVkRenderPass(create_info);
};

/////////////////////////////////////
/////////////////////////////////////
render::Framebuffer RenderPass::createFramebuffer(core::Extentu extent,
                                                  TextureViewConstPtrArray attachments) const {
    return Framebuffer { *this, std::move(extent), std::move(attachments) };
}

/////////////////////////////////////
/////////////////////////////////////
render::FramebufferOwnedPtr
    RenderPass::createFramebufferPtr(core::Extentu extent,
                                     TextureViewConstPtrArray attachments) const {
    return std::make_unique<Framebuffer>(*this, std::move(extent), std::move(attachments));
}

/////////////////////////////////////
/////////////////////////////////////
bool RenderPass::isCompatible(const RenderPass &render_pass) const noexcept {
    // TODO implement proper compatibility check
    // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/html/chap7.html#renderpass-compatibility

    return &render_pass == this;
}

