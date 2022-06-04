// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    template<core::USize ComponentCount>
    template<core::Is<gpu::Buffer::CreateInfo>... CreateInfos>
    requires(sizeof...(CreateInfos) == ComponentCount)
        ShaderInputBuffer<ComponentCount>::ShaderInputBuffer(Engine &engine,
                                                             const CreateInfos &...infos)
        : EngineObject { engine }, m_create_infos { infos... } {
        auto i = 0;
        ([&]() { m_components[i++].emplace_back(this->engine(), infos); }(), ...);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<core::USize ComponentCount>
    ShaderInputBuffer<ComponentCount>::~ShaderInputBuffer() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<core::USize ComponentCount>
    ShaderInputBuffer<ComponentCount>::ShaderInputBuffer(ShaderInputBuffer &&other) noexcept =
        default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<core::USize ComponentCount>
    auto ShaderInputBuffer<ComponentCount>::operator=(ShaderInputBuffer &&other) noexcept
        -> ShaderInputBuffer                      & = default;

    /////////////////////////////////////
    /////////////////////////////////////
    template<core::USize ComponentCount>
    auto ShaderInputBuffer<ComponentCount>::allocateBlock(core::USize size,
                                                          ComponentHandle component_handle) noexcept
        -> Allocation {
        STORMKIT_EXPECTS(component_handle < ComponentCount);

        auto block_handle  = BlockBuffer::Block::Handle::invalidHandle();
        auto buffer_handle = BufferHandle::invalidHandle();

        auto i = 0u;
        for (auto &buffer : m_components[component_handle]) {
            block_handle  = buffer.requestBlock(size);
            buffer_handle = BufferHandle { i++ };

            if (block_handle != BlockBuffer::Block::Handle::invalidHandle()) break;
        }

        if (block_handle == BlockBuffer::Block::Handle::invalidHandle()) {
            auto &buffer =
                m_components[component_handle].emplace_back(engine(),
                                                            m_create_infos[component_handle]);

            block_handle  = buffer.requestBlock(size);
            buffer_handle = BufferHandle { std::size(m_components[component_handle]) - 1 };
        }

        return { component_handle, buffer_handle, block_handle };
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<core::USize ComponentCount>
    auto ShaderInputBuffer<ComponentCount>::getBlock(const Allocation &allocation) noexcept
        -> const BlockBuffer::Block & {
        STORMKIT_EXPECTS(allocation.component < ComponentCount);
        STORMKIT_EXPECTS(allocation.buffer < std::size(m_components[allocation.component]));

        return m_components[allocation.component][allocation.buffer].getBlock(allocation.block);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<core::USize ComponentCount>
    auto ShaderInputBuffer<ComponentCount>::getBuffer(const Allocation &allocation) noexcept
        -> const gpu::Buffer & {
        STORMKIT_EXPECTS(allocation.component < ComponentCount);
        STORMKIT_EXPECTS(allocation.buffer < std::size(m_components[allocation.component]));

        return m_components[allocation.component][allocation.buffer].gpuBuffer();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<core::USize ComponentCount>
    auto ShaderInputBuffer<ComponentCount>::freeBlock(const Allocation &allocation) noexcept
        -> void {
        STORMKIT_EXPECTS(allocation.component < ComponentCount);
        STORMKIT_EXPECTS(allocation.buffer < std::size(m_components[allocation.component]));

        m_components[allocation.component][allocation.buffer].freeBlock(allocation.block);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    template<core::USize ComponentCount>
    auto ShaderInputBuffer<ComponentCount>::defragment() noexcept -> void {
        for (auto &component : m_components)
            for (auto &buffer : component) buffer.defragment();
    }
} // namespace stormkit::engine
