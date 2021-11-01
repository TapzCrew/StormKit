// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <vector>

/////////// - StormKit::core - ///////////
#include <storm/core/Hash.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/core/Enums.hpp>

namespace storm::render {
    struct AttachmentDescription {
        PixelFormat format;
        SampleCountFlag samples = SampleCountFlag::C1_BIT;

        AttachmentLoadOperation load_op   = AttachmentLoadOperation::Clear;
        AttachmentStoreOperation store_op = AttachmentStoreOperation::Store;

        AttachmentLoadOperation stencil_load_op   = AttachmentLoadOperation::Dont_Care;
        AttachmentStoreOperation stencil_store_op = AttachmentStoreOperation::Dont_Care;

        TextureLayout source_layout      = TextureLayout::Undefined;
        TextureLayout destination_layout = TextureLayout::Present_Src;

        bool resolve = false;
    };
    using AttachmentDescriptions = std::vector<AttachmentDescription>;

    struct Subpass {
        struct Ref {
            core::UInt32 attachment_id;

            TextureLayout layout = TextureLayout::Color_Attachment_Optimal;
        };

        PipelineBindPoint bind_point;
        std::vector<Ref> attachment_refs;
    };
    using Subpasses = std::vector<Subpass>;

    struct RenderPassDescription {
        AttachmentDescriptions attachments;
        Subpasses subpasses;

        bool isCompatible(const RenderPassDescription &description) const noexcept;
    };
} // namespace storm::render

HASH_FUNC(storm::render::AttachmentDescription)
HASH_FUNC(storm::render::Subpass::Ref)
HASH_FUNC(storm::render::Subpass)
HASH_FUNC(storm::render::RenderPassDescription)
