// Copyright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

/////////// - STL - ///////////
#include <bitset>
#include <set>

/////////// - StormKit::core - ///////////
#include <storm/core/HashMap.hpp>
#include <storm/core/Numerics.hpp>
#include <storm/core/Platform.hpp>
#include <storm/core/RingBuffer.hpp>

/////////// - StormKit::entities - ///////////
#include <storm/entities/Entity.hpp>
#include <storm/entities/Fwd.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/Fwd.hpp>

#include <storm/engine/render/Camera.hpp>
#include <storm/engine/render/RenderQueue.hpp>
#include <storm/engine/render/RenderSystem.hpp>
#include <storm/engine/render/Transform.hpp>

namespace storm::engine {
    class STORMKIT_PUBLIC PbrRenderSystem: public RenderSystem {
      public:
        enum class RenderPassID { Forward, Deffered, Shadow, Lightning };

        PbrRenderSystem(Engine &engine, entities::EntityManager &manager);
        ~PbrRenderSystem();

        PbrRenderSystem(PbrRenderSystem &&) noexcept;
        PbrRenderSystem &operator=(PbrRenderSystem &&) noexcept;

        void preUpdate() override;
        void update(core::Secondf delta) override;

        void resetCamera() override;

      protected:
        void setupFrameGraph(FrameGraph &frame_graph, FrameGraphResourceID backbuffer) override;

      private:
        static constexpr auto UPDATE_FREQUENCY = 1.f / 60.f;

        void updateSceneGlobalBuffer(FrameGraph &frame_graph);

        core::UInt32 m_buffering_count;

        std::string m_system_name;
        std::string m_forward_plus_pass_name;
        std::string m_update_scene_global_buffer_pass_name;

        RenderQueue m_render_queue;

        Camera m_default_camera;

        core::RingBuffer<render::HardwareBuffer> m_scene_global_buffer_pool;

        FrameGraphResourceID m_scene_global_buffer_id;

        std::chrono::high_resolution_clock::time_point m_last_update;
        core::Secondf m_cumuled_deltas;

        render::ShaderPtr m_pbr_forward_vertex_shader;
        render::ShaderPtr m_pbr_forward_fragment_shader;

        bool m_first_iteration = true;

        core::UInt32 m_frame_counter = 0u;
    };
} // namespace storm::engine

#include "PbrRenderSystem.inl"
