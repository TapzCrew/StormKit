// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - StormKit::core - ///////////
#include <storm/core/Memory.hpp>
#include <storm/core/NonCopyable.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/RingBuffer.hpp>

/////////// - StormKit::entities - ///////////
#include <storm/entities/Entity.hpp>
#include <storm/entities/Fwd.hpp>

/////////// - StormKit::render - ///////////
#include <storm/render/Fwd.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

#include <storm/engine/render/Transform.hpp>

namespace storm::engine {
    class STORMKIT_PUBLIC RenderQueue: public core::NonCopyable {
      public:
        struct DrawInstance {
            core::UInt32 draw_data;
            entities::Entity entity;
        };

        struct alignas(16) DrawData {
            core::UInt32 transform_id;
            core::Int32 material_id;
            core::Int32 skinning_id;
        };

        RenderQueue(Engine &engine,
                    entities::EntityManager &manager,
                    core::UInt32 buffering_count,
                    std::string_view system_name);
        ~RenderQueue();

        RenderQueue(RenderQueue &&) noexcept;
        RenderQueue &operator=(RenderQueue &&) noexcept;

        void addDrawInstance(entities::Entity e, const Drawable &drawable);

        void update(FrameGraph &frame_graph);

        [[nodiscard]] FrameGraphResourceID vertexBufferID() const noexcept;
        [[nodiscard]] FrameGraphResourceID indexBufferID() const noexcept;
        [[nodiscard]] FrameGraphResourceID drawDataBufferID() const noexcept;
        [[nodiscard]] FrameGraphResourceID drawCommandBufferID() const noexcept;
        [[nodiscard]] FrameGraphResourceID transformBufferID() const noexcept;

        [[nodiscard]] core::ArraySize drawCommandCount() const noexcept;

        ENGINE_GETTERS

        ALLOCATE_HELPERS(RenderQueue)
      private:
        struct DrawEntry {
            core::Hash64 hash;
            bool merged = false;

            std::vector<core::UInt32> draw_instances;
        };

        void updateVertexAndIndexBuffer(FrameGraph &frame_graph);
        void updateDrawDataBuffer(FrameGraph &frame_graph);
        void updateDrawCommandsBuffer(FrameGraph &frame_graph);
        void updateTransformBuffer(FrameGraph &frame_graph);

        EngineRef m_engine;
        entities::EntityManagerRef m_manager;

        core::UInt32 m_buffering_count;

        std::string_view m_system_name;

        std::string m_vertex_buffer_name;
        std::string m_index_buffer_name;
        std::string m_draw_data_buffer_name;
        std::string m_draw_command_buffer_name;

        std::string m_transform_buffer_name;
        std::string m_material_buffer_name;

        std::string m_update_vertices_and_indices_pass_name;
        std::string m_update_draw_data_buffer_pass_name;
        std::string m_update_draw_command_buffer_pass_name;
        std::string m_update_transform_buffer_pass_name;

        std::vector<DrawEntry> m_draws;

        std::vector<DrawInstance> m_draw_instances;

        std::vector<DrawData> m_draw_data;
        bool m_need_update_draw_data = true;

        std::vector<vk::DrawIndexedIndirectCommand> m_draw_commands;
        bool m_need_update_draw_commands = true;

        std::vector<Transform::Data> m_transforms;

        using RingHardwareBuffer = core::RingBuffer<render::HardwareBuffer>;
        RingHardwareBuffer m_vertex_buffer_pool;
        RingHardwareBuffer m_index_buffer_pool;
        RingHardwareBuffer m_draw_data_buffer_pool;
        RingHardwareBuffer m_draw_command_buffer_pool;

        RingHardwareBuffer m_transform_buffer_pool;
        RingHardwareBuffer m_material_buffer_pool;

        FrameGraphResourceID m_vertex_buffer_id;
        FrameGraphResourceID m_index_buffer_id;
        FrameGraphResourceID m_draw_data_buffer_id;
        FrameGraphResourceID m_draw_command_buffer_id;

        FrameGraphResourceID m_transform_buffer_id;
        FrameGraphResourceID m_material_buffer_id;

        core::UInt32 m_frame_counter = 0;
    };
} // namespace storm::engine

#include "RenderQueue.inl"
