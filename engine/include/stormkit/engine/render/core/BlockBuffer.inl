// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto BlockBuffer::size() const noexcept -> core::USize { return m_size; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto BlockBuffer::freeSpace() const noexcept -> core::USize { return m_free_space; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto BlockBuffer::getBlock(Block::Handle handle) -> const Block & {
        auto it = std::ranges::find_if(m_blocks, [&](const auto &b) { return b.handle == handle; });
        STORMKIT_ENSURES(it != std::ranges::cend(m_blocks));
        return *it;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto BlockBuffer::gpuBuffer() const noexcept -> const gpu::Buffer & { return m_buffer; }
} // namespace stormkit::engine
