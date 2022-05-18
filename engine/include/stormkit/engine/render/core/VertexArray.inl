// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace stormkit::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto VertexArray::data() noexcept -> std::span<VertexView> { return m_vertices; }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto VertexArray::data() const noexcept -> std::span<const VertexView> {
        return m_vertices;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto VertexArray::begin() noexcept -> Iterator { return std::begin(m_vertices); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto VertexArray::begin() const noexcept -> ConstIterator {
        return std::begin(m_vertices);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto VertexArray::cbegin() const noexcept -> ConstIterator {
        return std::cbegin(m_vertices);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto VertexArray::end() noexcept -> Iterator { return std::end(m_vertices); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto VertexArray::end() const noexcept -> ConstIterator { return std::end(m_vertices); }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto VertexArray::cend() const noexcept -> ConstIterator {
        return std::cend(m_vertices);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto VertexArray::positions() const noexcept -> std::span<const core::Vector3f> {
        return m_positions;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto VertexArray::normals() const noexcept -> std::span<const core::Vector3f> {
        return m_normals;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto VertexArray::colors() const noexcept -> std::span<const core::Vector3f> {
        return m_colors;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto VertexArray::uvs() const noexcept -> std::span<const core::Vector2f> {
        return m_uvs;
    }
    /////////////////////////////////////
    /////////////////////////////////////
    inline auto VertexArray::bindingDescriptions() noexcept
        -> std::span<const stormkit::gpu::VertexBindingDescription> {
        static constexpr auto BINDING_DESCRIPTIONS = std::array {
            gpu::VertexBindingDescription { .binding = 0, .stride = sizeof(core::Vector3f) },
            gpu::VertexBindingDescription { .binding = 1, .stride = sizeof(core::Vector3f) },
            gpu::VertexBindingDescription { .binding = 2, .stride = sizeof(core::Vector3f) },
            gpu::VertexBindingDescription { .binding = 3, .stride = sizeof(core::Vector2f) }
        };

        return BINDING_DESCRIPTIONS;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    inline auto VertexArray::attributesDescriptions() noexcept
        -> std::span<const stormkit::gpu::VertexInputAttributeDescription> {
        static constexpr auto VERTEX_ATTRIBUTE_DESCRIPTIONS = std::array {
            gpu::VertexInputAttributeDescription { .location = 0,
                                                   .binding  = 0,
                                                   .format   = gpu::Format::Float3,
                                                   .offset   = 0 },
            gpu::VertexInputAttributeDescription { .location = 1,
                                                   .binding  = 1,
                                                   .format   = gpu::Format::Float3,
                                                   .offset   = 0 },
            gpu::VertexInputAttributeDescription { .location = 2,
                                                   .binding  = 2,
                                                   .format   = gpu::Format::Float3,
                                                   .offset   = 0 },
            gpu::VertexInputAttributeDescription { .location = 3,
                                                   .binding  = 3,
                                                   .format   = gpu::Format::Float2,
                                                   .offset   = 0 },
        };

        return VERTEX_ATTRIBUTE_DESCRIPTIONS;
    }
} // namespace stormkit::engine
