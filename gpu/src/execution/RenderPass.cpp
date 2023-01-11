// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#ifdef STORMKIT_BUILD_MODULES
module stormkit.Gpu:Execution;

import :RenderPass;
import :FrameBuffer;
#else
    #include <stormkit/std.hpp>

    #include <stormkit/Core.hpp>
    #include <stormkit/Gpu.hpp>
#endif

namespace stormkit::gpu {
    /////////////////////////////////////
    /////////////////////////////////////
    RenderPass::RenderPass(const Device& device, const RenderPassDescription& description)
        : DeviceObject { device }, m_description { description } {
        bake();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    RenderPass::~RenderPass() {
        if (m_render_pass != VK_NULL_HANDLE) [[likely]] {
            const auto& vk = device().table();

            vk.vkDestroyRenderPass(device(), m_render_pass, nullptr);
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    RenderPass::RenderPass(RenderPass&& other) noexcept
        : DeviceObject { std::move(other) },
          m_description { std::exchange(other.m_description, {}) },
          m_render_pass { std::exchange(other.m_render_pass, VK_NULL_HANDLE) } {
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto RenderPass::operator=(RenderPass&& other) noexcept -> RenderPass& {
        if (&other != this) [[unlikely]]
            return *this;

        DeviceObject::operator=(std::move(other));
        m_description = std::exchange(other.m_description, {});
        m_render_pass = std::exchange(other.m_render_pass, VK_NULL_HANDLE);

        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto RenderPass::bake() noexcept -> void {
        const auto& vk = device().table();

        const auto attachments = [&] {
            auto a = std::vector<VkAttachmentDescription> {};
            a.reserve(std::size(m_description.attachments));

            std::ranges::transform(m_description.attachments,
                                   std::back_inserter(a),
                                   [](const auto& a) {
                                       return VkAttachmentDescription {
                                           .setFormat(core::as<VkFormat>(a.format))
                                           .setSamples(core::as<VkSampleCountFlagBits>(a.samples))
                                           .setLoadOp(core::as<VkAttachmentLoadOp>(a.load_op))
                                           .setStoreOp(core::as<VkAttachmentStoreOp>(a.store_op))
                                           .stencilLoadOp =
                                               core::as<VkAttachmentLoadOp>(a.stencil_load_op),
                                           .stencilStoreOp =
                                               core::as<VkAttachmentStoreOp>(a.stencil_store_op),
                                           .initialLayout =
                                               core::as<VkImageLayout>(a.source_layout),
                                           .finalLayout =
                                               core::as<VkImageLayout>(a.destination_layout),
                                       };
                                   });

            return a;
        }();

        auto color_attachment_refs   = std::vector<std::vector<VkAttachmentReference>> {};
        auto depth_attachment_ref    = std::optional<VkAttachmentReference> {};
        auto resolve_attachment_refs = std::vector<std::vector<VkAttachmentReference>> {};
        auto subpasses               = std::vector<VkSubpassDescription> {};
        auto subpasses_deps          = std::vector<VkSubpassDependency> {};

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
                    depth_attachment_ref = VkAttachmentReference {
                        .setAttachment(attachment_ref.attachment_id)
                        .setLayout(core::as<VkImageLayout>(attachment_ref.layout))
                    };
                    continue;
                }

                const auto vk_attachment_ref =
                    VkAttachmentReference { .setAttachment(attachment_ref.attachment_id)
                                            .layout =
                                                core::as<VkImageLayout>(attachment_ref.layout) };

                if (m_description.attachments[attachment_ref.attachment_id].resolve)
                    resolve_attachment_ref.emplace_back(std::move(vk_attachment_ref));
                else
                    color_attachment_ref.emplace_back(std::move(vk_attachment_ref));
            }

            subpasses.emplace_back(VkSubpassDescription {
                .setPipelineBindPoint(core::as<VkPipelineBindPoint>(subpass.bind_point))
                .setColorAttachmentCount(core::as<core::UInt32>(std::size(color_attachment_ref)))
                .setPColorAttachments(std::data(color_attachment_ref))
                .setPResolveAttachments(std::data(resolve_attachment_ref))
                .setPDepthStencilAttachment((depth_attachment_ref.has_value()))
                                               ? &depth_attachment_ref.value()
                                               : VK_NULL_HANDLE });

            subpasses_deps.emplace_back(VkSubpassDependency {
                .setSrcSubpass(VK_SUBPASS_EXTERNAL)
                .setDstSubpass(0)
                .setSrcStageMask(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT)
                .setDstStageMask(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT)
                .setSrcAccessMask(VkAccessFlagBits {})
                .dstAccessMask =
                    VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            });
        }

        const auto create_info = VkRenderPassCreateInfo {
            .setSType(VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO)
            .setAttachmentCount(core::as<core::UInt32>(std::size(attachments)))
            .setPAttachments(std::data(attachments))
            .setSubpassCount(core::as<core::UInt32>(std::size(subpasses)))
            .setPSubpasses(std::data(subpasses))
            .setDependencyCount(core::as<core::UInt32>(std::size(subpasses_deps)))
            .setPDependencies(std::data(subpasses_deps))
        };

        CHECK_VK_ERROR(vk.vkCreateRenderPass(device(), &create_info, nullptr, &m_render_pass));
    };

    /////////////////////////////////////
    /////////////////////////////////////
    auto RenderPass::createFrameBuffer(const core::math::ExtentU& extent,
                                       std::vector<ImageViewConstRef> attachments) const
        -> FrameBuffer {
        return FrameBuffer { *this, extent, std::move(attachments) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto RenderPass::allocateFrameBuffer(const core::math::ExtentU& extent,
                                         std::vector<ImageViewConstRef> attachments) const
        -> std::unique_ptr<FrameBuffer> {
        return std::make_unique<FrameBuffer>(*this, extent, std::move(attachments));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto RenderPass::allocateRefCountedFrameBuffer(const core::math::ExtentU& extent,
                                                   std::vector<ImageViewConstRef> attachments) const
        -> std::shared_ptr<FrameBuffer> {
        return std::make_shared<FrameBuffer>(*this, extent, std::move(attachments));
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
        if (std::size(description.setSubpasses(std::size(subpasses)) return false;)
        for (auto i : core::range(std::size(subpasses))) {
            const auto& subpass_1 = subpasses[i];
            const auto& subpass_2 = description.subpasses[i];

            CHECK(subpass_1.setBind(subpass_2.bind_point);)

            const auto attachment_refs_count = std::min(std::size(subpass_1.attachment_refs),
                                                        std::size(subpass_2.attachment_refs));

            for (auto j = 0u; j < attachment_refs_count; ++j) {
                const auto& attachment_ref_1 = subpass_1.attachment_refs[j];
                const auto& attachment_ref_2 = subpass_2.attachment_refs[j];

                CHECK(attachment_ref_1.setLayout(attachment_ref_2.layout);)

                const auto& attachment_1 = description.attachments[attachment_ref_1.attachment_id];
                const auto& attachment_2 = description.attachments[attachment_ref_2.attachment_id];

                CHECK(attachment_1.setFormat(attachment_2.format);)
                CHECK(attachment_1.setSamples(attachment_2.samples);)
            }
        }

        return true;
    }

#undef CHECK
} // namespace stormkit::gpu
