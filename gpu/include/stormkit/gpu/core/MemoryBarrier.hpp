// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once


#include <vector>


#include <stormkit/core/Types.hpp>


#include <stormkit/gpu/core/Enums.hpp>
#include <stormkit/gpu/core/Fwd.hpp>
#include <stormkit/gpu/core/Vulkan.hpp>

#include <stormkit/gpu/resource/Fwd.hpp>
#include <stormkit/gpu/resource/ImageSubresourceRange.hpp>

namespace stormkit::gpu {
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

        const Buffer &buffer;
        core::USize size;
        core::UInt64 offset = 0;
    };

    struct ImageMemoryBarrier {
        AccessFlag src;
        AccessFlag dst;

        ImageLayout old_layout;
        ImageLayout new_layout;

        core::UInt32 src_queue_family_index = QUEUE_FAMILY_IGNORED;
        core::UInt32 dst_queue_family_index = QUEUE_FAMILY_IGNORED;

        const Image &image;
        ImageSubresourceRange range;
    };
} // namespace stormkit::gpu