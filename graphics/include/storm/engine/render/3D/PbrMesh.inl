// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace storm::engine {
    inline auto PbrMesh::Node::localTransform() const noexcept -> core::Matrix {
        auto transform = core::translate(core::Matrix { 1.f }, translation) *
                         core::Matrix { rotation } * core::scale(core::Matrix { 1.f }, scale) *
                         matrix;

        return transform;
    }

    inline auto PbrMesh::Node::globalTransform(core::span<const Node> nodes) const noexcept
        -> core::Matrix {
        auto transform = localTransform();

        auto parent_id = parent;
        while (parent_id != Node::NO_SKIN) {
            const auto &parent = nodes[parent_id];

            transform = parent.localTransform() * transform;

            parent_id = parent.parent;
        }

        return transform;
    }

    inline auto PbrMesh::getNode(Node::ID id) const noexcept -> const Node & {
        STORMKIT_EXPECTS(id < std::size(m_nodes));

        return m_nodes[id];
    }

    /////////////////////////////////////
    /////////////////////////////////////
    constexpr auto PbrMesh::vertexSize() const noexcept -> core::ArraySize {
        return sizeof(Vertex);
    }
} // namespace storm::engine
