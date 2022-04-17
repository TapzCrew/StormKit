// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

/////////// - StormKit::render - ///////////
#include <stormkit/render/core/Device.mpp>

#include <stormkit/render/pipeline/Framebuffer.mpp>
#include <stormkit/render/pipeline/RenderPass.mpp>

namespace stormkit::render {
    /////////////////////////////////////
    /////////////////////////////////////
    RenderPass::RenderPass(const Device &device, const RenderPassDescription &description)
        : m_device { &device }, m_description { description } {
        bake();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    RenderPass::~RenderPass() {
        if (m_render_pass != VK_NULL_HANDLE) [[likely]] {
            const auto &vk = device().table();

            vk.vkDestroyRenderPass(device(), m_render_pass, nullptr);
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    RenderPass::RenderPass(RenderPass &&other) noexcept
        : m_device { std::exchange(other.m_device, nullptr) },
          m_description { std::exchange(other.m_description, {}) }, m_render_pass {
              std::exchange(other.m_render_pass, VK_NULL_HANDLE)
          } {}

    /////////////////////////////////////
    /////////////////////////////////////
    auto RenderPass::operator=(RenderPass &&other) noexcept -> RenderPass & {
        if (&other != this) [[unlikely]]
            return *this;

        m_device      = std::exchange(other.m_device, nullptr);
        m_description = std::exchange(other.m_description, {});
        m_render_pass = std::exchange(other.m_render_pass, VK_NULL_HANDLE);

        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto RenderPass::bake() noexcept -> void {
        const auto &vk = device().table();

        const auto attachments = [&] {
            auto a = std::vector<VkAttachmentDescription> {};
            a.reserve(std::size(m_description.attachments));

            std::ranges::transform(m_description.attachments,
                                   std::back_inserter(a),
                                   [](const auto &a) {
                                       return VkAttachmentDescription {
                                           .format  = core::as<VkFormat>(a.format),
                                           .samples = core::as<VkSampleCountFlagBits>(a.samples),
                                           .loadOp  = core::as<VkAttachmentLoadOp>(a.load_op),
                                           .storeOp = core::as<VkAttachmentStoreOp>(a.load_op),
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

        for (const auto &subpass : m_description.subpasses) {
            auto &color_attachment_ref = color_attachment_refs.emplace_back();
            color_attachment_ref.reserve(std::size(subpass.attachment_refs));
            auto &resolve_attachment_ref = resolve_attachment_refs.emplace_back();

            for (const auto &attachment_ref : subpass.attachment_refs) {
                if (isDepthFormat(m_description.attachments[attachment_ref.attachment_id].format)) {
                    depth_attachment_ref = VkAttachmentReference {
                        .attachment = attachment_ref.attachment_id,
                        .layout     = core::as<VkImageLayout>(attachment_ref.layout)
                    };
                    continue;
                }

                const auto vk_attachment_ref =
                    VkAttachmentReference { .attachment = attachment_ref.attachment_id,
                                            .layout =
                                                core::as<VkImageLayout>(attachment_ref.layout) };

                if (m_description.attachments[attachment_ref.attachment_id].resolve)
                    resolve_attachment_ref.emplace_back(std::move(vk_attachment_ref));
                else
                    color_attachment_ref.emplace_back(std::move(vk_attachment_ref));
            }

            subpasses.emplace_back(VkSubpassDescription {
                .pipelineBindPoint       = core::as<VkPipelineBindPoint>(subpass.bind_point),
                .colorAttachmentCount    = core::as<core::UInt32>(std::size(color_attachment_ref)),
                .pColorAttachments       = std::data(color_attachment_ref),
                .pResolveAttachments     = std::data(resolve_attachment_ref),
                .pDepthStencilAttachment = (depth_attachment_ref.has_value())
                                               ? &depth_attachment_ref.value()
                                               : VK_NULL_HANDLE });

            subpasses_deps.emplace_back(VkSubpassDependency {
                .srcSubpass    = VK_SUBPASS_EXTERNAL,
                .dstSubpass    = 0,
                .srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                .dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                .srcAccessMask = VkAccessFlagBits {},
                .dstAccessMask =
                    VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            });
        }

        const auto create_info = VkRenderPassCreateInfo {
            .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .attachmentCount = core::as<core::UInt32>(std::size(attachments)),
            .pAttachments    = std::data(attachments),
            .subpassCount    = core::as<core::UInt32>(std::size(subpasses)),
            .pSubpasses      = std::data(subpasses),
            .dependencyCount = core::as<core::UInt32>(std::size(subpasses_deps)),
            .pDependencies   = std::data(subpasses_deps)
        };

        CHECK_VK_ERROR(vk.vkCreateRenderPass(device(), &create_info, nullptr, &m_render_pass));
    };

    /////////////////////////////////////
    /////////////////////////////////////
    auto RenderPass::createFramebuffer(const core::ExtentU &extent,
                                       std::vector<ImageViewConstRef> attachments) const
        -> Framebuffer {
        return Framebuffer { *this, extent, std::move(attachments) };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto RenderPass::allocateFramebuffer(const core::ExtentU &extent,
                                         std::vector<ImageViewConstRef> attachments) const
        -> FramebufferOwnedPtr {
        return std::make_unique<Framebuffer>(*this, extent, std::move(attachments));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto RenderPass::allocateRefCountedFramebuffer(const core::ExtentU &extent,
                                                   std::vector<ImageViewConstRef> attachments) const
        -> FramebufferSharedPtr {
        return std::make_shared<Framebuffer>(*this, extent, std::move(attachments));
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto RenderPass::isCompatible(const RenderPass &render_pass) const noexcept -> bool {
        // TODO implement proper compatibility check
        // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/html/chap7.html#renderpass-compatibility

        return &render_pass == this;
    }
} // namespace stormkit::render
