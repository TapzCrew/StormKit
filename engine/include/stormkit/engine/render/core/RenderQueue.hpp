// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <stormkit/core/PlatformMacro.hpp>

#include <stormkit/core/Types.hpp>

#include <stormkit/gpu/Fwd.hpp>

#include <stormkit/gpu/resource/Buffer.hpp>
#include <stormkit/gpu/resource/ImageView.hpp>

#include <stormkit/engine/EngineObject.hpp>
#include <stormkit/engine/Fwd.hpp>
#include <stormkit/engine/render/core/ShaderInputBuffer.hpp>

namespace stormkit::engine {
    class STORMKIT_PUBLIC RenderQueue: public EngineObject {
      public:
        using StaticMeshInputBuffer      = ShaderInputBuffer<5>;
        using ID                         = core::UInt64;
        static constexpr auto INVALID_ID = std::numeric_limits<ID>::max();

        explicit RenderQueue(Engine &engine);
        ~RenderQueue();

        RenderQueue(const RenderQueue &) = delete;
        auto operator=(const RenderQueue &) -> RenderQueue & = delete;

        RenderQueue(RenderQueue &&) noexcept;
        auto operator=(RenderQueue &&) noexcept -> RenderQueue &;

        auto addMesh(const MeshComponent &mesh) -> ID;
        auto removeMesh(ID id) -> void;

        auto update() -> void;
        auto draw(gpu::CommandBuffer &cmb) -> void;

      private:
        RenderQueue(RenderQueue &&other, std::lock_guard<std::mutex> lock) noexcept;

        struct RenderEntry {
            RenderEntry(ID _id, const MeshComponent &_mesh) noexcept
                : id { _id }, mesh { &_mesh } {}

            ID id;

            const MeshComponent *mesh;

            struct SubMesh {
                StaticMeshInputBuffer::Allocation positions;
                StaticMeshInputBuffer::Allocation normals;
                StaticMeshInputBuffer::Allocation colors;
                StaticMeshInputBuffer::Allocation uvs;

                StaticMeshInputBuffer::Allocation indices;

                core::USize submesh_index;
            };

            std::vector<SubMesh> submeshes;
        };
        using RenderEntries = std::vector<RenderEntry>;

        std::mutex m_mutex;

        ID m_next_id = 0;
        RenderEntries m_entries;

        RenderEntries m_to_be_added;
        std::vector<ID> m_to_be_removed;

        StaticMeshInputBuffer m_shader_input;

        std::vector<gpu::CommandBuffer> m_command_buffers;
        std::vector<gpu::Fence> m_vertex_buffer_fences;
        std::vector<gpu::Buffer> m_vertex_staging_buffers;
    };
} // namespace stormkit::engine
