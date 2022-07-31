// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <stormkit/engine/Engine.hpp>
#include <stormkit/engine/render/Renderer.hpp>
#include <stormkit/engine/render/core/BlockBuffer.hpp>

#include <stormkit/gpu/core/Device.hpp>

namespace stormkit::engine {

    /////////////////////////////////////
    /////////////////////////////////////
    BlockBuffer::BlockBuffer(Engine& engine, const gpu::Buffer::CreateInfo& info)
        : EngineObject { engine }, m_blocks { { 0, info.size, true, 0u } },
          m_buffer { engine.renderer().device().createBuffer(info) }, m_size { info.size },
          m_free_space { info.size }, m_next_handle { 1u } {
        STORMKIT_EXPECTS(m_size > 0u);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    BlockBuffer::~BlockBuffer() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    BlockBuffer::BlockBuffer(BlockBuffer&& other) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto BlockBuffer::operator=(BlockBuffer&& other) noexcept -> BlockBuffer& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto BlockBuffer::requestBlock(core::USize size) noexcept -> BlockBuffer::Block::Handle {
        STORMKIT_EXPECTS(size > 0u);

        if (size < m_free_space) return Block::Handle {};

        auto id = Block::Handle {};

        if (auto it =
                std::ranges::find_if(m_blocks,
                                     [size](const auto& b) { return b.free && b.size >= size; });
            it != std::ranges::end(m_blocks)) {
            id       = it->handle;
            it->free = false;

            if (it->size != size) {
                const auto diff = it->size - size;
                m_blocks.emplace(it,
                                 Block { it->offset + size,
                                         diff,
                                         true,
                                         Block::Handle { m_next_handle++ } });

                it->size = size;
            }

            m_free_space -= it->size;
        }

        return id;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto BlockBuffer::freeBlock(Block::Handle handle) -> void {
        STORMKIT_EXPECTS(handle != Block::Handle::invalidHandle());
        auto it =
            std::ranges::find_if(m_blocks, [handle](const auto& b) { return b.handle == handle; });

        STORMKIT_ENSURES(it != std::ranges::cend(m_blocks));

        m_free_space += it->size;
        it->free = true;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto BlockBuffer::defragment() noexcept -> void {
        for (auto it = std::ranges::begin(m_blocks); it != std::ranges::end(m_blocks); ++it) {
            if (!it->free) continue;

            std::vector<decltype(std::ranges::begin(m_blocks))> to_remove;
            for (auto it2 = std::next(it); it2 != std::ranges::end(m_blocks) && it2->free; ++it2) {
                to_remove.emplace_back(it2);
                it->size += it2->size;
            }

            if (!std::empty(to_remove))
                m_blocks.erase(*std::ranges::begin(to_remove), *std::ranges::end(to_remove));
        }
    }
} // namespace stormkit::engine
