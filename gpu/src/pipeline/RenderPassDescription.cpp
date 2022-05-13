#include <stormkit/gpu/pipeline/RenderPassDescription.mpp>

namespace stormkit::gpu {
#define CHECK(x) \
    if (!(x)) return false
    /////////////////////////////////////
    /////////////////////////////////////
    auto
        RenderPassDescription::isCompatible(const RenderPassDescription &description) const noexcept
        -> bool {
        if (std::size(description.subpasses) != std::size(subpasses)) return false;
        for (auto i : core::range(std::size(subpasses))) {
            const auto &subpass_1 = subpasses[i];
            const auto &subpass_2 = description.subpasses[i];

            CHECK(subpass_1.bind_point == subpass_2.bind_point);

            const auto attachment_refs_count = std::min(std::size(subpass_1.attachment_refs),
                                                        std::size(subpass_2.attachment_refs));

            for (auto j = 0u; j < attachment_refs_count; ++j) {
                const auto &attachment_ref_1 = subpass_1.attachment_refs[j];
                const auto &attachment_ref_2 = subpass_2.attachment_refs[j];

                CHECK(attachment_ref_1.layout == attachment_ref_2.layout);

                const auto &attachment_1 = description.attachments[attachment_ref_1.attachment_id];
                const auto &attachment_2 = description.attachments[attachment_ref_2.attachment_id];

                CHECK(attachment_1.format == attachment_2.format);
                CHECK(attachment_1.samples == attachment_2.samples);
            }
        }

        return true;
    }
#undef CHECK
} // namespace stormkit::gpu

using namespace stormkit;

namespace std {
    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<gpu::AttachmentDescription>::operator()(
        const gpu::AttachmentDescription &description) const noexcept -> core::Hash64 {
        auto hash = core::Hash64 { 0 };

        core::hashCombine(hash, description.format);
        core::hashCombine(hash, description.samples);
        core::hashCombine(hash, description.load_op);
        core::hashCombine(hash, description.store_op);
        core::hashCombine(hash, description.stencil_load_op);
        core::hashCombine(hash, description.stencil_store_op);
        core::hashCombine(hash, description.source_layout);
        core::hashCombine(hash, description.destination_layout);
        core::hashCombine(hash, description.resolve);

        return hash;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<gpu::Subpass>::operator()(const gpu::Subpass &description) const noexcept
        -> core::Hash64 {
        auto hash = core::Hash64 { 0 };

        core::hashCombine(hash, description.bind_point);

        for (const auto &ref : description.attachment_refs) core::hashCombine(hash, ref);

        return hash;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<gpu::Subpass::Ref>::operator()(const gpu::Subpass::Ref &description) const noexcept
        -> core::Hash64 {
        auto hash = core::Hash64 { 0 };

        core::hashCombine(hash, description.attachment_id);
        core::hashCombine(hash, description.layout);

        return hash;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<gpu::RenderPassDescription>::operator()(
        const gpu::RenderPassDescription &description) const noexcept -> core::Hash64 {
        auto hash = core::Hash64 { 0 };

        for (const auto &attachment : description.attachments) core::hashCombine(hash, attachment);

        for (const auto &subpass : description.subpasses) core::hashCombine(hash, subpass);

        return hash;
    }
} // namespace std
