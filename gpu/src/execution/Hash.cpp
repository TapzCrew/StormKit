// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#ifdef STORMKIT_BUILD_MODULES
module stormkit.Gpu:Execution;

import :DescriptorSet;
import :RenderPass;
#else
    #include <stormkit/std.hpp>

    #include <stormkit/Core.hpp>
    #include <stormkit/Gpu.hpp>
#endif

using namespace stormkit;

namespace std {
    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<gpu::BufferDescriptor>::operator()(
        const gpu::BufferDescriptor& descriptor) const noexcept -> core::Hash64 {
        auto hash = core::Hash64 { 0 };

        core::hashCombine(hash, descriptor.type);
        core::hashCombine(hash, descriptor.binding);
        core::hashCombine(hash, descriptor.buffer);
        core::hashCombine(hash, descriptor.range);
        core::hashCombine(hash, descriptor.offset);

        return hash;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<gpu::ImageDescriptor>::operator()(
        const gpu::ImageDescriptor& descriptor) const noexcept -> core::Hash64 {
        auto hash = core::Hash64 { 0 };

        core::hashCombine(hash, descriptor.type);
        core::hashCombine(hash, descriptor.binding);
        core::hashCombine(hash, descriptor.layout);
        core::hashCombine(hash, descriptor.image_view);
        core::hashCombine(hash, descriptor.sampler);

        return hash;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<gpu::Descriptor>::operator()(const gpu::Descriptor& descriptor_proxy) const noexcept
        -> core::Hash64 {
        auto hash = core::Hash64 { 0 };

        std::visit(core::Overload { [&hash](auto& descriptor) {
                       core::hashCombine(hash, descriptor);
                   } },
                   descriptor_proxy);

        return hash;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<gpu::DescriptorSetLayout>::operator()(
        const gpu::DescriptorSetLayout& layout) const noexcept -> core::Hash64 {
        return layout.hash();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<gpu::DescriptorSetLayoutBinding>::operator()(
        const gpu::DescriptorSetLayoutBinding& binding) const noexcept -> core::Hash64 {
        auto hash = core::Hash64 { 0 };
        core::hashCombine(hash, binding.binding);
        core::hashCombine(hash, binding.type);
        core::hashCombine(hash, binding.stages);
        core::hashCombine(hash, binding.descriptor_count);

        return hash;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<gpu::AttachmentDescription>::operator()(
        const gpu::AttachmentDescription& description) const noexcept -> core::Hash64 {
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
    auto hash<gpu::Subpass>::operator()(const gpu::Subpass& description) const noexcept
        -> core::Hash64 {
        auto hash = core::Hash64 { 0 };

        core::hashCombine(hash, description.bind_point);

        for (const auto& ref : description.attachment_refs) core::hashCombine(hash, ref);

        return hash;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<gpu::Subpass::Ref>::operator()(const gpu::Subpass::Ref& description) const noexcept
        -> core::Hash64 {
        auto hash = core::Hash64 { 0 };

        core::hashCombine(hash, description.attachment_id);
        core::hashCombine(hash, description.layout);

        return hash;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto hash<gpu::RenderPassDescription>::operator()(
        const gpu::RenderPassDescription& description) const noexcept -> core::Hash64 {
        auto hash = core::Hash64 { 0 };

        for (const auto& attachment : description.attachments) core::hashCombine(hash, attachment);

        for (const auto& subpass : description.subpasses) core::hashCombine(hash, subpass);

        return hash;
    }
} // namespace std
