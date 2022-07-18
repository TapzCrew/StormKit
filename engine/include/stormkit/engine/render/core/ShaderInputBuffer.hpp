// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/PlatformMacro.hpp>

#include <stormkit/core/Types.hpp>

#include <stormkit/gpu/Fwd.hpp>

#include <stormkit/gpu/resource/Buffer.hpp>

#include <stormkit/engine/Engine.hpp>
#include <stormkit/engine/EngineObject.hpp>
#include <stormkit/engine/Fwd.hpp>

#include <stormkit/engine/render/core/BlockBuffer.hpp>

namespace stormkit::engine {
    template<core::USize ComponentCount>
    class STORMKIT_PUBLIC ShaderInputBuffer: public EngineObject {
      public:
        using ComponentHandle = core::Handle64<std::vector<BlockBuffer>>;
        using BufferHandle    = core::Handle64<BlockBuffer>;

        struct Allocation {
            ComponentHandle component;
            BufferHandle buffer;
            BlockBuffer::Block::Handle block;
        };

        template<core::Is<gpu::Buffer::CreateInfo>... CreateInfos>
        requires(sizeof...(CreateInfos) ==
                 ComponentCount) explicit ShaderInputBuffer(Engine &engine,
                                                            const CreateInfos &...infos);
        ~ShaderInputBuffer();

        ShaderInputBuffer(const ShaderInputBuffer &) = delete;
        auto operator=(const ShaderInputBuffer &) -> ShaderInputBuffer & = delete;

        ShaderInputBuffer(ShaderInputBuffer &&) noexcept;
        auto operator=(ShaderInputBuffer &&) noexcept -> ShaderInputBuffer &;

        [[nodiscard]] auto allocateBlock(core::USize size,
                                         ComponentHandle component_handle) noexcept -> Allocation;
        [[nodiscard]] auto getBlock(const Allocation &allocation) noexcept
            -> const BlockBuffer::Block &;
        [[nodiscard]] auto getBuffer(const Allocation &allocation) noexcept -> const gpu::Buffer &;
        auto freeBlock(const Allocation &allocation) noexcept -> void;

        auto defragment() noexcept -> void;

      private:
        std::array<gpu::Buffer::CreateInfo, ComponentCount> m_create_infos;
        std::array<std::vector<BlockBuffer>, ComponentCount> m_components;
    };
} // namespace stormkit::engine

#include "ShaderInputBuffer.inl"
