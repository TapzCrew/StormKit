// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module stormkit.Gpu;

import std;

import stormkit.Core;

import :Execution.RenderPass;

import vulkan;

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    RenderPass::RenderPass(const Device& device, const RenderPassDescription& description)
        : DeviceObject { device }, m_description { description } {
#if 0
        bake();
#endif
    }

    /////////////////////////////////////
    /////////////////////////////////////
    RenderPass::~RenderPass() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    RenderPass::RenderPass(RenderPass&& other) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto RenderPass::operator=(RenderPass&& other) noexcept -> RenderPass& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto RenderPass::bake() noexcept -> void {
        const auto attachments =
            m_description.attachments | std::views::transform([](auto&& attachment) {
                return vk::AttachmentDescription {}
                    .setFormat(core::as<vk::Format>(attachment.format))
                    .setSamples(core::as<vk::SampleCountFlagBits>(attachment.samples))
                    .setLoadOp(core::as<vk::AttachmentLoadOp>(attachment.load_op))
                    .setStoreOp(core::as<vk::AttachmentStoreOp>(attachment.store_op))
                    .setStencilLoadOp(core::as<vk::AttachmentLoadOp>(attachment.stencil_load_op))
                    .setStencilStoreOp(core::as<vk::AttachmentStoreOp>(attachment.stencil_store_op))
                    .setInitialLayout(core::as<vk::ImageLayout>(attachment.source_layout))
                    .setFinalLayout(core::as<vk::ImageLayout>(attachment.destination_layout));
            }) |
            std::ranges::to<std::vector>();

        auto color_attachment_refs   = std::vector<std::vector<vk::AttachmentReference>> {};
        auto depth_attachment_ref    = std::optional<vk::AttachmentReference> {};
        auto resolve_attachment_refs = std::vector<std::vector<vk::AttachmentReference>> {};
        auto subpasses               = std::vector<vk::SubpassDescription> {};
        auto subpasses_deps          = std::vector<vk::SubpassDependency> {};

        color_attachment_refs.reserve(std::size(m_description.subpasses));
        resolve_attachment_refs.reserve(std::size(m_description.subpasses));
        subpasses.reserve(std::size(m_description.subpasses));
        subpasses_deps.reserve(std::size(m_description.subpasses));

        for (const auto& subpass : m_description.subpasses) {
            auto& color_attachment_ref = color_attachment_refs.emplace_back();
            color_attachment_ref.reserve(std::size(subpass.attachment_refs));
            auto& resolve_attachment_ref = resolve_attachment_refs.emplace_back();

            for (const auto& attachment_ref : subpass.attachment_refs) {
                if (isDepthFormat(m_description.attachments[attachment_ref.attachment_id].format)) {
                    depth_attachment_ref =
                        vk::AttachmentReference {}
                            .setAttachment(attachment_ref.attachment_id)
                            .setLayout(core::as<vk::ImageLayout>(attachment_ref.layout));
                    continue;
                }

                const auto vk_attachment_ref =
                    vk::AttachmentReference {}
                        .setAttachment(attachment_ref.attachment_id)
                        .setLayout(core::as<vk::ImageLayout>(attachment_ref.layout));

                if (m_description.attachments[attachment_ref.attachment_id].resolve)
                    resolve_attachment_ref.emplace_back(std::move(vk_attachment_ref));
                else
                    color_attachment_ref.emplace_back(std::move(vk_attachment_ref));
            }

            subpasses.emplace_back(
                vk::SubpassDescription {}
                    .setPipelineBindPoint(core::as<vk::PipelineBindPoint>(subpass.bind_point))
                    .setColorAttachments(color_attachment_ref)
                    .setResolveAttachments(resolve_attachment_ref)
                    .setPDepthStencilAttachment(depth_attachment_ref.has_value()
                                                    ? &depth_attachment_ref.value()
                                                    : nullptr));

            subpasses_deps.emplace_back(
                vk::SubpassDependency {}
                    .setSrcSubpass(vk::SubpassExternal)
                    .setDstSubpass(0)
                    .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
                    .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
                    .setSrcAccessMask(vk::AccessFlagBits {})
                    .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentRead |
                                      vk::AccessFlagBits::eColorAttachmentWrite));

            const auto create_info = vk::RenderPassCreateInfo {}
                                         .setAttachments(attachments)
                                         .setSubpasses(subpasses)
                                         .setDependencies(subpasses_deps);

            try {
                m_vk_render_pass = vk::raii::RenderPass { this->device().vkHandle(), create_info };
            } catch (const vk::SystemError& err) {
                throw std::unexpected { core::as<Result>(err.code().value()) };
            }
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto RenderPass::isCompatible(const RenderPass& render_pass) const noexcept -> bool {
        // TODO implement proper compatibility check
        // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/html/chap7.html#renderpass-compatibility

        return &render_pass == this;
    }

#define CHECK(x) \
    if (not(x)) return false

    /////////////////////////////////////
    /////////////////////////////////////
    auto
        RenderPassDescription::isCompatible(const RenderPassDescription& description) const noexcept
        -> bool {
        if (std::ranges::size(subpasses) != std::ranges::size(subpasses)) return false;

        for (auto i : core::range(std::size(subpasses))) {
            const auto& subpass_1 = subpasses[i];
            const auto& subpass_2 = description.subpasses[i];

            if (subpass_1.bind_point != subpass_2.bind_point) return false;

            const auto attachment_refs_count = std::min(std::size(subpass_1.attachment_refs),
                                                        std::size(subpass_2.attachment_refs));

            for (auto j = 0u; j < attachment_refs_count; ++j) {
                const auto& attachment_ref_1 = subpass_1.attachment_refs[j];
                const auto& attachment_ref_2 = subpass_2.attachment_refs[j];

                const auto& attachment_1 = description.attachments[attachment_ref_1.attachment_id];
                const auto& attachment_2 = description.attachments[attachment_ref_2.attachment_id];

                if (attachment_1.format != attachment_2.format) return false;
                if (attachment_1.samples != attachment_2.samples) return false;
            }
        }

        return true;
    }
} // namespace stormkit::gpu
