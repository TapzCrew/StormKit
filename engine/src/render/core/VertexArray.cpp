// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/Core/Numerics.hpp>
#include <stormkit/Core/Types.hpp>

module stormkit.engine.render.core.VertexArray;

namespace stormkit::engine {
    /////////////////////////////////////
    /////////////////////////////////////
    VertexArray::VertexArray() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    VertexArray::VertexArray(std::span<const Vertex> vertices) {
        addVertices(vertices);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    VertexArray::~VertexArray() = default;

    /////////////////////////////////////
    /////////////////////////////////////
    VertexArray::VertexArray(const VertexArray& other) = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto VertexArray::operator=(const VertexArray& other) -> VertexArray& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    VertexArray::VertexArray(VertexArray&& other) noexcept = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto VertexArray::operator=(VertexArray&& other) noexcept -> VertexArray& = default;

    /////////////////////////////////////
    /////////////////////////////////////
    auto VertexArray::resize(core::RangeExtent size) -> void {
        if (m_size < size) {
            const auto diff = m_size - size;

            const auto vertices = std::vector<Vertex> { diff };
            addVertices(vertices);
        } else {
            m_positions.resize(size);
            m_normals.resize(size);
            m_colors.resize(size);
            m_uvs.resize(size);

            m_vertices.reserve(size);
            for (auto i : core::range(size))
                m_vertices.emplace_back(m_positions[i], m_normals[i], m_colors[i], m_uvs[i]);
        }

        m_size = size;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto VertexArray::addVertices(std::span<const Vertex> vertices) -> void {
        const auto size = std::size(vertices);

        m_positions.reserve(size);
        m_normals.reserve(size);
        m_colors.reserve(size);
        m_uvs.reserve(size);
        m_vertices.reserve(size);

        for (const auto& vertex : vertices) addVertex(vertex);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto VertexArray::addVertex(const Vertex& vertex) -> void {
        auto& position = m_positions.emplace_back(vertex.position);
        auto& normal   = m_normals.emplace_back(vertex.normal);
        auto& color    = m_colors.emplace_back(vertex.color);
        auto& uv       = m_uvs.emplace_back(vertex.uv);

        m_vertices.emplace_back(position, normal, color, uv);

        m_size += 1;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto VertexArray::removeVertex(core::RangeExtent position) -> void {
        core::expects(position < m_size);

        m_positions.erase(std::begin(m_positions) + position);
        m_normals.erase(std::begin(m_normals) + position);
        m_colors.erase(std::begin(m_colors) + position);
        m_uvs.erase(std::begin(m_uvs) + position);
        m_vertices.erase(std::begin(m_vertices) + position);

        m_size -= 1;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto VertexArray::removeVertices(core::RangeExtent first, core::RangeExtent count) -> void {
        core::expects(first + count < m_size);

        m_positions.erase(std::begin(m_positions) + first, std::begin(m_positions) + first + count);
        m_normals.erase(std::begin(m_normals) + first, std::begin(m_normals) + first + count);
        m_colors.erase(std::begin(m_colors) + first, std::begin(m_colors) + first + count);
        m_uvs.erase(std::begin(m_uvs) + first, std::begin(m_uvs) + first + count);
        m_vertices.erase(std::begin(m_vertices) + first, std::begin(m_vertices) + first + count);

        m_size -= count;
    }
} // namespace stormkit::engine
