// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <variant>

#include <storm/core/Types.hpp>

#include <storm/render/core/Enums.hpp>
#include <storm/render/core/Types.hpp>

#include <storm/render/resource/Fwd.hpp>
#include <storm/render/resource/TextureSubresourceRange.hpp>

namespace storm::render {
    static constexpr auto QUEUE_FAMILY_IGNORED = core::UInt32 { VK_QUEUE_FAMILY_IGNORED };

    struct MemoryBarrier {
        AccessFlag src;
        AccessFlag dst;
    };

    struct BufferMemoryBarrier {
        AccessFlag src;
        AccessFlag dst;

        core::UInt32 src_queue_family_index = QUEUE_FAMILY_IGNORED;
        core::UInt32 dst_queue_family_index = QUEUE_FAMILY_IGNORED;

        const HardwareBuffer &buffer;
        core::ArraySize size;
        core::Int32 offset = 0;
    };

    struct ImageMemoryBarrier {
        AccessFlag src;
        AccessFlag dst;

        TextureLayout old_layout;
        TextureLayout new_layout;

        core::UInt32 src_queue_family_index = QUEUE_FAMILY_IGNORED;
        core::UInt32 dst_queue_family_index = QUEUE_FAMILY_IGNORED;

        const Texture &texture;
        TextureSubresourceRange range;
    };

    using MemoryBarriers       = std::vector<MemoryBarrier>;
    using BufferMemoryBarriers = std::vector<BufferMemoryBarrier>;
    using ImageMemoryBarriers  = std::vector<ImageMemoryBarrier>;
} // namespace storm::render
