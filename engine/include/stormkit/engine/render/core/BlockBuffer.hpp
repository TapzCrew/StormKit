// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <list>

#include <stormkit/core/PlatformMacro.hpp>

#include <stormkit/core/Types.hpp>

#include <stormkit/gpu/Fwd.hpp>

#include <stormkit/gpu/resource/Buffer.hpp>

#include <stormkit/engine/EngineObject.hpp>
#include <stormkit/engine/Fwd.hpp>

namespace stormkit::engine {
    class STORMKIT_PUBLIC BlockBuffer: public EngineObject {
      public:
        struct Block {
            using Handle = core::Handle64<Block>;

            core::USize size;
            core::USize offset;

            bool free;

            Handle handle;
        };

        BlockBuffer(Engine& engine, const gpu::Buffer::CreateInfo& info);
        ~BlockBuffer();

        BlockBuffer(const BlockBuffer&)                    = delete;
        auto operator=(const BlockBuffer&) -> BlockBuffer& = delete;

        BlockBuffer(BlockBuffer&&) noexcept;
        auto operator=(BlockBuffer&&) noexcept -> BlockBuffer&;

        [[nodiscard]] auto size() const noexcept -> core::USize;
        [[nodiscard]] auto freeSpace() const noexcept -> core::USize;

        [[nodiscard]] auto requestBlock(core::USize size) noexcept -> Block::Handle;
        auto freeBlock(Block::Handle handle) -> void;

        [[nodiscard]] auto getBlock(Block::Handle handle) -> const Block&;

        auto defragment() noexcept -> void;

        [[nodiscard]] auto gpuBuffer() const noexcept -> const gpu::Buffer&;

      private:
        std::list<Block> m_blocks;
        gpu::Buffer m_buffer;

        core::USize m_size;
        core::USize m_free_space;

        core::UInt64 m_next_handle;
    };
} // namespace stormkit::engine

#include "BlockBuffer.inl"
